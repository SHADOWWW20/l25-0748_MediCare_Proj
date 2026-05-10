#include "LoginSystem.h"

#include "CoreStrings.h"
#include "FileHandler.h"
#include "MediCoreUi.h"
#include "Validator.h"

#include <iostream>

// Shows role options and reads a valid role choice.
void LoginSystem_selectRole(int* choiceOut)
{
    medi_ui_print_line("Welcome to MediCore Hospital Management System");
    medi_ui_print_line("===============================================");
    medi_ui_print_line("Login as:");
    medi_ui_print_line("1. Patient");
    medi_ui_print_line("2. Doctor");
    medi_ui_print_line("3. Admin");
    medi_ui_print_line("4. Exit");
    int ch = 0;
    medi_ui_print_str("Enter choice: ");
    medi_ui_read_int(&ch);
    while (!Validator::validateMenuChoice(ch, 1, 4))
    {
        medi_ui_print_line("Invalid choice.");
        medi_ui_print_str("Enter choice: ");
        medi_ui_read_int(&ch);
    }
    *choiceOut = ch;
}

// Checks credentials and returns the matched user object.
bool LoginSystem_validateLogin(int roleChoice, int userId, char* password, Storage<Patient>* patients,
                               Storage<Doctor>* doctors, Admin* admin, Patient** outPatient,
                               Doctor** outDoctor, Admin** outAdmin)
{
    *outPatient = nullptr;
    *outDoctor = nullptr;
    *outAdmin = nullptr;

    if (roleChoice == 1)
    {
        Patient* p = patients->findByID(userId);
        if (p != nullptr && p->login(userId, password))
        {
            *outPatient = p;
            return true;
        }
        return false;
    }

    if (roleChoice == 2)
    {
        Doctor* d = doctors->findByID(userId);
        if (d != nullptr && d->login(userId, password))
        {
            *outDoctor = d;
            return true;
        }
        return false;
    }

    if (roleChoice == 3)
    {
        if (admin != nullptr && admin->login(userId, password))
        {
            *outAdmin = admin;
            return true;
        }
        return false;
    }

    return false;
}

// Writes a failed-login row to the security log.
static void login_log_failure(FileHandler* fh, char* roleLabel, int enteredId)
{
    (void)fh;
    char ts[96];
    core_timestamp_log(ts, sizeof(ts));
    char idbuf[32];
    core_int_to_str(enteredId, idbuf);
    FileHandler::logSecurityEvent(ts, roleLabel, idbuf, (char*)"FAILED");
}

// Writes an account-lock row to the security log.
static void login_log_lock(FileHandler* fh, char* roleLabel, int enteredId)
{
    (void)fh;
    char ts[96];
    core_timestamp_log(ts, sizeof(ts));
    char idbuf[32];
    core_int_to_str(enteredId, idbuf);
    FileHandler::logSecurityEvent(ts, roleLabel, idbuf, (char*)"FAILED_LOCKOUT");
}

// Runs one login attempt and updates lockout counters.
int LoginSystem_login(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin, FileHandler* fh,
                      int roleChoice, Patient** outPatient, Doctor** outDoctor, Admin** outAdmin,
                      int* failedAttemptsInOut)
{
    *outPatient = nullptr;
    *outDoctor = nullptr;
    *outAdmin = nullptr;

    char pwdBuf[128];
    medi_ui_print_str("Enter User ID: ");
    int uid = 0;
    if (!medi_ui_read_int(&uid))
    {
        medi_ui_print_line("Invalid ID.");
        return 0;
    }

    medi_ui_print_str("Enter Password: ");
    medi_ui_read_line(pwdBuf, sizeof(pwdBuf));

    bool ok =
        LoginSystem_validateLogin(roleChoice, uid, pwdBuf, patients, doctors, admin, outPatient, outDoctor, outAdmin);

    char roleLabel[32];
    roleLabel[0] = '\0';
    if (roleChoice == 1)
    {
        core_str_cpy(roleLabel, "Patient");
    }
    else if (roleChoice == 2)
    {
        core_str_cpy(roleLabel, "Doctor");
    }
    else if (roleChoice == 3)
    {
        core_str_cpy(roleLabel, "Admin");
    }
    else
    {
        core_str_cpy(roleLabel, "Unknown");
    }

    if (ok)
    {
        *failedAttemptsInOut = 0;
        return 1;
    }

    *failedAttemptsInOut = *failedAttemptsInOut + 1;
    login_log_failure(fh, roleLabel, uid);

    if (*failedAttemptsInOut >= 3)
    {
        // Lock the account flow after three failed attempts.
        medi_ui_print_line("Account locked. Contact admin.");
        login_log_lock(fh, roleLabel, uid);
        *failedAttemptsInOut = 0;
        return 2;
    }

    medi_ui_print_line("Invalid credentials.");
    return 0;
}
