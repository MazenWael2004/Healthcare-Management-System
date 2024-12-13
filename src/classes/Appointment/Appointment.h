#ifndef APPOINTMENT_H
#define APPOINTMENT_H

#include <vector>
#include <map>
#include <list>
#include <string>
#include "../index_utils/IndexUtils.h"

using namespace std;

class Appointment
{
private:
    char appointmentID[14];
    char appointmentDate[29];
    char doctorID[14];

public:
    // Constructor
    Appointment(const char* id, const char* date, const char* doctor);

    // Getter functions
    const char *getAppointmentID() const { return appointmentID; }
    const char *getAppointmentDate() const { return appointmentDate; }
    const char *getDoctorID() const { return doctorID; }

    // Add Appointment
    static bool addAppointment(const Appointment &appointment, vector<long> &availList);

    // Delete Appointment
static bool deleteAppointment(const char* appointmentID, vector<long> &availList);


    // Update Appointment
static bool updateAppointment(const char* appointmentID, const char* newDate);


    // Print Appointment Info by ID
    static void printAppointmentInfoByID(const char* appointmentID);
};

#endif // APPOINTMENT_H
