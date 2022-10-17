#pragma once
#include <time.h>
#include <iostream>
#include <stdlib.h>
#include <Array>
#include "CustomerMgmt.h"
#include "OrderHistoryMgmt.h"

class MainMgmt
{
private:
	CustomerMgmt* custMgmt;
	OrderHistoryMgmt* orderMgmt;
	Customer _curCustomer;

public:
	MainMgmt();	
	~MainMgmt();
	int MainMenu();

	bool _Login();

	void _Register();

	void LogOff();

	void ModifyCustInfo();

	void DeleteCustInfo();

	void ViewCustInfo();

	void ViewCustList();
	
	void NewOrder();

	void ModifyOrder();

	void DeleteOrder();

	void ViewOrderInfo();

	void ViewOrderList();

	void FindOrder();
};

