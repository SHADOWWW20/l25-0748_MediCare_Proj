#ifndef ADMINMENU_H
#define ADMINMENU_H

#include "Storage.h"

class Doctor;
class Patient;
class Appointment;
class Bill;
class Prescription;
class Admin;
class FileHandler;
void addDoctor(Storage<Doctor>* doctors, FileHandler* fh);

void removeDoctor(Storage<Doctor>* doctors, Storage<Appointment>* appointments, FileHandler* fh);

void viewAllPatients(Storage<Patient>* patients, Storage<Bill>* bills);

void viewAllDoctors(Storage<Doctor>* doctors);

void viewAllAppointments(Storage<Appointment>* appointments, Storage<Patient>* patients,
                        Storage<Doctor>* doctors);

void viewUnpaidBills(Storage<Bill>* bills, Storage<Patient>* patients);

void dischargePatient(Storage<Patient>* patients, Storage<Appointment>* appointments, Storage<Bill>* bills,
                      Storage<Prescription>* prescriptions, Storage<Doctor>* doctors, Admin* admin,
                      FileHandler* fh);

void viewSecurityLog();

void generateDailyReport(Storage<Appointment>* appointments, Storage<Bill>* bills, Storage<Patient>* patients,
                         Storage<Doctor>* doctors);

void showAdminMenu(Admin* admin, Storage<Patient>* patients, Storage<Doctor>* doctors,
                   Storage<Appointment>* appointments, Storage<Bill>* bills, Storage<Prescription>* prescriptions,
                   FileHandler* fh);

#endif

