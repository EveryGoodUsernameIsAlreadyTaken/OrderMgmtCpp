#include "mssqldb.h"

// define handles and variables
//SQLHANDLE sqlConnHandle;
SQLHENV henv;
SQLHDBC hdbc1;
SQLHSTMT hstmt1;

SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
RETCODE retcode;

void mssqldb::ConnectDB()
{
	hstmt1 = NULL;
	hdbc1 = SQL_NULL_HDBC;

	cout << "Attempting connection to SQL Serer..." << endl;

	// Allocate the ODBC environment and save handle.  
	retcode = SQLAllocHandle(SQL_HANDLE_ENV, NULL, &henv);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLAllocHandle(Env) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	// Notify ODBC that this is an ODBC 3.0 app.  
	retcode = SQLSetEnvAttr(henv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER)SQL_OV_ODBC3, SQL_IS_INTEGER);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLSetEnvAttr(ODBC version) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	// Allocate ODBC connection handle and connect.  
	retcode = SQLAllocHandle(SQL_HANDLE_DBC, henv, &hdbc1);
	if ((retcode != SQL_SUCCESS_WITH_INFO) && (retcode != SQL_SUCCESS)) {
		printf("SQLAllocHandle(hdbc1) Failed\n\n");
		getchar();
		DisconnectDB();
	}

	SQLWCHAR retconstring[SQL_RETURN_CODE_LEN];
	// This sample use Integrated Security. Create the SQL Server DSN by using the Windows NT authentication.   
	//retcode = SQLConnect(hdbc1, L"DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;", SQL_NTS, (SQLWCHAR*)"", SQL_NTS, (SQLWCHAR*)"", SQL_NTS);
	retcode = SQLDriverConnect(hdbc1,
		NULL,
		L"DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;",
		SQL_NTS,
		retconstring, 1024, NULL, SQL_DRIVER_NOPROMPT);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLConnect() Failed\n\n");
		getchar();
		DisconnectDB();
	}

	//ConnectStetement();
}

void mssqldb::ConnectStatement()
{
	// Allocate statement handle.  
	retcode = SQLAllocHandle(SQL_HANDLE_STMT, hdbc1, &hstmt1);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLAllocHandle(hstmt1) Failed\n\n");
		getchar();
		DisconnectDB();
	}
}

void mssqldb::DisconnectDB()
{
	if (hstmt1 != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);

	if (hdbc1 != SQL_NULL_HDBC) {
		SQLDisconnect(hdbc1);
		SQLFreeHandle(SQL_HANDLE_DBC, hdbc1);
	}

	if (henv != SQL_NULL_HENV)
		SQLFreeHandle(SQL_HANDLE_ENV, henv);
}

void mssqldb::DisconnectStatement()
{
	if (hstmt1 != SQL_NULL_HSTMT)
		SQLFreeHandle(SQL_HANDLE_STMT, hstmt1);
}

SQLWCHAR* mssqldb::GetWString(string szStr)
{
	int iLen = strlen(szStr.c_str()) + 1;
	SQLWCHAR* wStr = new SQLWCHAR[iLen];

	for (int i = 0; i < iLen; i++)
	{
		wStr[i] = szStr[i];
	}
	return wStr;
}

void mssqldb::ShowErrorMsg(char *fn, SQLHANDLE handle, SQLSMALLINT type)
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

void mssqldb::ExecDBData(string szSQL, DExecType DEType)
{
	string arAlert[]{ "Creating table...", "Inserting data...", "Updating Data...", "Deleting Data..." };
	cout << arAlert[(int)DEType] << endl;

	//if there is a problem execuing the query then exit application
	//else display query result
	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	RETCODE rc = SQLExecDirect(hstmt1, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg((char*)(arAlert[(int)DEType]).c_str(), hstmt1, SQL_HANDLE_STMT);
		//Disconnection();
	}
	else {
		string arAlert[]{ "Created table", "Inserted data", "Updated data", "Deleted data" };
		std::cout << arAlert[(int)DEType] << " successfully" << endl << endl;
	}
	DisconnectStatement();
}

