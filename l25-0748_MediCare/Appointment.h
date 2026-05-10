#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <iosfwd>

/**
 * Scheduled visit linking patient and doctor by IDs.
 */
class Appointment
{
private:
    int appointmentID;
    int patientID;
    int doctorID;
    char date[11];
    char timeSlot[6];
    char status[20];

public:
    Appointment();
    Appointment(int aid, int pid, int did, const char* adate, const char* slot, const char* st);
    ~Appointment();

    int getId() const;
    int getAppointmentID() const;
    int getPatientID() const;
    int getDoctorID() const;
    char* getDate();
    const char* getDateConst() const;
    char* getTimeSlot();
    const char* getTimeSlotConst() const;
    char* getStatus();
    const char* getStatusConst() const;

    void setAppointmentID(int v);
    void setPatientID(int v);
    void setDoctorID(int v);
    void setDate(const char* v);
    void setTimeSlot(const char* v);
    void setStatus(const char* v);

    bool operator==(const Appointment& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Appointment& a);
};

#endif

