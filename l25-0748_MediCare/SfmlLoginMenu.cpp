#include "SfmlLoginMenu.h"
#if defined(MEDICORE_ENABLE_SFML)

#include "LoginSystem.h"

#include <iostream>
#include <sstream>

// Converts script text by replacing '|' with newlines for backend cin reading.
static void sfmlNormalizeScript(const char* src, std::string& out)
{
    out.clear();
    int i = 0;
    while (src[i] != '\0')
    {
        char c = src[i];
        out.push_back(c == '|' ? '\n' : c);
        i++;
    }
    out.push_back('\n');
}

// Copies stream output into a fixed char buffer.
static void sfmlCopyToBuffer(const std::string& s, char* out, int cap)
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

// Runs one backend login attempt and captures all console text.
int sfmlLoginTry(const char* inputScript, int roleChoice, Storage<Patient>* patients, Storage<Doctor>* doctors,
                 Admin* admin, Patient** outPatient, Doctor** outDoctor, Admin** outAdmin, int* failCounter,
                 char* outputBuf, int outputCap)
{
    std::string normalized;
    sfmlNormalizeScript(inputScript, normalized);

    std::istringstream fakeIn(normalized);
    std::ostringstream fakeOut;

    std::streambuf* oldCin = std::cin.rdbuf(fakeIn.rdbuf());
    std::streambuf* oldCout = std::cout.rdbuf(fakeOut.rdbuf());

    int code = LoginSystem_login(patients, doctors, admin, nullptr, roleChoice, outPatient, outDoctor, outAdmin,
                                 failCounter);

    std::cin.rdbuf(oldCin);
    std::cout.rdbuf(oldCout);

    sfmlCopyToBuffer(fakeOut.str(), outputBuf, outputCap);
    return code;
}

#else

int sfmlLoginTry(const char*, int, Storage<Patient>*, Storage<Doctor>*, Admin*, Patient**, Doctor**, Admin**, int*,
                 char* outputBuf, int outputCap)
{
    if (outputCap > 0)
    {
        outputBuf[0] = '\0';
    }
    return 0;
}

#endif
