#include "Bill.h"

#include "CoreStrings.h"

// Initializes a bill with empty values.
Bill::Bill()
{
    this->billID = 0;
    this->patientID = 0;
    this->appointmentID = 0;
    this->amount = 0.0f;
    this->status[0] = '\0';
    this->date[0] = '\0';
}

// Initializes a bill with provided details.
Bill::Bill(int bid, int pid, int aid, float amt, const char* st, const char* dt)
{
    this->billID = bid;
    this->patientID = pid;
    this->appointmentID = aid;
    this->amount = amt;
    this->status[0] = '\0';
    this->date[0] = '\0';
    core_str_cpy(this->status, st);
    core_str_cpy(this->date, dt);
}

// Releases bill resources.
Bill::~Bill()
{
}

// Returns the bill id.
int Bill::getId() const
{
    return this->billID;
}

// Returns the bill id field.
int Bill::getBillID() const
{
    return this->billID;
}

// Returns the linked patient id.
int Bill::getPatientID() const
{
    return this->patientID;
}

// Returns the linked appointment id.
int Bill::getAppointmentID() const
{
    return this->appointmentID;
}

// Returns the bill amount.
float Bill::getAmount() const
{
    return this->amount;
}

// Returns the editable bill status text.
char* Bill::getStatus()
{
    return this->status;
}

// Returns the read-only bill status text.
const char* Bill::getStatusConst() const
{
    return this->status;
}

// Returns the editable bill date text.
char* Bill::getDate()
{
    return this->date;
}

// Returns the read-only bill date text.
const char* Bill::getDateConst() const
{
    return this->date;
}

// Updates the bill id field.
void Bill::setBillID(int v)
{
    this->billID = v;
}

// Updates the linked patient id.
void Bill::setPatientID(int v)
{
    this->patientID = v;
}

// Updates the linked appointment id.
void Bill::setAppointmentID(int v)
{
    this->appointmentID = v;
}

// Updates the bill amount.
void Bill::setAmount(float v)
{
    this->amount = v;
}

// Updates the bill status text.
void Bill::setStatus(const char* v)
{
    core_str_cpy(this->status, v);
}

// Updates the bill date text.
void Bill::setDate(const char* v)
{
    core_str_cpy(this->date, v);
}
