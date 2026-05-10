#include "FileHandler.h"

#include "Admin.h"
#include "Appointment.h"
#include "Bill.h"
#include "CoreStrings.h"
#include "Doctor.h"
#include "FileNotFoundException.h"
#include "Patient.h"
#include "Prescription.h"

#include <fstream>
#include <iostream>

static const char* const PATH_PATIENTS = "patients.txt";
static const char* const PATH_DOCTORS = "doctors.txt";
static const char* const PATH_ADMIN = "admin.txt";
static const char* const PATH_APPOINTMENTS = "appointments.txt";
static const char* const PATH_BILLS = "bills.txt";
static const char* const PATH_PRESCRIPTIONS = "prescriptions.txt";
static const char* const PATH_SECURITY = "security_log.txt";
static const char* const PATH_DISCHARGED = "discharged.txt";

// Reads one CSV field by index from a line.
static bool fh_extract_csv_field(const char* line, int fieldIndex, char* out, int cap)
{
    int curField = 0;
    int pos = 0;
    out[0] = '\0';
    int i = 0;
    while (line[i] != '\0')
    {
        char ch = line[i];
        if (ch == ',')
        {
            // Move to next field when a comma delimiter appears.
            if (curField == fieldIndex)
            {
                out[pos] = '\0';
                return true;
            }
            curField++;
            pos = 0;
            out[0] = '\0';
        }
        else
        {
            if (curField == fieldIndex)
            {
                if (pos < cap - 1)
                {
                    out[pos] = ch;
                    pos++;
                    out[pos] = '\0';
                }
            }
        }
        i++;
    }
    return curField == fieldIndex;
}

// Reads the first CSV field and converts it to an integer ID.
static int fh_first_field_int(const char* line)
{
    char buf[64];
    fh_extract_csv_field(line, 0, buf, sizeof(buf));
    return core_parse_int(buf);
}

// Appends one raw line to the target file.
static void fh_append_raw_line(const char* path, const char* line)
{
    std::ofstream out(path, std::ios::app);
    if (!out)
    {
        throw FileNotFoundException(path);
    }
    out << line << '\n';
}

// Loads all non-empty lines from a file into memory.
static bool fh_read_all_lines(const char* path, char*** linesOut, int* countOut)
{
    *linesOut = nullptr;
    *countOut = 0;
    std::ifstream in(path);
    if (!in)
    {
        return false;
    }
    int cap = 16;
    char** arr = new char*[cap];
    int n = 0;
    char buffer[2048];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        if (n >= cap)
        {
            int newCap = cap * 2;
            char** grown = new char*[newCap];
            int j = 0;
            while (j < n)
            {
                grown[j] = arr[j];
                j++;
            }
            delete[] arr;
            arr = grown;
            cap = newCap;
        }
        int L = core_str_len(buffer);
        char* dup = new char[L + 1];
        core_str_cpy(dup, buffer);
        arr[n] = dup;
        n++;
    }
    *linesOut = arr;
    *countOut = n;
    return true;
}

// Releases dynamically allocated line buffers.
static void fh_free_lines(char** lines, int count)
{
    int i = 0;
    while (i < count)
    {
        delete[] lines[i];
        i++;
    }
    delete[] lines;
}

// Writes all given lines back to a file.
static void fh_write_lines(const char* path, char** lines, int count)
{
    std::ofstream out(path, std::ios::trunc);
    if (!out)
    {
        throw FileNotFoundException(path);
    }
    int i = 0;
    while (i < count)
    {
        out << lines[i] << '\n';
        i++;
    }
}

// Converts a patient record into a CSV line.
static void fh_serialize_patient_line(Patient* p, char* buf, int cap)
{
    char idb[32];
    char ageb[32];
    char balb[64];
    core_int_to_str(p->getId(), idb);
    core_int_to_str(p->getAge(), ageb);
    core_float_to_str(p->getBalance(), balb);
    buf[0] = '\0';
    core_str_cat(buf, idb);
    core_str_cat(buf, ",");
    core_str_cat(buf, p->getName());
    core_str_cat(buf, ",");
    core_str_cat(buf, ageb);
    core_str_cat(buf, ",");
    char gbuf[8];
    gbuf[0] = p->getGender();
    gbuf[1] = '\0';
    core_str_cat(buf, gbuf);
    core_str_cat(buf, ",");
    core_str_cat(buf, p->getContact());
    core_str_cat(buf, ",");
    core_str_cat(buf, p->getPassword());
    core_str_cat(buf, ",");
    core_str_cat(buf, balb);
    (void)cap;
}

