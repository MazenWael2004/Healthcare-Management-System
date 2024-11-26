#include <bits/stdc++.h>

using namespace std;
class Doctor{
    char ID[15];
    char Name[30];
    char Address[30];
public:
    Doctor(char* id,char* name,char* address){
        this->ID = id;
        this->Name = name;
        this->Address = address;
    }
    
    char* getID(){
        return ID;
    }

    char* getName(){
        return Name;
    }

    char* getAddress(){
        return Address;
    }
};
class Appointment{
    char ID[15];
    char Date[30];
    char Doctor_ID[15];
public:
    
    Appointment(char* id,char* date,char* doctor_id){
        this->ID = id;
        this->Date = date;
        this->Doctor_ID = doctor_id;
    }
    
    char* getID(){
        return ID;
    }

    char* getDate(){
        return Date;
    }

    char* getDoctorID(){
        return Doctor_ID;
    }
};
int main()
{
    int option;
    bool flag = 1;
    /// Opening all data files.
    fstream DoctorDataFile,AppointmentDataFile,PrimaryForDoctorID,PrimaryForAppointmentID,SecondaryForDoctorID,SecondaryForDoctorName;
    DoctorDataFile.open("Doctor_DataFile.txt",ios::out | ios:: in | ios:: app);
    AppointmentDataFile.open("Appointment_DataFile.txt",ios::out | ios:: in |  ios:: app);
    PrimaryForDoctorID.open("PrimaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    PrimaryForAppointmentID.open("PrimaryIndexForAppointmentID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorID.open("SecondaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorName.open("SecondaryIndexForDoctorName.txt",ios::out | ios:: in |  ios:: app);
    ///-------------------------------------------------------------------------------------------------------


    while(flag){
     cout << "-------------------------------------\n\n"
			<< "1)  Add New Doctor\n"
			<< "2)  Add New Appointment\n"
			<< "3)  Update Doctor Name\n"
			<< "4)  Update Appointment Date\n"
			<< "5)  Delete Appointment\n"
			<< "6)  Delete Doctor\n"
			<< "7)  Print Doctor info\n"
			<< "8)  Print Appointment info\n"
			<< "9)  Write Query\n"
			<< "10) Exit\n\n"
			<< "Please Enter Your Choice : ";
		cin >> option;
    }



}
