#include "Appointment.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <sstream> 

using namespace std;

// Constructor
Appointment::Appointment(const char* id, const char* date, const char* doctor)
{
    strncpy(this->appointmentID, id, 15);
    this->appointmentID[14] = '\0';
    strncpy(this->appointmentDate, date, 30);
    this->appointmentDate[29] = '\0';
    strncpy(this->doctorID, doctor, 30);
    this->doctorID[29] = '\0';
}

// Add Appointment
bool Appointment::addAppointment(const Appointment &appointment, vector<long> &availList)
{
    // Check if the appointmentID already exists in the primary index
    short offset = BinarySearchAppointmentID(atoi(appointment.getAppointmentID()));  // Search by appointmentID
    if (offset != -1)  // If the appointment ID already exists
    {
        cout << "Error: Appointment ID already exists.\n";
        return false;  // Appointment ID already exists, so return false
    }

    // Open the data file
    fstream file("..\\data\\Appointment_DataFile.txt", ios::in | ios::out | ios::app);
    if (!file)
    {
        cerr << "Error: Unable to open appointment data file.\n";
        return false;
    }

    // Calculate the position to write the new appointment
    if (!availList.empty())
    {
        // Reuse a slot from the avail list
        offset = availList.back();
        availList.pop_back();
        file.seekp(offset, ios::beg);
    }
    else
    {
        // Append the record to the end of the file
        file.seekp(0, ios::end);
        offset = file.tellp();
    }

    // Prepare the fields with delimiter '|'
    string appointmentID = appointment.getAppointmentID();
    string appointmentDate = appointment.getAppointmentDate();
    string doctorID = appointment.getDoctorID();

    // Create the record without trailing '|'
    string record = appointmentID + "|" + appointmentDate + "|" + doctorID;

    // Convert the record length to string
    string recordLength = to_string(record.size());

    // Write the record length and the record separated by '|', followed by a newline
    file << recordLength << "|" << record << "\n";

    file.close();

    // Insert into the primary index
    InsertByPrimaryIndexAppointment(appointment.getAppointmentID(), offset);

    // Insert into the secondary index
    insertbydoctor_id_in_appointments(appointment.getDoctorID(), appointment.getAppointmentID());

    return true;
}




bool Appointment::deleteAppointment(const char* appointmentID, vector<long>& availList) {
    // Convert appointmentID to string for easier handling
    string appointmentIDStr(appointmentID);

    // Search for the record by appointmentID in the primary index
    short offset = BinarySearchAppointmentID(atoi(appointmentID));
    if (offset == -1) { // If the appointment ID does not exist
        cout << "Error: Appointment ID not found.\n";
        return false;
    }

    // Open the data file
    fstream file("..\\data\\Appointment_DataFile.txt", ios::in | ios::out);
    if (!file) {
        cerr << "Error: Unable to open appointment data file.\n";
        return false;
    }

    // Seek to the record's position
    file.seekg(offset, ios::beg);

    // Read the length of the record
    string recordLengthStr;
    getline(file, recordLengthStr, '|');
    int recordLength = stoi(recordLengthStr);
    
    recordLength++;
    recordLengthStr = to_string(recordLength);

    // Read the rest of the record
    string record;
    getline(file, record);

    // Split the record by '|'
    stringstream ss(record);
    string appID, date, doctorID;
    getline(ss, appID, '|');
    getline(ss, date, '|');
    getline(ss, doctorID, '|');

    // Prepare a buffer to mark the record as deleted
    string buffer = "*" + appID; // Add '*' before APPID
    buffer += "|" + date + "|" + doctorID; // Append the date and doctorID to the buffer

    // Ensure the buffer length matches the updated record length
    while (buffer.size() < recordLength) {
        buffer += ' '; // Pad with spaces to maintain alignment
    }
    
    // Remaning content
    string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Overwrite the modified record back to the file
    file.seekp(offset, ios::beg); // Go back to the exact record position
    file.write(recordLengthStr.c_str(), recordLengthStr.size()); // Write the record length
    file.put('|');
    file.write(buffer.c_str(), buffer.size()); // Write the modified record with updated length

    // Ensure the record is properly terminated with a newline character
    file.put('\n');
    
    file.write(content.c_str(), content.size());

    file.close();

    // Add the offset to the available list
    if (find(availList.begin(), availList.end(), offset) == availList.end()) {
        availList.push_back(offset); // Avoid duplicates in the available list
    }

    // Remove from primary index
    DeletePrimaryAppointment(atoi(appointmentID));

    // Remove from secondary index using the extracted doctorID
    DeleteSecondaryDoctorID(doctorID.c_str()); // Use the parsed doctorID

    return true;
}




