#include <ctime>

#include "myfuncs_cpp.h"

int random1_100() {
	srand(time(NULL));
	int r = rand() % 100 + 1;
	return r;
}

void set_locale() {
	setlocale(LC_ALL, "");
}

void console_out(string s) {
	cout << s;
}

void console_out_int(int a) {
	cout << a;
}

int get_value() {
	string s;
	cin >> s;
	return stoi(s);
}

void wait_any_char() {
	system("pause");
}
