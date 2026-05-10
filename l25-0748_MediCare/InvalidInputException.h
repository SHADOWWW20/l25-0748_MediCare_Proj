#ifndef INVALIDINPUTEXCEPTION_H
#define INVALIDINPUTEXCEPTION_H

#include "HospitalException.h"

class InvalidInputException : public HospitalException
{
public:
    explicit InvalidInputException(const char* detail);
    virtual ~InvalidInputException();
};

#endif

