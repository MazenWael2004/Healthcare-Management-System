#ifndef DOCTOR_H
#define DOCTOR_H



class Doctor {
    char ID[15];
    char Name[30];
    char Address[30];
public:
    Doctor(const char* id, const char* name, const char* address);

    const char* getID() const;

    const char* getName() const;

    const char* getAddress() const;

    void printDoctorData() const;
};

#endif //DOCTOR_H
