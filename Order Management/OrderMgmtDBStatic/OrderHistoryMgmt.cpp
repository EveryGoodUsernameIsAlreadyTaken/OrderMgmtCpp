#include "OrderHistoryMgmt.h"

OrderHistoryMgmt::OrderHistoryMgmt(int orderNo) {
	_OrderNo = orderNo;
	curOrder = new Order();

	string szSQL = "CREATE TABLE ORDERHISTORY \
		(ORDERNO	INT PRIMARY KEY NOT NULL, \
		CUSTNO		VARCHAR(5) NOT NULL, \
		DATE		VARCHAR(10), \
		PRODUCT		VARCHAR(20) NOT NULL DEFAULT(''), \
		PRICE		DECIMAL(8, 2), \
		QUANTITY	INT )";

	//mssqldb::CreateTable(szSQL);

	//MSDBConn::AddColumn("ALTER TABLE ORDERHISTORY ADD COLUMN ORDERNO VARCHAR(6), ADD COLUMN CUSTNO VARCHAR(6), ADD COLUMN DATE VARCHAR(11), ADD COLUMN PRODUCT VARCHAR(16), ADD COLUMN PRICE VARCHAR(11), ADD COLUMN QUANTITY VARCHAR(11)");
}

string OrderHistoryMgmt::partition(string& line) {
	string ret = line.substr(0, line.find(","));
	line = line.substr(line.find(",") + 1);
	void ViewCustomerInfo(string szOrderNo);
	return ret;
}

int OrderHistoryMgmt::GetNewOrderId() {
	return mssqldb::NextOrderNo();
};

void OrderHistoryMgmt::AppendNewOrderSQL(string custNo) {
	getchar();
	Message::Out("Order page\nPlease enter your information\n\n", true);

	struct tm newtime;
	__time64_t long_time;
	errno_t err;
	_time64(&long_time);
	err = _localtime64_s(&newtime, &long_time); 
	
	_OrderNo = GetNewOrderId();
	curOrder->OrderNo = _OrderNo;
	curOrder->CustNo = custNo;
	curOrder->Date = to_string(newtime.tm_mon + 1) + "/" + to_string(newtime.tm_mday) + "/" + to_string(newtime.tm_year + 1900);

	cout << "Product: ";
	getline(cin, curOrder->Product);
	cout << endl;

	char* iEnd;
	string input;
	
	while (1) {
		cout << "Price: ";
		getline(cin, input);
		
		curOrder->Price = strtod(input.c_str(), &iEnd);
		if (!input.empty() && input.find_first_not_of("0123456789") == string::npos)
		{
			curOrder->Price = stod(input);
			break;
		}
		else if (input.empty() || iEnd == input.c_str() || *iEnd != '\0')
			Message::OutLine("That is not a valid input! Please try again.\n", true);
		else
			break;
	}
	cout << endl;

	while (1) {
		cout << "Quantity: ";
		getline(cin, input);

		if (input.find_first_not_of("0123456789") == string::npos) {
			curOrder->Quantity = stoi(input);
			break;
		}
		else
			Message::OutLine("That is not a valid input! Please try again.\n", true);
	}
	cout << endl;

	string szSQL = "INSERT INTO ORDERHISTORY VALUES( ";
	szSQL += "'" + to_string(curOrder->OrderNo) + "', ";
	szSQL += "'" + curOrder->CustNo + "', ";
	szSQL += "'" + curOrder->Date + "', ";
	szSQL += "'" + curOrder->Product + "', ";
	szSQL += "'" + to_string(curOrder->Price) + "', ";
	szSQL += "'" + to_string(curOrder->Quantity) + "' )";
	
	//cout << szSQL << endl;
	
	mssqldb::ExecDBData(szSQL, INS);
}

