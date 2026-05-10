#include "HospitalException.h"

#include "CoreStrings.h"

// Initializes an empty exception message.
HospitalException::HospitalException()
{
    this->message[0] = '\0';
}

// Initializes the exception with a custom message.
HospitalException::HospitalException(const char* msg)
{
    this->message[0] = '\0';
    core_str_cpy(this->message, msg);
}

// Releases exception resources.
HospitalException::~HospitalException()
{
}

// Returns the stored exception message text.
const char* HospitalException::what() const noexcept
{
    return this->message;
}
