#include "textutil.h"

#define PLAYER '@'

int x = 0, y = 0;

int main() {
	int quit = 0;
	int k;

	tu_init();
	tu_clear_screen();
	tu_hide_cursor();
	k = -1;
	x = y = 0;
	tu_clear_screen();
	tu_set_pos(x, y);
	tu_put_char(PLAYER);
	while (!quit) {
		k = tu_get_key_async();
		if (k != KEY_NOTHING) {
			tu_set_pos(x, y);
			tu_put_char(' ');
			switch (k) {
			case KEY_UP:
			case 'w':
			case 'W':
				y--;
				break;
			case KEY_LEFT:
			case 'a':
			case 'A':
				x--;
				break;
			case KEY_DOWN:
			case 's':
			case 'S':
				y++;
				break;
			case KEY_RIGHT:
			case 'd':
			case 'D':
				x++;
				break;
			}
			tu_set_pos(x, y);
			tu_put_char(PLAYER);
		}
		if (k == 'q')
			quit = 1;
	}
	tu_show_cursor();
	tu_shutdown();

	return 0;
}
