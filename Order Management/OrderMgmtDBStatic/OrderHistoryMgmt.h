#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "Order.h"
#include "mssqldb.h"
#include "MyUtils.h"

class OrderHistoryMgmt
{
private:
	//string FileName = "OrderHistoryInfo.txt";
	int _OrderNo;
	Order* curOrder;

public:
	OrderHistoryMgmt(int orderNo = 0);

	string partition(string& line);
	
	int GetNewOrderId();

	void AppendNewOrderSQL(string custNo);
	void AppendNewOrderSP(string custNo);

	void ModifyOrderInfoSQL(string custNo);
	void ModifyOrderInfoSP(string custNo);

	void DeleteOrderInfoSQL(string custNo);
	void DeleteOrderInfoSP(string custNo);

	void ViewOrderInfo(string custNo);

	void ViewOrderList(string custNo);

	void FindOrder(string custNo);
};

