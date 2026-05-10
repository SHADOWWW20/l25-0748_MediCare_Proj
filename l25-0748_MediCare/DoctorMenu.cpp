#include "DoctorMenu.h"

#include "Appointment.h"
#include "Bill.h"
#include "CoreStrings.h"
#include "Doctor.h"
#include "FileHandler.h"
#include "MediCoreUi.h"
#include "Patient.h"
#include "Prescription.h"
#include "Validator.h"

#include <iostream>




using std::cout;
using std::endl;
// Copies text into a fixed buffer without overflow.
static void doctor_copy_limit(char* dest, const char* src, int destCapacity)
{
    int maxChars = destCapacity - 1;
    int i = 0;
    while (src[i] != '\0' && i < maxChars)
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

// Sorts records in ascending order for display.
static void sort_today_appointments_time_asc(Appointment** arr, int n)
{
    int i = 0;
    while (i < n - 1)
    {
        int j = 0;
        while (j < n - 1 - i)
        {
            Appointment* a = arr[j];
            Appointment* b = arr[j + 1];
            if (core_compare_times_hh_mm(a->getTimeSlotConst(), b->getTimeSlotConst()) > 0)
            {
                arr[j] = b;
                arr[j + 1] = a;
            }
            j++;
        }
        i++;
    }
}

// Finds the bill linked to an appointment id.
static Bill* doctor_find_bill_for_appointment(Storage<Bill>* bills, int appointmentId)
{
    int n = bills->size();
    Bill** arr = bills->getAll();
    int i = 0;
    while (i < n)
    {
        Bill* b = arr[i];
        if (b != nullptr && b->getAppointmentID() == appointmentId)
        {
            return b;
        }
        i++;
    }
    return nullptr;
}

// Checks whether this doctor completed any visit for the patient.
static bool doctor_has_completed_relationship(Doctor* doctor, int patientId,
                                              Storage<Appointment>* appointments)
{
    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    int i = 0;
    while (i < n)
    {
        Appointment* a = arr[i];
        if (a != nullptr && a->getPatientID() == patientId && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getStatusConst(), "completed") == 0)
        {
            return true;
        }
        i++;
    }
    return false;
}

// Sorts records in descending order for display.
static void sort_prescriptions_date_desc_doc(Prescription** arr, int n)
{
    int i = 0;
    while (i < n - 1)
    {
        int j = 0;
        while (j < n - 1 - i)
        {
            Prescription* a = arr[j];
            Prescription* b = arr[j + 1];
            if (core_compare_dates_dd_mm_yyyy(a->getDateConst(), b->getDateConst()) < 0)
            {
                arr[j] = b;
                arr[j + 1] = a;
            }
            j++;
        }
        i++;
    }
}

// Displays the requested records in the console.
void viewTodayAppointments(Doctor* doctor, Storage<Appointment>* appointments, Storage<Patient>* patients)
{
    char todayBuf[16];
    core_get_today_dd_mm_yyyy(todayBuf);

    int n = appointments->size();
    Appointment** raw = appointments->getAll();
    int cnt = 0;
    int i = 0;
    while (i < n)
    {
        Appointment* a = raw[i];
        if (a != nullptr && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getDateConst(), todayBuf) == 0)
        {
            cnt++;
        }
        i++;
    }

    if (cnt == 0)
    {
        medi_ui_print_line("No appointments scheduled for today.");
        return;
    }

    Appointment** arr = new Appointment*[cnt];
    int w = 0;
    int j = 0;
    while (j < n)
    {
        Appointment* a = raw[j];
        if (a != nullptr && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getDateConst(), todayBuf) == 0)
        {
            arr[w] = a;
            w++;
        }
        j++;
    }

    sort_today_appointments_time_asc(arr, cnt);

    medi_ui_print_line("Today's schedule:");
    int k = 0;
    while (k < cnt)
    {
        Appointment* a = arr[k];
        Patient* p = patients->findByID(a->getPatientID());
        const char* pname = (p != nullptr) ? p->getNameConst() : "Unknown";
        cout << "Appointment ID " << a->getAppointmentID() << " | Patient " << pname << " | Slot "
                  << a->getTimeSlotConst() << " | Status " << a->getStatusConst() << endl;
        k++;
    }

    delete[] arr;
}

