#include <cstring>
#include "Appointment.h"

Appointment::Appointment(const char* id, const char* date, const char* doctor_id) {
    strcpy(ID, id);
    strcpy(Date, date);
    strcpy(Doctor_ID, doctor_id);
}

const char* Appointment::getID() const {
    return ID;
}

const char* Appointment::getDate() const {
    return Date;
}

const char* Appointment::getDoctorID() const {
    return Doctor_ID;
}