string mssqldb::NextCustNo()
{
	std::cout << "\nExecuting T_SQL Query..." << endl;
	SQLINTEGER ptrSqlVersion;
	SQLCHAR sqlCustNo[6];
	string szNewCustNo;

	try {
		string szSQL = "SELECT CAST(MAX(CUSTNO) AS int) + 1 FROM CUSTOMERS";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		ConnectStatement();
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS)
			{
				SQLGetData(hstmt1, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
				szNewCustNo = (char*)sqlCustNo;
			}
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "No data" << endl;
	}

	DisconnectStatement();

	return szNewCustNo;
}

bool mssqldb::CheckID(string custNo)
{
	std::cout << "\nExecuting T_SQL Query..." << endl;

	bool bRet = false;
	
	try {
		string szSQL = "SELECT * FROM CUSTOMERS WHERE CUSTNO ='" + custNo + "'";
		ConnectStatement();
		SQLWCHAR* szWSQL = GetWString(szSQL);
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS) bRet = true;
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "cannot find customer" << endl;
	}
	DisconnectStatement();

	return bRet;
}

Customer mssqldb::QueryCustomerInfo(string szSQL)
{
	Customer curCustInfo;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
		DisconnectStatement();
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

		if (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);

			curCustInfo._CustNo = ((char*)sqlCustNo);
			curCustInfo._FirstName = ((char*)sqlFirstName);
			curCustInfo._LastName = ((char*)sqlLastName);
			curCustInfo._City = ((char*)sqlCity);
			curCustInfo._Addr = ((char*)sqlAddress);
			curCustInfo._PhoneNo = ((char*)sqlPhone);

			// for Debug
			//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			//std::cout << buffer << endl;
		}
	}
	DisconnectStatement();
	return curCustInfo;
}

vector<Customer> mssqldb::QueryCustomerList(string szSQL)
{
	vector <Customer> vCustList;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
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

		while (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);
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
	}

	DisconnectStatement();
	return vCustList;
}

//-- ORDER HISTORY
int mssqldb::NextOrderNo() 
{
	int newOrderNo;

	cout << "\nExecuting T_SQL Query..." << endl;
	SQLINTEGER ptrSqlVersion;
	SQLCHAR sqlOrderNo[6];

	try {
		string szSQL = "SELECT CASE WHEN COUNT(ORDERNO) = 0 THEN  1 ELSE MAX(ORDERNO) + 1 END ORDERNO FROM ORDERHISTORY";
		SQLWCHAR* szWSQL = GetWString(szSQL);

		ConnectStatement(); 
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS)
			{
				SQLGetData(hstmt1, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
				newOrderNo = stoi((char*)sqlOrderNo);
			}
			else cout << "No data" << endl;
	}
	catch (...) {
		cout << "No data" << endl;
	}

	DisconnectStatement();

	return newOrderNo;
}

bool mssqldb::CheckOrderId(string orderNo, string custNo) 
{
	cout << "\nExecuting T_SQL Query..." << endl;

	bool bRet = false;

	try {
		string szSQL = "SELECT ORDERNO FROM ORDERHISTORY WHERE ORDERNO ='" + orderNo + "' AND CUSTNO = '" + custNo + "'";
		SQLWCHAR* szWSQL = GetWString(szSQL);
		ConnectStatement();
		if (SQL_SUCCESS == SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
			if (SQLFetch(hstmt1) == SQL_SUCCESS) bRet = true;
			else cout << "No data" << endl;
	}
	catch (...)
	{
		cout << "cannot find order" << endl;
	}
	DisconnectStatement();
	return bRet;
}

Order mssqldb::QueryOrderInfo(string szSQL)
{
	Order curOrderInfo;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
	{
		std::cout << "Error querying SQL Server" << endl;
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

		if (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_CHAR, sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			curOrderInfo.OrderNo = stoi((char*)sqlOrderNo);
			curOrderInfo.CustNo = ((char*)sqlCustNo);
			curOrderInfo.Date = ((char*)sqlDate);
			curOrderInfo.Product = ((char*)sqlProduct);
			curOrderInfo.Price = stod((char*)sqlPrice);
			curOrderInfo.Quantity = stoi((char*)sqlQuantity);

			// for Debug
			//sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			//std::cout << buffer << endl;
		}
	}
	DisconnectStatement();
	return curOrderInfo;
}

vector<Order> mssqldb::QueryOrderList(string szSQL )
{
	vector <Order> vOrderList;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
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

		while (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_CHAR, sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			Order curOrderInfo;
			curOrderInfo.OrderNo = stoi((char*)sqlOrderNo);
			curOrderInfo.CustNo = ((char*)sqlCustNo);
			curOrderInfo.Date = ((char*)sqlDate);
			curOrderInfo.Product = ((char*)sqlProduct);
			curOrderInfo.Price = stod((char*)sqlPrice);
			curOrderInfo.Quantity = stoi((char*)sqlQuantity);
			vOrderList.push_back(curOrderInfo);
		}
	}

	DisconnectStatement();
	return vOrderList;
}

