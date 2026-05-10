#include "Person.h"

#include "CoreStrings.h"

// Initializes a person with empty fields.
Person::Person()
{
    this->name[0] = '\0';
    this->id = 0;
    this->password[0] = '\0';
}

// Initializes a person with id, name, and password.
Person::Person(int pid, const char* pname, const char* pwd)
{
    this->id = pid;
    this->name[0] = '\0';
    this->password[0] = '\0';
    core_str_cpy(this->name, pname);
    core_str_cpy(this->password, pwd);
}

// Releases person resources.
Person::~Person()
{
}

// Returns the stored person id.
int Person::getId() const
{
    return this->id;
}

// Returns the editable person name text.
char* Person::getName()
{
    return this->name;
}

// Returns the read-only person name text.
const char* Person::getNameConst() const
{
    return this->name;
}

// Returns the editable password text.
char* Person::getPassword()
{
    return this->password;
}

// Updates the person id value.
void Person::setId(int pid)
{
    this->id = pid;
}

// Updates the person name text.
void Person::setName(const char* pname)
{
    core_str_cpy(this->name, pname);
}

// Updates the person password text.
void Person::setPassword(const char* pwd)
{
    core_str_cpy(this->password, pwd);
}

// Checks whether id and password both match.
bool Person::login(int pid, char* pwd)
{
    if (pid != this->id)
    {
        return false;
    }
    return core_str_cmp(this->password, pwd) == 0;
}
