#include "MainMgmt.h"
#include "MyUtils.h"

using namespace std;

MainMgmt::MainMgmt() 
{
	custMgmt = new CustomerMgmt();
	orderMgmt = new OrderHistoryMgmt();
}


MainMgmt::~MainMgmt() 
{
}

int MainMgmt::MainMenu() {
	int MenuStatus;

	if (_curCustomer._CustNo == "") {
		cout << "press (1)Login\npress (2)Register\npress (0)Exit\n";
	}
	else {
		cout << "press (3)Modify Customer\npress (4)Delete Customer\npress (5)View Customer Info\npress (6)View All Customer List\npress (7)New Order\npress (8)Modify Order\npress (9)Delete Order\npress (10)View Order\npress (11)View All Order List\npress (12)Find Order\npress (13)Main Menu\n";
	}
	cout << "Your choice: ";
	cin >> MenuStatus;
	cout << endl;

	return MenuStatus;
}


bool MainMgmt::_Login() 
{
	string custNo;
	// connection string
	//SQLWCHAR connStr[] = L"DRIVER={SQL Server};SERVER=noesys, 1433; DATABASE=CPPDB; UID=cpptest; PWD=cpptest;";

	//MSDBConn* myDBConn = new MSDBConn(connStr);

	// insert data
	//SQLWCHAR szInsertSQL[] = L"INSERT INTO CUSTOMERS VALUES(11, 'JJ', 'Park', 'Franklin Lakes', '100 Main St', '555-111-9999');";
	//myDBConn->InsertData(szInsertSQL);

	//SQLWCHAR szUpdateSQL[] = L"UPDATE CUSTOMERS SET FIRSTNAME='PPeter' WHERE CUSTNO =2;";
	//myDBConn->InsertData(szUpdateSQL);

	//SQLWCHAR szDeleteSQL[] = L"DELETE FROM CUSTOMERS WHERE CUSTNO =11;";
	//myDBConn->InsertData(szDeleteSQL);

	// select data from db
	//SQLWCHAR szSQL[] = L"SELECT * FROM CUSTOMERS";
	//myDBConn->QueryData(szSQL);

	//myDBConn->Disconnection();
	//pause the console window

	//std::cout << "\nPress any key to exit..." << endl;
	//getchar();


	while (1) 
	{
		cout << "\nPlease sign in with your 5 digit customer number: ";
		cin >> custNo;
	
		if (custMgmt->_FindId(custNo)) {
			_curCustomer = custMgmt->GetCustInfo(custNo);
			Message::Out("Login page \n\n", true);
			cout << "\t\tWelcome " << _curCustomer.FullName() << "!" << endl << endl;
			getchar();
			return true;
		}
		else 
		{
			char buffer[100];
			sprintf_s(buffer, "\n>> Result - No data with CustNo#%s\n\n\nPress anything to try again or press (0) to main menu:", custNo.c_str());
			Message::Out(buffer, true);
			getchar();
			getline(cin, custNo);
			if (custNo == "0") return false;
		}
	}
	return false;
}

//SQLWCHAR* GetWString(string szStr)
//{
//	int iLen = strlen(szStr.c_str()) + 1;
//	SQLWCHAR* wStr = new SQLWCHAR[iLen];
//
//	for (int i = 0; i < iLen; i++)
//	{
//		wStr[i] = szStr[i];
//	}
//	return wStr;
//}

void MainMgmt::_Register() 
{
	_curCustomer = custMgmt->AppendCustomerInfo();
}

void MainMgmt::LogOff() {
	_curCustomer._CustNo = "";
}

void MainMgmt::ModifyCustInfo() {
	custMgmt->ModifyCustomerInfo(_curCustomer._CustNo);
}

void MainMgmt::DeleteCustInfo() {
	custMgmt->DeleteCustomerInfo(_curCustomer._CustNo);
}

void MainMgmt::ViewCustInfo() {
	custMgmt->ViewCustomerInfo(_curCustomer._CustNo);
}

void MainMgmt::ViewCustList() {
	custMgmt->ViewCustomerList(_curCustomer._CustNo);
}

void MainMgmt::NewOrder() {
	//orderMgmt->AppendNewOrder(_curCustomer._CustNo);
	orderMgmt->AppendNewOrderSP(_curCustomer._CustNo);
}

void MainMgmt::ModifyOrder() {
	//orderMgmt->ModifyOrderInfo(_curCustomer._CustNo);
	orderMgmt->ModifyOrderInfoSP(_curCustomer._CustNo);
}

void MainMgmt::DeleteOrder() {
	//orderMgmt->DeleteOrderInfo(_curCustomer._CustNo);
	orderMgmt->DeleteOrderInfoSP(_curCustomer._CustNo);
}

void MainMgmt::ViewOrderInfo() {
	orderMgmt->ViewOrderInfo(_curCustomer._CustNo);
}

void MainMgmt::ViewOrderList() {
	orderMgmt->ViewOrderList(_curCustomer._CustNo);
}

void MainMgmt::FindOrder() {
	orderMgmt->FindOrder(_curCustomer._CustNo);
}
