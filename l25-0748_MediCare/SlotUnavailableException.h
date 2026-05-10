#ifndef SLOTUNAVAILABLEEXCEPTION_H
#define SLOTUNAVAILABLEEXCEPTION_H

#include "HospitalException.h"

class SlotUnavailableException : public HospitalException
{
public:
    SlotUnavailableException();
    virtual ~SlotUnavailableException();
};

#endif

