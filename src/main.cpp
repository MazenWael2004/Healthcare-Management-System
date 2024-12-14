#include <bits/stdc++.h>
#include "classes/Doctor/Doctor.h"
#include "classes/Appointment/Appointment.h"

using namespace std;

void showWelcomeMessage()
{
    cout << "=========================================================\n";
    cout << "==== Welcome in Health Care Management System - HCMS ====\n";
    cout << "=========================================================\n";
}

void showMenu()
{
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

vector<long> doctorAvailList; // Stores positions of deleted records

short CntDoctorID = 0, CntAppointmentSec = 0, CntAppointmentID = 0, CntDoctorNameSec = 0, CntDoctorNameLL = 0, CntDoctorIDSec = 0, CntDoctorIDLL = 0;
vector<pair<int, short>> primaryAppointmentRecords;
vector<long> availList;

void loadPrimaryIndexForAppointment()
{
    // Clear the in-memory records first
    primaryAppointmentRecords.clear();

    // Load Primary Index for Appointment
    fstream primaryAppointment("PrimaryIndexForAppointmentID.txt", ios::in | ios::binary);
    if (primaryAppointment)
    {
        CntAppointmentID = 0;
        int tempID;
        short tempOffset;
        while (primaryAppointment.read((char *)&tempID, sizeof(tempID)) &&
               primaryAppointment.read((char *)&tempOffset, sizeof(tempOffset)))
        {
            primaryAppointmentRecords.push_back({tempID, tempOffset});
            CntAppointmentID++;
        }
        primaryAppointment.close();
    }
}

void savePrimaryIndex()
{
    fstream primaryAppointment("PrimaryIndexForAppointmentID.txt", ios::out | ios::binary | ios::trunc);
    if (!primaryAppointment)
    {
        cerr << "Error: Could not open PrimaryIndexForAppointmentID.txt for saving.\n";
        return;
    }

    // Write all records to the primary index file
    for (const auto &record : primaryAppointmentRecords)
    {
        primaryAppointment.write((char *)&record.first, sizeof(record.first));   // Write ID
        primaryAppointment.write((char *)&record.second, sizeof(record.second)); // Write offset
    }

    primaryAppointment.close();
    cout << "Primary index saved successfully.\n";
}

void InsertByPrimaryIndexDoctor(char id[], short offset)
{
    /// First, Convert ID into integer to easily compare.
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt", ios::in | ios::out | ios::binary);

    int ID_to_be_inserted = 0;
    for (int i = 0; id[i] != '\0'; i++)
    {
        ID_to_be_inserted *= 10;
        ID_to_be_inserted += (id[i] - '0');
    }

    int temp = 0;
    short of = 0;
    bool check = false;
    if (CntDoctorID == 0)
    { /// 1 ST RECORD TO BE INSERTED..
        /// Simply Insert inside the index file.
        primaryDoctor.seekp(0, ios::beg);
        primaryDoctor.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryDoctor.write((char *)&offset, sizeof(short));
        CntDoctorID++;
        return;
    }

    primaryDoctor.read((char *)&temp, sizeof(temp));
    /// 1 2 3 4
    /// 2.5
    while (primaryDoctor.good())
    {
        if (temp > ID_to_be_inserted)
        {
            check = true;
            primaryDoctor.seekg(-4, ios::cur); /// Save the place(offset) in which the id is inserted.
            of = primaryDoctor.tellg();
            break;
        }
        primaryDoctor.seekg(2, ios::cur); /// Skip the current offset and move to next id.
        primaryDoctor.read((char *)&temp, sizeof(temp));
    }
    primaryDoctor.close();
    primaryDoctor.open("PrimaryIndexForDoctorID.txt", ios::in | ios::out | ios::binary);

    if (!check)
    { /// If in the last record...
        primaryDoctor.seekg(CntDoctorID * 6, ios::beg);
        primaryDoctor.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryDoctor.write((char *)&offset, sizeof(short));
        CntDoctorID++;
    }

    else
    {
        primaryDoctor.seekg((CntDoctorID - 1) * 6, ios::beg); /// Cursor at the beginning of last record to store it.
        int numend;
        short ofend;
        primaryDoctor.read((char *)&numend, sizeof(numend));
        primaryDoctor.read((char *)&ofend, sizeof(ofend));

        primaryDoctor.seekg(of, ios::beg); /// To the place in which to insert new id.

        while (primaryDoctor.good())
        { /// Time to shift numbers...
            int tmpnum;
            short tmpof;
            int tmpnum1;
            short tmpof1;
            primaryDoctor.read((char *)&tmpnum, sizeof(tmpnum));
            primaryDoctor.read((char *)&tmpof, sizeof(tmpof));
            primaryDoctor.read((char *)&tmpnum1, sizeof(tmpnum1));
            primaryDoctor.read((char *)&tmpof1, sizeof(tmpof1));
            primaryDoctor.seekg(-6, ios::cur);
            primaryDoctor.write((char *)&tmpnum, sizeof(tmpnum));
            primaryDoctor.write((char *)&tmpof, sizeof(tmpof));
        }
        primaryDoctor.close();
        primaryDoctor.open("PrimaryIndexForDoctorID.txt", ios::in | ios::out | ios::binary);
        primaryDoctor.seekg(0, ios::end);
        primaryDoctor.write((char *)&numend, sizeof(numend)); /// Write the last record that we stored...
        primaryDoctor.write((char *)&ofend, sizeof(ofend));
        primaryDoctor.seekg(of, ios::beg);
        primaryDoctor.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryDoctor.write((char *)&offset, sizeof(short));
        CntDoctorID++;
    }
}

short BinarySearchID(int id)
{
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt", ios::in | ios::out | ios::binary);
    short start = 0;
    short end = CntDoctorID - 1;
    int mid;
    bool found = false;
    int temp;
    while (start <= end && !found)
    {
        mid = (start + end) / 2;
        primaryDoctor.seekg(mid * 6, ios::beg);
        primaryDoctor.read((char *)&temp, sizeof(temp)); /// Take the id of middle,then compare

        if (temp == id)
        {
            found = true;
        }
        else if (temp > id)
        {
            end = mid - 1; /// Second half
        }
        else
        {
            start = mid + 1; /// First Half
        }
    }

    if (found)
    {
        short of;
        primaryDoctor.seekg((mid * 6) + 4, ios::beg); /// Go to offset of found record.
        primaryDoctor.read((char *)&of, sizeof(of));
        primaryDoctor.close();
        return of;
    }
    else
    {
        short notfound = -1;
        primaryDoctor.close();
        return notfound;
    }
}

void DeletePrimaryDoctor(int id)
{
    fstream primaryDoctor("PrimaryIndexForDoctorID.txt", ios::in | ios::out | ios::binary);
    short start = 0;
    short end = CntDoctorID - 1;
    bool found = false;
    int temp, mid;
    while (start <= end && !found)
    {
        mid = (start + end) / 2;
        primaryDoctor.seekg(mid * 6, ios::beg);
        primaryDoctor.read((char *)&temp, sizeof(temp)); /// Take the id of middle,then compare

        if (temp == id)
        {
            found = true;
        }
        else if (temp > id)
        {
            end = mid - 1; /// Second half
        }
        else
        {
            start = mid + 1; /// First Half
        }
    }

    if (found)
    {
        /// Go to offset of found record.
        /// Suppose numbers 1 2 3 and you want to delete 2.
        primaryDoctor.seekg((mid + 1) * 6, ios::beg); /// Go to 3

        while (primaryDoctor.good())
        { /// Start to shift
            /// 1 2 3 ---> 1 3 3 ---> 1 3
            int tmpnum;
            short tmpof;
            primaryDoctor.read((char *)&tmpnum, sizeof(tmpnum));
            primaryDoctor.read((char *)&tmpof, sizeof(tmpof));

            primaryDoctor.seekg(-12, ios::cur); /// Go back two records back to 2.
            primaryDoctor.write((char *)&tmpnum, sizeof(tmpnum));
            primaryDoctor.write((char *)&tmpof, sizeof(tmpof)); /// make 2 = 3--> 1 3 3
            primaryDoctor.seekg(6, ios::cur);                   /// go to the next number.
        }
        primaryDoctor.close();
        CntDoctorID--;
    }
    else
    {
        cout << "Cannot Find Record You are looking for." << endl;
    }
}

void insertbydoctorname_in_doctor(char doctor_name[30], char doctor_id[15])
{
    fstream DoctorNameSec("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
    short first = 0;
    short last = CntDoctorNameSec - 1;
    short mid;
    bool found = false;
    char temp[30];

    // Binary search to find if the doctor_name already exists
    while (first <= last && !found)
    {
        mid = (first + last) / 2;
        DoctorNameSec.seekg(mid * 32, ios::beg);
        DoctorNameSec.read((char *)&temp, sizeof(temp));

        if (strcmp(temp, doctor_name) == 0)
            found = true;
        else if (strcmp(temp, doctor_name) == 1)
            last = mid - 1;
        else
            first = mid + 1;
    }

    DoctorNameSec.close();

    if (!found)
    {
        if (CntDoctorNameLL == 0)
        { // Empty
            // Open in append mode if file is empty
            DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
            DoctorNameSec.seekg(0, ios::end);
            DoctorNameSec.write(doctor_name, 30);
            DoctorNameSec.write((char *)&CntDoctorNameLL, sizeof(CntDoctorNameLL)); // Initial count
            CntDoctorNameSec++;
            DoctorNameSec.close();

            // Write to linked list file
            fstream doc_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
            doc_name2.seekg(0, ios::end);
            doc_name2.write(doctor_name, 30);
            doc_name2.write(doctor_id, 15);
            short nega = -1;
            doc_name2.write((char *)&nega, sizeof(nega));
            doc_name2.close();

            CntDoctorNameLL++;
        }
        else
        {
            // Start shifting logic
            short offplace = -1;
            DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
            int i = 0;
            DoctorNameSec.seekg(0, ios::beg);

            while (i < CntDoctorNameSec)
            {
                char name[30];
                DoctorNameSec.read(name, sizeof(name));
                if (strcmp(doctor_name, name) == -1)
                {
                    offplace = DoctorNameSec.tellg();
                    offplace -= 30; // Adjust for name size
                    break;
                }
                else
                {
                    DoctorNameSec.seekg(2, ios::cur); /// mkkkkk kanet 32 Move to next name record (30 + 2 for short)
                }
                i++;
            }
            DoctorNameSec.close();

            if (offplace == -1)
            { // If it is the biggest one
                DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                DoctorNameSec.seekg(CntDoctorNameSec * 32, ios::beg);
                DoctorNameSec.write(doctor_name, 30);
                DoctorNameSec.write((char *)&CntDoctorNameLL, sizeof(CntDoctorNameLL));
                CntDoctorNameSec++;
                CntDoctorNameLL++;
                DoctorNameSec.close();

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short t = -1;
                doctor_name2.write((char *)&t, sizeof(t));
                doctor_name2.close();
            }
            else
            {
                // Insert somewhere in the middle
                DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
                char nametmpend[30];
                short offsetnameend;
                DoctorNameSec.seekg((CntDoctorNameSec - 1) * 32, ios::beg);
                DoctorNameSec.read(nametmpend, 30);
                DoctorNameSec.read((char *)&offsetnameend, sizeof(offsetnameend));

                char tmp1[30], tmp2[30];
                short oftmp1, oftmp2;
                DoctorNameSec.seekg(offplace, ios::beg);
                DoctorNameSec.read(tmp1, 30);
                DoctorNameSec.read((char *)&oftmp1, sizeof(oftmp1));

                int i = 0;
                while (i < CntDoctorNameSec - 1)
                {
                    DoctorNameSec.read(tmp2, 30);
                    DoctorNameSec.read((char *)&oftmp2, sizeof(oftmp2));
                    DoctorNameSec.seekp(-32, ios::cur); // Go back to overwrite
                    DoctorNameSec.write(tmp1, 30);
                    DoctorNameSec.write((char *)&oftmp1, sizeof(oftmp1));
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
                DoctorNameSec.write((char *)&CntDoctorNameLL, sizeof(CntDoctorNameLL));

                DoctorNameSec.seekp(CntDoctorNameSec * 32, ios::beg);
                DoctorNameSec.write(nametmpend, 30);
                DoctorNameSec.write((char *)&offsetnameend, sizeof(offsetnameend));

                DoctorNameSec.close();
                CntDoctorNameSec++;

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short nega = -1;
                doctor_name2.write((char *)&nega, sizeof(nega));
                doctor_name2.close();
                CntDoctorNameLL++;
            }
        }
    }
    else
    { // if name is already there
        short off;
        DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
        DoctorNameSec.seekg((mid * 32) + 30, ios::beg); // Go to offset of found record
        DoctorNameSec.read((char *)&off, sizeof(off));  // Store the offset
        DoctorNameSec.close();

        fstream doctor_name2("LLIndexForDoctorName.txt", ios::binary | ios::in | ios::out);
        doctor_name2.seekg((mid * 47), ios::beg); // Offset of found record
        short next = -2;
        while (1)
        {
            doctor_name2.read((char *)&next, sizeof(next)); // Get hold of pointer to check if there is another occurrence
            if (next == -1)
            {                                     // Pointer = -1
                doctor_name2.seekg(-2, ios::cur); // Go to that name
                doctor_name2.write((char *)&CntDoctorNameLL, sizeof(CntDoctorNameLL));
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_name, 30);
                doctor_name2.write(doctor_id, 15);
                short t = -1;
                doctor_name2.write((char *)&t, sizeof(t));
                break;
            }
            else
            {
                doctor_name2.seekg((next * 47) + 45, ios::beg); // Simply go to the next occurrence of the record
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
        DoctorNameSec.read((char *)&temp, sizeof(temp));

        if (strcmp(temp, name) == 0)
            found = true;
        else if (strcmp(temp, name) == 1)
            last = mid - 1;
        else
            first = mid + 1;
    }
    DoctorNameSec.close();
    if (!found)
        return;

    DoctorNameSec.open("SecondaryIndexForDoctorName.txt", ios::binary | ios::in | ios::out); // open namesec 5

    DoctorNameSec.seekg(((CntDoctorNameSec - 1) * 32), ios::beg);
    DoctorNameSec.seekg((mid + 32), ios::beg);

    int i = mid / 32;
    while (i < CntDoctorNameSec - 1) /// start to shift
    {
        char tempname[30];
        short tempof;
        DoctorNameSec.read(tempname, 30);
        DoctorNameSec.read((char *)&tempof, sizeof(tempof));

        DoctorNameSec.seekg(-64, ios::cur);
        DoctorNameSec.write(tempname, 30);
        DoctorNameSec.write((char *)&tempof, sizeof(tempof));
        DoctorNameSec.seekg(32, ios::cur);
        i++;
    }
    CntDoctorNameSec--;
    DoctorNameSec.close();
}

void printAppointmentByID(const string &searchID)
{
    ifstream appointmentFile("..\\data\\Appointment_DataFile.txt");

    if (!appointmentFile.is_open())
    {
        cout << "Error: Could not open Appointment Data File.\n";
        return;
    }

    string lengthIndicator, appointmentID, appointmentDate, DoctorID;
    bool found = false;
    while (getline(appointmentFile, lengthIndicator, '|') &&
           getline(appointmentFile, appointmentID, '|') &&
           getline(appointmentFile, appointmentDate, '|') &&
           getline(appointmentFile, DoctorID))
    {
        if (appointmentID == searchID)
        {
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
    if (!found)
    {
        cout << "Doctor with ID " << searchID << " not found.\n";
    }

    appointmentFile.close();
}

void printDoctorByID(const string &searchID)
{
    ifstream doctorFile("..\\data\\Doctor_DataFile.txt");

    if (!doctorFile.is_open())
    {
        cout << "Error: Could not open Doctor Data File.\n";
        return;
    }

    string lengthIndicator, doctorID, doctorName, Address;
    bool found = false;

    while (getline(doctorFile, lengthIndicator, '|') &&
           getline(doctorFile, doctorID, '|') &&
           getline(doctorFile, doctorName, '|') &&
           getline(doctorFile, Address))
    {
        if (doctorID == searchID)
        {
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

    if (!found)
    {
        cout << "Doctor with ID " << searchID << " not found.\n";
    }

    doctorFile.close();
}

// Initializing file paths (primary index file and data file)
const string DOCTORS_FILE = "..\\data\\Doctor_DataFile.txt";
const string PRIMARY_INDEX_FILE = "..\\indexes\\primary\\PrimaryIndexForDoctorID.txt";

// map to store primary index file when loading
map<string, streampos> primaryIndex;

// loads primary index file into map
void loadPrimaryIndex()
{
    primaryIndex.clear();
    ifstream indexFile(PRIMARY_INDEX_FILE);
    if (indexFile.is_open())
    {
        string doctorID;
        std::streamoff offset;
        while (indexFile >> doctorID >> offset)
        {
            primaryIndex[doctorID] = offset;
        }
        indexFile.close();
    }
}

// Strips strings from whitespaces
string trim(const string &str)
{
    size_t first = str.find_first_not_of(' ');
    size_t last = str.find_last_not_of(' ');
    if (first == string::npos || last == string::npos)
    {
        return "";
    }
    else
    {
        return str.substr(first, last - first + 1);
    }
}

// decomposes the "where" clause into: field, operator, value
pair<string, string> parseConditions(const string &whereClause)
{
    string field, value;
    pair<string, string> condition;

    field = whereClause.substr(0, whereClause.find('='));
    value = whereClause.substr(whereClause.find('\''), whereClause.length() - whereClause.find('\''));
    value = value.substr(1, value.length() - 2);
    condition = make_pair(field, value);

    return condition;
}

// matching records satisfying "where" clause
bool matchConditions(const string &record, const pair<string, string> &condition)
{
    string doctorID = trim(record.substr(0, 15));
    string doctorName = trim(record.substr(15, 30));
    string address = trim(record.substr(45, 30));
    if (condition.first == "Doctor ID" && doctorID != condition.second)
        return false;
    if (condition.first == "Doctor Name" && doctorName != condition.second)
        return false;
    if (condition.first == "Address" && address != condition.second)
        return false;
    return true;
}

vector<string> split(const string &fields, char delimiter)
{
    vector<string> tokens;
    size_t start = 0, end;

    while ((end = fields.find(delimiter, start)) != string::npos)
    {
        tokens.push_back(trim(fields.substr(start, end - start)));
        start = end + 1;
    }

    tokens.push_back(trim(fields.substr(start)));

    return tokens;
}

void executeQuery(const string &query)
{
    vector<string> fieldsVector;
    string select, fields, from, table, where, whereClause;

    select = query.substr(0, 6);
    fields = query.substr(7, query.find(" FROM ") - 7);
    fieldsVector = split(fields, ',');

    from = trim(query.substr(query.find(" FROM "), 5));
    table = trim(query.substr(query.find(" FROM ") + 6, query.find(" WHERE ") - (query.find(" FROM ") + 6)));
    where = trim(query.substr(query.find(" WHERE "), 6));

    whereClause = trim(query.substr(query.find(" WHERE ") + 7, query.find(';') - (query.find(" WHERE ") + 7)));

    if (select != "SELECT" || from != "FROM" || where != "WHERE" || table != "Doctors")
    {
        cout << "Invalid query syntax.\n";
        return;
    }

    pair<string, string> condition = parseConditions(whereClause);

    ifstream doctorsFile(DOCTORS_FILE);
    if (doctorsFile.is_open())
    {
        string record;
        vector<string> results;

        while (getline(doctorsFile, record))
        {
            if (record[0] != '*' && matchConditions(record, condition))
            {
                if (fields == "ALL")
                {
                    results.push_back(record);
                }
                else
                {
                    stringstream result;
                    if (fields.find("Doctor ID") != string::npos)
                    {
                        string doctorID = trim(record.substr(0, 15));
                        result << doctorID << " ";
                    }
                    if (fields.find("Doctor Name") != string::npos)
                    {
                        string doctorName = trim(record.substr(15, 30));
                        result << doctorName << " ";
                    }
                    if (fields.find("Address") != string::npos)
                    {
                        string address = trim(record.substr(45, 30));
                        result << address << " ";
                    }

                    results.push_back(result.str());
                }
            }
        }
        doctorsFile.close();

        if (results.empty())
        {
            cout << "No matching records found.\n";
        }
        else
        {
            for (const string &res : results)
            {
                cout << res << "\n";
            }
        }
    }
    else
    {
        cout << "Error: Could not open the doctors file.\n";
    }
}

// -------------
// Add doctor
int saveDoctorAtPosition(const Doctor &doctor, fstream &DoctorDataFile, long pos)
{
    string record = string(doctor.getID()) + "|" + doctor.getName() + "|" + doctor.getAddress();
    size_t length = record.size() + 1; // Include '|'

    DoctorDataFile.clear();
    DoctorDataFile.seekp(pos, ios::beg);
    if (DoctorDataFile)
    {
        DoctorDataFile << length << "|" << record << "\n";
        return 1;
    }
    return 0;
}

int saveDoctor(const Doctor &doctor, fstream &DoctorDataFile)
{
    DoctorDataFile.clear();
    DoctorDataFile.seekp(0, ios::end);
    if (DoctorDataFile)
    {
        string record = string(doctor.getID()) + "|" + doctor.getName() + "|" + doctor.getAddress();
        size_t length = record.size() + 1; // Include '|'
        DoctorDataFile << length << "|" << record << "\n";
        return 1;
    }
    return 0;
}

bool isDoctorExists(const char *id, fstream &DoctorDataFile)
{
    DoctorDataFile.clear();
    DoctorDataFile.seekg(0, ios::beg);

    string line;
    while (getline(DoctorDataFile, line))
    {
        if (line[0] == '*')
            continue;
        size_t pos = line.find('|');
        if (pos != string::npos)
        {
            string doctorID = line.substr(pos + 1, strlen(id));
            if (doctorID == id)
            {
                return true;
            }
        }
    }
    return false;
}

int addDoctor(const char *id, const char *name, const char *address, fstream &DoctorDataFile)
{
    if (isDoctorExists(id, DoctorDataFile))
    {
        cout << "Error: Doctor with this ID already exists.\n";
        return 0;
    }
    const Doctor doctor(id, name, address);

    // Use available space from the Avail List if possible
    if (!doctorAvailList.empty())
    {
        long pos = doctorAvailList.back();
        doctorAvailList.pop_back();
        return saveDoctorAtPosition(doctor, DoctorDataFile, pos);
    }
    else
    {
        return saveDoctor(doctor, DoctorDataFile);
    }
}

bool deleteDoctor(const char *id, fstream &DoctorDataFile)
{
    DoctorDataFile.clear();
    DoctorDataFile.seekg(0, ios::beg);
    string line;
    vector<string> lines;
    bool found = false;

    while (getline(DoctorDataFile, line))
    {
        size_t firstDelimiter = line.find('|');
        if (firstDelimiter != string::npos)
        {
            size_t secondDelimiter = line.find('|', firstDelimiter + 1);
            if (secondDelimiter != string::npos)
            {
                string doctorID = line.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);
                if (doctorID == id)
                {
                    // Add '*' to the line and mark it
                    line = "*" + line;
                    found = true;
                }
            }
        }
        lines.push_back(line);
    }

    if (found)
    {
        DoctorDataFile.clear();
        DoctorDataFile.seekp(0, ios::beg);

        for (const auto &record : lines)
        {
            DoctorDataFile << record << '\n';
        }

        DoctorDataFile.flush();
        cout << "Doctor record marked as deleted successfully.\n";
        return true;
    }

    cout << "Error: Doctor record not found.\n";
    return false;
}

bool updateDoctorName(const char *id, const char *newName, fstream &DoctorDataFile)
{
    DoctorDataFile.clear();
    DoctorDataFile.seekg(0, ios::beg);

    string line;
    vector<string> lines;
    bool updated = false;

    while (getline(DoctorDataFile, line))
    {
        if (!line.empty() && line[0] == '*')
        {
            lines.push_back(line);
            continue;
        }

        size_t firstDelimiter = line.find('|');
        if (firstDelimiter != string::npos)
        {
            size_t secondDelimiter = line.find('|', firstDelimiter + 1);
            if (secondDelimiter != string::npos)
            {
                string doctorID = line.substr(firstDelimiter + 1, secondDelimiter - firstDelimiter - 1);
                if (doctorID == id)
                {
                    size_t nameStart = secondDelimiter + 1;
                    size_t nameEnd = line.find('|', nameStart);

                    if (nameEnd != string::npos)
                    {
                        line = line.substr(0, nameStart) + newName + line.substr(nameEnd);
                        updated = true;
                    }
                }
            }
        }
        lines.push_back(line);
    }

    if (updated)
    {
        DoctorDataFile.clear();
        DoctorDataFile.seekp(0, ios::beg);

        for (const auto &record : lines)
        {
            DoctorDataFile << record << '\n';
        }

        DoctorDataFile.flush();
        cout << "Doctor name updated successfully.\n";
        return true;
    }

    cout << "Error: Doctor not found.\n";
    return false;
}

// ------------ END CRUD for Doctor

void InsertByPrimaryIndexAppointment(const char id[], short offset)
{
    // Convert the Appointment ID into an integer for easy comparison
    int ID_to_be_inserted = 0;
    for (int i = 0; id[i] != '\0'; i++)
    {
        ID_to_be_inserted = ID_to_be_inserted * 10 + (id[i] - '0');
    }

    fstream primaryAppointment("PrimaryIndexForAppointmentID.txt", ios::app | ios::in | ios::out | ios::binary);
    if (!primaryAppointment)
    {
        cerr << "Error: Could not open PrimaryIndexForAppointmentID.txt\n";
        return;
    }

    int temp = 0;
    short of = 0;
    bool check = false;

    if (CntAppointmentID == 0) // First record to be inserted
    {
        primaryAppointment.seekp(0, ios::beg); // Go to the start of the file
        primaryAppointment.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryAppointment.write((char *)&offset, sizeof(short));
        primaryAppointmentRecords.push_back({ID_to_be_inserted, offset}); // Add to in-memory records
        CntAppointmentID++;
        primaryAppointment.close();
        savePrimaryIndex(); // Save to file after insertion
        return;
    }

    // Read the existing records to find the insertion point
    primaryAppointment.seekg(0, ios::beg);
    primaryAppointment.read((char *)&temp, sizeof(temp));

    while (primaryAppointment.good())
    {
        if (temp > ID_to_be_inserted)
        {
            check = true;
            primaryAppointment.seekg(-4, ios::cur); // Save the place(offset) where the ID should be inserted
            of = primaryAppointment.tellg();
            break;
        }
        primaryAppointment.seekg(2, ios::cur); // Skip the current offset and move to the next ID
        primaryAppointment.read((char *)&temp, sizeof(temp));
    }
    primaryAppointment.close();

    // Reopen the file to insert the record
    primaryAppointment.open("PrimaryIndexForAppointmentID.txt", ios::in | ios::out | ios::binary);

    if (!check) // If the ID is larger than the last record
    {
        primaryAppointment.seekg(CntAppointmentID * 6, ios::beg);
        primaryAppointment.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryAppointment.write((char *)&offset, sizeof(short));
        primaryAppointmentRecords.push_back({ID_to_be_inserted, offset}); // Add to in-memory records
        CntAppointmentID++;
    }
    else
    {
        // Shift the records to make room for the new ID
        primaryAppointment.seekg((CntAppointmentID - 1) * 6, ios::beg);
        int numend;
        short ofend;
        primaryAppointment.read((char *)&numend, sizeof(numend));
        primaryAppointment.read((char *)&ofend, sizeof(ofend));

        primaryAppointment.seekg(of, ios::beg); // Go to the place where the new ID will be inserted

        while (primaryAppointment.good())
        {
            int tmpnum;
            short tmpof;
            int tmpnum1;
            short tmpof1;
            primaryAppointment.read((char *)&tmpnum, sizeof(tmpnum));
            primaryAppointment.read((char *)&tmpof, sizeof(tmpof));
            primaryAppointment.read((char *)&tmpnum1, sizeof(tmpnum1));
            primaryAppointment.read((char *)&tmpof1, sizeof(tmpof1));

            primaryAppointment.seekg(-6, ios::cur);
            primaryAppointment.write((char *)&tmpnum, sizeof(tmpnum));
            primaryAppointment.write((char *)&tmpof, sizeof(tmpof));
        }
        primaryAppointment.close();
        primaryAppointment.open("PrimaryIndexForAppointmentID.txt", ios::in | ios::out | ios::binary);
        primaryAppointment.seekg(0, ios::end);
        primaryAppointment.write((char *)&numend, sizeof(numend)); // Write the last record that we stored
        primaryAppointment.write((char *)&ofend, sizeof(ofend));

        primaryAppointment.seekg(of, ios::beg);
        primaryAppointment.write((char *)&ID_to_be_inserted, sizeof(int));
        primaryAppointment.write((char *)&offset, sizeof(short));
        primaryAppointmentRecords.push_back({ID_to_be_inserted, offset}); // Add to in-memory records
        CntAppointmentID++;
    }

    primaryAppointment.close(); // Close the file after insertion
    savePrimaryIndex();         // Save the index after insertion
}

short BinarySearchAppointmentID(int id)
{
    fstream primaryAppointment("PrimaryIndexForAppointmentID.txt", ios::app | ios::in | ios::binary);
    if (!primaryAppointment)
    {
        cerr << "Error: Could not open PrimaryIndexForAppointmentID.txt\n";
        return -1;
    }

    short start = 0;
    short end = CntAppointmentID - 1;
    int mid;
    int tempID;
    short offset;
    bool found = false;

    while (start <= end)
    {
        mid = (start + end) / 2;
        primaryAppointment.seekg(mid * (sizeof(int) + sizeof(short)), ios::beg);
        primaryAppointment.read((char *)&tempID, sizeof(tempID));
        primaryAppointment.read((char *)&offset, sizeof(offset));

        if (tempID == id)
        {
            primaryAppointment.close();
            return offset;
        }
        else if (tempID > id)
        {
            end = mid - 1;
        }
        else
        {
            start = mid + 1;
        }
    }

    primaryAppointment.close();
    return -1; // Return -1 if not found
}

void DeletePrimaryAppointment(int id)
{
    fstream primaryAppointment("PrimaryIndexForAppointmentID.txt", ios::in | ios::out | ios::binary);
    short start = 0;
    short end = CntAppointmentID - 1;
    bool found = false;
    short mid;
    int temp;
    while (start <= end && !found)
    {
        mid = (start + end) / 2;
        primaryAppointment.seekg(mid * 6, ios::beg);
        primaryAppointment.read((char *)&temp, sizeof(temp)); /// Take the id of middle,then compare

        if (temp == id)
        {
            found = true;
        }
        else if (temp > id)
        {
            end = mid - 1; /// First half
        }
        else
        {
            start = mid + 1; /// Second Half
        }
    }

    if (found)
    {
        /// Go to offset of found record.
        /// Suppose numbers 1 2 3 and you want to delete 2.
        cout << "dELETED";
        primaryAppointment.seekg((mid + 1) * 6, ios::beg); /// Go to 3

        while (primaryAppointment.good())
        { /// Start to shift
            /// 1 2 3 ---> 1 3 3 ---> 1 3
            int tmpnum;
            short tmpof;
            primaryAppointment.read((char *)&tmpnum, sizeof(tmpnum));
            primaryAppointment.read((char *)&tmpof, sizeof(tmpof));

            primaryAppointment.seekg(-12, ios::cur); /// Go back two records back to 2.
            primaryAppointment.write((char *)&tmpnum, sizeof(tmpnum));
            primaryAppointment.write((char *)&tmpof, sizeof(tmpof)); /// make 2 = 3--> 1 3 3
            primaryAppointment.seekg(6, ios::cur);                   /// go to the next number.
        }
        primaryAppointment.close();
        CntAppointmentID--;
    }
    else
    {
        cout << "Cannot Find Record You are looking for." << endl;
    }
}

void insertbydoctor_id_in_appointments(const char doctor_id[15], const char appointment_id[15])
{

    fstream DoctorNameSec("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
    short first = 0;
    short last = CntDoctorIDSec - 1;
    short mid;
    bool found = false;
    char temp[15];

    // Binary search to find if the doctor_name already exists
    while (first <= last && !found)
    {
        mid = (first + last) / 2;
        DoctorNameSec.seekg(mid * 17, ios::beg);
        DoctorNameSec.read((char *)&temp, sizeof(temp));

        if (strcmp(temp, doctor_id) == 0)
            found = true;
        else if (strcmp(temp, doctor_id) == 1)
            last = mid - 1;
        else
            first = mid + 1;
    }

    DoctorNameSec.close();

    if (!found)
    {
        if (CntDoctorIDLL == 0)
        { // Empty
            // Open in append mode if file is empty
            DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out | ios::app);
            DoctorNameSec.seekg(0, ios::end);
            DoctorNameSec.write(doctor_id, 15);
            DoctorNameSec.write((char *)&CntDoctorIDLL, sizeof(CntDoctorIDLL)); // Initial count
            CntDoctorIDLL++;
            DoctorNameSec.close();

            // Write to linked list file
            fstream doc_name2("LLIndexForDoctorID.txt", ios::binary | ios::in | ios::out | ios::app);
            doc_name2.seekg(0, ios::end);
            doc_name2.write(doctor_id, 15);
            doc_name2.write(appointment_id, 15);
            short nega = -1;
            doc_name2.write((char *)&nega, sizeof(nega));
            doc_name2.close();

            CntDoctorIDLL++;
        }
        else
        {
            // Start shifting logic
            short offplace = -1;
            DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
            int i = 0;
            DoctorNameSec.seekg(0, ios::beg);

            while (i < CntDoctorIDSec)
            {
                char id[15];
                DoctorNameSec.read(id, sizeof(id));
                if (strcmp(doctor_id, id) == -1)
                {
                    offplace = DoctorNameSec.tellg();
                    offplace -= 15; // Adjust for id size
                    break;
                }
                else
                {
                    DoctorNameSec.seekg(2, ios::cur); /// mkkkkk kanet 32 Move to next name record (30 + 2 for short)
                }
                i++;
            }
            DoctorNameSec.close();

            if (offplace == -1)
            { // If it is the biggest one
                DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out | ios::app);
                DoctorNameSec.seekg(CntDoctorIDSec * 17, ios::beg);
                DoctorNameSec.write(doctor_id, 30);
                DoctorNameSec.write((char *)&CntDoctorIDLL, sizeof(CntDoctorIDLL));
                CntDoctorIDSec++;
                CntDoctorIDLL++;
                DoctorNameSec.close();

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorID.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_id, 15);
                doctor_name2.write(appointment_id, 15);
                short t = -1;
                doctor_name2.write((char *)&t, sizeof(t));
                doctor_name2.close();
            }
            else
            {
                // Insert somewhere in the middle
                DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
                char idtmpend[15];
                short offsetnameend;
                DoctorNameSec.seekg((CntDoctorIDSec - 1) * 17, ios::beg);
                DoctorNameSec.read(idtmpend, 15);
                DoctorNameSec.read((char *)&offsetnameend, sizeof(offsetnameend));

                char tmp1[15], tmp2[15];
                short oftmp1, oftmp2;
                DoctorNameSec.seekg(offplace, ios::beg);
                DoctorNameSec.read(tmp1, 15);
                DoctorNameSec.read((char *)&oftmp1, sizeof(oftmp1));

                int i = 0;
                while (i < CntDoctorIDSec - 1)
                {
                    DoctorNameSec.read(tmp2, 15);
                    DoctorNameSec.read((char *)&oftmp2, sizeof(oftmp2));
                    DoctorNameSec.seekp(-17, ios::cur); // Go back to overwrite
                    DoctorNameSec.write(tmp1, 15);
                    DoctorNameSec.write((char *)&oftmp1, sizeof(oftmp1));
                    oftmp1 = oftmp2;
                    int j;
                    for (j = 0; tmp2[j] != '\0'; j++)
                        tmp1[j] = tmp2[j];
                    tmp1[j] = '\0';
                    i++;
                }
                DoctorNameSec.close();

                DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
                DoctorNameSec.seekp(offplace, ios::beg);
                DoctorNameSec.write(doctor_id, 15);
                DoctorNameSec.write((char *)&CntDoctorIDLL, sizeof(CntDoctorIDLL));

                DoctorNameSec.seekp(CntDoctorIDSec * 17, ios::beg);
                DoctorNameSec.write(idtmpend, 15);
                DoctorNameSec.write((char *)&offsetnameend, sizeof(offsetnameend));

                DoctorNameSec.close();
                CntDoctorIDSec++;

                // Write to linked list file
                fstream doctor_name2("LLIndexForDoctorID.txt", ios::binary | ios::in | ios::out | ios::app);
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_id, 15);
                doctor_name2.write(appointment_id, 15);
                short nega = -1;
                doctor_name2.write((char *)&nega, sizeof(nega));
                doctor_name2.close();
                CntDoctorIDLL++;
            }
        }
    }
    else
    { // if name is already there
        short off;
        DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
        DoctorNameSec.seekg((mid * 17) + 15, ios::beg); // Go to offset of found record
        DoctorNameSec.read((char *)&off, sizeof(off));  // Store the offset
        DoctorNameSec.close();

        fstream doctor_name2("LLIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
        doctor_name2.seekg((mid * 32), ios::beg); // Offset of found record
        short next = -2;
        while (1)
        {
            doctor_name2.read((char *)&next, sizeof(next)); // Get hold of pointer to check if there is another occurrence
            if (next == -1)
            {                                     // Pointer = -1
                doctor_name2.seekg(-2, ios::cur); // Go to that name
                doctor_name2.write((char *)&CntDoctorIDLL, sizeof(CntDoctorIDLL));
                doctor_name2.seekg(0, ios::end);
                doctor_name2.write(doctor_id, 15);
                doctor_name2.write(appointment_id, 15);
                short t = -1;
                doctor_name2.write((char *)&t, sizeof(t));
                break;
            }
            else
            {
                doctor_name2.seekg((next * 32) + 30, ios::beg); // Simply go to the next occurrence of the record
            }
        }

        CntDoctorIDLL++;
        doctor_name2.close();
    }
}

void DeleteSecondaryDoctorID(const char id[15])
{
    fstream DoctorNameSec("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out);
    short first = 0;
    short last = CntDoctorIDSec - 1;
    short mid;
    bool found = false;
    char temp[15];
    while (first <= last && !found) /// search for it
    {
        mid = (first + last) / 2;
        DoctorNameSec.seekg(mid * 17, ios::beg);
        DoctorNameSec.read((char *)&temp, sizeof(temp));

        if (strcmp(temp, id) == 0)
            found = true;
        else if (strcmp(temp, id) == 1)
            last = mid - 1;
        else
            first = mid + 1;
    }
    DoctorNameSec.close();
    if (!found)
        return;

    DoctorNameSec.open("SecondaryIndexForDoctorID.txt", ios::binary | ios::in | ios::out); // open namesec 5

    DoctorNameSec.seekg(((CntDoctorIDSec - 1) * 17), ios::beg);
    DoctorNameSec.seekg((mid + 17), ios::beg);

    int i = mid / 17;
    while (i < CntDoctorIDSec - 1) /// start to shift
    {
        char tempid[15];
        short tempof;
        DoctorNameSec.read(tempid, 15);
        DoctorNameSec.read((char *)&tempof, sizeof(tempof));

        DoctorNameSec.seekg(-34, ios::cur);
        DoctorNameSec.write(tempid, 15);
        DoctorNameSec.write((char *)&tempof, sizeof(tempof));
        DoctorNameSec.seekg(17, ios::cur);
        i++;
    }
    CntDoctorIDSec--;
    DoctorNameSec.close();
}

int main()
{
    loadPrimaryIndexForAppointment();
    loadPrimaryIndex();
    int option;
    bool flag = true;
    /// Opening all data files.
    fstream DoctorDataFile, AppointmentDataFile, PrimaryForDoctorID, PrimaryForAppointmentID, SecondaryForDoctorID, SecondaryForDoctorName;
    DoctorDataFile.open("..\\data\\Doctor_DataFile.txt", ios::in | ios::out);
    AppointmentDataFile.open("..\\data\\Appointment_DataFile.txt", ios::out | ios::in | ios::app);
    PrimaryForDoctorID.open("..\\indexes\\primary\\PrimaryIndexForDoctorID.txt", ios::out | ios::in | ios::app);
    PrimaryForAppointmentID.open("..\\indexes\\primary\\PrimaryIndexForAppointmentID.txt", ios::out | ios::in | ios::app);
    SecondaryForDoctorID.open("..\\indexes\\secondary\\SecondaryIndexForDoctorID.txt", ios::out | ios::in | ios::app);
    SecondaryForDoctorName.open("..\\indexes\\secondary\\SecondaryIndexForDoctorName.txt", ios::out | ios::in | ios::app);
    ///-------------------------------------------------------------------------------------------------------
    if (!DoctorDataFile)
    {
        cout << "Error open doctor file!\n";
        return -1;
    }

    showWelcomeMessage();
    string doctorId;
    string query1 = "SELECT ALL FROM Doctors WHERE Doctor ID='D001';";
    string query2 = "SELECT Doctor Name FROM Doctors WHERE Address='123 Elm St';";
    string appointmentId;
    while (flag)
    {
        showMenu();
        cin >> option;
        switch (option)
        {
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
            if (addDoctor(id, name, address, DoctorDataFile))
            {
                cout << "Doctor added successfully.\n";
            }
            else
            {
                cout << "Unexpected error, please try again.\n";
            }
            break;
        case 2:
            cout << "Add New Appointment selected.\n";
            {
                string appointmentID, appointmentDate, doctorID;

                // Get input from the user
                cout << "Enter Appointment ID: ";
                cin >> appointmentID; // Get Appointment ID as a string

                cout << "Enter Appointment Date (YYYY-MM-DD): ";
                cin.ignore();
                getline(cin, appointmentDate); // Get Appointment Date as a string

                cout << "Enter Doctor ID: ";
                cin >> doctorID; // Get Doctor ID as a string

                // Convert string to const char* using c_str() and create an Appointment object
                Appointment newAppointment(appointmentID.c_str(), appointmentDate.c_str(), doctorID.c_str());

                if (Appointment::addAppointment(newAppointment, availList))
                {
                    cout << "Appointment added successfully.\n";
                }
                else
                {
                    cout << "Failed to add appointment. It may already exist.\n";
                }
            }
            break;
        case 3:
            cout << "Update Doctor Name selected.\n";
            cout << "Enter ID: ";
            cin.ignore();
            cin.getline(id, 15);
            cout << "Enter New Name: ";
            cin.getline(name, 30);
            if (updateDoctorName(id, name, DoctorDataFile))
            {
                cout << "Doctor name updated successfully.\n";
            }
            else
            {
                cout << "Doctor not found.\n";
            }
            break;
        case 4:
            cout << "Update Appointment Date selected.\n";
            {
                string appointmentID, newDate;
                cout << "Enter Appointment ID: ";
                cin >> appointmentID;
                cout << "Enter New Appointment Date: ";
                cin.ignore();
                getline(cin, newDate);

                if (Appointment::updateAppointment(appointmentID.c_str(), newDate.c_str()))
                {
                    cout << "Appointment date updated successfully.\n";
                }
                else
                {
                    cout << "Failed to update appointment. ID not found or an error occurred.\n";
                }
            }
            break;
        case 5:
            cout << "Delete Appointment selected.\n";
            {
                string appointmentID;
                cout << "Enter Appointment ID: ";
                cin >> appointmentID;

                if (Appointment::deleteAppointment(appointmentID.c_str(), availList))
                {
                    cout << "Appointment deleted successfully.\n";
                }
                else
                {
                    cout << "Failed to delete appointment. ID not found or an error occurred.\n";
                }
            }
            break;
        case 6:
            cout << "Delete Doctor selected.\n";
            cout << "Enter ID: ";
            cin.ignore();
            cin.getline(id, 15);
            if (deleteDoctor(id, DoctorDataFile))
            {
                cout << "Doctor deleted successfully.\n";
            }
            else
            {
                cout << "Doctor not found.\n";
            }
            break;
        case 7:
            cout << "Print Doctor Info selected.\n";
            cin >> doctorId;
            printDoctorByID(doctorId);
            break;
        case 8:
        {
            cout << "Print Appointment Info selected.\n";
            string appointmentID;
            cout << "Enter Appointment ID: ";
            cin >> appointmentID;
            // Print Appointment Info based on entered Appointment ID
            Appointment::printAppointmentInfoByID(appointmentID.c_str());
        }
        break;
        case 9:
            cout << "Write Query selected.\n";
            executeQuery(query1);
            executeQuery(query2);
            break;
        case 10:
            cout << "Exiting the program.\n";
            flag = false;
            DoctorDataFile.close();
            break;
        default:
            cout << "Invalid choice. Please try again.\n";
        }
    }
}