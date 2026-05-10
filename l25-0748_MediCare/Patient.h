#ifndef PATIENT_H
#define PATIENT_H

#include "Person.h"

#include <iosfwd>

/**
 * Registered hospital patient — aggregation targets via IDs elsewhere.
 */
class Patient : public Person
{
private:
    int age;
    char gender;
    char contact[12];
    float balance;

public:
    Patient();
    Patient(int pid, const char* pname, int page, char pgender, const char* pcontact, const char* pwd,
            float pbalance);
    virtual ~Patient();

    int getAge() const;
    char getGender() const;
    char* getContact();
    const char* getContactConst() const;
    float getBalance() const;
    void setAge(int a);
    void setGender(char g);
    void setContact(const char* c);
    void setBalance(float b);

    Patient& operator+=(float amount);
    Patient& operator-=(float amount);
    bool operator==(const Patient& other) const;

    virtual void displayInfo();
    virtual char* getRole();

    friend std::ostream& operator<<(std::ostream& os, const Patient& p);
};

#endif

