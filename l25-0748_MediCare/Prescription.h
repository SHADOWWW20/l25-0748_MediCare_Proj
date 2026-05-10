#ifndef PRESCRIPTION_H
#define PRESCRIPTION_H

/**
 * Medical prescription tied to completed appointment.
 */
class Prescription
{
private:
    int prescriptionID;
    int appointmentID;
    int patientID;
    int doctorID;
    char date[11];
    char medicines[500];
    char notes[300];

public:
    Prescription();
    Prescription(int prid, int apid, int pid, int did, const char* dt, const char* med, const char* nt);
    ~Prescription();

    int getId() const;
    int getPrescriptionID() const;
    int getAppointmentID() const;
    int getPatientID() const;
    int getDoctorID() const;
    char* getDate();
    const char* getDateConst() const;
    char* getMedicines();
    const char* getMedicinesConst() const;
    char* getNotes();
    const char* getNotesConst() const;

    void setPrescriptionID(int v);
    void setAppointmentID(int v);
    void setPatientID(int v);
    void setDoctorID(int v);
    void setDate(const char* v);
    void setMedicines(const char* v);
    void setNotes(const char* v);
};

#endif

