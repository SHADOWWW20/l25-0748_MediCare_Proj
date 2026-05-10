#include "MediCoreApp.h"

#include "Admin.h"
#include "AdminMenu.h"
#include "Appointment.h"
#include "Bill.h"
#include "Doctor.h"
#include "DoctorMenu.h"
#include "FileHandler.h"
#include "FileNotFoundException.h"
#include "LoginSystem.h"
#include "Patient.h"
#include "PatientMenu.h"
#include "Prescription.h"
#include "Storage.h"

#include <iostream>




using std::cout;
using std::endl;
// Loads all records from disk into memory storages.
static void medi_core_load_everything(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
                                      Storage<Appointment>* appointments, Storage<Bill>* bills,
                                      Storage<Prescription>* prescriptions)
{
    FileHandler::loadPatients(patients);
    FileHandler::loadDoctors(doctors);
    FileHandler::loadAdmin(admin);
    FileHandler::loadAppointments(appointments);
    FileHandler::loadBills(bills);
    FileHandler::loadPrescriptions(prescriptions);
}

// Releases all heap objects created for the app runtime.
static void medi_core_release_everything(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
                                         Storage<Appointment>* appointments, Storage<Bill>* bills,
                                         Storage<Prescription>* prescriptions)
{
    delete patients;
    delete doctors;
    delete admin;
    delete appointments;
    delete bills;
    delete prescriptions;
}

// Runs role selection, login, and menu routing.
static void medi_core_run_main_loop(Storage<Patient>* patients, Storage<Doctor>* doctors, Admin* admin,
                                    Storage<Appointment>* appointments, Storage<Bill>* bills,
                                    Storage<Prescription>* prescriptions)
{
    bool exitProgram = false;
    while (!exitProgram)
    {
        int roleChoice = 0;
        LoginSystem_selectRole(&roleChoice);

        if (roleChoice == 4)
        {
            exitProgram = true;
            break;
        }

        bool sessionStarted = false;
        int failures = 0;
        Patient* loggedPatient = nullptr;
        Doctor* loggedDoctor = nullptr;
        Admin* loggedAdmin = nullptr;

        while (!sessionStarted)
        {
            int loginCode =
                LoginSystem_login(patients, doctors, admin, nullptr, roleChoice, &loggedPatient, &loggedDoctor,
                                  &loggedAdmin, &failures);
            if (loginCode == 1)
            {
                sessionStarted = true;
                break;
            }
            if (loginCode == 2)
            {
                break;
            }
        }

        if (!sessionStarted)
        {
            continue;
        }

        if (roleChoice == 1 && loggedPatient != nullptr)
        {
            showPatientMenu(loggedPatient, doctors, appointments, bills, prescriptions, nullptr);
        }
        else if (roleChoice == 2 && loggedDoctor != nullptr)
        {
            showDoctorMenu(loggedDoctor, appointments, patients, prescriptions, bills, nullptr);
        }
        else if (roleChoice == 3 && loggedAdmin != nullptr)
        {
            showAdminMenu(loggedAdmin, patients, doctors, appointments, bills, prescriptions, nullptr);
        }
    }
}

// Runs the main program loop until the user exits.
void runMediCore()
{
    Storage<Patient>* patients = new Storage<Patient>();
    Storage<Doctor>* doctors = new Storage<Doctor>();
    Admin* admin = new Admin();
    Storage<Appointment>* appointments = new Storage<Appointment>();
    Storage<Bill>* bills = new Storage<Bill>();
    Storage<Prescription>* prescriptions = new Storage<Prescription>();

    try
    {
        medi_core_load_everything(patients, doctors, admin, appointments, bills, prescriptions);
        medi_core_run_main_loop(patients, doctors, admin, appointments, bills, prescriptions);
    }
    catch (FileNotFoundException& ex)
    {
        cout << ex.what() << endl;
    }
    catch (...)
    {
        cout << "Unexpected failure while starting MediCore." << endl;
    }

    medi_core_release_everything(patients, doctors, admin, appointments, bills, prescriptions);
}
