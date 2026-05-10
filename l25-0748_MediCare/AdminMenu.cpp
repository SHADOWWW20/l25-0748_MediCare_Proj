#include "AdminMenu.h"

#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "CoreStrings.h"
#include "Doctor.h"
#include "FileHandler.h"
#include "MediCoreUi.h"
#include "Patient.h"
#include "Prescription.h"
#include "Validator.h"

#include <ctime>
#include <fstream>
#include <iostream>




using std::cout;
using std::endl;
// Reloads all storages from disk after major changes.
static void admin_reload_entire_system(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
                                       Storage<Appointment>* appointments, Storage<Bill>* bills,
                                       Storage<Prescription>* prescriptions)
{
    patients->clear();
    doctors->clear();
    appointments->clear();
    bills->clear();
    prescriptions->clear();
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAdmin(admin);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);
}

// Sorts records in descending order for display.
static void sort_appointments_date_desc(Appointment** arr, int n)
{
    int i = 0;
    while (i < n - 1)
    {
        int j = 0;
        while (j < n - 1 - i)
        {
            Appointment* a = arr[j];
            Appointment* b = arr[j + 1];
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

// Counts unpaid bills for one patient id.
static int admin_count_unpaid_for_patient(Storage<Bill>* bills, int patientId)
{
    int total = 0;
    int n = bills->size();
    Bill** arr = bills->getAll();
    int i = 0;
    while (i < n)
    {
        Bill* b = arr[i];
        if (b != nullptr && b->getPatientID() == patientId && core_str_cmp(b->getStatusConst(), "unpaid") == 0)
        {
            total++;
        }
        i++;
    }
    return total;
}

// Checks whether a doctor still has pending appointments.
static bool doctor_has_pending_appointment(Storage<Appointment>* appointments, int doctorId)
{
    int n = appointments->size();
    Appointment** arr = appointments->getAll();
    int i = 0;
    while (i < n)
    {
        Appointment* a = arr[i];
        if (a != nullptr && a->getDoctorID() == doctorId && core_str_cmp(a->getStatusConst(), "pending") == 0)
        {
            return true;
        }
        i++;
    }
    return false;
}

// Adds a new record to storage and updates in-memory data.
void addDoctor(Storage<Doctor>* doctors, FileHandler* fh)
{
    (void)fh;

    char nameBuf[128];
    char specBuf[128];
    char contactBuf[32];
    char passBuf[128];
    char feeBuf[64];

    medi_ui_print_str("Doctor full name (max 50 chars): ");
    medi_ui_read_line(nameBuf, sizeof(nameBuf));
    if (core_str_len(nameBuf) == 0 || core_str_len(nameBuf) > 50)
    {
        medi_ui_print_line("Invalid name length.");
        return;
    }

    medi_ui_print_str("Specialization (max 50 chars): ");
    medi_ui_read_line(specBuf, sizeof(specBuf));
    if (core_str_len(specBuf) == 0 || core_str_len(specBuf) > 50)
    {
        medi_ui_print_line("Invalid specialization.");
        return;
    }

    medi_ui_print_str("Contact (11 digits): ");
    medi_ui_read_line(contactBuf, sizeof(contactBuf));
    if (!Validator::validateContact(contactBuf))
    {
        medi_ui_print_line("Invalid contact number.");
        return;
    }

    medi_ui_print_str("Password (min 6 chars): ");
    medi_ui_read_line(passBuf, sizeof(passBuf));
    if (!Validator::validatePassword(passBuf))
    {
        medi_ui_print_line("Password too short.");
        return;
    }

    medi_ui_print_str("Consultation fee (PKR): ");
    medi_ui_read_line(feeBuf, sizeof(feeBuf));
    if (!Validator::validatePositiveFloat(feeBuf))
    {
        medi_ui_print_line("Invalid fee.");
        return;
    }

    float fee = core_parse_float(feeBuf);
    int nextId = FileHandler::maxDoctorIdFromDisk() + 1;
    Doctor* d = new Doctor(nextId, nameBuf, specBuf, contactBuf, passBuf, fee);
    doctors->add(d);
    FileHandler::appendDoctor(d);

    char msg[128];
    msg[0] = '\0';
    core_str_cat(msg, "Doctor added successfully. ID: ");
    char ib[32];
    core_int_to_str(nextId, ib);
    core_str_cat(msg, ib);
    medi_ui_print_line(msg);
}

// Removes the matching record and updates in-memory data.
void removeDoctor(Storage<Doctor>* doctors, Storage<Appointment>* appointments, FileHandler* fh)
{
    (void)fh;

    medi_ui_print_line("Doctors list:");
    int dn = doctors->size();
    Doctor** darr = doctors->getAll();
    int di = 0;
    while (di < dn)
    {
        Doctor* d = darr[di];
        if (d != nullptr)
        {
            cout << d->getId() << " | " << d->getNameConst() << " | " << d->getSpecializationConst()
                      << " | Fee PKR " << d->getFee() << endl;
        }
        di++;
    }

    medi_ui_print_str("Enter Doctor ID to remove: ");
    int pick = 0;
    medi_ui_read_int(&pick);

    Doctor* doc = doctors->findByID(pick);
    if (doc == nullptr)
    {
        medi_ui_print_line("Doctor not found.");
        return;
    }

    if (doctor_has_pending_appointment(appointments, pick))
    {
        medi_ui_print_line("Cannot remove doctor with pending appointments. Cancel or reassign them first.");
        return;
    }

    doctors->removeByID(pick);
    FileHandler::deleteDoctor(pick);
    medi_ui_print_line("Doctor removed.");
}

// Displays the requested records in the console.
void viewAllPatients(Storage<Patient>* patients, Storage<Bill>* bills)
{
    medi_ui_print_line("Patients:");
    int n = patients->size();
    Patient** arr = patients->getAll();
    int i = 0;
    while (i < n)
    {
        Patient* p = arr[i];
        if (p != nullptr)
        {
            int unpaid = admin_count_unpaid_for_patient(bills, p->getId());
            cout << p->getId() << " | " << p->getNameConst() << " | Age " << p->getAge() << " | Gender "
                      << p->getGender() << " | Contact " << p->getContactConst() << " | Balance PKR "
                      << p->getBalance() << " | Unpaid bills " << unpaid << endl;
        }
        i++;
    }
}

// Displays the requested records in the console.
void viewAllDoctors(Storage<Doctor>* doctors)
{
    medi_ui_print_line("Doctors:");
    int n = doctors->size();
    Doctor** arr = doctors->getAll();
    int i = 0;
    while (i < n)
    {
        Doctor* d = arr[i];
        if (d != nullptr)
        {
            cout << d->getId() << " | " << d->getNameConst() << " | " << d->getSpecializationConst()
                      << " | Contact " << d->getDoctorContactConst() << " | Fee PKR " << d->getFee() << endl;
        }
        i++;
    }
}

// Displays the requested records in the console.
void viewAllAppointments(Storage<Appointment>* appointments, Storage<Patient>* patients,
                         Storage<Doctor>* doctors)
{
    int n = appointments->size();
    if (n == 0)
    {
        medi_ui_print_line("No appointments.");
        return;
    }

    Appointment** raw = appointments->getAll();
    Appointment** arr = new Appointment*[n];
    int i = 0;
    while (i < n)
    {
        arr[i] = raw[i];
        i++;
    }

    sort_appointments_date_desc(arr, n);

    medi_ui_print_line("All appointments (newest dates first):");
    int k = 0;
    while (k < n)
    {
        Appointment* a = arr[k];
        Patient* p = patients->findByID(a->getPatientID());
        Doctor* d = doctors->findByID(a->getDoctorID());
        const char* pn = (p != nullptr) ? p->getNameConst() : "Unknown";
        const char* dn = (d != nullptr) ? d->getNameConst() : "Unknown";
        cout << a->getAppointmentID() << " | " << pn << " | " << dn << " | " << a->getDateConst() << " | "
                  << a->getTimeSlotConst() << " | " << a->getStatusConst() << endl;
        k++;
    }

    delete[] arr;
}

// Displays the requested records in the console.
void viewUnpaidBills(Storage<Bill>* bills, Storage<Patient>* patients)
{
    medi_ui_print_line("Unpaid bills:");
    std::time_t now = std::time(nullptr);

    int n = bills->size();
    Bill** arr = bills->getAll();
    bool any = false;
    int i = 0;
    while (i < n)
    {
        Bill* b = arr[i];
        if (b != nullptr && core_str_cmp(b->getStatusConst(), "unpaid") == 0)
        {
            any = true;
            Patient* p = patients->findByID(b->getPatientID());
            const char* pname = (p != nullptr) ? p->getNameConst() : "Unknown";

            double billSec = core_seconds_since_epoch_for_date_dd_mm_yyyy(b->getDateConst());
            double diff = std::difftime(now, (std::time_t)billSec);

            cout << "Bill ID " << b->getBillID() << " | " << pname << " | Amount PKR " << b->getAmount()
                      << " | Date " << b->getDateConst();
            if (diff > (double)(7 * 24 * 3600))
            {
                cout << " [OVERDUE]";
            }
            cout << endl;
        }
        i++;
    }

    if (!any)
    {
        medi_ui_print_line("No unpaid bills.");
    }
}

// Archives a patient after unpaid and pending checks pass.
void dischargePatient(Storage<Patient>* patients, Storage<Appointment>* appointments, Storage<Bill>* bills,
                      Storage<Prescription>* prescriptions, Storage<Doctor>* doctors, Admin* admin,
                      FileHandler* fh)
{
    (void)fh;

    medi_ui_print_str("Enter Patient ID: ");
    int pid = 0;
    medi_ui_read_int(&pid);

    Patient* p = patients->findByID(pid);
    if (p == nullptr)
    {
        medi_ui_print_line("Patient not found.");
        return;
    }

    int bn = bills->size();
    Bill** barr = bills->getAll();
    int bi = 0;
    while (bi < bn)
    {
        Bill* bill = barr[bi];
        if (bill != nullptr && bill->getPatientID() == pid && core_str_cmp(bill->getStatusConst(), "unpaid") == 0)
        {
            medi_ui_print_line("Cannot discharge patient with unpaid bills.");
            return;
        }
        bi++;
    }

    int an = appointments->size();
    Appointment** aarr = appointments->getAll();
    int ai = 0;
    while (ai < an)
    {
        Appointment* ap = aarr[ai];
        if (ap != nullptr && ap->getPatientID() == pid && core_str_cmp(ap->getStatusConst(), "pending") == 0)
        {
            medi_ui_print_line("Cannot discharge patient with pending appointments.");
            return;
        }
        ai++;
    }

    FileHandler::archivePatient(pid);
    admin_reload_entire_system(patients, doctors, admin, appointments, bills, prescriptions);

    medi_ui_print_line("Patient discharged and archived successfully.");
}

// Displays the requested records in the console.
void viewSecurityLog()
{
    std::ifstream in("security_log.txt");
    if (!in)
    {
        medi_ui_print_line("No security events logged.");
        return;
    }

    char buffer[512];
    bool any = false;
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        any = true;
        medi_ui_print_line(buffer);
    }

    if (!any)
    {
        medi_ui_print_line("No security events logged.");
    }
}

// Prints daily appointment, billing, and doctor summaries.
void generateDailyReport(Storage<Appointment>* appointments, Storage<Bill>* bills, Storage<Patient>* patients,
                         Storage<Doctor>* doctors)
{
    char todayBuf[16];
    core_get_today_dd_mm_yyyy(todayBuf);

    int totalToday = 0;
    int pend = 0;
    int comp = 0;
    int noshow = 0;
    int cancelled = 0;

    int an = appointments->size();
    Appointment** aarr = appointments->getAll();
    int ai = 0;
    while (ai < an)
    {
        Appointment* ap = aarr[ai];
        if (ap != nullptr && core_str_cmp(ap->getDateConst(), todayBuf) == 0)
        {
            totalToday++;
            if (core_str_cmp(ap->getStatusConst(), "pending") == 0)
            {
                pend++;
            }
            else if (core_str_cmp(ap->getStatusConst(), "completed") == 0)
            {
                comp++;
            }
            else if (core_str_cmp(ap->getStatusConst(), "no-show") == 0)
            {
                noshow++;
            }
            else if (core_str_cmp(ap->getStatusConst(), "cancelled") == 0)
            {
                cancelled++;
            }
        }
        ai++;
    }

    float revenue = 0.0f;
    int bn = bills->size();
    Bill** barr = bills->getAll();
    int bi = 0;
    while (bi < bn)
    {
        Bill* bill = barr[bi];
        if (bill != nullptr && core_str_cmp(bill->getStatusConst(), "paid") == 0 &&
            core_str_cmp(bill->getDateConst(), todayBuf) == 0)
        {
            revenue += bill->getAmount();
        }
        bi++;
    }

    medi_ui_print_line("--- Daily report ---");
    char line[256];
    line[0] = '\0';
    core_str_cat(line, "Total appointments today: ");
    char tmp[32];
    core_int_to_str(totalToday, tmp);
    core_str_cat(line, tmp);
    medi_ui_print_line(line);

    medi_ui_print_str("Breakdown — Pending: ");
    core_int_to_str(pend, tmp);
    medi_ui_print_str(tmp);
    medi_ui_print_str(", Completed: ");
    core_int_to_str(comp, tmp);
    medi_ui_print_str(tmp);
    medi_ui_print_str(", No-show: ");
    core_int_to_str(noshow, tmp);
    medi_ui_print_str(tmp);
    medi_ui_print_str(", Cancelled: ");
    core_int_to_str(cancelled, tmp);
    medi_ui_print_str(tmp);
    medi_ui_print_blank_line();

    char revLine[128];
    revLine[0] = '\0';
    core_str_cat(revLine, "Revenue collected today (paid bills): PKR ");
    char amt[64];
    core_float_to_str(revenue, amt);
    core_str_cat(revLine, amt);
    medi_ui_print_line(revLine);

    medi_ui_print_line("Patients with outstanding unpaid bills:");
    int pn = patients->size();
    Patient** parr = patients->getAll();
    int pi = 0;
    while (pi < pn)
    {
        Patient* patient = parr[pi];
        if (patient == nullptr)
        {
            pi++;
            continue;
        }
        float owed = 0.0f;
        int bj = 0;
        while (bj < bn)
        {
            Bill* bb = barr[bj];
            if (bb != nullptr && bb->getPatientID() == patient->getId() &&
                core_str_cmp(bb->getStatusConst(), "unpaid") == 0)
            {
                owed += bb->getAmount();
            }
            bj++;
        }
        if (owed > 0.0f)
        {
            cout << patient->getNameConst() << " | Total owed PKR " << owed << endl;
        }
        pi++;
    }

    medi_ui_print_line("Doctor-wise summary for today:");
    int dn = doctors->size();
    Doctor** darr = doctors->getAll();
    int di = 0;
    while (di < dn)
    {
        Doctor* doc = darr[di];
        if (doc == nullptr)
        {
            di++;
            continue;
        }
        int cCompleted = 0;
        int cPending = 0;
        int cNoShow = 0;
        int aj = 0;
        while (aj < an)
        {
            Appointment* ap = aarr[aj];
            if (ap != nullptr && ap->getDoctorID() == doc->getId() &&
                core_str_cmp(ap->getDateConst(), todayBuf) == 0)
            {
                if (core_str_cmp(ap->getStatusConst(), "completed") == 0)
                {
                    cCompleted++;
                }
                else if (core_str_cmp(ap->getStatusConst(), "pending") == 0)
                {
                    cPending++;
                }
                else if (core_str_cmp(ap->getStatusConst(), "no-show") == 0)
                {
                    cNoShow++;
                }
            }
            aj++;
        }

        cout << doc->getNameConst() << " | Completed " << cCompleted << " | Pending " << cPending
                  << " | No-show " << cNoShow << endl;
        di++;
    }
}

// Displays the requested records in the console.
void showAdminMenu(Admin* admin, Storage<Patient>* patients, Storage<Doctor>* doctors,
                   Storage<Appointment>* appointments, Storage<Bill>* bills, Storage<Prescription>* prescriptions,
                   FileHandler* fh)
{
    (void)admin;

    bool logout = false;
    while (!logout)
    {
        medi_ui_print_blank_line();
        medi_ui_print_line("Admin Panel — MediCore");
        medi_ui_print_line("======================");
        medi_ui_print_line("1. Add Doctor");
        medi_ui_print_line("2. Remove Doctor");
        medi_ui_print_line("3. View All Patients");
        medi_ui_print_line("4. View All Doctors");
        medi_ui_print_line("5. View All Appointments");
        medi_ui_print_line("6. View Unpaid Bills");
        medi_ui_print_line("7. Discharge Patient");
        medi_ui_print_line("8. View Security Log");
        medi_ui_print_line("9. Generate Daily Report");
        medi_ui_print_line("10. Logout");
        medi_ui_print_str("Select option: ");

        int choice = 0;
        medi_ui_read_int(&choice);
        if (!Validator::validateMenuChoice(choice, 1, 10))
        {
            medi_ui_print_line("Invalid menu choice.");
            continue;
        }

        if (choice == 1)
        {
            addDoctor(doctors, fh);
        }
        else if (choice == 2)
        {
            removeDoctor(doctors, appointments, fh);
        }
        else if (choice == 3)
        {
            viewAllPatients(patients, bills);
        }
        else if (choice == 4)
        {
            viewAllDoctors(doctors);
        }
        else if (choice == 5)
        {
            viewAllAppointments(appointments, patients, doctors);
        }
        else if (choice == 6)
        {
            viewUnpaidBills(bills, patients);
        }
        else if (choice == 7)
        {
            dischargePatient(patients, appointments, bills, prescriptions, doctors, admin, fh);
        }
        else if (choice == 8)
        {
            viewSecurityLog();
        }
        else if (choice == 9)
        {
            generateDailyReport(appointments, bills, patients, doctors);
        }
        else if (choice == 10)
        {
            logout = true;
        }
    }
}
