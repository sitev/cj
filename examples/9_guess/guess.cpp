#include "quess.h"

namespace  {
int main() {
	int r;
	r = random1_100();
	set_locale();
	console_out("Я загадал число от 1 до 100 включительно!\nУгадай его!\n\n");
	int cnt;
	cnt = 0;
{
	console_out("Введите число от 1 до 100: ");
	int v;
	v = get_value();
	cnt = cnt + 1;
{
	console_out("Вы угадали! C ");
	console_out_int(cnt);
	console_out(" попытки!");
	wait_any_char();
	return 0;
}

	if (r > v) console_out("Больше!\n");
	else if (r < v) console_out("Меньше!\n");
	else }

	while (1) 	return 0;
}


}
