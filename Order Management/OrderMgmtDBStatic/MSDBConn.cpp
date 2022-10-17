#include "stdafx.h"
#include "MSDBConn.h"
#include "Customer.h"

// define handles and variables
SQLHANDLE sqlConnHandle;
SQLHANDLE sqlStmtHandle;
SQLHANDLE sqlEnvHandle;
SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];

void MSDBConn::DisconnectDB()
{
	SQLFreeHandle(SQL_HANDLE_STMT, sqlStmtHandle);
	SQLDisconnect(sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_DBC, sqlConnHandle);
	SQLFreeHandle(SQL_HANDLE_ENV, sqlEnvHandle);
	sqlStmtHandle = NULL;
}

void MSDBConn::ConnectDB()
{
	SQLWCHAR connString[] = L"DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;";
	sqlConnHandle = NULL;
	sqlStmtHandle = NULL;

	//allocations
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &sqlEnvHandle))
		DisconnectDB();

	if (SQL_SUCCESS != SQLSetEnvAttr(sqlEnvHandle, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, 0))
		DisconnectDB();

	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_DBC, sqlEnvHandle, &sqlConnHandle))
		DisconnectDB();

	cout << "Attempting connection to SQL Server..." << endl;

	switch (SQLDriverConnect(sqlConnHandle,
		NULL,
		connString,
		//(SQLWCHAR*)L"DRIVER={SQL Server};SERVER=localhost, 1433; DATABASE=TN2Mgmt; UID=sa; PWD=mingunook2;",
		SQL_NTS,
		retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT))
	{
	case SQL_SUCCESS:
		std::cout << "successfully connected to SQL Server" << endl << endl; break;
	case SQL_SUCCESS_WITH_INFO:
		std::cout << "Successfully connected to SQL Server" << endl << endl; break;
	case SQL_INVALID_HANDLE:
		std::cout << "Cound not connect to SQL Server" << endl << endl;
		DisconnectDB();
		break;
	case SQL_ERROR:
		std::cout << "Could not connect to SQL Server" << endl << endl;
		DisconnectDB();
		break;
	default:
		break;
	}

	// if there is a problem connecting then exit application
	if (SQL_SUCCESS != SQLAllocHandle(SQL_HANDLE_STMT, sqlConnHandle, &sqlStmtHandle))
		DisconnectDB();
}

SQLWCHAR* MSDBConn::GetWString(string szStr)
{
	int iLen = strlen(szStr.c_str()) + 1;
	SQLWCHAR* wStr = new SQLWCHAR[iLen];

	for (int i = 0; i < iLen; i++)
	{
		wStr[i] = szStr[i];
	}
	return wStr;
}

void MSDBConn::QueryData(string szSQL)
{
	ConnectDB();

	SQLWCHAR* szWSQL = GetWString(szSQL);

	//output
	std::cout << "\n";
	std::cout << "Executing T_SQL Query..." << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
	{
		std::cout << "Error querying SQL Server" << endl;
		//Disconnection();
	}
	else {
		//declare output variable and pointer
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;

		SQLCHAR sqlCustNo[10];
		SQLCHAR sqlFirstName[30];
		SQLCHAR sqlLastName[30];
		SQLCHAR sqlCity[30];
		SQLCHAR sqlAddress[30];
		SQLCHAR sqlPhone[13];

		// Print Title
		char buffer[1000];
		std::cout << endl;
		sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "CUSTNO", "FIRSTNAME", "LASTNAME", "CITY", "ADDRESS", "PHONE");
		std::cout << buffer << endl;
		sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "------", "---------", "--------", "----", "-------", "-----");
		std::cout << buffer << endl;

		while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);

			sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			std::cout << buffer << endl;

		}
	}
	DisconnectDB();
}

void MSDBConn::InsertData(string szSQL)
{
	ConnectDB();

	//output
	std::cout <<endl << "Inserting Data to DB..." << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	SQLWCHAR* szWSQL = GetWString(szSQL);
	RETCODE rc= SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg("[Insert Data]", sqlStmtHandle, SQL_HANDLE_STMT);
		//Disconnection();
	}
	else {
		std::cout << "inserted successfully" << endl;
	}
	DisconnectDB();
}

void MSDBConn::ExecDBData(string szSQL, DExecType DEType)
{
	ConnectDB();

	//output
	string arAlert[]{ "Creating table...", "Inserting data...", "Updating Data...", "Deleting Data..." };
	cout << arAlert[(int)DEType] << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	SQLWCHAR* szWSQL = GetWString(szSQL);
	RETCODE rc = SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg((char*)(arAlert[(int)DEType]).c_str(), sqlStmtHandle, SQL_HANDLE_STMT);
		//Disconnection();
	}
	else {
		string arAlert[]{ "Created table", "Inserted data", "Updated data", "Deleted data" };
		std::cout << arAlert[(int)DEType] << " successfully" << endl << endl;
	}
	DisconnectDB();
}