//-- Stored Procedure
Order mssqldb::ExecSPOrderInfo(SQLWCHAR* szSPName, int iOrderNo)
{
	ConnectStatement();
	retcode = SQLBindParameter(hstmt1, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &iOrderNo, 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm1) Failed\n\n");
		getchar();
	}

	Order curOrder;
	//SQLWCHAR * strCallSP = L"{CALL PQ_SELECT_ORDER(?)}";
	//SQLWCHAR * strCallSP2 = L"PQ_CUSTOMERLIST";
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{	
		printf("SQLExecDirect Failed\n\n");
		getchar();
	}
	else {
		//declare output variable and pointer
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;


		SQLINTEGER sqlOrderNo;
		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlDate[10];
		SQLCHAR sqlProduct[20];
		SQLDOUBLE sqlPrice;
		SQLINTEGER sqlQuantity;


		char buffer[1000];

		// Print Title
		//std::cout << std::endl;
		//sprintf_s(buffer, "%-10s %-10s %-15s %-20s %-8s %-10s", "ORDERNO", "CUSTNO", "DATE", "PRODUCT", "PRICE", "QNTY");
		//std::cout << buffer << std::endl;
		//sprintf_s(buffer, "%-10s %-10s %-15s %-20s %-8s %-10s", "-------", "--------", "-----", "-------", "-------", "----");
		//std::cout << buffer << std::endl;

		while (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_INTEGER, &sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_DOUBLE, &sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_INTEGER, &sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			curOrder.OrderNo = sqlOrderNo;
			curOrder.CustNo = (char*)sqlCustNo;
			curOrder.Date = (char*)sqlDate;
			curOrder.Product = (char*)sqlProduct;
			curOrder.Price = sqlPrice;
			curOrder.Quantity = sqlQuantity;

			//sprintf_s(buffer, "%-10d %-10s %-15s %-20s %-8.2lf %-10d", sqlOrderNo, sqlCustNo, sqlDate, sqlProduct, sqlPrice, sqlQuantity);
			//std::cout << buffer << std::endl;

		}
	}
	DisconnectStatement();
	return curOrder;
}

vector<Order> mssqldb::ExecSPOrderList(SQLWCHAR* szSPName, string custNo)
{
	vector <Order> vOrderList;

	ConnectStatement();
	retcode = SQLBindParameter(hstmt1, 1, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 5, 0, (SQLPOINTER)custNo.c_str(), 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm1) Failed\n\n");
		getchar();
	}

	Order curOrder;

	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
		getchar();
	}
	else {
		SQLCHAR sqlResult[SQL_RESULT_LEN];
		SQLINTEGER ptrSqlVersion;


		SQLINTEGER sqlOrderNo;
		SQLCHAR sqlCustNo[6];
		SQLCHAR sqlDate[10];
		SQLCHAR sqlProduct[20];
		SQLDOUBLE sqlPrice;
		SQLINTEGER sqlQuantity;


		char buffer[1000];
		while (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_INTEGER, &sqlOrderNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlDate, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlProduct, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_DOUBLE, &sqlPrice, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_INTEGER, &sqlQuantity, SQL_RESULT_LEN, &ptrSqlVersion);

			curOrder.OrderNo = sqlOrderNo;
			curOrder.CustNo = (char*)sqlCustNo;
			curOrder.Date = (char*)sqlDate;
			curOrder.Product = (char*)sqlProduct;
			curOrder.Price = sqlPrice;
			curOrder.Quantity = sqlQuantity;
			vOrderList.push_back(curOrder);
		}
	}
	DisconnectStatement();
	return vOrderList;
}

