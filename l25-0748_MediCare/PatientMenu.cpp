#include "PatientMenu.h"

#include "Appointment.h"
#include "Bill.h"
#include "CoreStrings.h"
#include "Doctor.h"
#include "FileHandler.h"
#include "HospitalException.h"
#include "InsufficientFundsException.h"
#include "InvalidInputException.h"
#include "MediCoreUi.h"
#include "Patient.h"
#include "Prescription.h"
#include "SlotUnavailableException.h"
#include "Validator.h"

#include <iostream>




using std::cout;
using std::endl;
static const char* const FIXED_SLOTS[8] = {"09:00", "10:00", "11:00", "12:00",
                                           "13:00", "14:00", "15:00", "16:00"};

// Checks whether the doctor already has this date and slot booked.
static bool slot_conflicts(int doctorId, char* dateStr, char* slotStr, Storage<Appointment>* appointments)
{
    Appointment probe(0, 0, doctorId, dateStr, slotStr, (char*)"pending");
    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    int i = 0;
    while (i < n)
    {
        Appointment* ex = arr[i];
        if (ex != nullptr && probe == (*ex))
        {
            return true;
        }
        i++;
    }
    return false;
}

// Finds the bill linked to an appointment id.
static Bill* find_bill_by_appointment(Storage<Bill>* bills, int appointmentId)
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

// Sorts records in ascending order for display.
static void sort_appointments_date_asc(Appointment** arr, int n)
{
    int i = 0;
    while (i < n - 1)
    {
        int j = 0;
        while (j < n - 1 - i)
        {
            Appointment* a = arr[j];
            Appointment* b = arr[j + 1];
            if (core_compare_dates_dd_mm_yyyy(a->getDateConst(), b->getDateConst()) > 0)
            {
                arr[j] = b;
                arr[j + 1] = a;
            }
            j++;
        }
        i++;
    }
}

// Sorts records in descending order for display.
static void sort_prescriptions_date_desc(Prescription** arr, int n)
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

// Creates a new appointment after validation checks pass.
void bookAppointment(Patient* patient, Storage<Doctor>* doctors, Storage<Appointment>* appointments,
                     Storage<Bill>* bills, FileHandler* fh)
{
    (void)fh;

    char specBuf[128];
    medi_ui_print_str("Enter specialization to search (e.g. Cardiology): ");
    medi_ui_read_line(specBuf, sizeof(specBuf));

    int docCount = doctors->size();
    Doctor** darr = doctors->getAll();
    bool anyDoctor = false;
    medi_ui_print_line("Matching doctors:");
    int di = 0;
    while (di < docCount)
    {
        Doctor* d = darr[di];
        if (d != nullptr && Validator::caseInsensitiveCompare(d->getSpecialization(), specBuf))
        {
            anyDoctor = true;
            cout << "ID " << d->getId() << " | " << d->getNameConst() << " | Fee PKR " << d->getFee()
                      << endl;
        }
        di++;
    }

    if (!anyDoctor)
    {
        medi_ui_print_line("No doctors available for that specialization.");
        return;
    }

    medi_ui_print_str("Enter Doctor ID: ");
    int doctorPick = 0;
    medi_ui_read_int(&doctorPick);
    Doctor* picked = doctors->findByID(doctorPick);
    if (picked == nullptr)
    {
        medi_ui_print_line("Doctor not found.");
        return;
    }

    char dateBuf[16];
    int dateAttempts = 0;
    while (dateAttempts < 3)
    {
        medi_ui_print_str("Enter date (DD-MM-YYYY): ");
        medi_ui_read_line(dateBuf, sizeof(dateBuf));
        if (Validator::validateDate(dateBuf))
        {
            break;
        }
        medi_ui_print_line("Invalid date. Use format DD-MM-YYYY.");
        dateAttempts++;
    }

    if (dateAttempts >= 3)
    {
        return;
    }

    bool bookingDone = false;
    while (!bookingDone)
    {
        medi_ui_print_line("Available slots:");
        int si = 0;
        while (si < 8)
        {
            char slotBuf[8];
            core_str_cpy(slotBuf, FIXED_SLOTS[si]);
            // Show only slots that are not already booked.
            if (!slot_conflicts(picked->getId(), dateBuf, slotBuf, appointments))
            {
                medi_ui_print_line(FIXED_SLOTS[si]);
            }
            si++;
        }

        char slotPick[16];
        medi_ui_print_str("Enter time slot (e.g. 09:00): ");
        medi_ui_read_line(slotPick, sizeof(slotPick));

        if (!Validator::validateTimeSlot(slotPick))
        {
            medi_ui_print_line("Invalid slot selection.");
            continue;
        }

        try
        {
            if (slot_conflicts(picked->getId(), dateBuf, slotPick, appointments))
            {
                throw SlotUnavailableException();
            }

            if (patient->getBalance() < picked->getFee())
            {
                throw InsufficientFundsException();
            }

            *patient -= picked->getFee();
            FileHandler::updatePatient(patient);

            int nextAppt = FileHandler::maxAppointmentIdFromDisk() + 1;
            int nextBill = FileHandler::maxBillIdFromDisk() + 1;

            Appointment* ap =
                new Appointment(nextAppt, patient->getId(), picked->getId(), dateBuf, slotPick, (char*)"pending");
            appointments->add(ap);
            FileHandler::appendAppointment(ap);

            Bill* bill = new Bill(nextBill, patient->getId(), nextAppt, picked->getFee(), (char*)"unpaid", dateBuf);
            bills->add(bill);
            FileHandler::appendBill(bill);

            char msg[160];
            msg[0] = '\0';
            core_str_cat(msg, "Appointment booked successfully. Appointment ID: ");
            char ib[32];
            core_int_to_str(nextAppt, ib);
            core_str_cat(msg, ib);
            medi_ui_print_line(msg);
            bookingDone = true;
        }
        catch (InsufficientFundsException& ex)
        {
            medi_ui_print_line(ex.what());
            bookingDone = true;
        }
        catch (SlotUnavailableException& ex)
        {
            medi_ui_print_line(ex.what());
        }
        catch (HospitalException& ex)
        {
            medi_ui_print_line(ex.what());
        }
    }
}

