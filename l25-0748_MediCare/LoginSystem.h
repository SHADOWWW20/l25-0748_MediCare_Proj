#ifndef LOGINSYSTEM_H
#define LOGINSYSTEM_H

#include "Admin.h"
#include "Doctor.h"
#include "Patient.h"
#include "Storage.h"

class FileHandler;

/**
 * Authentication shell shared by all personas.
 */
void LoginSystem_selectRole(int* choiceOut);

bool LoginSystem_validateLogin(int roleChoice, int userId, char* password, Storage<Patient>* patients,
                               Storage<Doctor>* doctors, Admin* admin, Patient** outPatient,
                               Doctor** outDoctor, Admin** outAdmin);

/**
 * Credential prompt — returns 1 success (failures reset), 0 retryable failure, 2 lockout (session cleared).
 */
int LoginSystem_login(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin, FileHandler* fh,
                      int roleChoice, Patient** outPatient, Doctor** outDoctor, Admin** outAdmin,
                      int* failedAttemptsInOut);

#endif

