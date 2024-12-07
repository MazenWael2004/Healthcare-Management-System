#ifndef APPOINTMENT_H
#define APPOINTMENT_H


class Appointment {
    char ID[15];
    char Date[30];
    char Doctor_ID[15];
public:
    Appointment(const char* id, const char* date, const char* doctor_id);
    const char* getID() const;

    const char* getDate() const;

    const char* getDoctorID() const;
};

#endif //APPOINTMENT_H