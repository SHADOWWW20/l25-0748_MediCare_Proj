#include "SlotUnavailableException.h"

#include "CoreStrings.h"

// Sets the message for already-booked doctor slots.
SlotUnavailableException::SlotUnavailableException()
    : HospitalException()
{
    core_str_cpy(this->message, "That time slot is already booked for this doctor.");
}

// Releases slot-unavailable exception resources.
SlotUnavailableException::~SlotUnavailableException()
{
}
