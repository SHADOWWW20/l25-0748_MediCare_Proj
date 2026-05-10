#ifndef DOCTORMENU_H
#define DOCTORMENU_H

#include "Storage.h"

class Doctor;
class Appointment;
class Patient;
class Prescription;
class Bill;
class FileHandler;

void viewTodayAppointments(Doctor* doctor, Storage<Appointment>* appointments, Storage<Patient>* patients);

void markComplete(Doctor* doctor, Storage<Appointment>* appointments, FileHandler* fh);

void markNoShow(Doctor* doctor, Storage<Appointment>* appointments, Storage<Bill>* bills, FileHandler* fh);

void writePrescription(Doctor* doctor, Storage<Appointment>* appointments, Storage<Prescription>* prescriptions,
                       FileHandler* fh);

void viewPatientHistory(Doctor* doctor, Storage<Patient>* patients, Storage<Prescription>* prescriptions,
                        Storage<Appointment>* appointments);

void showDoctorMenu(Doctor* doctor, Storage<Appointment>* appointments, Storage<Patient>* patients,
                    Storage<Prescription>* prescriptions, Storage<Bill>* bills, FileHandler* fh);

#endif

