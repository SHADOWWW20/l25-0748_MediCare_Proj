#include "Validator.h"

#include "CoreStrings.h"

// Checks whether a date matches DD-MM-YYYY and is not in past years.
bool Validator::validateDate(const char* dateStr)
{
    if (dateStr == nullptr)
    {
        return false;
    }
    int len = core_str_len(dateStr);
    if (len != 10)
    {
        return false;
    }
    /* DD-MM-YYYY */
    int i = 0;
    while (i < 10)
    {
        char c = dateStr[i];
        if (i == 2 || i == 5)
        {
            if (c != '-')
            {
                return false;
            }
        }
        else
        {
            if (c < '0' || c > '9')
            {
                return false;
            }
        }
        i++;
    }
    char dayBuf[8];
    char monthBuf[8];
    char yearBuf[16];
    dayBuf[0] = dateStr[0];
    dayBuf[1] = dateStr[1];
    dayBuf[2] = '\0';
    monthBuf[0] = dateStr[3];
    monthBuf[1] = dateStr[4];
    monthBuf[2] = '\0';
    yearBuf[0] = dateStr[6];
    yearBuf[1] = dateStr[7];
    yearBuf[2] = dateStr[8];
    yearBuf[3] = dateStr[9];
    yearBuf[4] = '\0';
    int dd = core_parse_int(dayBuf);
    int mm = core_parse_int(monthBuf);
    int yyyy = core_parse_int(yearBuf);
    if (mm < 1 || mm > 12)
    {
        return false;
    }
    if (dd < 1 || dd > 31)
    {
        return false;
    }
    int cy = 0;
    core_get_current_year(&cy);
    if (yyyy < cy)
    {
        return false;
    }
    return true;
}

// Checks whether a slot is one of the fixed clinic times.
bool Validator::validateTimeSlot(const char* slotStr)
{
    if (slotStr == nullptr)
    {
        return false;
    }
    const char* slots[8];
    slots[0] = "09:00";
    slots[1] = "10:00";
    slots[2] = "11:00";
    slots[3] = "12:00";
    slots[4] = "13:00";
    slots[5] = "14:00";
    slots[6] = "15:00";
    slots[7] = "16:00";
    int k = 0;
    while (k < 8)
    {
        if (core_str_cmp(slotStr, slots[k]) == 0)
        {
            return true;
        }
        k++;
    }
    return false;
}

// Checks whether contact text has exactly 11 digits.
bool Validator::validateContact(const char* contactStr)
{
    if (contactStr == nullptr)
    {
        return false;
    }
    int len = core_str_len(contactStr);
    if (len != 11)
    {
        return false;
    }
    int i = 0;
    while (i < 11)
    {
        char c = contactStr[i];
        if (c < '0' || c > '9')
        {
            return false;
        }
        i++;
    }
    return true;
}

// Checks whether password length is at least six characters.
bool Validator::validatePassword(const char* passwordStr)
{
    if (passwordStr == nullptr)
    {
        return false;
    }
    return core_str_len(passwordStr) >= 6;
}

// Checks whether text is a positive numeric amount.
bool Validator::validatePositiveFloat(const char* floatStr)
{
    if (floatStr == nullptr)
    {
        return false;
    }
    int len = core_str_len(floatStr);
    if (len == 0)
    {
        return false;
    }
    int i = 0;
    int dotSeen = 0;
    int digitSeen = 0;
    if (floatStr[0] == '-')
    {
        return false;
    }
    while (i < len)
    {
        char c = floatStr[i];
        if (c == '.')
        {
            if (dotSeen)
            {
                return false;
            }
            dotSeen = 1;
        }
        else if (c >= '0' && c <= '9')
        {
            digitSeen = 1;
        }
        else
        {
            return false;
        }
        i++;
    }
    if (!digitSeen)
    {
        return false;
    }
    float v = core_parse_float(floatStr);
    return v > 0.0f;
}

// Checks whether menu choice is inside the valid range.
bool Validator::validateMenuChoice(int choice, int minVal, int maxVal)
{
    return choice >= minVal && choice <= maxVal;
}

// Checks whether two texts are equal without case sensitivity.
bool Validator::caseInsensitiveCompare(const char* a, const char* b)
{
    return core_str_equal_ci(a, b);
}
