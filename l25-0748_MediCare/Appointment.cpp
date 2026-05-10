#include "Appointment.h"

#include "CoreStrings.h"

#include <iostream>

// Initializes an appointment with empty values.
Appointment::Appointment()
{
    this->appointmentID = 0;
    this->patientID = 0;
    this->doctorID = 0;
    this->date[0] = '\0';
    this->timeSlot[0] = '\0';
    this->status[0] = '\0';
}

// Initializes an appointment with provided details.
Appointment::Appointment(int aid, int pid, int did, const char* adate, const char* slot, const char* st)
{
    this->appointmentID = aid;
    this->patientID = pid;
    this->doctorID = did;
    this->date[0] = '\0';
    this->timeSlot[0] = '\0';
    this->status[0] = '\0';
    core_str_cpy(this->date, adate);
    core_str_cpy(this->timeSlot, slot);
    core_str_cpy(this->status, st);
}

// Releases appointment resources.
Appointment::~Appointment()
{
}

// Returns the appointment id.
int Appointment::getId() const
{
    return this->appointmentID;
}

// Returns the appointment id field.
int Appointment::getAppointmentID() const
{
    return this->appointmentID;
}

// Returns the linked patient id.
int Appointment::getPatientID() const
{
    return this->patientID;
}

// Returns the linked doctor id.
int Appointment::getDoctorID() const
{
    return this->doctorID;
}

// Returns the editable appointment date text.
char* Appointment::getDate()
{
    return this->date;
}

// Returns the read-only appointment date text.
const char* Appointment::getDateConst() const
{
    return this->date;
}

// Returns the editable time-slot text.
char* Appointment::getTimeSlot()
{
    return this->timeSlot;
}

// Returns the read-only time-slot text.
const char* Appointment::getTimeSlotConst() const
{
    return this->timeSlot;
}

// Returns the editable status text.
char* Appointment::getStatus()
{
    return this->status;
}

// Returns the read-only status text.
const char* Appointment::getStatusConst() const
{
    return this->status;
}

// Updates the appointment id field.
void Appointment::setAppointmentID(int v)
{
    this->appointmentID = v;
}

// Updates the linked patient id.
void Appointment::setPatientID(int v)
{
    this->patientID = v;
}

// Updates the linked doctor id.
void Appointment::setDoctorID(int v)
{
    this->doctorID = v;
}

// Updates the appointment date text.
void Appointment::setDate(const char* v)
{
    core_str_cpy(this->date, v);
}

// Updates the appointment time-slot text.
void Appointment::setTimeSlot(const char* v)
{
    core_str_cpy(this->timeSlot, v);
}

// Updates the appointment status text.
void Appointment::setStatus(const char* v)
{
    core_str_cpy(this->status, v);
}

// Checks whether a status is marked as cancelled.
static bool isCancelledStatus(const char* st)
{
    return core_str_cmp(st, "cancelled") == 0;
}

// Checks whether two active slots conflict.
bool Appointment::operator==(const Appointment& other) const
{
    if (this->doctorID != other.doctorID)
    {
        return false;
    }
    if (core_str_cmp(this->date, other.date) != 0)
    {
        return false;
    }
    if (core_str_cmp(this->timeSlot, other.timeSlot) != 0)
    {
        return false;
    }
    bool a = !isCancelledStatus(this->status);
    bool b = !isCancelledStatus(other.status);
    return a && b;
}

// Prints this object in one readable output line.
std::ostream& operator<<(std::ostream& os, const Appointment& a)
{
    os << "[Appt " << a.getAppointmentID() << "] Patient " << a.getPatientID() << " | Doctor " << a.getDoctorID()
       << " | Date " << a.getDateConst() << " | Slot " << a.getTimeSlotConst() << " | Status "
       << a.getStatusConst();
    return os;
}