void OrderHistoryMgmt::AppendNewOrderSP(string custNo) {
	getchar();
	Message::Out("Order page\nPlease enter your information\n\n", true);

	struct tm newtime;
	__time64_t long_time;
	errno_t err;
	_time64(&long_time);
	err = _localtime64_s(&newtime, &long_time);

	_OrderNo = GetNewOrderId();
	curOrder->OrderNo = _OrderNo;
	curOrder->CustNo = custNo;
	curOrder->Date = to_string(newtime.tm_mon + 1) + "/" + to_string(newtime.tm_mday) + "/" + to_string(newtime.tm_year + 1900);

	cout << "Product: ";
	getline(cin, curOrder->Product);
	cout << endl;

	char* iEnd;
	string input;

	while (1) {
		cout << "Price: ";
		getline(cin, input);

		curOrder->Price = strtod(input.c_str(), &iEnd);
		if (!input.empty() && input.find_first_not_of("0123456789") == string::npos)
		{
			curOrder->Price = stod(input);
			break;
		}
		else if (input.empty() || iEnd == input.c_str() || *iEnd != '\0')
			Message::OutLine("That is not a valid input! Please try again.\n", true);
		else
			break;
	}
	cout << endl;

	while (1) {
		cout << "Quantity: ";
		getline(cin, input);

		if (input.find_first_not_of("0123456789") == string::npos) {
			curOrder->Quantity = stoi(input);
			break;
		}
		else
			Message::OutLine("That is not a valid input! Please try again.\n", true);
	}
	cout << endl;

	//cout << szSQL << endl;

	mssqldb::ExecSPOrder(L"{CALL PQ_INSERT_ORDER(?,?,?,?,?,?)}", *curOrder, INS);
}

void OrderHistoryMgmt::ModifyOrderInfoSQL(string custNo) {
	string szOrderNo;
	while (1) {
		cout << "Please type the ID of the order you want to modify\n OrderID: ";
		getchar();
		getline(cin, szOrderNo);
		if (mssqldb::CheckOrderId(szOrderNo, custNo)) break;
		else {
			cout << "There is no such ID.\nDo you want to try again? press Y(y) to try again or press anything else to cancel.\n";
			getline(cin, szOrderNo);
			if (szOrderNo != "Y" && szOrderNo != "y") return;
		}
	}

	string szOrderSQL = "SELECT * FROM ORDERHISTORY WHERE ORDERNO = '" + szOrderNo + "'";
	Order curOrderInfo = mssqldb::QueryOrderInfo(szOrderSQL);

	Message::Out("Modify Order page \nPlease enter your information\n", true);

	Order* newOrderInfo = new Order();
	newOrderInfo->OrderNo = stoi(szOrderNo);
	newOrderInfo->Date = curOrderInfo.Date;
	newOrderInfo->CustNo = curOrderInfo.CustNo;

	char buffer[100];
	sprintf_s(buffer, "Change Product (current - %s): ", curOrderInfo.Product.c_str());
	Message::Out(buffer, false);

	getline(cin, newOrderInfo->Product);
	if (newOrderInfo->Product.empty()) newOrderInfo->Product = curOrderInfo.Product;
	cout << endl;

	string input;
	char* iEnd;
	while (1) {
		sprintf_s(buffer, "Change Price (current - %.2lf): ", curOrderInfo.Price);
		Message::Out(buffer, false);
		getline(cin, input);

		if (input.empty())
		{
			newOrderInfo->Price = curOrderInfo.Price;
			break;
		}

		newOrderInfo->Price = strtod(input.c_str(), &iEnd);
		if (!input.empty() && input.find_first_not_of("0123456789") == string::npos)
		{
			newOrderInfo->Price = stod(input);
			break;
		}
		else if (input.empty() || iEnd == input.c_str() || *iEnd != '\0')
			Message::OutLine("That is not a valid input! Please try again.\n", true);
		else
			break;
	}
	cout << endl;

	while (1) {
		sprintf_s(buffer, "Change Quantity (current - %d): ", curOrderInfo.Quantity);
		Message::Out(buffer, false);
		getline(cin, input);

		if (input.empty())
		{
			newOrderInfo->Price = curOrderInfo.Price;
			break;
		}

		if (input.find_first_not_of("0123456789") == string::npos) {
			newOrderInfo->Quantity = stoi(input);
			break;
		}
		else
			Message::OutLine("That is not a valid input! Please try again.\n", true);
	}
	cout << endl;

	string szSQL = "UPDATE ORDERHISTORY SET PRODUCT='" + newOrderInfo->Product + "', ";
	szSQL += " PRICE = '" + to_string(newOrderInfo->Price) + "', ";
	szSQL += " QUANTITY = '" + to_string(newOrderInfo->Quantity) + "' ";
	szSQL += " WHERE ORDERNO = '" + to_string(newOrderInfo->OrderNo) + "' ";
	mssqldb::ExecDBData(szSQL, UPD);
}

