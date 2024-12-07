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

short CntDoctorID = 0,  CntAppointmentSec = 0 , CntAppointmentID= 0,CntDoctorNameSec = 0,CntDoctorNameLL = 0;
void InsertByPrimaryIndexDoctor(char id[],short offset){
    /// First, Convert ID into integer to easily compare.
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt",ios::in | ios::out | ios::binary);

    int ID_to_be_inserted = 0;
    for(int i =0; id[i] != '\0'; i++){
        ID_to_be_inserted*=10;
        ID_to_be_inserted+=(id[i]-'0');
    }

    int temp = 0;
    short of = 0;
    bool check  = false;
    if(CntDoctorID == 0){ /// 1 ST RECORD TO BE INSERTED..
        /// Simply Insert inside the index file.
        primaryDoctor.seekp(0,ios::beg);
        primaryDoctor.write((char*)&ID_to_be_inserted,sizeof(int));
        primaryDoctor.write((char*)&offset,sizeof(short));
        CntDoctorID++;
        return;
    }

    primaryDoctor.read((char*)&temp,sizeof(temp));
    /// 1 2 3 4
    /// 2.5
    while(primaryDoctor.good()){
        if(temp > ID_to_be_inserted){
        check  = true;
        primaryDoctor.seekg(-4,ios::cur); /// Save the place(offset) in which the id is inserted.
        of = primaryDoctor.tellg();
        break;
        }
        primaryDoctor.seekg(2,ios::cur); /// Skip the current offset and move to next id.
        primaryDoctor.read((char*)&temp,sizeof(temp));
    }
    primaryDoctor.close();
    primaryDoctor.open("PrimaryIndexForDoctorID.txt",ios::in | ios::out | ios::binary);

    if(!check) { /// If in the last record...
        primaryDoctor.seekg(CntDoctorID*6,ios::beg);
        primaryDoctor.write((char*)&ID_to_be_inserted,sizeof(int));
        primaryDoctor.write((char*)&offset,sizeof(short));
        CntDoctorID++;
    }

    else{
        primaryDoctor.seekg((CntDoctorID-1)*6,ios::beg); /// Cursor at the beginning of last record to store it.
        int numend;
        short ofend;
        primaryDoctor.read((char*)& numend,sizeof(numend));
        primaryDoctor.read((char*)& ofend,sizeof(ofend));

        primaryDoctor.seekg(of,ios::beg); /// To the place in which to insert new id.

        while(primaryDoctor.good()){  /// Time to shift numbers...
          int tmpnum; short tmpof;
          int tmpnum1; short tmpof1;
          primaryDoctor.read((char*)& tmpnum,sizeof(tmpnum));
          primaryDoctor.read((char*)& tmpof,sizeof(tmpof));
          primaryDoctor.read((char*)& tmpnum1,sizeof(tmpnum1));
          primaryDoctor.read((char*)& tmpof1,sizeof(tmpof1));
          primaryDoctor.seekg(-6,ios::cur);
           primaryDoctor.write((char*)& tmpnum,sizeof(tmpnum));
          primaryDoctor.write((char*)& tmpof,sizeof(tmpof));

        }
        primaryDoctor.close();
        primaryDoctor.open("PrimaryIndexForDoctorID.txt",ios::in | ios::out | ios::binary);
        primaryDoctor.seekg(0,ios::end);
        primaryDoctor.write((char*)& numend,sizeof(numend)); /// Write the last record that we stored...
        primaryDoctor.write((char*)& ofend,sizeof(ofend));
        primaryDoctor.seekg(of,ios::beg);
        primaryDoctor.write((char*)&ID_to_be_inserted,sizeof(int));
        primaryDoctor.write((char*)&offset,sizeof(short));
        CntDoctorID++;


    }
}

short BinarySearchID(int id){
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt",ios::in | ios::out | ios::binary);
    short start = 0;
    short end = CntDoctorID-1;
    int mid;
    bool found = false;
    int temp;
    while(start <= end && !found){
         mid = (start+end)/2;
        primaryDoctor.seekg(mid*6,ios::beg);
        primaryDoctor.read((char*)& temp,sizeof(temp)); /// Take the id of middle,then compare

        if(temp == id){
            found    = true;
        }
        else if(temp > id){
            end = mid-1; /// Second half
            }
            else{
                    start = mid+1; /// First Half
            }
    }

    if(found){
        short of;
        primaryDoctor.seekg((mid*6)+4,ios::beg); /// Go to offset of found record.
        primaryDoctor.read((char*)&of,sizeof(of));
        primaryDoctor.close();
        return of;
    }
    else{
        short notfound = -1;
        primaryDoctor.close();
        return notfound;
    }
}