// Updates appointment status and saves the result.
void markComplete(Doctor* doctor, Storage<Appointment>* appointments, FileHandler* fh)
{
    (void)fh;

    char todayBuf[16];
    core_get_today_dd_mm_yyyy(todayBuf);

    int n = appointments->size();
    Appointment** raw = appointments->getAll();
    bool any = false;
    medi_ui_print_line("Today's pending appointments:");
    int i = 0;
    while (i < n)
    {
        Appointment* a = raw[i];
        if (a != nullptr && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getDateConst(), todayBuf) == 0 && core_str_cmp(a->getStatusConst(), "pending") == 0)
        {
            any = true;
            cout << "ID " << a->getAppointmentID() << " | Slot " << a->getTimeSlotConst() << endl;
        }
        i++;
    }

    if (!any)
    {
        medi_ui_print_line("No pending appointments for today.");
        return;
    }

    medi_ui_print_str("Enter Appointment ID: ");
    int pick = 0;
    medi_ui_read_int(&pick);

    Appointment* ap = appointments->findByID(pick);
    if (ap == nullptr || ap->getDoctorID() != doctor->getId() ||
        core_str_cmp(ap->getDateConst(), todayBuf) != 0 || core_str_cmp(ap->getStatusConst(), "pending") != 0)
    {
        medi_ui_print_line("Invalid appointment selection.");
        return;
    }

    ap->setStatus((char*)"completed");
    FileHandler::updateAppointment(ap);
    medi_ui_print_line("Appointment marked as completed.");
}

// Updates appointment status and saves the result.
void markNoShow(Doctor* doctor, Storage<Appointment>* appointments, Storage<Bill>* bills, FileHandler* fh)
{
    (void)fh;

    char todayBuf[16];
    core_get_today_dd_mm_yyyy(todayBuf);

    int n = appointments->size();
    Appointment** raw = appointments->getAll();
    bool any = false;
    medi_ui_print_line("Today's pending appointments:");
    int i = 0;
    while (i < n)
    {
        Appointment* a = raw[i];
        if (a != nullptr && a->getDoctorID() == doctor->getId() &&
            core_str_cmp(a->getDateConst(), todayBuf) == 0 && core_str_cmp(a->getStatusConst(), "pending") == 0)
        {
            any = true;
            cout << "ID " << a->getAppointmentID() << " | Slot " << a->getTimeSlotConst() << endl;
        }
        i++;
    }

    if (!any)
    {
        medi_ui_print_line("No pending appointments for today.");
        return;
    }

    medi_ui_print_str("Enter Appointment ID: ");
    int pick = 0;
    medi_ui_read_int(&pick);

    Appointment* ap = appointments->findByID(pick);
    if (ap == nullptr || ap->getDoctorID() != doctor->getId() ||
        core_str_cmp(ap->getDateConst(), todayBuf) != 0 || core_str_cmp(ap->getStatusConst(), "pending") != 0)
    {
        medi_ui_print_line("Invalid appointment selection.");
        return;
    }

    ap->setStatus((char*)"no-show");
    FileHandler::updateAppointment(ap);

    Bill* b = doctor_find_bill_for_appointment(bills, ap->getAppointmentID());
    if (b != nullptr)
    {
        b->setStatus((char*)"cancelled");
        FileHandler::updateBill(b);
    }

    medi_ui_print_line("Appointment marked as no-show.");
}

// Collects input data and stores a new entry.
void writePrescription(Doctor* doctor, Storage<Appointment>* appointments, Storage<Prescription>* prescriptions,
                       FileHandler* fh)
{
    (void)fh;

    medi_ui_print_str("Enter Appointment ID: ");
    int apptId = 0;
    medi_ui_read_int(&apptId);

    Appointment* ap = appointments->findByID(apptId);
    if (ap == nullptr || ap->getDoctorID() != doctor->getId() ||
        core_str_cmp(ap->getStatusConst(), "completed") != 0)
    {
        medi_ui_print_line("Invalid appointment selection.");
        return;
    }

    int pn = prescriptions->size();
    Prescription** parr = prescriptions->getAll();
    int pi = 0;
    while (pi < pn)
    {
        Prescription* pr = parr[pi];
        if (pr != nullptr && pr->getAppointmentID() == apptId)
        {
            medi_ui_print_line("Prescription already written for this appointment.");
            return;
        }
        pi++;
    }

    char meds[1024];
    char notes[512];
    medi_ui_print_str("Enter medicines (Medicine Dosage;Medicine2 Dosage): ");
    medi_ui_read_line(meds, sizeof(meds));
    char medsLimited[500];
    doctor_copy_limit(medsLimited, meds, 500);

    medi_ui_print_str("Enter notes (max 300 chars): ");
    medi_ui_read_line(notes, sizeof(notes));
    char notesLimited[300];
    doctor_copy_limit(notesLimited, notes, 300);

    int nextId = FileHandler::maxPrescriptionIdFromDisk() + 1;
    Prescription* pr = new Prescription(nextId, apptId, ap->getPatientID(), doctor->getId(), ap->getDate(),
                                        medsLimited, notesLimited);
    prescriptions->add(pr);
    FileHandler::appendPrescription(pr);

    medi_ui_print_line("Prescription saved.");
}