// Converts a doctor record into a CSV line.
static void fh_serialize_doctor_line(Doctor* d, char* buf, int cap)
{
    char idb[32];
    char feeb[64];
    core_int_to_str(d->getId(), idb);
    core_float_to_str(d->getFee(), feeb);
    buf[0] = '\0';
    core_str_cat(buf, idb);
    core_str_cat(buf, ",");
    core_str_cat(buf, d->getName());
    core_str_cat(buf, ",");
    core_str_cat(buf, d->getSpecialization());
    core_str_cat(buf, ",");
    core_str_cat(buf, d->getDoctorContact());
    core_str_cat(buf, ",");
    core_str_cat(buf, d->getPassword());
    core_str_cat(buf, ",");
    core_str_cat(buf, feeb);
    (void)cap;
}

// Converts a appointment record into a CSV line.
static void fh_serialize_appointment_line(Appointment* a, char* buf, int cap)
{
    char idb[32];
    char pidb[32];
    char didb[32];
    core_int_to_str(a->getAppointmentID(), idb);
    core_int_to_str(a->getPatientID(), pidb);
    core_int_to_str(a->getDoctorID(), didb);
    buf[0] = '\0';
    core_str_cat(buf, idb);
    core_str_cat(buf, ",");
    core_str_cat(buf, pidb);
    core_str_cat(buf, ",");
    core_str_cat(buf, didb);
    core_str_cat(buf, ",");
    core_str_cat(buf, a->getDate());
    core_str_cat(buf, ",");
    core_str_cat(buf, a->getTimeSlot());
    core_str_cat(buf, ",");
    core_str_cat(buf, a->getStatus());
    (void)cap;
}

// Converts a bill record into a CSV line.
static void fh_serialize_bill_line(Bill* b, char* buf, int cap)
{
    char idb[32];
    char pidb[32];
    char aidb[32];
    char amb[64];
    core_int_to_str(b->getBillID(), idb);
    core_int_to_str(b->getPatientID(), pidb);
    core_int_to_str(b->getAppointmentID(), aidb);
    core_float_to_str(b->getAmount(), amb);
    buf[0] = '\0';
    core_str_cat(buf, idb);
    core_str_cat(buf, ",");
    core_str_cat(buf, pidb);
    core_str_cat(buf, ",");
    core_str_cat(buf, aidb);
    core_str_cat(buf, ",");
    core_str_cat(buf, amb);
    core_str_cat(buf, ",");
    core_str_cat(buf, b->getStatus());
    core_str_cat(buf, ",");
    core_str_cat(buf, b->getDate());
    (void)cap;
}

// Converts a prescription record into a CSV line.
static void fh_serialize_prescription_line(Prescription* pr, char* buf, int cap)
{
    char idb[32];
    char apidb[32];
    char pidb[32];
    char didb[32];
    core_int_to_str(pr->getPrescriptionID(), idb);
    core_int_to_str(pr->getAppointmentID(), apidb);
    core_int_to_str(pr->getPatientID(), pidb);
    core_int_to_str(pr->getDoctorID(), didb);
    buf[0] = '\0';
    core_str_cat(buf, idb);
    core_str_cat(buf, ",");
    core_str_cat(buf, apidb);
    core_str_cat(buf, ",");
    core_str_cat(buf, pidb);
    core_str_cat(buf, ",");
    core_str_cat(buf, didb);
    core_str_cat(buf, ",");
    core_str_cat(buf, pr->getDate());
    core_str_cat(buf, ",");
    core_str_cat(buf, pr->getMedicines());
    core_str_cat(buf, ",");
    core_str_cat(buf, pr->getNotes());
    (void)cap;
}

// Finds the highest ID stored in a CSV file.
static int fh_max_id_file(const char* path)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(path, &lines, &n))
    {
        return 0;
    }
    int mx = 0;
    int i = 0;
    while (i < n)
    {
        int v = fh_first_field_int(lines[i]);
        if (v > mx)
        {
            mx = v;
        }
        i++;
    }
    fh_free_lines(lines, n);
    return mx;
}

