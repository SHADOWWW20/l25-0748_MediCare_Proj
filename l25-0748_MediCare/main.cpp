#include "MediCoreApp.h"
#include "SfmlFrontend.h"
#define MEDICORE_ENABLE_SFML
#if defined(MEDICORE_ENABLE_SFML)
#include <iostream>
#endif

// Starts the MediCore application.
int main()
{
#if defined(MEDICORE_ENABLE_SFML)
    // Gives the user a simple choice between console mode and SFML mode.
    std::cout << "Select mode:\n1. Console\n2. SFML\nEnter choice: ";
    int mode = 1;
    std::cin >> mode;
    if (mode == 2)
    {
        runMediCoreSfmlFrontend();
    }
    else
    {
        runMediCore();
    }
#else
    // Keeps existing console flow as default backend entry point.
    runMediCore();
#endif
    return 0;
}
