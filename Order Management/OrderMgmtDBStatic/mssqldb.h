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

enum DExecType { TABLE = 0, INS = 1, UPD = 2, DEL = 3 };

#define SQL_RESULT_LEN 240
#define SQL_RETURN_CODE_LEN 1000

using namespace std;

class mssqldb
{
public:
	static void ConnectDB();
	static void DisconnectDB();
	static void ConnectStatement();
	static void DisconnectStatement();

	static SQLWCHAR* GetWString(string szStr);
	static void ShowErrorMsg(char* fn, SQLHANDLE handle, SQLSMALLINT type);

	static bool CheckID(string custNo);
	static string NextCustNo();
	static void ExecDBData(string szSQL, DExecType DEType);
	static Customer QueryCustomerInfo(string szSQL);
	static vector<Customer> QueryCustomerList(string szSQL);

	// For Orders
	static bool CheckOrderId(string orderNo, string custNo);
	static int NextOrderNo();
	static Order QueryOrderInfo(string szSQL);
	static vector<Order> QueryOrderList(string szSQL);

	// stored procedure
	static Order ExecSPOrderInfo(SQLWCHAR* szSPName, int iOrderNo);
	static vector<Order> ExecSPOrderList(SQLWCHAR* szSPName, string custNo);
	static void ExecSPOrder(SQLWCHAR* szSPName, Order curOrder, DExecType cDEType);

	//--REFERENCE
	static void CreateTable(string szSQL);
	static void QueryData(string szSQL);
	static void InsertData(string szSQL);
};