// Loads patient rows from file into storage.
void FileHandler::loadPatients(Storage<Patient>* storage)
{
    storage->clear();
    std::ifstream in(PATH_PATIENTS);
    if (!in)
    {
        throw FileNotFoundException(PATH_PATIENTS);
    }
    char buffer[2048];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        char f0[64];
        char f1[128];
        char f2[64];
        char f3[16];
        char f4[64];
        char f5[64];
        char f6[64];
        fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
        fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
        fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
        fh_extract_csv_field(buffer, 3, f3, sizeof(f3));
        fh_extract_csv_field(buffer, 4, f4, sizeof(f4));
        fh_extract_csv_field(buffer, 5, f5, sizeof(f5));
        fh_extract_csv_field(buffer, 6, f6, sizeof(f6));
        int pid = core_parse_int(f0);
        int age = core_parse_int(f2);
        char gender = f3[0];
        float bal = core_parse_float(f6);
        Patient* p = new Patient(pid, f1, age, gender, f4, f5, bal);
        storage->add(p);
    }
}

// Loads doctor rows from file into storage.
void FileHandler::loadDoctors(Storage<Doctor>* storage)
{
    storage->clear();
    std::ifstream in(PATH_DOCTORS);
    if (!in)
    {
        throw FileNotFoundException(PATH_DOCTORS);
    }
    char buffer[2048];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        char f0[64];
        char f1[128];
        char f2[128];
        char f3[64];
        char f4[64];
        char f5[64];
        fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
        fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
        fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
        fh_extract_csv_field(buffer, 3, f3, sizeof(f3));
        fh_extract_csv_field(buffer, 4, f4, sizeof(f4));
        fh_extract_csv_field(buffer, 5, f5, sizeof(f5));
        int did = core_parse_int(f0);
        float fee = core_parse_float(f5);
        Doctor* d = new Doctor(did, f1, f2, f3, f4, fee);
        storage->add(d);
    }
}

// Loads the single admin record from file.
void FileHandler::loadAdmin(Admin* admin)
{
    std::ifstream in(PATH_ADMIN);
    if (!in)
    {
        throw FileNotFoundException(PATH_ADMIN);
    }
    char buffer[512];
    if (!in.getline(buffer, sizeof(buffer)))
    {
        return;
    }
    core_trim_newline(buffer);
    char f0[64];
    char f1[128];
    char f2[128];
    fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
    fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
    fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
    admin->setId(core_parse_int(f0));
    admin->setName(f1);
    admin->setPassword(f2);
}

// Loads appointment rows from file into storage.
void FileHandler::loadAppointments(Storage<Appointment>* storage)
{
    storage->clear();
    std::ifstream in(PATH_APPOINTMENTS);
    if (!in)
    {
        throw FileNotFoundException(PATH_APPOINTMENTS);
    }
    char buffer[2048];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        char f0[64];
        char f1[64];
        char f2[64];
        char f3[32];
        char f4[32];
        char f5[64];
        fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
        fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
        fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
        fh_extract_csv_field(buffer, 3, f3, sizeof(f3));
        fh_extract_csv_field(buffer, 4, f4, sizeof(f4));
        fh_extract_csv_field(buffer, 5, f5, sizeof(f5));
        Appointment* a =
            new Appointment(core_parse_int(f0), core_parse_int(f1), core_parse_int(f2), f3, f4, f5);
        storage->add(a);
    }
}

// Loads bill rows from file into storage.
void FileHandler::loadBills(Storage<Bill>* storage)
{
    storage->clear();
    std::ifstream in(PATH_BILLS);
    if (!in)
    {
        throw FileNotFoundException(PATH_BILLS);
    }
    char buffer[2048];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        char f0[64];
        char f1[64];
        char f2[64];
        char f3[64];
        char f4[64];
        char f5[64];
        fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
        fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
        fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
        fh_extract_csv_field(buffer, 3, f3, sizeof(f3));
        fh_extract_csv_field(buffer, 4, f4, sizeof(f4));
        fh_extract_csv_field(buffer, 5, f5, sizeof(f5));
        Bill* b = new Bill(core_parse_int(f0), core_parse_int(f1), core_parse_int(f2), core_parse_float(f3), f4, f5);
        storage->add(b);
    }
}

