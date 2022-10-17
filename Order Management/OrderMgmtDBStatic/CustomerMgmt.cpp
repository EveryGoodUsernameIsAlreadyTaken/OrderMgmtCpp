#include <fstream>
#include <iostream>
#include <exception>

#include "CustomerMgmt.h"
#include "MyUtils.h"
#include <vector>

using namespace std;

CustomerMgmt::CustomerMgmt() {
	_CustNo = "";
	curCustomer = new Customer();
}

CustomerMgmt::CustomerMgmt(string custNo) {
	_CustNo = custNo;
	curCustomer = new Customer();
}

bool CustomerMgmt::_FindId(string custNo) {
	
	try {
		if (mssqldb::CheckID(custNo)) return true;
		else return false;
	}
	catch (...)
	{
		cout << "cannot find customer" << endl;
		return false;
	}
	return false;
}

string CustomerMgmt::partition(string& line) {
	string ret = line.substr(0, line.find(","));
	line = line.substr(line.find(",") + 1);

	return ret;
}

string CustomerMgmt::GetNewCustID()
{

	string szNewCustNo = mssqldb::NextCustNo();

	//szNewCustNo = to_string(stoi(szNewCustNo) + 1);
	while (szNewCustNo.size() < 5) szNewCustNo = "0" + szNewCustNo;

	return szNewCustNo;
}

Customer CustomerMgmt::AppendCustomerInfo() 
{
	Customer newCustInfo;
	_CustNo = curCustomer->_CustNo = GetNewCustID();
	newCustInfo._CustNo = _CustNo;
	getchar();
	Message::Out("Register page\nPlease enter your information\n\n", true);

	cout << "Type your first name: ";
	getline(cin, newCustInfo._FirstName);
	cout << endl;

	cout << "Type your last name: ";
	getline(cin, newCustInfo._LastName);
	cout << endl;

	cout << "Type your city: ";
	getline(cin, newCustInfo._City);
	cout << endl;

	cout << "Type your address: ";
	getline(cin, newCustInfo._Addr);
	cout << endl;

	cout << "Type your phone number: ";
	getline(cin, newCustInfo._PhoneNo);
	cout << endl;
	
	//
	string szSQL = "INSERT INTO CUSTOMERS VALUES( ";
	szSQL += "'" + newCustInfo._CustNo + "', ";
	szSQL += "'" + newCustInfo._FirstName + "', ";
	szSQL += "'" + newCustInfo._LastName + "', ";
	szSQL += "'" + newCustInfo._City + "', ";
	szSQL += "'" + newCustInfo._Addr + "', ";
	szSQL += "'" + newCustInfo._PhoneNo + "' )";
	mssqldb::ExecDBData(szSQL, INS);

	char buffer[100];
	sprintf_s(buffer, "\nRegister sucessfully.\nAppended Customer: %s \n\n", newCustInfo.FullName().c_str());
	Message::Out(buffer, true);

	return newCustInfo;
}

void CustomerMgmt::ModifyCustomerInfo(string szCustNo) 
{
	//Get Ord Data
	string szCustSQL = "SELECT * FROM CUSTOMERS WHERE CUSTNO ='" + szCustNo + "'";
	Customer curCustInfo = mssqldb::QueryCustomerInfo(szCustSQL);

	Message::Out("Modification page \nPlease enter your information\n\n", true);
	cout << "Current customer: " << curCustInfo._FirstName << " " << curCustInfo._LastName << endl;

	Customer* newCustInfo = new Customer();
	newCustInfo->_CustNo = curCustInfo._CustNo;
	
	char buffer[100];
	sprintf_s(buffer, "Change your first name (current - %s): ", curCustInfo._FirstName.c_str());
	Message::Out(buffer, false);
	getchar();

	getline(cin, newCustInfo->_FirstName);
	if (newCustInfo->_FirstName.empty()) newCustInfo->_FirstName = curCustInfo._FirstName;
	cout << endl;


	sprintf_s(buffer, "Change your last name (current - %s): ", curCustInfo._LastName.c_str());
	Message::Out(buffer, false);
	getline(cin, newCustInfo->_LastName);
	if (newCustInfo->_LastName.empty())  newCustInfo->_LastName = curCustInfo._LastName;

	cout << endl;

	sprintf_s(buffer, "Change your city (current - %s): ", curCustInfo._City.c_str());
	Message::Out(buffer, false);
	getline(cin, newCustInfo->_City);
	if (newCustInfo->_City.empty())  newCustInfo->_City = curCustInfo._City;

	cout << endl;

	sprintf_s(buffer, "Change your address (current - %s): ", curCustInfo._Addr.c_str());
	Message::Out(buffer, false);
	getline(cin, newCustInfo->_Addr);
	if (newCustInfo->_Addr.empty())  newCustInfo->_Addr = curCustInfo._Addr;

	cout << endl;

	sprintf_s(buffer, "Change your phone number (current - %s): ", curCustInfo._PhoneNo.c_str());
	Message::Out(buffer, false);
	getline(cin, newCustInfo->_PhoneNo);
	if (newCustInfo->_PhoneNo.empty())  newCustInfo->_PhoneNo = curCustInfo._PhoneNo;

	cout << endl;

	string szSQL = "UPDATE CUSTOMERS SET FIRSTNAME='"+newCustInfo->_FirstName+ "', ";
	szSQL += " LASTNAME = '"+newCustInfo->_LastName+ "', ";
	szSQL += " CITY = '" + newCustInfo->_City + "', ";
	szSQL += " ADDR = '" + newCustInfo->_Addr + "', ";
	szSQL += " PHONE = '" + newCustInfo->_PhoneNo + "' ";
	szSQL += " WHERE CUSTNO = '" + curCustInfo._CustNo + "' ";
	mssqldb::ExecDBData(szSQL, UPD);
}

