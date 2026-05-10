#ifndef HOSPITALEXCEPTION_H
#define HOSPITALEXCEPTION_H

#include <exception>

/**
 * Base exception for MediCore — human-readable message in fixed buffer.
 */
class HospitalException : public std::exception
{
protected:
    char message[200];

public:
    HospitalException();
    explicit HospitalException(const char* msg);
    virtual ~HospitalException();
    virtual const char* what() const noexcept override;
};

#endif

