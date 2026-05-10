#ifndef DOCTOR_H
#define DOCTOR_H

#include "Person.h"

#include <iosfwd>

/**
 * Doctor profile — consulted via appointments (aggregation by ID).
 */
class Doctor : public Person
{
private:
    char specialization[50];
    char contact[12];
    float fee;

public:
    Doctor();
    Doctor(int did, const char* dname, const char* pspec, const char* dcontact, const char* pwd, float dfee);
    virtual ~Doctor();

    char* getSpecialization();
    const char* getSpecializationConst() const;
    char* getDoctorContact();
    const char* getDoctorContactConst() const;
    float getFee() const;
    void setSpecialization(const char* s);
    void setDoctorContact(const char* c);
    void setFee(float f);

    bool operator==(const Doctor& other) const;

    virtual void displayInfo();
    virtual char* getRole();

    friend std::ostream& operator<<(std::ostream& os, const Doctor& d);
};

#endif

