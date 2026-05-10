#ifndef SFMLLOGINMENU_H
#define SFMLLOGINMENU_H

#include "Storage.h"

class Patient;
class Doctor;
class Admin;

// Runs one backend login attempt using script: "id|password".
int sfmlLoginTry(const char* inputScript, int roleChoice, Storage<Patient>* patients, Storage<Doctor>* doctors,
    Admin* admin, Patient** outPatient, Doctor** outDoctor, Admin** outAdmin, int* failCounter,
    char* outputBuf, int outputCap);

#endif
