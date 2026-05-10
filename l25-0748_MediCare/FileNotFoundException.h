#ifndef FILENOTFOUNDEXCEPTION_H
#define FILENOTFOUNDEXCEPTION_H

#include "HospitalException.h"

class FileNotFoundException : public HospitalException
{
public:
    explicit FileNotFoundException(const char* filepath);
    virtual ~FileNotFoundException();
};

#endif