void OrderHistoryMgmt::ModifyOrderInfoSP(string custNo) {
	string szOrderNo;
	while (1) {
		cout << "Please type the ID of the order you want to modify\n OrderID: ";
		getchar();
		getline(cin, szOrderNo);
		if (mssqldb::CheckOrderId(szOrderNo, custNo)) break;
		else {
			cout << "There is no such ID.\nDo you want to try again? press Y(y) to try again or press anything else to cancel.\n";
			getline(cin, szOrderNo);
			if (szOrderNo != "Y" && szOrderNo != "y") return;
		}
	}

	string szOrderSQL = "SELECT * FROM ORDERHISTORY WHERE ORDERNO = '" + szOrderNo + "'";
	Order curOrderInfo = mssqldb::QueryOrderInfo(szOrderSQL);

	Message::Out("Modify Order page \nPlease enter your information\n", true);

	Order* newOrderInfo = new Order();
	newOrderInfo->OrderNo = stoi(szOrderNo);
	newOrderInfo->Date = curOrderInfo.Date;
	newOrderInfo->CustNo = curOrderInfo.CustNo;

	char buffer[100];
	sprintf_s(buffer, "Change Product (current - %s): ", curOrderInfo.Product.c_str());
	Message::Out(buffer, false);

	getline(cin, newOrderInfo->Product);
	if (newOrderInfo->Product.empty()) newOrderInfo->Product = curOrderInfo.Product;
	cout << endl;

	string input;
	char* iEnd;
	while (1) {
		sprintf_s(buffer, "Change Price (current - %.2lf): ", curOrderInfo.Price);
		Message::Out(buffer, false);
		getline(cin, input);

		if (input.empty())
		{
			newOrderInfo->Price = curOrderInfo.Price;
			break;
		}

		newOrderInfo->Price = strtod(input.c_str(), &iEnd);
		if (!input.empty() && input.find_first_not_of("0123456789") == string::npos)
		{
			newOrderInfo->Price = stod(input);
			break;
		}
		else if (input.empty() || iEnd == input.c_str() || *iEnd != '\0')
			Message::OutLine("That is not a valid input! Please try again.\n", true);
		else
			break;
	}
	cout << endl;

	while (1) {
		sprintf_s(buffer, "Change Quantity (current - %d): ", curOrderInfo.Quantity);
		Message::Out(buffer, false);
		getline(cin, input);

		if (input.empty())
		{
			newOrderInfo->Price = curOrderInfo.Price;
			break;
		}

		if (input.find_first_not_of("0123456789") == string::npos) {
			newOrderInfo->Quantity = stoi(input);
			break;
		}
		else
			Message::OutLine("That is not a valid input! Please try again.\n", true);
	}
	cout << endl;

	
	mssqldb::ExecSPOrder(L"{CALL PQ_UPDATE_ORDER(?,?,?,?)}", *newOrderInfo, UPD);
}

void OrderHistoryMgmt::DeleteOrderInfoSQL(string custNo) {
	string szOrderNo;
	while (1) {
		cout << "Please type the ID of the order you want to delete\n OrderID: ";
		getchar();
		getline(cin, szOrderNo);
		if (mssqldb::CheckOrderId(szOrderNo, custNo)) break;
		else {
			cout << "There is no such ID.\nDo you want to try again? press Y(y) to try again or press anything else to cancel.\n";
			getline(cin, szOrderNo);
			if (szOrderNo != "Y" && szOrderNo != "y") return;
		}
	}

	Message::Out("Delete Order page \n\n", true);
	std::cout << "Are you sure to delete order [" + szOrderNo + "]\npress Y(y) to continue or press anything else to cancel: ";

	string input;
	getline(cin, input);

	try {
		if (input == "y" || input == "Y")
		{
			string szSQL = "DELETE FROM ORDERHISTORY WHERE ORDERNO = '" + szOrderNo + "' ";
			mssqldb::ExecDBData(szSQL, DEL);
		}
		else {
			cout << "Deletion cancelled.\n\n";
		}
	}
	catch (...)
	{
		cout << "An unexpected error occurred." << endl;
	}
}