// Loads prescription rows from file into storage.
void FileHandler::loadPrescriptions(Storage<Prescription>* storage)
{
    storage->clear();
    std::ifstream in(PATH_PRESCRIPTIONS);
    if (!in)
    {
        throw FileNotFoundException(PATH_PRESCRIPTIONS);
    }
    char buffer[4096];
    while (in.getline(buffer, sizeof(buffer)))
    {
        core_trim_newline(buffer);
        if (core_str_len(buffer) == 0)
        {
            continue;
        }
        char f0[64];
        char f1[64];
        char f2[64];
        char f3[64];
        char f4[32];
        char f5[600];
        char f6[400];
        fh_extract_csv_field(buffer, 0, f0, sizeof(f0));
        fh_extract_csv_field(buffer, 1, f1, sizeof(f1));
        fh_extract_csv_field(buffer, 2, f2, sizeof(f2));
        fh_extract_csv_field(buffer, 3, f3, sizeof(f3));
        fh_extract_csv_field(buffer, 4, f4, sizeof(f4));
        fh_extract_csv_field(buffer, 5, f5, sizeof(f5));
        fh_extract_csv_field(buffer, 6, f6, sizeof(f6));
        Prescription* pr =
            new Prescription(core_parse_int(f0), core_parse_int(f1), core_parse_int(f2), core_parse_int(f3), f4, f5, f6);
        storage->add(pr);
    }
}

// Appends one patient row to the patient file.
void FileHandler::appendPatient(Patient* patient)
{
    char line[1024];
    fh_serialize_patient_line(patient, line, sizeof(line));
    fh_append_raw_line(PATH_PATIENTS, line);
}

// Appends one doctor row to the doctor file.
void FileHandler::appendDoctor(Doctor* doctor)
{
    char line[1024];
    fh_serialize_doctor_line(doctor, line, sizeof(line));
    fh_append_raw_line(PATH_DOCTORS, line);
}

// Appends one appointment row to the appointments file.
void FileHandler::appendAppointment(Appointment* appointment)
{
    char line[1024];
    fh_serialize_appointment_line(appointment, line, sizeof(line));
    fh_append_raw_line(PATH_APPOINTMENTS, line);
}

// Appends one bill row to the bills file.
void FileHandler::appendBill(Bill* bill)
{
    char line[1024];
    fh_serialize_bill_line(bill, line, sizeof(line));
    fh_append_raw_line(PATH_BILLS, line);
}

// Appends one prescription row to the prescriptions file.
void FileHandler::appendPrescription(Prescription* prescription)
{
    char line[2048];
    fh_serialize_prescription_line(prescription, line, sizeof(line));
    fh_append_raw_line(PATH_PRESCRIPTIONS, line);
}

// Rewrites one patient row matched by id.
void FileHandler::updatePatient(Patient* patient)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(PATH_PATIENTS, &lines, &n))
    {
        throw FileNotFoundException(PATH_PATIENTS);
    }
    char newline[1024];
    fh_serialize_patient_line(patient, newline, sizeof(newline));
    int i = 0;
    while (i < n)
    {
        if (fh_first_field_int(lines[i]) == patient->getId())
        {
            delete[] lines[i];
            int L = core_str_len(newline);
            char* repl = new char[L + 1];
            core_str_cpy(repl, newline);
            lines[i] = repl;
            break;
        }
        i++;
    }
    fh_write_lines(PATH_PATIENTS, lines, n);
    fh_free_lines(lines, n);
}

// Rewrites one appointment row matched by id.
void FileHandler::updateAppointment(Appointment* appointment)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(PATH_APPOINTMENTS, &lines, &n))
    {
        throw FileNotFoundException(PATH_APPOINTMENTS);
    }
    char newline[1024];
    fh_serialize_appointment_line(appointment, newline, sizeof(newline));
    int i = 0;
    while (i < n)
    {
        if (fh_first_field_int(lines[i]) == appointment->getAppointmentID())
        {
            delete[] lines[i];
            int L = core_str_len(newline);
            char* repl = new char[L + 1];
            core_str_cpy(repl, newline);
            lines[i] = repl;
            break;
        }
        i++;
    }
    fh_write_lines(PATH_APPOINTMENTS, lines, n);
    fh_free_lines(lines, n);
}