void DeletePrimaryDoctor(int id){
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt",ios::in | ios::out | ios::binary);
    short start = 0;
    short end = CntDoctorID-1;
    bool found = false;
    int temp,mid;
    while(start <= end && !found){
         mid = (start+end)/2;
        primaryDoctor.seekg(mid*6,ios::beg);
        primaryDoctor.read((char*)& temp,sizeof(temp)); /// Take the id of middle,then compare

        if(temp == id){
            found    = true;
        }
        else if(temp > id){
            end = mid-1; /// Second half
            }
            else{
                    start = mid+1; /// First Half
            }
    }

    if(found){
        /// Go to offset of found record.
        /// Suppose numbers 1 2 3 and you want to delete 2.
        primaryDoctor.seekg((mid+1)*6,ios::beg); /// Go to 3

        while(primaryDoctor.good()){ /// Start to shift
                /// 1 2 3 ---> 1 3 3 ---> 1 3
                int tmpnum; short tmpof;
                primaryDoctor.read((char*)& tmpnum,sizeof(tmpnum));
                primaryDoctor.read((char*)& tmpof,sizeof(tmpof));

                primaryDoctor.seekg(-12,ios::cur); /// Go back two records back to 2.
                primaryDoctor.write((char*)&tmpnum,sizeof(tmpnum));
                primaryDoctor.write((char*)&tmpof,sizeof(tmpof)); /// make 2 = 3--> 1 3 3
                primaryDoctor.seekg(6,ios::cur); /// go to the next number.

        }
        primaryDoctor.close();
        CntDoctorID--;


    }
    else{
        cout << "Cannot Find Record You are looking for." << endl;
    }
}

