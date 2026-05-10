#include "InsufficientFundsException.h"

#include "CoreStrings.h"

// Sets the message for low-balance payment attempts.
InsufficientFundsException::InsufficientFundsException()
    : HospitalException()
{
    core_str_cpy(this->message, "Insufficient balance for this operation.");
}

// Releases insufficient-funds exception resources.
InsufficientFundsException::~InsufficientFundsException()
{
}