// Cancels the selected appointment and persists the change.
void cancelAppointment(Patient* patient, Storage<Appointment>* appointments, Storage<Doctor>* doctors,
                       Storage<Bill>* bills, FileHandler* fh)
{
    (void)fh;

    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    bool anyPending = false;
    medi_ui_print_line("Pending appointments:");
    int i = 0;
    while (i < n)
    {
        Appointment* a = arr[i];
        if (a != nullptr && a->getPatientID() == patient->getId() &&
            core_str_cmp(a->getStatusConst(), "pending") == 0)
        {
            anyPending = true;
            Doctor* d = doctors->findByID(a->getDoctorID());
            const char* docName = (d != nullptr) ? d->getNameConst() : "Unknown";
            cout << "Appointment ID " << a->getAppointmentID() << " | Doctor " << docName << " | Date "
                      << a->getDateConst() << " | Slot " << a->getTimeSlotConst() << endl;
        }
        i++;
    }

    if (!anyPending)
    {
        medi_ui_print_line("You have no pending appointments.");
        return;
    }

    medi_ui_print_str("Enter Appointment ID to cancel: ");
    int pick = 0;
    medi_ui_read_int(&pick);

    Appointment* target = appointments->findByID(pick);
    if (target == nullptr || target->getPatientID() != patient->getId() ||
        core_str_cmp(target->getStatusConst(), "pending") != 0)
    {
        medi_ui_print_line("Invalid appointment ID.");
        return;
    }

    Doctor* doc = doctors->findByID(target->getDoctorID());
    float refund = 0.0f;
    if (doc != nullptr)
    {
        refund = doc->getFee();
    }

    target->setStatus((char*)"cancelled");
    FileHandler::updateAppointment(target);

    *(patient) += refund;
    FileHandler::updatePatient(patient);

    Bill* b = find_bill_by_appointment(bills, target->getAppointmentID());
    if (b != nullptr)
    {
        b->setStatus((char*)"cancelled");
        FileHandler::updateBill(b);
    }

    char buf[160];
    buf[0] = '\0';
    core_str_cat(buf, "Appointment cancelled. PKR ");
    char fb[64];
    core_float_to_str(refund, fb);
    core_str_cat(buf, fb);
    core_str_cat(buf, " refunded to your balance.");
    medi_ui_print_line(buf);
}