void insertbydoctorname_in_doctor(char doctor_name[30], char doctor_id[15]) {
    fstream DoctorNameSec("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
    short first = 0;
    short last = CntDoctorNameSec - 1;
    short mid;
    bool found = false;
    char temp[30];

    // Binary search to find if the doctor_name already exists
    while (first <= last && !found) {
        mid = (first + last) / 2;
        DoctorNameSec.seekg(mid * 32, ios::beg);
        DoctorNameSec.read((char*)&temp, sizeof(temp));

      if (strcmp(temp, doctor_name) == 0)
			found = true;
		else if (strcmp(temp, doctor_name) == 1)
			last = mid - 1;
		else
			first = mid + 1;
    }

    DoctorNameSec.close();

    if (!found) {
        if (CntDoctorNameLL == 0) { // Empty
            // Open in append mode if file is empty
            DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
            DoctorNameSec.seekg(0,ios::end);
            DoctorNameSec.write(doctor_name, 30);
            DoctorNameSec.write((char*)&CntDoctorNameLL, sizeof(CntDoctorNameLL)); // Initial count
            CntDoctorNameSec++;
            DoctorNameSec.close();

            // Write to linked list file
            fstream doc_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
            doc_name2.seekg(0,ios::end);
            doc_name2.write(doctor_name, 30);
            doc_name2.write(doctor_id, 15);
            short nega = -1;
            doc_name2.write((char*)&nega, sizeof(nega));
            doc_name2.close();

            CntDoctorNameLL++;
        } else {
            // Start shifting logic
            short offplace = -1;
            DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
            int i = 0;
            DoctorNameSec.seekg(0,ios::beg);

            while (i < CntDoctorNameSec) {
                char name[30];
                DoctorNameSec.read(name, sizeof(name));
                if (strcmp(doctor_name, name) == -1) {
                    offplace = DoctorNameSec.tellg();
                    offplace -= 30; // Adjust for name size
                    break;
                } else {
                    DoctorNameSec.seekg(2, ios::cur); /// mkkkkk kanet 32 Move to next name record (30 + 2 for short)
                }
                i++;
            }
            DoctorNameSec.close();

            if (offplace == -1) { // If it is the biggest one
                DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                DoctorNameSec.seekg(CntDoctorNameSec*32,ios::beg);
                DoctorNameSec.write(doctor_name, 30);
                DoctorNameSec.write((char*)&CntDoctorNameLL, sizeof(CntDoctorNameLL));
                CntDoctorNameSec++;
                CntDoctorNameLL++;
                DoctorNameSec.close();

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0,ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short t = -1;
                doctor_name2.write((char*)&t, sizeof(t));
                doctor_name2.close();
            } else {
                // Insert somewhere in the middle
                DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
                char nametmpend[30];
                short offsetnameend;
                DoctorNameSec.seekg((CntDoctorNameSec - 1) * 32, ios::beg);
                DoctorNameSec.read(nametmpend, 30);
                DoctorNameSec.read((char*)&offsetnameend, sizeof(offsetnameend));

                char tmp1[30], tmp2[30];
                short oftmp1, oftmp2;
                DoctorNameSec.seekg(offplace, ios::beg);
                DoctorNameSec.read(tmp1, 30);
                DoctorNameSec.read((char*)&oftmp1, sizeof(oftmp1));

                int i = 0;
                while (i < CntDoctorNameSec - 1) {
                    DoctorNameSec.read(tmp2, 30);
                    DoctorNameSec.read((char*)&oftmp2, sizeof(oftmp2));
                    DoctorNameSec.seekp(-32, ios::cur); // Go back to overwrite
                    DoctorNameSec.write(tmp1, 30);
                    DoctorNameSec.write((char*)&oftmp1, sizeof(oftmp1));
                    oftmp1 = oftmp2;
                    int j;
                    for (j = 0; tmp2[j] != '\0'; j++)
                        tmp1[j] = tmp2[j];
                    tmp1[j] = '\0';
                    i++;
                }
                DoctorNameSec.close();

                DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
                DoctorNameSec.seekp(offplace, ios::beg);
                DoctorNameSec.write(doctor_name, 30);
                DoctorNameSec.write((char*)&CntDoctorNameLL, sizeof(CntDoctorNameLL));

                DoctorNameSec.seekp(CntDoctorNameSec * 32, ios::beg);
                DoctorNameSec.write(nametmpend, 30);
                DoctorNameSec.write((char*)&offsetnameend, sizeof(offsetnameend));

                DoctorNameSec.close();
                CntDoctorNameSec++;

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0,ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short nega = -1;
                doctor_name2.write((char*)&nega, sizeof(nega));
                doctor_name2.close();
                CntDoctorNameLL++;
            }
        }
    } else { // if name is already there
        short off;
        DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
        DoctorNameSec.seekg((mid * 32) + 30, ios::beg); // Go to offset of found record
        DoctorNameSec.read((char*)&off, sizeof(off)); // Store the offset
        DoctorNameSec.close();

        fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
        doctor_name2.seekg((mid * 47), ios::beg); // Offset of found record
        short next = -2;
        while (1) {
            doctor_name2.read((char*)&next, sizeof(next)); // Get hold of pointer to check if there is another occurrence
            if (next == -1) { // Pointer = -1
                doctor_name2.seekg(-2, ios::cur); // Go to that name
                doctor_name2.write((char*)&CntDoctorNameLL, sizeof(CntDoctorNameLL));
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short t = -1;
                doctor_name2.write((char*)&t, sizeof(t));
                break;
            } else {
                doctor_name2.seekg((next*47)+45, ios::beg); // Simply go to the next occurrence of the record
            }
        }

        CntDoctorNameLL++;
        doctor_name2.close();
    }
}

void DeleteSecondaryDoctorName(char name[30])
{
    fstream DoctorNameSec("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
	short first = 0;
	short last = CntDoctorNameSec - 1;
	short mid;
	bool found = false;
	char temp[30];
	while (first <= last && !found) /// search for it
	{
		mid = (first + last) / 2;
		DoctorNameSec.seekg(mid * 32, ios::beg);
		DoctorNameSec.read((char*)&temp, sizeof(temp));

		if (strcmp(temp, name) == 0)
			found = true;
		else if (strcmp(temp, name) == 1)
			last = mid - 1;
		else
			first = mid + 1;
	}
	DoctorNameSec.close();
	if (!found)
        return ;


    DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out); // open namesec 5


        DoctorNameSec.seekg(((CntDoctorNameSec-1) * 32 ), ios::beg);
        DoctorNameSec.seekg((mid + 32 ), ios::beg);

        int i = mid/32 ;
        while(i < CntDoctorNameSec-1)               /// start to shift
        {
            char tempname[30] ;
            short tempof ;
            DoctorNameSec.read(tempname , 30) ;
            DoctorNameSec.read((char*)&tempof , sizeof(tempof)) ;

            DoctorNameSec.seekg(-64,ios::cur) ;
            DoctorNameSec.write(tempname , 30) ;
            DoctorNameSec.write((char*)&tempof , sizeof(tempof)) ;
            DoctorNameSec.seekg(32,ios::cur) ;
            i++ ;
        }
        CntDoctorNameSec-- ;
        DoctorNameSec.close() ;



}

void printAppointmentByID(const string& searchID) {
    ifstream appointmentFile("..\\data\\Appointment_DataFile.txt");

    if (!appointmentFile.is_open()) {
        cout << "Error: Could not open Appointment Data File.\n";
        return;
    }

    string appointmentID , appointmentDate, DoctorID;
    bool found = false;
    char s;
    while (getline(appointmentFile, appointmentID ,'|') &&
           getline(appointmentFile, appointmentDate ,'|') &&
           getline(appointmentFile, DoctorID)) {
        if (appointmentID == searchID) {
            cout << "Appointment Found:\n";
            cout << "-------------------------\n";
            cout << "Appointment ID: " << appointmentID << "\n";
            cout << "Appointment Date: " << appointmentDate << "\n";
            cout << "Doctor ID: " << DoctorID << "\n";
            cout << "-------------------------\n";
            found = true;
            break;
        }
    }
    if(!found){
        cout << "Doctor with ID " << searchID << " not found.\n";
    }

    appointmentFile.close();
}

void printDoctorByID(const string& searchID) {
    ifstream doctorFile("..\\data\\Doctor_DataFile.txt");

    if (!doctorFile.is_open()) {
        cout << "Error: Could not open Doctor Data File.\n";
        return;
    }

    string doctorID, doctorName, Address;
    bool found = false;
    char s;
    while (getline(doctorFile, doctorID ,'|') &&
           getline(doctorFile, doctorName,'|') &&
           getline(doctorFile, Address)) {
        if (doctorID == searchID) {
            cout << "Doctor Found:\n";
            cout << "-------------------------\n";
            cout << "Doctor ID: " << doctorID << "\n"
                 << "Name: " << doctorName << "\n"
                 << "Address: " << Address << "\n"
                 << "-------------------------\n";
            found = true;
            break;
        }
    }

    if (!found) {
        cout << "Doctor with ID " << searchID << " not found.\n";
    }

    doctorFile.close();
}


// Add doctor
// Not affect index yet.
int saveDoctor(const Doctor& doctor, fstream& DoctorDataFile) {
	DoctorDataFile.clear();
	DoctorDataFile.seekp(0, ios::end);
	if (DoctorDataFile) {
		string record = string(doctor.getID()) + "|" + doctor.getName() + "|" + doctor.getAddress();
		size_t length = record.size() + 1; // 1 is '|' after length indocator
		DoctorDataFile << length << "|" << record << "\n";
		return 1;
	}
	return 0;
}

bool isDoctorExists(const char* id, fstream& DoctorDataFile) {
	DoctorDataFile.clear();
	DoctorDataFile.seekg(0, ios::beg);

	string line;
	while (getline(DoctorDataFile, line)) {
		size_t pos = line.find('|');
		if (pos != string::npos) {
			string doctorID = line.substr(pos + 1, strlen(id));
			if (doctorID == id) {
				return true;
			}
		}
	}
	return false;
}

int addDoctor(const char* id, const char* name, const char* address, fstream& DoctorDataFile) {
	if (isDoctorExists(id, DoctorDataFile)) {
		cout << "Error: Doctor with this ID already exists.\n";
		return 0;
	}

	const Doctor doctor(id, name, address);
	return saveDoctor(doctor, DoctorDataFile);
}


int main()
{
    int option;
    bool flag = true;
    /// Opening all data files.
    fstream DoctorDataFile,AppointmentDataFile,PrimaryForDoctorID,PrimaryForAppointmentID,SecondaryForDoctorID,SecondaryForDoctorName;
    DoctorDataFile.open("..\\data\\Doctor_DataFile.txt",ios::out | ios:: in | ios:: app);
    AppointmentDataFile.open("data\\Appointment_DataFile.txt",ios::out | ios:: in |  ios:: app);
    PrimaryForDoctorID.open("indexes\\primary\\PrimaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    PrimaryForAppointmentID.open("indexes\\primary\\PrimaryIndexForAppointmentID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorID.open("indexes\\secondary\\SecondaryIndexForDoctorID.txt", ios::out | ios:: in |  ios:: app);
    SecondaryForDoctorName.open("indexes\\secondary\\SecondaryIndexForDoctorName.txt",ios::out | ios:: in |  ios:: app);
    ///-------------------------------------------------------------------------------------------------------
	if(!DoctorDataFile) {
		cout << "Error open doctor file!\n";
		return -1;
	}

	showWelcomeMessage();
	showMenu();
	string doctorId;
    string appointmentId;
    while(flag){
    	cin >> option;
    	switch (option) {
    		case 0:
    			showMenu();
    		break;
    		case 1:
    				cout << "Add New Doctor selected.\n";
    	            cout << "Please enter doctor data: ID, Name and Address\n";
    	            char id[15];
    	            char name[30];
    	            char address[30];
    	            cout << "Enter ID: ";
    	            cin.ignore();
                    cin.getline(id, 15);
                    cout << "Enter Name: ";
                    cin.getline(name, 30);
                    cout << "Enter Address: ";
                    cin.getline(address, 30);
    	            if(addDoctor(id, name, address, DoctorDataFile)) {
    	                cout << "Doctor added successfully.\n";
    	            }
    	            else {
    	                cout << "Unexpected error, please try again.\n";
    	            }
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
                cin >> doctorId;
                printDoctorByID(doctorId);
    		break;
    		case 8:
    				cout << "Print Appointment Info selected.\n";
                    cin >> appointmentId;
                    printAppointmentByID(appointmentId);
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
        showMenu();
    }



}
