#include "MediCoreUi.h"

#include "CoreStrings.h"

#include <iostream>




using std::cout;
using std::cin;
using std::endl;
// Prints one blank line in the console.
void medi_ui_print_blank_line()
{
    cout << endl;
}

// Prints text without adding a newline.
void medi_ui_print_str(const char* s)
{
    cout << s;
}

// Prints text followed by a newline.
void medi_ui_print_line(const char* s)
{
    cout << s << endl;
}

// Reads one input line into the provided buffer.
void medi_ui_read_line(char* buf, int cap)
{
    if (cap <= 0)
    {
        return;
    }
    buf[0] = '\0';
    cin.getline(buf, cap);
    if (cin.fail())
    {
        cin.clear();
        char drain[512];
        cin.getline(drain, sizeof(drain));
    }
}

// Reads an integer line and reports whether parsing succeeded.
bool medi_ui_read_int(int* out)
{
    char tmp[64];
    medi_ui_read_line(tmp, sizeof(tmp));
    int i = 0;
    while (tmp[i] != '\0')
    {
        char c = tmp[i];
        if (c < '0' || c > '9')
        {
            if (c != '-' || i != 0)
            {
                return false;
            }
        }
        i++;
    }
    *out = core_parse_int(tmp);
    return true;
}
