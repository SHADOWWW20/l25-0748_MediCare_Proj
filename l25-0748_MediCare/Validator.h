#ifndef VALIDATOR_H
#define VALIDATOR_H

/**
 * Centralised validation — static helpers only.
 */
class Validator
{
public:
    static bool validateDate(const char* dateStr);
    static bool validateTimeSlot(const char* slotStr);
    static bool validateContact(const char* contactStr);
    static bool validatePassword(const char* passwordStr);
    static bool validatePositiveFloat(const char* floatStr);
    static bool validateMenuChoice(int choice, int minVal, int maxVal);
    static bool caseInsensitiveCompare(const char* a, const char* b);
};

#endif