void OrderHistoryMgmt::DeleteOrderInfoSP(string custNo) {
	string szOrderNo;
	while (1) {
		cout << "Please type the ID of the order you want to delete\n OrderID: ";
		getchar();
		getline(cin, szOrderNo);
		if (mssqldb::CheckOrderId(szOrderNo, custNo)) break;
		else {
			cout << "There is no such ID.\nDo you want to try again? press Y(y) to try again or press anything else to cancel.\n";
			getline(cin, szOrderNo);
			if (szOrderNo != "Y" && szOrderNo != "y") return;
		}
	}

	Message::Out("Delete Order page \n\n", true);
	std::cout << "Are you sure to delete order [" + szOrderNo + "]\npress Y(y) to continue or press anything else to cancel: ";

	string input;
	getline(cin, input);

	try {
		if (input == "y" || input == "Y")
		{
			mssqldb::ExecSPOrder(L"{CALL PQ_DELETE_ORDER(?)}", mssqldb::QueryOrderInfo("SELECT * FROM ORDERHISTORY WHERE ORDERNO = '" + szOrderNo + "'"), DEL);
		}
		else {
			cout << "Deletion cancelled.\n\n";
		}
	}
	catch (...)
	{
		cout << "An unexpected error occurred." << endl;
	}
}

void OrderHistoryMgmt::ViewOrderInfo(string custNo) {
	Message::Out("View Order page \n\n", true);
	if (!mssqldb::CheckOrderId(to_string(_OrderNo), custNo)) {
		cout << "The current order does not exist\n\n";
		return;
	}

	Order order = mssqldb::ExecSPOrderInfo(L"{CALL PQ_ORDERINFO(?)}", _OrderNo);

	char buffer[1000];
	std::cout << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "ORDERNO", "CUSTNO", "DATE", "PRODUCT", "PRICE", "QUANTITY");
	std::cout << buffer << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "-------", "------", "----", "-------", "-----", "--------");
	std::cout << buffer << endl;
	sprintf_s(buffer, "%-10d %-30s %-30s %-30s %-30.2lf %-12d", order.OrderNo, order.CustNo.c_str(), order.Date.c_str(), order.Product.c_str(), order.Price, order.Quantity);
	cout << buffer << endl << endl;
}

void OrderHistoryMgmt::ViewOrderList(string custNo) {
	Message::Out("View Orders page \n\n", true);

	vector<Order> orders = mssqldb::ExecSPOrderList(L"{CALL PQ_ORDERLIST(?)}", custNo);

	char buffer[1000];
	std::cout << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "ORDERNO", "CUSTNO", "DATE", "PRODUCT", "PRICE", "QUANTITY");
	std::cout << buffer << endl;
	sprintf_s(buffer, "%-10s %-30s %-30s %-30s %-30s %-12s", "-------", "------", "----", "-------", "-----", "--------");
	std::cout << buffer << endl;

	for (int i = 0; i < orders.size(); i++) {
		sprintf_s(buffer, "%-10d %-30s %-30s %-30s %-30.2lf %-12d", orders[i].OrderNo, orders[i].CustNo.c_str(), orders[i].Date.c_str(), orders[i].Product.c_str(), orders[i].Price, orders[i].Quantity);
		cout << buffer << endl;
	}
	cout << endl;
}

void OrderHistoryMgmt::FindOrder(string custNo) {
	string szOrderNo;
	while (1) {
		cout << "Please type the ID of the order you want\n OrderID: ";
		getchar();
		getline(cin, szOrderNo);
		if (mssqldb::CheckOrderId(szOrderNo, custNo)) break;
		else {
			cout << "There is no such ID.\nDo you want to try again? press Y(y) to try again or press anything else to cancel.\n";
			getline(cin, szOrderNo);
			if (szOrderNo != "Y" || szOrderNo != "y") return;
		}
	}

	string szSQL = "SELECT * FROM ORDERHISTORY WHERE ORDERNO = '" + szOrderNo + "'";
	*curOrder = mssqldb::QueryOrderInfo(szSQL);
	_OrderNo = curOrder->OrderNo;

	cout << "Successfully found order!\n\n";
}