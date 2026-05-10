#ifndef BILL_H
#define BILL_H

/**
 * Billing row tied to patient and appointment.
 */
class Bill
{
private:
    int billID;
    int patientID;
    int appointmentID;
    float amount;
    char status[20];
    char date[11];

public:
    Bill();
    Bill(int bid, int pid, int aid, float amt, const char* st, const char* dt);
    ~Bill();

    int getId() const;
    int getBillID() const;
    int getPatientID() const;
    int getAppointmentID() const;
    float getAmount() const;
    char* getStatus();
    const char* getStatusConst() const;
    char* getDate();
    const char* getDateConst() const;

    void setBillID(int v);
    void setPatientID(int v);
    void setAppointmentID(int v);
    void setAmount(float v);
    void setStatus(const char* v);
    void setDate(const char* v);
};

#endif

