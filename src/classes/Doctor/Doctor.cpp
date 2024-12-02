#include "cstring"
#include "Doctor.h"

Doctor::Doctor(const char* id, const char* name, const char* address) {
    strcpy(ID, id);
    strcpy(Name, name);
    strcpy(Address, address);
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