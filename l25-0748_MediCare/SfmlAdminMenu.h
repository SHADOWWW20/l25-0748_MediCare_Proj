#ifndef SFMLADMINMENU_H
#define SFMLADMINMENU_H

#include "Storage.h"

class Admin;
class Patient;
class Doctor;
class Appointment;
class Bill;
class Prescription;

// Runs one admin backend option and captures console output.
void sfmlAdminRunOption(int option, const char* inputScript, Admin* admin, Storage<Patient>* patients,
                        Storage<Doctor>* doctors, Storage<Appointment>* appointments, Storage<Bill>* bills,
                        Storage<Prescription>* prescriptions, char* outputBuf, int outputCap);

#endif
