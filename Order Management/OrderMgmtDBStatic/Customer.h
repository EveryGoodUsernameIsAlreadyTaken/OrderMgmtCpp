#pragma once
#include <string>

using namespace std;

class Customer
{
public:
	string _CustNo;
	string _FirstName;
	string _LastName;
	string _City;
	string _Addr;
	string _PhoneNo;

public:
	Customer()
	{

	}
	Customer(string custNo, string fname, string lname, string city, string addr, string phoneNo) {
		_CustNo = custNo;
		_FirstName = fname;
		_LastName = lname;
		_City = city;
		_Addr = addr;
		_PhoneNo = phoneNo;
	}

	string FullName() {
		return _FirstName + " " + _LastName;
	}

	//string getID();

	//string getID() {
	//	return _CustNo;
	//}

	//string getName() {
	//	return _Name;
	//}

	//string getCity() {
	//	return _City;
	//}

	//string getAdr() {
	//	return _Addr;
	//}

	//long getPhoneNo() {
	//	return _PhoneNo;
	//}


};