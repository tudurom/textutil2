#ifndef __TEXTUTIL_H
#define __TEXTUTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum TU_color {
	BLACK,
	BLUE,
	GREEN,
	CYAN,
	RED,
	MAGENTA,
	BROWN,
	GREY,
	DARKGREY,
	LIGHTBLUE,
	LIGHTGREEN,
	LIGHTCYAN,
	LIGHTRED,
	LIGHTMAGENTA,
	YELLOW,
	WHITE
} tu_color;

enum TU_key {
	KEY_NOTHING = -1,
	KEY_ESCAPE  = 0,
	KEY_ENTER   = 1,
	KEY_SPACE   = 32,

	KEY_INSERT  = 2,
	KEY_HOME    = 3,
	KEY_PGUP    = 4,
	KEY_DELETE  = 5,
	KEY_END     = 6,
	KEY_PGDOWN  = 7,

	KEY_UP      = 14,
	KEY_DOWN    = 15,
	KEY_LEFT    = 16,
	KEY_RIGHT   = 17,

	KEY_F1      = 18,
	KEY_F2      = 19,
	KEY_F3      = 20,
	KEY_F4      = 21,
	KEY_F5      = 22,
	KEY_F6      = 23,
	KEY_F7      = 24,
	KEY_F8      = 25,
	KEY_F9      = 26,
	KEY_F10     = 27,
	KEY_F11     = 28,
	KEY_F12     = 29,

	KEY_NUMDEL  = 30,
	KEY_NUMPAD0 = 31,
	KEY_NUMPAD1 = 127,
	KEY_NUMPAD2 = 128,
	KEY_NUMPAD3 = 129,
	KEY_NUMPAD4 = 130,
	KEY_NUMPAD5 = 131,
	KEY_NUMPAD6 = 132,
	KEY_NUMPAD7 = 133,
	KEY_NUMPAD8 = 134,
	KEY_NUMPAD9 = 135
};
void tu_init(void);
void tu_shutdown(void);

void tu_hide_cursor(void);
void tu_show_cursor(void);

void tu_clear_screen(void);

void tu_set_pos(int, int);
void tu_put_char(char);
int tu_put_string(char *);
int tu_put_stringf(const char *, ...);

void tu_set_fg_color(enum TU_color);
void tu_set_bg_color(enum TU_color);
void tu_reset_color(void);

int tu_get_key_sync(void);
int tu_get_key_async(void);

void tu_sleep(unsigned int);

void tu_get_term_size(int *, int *);

#ifdef _WIN32
/*#include "textutil_win32.h"*/
#error "WIN32 support not implemented (yet)."
#else
#include "textutil_posix.h"
#endif

static int
vasprintf(char **str, const char *fmt, va_list args)
{
	int size = 0;
	va_list tmpa;

	va_copy(tmpa, args);

	size = vsnprintf(NULL, size, fmt, tmpa);

	va_end(tmpa);

	if (size < 0)
		return -1;

	*str = (char *) malloc(size + 1);

	/* out of memory */
	if(*str == NULL)
		return -1;

	size = vsprintf(*str, fmt, args);
	return size;
}

int
tu_put_stringf(const char *fmt, ...)
{
	char *out;
	va_list ap;
	int ret;
	int len = 0;

	va_start(ap, fmt);
	ret = vasprintf(&out, fmt, ap);
	va_end(ap);

	if (ret > -1) {
		tu_put_string(out);
		len = strlen(out);
		free(out);
	}

	return len;
}

#ifdef __cplusplus
}
#endif

#endif /* __TEXTUTIL_H */

