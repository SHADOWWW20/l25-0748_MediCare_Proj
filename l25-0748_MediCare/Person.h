#ifndef PERSON_H
#define PERSON_H

/**
 * Abstract identity base — Patient, Doctor, Admin derive from Person (inheritance hierarchy).
 */
class Person
{
protected:
    char name[50];
    int id;
    char password[50];

public:
    Person();
    Person(int pid, const char* pname, const char* pwd);
    virtual ~Person();

    int getId() const;
    char* getName();
    const char* getNameConst() const;
    char* getPassword();
    void setId(int pid);
    void setName(const char* pname);
    void setPassword(const char* pwd);

    virtual void displayInfo() = 0;
    virtual char* getRole() = 0;
    virtual bool login(int pid, char* pwd);
};

#endif