// Displays the requested records in the console.
void viewAppointments(Patient* patient, Storage<Appointment>* appointments, Storage<Doctor>* doctors)
{
    int n = appointments->size();
    Appointment** raw = appointments->getAll();
    int cnt = 0;
    int i = 0;
    while (i < n)
    {
        Appointment* a = raw[i];
        if (a != nullptr && a->getPatientID() == patient->getId())
        {
            cnt++;
        }
        i++;
    }

    if (cnt == 0)
    {
        medi_ui_print_line("No appointments found.");
        return;
    }

    Appointment** arr = new Appointment*[cnt];
    int w = 0;
    int j = 0;
    while (j < n)
    {
        Appointment* a = raw[j];
        if (a != nullptr && a->getPatientID() == patient->getId())
        {
            arr[w] = a;
            w++;
        }
        j++;
    }

    sort_appointments_date_asc(arr, cnt);

    medi_ui_print_line("Your appointments (sorted by date ascending):");
    int k = 0;
    while (k < cnt)
    {
        Appointment* a = arr[k];
        Doctor* d = doctors->findByID(a->getDoctorID());
        const char* dn = (d != nullptr) ? d->getNameConst() : "Unknown";
        const char* sp = (d != nullptr) ? d->getSpecializationConst() : "Unknown";
        cout << a->getAppointmentID() << " | " << dn << " | " << sp << " | " << a->getDateConst() << " | "
                  << a->getTimeSlotConst() << " | " << a->getStatusConst() << endl;
        k++;
    }

    delete[] arr;
}

// Displays the requested records in the console.
void viewMedicalRecords(Patient* patient, Storage<Prescription>* prescriptions, Storage<Doctor>* doctors)
{
    int n = prescriptions->size();
    Prescription** raw = prescriptions->getAll();
    int cnt = 0;
    int i = 0;
    while (i < n)
    {
        Prescription* pr = raw[i];
        if (pr != nullptr && pr->getPatientID() == patient->getId())
        {
            cnt++;
        }
        i++;
    }

    if (cnt == 0)
    {
        medi_ui_print_line("No medical records found.");
        return;
    }

    Prescription** arr = new Prescription*[cnt];
    int w = 0;
    int j = 0;
    while (j < n)
    {
        Prescription* pr = raw[j];
        if (pr != nullptr && pr->getPatientID() == patient->getId())
        {
            arr[w] = pr;
            w++;
        }
        j++;
    }

    sort_prescriptions_date_desc(arr, cnt);

    medi_ui_print_line("Medical records (most recent first):");
    int k = 0;
    while (k < cnt)
    {
        Prescription* pr = arr[k];
        Doctor* d = doctors->findByID(pr->getDoctorID());
        const char* dn = (d != nullptr) ? d->getNameConst() : "Unknown";
        cout << pr->getDateConst() << " | " << dn << " | " << pr->getMedicinesConst() << " | "
                  << pr->getNotesConst() << endl;
        k++;
    }

    delete[] arr;
}

// Displays the requested records in the console.
void viewBills(Patient* patient, Storage<Bill>* bills)
{
    int n = bills->size();
    Bill** raw = bills->getAll();
    int cnt = 0;
    int i = 0;
    while (i < n)
    {
        Bill* b = raw[i];
        if (b != nullptr && b->getPatientID() == patient->getId())
        {
            cnt++;
        }
        i++;
    }

    if (cnt == 0)
    {
        medi_ui_print_line("No bills found.");
        return;
    }

    float unpaidTotal = 0.0f;
    medi_ui_print_line("Your bills:");
    int j = 0;
    while (j < n)
    {
        Bill* b = raw[j];
        if (b != nullptr && b->getPatientID() == patient->getId())
        {
            cout << "Bill ID " << b->getBillID() << " | Appt " << b->getAppointmentID() << " | Amount PKR "
                      << b->getAmount() << " | Status " << b->getStatusConst() << " | Date " << b->getDateConst()
                      << endl;
            if (core_str_cmp(b->getStatusConst(), "unpaid") == 0)
            {
                unpaidTotal += b->getAmount();
            }
        }
        j++;
    }

    cout << "Total outstanding unpaid PKR " << unpaidTotal << endl;
}