void MSDBConn::CreateTable(string szSQL)
{
	ConnectDB();

	//output
	std::cout << endl << "Creating Table to DB..." << endl;

	//if there is a problem executing the query then exit application
	//else display query result
	SQLWCHAR* szWSQL = GetWString(szSQL);
	RETCODE rc = SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg("[Create Table]", sqlStmtHandle, SQL_HANDLE_STMT);
		//Disconnection();
	}
	else {
		std::cout << "Created table successfully" << endl;
	}
	DisconnectDB();
}

void MSDBConn::ShowErrorMsg(char *fn, SQLHANDLE handle, SQLSMALLINT type)
{
	SQLSMALLINT i = 0;
	SQLINTEGER NativeError;
	SQLWCHAR SQLState[7];
	SQLWCHAR MessageText[256];
	SQLSMALLINT TextLength;
	SQLRETURN ret;

	fprintf(stderr, "\nThe driver reported the following error [%s]\n", fn);
	do
	{
		ret = SQLGetDiagRec(type, handle, ++i, SQLState, &NativeError,
			MessageText, sizeof(MessageText), &TextLength);
		if (SQL_SUCCEEDED(ret)) {
			printf_s("%s:%ld:%ld:%s\n\n", SQLState, (long)i, (long)NativeError, MessageText);
		}
	} while (ret == SQL_SUCCESS);
}

string MSDBConn::NextCustNo()
{
	ConnectDB();

	string szNewCustNo;
	//output
	std::cout << "\nExecuting T_SQL Query..." << endl;
	SQLINTEGER ptrSqlVersion;
	SQLCHAR sqlCustNo[6];

	//if there is a problem execuing the query then exit application
	//else display query result
	try {
		string szSQL = "SELECT CAST(MAX(CUSTNO) AS int) + 1 FROM CUSTOMERS";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
			if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
				szNewCustNo = (char*)sqlCustNo;
			}
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "No data" << endl;
	}
	DisconnectDB();
	return szNewCustNo;
}

bool MSDBConn::CheckID(string custNo) 
{
	ConnectDB();
	bool bRet = false;
	//output
	std::cout << "\nExecuting T_SQL Query..." << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	try {
		string szSQL = "SELECT CUSTNO FROM CUSTOMERS WHERE CUSTNO ='" + custNo + "'";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS) )
			if(SQLFetch(sqlStmtHandle) == SQL_SUCCESS) bRet = true;
		else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "cannot find customer" << endl;
	}
	DisconnectDB();
	return bRet;
}

Customer MSDBConn::QueryCustomerInfo(string szSQL)
{
	ConnectDB();

	Customer curCustInfo;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
		DisconnectDB();
	}
	else {
		//declare output variable and pointer
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;

		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlFirstName[30];
		SQLCHAR sqlLastName[30];
		SQLCHAR sqlCity[30];
		SQLCHAR sqlAddress[30];
		SQLCHAR sqlPhone[13];

		// Print Title
		//char buffer[1000];
		//std::cout << endl;
		//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "CUSTNO", "FIRSTNAME", "LASTNAME", "CITY", "ADDRESS", "PHONE");
		//std::cout << buffer << endl;
		//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "------", "---------", "--------", "----", "-------", "-----");
		//std::cout << buffer << endl;

		if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);

			curCustInfo._CustNo = ((char*)sqlCustNo);
			curCustInfo._FirstName = ((char*)sqlFirstName);
			curCustInfo._LastName = ((char*)sqlLastName);
			curCustInfo._City = ((char*)sqlCity);
			curCustInfo._Addr = ((char*)sqlAddress);
			curCustInfo._PhoneNo = ((char*)sqlPhone);

			// for Debug
			//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			//std::cout << buffer << endl;
			
			DisconnectDB();

			return curCustInfo;
		}
	}
	DisconnectDB();
}

