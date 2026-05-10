#include "Doctor.h"

#include "CoreStrings.h"

#include <iostream>




using std::cout;
using std::endl;
// Initializes a doctor with empty profile values.
Doctor::Doctor()
    : Person()
{
    this->specialization[0] = '\0';
    this->contact[0] = '\0';
    this->fee = 0.0f;
}

// Initializes a doctor with the provided profile data.
Doctor::Doctor(int did, const char* dname, const char* pspec, const char* dcontact, const char* pwd,
               float dfee)
    : Person(did, dname, pwd)
{
    this->specialization[0] = '\0';
    this->contact[0] = '\0';
    core_str_cpy(this->specialization, pspec);
    core_str_cpy(this->contact, dcontact);
    this->fee = dfee;
}

// Releases doctor resources.
Doctor::~Doctor()
{
}

// Returns the editable specialization text.
char* Doctor::getSpecialization()
{
    return this->specialization;
}

// Returns the read-only specialization text.
const char* Doctor::getSpecializationConst() const
{
    return this->specialization;
}

// Returns the editable contact text.
char* Doctor::getDoctorContact()
{
    return this->contact;
}

// Returns the read-only contact text.
const char* Doctor::getDoctorContactConst() const
{
    return this->contact;
}

// Returns the consultation fee amount.
float Doctor::getFee() const
{
    return this->fee;
}

// Updates the specialization text.
void Doctor::setSpecialization(const char* s)
{
    core_str_cpy(this->specialization, s);
}

// Updates the doctor contact text.
void Doctor::setDoctorContact(const char* c)
{
    core_str_cpy(this->contact, c);
}

// Updates the consultation fee amount.
void Doctor::setFee(float f)
{
    this->fee = f;
}

// Checks whether two doctors share the same id.
bool Doctor::operator==(const Doctor& other) const
{
    return this->id == other.id;
}

// Prints doctor details to the console.
void Doctor::displayInfo()
{
    cout << *(this) << endl;
}

// Returns the role label for this object.
char* Doctor::getRole()
{
    static char roleBuf[] = "Doctor";
    return roleBuf;
}

// Prints this object in one readable output line.
std::ostream& operator<<(std::ostream& os, const Doctor& d)
{
    os << "[Doctor " << d.getId() << "] " << d.getNameConst() << " | Specialization " << d.getSpecializationConst()
       << " | Contact " << d.getDoctorContactConst() << " | Fee PKR " << d.getFee();
    return os;
}
