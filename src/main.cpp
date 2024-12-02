#include <bits/stdc++.h>
#include "classes/Doctor/Doctor.h"
#include "classes/Appointment/Appointment.h"

using namespace std;


void showWelcomeMessage() {
	cout << "=========================================================\n";
	cout << "==== Welcome in Health Care Management System - HCMS ====\n";
	cout << "=========================================================\n";
}

void showMenu() {
	cout
	<< "\n---------------------------\n"
	<< "Main menu\n"
	<< "---------------------------\n"
	<< "1)  Add New Doctor\n"
	<< "2)  Add New Appointment\n"
	<< "3)  Update Doctor Name\n"
	<< "4)  Update Appointment Date\n"
	<< "5)  Delete Appointment\n"
	<< "6)  Delete Doctor\n"
	<< "7)  Print Doctor info\n"
	<< "8)  Print Appointment info\n"
	<< "9)  Write Query\n"
	<< "10) Exit\n"
	<< "0) Show menu\n"
	<< "---------------------------\n"
	<< "Please Enter Your Choice : ";
}
int main()
{
    int option;
    bool flag = true;
    /// Opening all data files.
    fstream DoctorDataFile,AppointmentDataFile,PrimaryForDoctorID,PrimaryForAppointmentID,SecondaryForDoctorID,SecondaryForDoctorName;
    DoctorDataFile.open("data\\Doctor_DataFile.txt",ios::out | ios:: in | ios:: app);
    AppointmentDataFile.open("data\\Appointment_DataFile.txt",ios::out | ios:: in |  ios:: app);
    PrimaryForDoctorID.open("indexes\\primary\\PrimaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    PrimaryForAppointmentID.open("indexes\\primary\\PrimaryIndexForAppointmentID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorID.open("indexes\\secondary\\SecondaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorName.open("indexes\\secondary\\SecondaryIndexForDoctorName.txt",ios::out | ios:: in |  ios:: app);
    ///-------------------------------------------------------------------------------------------------------

	showWelcomeMessage();
	showMenu();
    while(flag){
    	cin >> option;
    	switch (option) {
    		case 0:
    			showMenu();
    		break;
    		case 1:
    				cout << "Add New Doctor selected.\n";
    		break;
    		case 2:
    				cout << "Add New Appointment selected.\n";
    		break;
    		case 3:
    				cout << "Update Doctor Name selected.\n";
    		break;
    		case 4:
    				cout << "Update Appointment Date selected.\n";
    		break;
    		case 5:
    				cout << "Delete Appointment selected.\n";
    		break;
    		case 6:
    				cout << "Delete Doctor selected.\n";
    		break;
    		case 7:
    				cout << "Print Doctor Info selected.\n";
    		break;
    		case 8:
    				cout << "Print Appointment Info selected.\n";
    		break;
    		case 9:
    				cout << "Write Query selected.\n";
    		break;
    		case 10:
    			cout << "Exiting the program.\n";
    			flag = false;
    		break;
    		default:
    			cout << "Invalid choice. Please try again.\n";
    	}
    }



}