// Update Appointment
bool Appointment::updateAppointment(const char* appointmentID, const char* newDate)
{
    // Find the appointment in the primary index (e.g., using BinarySearchAppointmentID)
    short offset = BinarySearchAppointmentID(atoi(appointmentID));  // Convert appointmentID to an integer
    if (offset == -1)
    {
        cout << "Error: Appointment ID not found.\n";
        return false;
    }

    // Open the file to update the record
    fstream file("..\\data\\Appointment_DataFile.txt", ios::in | ios::out | ios::binary);
    if (!file)
    {
        cerr << "Error: Unable to open appointment data file.\n";
        return false;
    }

    // Calculate the position to update the appointmentDate (skip appointmentID and any other fields)
    file.seekg(offset, ios::beg);
    
    // Read the length of the record
    string recordLengthStr;
    getline(file, recordLengthStr, '|');
    int recordLength = stoi(recordLengthStr);
    
    // Read the rest of the record
    string record;
    getline(file, record);
    
    // Split the record by '|'
    stringstream ss(record);
    string appID, date, doctorID;
    getline(ss, appID, '|');
    getline(ss, date, '|');
    getline(ss, doctorID, '|');
    
    
    if(sizeof(newDate) > 30) {
        cout << "Error The Size of Date Should Be 30 or Less." << endl;
        return false;
    }
    
    string buffer = appID + '|' + newDate + '|' + doctorID;
    
    // Remaning content
    string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    // Overwrite the modified record back to the file
    file.seekp(offset, ios::beg); // Go back to the exact record position
    file.write(recordLengthStr.c_str(), recordLengthStr.size()); // Write the record length
    file.put('|');
    file.write(buffer.c_str(), buffer.size()); // Write the modified record with updated length

    // Ensure the record is properly terminated with a newline character
    file.put('\n');
    
    file.write(content.c_str(), content.size());

    file.close();
    return true;
}



// Print Appointment Info by ID
void Appointment::printAppointmentInfoByID(const char *appointmentID)
{
    short offset = BinarySearchAppointmentID(atoi(appointmentID));
    if (offset == -1)
    {
        cout << "Error: Appointment ID not found.\n";
        return;
    }

    fstream file("..\\data\\Appointment_DataFile.txt", ios::in | ios::binary);
    if (!file)
    {
        cerr << "Error: Unable to open appointment data file.\n";
        return;
    }

    file.seekg(offset, ios::beg);
    
    // Read the length of the record
    string recordLengthStr;
    getline(file, recordLengthStr, '|');
    int recordLength = stoi(recordLengthStr);
    
    // Read the rest of the record
    string record;
    getline(file, record);

    // Split the record by '|'
    stringstream ss(record);
    string appID, date, doctorID;
    getline(ss, appID, '|');
    getline(ss, date, '|');
    getline(ss, doctorID, '|');

    if (appID.size() == 0)
    {
        cout << "Error: Appointment data is corrupted or missing.\n";
        return;
    }

    // Check if the appointment has been marked as deleted by checking the first character of the appointmentID
    if (appID[0] == '*')
    {
        cout << "This appointment has been deleted.\n";
        file.close();
        return;  // Don't print deleted appointments
    }

    // If the appointment is not deleted, print the details
    cout << "Appointment ID: " << appID << endl;
    cout << "Appointment Date: " << date << endl;
    cout << "Doctor ID: " << doctorID << endl;

    file.close();
}
