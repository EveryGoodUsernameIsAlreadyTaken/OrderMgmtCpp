#pragma once
#include <iostream>
#include <string>

using namespace std;

class Order {
public:
	int OrderNo;
	string CustNo, Date, Product;
	double Price;
	int Quantity;

	Order() {};

	Order(int id, string cust, string date, string prd, double pr, int qty) {
		OrderNo = id;
		CustNo = cust;
		Date = date;
		Product = prd;
		Price = pr;
		Quantity = qty;
	}
};