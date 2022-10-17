// OrderMgmtDB.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <iostream>
#include "MyUtils.h"
#include "MainMgmt.h"
#include "mssqldb.h"

int main()
{
	// Conenct MSSQL DB
	mssqldb::ConnectDB();
	
	SQLWCHAR * strCallSP = L"{CALL PQ_ORDERINFO(?)}";
	mssqldb::ExecSPOrderInfo(strCallSP, 1);

	MainMgmt* mainMgmt = new MainMgmt();

	int choice;
	bool flag = false, logon = false;

	cout << "Welcome!\n" << endl;
	while (1) {
		choice = mainMgmt->MainMenu();

		if (!logon) {
			switch (choice) {
			case 0:
				cout << "Goodbye!" << endl;
				flag = true;
				break;
			case 1:
				logon = mainMgmt->_Login();
				break;
			case 2:
				mainMgmt->_Register();
				logon = true;
				break;
			default:
				Message::Out("That is not a choice!\n\n", true);
				break;
			}
		}
		else {
			switch (choice) {
			case 3:
				mainMgmt->ModifyCustInfo();
				break;
			case 4:
				mainMgmt->DeleteCustInfo();
				break;
			case 5:
				mainMgmt->ViewCustInfo();
				break;
			case 6:
				mainMgmt->ViewCustList();
				break;
			case 7:
				mainMgmt->NewOrder();
				break;
			case 8:
				mainMgmt->ModifyOrder();
				break;
			case 9:
				mainMgmt->DeleteOrder();
				break;
			case 10:
				mainMgmt->ViewOrderInfo();
				break;
			case 11:
				mainMgmt->ViewOrderList();
				break;
			case 12:
				mainMgmt->FindOrder();
				break;
			case 13:
				mainMgmt->LogOff();
				logon = false;
				break;
			default:
				Message::Out("That is not a choice!\n\n", true);
				break;
			}
		}

		if (flag) break;
	}

	mssqldb::DisconnectDB();
	return 0;
}