void mssqldb::ExecSPOrder(SQLWCHAR* szSPName, Order curOrder, DExecType cDEType)
{
	string arAlert[]{ "Creating table...", "Inserting data...", "Updating Data...", "Deleting Data..." };
	cout << arAlert[(int)cDEType] << endl;

	int ipar = 2;
	ConnectStatement();
	retcode = SQLBindParameter(hstmt1, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &curOrder.OrderNo, 0, NULL);
	if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
		printf("SQLBindParameter(sParm1) Failed\n\n");
		getchar();
	}

	if (cDEType != UPD && cDEType != DEL) 
	{
		retcode = SQLBindParameter(hstmt1, ipar++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 5, 0, (SQLPOINTER)curOrder.CustNo.c_str(), 0, NULL);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter(sParm2) Failed\n\n");
			getchar();
		}
		retcode = SQLBindParameter(hstmt1, ipar++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 10, 0, (SQLPOINTER)curOrder.Date.c_str(), 0, NULL);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter(sParm3) Failed\n\n");
			getchar();
		}
	}

	if(cDEType != DEL)
	{
		retcode = SQLBindParameter(hstmt1, ipar++, SQL_PARAM_INPUT, SQL_C_CHAR, SQL_VARCHAR, 20, 0, (SQLPOINTER)curOrder.Product.c_str(), 0, NULL);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter(sParm4) Failed\n\n");
			getchar();
		}
		retcode = SQLBindParameter(hstmt1, ipar++, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &curOrder.Price, 0, NULL);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter(sParm5) Failed\n\n");
			getchar();
		}
		retcode = SQLBindParameter(hstmt1, ipar++, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &curOrder.Quantity, 0, NULL);
		if ((retcode != SQL_SUCCESS) && (retcode != SQL_SUCCESS_WITH_INFO)) {
			printf("SQLBindParameter(sParm6) Failed\n\n");
			getchar();
		}
	}

	//SQLWCHAR * strCallSP = L"{CALL PQ_INSERT_ORDER(?,?,?,?,?,?)}";
	//SQLWCHAR * strCallSP2 = L"PQ_CUSTOMERLIST";
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szSPName, SQL_NTS))
	{
		printf("SQLExecDirect Failed\n\n");
		getchar();
	}
	else
	{
		string arAlert[]{ "Created table...", "Inserted data...", "Updated Data...", "Deleted Data..." };
		cout << arAlert[(int)cDEType] << " successfully." << endl;
	}

	DisconnectStatement();
}


//-- REFERENCE
void mssqldb::CreateTable(string szSQL)
{
	std::cout << endl << "Creating Table to DB..." << endl;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	RETCODE rc = SQLExecDirect(hstmt1, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg("[Create Table]", hstmt1, SQL_HANDLE_STMT);
	}
	else {
		std::cout << "Created table successfully" << endl;
	}
	DisconnectStatement();

}

void mssqldb::QueryData(string szSQL)
{
	std::cout << endl << "Executing T_SQL Query..." << endl;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	if (SQL_SUCCESS != SQLExecDirect(hstmt1, szWSQL, SQL_NTS))
	{
		std::cout << "Error quering SQL Server" << endl;
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

		while (SQLFetch(hstmt1) == SQL_SUCCESS)
		{
			SQLGetData(hstmt1, 1, SQL_CHAR, sqlCustNo, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 2, SQL_CHAR, sqlFirstName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 3, SQL_CHAR, sqlLastName, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 4, SQL_CHAR, sqlCity, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 5, SQL_CHAR, sqlAddress, SQL_RESULT_LEN, &ptrSqlVersion);
			SQLGetData(hstmt1, 6, SQL_CHAR, sqlPhone, SQL_RESULT_LEN, &ptrSqlVersion);

			sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", sqlCustNo, sqlFirstName, sqlLastName, sqlCity, sqlAddress, sqlPhone);
			std::cout << buffer << endl;
		}
	}
	DisconnectStatement();
}

void mssqldb::InsertData(string szSQL)
{
	std::cout << endl << "Inserting Data to DB..." << endl;

	SQLWCHAR* szWSQL = GetWString(szSQL);
	ConnectStatement();
	RETCODE rc = SQLExecDirect(hstmt1, szWSQL, SQL_NTS);
	if (rc != SQL_SUCCESS && rc != SQL_SUCCESS_WITH_INFO) {
		ShowErrorMsg("[Insert Data]", hstmt1, SQL_HANDLE_STMT);
	}
	else {
		std::cout << "inserted successfully" << endl;
	}
	DisconnectStatement();
}

