#include "Prescription.h"

#include "CoreStrings.h"

// Initializes a prescription with empty values.
Prescription::Prescription()
{
    this->prescriptionID = 0;
    this->appointmentID = 0;
    this->patientID = 0;
    this->doctorID = 0;
    this->date[0] = '\0';
    this->medicines[0] = '\0';
    this->notes[0] = '\0';
}

// Initializes a prescription with provided details.
Prescription::Prescription(int prid, int apid, int pid, int did, const char* dt, const char* med,
                           const char* nt)
{
    this->prescriptionID = prid;
    this->appointmentID = apid;
    this->patientID = pid;
    this->doctorID = did;
    this->date[0] = '\0';
    this->medicines[0] = '\0';
    this->notes[0] = '\0';
    core_str_cpy(this->date, dt);
    core_str_cpy(this->medicines, med);
    core_str_cpy(this->notes, nt);
}

// Releases prescription resources.
Prescription::~Prescription()
{
}

// Returns the prescription id.
int Prescription::getId() const
{
    return this->prescriptionID;
}

// Returns the prescription id field.
int Prescription::getPrescriptionID() const
{
    return this->prescriptionID;
}

// Returns the linked appointment id.
int Prescription::getAppointmentID() const
{
    return this->appointmentID;
}

// Returns the linked patient id.
int Prescription::getPatientID() const
{
    return this->patientID;
}

// Returns the linked doctor id.
int Prescription::getDoctorID() const
{
    return this->doctorID;
}

// Returns the editable prescription date text.
char* Prescription::getDate()
{
    return this->date;
}

// Returns the read-only prescription date text.
const char* Prescription::getDateConst() const
{
    return this->date;
}

// Returns the editable medicines text.
char* Prescription::getMedicines()
{
    return this->medicines;
}

// Returns the read-only medicines text.
const char* Prescription::getMedicinesConst() const
{
    return this->medicines;
}

// Returns the editable notes text.
char* Prescription::getNotes()
{
    return this->notes;
}

// Returns the read-only notes text.
const char* Prescription::getNotesConst() const
{
    return this->notes;
}

// Updates the prescription id field.
void Prescription::setPrescriptionID(int v)
{
    this->prescriptionID = v;
}

// Updates the linked appointment id.
void Prescription::setAppointmentID(int v)
{
    this->appointmentID = v;
}

// Updates the linked patient id.
void Prescription::setPatientID(int v)
{
    this->patientID = v;
}

// Updates the linked doctor id.
void Prescription::setDoctorID(int v)
{
    this->doctorID = v;
}

// Updates the prescription date text.
void Prescription::setDate(const char* v)
{
    core_str_cpy(this->date, v);
}

// Updates the medicines text.
void Prescription::setMedicines(const char* v)
{
    core_str_cpy(this->medicines, v);
}

// Updates the notes text.
void Prescription::setNotes(const char* v)
{
    core_str_cpy(this->notes, v);
}
