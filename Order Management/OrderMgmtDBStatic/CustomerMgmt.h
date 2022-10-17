#include <iostream>
#include <string>
#include <fstream>
#include "Customer.h"
#include "mssqldb.h"

class CustomerMgmt
{
private:
	string FileName = "CustomerInfo.txt";
	string _CustNo;
	Customer* curCustomer;

public:
	CustomerMgmt();

	CustomerMgmt(string custNo);

	bool _FindId(string custNo);


	string partition(string& line);
	
	string GetNewCustID();

	Customer AppendCustomerInfo();

	void ModifyCustomerInfo(string szCustNo);
	void DeleteCustomerInfo(string szCustNo);

	void ViewCustomerInfo(string szCustNo);
	void ViewCustomerList(string szCustNo);


	string GetCustFullName(string custNo);
	Customer GetCustInfo(string custNo);


};