// Rewrites one bill row matched by id.
void FileHandler::updateBill(Bill* bill)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(PATH_BILLS, &lines, &n))
    {
        throw FileNotFoundException(PATH_BILLS);
    }
    char newline[1024];
    fh_serialize_bill_line(bill, newline, sizeof(newline));
    int i = 0;
    while (i < n)
    {
        if (fh_first_field_int(lines[i]) == bill->getBillID())
        {
            delete[] lines[i];
            int L = core_str_len(newline);
            char* repl = new char[L + 1];
            core_str_cpy(repl, newline);
            lines[i] = repl;
            break;
        }
        i++;
    }
    fh_write_lines(PATH_BILLS, lines, n);
    fh_free_lines(lines, n);
}

// Removes a doctor row matched by id.
void FileHandler::deleteDoctor(int doctorId)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(PATH_DOCTORS, &lines, &n))
    {
        throw FileNotFoundException(PATH_DOCTORS);
    }
    char** kept = new char*[n];
    int nk = 0;
    int i = 0;
    while (i < n)
    {
        if (fh_first_field_int(lines[i]) != doctorId)
        {
            kept[nk] = lines[i];
            nk++;
        }
        else
        {
            delete[] lines[i];
        }
        i++;
    }
    delete[] lines;
    fh_write_lines(PATH_DOCTORS, kept, nk);
    int j = 0;
    while (j < nk)
    {
        delete[] kept[j];
        j++;
    }
    delete[] kept;
}

// Removes a patient row matched by id.
void FileHandler::deletePatient(int patientId)
{
    char** lines = nullptr;
    int n = 0;
    if (!fh_read_all_lines(PATH_PATIENTS, &lines, &n))
    {
        throw FileNotFoundException(PATH_PATIENTS);
    }
    char** kept = new char*[n];
    int nk = 0;
    int i = 0;
    while (i < n)
    {
        if (fh_first_field_int(lines[i]) != patientId)
        {
            kept[nk] = lines[i];
            nk++;
        }
        else
        {
            delete[] lines[i];
        }
        i++;
    }
    delete[] lines;
    fh_write_lines(PATH_PATIENTS, kept, nk);
    int j = 0;
    while (j < nk)
    {
        delete[] kept[j];
        j++;
    }
    delete[] kept;
}

// Appends one security event row to the log file.
void FileHandler::logSecurityEvent(char* timestamp, char* role, char* enteredId, char* result)
{
    char line[512];
    line[0] = '\0';
    core_str_cat(line, timestamp);
    core_str_cat(line, ",");
    core_str_cat(line, role);
    core_str_cat(line, ",");
    core_str_cat(line, enteredId);
    core_str_cat(line, ",");
    core_str_cat(line, result);
    fh_append_raw_line(PATH_SECURITY, line);
}

