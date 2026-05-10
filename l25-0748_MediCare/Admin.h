#ifndef ADMIN_H
#define ADMIN_H

#include "Person.h"

/**
 * Single administrator account loaded from admin.txt (composition with FileHandler loader).
 */
class Admin : public Person
{
public:
    Admin();
    Admin(int aid, const char* aname, const char* pwd);
    virtual ~Admin();

    virtual void displayInfo();
    virtual char* getRole();
};

#endif

