int random1_100() from "myfuncs_cpp";
void set_locale() from "myfuncs_cpp";
void console_out(string s) from "myfuncs_cpp";
void console_out_int(int a) from "myfuncs_cpp";
void wait_any_char() from "myfuncs_cpp";
int get_value() from "myfuncs_cpp";

int main() {
	int r = random1_100();
	set_locale();
	console_out("� ������� ����� �� 1 �� 100 ������������!\n������ ���!\n\n");
	int cnt = 0;
	while (1) {
		console_out("������� ����� �� 1 �� 100: ");
		int v = get_value();

		cnt = cnt + 1;

		if (r > v) console_out("������!\n");
		else if (r < v) console_out("������!\n");
		else {
			console_out("�� �������! C ");
			console_out_int(cnt);
			console_out(" �������!");
			wait_any_char();
			return 0;
		}

	}
	return 0;
}