#ifndef SFMLDOCTORMENU_H
#define SFMLDOCTORMENU_H

#include "Storage.h"

class Doctor;
class Appointment;
class Patient;
class Prescription;
class Bill;

// Runs one doctor backend option and captures console output.
void sfmlDoctorRunOption(int option, const char* inputScript, Doctor* doctor, Storage<Appointment>* appointments,
                         Storage<Patient>* patients, Storage<Prescription>* prescriptions, Storage<Bill>* bills,
                         char* outputBuf, int outputCap);

#endif