// Moves a patient's rows to archive and removes active rows.
void FileHandler::archivePatient(int patientId)
{
    char** plines = nullptr;
    int pn = 0;
    if (!fh_read_all_lines(PATH_PATIENTS, &plines, &pn))
    {
        throw FileNotFoundException(PATH_PATIENTS);
    }
    int pi = 0;
    char patientRow[2048];
    patientRow[0] = '\0';
    int foundPatient = 0;
    while (pi < pn)
    {
        if (fh_first_field_int(plines[pi]) == patientId)
        {
            core_str_cpy(patientRow, plines[pi]);
            foundPatient = 1;
            break;
        }
        pi++;
    }

    char** alines = nullptr;
    int an = 0;
    if (!fh_read_all_lines(PATH_APPOINTMENTS, &alines, &an))
    {
        alines = nullptr;
        an = 0;
    }

    char** rlines = nullptr;
    int rn = 0;
    if (!fh_read_all_lines(PATH_PRESCRIPTIONS, &rlines, &rn))
    {
        rlines = nullptr;
        rn = 0;
    }

    char** blines = nullptr;
    int bn = 0;
    if (!fh_read_all_lines(PATH_BILLS, &blines, &bn))
    {
        blines = nullptr;
        bn = 0;
    }

    if (foundPatient)
    {
        fh_append_raw_line(PATH_DISCHARGED, patientRow);
    }

    int ai = 0;
    while (ai < an)
    {
        char pf[64];
        fh_extract_csv_field(alines[ai], 1, pf, sizeof(pf));
        if (core_parse_int(pf) == patientId)
        {
            fh_append_raw_line(PATH_DISCHARGED, alines[ai]);
        }
        ai++;
    }

    int ri = 0;
    while (ri < rn)
    {
        char pf[64];
        fh_extract_csv_field(rlines[ri], 2, pf, sizeof(pf));
        if (core_parse_int(pf) == patientId)
        {
            fh_append_raw_line(PATH_DISCHARGED, rlines[ri]);
        }
        ri++;
    }

    int bi = 0;
    while (bi < bn)
    {
        char pf[64];
        fh_extract_csv_field(blines[bi], 1, pf, sizeof(pf));
        if (core_parse_int(pf) == patientId)
        {
            fh_append_raw_line(PATH_DISCHARGED, blines[bi]);
        }
        bi++;
    }

    // Remove the patient from active records.
    FileHandler::deletePatient(patientId);

    // Keep only appointments for other patients.
    if (an > 0 && alines != nullptr)
    {
        char** kept = new char*[an];
        int nk = 0;
        int i = 0;
        while (i < an)
        {
            char pf[64];
            fh_extract_csv_field(alines[i], 1, pf, sizeof(pf));
            if (core_parse_int(pf) != patientId)
            {
                kept[nk] = alines[i];
                nk++;
            }
            else
            {
                delete[] alines[i];
            }
            i++;
        }
        delete[] alines;
        fh_write_lines(PATH_APPOINTMENTS, kept, nk);
        int j = 0;
        while (j < nk)
        {
            delete[] kept[j];
            j++;
        }
        delete[] kept;
    }
    else
    {
        std::ofstream clearA(PATH_APPOINTMENTS, std::ios::trunc);
        if (!clearA)
        {
            throw FileNotFoundException(PATH_APPOINTMENTS);
        }
        delete[] alines;
    }

    // Keep only prescriptions for other patients.
    if (rn > 0 && rlines != nullptr)
    {
        char** kept = new char*[rn];
        int nk = 0;
        int i = 0;
        while (i < rn)
        {
            char pf[64];
            fh_extract_csv_field(rlines[i], 2, pf, sizeof(pf));
            if (core_parse_int(pf) != patientId)
            {
                kept[nk] = rlines[i];
                nk++;
            }
            else
            {
                delete[] rlines[i];
            }
            i++;
        }
        delete[] rlines;
        fh_write_lines(PATH_PRESCRIPTIONS, kept, nk);
        int j = 0;
        while (j < nk)
        {
            delete[] kept[j];
            j++;
        }
        delete[] kept;
    }
    else
    {
        std::ofstream clearR(PATH_PRESCRIPTIONS, std::ios::trunc);
        if (!clearR)
        {
            throw FileNotFoundException(PATH_PRESCRIPTIONS);
        }
        delete[] rlines;
    }

    // Keep only bills for other patients.
    if (bn > 0 && blines != nullptr)
    {
        char** kept = new char*[bn];
        int nk = 0;
        int i = 0;
        while (i < bn)
        {
            char pf[64];
            fh_extract_csv_field(blines[i], 1, pf, sizeof(pf));
            if (core_parse_int(pf) != patientId)
            {
                kept[nk] = blines[i];
                nk++;
            }
            else
            {
                delete[] blines[i];
            }
            i++;
        }
        delete[] blines;
        fh_write_lines(PATH_BILLS, kept, nk);
        int j = 0;
        while (j < nk)
        {
            delete[] kept[j];
            j++;
        }
        delete[] kept;
    }
    else
    {
        std::ofstream clearB(PATH_BILLS, std::ios::trunc);
        if (!clearB)
        {
            throw FileNotFoundException(PATH_BILLS);
        }
        delete[] blines;
    }

    fh_free_lines(plines, pn);
}

// Returns the highest appointment id found on disk.
int FileHandler::maxAppointmentIdFromDisk()
{
    return fh_max_id_file(PATH_APPOINTMENTS);
}

// Returns the highest prescription id found on disk.
int FileHandler::maxPrescriptionIdFromDisk()
{
    return fh_max_id_file(PATH_PRESCRIPTIONS);
}

// Returns the highest bill id found on disk.
int FileHandler::maxBillIdFromDisk()
{
    return fh_max_id_file(PATH_BILLS);
}

// Returns the highest doctor id found on disk.
int FileHandler::maxDoctorIdFromDisk()
{
    return fh_max_id_file(PATH_DOCTORS);
}

// Returns the highest patient id found on disk.
int FileHandler::maxPatientIdFromDisk()
{
    return fh_max_id_file(PATH_PATIENTS);
}
