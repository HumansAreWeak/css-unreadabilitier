#ifndef MINIMIZER_H_
#define MINIMIZER_H_

struct css_source {
	char *begin;
	char *end;
	char *cursor;
};

int minimizer_minimze_css(struct css_source *src, char *out);

#endif // MINIMIZER_H_
