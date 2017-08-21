#ifndef __TEXTUTIL_POSIX_H
#define __TEXTUTIL_POSIX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/types.h>

/* theoretically this is an include loop. Thanks include guards. */
#include "textutil.h"

void
__tu_atexit(int sig)
{
	/* silence warning */
	(void)(sig);
	tu_show_cursor();
	tu_shutdown();
	exit(1);
}

void
tu_init(void)
{
	signal(SIGINT, __tu_atexit);

	/* enter alternate buffer */
	printf("\033[?1049h");
	tu_clear_screen();
	fflush(stdout);
	tu_set_pos(0, 0);
}

void
tu_shutdown(void)
{
	/* exit alternate buffer */
	printf("\033[?1049l");
}

void
tu_hide_cursor(void)
{
	printf("%s", "\033[?25l");
}

void
tu_show_cursor(void)
{
	printf("%s", "\033[?25h");
}

void
tu_clear_screen(void)
{
	printf("%s", "\033[2J\033[3J"); /* clear screen */
	printf("%s", "\033[H"); /* cursor home */
}

void
tu_set_pos(int x, int y)
{
	y++;
	x++;
	printf("\033[%d;%df", y, x);
}

void
tu_put_char(char chr)
{
	fputc(chr, stdout);
	printf("%s", "\033[1D"); /* roll back the cursor */
	fflush(stdout);
}

int
tu_put_string(char *str)
{
	size_t len = strlen((char *)str);
	printf("%s", str);
	printf("\033[%zuD", len);

	return len;
}

/* tu_put_stringf is not implementation-specific */

static char * get_ANSI_fg_color(enum TU_color color)
{
	switch (color) {
	case BLACK       : return "\033[22;30m";
	case BLUE        : return "\033[22;34m"; // non-ANSI
	case GREEN       : return "\033[22;32m";
	case CYAN        : return "\033[22;36m";
	case RED         : return "\033[22;31m";
	case MAGENTA     : return "\033[22;35m";
	case BROWN       : return "\033[22;33m";
	case GREY        : return "\033[22;37m";
	case DARKGREY    : return "\033[01;30m";
	case LIGHTBLUE   : return "\033[01;34m";
	case LIGHTGREEN  : return "\033[01;32m";
	case LIGHTCYAN   : return "\033[01;36m";
	case LIGHTRED    : return "\033[22;31m";
	case LIGHTMAGENTA: return "\033[22;35m";
	case YELLOW      : return "\033[01;33m";
	case WHITE       : return "\033[01;37m";
	default          : return "";
	}
}

static char * get_ANSI_bg_color(enum TU_color color)
{
	switch (color) {
	case BLACK  : return "\033[40m";
	case BLUE   : return "\033[44m";
	case GREEN  : return "\033[42m";
	case CYAN   : return "\033[46m";
	case RED    : return "\033[41m";
	case MAGENTA: return "\033[45m";
	case BROWN  : return "\033[43m";
	case GREY   : return "\033[47m";
	default     : return "";
	}
}

void
tu_set_fg_color(enum TU_color color)
{
	printf("%s", get_ANSI_fg_color(color));
}

void
tu_set_bg_color(enum TU_color color)
{
	printf("%s", get_ANSI_bg_color(color));
}

void
tu_reset_color(void)
{
	printf("%s", "\033[0m");
}

static int
getch(void)
{
	struct termios oldt, newt;
	int ch = 0;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	read(fileno(stdin), &ch, 1);
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
}

/* Returns the number of characters hit */
static int kbhit(void)
{
	static struct termios oldt, newt;
	struct timeval tv;
	int cnt = 0;

	tcgetattr(STDIN_FILENO, &oldt);

	newt = oldt;
	newt.c_lflag    &= ~(ICANON | ECHO);
	newt.c_iflag     = 0; /* input mode */
	newt.c_oflag     = 0; /* output mode */
	newt.c_cc[VMIN]  = 1; /* minimum time to wait */
	newt.c_cc[VTIME] = 1; /* minimum characters to wait for */

	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ioctl(0, FIONREAD, &cnt); /* read count */

	tv.tv_sec  = 0;
	tv.tv_usec = 100;
	select(STDIN_FILENO+1, NULL, NULL, NULL, &tv); /* small time delay */
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return cnt;
}

static int
raw_key_to_key(int cnt, int k) {
	int kk;

	switch (k) {
	case 0: {
		switch (kk = getch()) {
		case 71: return KEY_NUMPAD7;
		case 72: return KEY_NUMPAD8;
		case 73: return KEY_NUMPAD9;
		case 75: return KEY_NUMPAD4;
		case 77: return KEY_NUMPAD6;
		case 79: return KEY_NUMPAD1;
		case 80: return KEY_NUMPAD2;
		case 81: return KEY_NUMPAD3;
		case 82: return KEY_NUMPAD0;
		case 83: return KEY_NUMDEL;
		default: return kk-59+KEY_F1; /* FN keys */
		}}
	case 224: {
		switch (kk = getch()) {
		case 71: return KEY_HOME;
		case 72: return KEY_UP;
		case 73: return KEY_PGUP;
		case 75: return KEY_LEFT;
		case 77: return KEY_RIGHT;
		case 79: return KEY_END;
		case 80: return KEY_DOWN;
		case 81: return KEY_PGDOWN;
		case 82: return KEY_INSERT;
		case 83: return KEY_DELETE;
		default: return kk-123+KEY_F1; /* FN keys */
		}}
	case 13: return KEY_ENTER;
	case 155: /* single character CSI */
	case 27: {
		/* process ANSI escape sequences */
		if (cnt >= 3 && getch() == '[') {
			switch (k = getch()) {
			case 'A': return KEY_UP;
			case 'B': return KEY_DOWN;
			case 'C': return KEY_RIGHT;
			case 'D': return KEY_LEFT;
			}
		} else {
			return KEY_ESCAPE;
		}
	}
	default: return k;
	}
}

int tu_get_key_sync(void)
{
	int cnt = kbhit();
	int k = getch();
	return raw_key_to_key(cnt, k);
}

int tu_get_key_async(void)
{
	int cnt = kbhit();
	if (cnt)
		return raw_key_to_key(cnt, getch());
	else
		return KEY_NOTHING;
}

void
tu_sleep(unsigned int millis)
{
	struct timespec ts[1] = {{1000000 * millis, 0}};
	nanosleep(ts, NULL);
}

void
tu_get_term_size(int *lines, int *cols)
{
	struct winsize ws;
	if (ioctl(1, TIOCGWINSZ, &ws) < 0)
		*lines = *cols = -1;

	*lines = ws.ws_row;
	*cols = ws.ws_col;
}

#ifdef __cplusplus
}
#endif

#endif
