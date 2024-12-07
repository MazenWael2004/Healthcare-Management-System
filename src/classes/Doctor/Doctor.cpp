#include <iostream>
#include <cstring>
#include "Doctor.h"

Doctor::Doctor(const char* id, const char* name, const char* address) {
    strncpy(this->ID, id, 15);
    this->ID[14] = '\0';
    strncpy(this->Name, name, 30);
    this->Name[29] = '\0';
    strncpy(this->Address, address, 30);
    this->Address[29] = '\0';
}

const char* Doctor::getID() const {
    return ID;
}

const char* Doctor::getName() const {
    return Name;
}

const char* Doctor::getAddress() const {
    return Address;
}

void Doctor::printDoctorData() const{
    std::cout << "Doctor Details:\n"
              << "ID: " << ID << "\n"
              << "Name: " << Name << "\n"
              << "Address: " << Address << "\n";
}