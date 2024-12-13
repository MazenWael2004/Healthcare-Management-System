#ifndef IndexUtils_H
#define IndexUtils_H

#include <string>
#include <vector>
#include <list>

using namespace std;

// Function declarations for primary index operations (Appointment)
void InsertByPrimaryIndexAppointment(const char id[], short offset);
short BinarySearchAppointmentID(int id);
void DeletePrimaryAppointment(int id);

// Function declarations for secondary index operations (Appointment)
void insertbydoctor_id_in_appointments(const char doctor_id[15], const char appointment_id[15]);
void DeleteSecondaryDoctorID(const char* doctorID);

#endif // IndexUtils_H
