#include "SfmlAdminMenu.h"

#if defined(MEDICORE_ENABLE_SFML)

#include "AdminMenu.h"

#include <iostream>
#include <sstream>
#include <string>

// Converts GUI script text into multiple backend input lines.
static void sfmlAdminNormalize(const char* src, std::string& out)
{
    out.clear();
    int i = 0;
    while (src[i] != '\0')
    {
        out.push_back(src[i] == '|' ? '\n' : src[i]);
        i++;
    }
    out.push_back('\n');
}

// Copies backend output into a char buffer for drawing.
static void sfmlAdminCopy(const std::string& s, char* out, int cap)
{
    if (cap <= 0)
    {
        return;
    }
    int i = 0;
    while (i < cap - 1 && i < (int)s.size())
    {
        out[i] = s[(std::size_t)i];
        i++;
    }
    out[i] = '\0';
}

// Calls selected admin backend option and captures terminal text.
void sfmlAdminRunOption(int option, const char* inputScript, Admin* admin, Storage<Patient>* patients,
                        Storage<Doctor>* doctors, Storage<Appointment>* appointments, Storage<Bill>* bills,
                        Storage<Prescription>* prescriptions, char* outputBuf, int outputCap)
{
    std::string normalized;
    sfmlAdminNormalize(inputScript, normalized);

    std::istringstream fakeIn(normalized);
    std::ostringstream fakeOut;
    std::streambuf* oldCin = std::cin.rdbuf(fakeIn.rdbuf());
    std::streambuf* oldCout = std::cout.rdbuf(fakeOut.rdbuf());

    if (option == 1)
    {
        addDoctor(doctors, nullptr);
    }
    else if (option == 2)
    {
        removeDoctor(doctors, appointments, nullptr);
    }
    else if (option == 3)
    {
        viewAllPatients(patients, bills);
    }
    else if (option == 4)
    {
        viewAllDoctors(doctors);
    }
    else if (option == 5)
    {
        viewAllAppointments(appointments, patients, doctors);
    }
    else if (option == 6)
    {
        viewUnpaidBills(bills, patients);
    }
    else if (option == 7)
    {
        dischargePatient(patients, appointments, bills, prescriptions, doctors, admin, nullptr);
    }
    else if (option == 8)
    {
        viewSecurityLog();
    }
    else if (option == 9)
    {
        generateDailyReport(appointments, bills, patients, doctors);
    }

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    sfmlAdminCopy(fakeOut.str(), outputBuf, outputCap);
}

#else

void sfmlAdminRunOption(int, const char*, Admin*, Storage<Patient>*, Storage<Doctor>*, Storage<Appointment>*,
                        Storage<Bill>*, Storage<Prescription>*, char* outputBuf, int outputCap)
{
    if (outputCap > 0)
    {
        outputBuf[0] = '\0';
    }
}

#endif
