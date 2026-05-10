#include "Patient.h"

#include "CoreStrings.h"

#include <iostream>




using std::cout;
using std::endl;
// Initializes a patient with empty profile values.
Patient::Patient()
    : Person()
{
    this->age = 0;
    this->gender = ' ';
    this->contact[0] = '\0';
    this->balance = 0.0f;
}

// Initializes a patient with the provided profile data.
Patient::Patient(int pid, const char* pname, int page, char pgender, const char* pcontact, const char* pwd,
                 float pbalance)
    : Person(pid, pname, pwd)
{
    this->age = page;
    this->gender = pgender;
    this->contact[0] = '\0';
    core_str_cpy(this->contact, pcontact);
    this->balance = pbalance;
}

// Releases patient resources.
Patient::~Patient()
{
}

// Returns the patient's age.
int Patient::getAge() const
{
    return this->age;
}

// Returns the patient's gender code.
char Patient::getGender() const
{
    return this->gender;
}

// Returns the editable contact text.
char* Patient::getContact()
{
    return this->contact;
}

// Returns the read-only contact text.
const char* Patient::getContactConst() const
{
    return this->contact;
}

// Returns the current wallet balance.
float Patient::getBalance() const
{
    return this->balance;
}

// Updates the patient's age.
void Patient::setAge(int a)
{
    this->age = a;
}

// Updates the patient's gender code.
void Patient::setGender(char g)
{
    this->gender = g;
}

// Updates the patient's contact text.
void Patient::setContact(const char* c)
{
    core_str_cpy(this->contact, c);
}

// Updates the wallet balance amount.
void Patient::setBalance(float b)
{
    this->balance = b;
}

// Adds money to the patient balance.
Patient& Patient::operator+=(float amount)
{
    this->balance += amount;
    return *this;
}

// Deducts money from the patient balance.
Patient& Patient::operator-=(float amount)
{
    this->balance -= amount;
    return *this;
}

// Checks whether two patients share the same id.
bool Patient::operator==(const Patient& other) const
{
    return this->id == other.id;
}

// Prints patient details to the console.
void Patient::displayInfo()
{
    cout << *(this) << endl;
}

// Returns the role label for this object.
char* Patient::getRole()
{
    static char roleBuf[] = "Patient";
    return roleBuf;
}

// Prints this object in one readable output line.
std::ostream& operator<<(std::ostream& os, const Patient& p)
{
    os << "[Patient " << p.getId() << "] " << p.getNameConst() << " | Age " << p.getAge() << " | Gender "
       << p.getGender() << " | Contact " << p.getContactConst() << " | Balance PKR " << p.getBalance();
    return os;
}
