#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include "Storage.h"

class Patient;
class Doctor;
class Admin;
class Appointment;
class Bill;
class Prescription;

/**
 * Sole gateway for persistent CSV storage — all fstream access lives here.
 */
class FileHandler
{
public:
    static void loadPatients(Storage<Patient>* storage);
    static void loadDoctors(Storage<Doctor>* storage);
    static void loadAdmin(Admin* admin);
    static void loadAppointments(Storage<Appointment>* storage);
    static void loadBills(Storage<Bill>* storage);
    static void loadPrescriptions(Storage<Prescription>* storage);

    static void appendPatient(Patient* patient);
    static void appendDoctor(Doctor* doctor);
    static void appendAppointment(Appointment* appointment);
    static void appendBill(Bill* bill);
    static void appendPrescription(Prescription* prescription);

    static void updateAppointment(Appointment* appointment);
    static void updateBill(Bill* bill);
    static void updatePatient(Patient* patient);

    static void deleteDoctor(int doctorId);
    static void deletePatient(int patientId);

    static void logSecurityEvent(char* timestamp, char* role, char* enteredId, char* result);
    static void archivePatient(int patientId);

    static int maxAppointmentIdFromDisk();
    static int maxPrescriptionIdFromDisk();
    static int maxBillIdFromDisk();
    static int maxDoctorIdFromDisk();
    static int maxPatientIdFromDisk();
};

#endif

