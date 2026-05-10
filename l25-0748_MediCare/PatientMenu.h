#ifndef PATIENTMENU_H
#define PATIENTMENU_H

#include "Storage.h"

class Patient;
class Doctor;
class Appointment;
class Bill;
class Prescription;
class FileHandler;

/**
 * Bills storage is required internally for booking/cancellation bookkeeping even though the brief lists FileHandler only.
 */
void bookAppointment(Patient* patient, Storage<Doctor>* doctors, Storage<Appointment>* appointments,
                     Storage<Bill>* bills, FileHandler* fh);

void cancelAppointment(Patient* patient, Storage<Appointment>* appointments, Storage<Doctor>* doctors,
                       Storage<Bill>* bills, FileHandler* fh);

void viewAppointments(Patient* patient, Storage<Appointment>* appointments, Storage<Doctor>* doctors);

void viewMedicalRecords(Patient* patient, Storage<Prescription>* prescriptions, Storage<Doctor>* doctors);

void viewBills(Patient* patient, Storage<Bill>* bills);

void payBill(Patient* patient, Storage<Bill>* bills, FileHandler* fh);

void topUpBalance(Patient* patient, FileHandler* fh);

void showPatientMenu(Patient* patient, Storage<Doctor>* doctors, Storage<Appointment>* appointments,
                     Storage<Bill>* bills, Storage<Prescription>* prescriptions, FileHandler* fh);

#endif

