#include "SfmlDoctorMenu.h"

#if defined(MEDICORE_ENABLE_SFML)

#include "DoctorMenu.h"

#include <iostream>
#include <sstream>
#include <string>

// Converts GUI script format into backend input lines.
static void sfmlDoctorNormalize(const char* src, std::string& out)
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

// Copies captured backend output for SFML display.
static void sfmlDoctorCopy(const std::string& s, char* out, int cap)
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

// Calls selected doctor backend option and captures text output.
void sfmlDoctorRunOption(int option, const char* inputScript, Doctor* doctor, Storage<Appointment>* appointments,
                         Storage<Patient>* patients, Storage<Prescription>* prescriptions, Storage<Bill>* bills,
                         char* outputBuf, int outputCap)
{
    std::string normalized;
    sfmlDoctorNormalize(inputScript, normalized);

    std::istringstream fakeIn(normalized);
    std::ostringstream fakeOut;
    std::streambuf* oldCin = std::cin.rdbuf(fakeIn.rdbuf());
    std::streambuf* oldCout = std::cout.rdbuf(fakeOut.rdbuf());

    if (option == 1)
    {
        viewTodayAppointments(doctor, appointments, patients);
    }
    else if (option == 2)
    {
        markComplete(doctor, appointments, nullptr);
    }
    else if (option == 3)
    {
        markNoShow(doctor, appointments, bills, nullptr);
    }
    else if (option == 4)
    {
        writePrescription(doctor, appointments, prescriptions, nullptr);
    }
    else if (option == 5)
    {
        viewPatientHistory(doctor, patients, prescriptions, appointments);
    }

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);
    sfmlDoctorCopy(fakeOut.str(), outputBuf, outputCap);
}

#else

void sfmlDoctorRunOption(int, const char*, Doctor*, Storage<Appointment>*, Storage<Patient>*, Storage<Prescription>*,
                         Storage<Bill>*, char* outputBuf, int outputCap)
{
    if (outputCap > 0)
    {
        outputBuf[0] = '\0';
    }
}

#endif