// Applies a payment to the selected bill and updates balances.
void payBill(Patient* patient, Storage<Bill>* bills, FileHandler* fh)
{
    (void)fh;

    int n = bills->size();
    Bill** raw = bills->getAll();
    bool anyUnpaid = false;
    medi_ui_print_line("Unpaid bills:");
    int i = 0;
    while (i < n)
    {
        Bill* b = raw[i];
        if (b != nullptr && b->getPatientID() == patient->getId() &&
            core_str_cmp(b->getStatusConst(), "unpaid") == 0)
        {
            anyUnpaid = true;
            cout << "Bill ID " << b->getBillID() << " | Appt " << b->getAppointmentID() << " | Amount PKR "
                      << b->getAmount() << endl;
        }
        i++;
    }

    if (!anyUnpaid)
    {
        medi_ui_print_line("No unpaid bills.");
        return;
    }

    medi_ui_print_str("Enter Bill ID to pay: ");
    int bid = 0;
    medi_ui_read_int(&bid);

    Bill* bill = bills->findByID(bid);
    if (bill == nullptr || bill->getPatientID() != patient->getId() ||
        core_str_cmp(bill->getStatusConst(), "unpaid") != 0)
    {
        medi_ui_print_line("Invalid bill selection.");
        return;
    }

    try
    {
        if (patient->getBalance() < bill->getAmount())
        {
            throw InsufficientFundsException();
        }

        *patient -= bill->getAmount();
        bill->setStatus((char*)"paid");
        FileHandler::updatePatient(patient);
        FileHandler::updateBill(bill);

        char msg[160];
        msg[0] = '\0';
        core_str_cat(msg, "Bill paid successfully. Remaining balance: PKR ");
        char fb[64];
        core_float_to_str(patient->getBalance(), fb);
        core_str_cat(msg, fb);
        medi_ui_print_line(msg);
    }
    catch (HospitalException& ex)
    {
        medi_ui_print_line(ex.what());
    }
}

// Adds money to the patient wallet and saves the new balance.
void topUpBalance(Patient* patient, FileHandler* fh)
{
    (void)fh;

    int attempts = 0;
    while (attempts < 3)
    {
        try
        {
            char amtBuf[64];
            medi_ui_print_str("Enter amount to add (PKR): ");
            medi_ui_read_line(amtBuf, sizeof(amtBuf));
            if (!Validator::validatePositiveFloat(amtBuf))
            {
                throw InvalidInputException((char*)"Amount must be a positive number.");
            }
            float addAmt = core_parse_float(amtBuf);
            if (addAmt <= 0.0f)
            {
                throw InvalidInputException((char*)"Amount must be greater than zero.");
            }

            *(patient) += addAmt;
            FileHandler::updatePatient(patient);

            char msg[160];
            msg[0] = '\0';
            core_str_cat(msg, "Balance updated. New balance: PKR ");
            char fb[64];
            core_float_to_str(patient->getBalance(), fb);
            core_str_cat(msg, fb);
            medi_ui_print_line(msg);
            return;
        }
        catch (InvalidInputException& ex)
        {
            medi_ui_print_line(ex.what());
            attempts++;
        }
        catch (HospitalException& ex)
        {
            medi_ui_print_line(ex.what());
            attempts++;
        }
    }
}

// Displays the requested records in the console.
void showPatientMenu(Patient* patient, Storage<Doctor>* doctors, Storage<Appointment>* appointments,
                     Storage<Bill>* bills, Storage<Prescription>* prescriptions, FileHandler* fh)
{
    bool logout = false;
    while (!logout)
    {
        medi_ui_print_blank_line();
        char hdr[160];
        hdr[0] = '\0';
        core_str_cat(hdr, "Welcome, ");
        core_str_cat(hdr, patient->getName());
        medi_ui_print_line(hdr);

        char balLine[160];
        balLine[0] = '\0';
        core_str_cat(balLine, "Balance: PKR ");
        char fb[64];
        core_float_to_str(patient->getBalance(), fb);
        core_str_cat(balLine, fb);
        medi_ui_print_line(balLine);

        medi_ui_print_line("========================");
        medi_ui_print_line("1. Book Appointment");
        medi_ui_print_line("2. Cancel Appointment");
        medi_ui_print_line("3. View My Appointments");
        medi_ui_print_line("4. View My Medical Records");
        medi_ui_print_line("5. View My Bills");
        medi_ui_print_line("6. Pay Bill");
        medi_ui_print_line("7. Top Up Balance");
        medi_ui_print_line("8. Logout");
        medi_ui_print_str("Select option: ");

        int choice = 0;
        medi_ui_read_int(&choice);
        if (!Validator::validateMenuChoice(choice, 1, 8))
        {
            medi_ui_print_line("Invalid menu choice.");
            continue;
        }

        if (choice == 1)
        {
            bookAppointment(patient, doctors, appointments, bills, fh);
        }
        else if (choice == 2)
        {
            cancelAppointment(patient, appointments, doctors, bills, fh);
        }
        else if (choice == 3)
        {
            viewAppointments(patient, appointments, doctors);
        }
        else if (choice == 4)
        {
            viewMedicalRecords(patient, prescriptions, doctors);
        }
        else if (choice == 5)
        {
            viewBills(patient, bills);
        }
        else if (choice == 6)
        {
            payBill(patient, bills, fh);
        }
        else if (choice == 7)
        {
            topUpBalance(patient, fh);
        }
        else if (choice == 8)
        {
            logout = true;
        }
    }
}
