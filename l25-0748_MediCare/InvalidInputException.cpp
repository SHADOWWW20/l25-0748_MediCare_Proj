#include "InvalidInputException.h"

#include "CoreStrings.h"

// Builds an invalid-input message from the given detail text.
InvalidInputException::InvalidInputException(const char* detail)
    : HospitalException()
{
    core_str_cpy(this->message, detail);
}

// Releases invalid-input exception resources.
InvalidInputException::~InvalidInputException()
{
}
