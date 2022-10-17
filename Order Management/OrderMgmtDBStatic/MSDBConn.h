#pragma once
#include <string>
#include <iostream>
#include <Windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include "Customer.h"
#include "Order.h"

enum DExecType { TABLE = 0, INS=1, UPD=2, DEL=3 };

#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000

using namespace std;

class MSDBConn
{
private:

public:
	static void ConnectDB();
	static void DisconnectDB();

	static SQLWCHAR* GetWString(string szStr);
	static void QueryData(string szSQL);
	static void CreateTable(string szSQL);
	static void InsertData(string szSQL);
	static void ExecDBData(string szSQL, DExecType DEType);
	static void ShowErrorMsg(char *fn, SQLHANDLE handle, SQLSMALLINT type);

	// For Customers
	static bool CheckID(string custNo);
	static string NextCustNo();
	static Customer QueryCustomerInfo(string szSQL);
	static vector<Customer> QueryCustomerList(string szSQL);

	// For Orders
	static bool CheckOrderId(string orderNo, string custNo);
	static string NextOrderNo();
	static Order QueryOrderInfo(string szSQL);
	static vector<Order> QueryOrderList(string szSQL);
};

