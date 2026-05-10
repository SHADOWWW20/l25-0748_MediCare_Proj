#include "Admin.h"

#include <iostream>




using std::cout;
using std::endl;
// Initializes an admin with empty inherited fields.
Admin::Admin()
    : Person()
{
}

// Initializes an admin with id, name, and password.
Admin::Admin(int aid, const char* aname, const char* pwd)
    : Person(aid, aname, pwd)
{
}

// Releases admin resources.
Admin::~Admin()
{
}

// Prints admin identity details.
void Admin::displayInfo()
{
    cout << "[Admin " << this->getId() << "] " << this->getNameConst() << endl;
}

// Returns the role label for this object.
char* Admin::getRole()
{
    static char roleBuf[] = "Admin";
    return roleBuf;
}
