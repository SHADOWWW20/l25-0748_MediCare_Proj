#include "SfmlPatientMenu.h"

#if defined(MEDICORE_ENABLE_SFML)

#include "PatientMenu.h"

#include <iostream>
#include <sstream>
#include <string>

// Replaces '|' with newlines so one-line GUI input can feed backend prompts.
static void sfmlPatientNormalize(const char* src, std::string& out)
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

// Copies captured text to a fixed char array for SFML text drawing.
static void sfmlPatientCopy(const std::string& s, char* out, int cap)
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

// Executes a patient action by calling existing backend functions only.
void sfmlPatientRunOption(int option, const char* inputScript, Patient* patient, Storage<Doctor>* doctors,
                          Storage<Appointment>* appointments, Storage<Bill>* bills,
                          Storage<Prescription>* prescriptions, char* outputBuf, int outputCap)
{
    std::string normalized;
    sfmlPatientNormalize(inputScript, normalized);
    std::istringstream fakeIn(normalized);
    std::ostringstream fakeOut;

    std::streambuf* oldCin = std::cin.rdbuf(fakeIn.rdbuf());
    std::streambuf* oldCout = std::cout.rdbuf(fakeOut.rdbuf());

    if (option == 1)
    {
        bookAppointment(patient, doctors, appointments, bills, nullptr);
    }
    else if (option == 2)
    {
        cancelAppointment(patient, appointments, doctors, bills, nullptr);
    }
    else if (option == 3)
    {
        viewAppointments(patient, appointments, doctors);
    }
    else if (option == 4)
    {
        viewMedicalRecords(patient, prescriptions, doctors);
    }
    else if (option == 5)
    {
        viewBills(patient, bills);
    }
    else if (option == 6)
    {
        payBill(patient, bills, nullptr);
    }
    else if (option == 7)
    {
        topUpBalance(patient, nullptr);
    }

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    sfmlPatientCopy(fakeOut.str(), outputBuf, outputCap);
}

#else

void sfmlPatientRunOption(int, const char*, Patient*, Storage<Doctor>*, Storage<Appointment>*, Storage<Bill>*,
                          Storage<Prescription>*, char* outputBuf, int outputCap)
{
    if (outputCap > 0)
    {
        outputBuf[0] = '\0';
    }
}

#endif
