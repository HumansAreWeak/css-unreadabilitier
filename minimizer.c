#include "minimizer.h"

struct out_buffer {
	char *buf;
	int i;
};

static void buf_write_byte(struct out_buffer *buf, char byte) {
	buf->buf[buf->i++] = byte;
}

static inline char convert_whitespace_to_space(char c) {
	switch (c) {
	case '\t':
	case '\n':
	case '\r':
	case '\f':
		return ' ';
	default:
		return c;
	}
}

static inline char peek_next(struct css_source *src) {
	if ((src->cursor + 1) < src->end) {
		return convert_whitespace_to_space(*(src->cursor + 1));
	}
	return '\0';
}

static inline char peek_current(struct css_source *src) {
	return convert_whitespace_to_space(*src->cursor);
}

static char fetch_current(struct css_source *src) {
	char c = peek_current(src);
	++src->cursor;
	return c;
}

static inline int in_bounds(struct css_source *src) {
	return src->cursor < src->end;
}

static int is_comment_begin(struct css_source *src) {
	return peek_current(src) == '/' && peek_next(src) == '*';
}

static int is_comment_end(struct css_source *src) {
	return peek_current(src) == '*' && peek_next(src) == '/';
}

static int is_space(struct css_source *src) {
	return peek_current(src) == ' ';
}

static int is_string_char(char c) {
	switch (c) {
	case '"':
	case '\'':
		return 1;
	default:
		return 0;
	}
}

static int is_string(struct css_source *src) {
	return is_string_char(peek_current(src));
}

static void skip_to_last_space(struct css_source *src) {
	while (peek_next(src) == ' ') {
		++src->cursor;
	}
}

static void skip_all_spaces(struct css_source *src) {
	while (peek_current(src) == ' ') {
		++src->cursor;
	}
}

static void buf_read_string(struct out_buffer *buf, struct css_source *src) {
	char c = fetch_current(src);;
	buf_write_byte(buf, c);
	do {
		c = fetch_current(src);;
		buf_write_byte(buf, c);
	} while(!is_string_char(c));
}

int minimizer_minimze_css(struct css_source *src, char *out) {
	struct out_buffer buf;
	buf.buf = out;
	buf.i = 0;
	while (in_bounds(src)) {
		
		if (is_comment_begin(src)) {
			do {
				++src->cursor;
			} while(in_bounds(src) && !is_comment_end(src));
			src->cursor += 2;
			skip_all_spaces(src);
			continue;
		}

		if (is_space(src)) {
			skip_to_last_space(src);
		}

		if (is_string(src)) {
			buf_read_string(&buf, src);
			continue;
		}

		buf_write_byte(&buf, fetch_current(src));
	}
	return 0;
}