// Displays the requested records in the console.
void viewPatientHistory(Doctor* doctor, Storage<Patient>* patients, Storage<Prescription>* prescriptions,
                        Storage<Appointment>* appointments)
{
    medi_ui_print_str("Enter Patient ID: ");
    int pid = 0;
    medi_ui_read_int(&pid);

    Patient* p = patients->findByID(pid);
    if (p == nullptr || !doctor_has_completed_relationship(doctor, pid, appointments))
    {
        medi_ui_print_line("Access denied. You can only view records of your own patients.");
        return;
    }

    int n = prescriptions->size();
    Prescription** raw = prescriptions->getAll();
    int cnt = 0;
    int i = 0;
    while (i < n)
    {
        Prescription* pr = raw[i];
        if (pr != nullptr && pr->getPatientID() == pid && pr->getDoctorID() == doctor->getId())
        {
            cnt++;
        }
        i++;
    }

    if (cnt == 0)
    {
        medi_ui_print_line("No prescriptions found for this patient.");
        return;
    }

    Prescription** arr = new Prescription*[cnt];
    int w = 0;
    int j = 0;
    while (j < n)
    {
        Prescription* pr = raw[j];
        if (pr != nullptr && pr->getPatientID() == pid && pr->getDoctorID() == doctor->getId())
        {
            arr[w] = pr;
            w++;
        }
        j++;
    }

    sort_prescriptions_date_desc_doc(arr, cnt);

    medi_ui_print_line("Patient prescription history:");
    int k = 0;
    while (k < cnt)
    {
        Prescription* pr = arr[k];
        cout << pr->getDateConst() << " | " << pr->getMedicinesConst() << " | " << pr->getNotesConst()
                  << endl;
        k++;
    }

    delete[] arr;
}

// Displays the requested records in the console.
void showDoctorMenu(Doctor* doctor, Storage<Appointment>* appointments, Storage<Patient>* patients,
                    Storage<Prescription>* prescriptions, Storage<Bill>* bills, FileHandler* fh)
{
    bool logout = false;
    while (!logout)
    {
        medi_ui_print_blank_line();
        char hdr[256];
        hdr[0] = '\0';
        core_str_cat(hdr, "Welcome, Dr. ");
        core_str_cat(hdr, doctor->getName());
        medi_ui_print_line(hdr);

        char specLine[128];
        specLine[0] = '\0';
        core_str_cat(specLine, "Specialization: ");
        core_str_cat(specLine, doctor->getSpecialization());
        medi_ui_print_line(specLine);

        medi_ui_print_line("===============================================");
        medi_ui_print_line("1. View Today's Appointments");
        medi_ui_print_line("2. Mark Appointment Complete");
        medi_ui_print_line("3. Mark Appointment No-Show");
        medi_ui_print_line("4. Write Prescription");
        medi_ui_print_line("5. View Patient Medical History");
        medi_ui_print_line("6. Logout");
        medi_ui_print_str("Select option: ");

        int choice = 0;
        medi_ui_read_int(&choice);
        if (!Validator::validateMenuChoice(choice, 1, 6))
        {
            medi_ui_print_line("Invalid menu choice.");
            continue;
        }

        if (choice == 1)
        {
            viewTodayAppointments(doctor, appointments, patients);
        }
        else if (choice == 2)
        {
            markComplete(doctor, appointments, fh);
        }
        else if (choice == 3)
        {
            markNoShow(doctor, appointments, bills, fh);
        }
        else if (choice == 4)
        {
            writePrescription(doctor, appointments, prescriptions, fh);
        }
        else if (choice == 5)
        {
            viewPatientHistory(doctor, patients, prescriptions, appointments);
        }
        else if (choice == 6)
        {
            logout = true;
        }
    }
}