vector<Customer> MSDBConn::QueryCustomerList(string szSQL)
{
	ConnectDB();

	vector <Customer> vCustList;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
		DisconnectDB();
	}
	else {
		//declare output variable and pointer
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;

		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlFirstName[30];
		SQLCHAR sqlLastName[30];
		SQLCHAR sqlCity[30];
		SQLCHAR sqlAddress[30];
		SQLCHAR sqlPhone[13];

		// Print Title
		//char buffer[1000];
		//std::cout << endl;
		//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "CUSTNO", "FIRSTNAME", "LASTNAME", "CITY", "ADDRESS", "PHONE");
		//std::cout << buffer << endl;
		//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "------", "---------", "--------", "----", "-------", "-----");
		//std::cout << buffer << endl;

		while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);
			Customer curCustInfo;
			curCustInfo._CustNo = ((char*)sqlCustNo);
			curCustInfo._FirstName = ((char*)sqlFirstName);
			curCustInfo._LastName = ((char*)sqlLastName);
			curCustInfo._City = ((char*)sqlCity);
			curCustInfo._Addr = ((char*)sqlAddress);
			curCustInfo._PhoneNo = ((char*)sqlPhone);
			vCustList.push_back(curCustInfo);
			// for Debug
			//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			//std::cout << buffer << endl;
		}
		DisconnectDB();

		return vCustList;

	}
}

string MSDBConn::NextOrderNo() {
	ConnectDB();

	string newOrderNo;

	cout << "\nExecuting T_SQL Query..." << endl;
	SQLINTEGER ptrSqlVersion;
	SQLCHAR sqlOrderNo[6];

	try {
		string szSQL = "SELECT CAST(MAX(ORDERNO) AS int) + 1 FROM ORDERHISTORY";
		SQLWCHAR* szWSQL = GetWString(szSQL);

		if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
			if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
			{
				SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
				newOrderNo = (char*)sqlOrderNo;
			}
			else cout << "No data" << endl;
	}
	catch (...) {
		cout << "No data" << endl;
	}

	DisconnectDB();

	return newOrderNo;
}

bool MSDBConn::CheckOrderId(string orderNo, string custNo) {
	ConnectDB();
	bool bRet = false;

	cout << "\nExecuting T_SQL Query..." << endl;

	try {
		string szSQL = "SELECT ORDERNO FROM ORDERHISTORY WHERE ORDERNO ='" + orderNo + "'";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
			if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) bRet = true;
			else cout << "No data" << endl;

		szSQL = "SELECT CUSTNO FROM ORDERHISTORY WHERE CUSTNO ='" + custNo + "'";
		szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
			if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS) bRet &= true;
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "cannot find order" << endl;
	}
	DisconnectDB();
	return bRet;
}

Order MSDBConn::QueryOrderInfo(string szSQL)
{
	ConnectDB();

	Order curOrderInfo;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
	{
		std::cout << "Error querying SQL Server" << endl;
		DisconnectDB();
	}
	else {
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;

		SQLCHAR sqlOrderNo[6];
		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlDate[11];
		SQLCHAR sqlProduct[16];
		SQLCHAR sqlPrice[11];
		SQLCHAR sqlQuantity[11];

		if (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 5, SQL_CHAR, sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 6, SQL_CHAR, sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			curOrderInfo.OrderNo = ((char*)sqlOrderNo);
			curOrderInfo.CustNo = ((char*)sqlCustNo);
			curOrderInfo.Date = ((char*)sqlDate);
			curOrderInfo.Product = ((char*)sqlProduct);
			curOrderInfo.Price = stod((char*)sqlPrice);
			curOrderInfo.Quantity = stoi((char*)sqlQuantity);

			// for Debug
			//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			//std::cout << buffer << endl;

			DisconnectDB();

			return curOrderInfo;
		}
	}
	DisconnectDB();
}

vector<Order> MSDBConn::QueryOrderList(string szSQL)
{
	ConnectDB();

	vector <Order> vOrderList;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	if (SQL_SUCCESS != SQLExecDirect(sqlStmtHandle, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
		DisconnectDB();
	}
	else {
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;

		SQLCHAR sqlOrderNo[6];
		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlDate[11];
		SQLCHAR sqlProduct[16];
		SQLCHAR sqlPrice[11];
		SQLCHAR sqlQuantity[11];

		while (SQLFetch(sqlStmtHandle) == SQL_SUCCESS)
		{
			SQLGetData(sqlStmtHandle, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 5, SQL_CHAR, sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(sqlStmtHandle, 6, SQL_CHAR, sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			Order curOrderInfo;
			curOrderInfo.OrderNo = ((char*)sqlOrderNo);
			curOrderInfo.CustNo = ((char*)sqlCustNo);
			curOrderInfo.Date = ((char*)sqlDate);
			curOrderInfo.Product = ((char*)sqlProduct);
			curOrderInfo.Price = stod((char*)sqlPrice);
			curOrderInfo.Quantity = stoi((char*)sqlQuantity);
			vOrderList.push_back(curOrderInfo);
		}
		DisconnectDB();

		return vOrderList;

	}
}