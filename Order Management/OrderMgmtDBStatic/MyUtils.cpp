#include "MyUtils.h"

using namespace std;

void Message::Out(string msg, int lines, bool bClear) {
	if (bClear) system("CLS");

	cout << msg;
	while (lines--) cout << endl;
}