void CustomerMgmt::DeleteCustomerInfo(string szCustNo) {
	Message::Out("Deletion page \n\n", true);
	std::cout << "Are you sure to delete current customer[" + szCustNo + "]\npress Y(y) to continue or press anything else to cancel:";

	string input;
	getchar();
	getline(cin, input);

	try {
		if (input == "y" || input == "Y")
		{
			string szSQL = "DELETE FROM CUSTOMERS WHERE CUSTNO = '" + szCustNo + "' ";
			mssqldb::ExecDBData(szSQL, DEL);
		}else {
			cout << "Deletion canceled.\n\n";
		}
	}
	catch (...)
	{
		cout << "An unexpected error occurred." << endl;
	}
}


void CustomerMgmt::ViewCustomerInfo(string szCustNo) {

	//Get Ord Data
	string szCustSQL = "neSELECT * FROM CUSTOMERS WHERE CUSTNO ='" + szCustNo + "'";
	Customer curCustInfo = mssqldb::QueryCustomerInfo(szCustSQL);
	Message::Out("Customer infomation page \nPlease enter your information\n\n", true);

	cout << "Your Info\n";
	cout << "ID: \t\t" << curCustInfo._CustNo << endl;
	cout << "First name: \t" << curCustInfo._FirstName << endl;
	cout << "Last name: \t" << curCustInfo._LastName << endl;
	cout << "City: \t\t" << curCustInfo._City << endl;
	cout << "Address: \t" << curCustInfo._Addr << endl;
	cout << "Phone Number: \t" << curCustInfo._PhoneNo << endl << endl;
}


void CustomerMgmt::ViewCustomerList(string szCustNo) {

	//Get Ord Data
	string szCustSQL = "SELECT * FROM CUSTOMERS";
	vector <Customer> vCustList = mssqldb::QueryCustomerList(szCustSQL);
	Message::Out("View All customer list \nPlease enter your information\n\n", true);

	// Print Title
	char buffer[1000];
	std::cout << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "CUSTNO", "FIRSTNAME", "LASTNAME", "CITY", "ADDRESS", "PHONE");
	std::cout << buffer << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "------", "---------", "--------", "----", "-------", "-----");
	std::cout << buffer << endl;

	string szCustomerNo;
	string szFirstName;
	string szLastName;
	string szCity;
	string szAddr;
	string szPhone;

	for(int i=0;i< vCustList.size(); i++)
	{
		szCustomerNo = vCustList[i]._CustNo;
		szFirstName = vCustList[i]._FirstName;
		szLastName = vCustList[i]._LastName;
		szCity = vCustList[i]._City;
		szAddr = vCustList[i]._Addr;
		szPhone = vCustList[i]._PhoneNo;

		// for Debug
		sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", szCustomerNo.c_str(), szFirstName.c_str(), szLastName.c_str(), szCity.c_str(), szAddr.c_str(), szPhone.c_str());
		std::cout << buffer << endl;
	}
	std::cout << endl;
}

string CustomerMgmt::GetCustFullName(string custNo) 
{
	try
	{
		string szSQL = "SELECT * FROM CUSTOMERS WHERE CUSTNO ='" + custNo + "'";

		Customer curCustomer = mssqldb::QueryCustomerInfo(szSQL);

		return curCustomer._FirstName + " " + curCustomer._LastName;
		//return "Peter Parkkkk";

	}
	catch (...) {
		cout << "file handling error" << endl;
	}
	return "";
}

Customer CustomerMgmt::GetCustInfo(string custNo)
{
	Customer curCustomer;
	try
	{
		string szSQL = "SELECT * FROM CUSTOMERS WHERE CUSTNO ='" + custNo + "'";

		curCustomer = mssqldb::QueryCustomerInfo(szSQL);

		return curCustomer;
		//return "Peter Parkkkk";

	}
	catch (...) {
		cout << "file handling error" << endl;
	}
	return curCustomer;
}




