#include "quess.h"

namespace  {
int main() {
	int r;
	r = random1_100();
	set_locale();
	console_out("� ������� ����� �� 1 �� 100 ������������!\n������ ���!\n\n");
	int cnt;
	cnt = 0;
{
	console_out("������� ����� �� 1 �� 100: ");
	int v;
	v = get_value();
	cnt = cnt + 1;
{
	console_out("�� �������! C ");
	console_out_int(cnt);
	console_out(" �������!");
	wait_any_char();
	return 0;
}

	if (r > v) console_out("������!\n");
	else if (r < v) console_out("������!\n");
	else }

	while (1) 	return 0;
}


}
