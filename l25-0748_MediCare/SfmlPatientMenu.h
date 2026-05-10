#ifndef SFMLPATIENTMENU_H
#define SFMLPATIENTMENU_H

#include "Storage.h"

class Patient;
class Doctor;
class Appointment;
class Bill;
class Prescription;

// Runs one patient backend option and captures console output.
void sfmlPatientRunOption(int option, const char* inputScript, Patient* patient, Storage<Doctor>* doctors,
                          Storage<Appointment>* appointments, Storage<Bill>* bills,
                          Storage<Prescription>* prescriptions, char* outputBuf, int outputCap);

#endif
