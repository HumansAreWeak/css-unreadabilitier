#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "minimizer.h"

#define BUF_SZ 8192

struct cmd_args {
	FILE *in, *out;
};

void init_cmd_args(struct cmd_args *args) {
	args->in = stdin;
	args->out = stdout;
}

FILE* process_to_file(FILE *f) {
	int read, written;
	char buf[BUF_SZ];
	FILE *out;
	if (f != stdin && f != stdout) {
		return f;
	}
	out = tmpfile();
	if (out == NULL) {
		return NULL;
	}
	do {
		read = fread(buf, 1, BUF_SZ, f);
		if (read > 0) {
			written = fwrite(buf, 1, read, out);
		}
	} while(read == BUF_SZ && read == written);
	return out;
}

FILE* parse_file(char *path, const char *mode) {
	return fopen(path, mode);
}

int parse_output_file(struct cmd_args *args, char *output) {
	args->out = fopen(output, "w");
	return args->out == NULL;
}

int parse_input_file(struct cmd_args *args, char *input) {
	args->in = fopen(input, "r");
	return args->in == NULL;
}

void print_help_screen() {
	fprintf(stderr, "usage: css-unreadabilitier [-h] [-i] [-o]\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "make your css stylesheet unreadable, by turning it into a one-liner.\n");
}

int main(int argc, char **argv) {
	struct cmd_args args;
	struct css_source src = {0};
	char *buf, *out, *arg;
	int size, read, i;
	int ret = 0;
	init_cmd_args(&args);
	// I don't care about leaking anything in here...
	for (i = 0; i < argc; ++i) {
		arg = argv[i];
		if (arg[0] != '-') {
			continue;
		}
		switch (arg[1]) {
		case 'i':
			if (arg[2] == '\0') {
				++i;
				if (argc < i) {
					print_help_screen();
					return -1;
				}
				ret = parse_input_file(&args, argv[i]);
			} else {
				ret = parse_input_file(&args, &arg[2]);				
			}
			if (ret != 0) {
				fprintf(stderr, "ERROR: could not parse input file: %s\n", strerror(errno));
				return -1;
			}
			continue;
		case 'o':
			if (arg[2] == '\0') {
				++i;
				if (argc < i) {
					print_help_screen();
					return -1;
				}
				ret = parse_output_file(&args, argv[i]);
			} else {
				ret = parse_output_file(&args, &arg[2]);				
			}
			if (ret != 0) {
				fprintf(stderr, "ERROR: could not parse output file: %s\n", strerror(errno));
				return -1;
			}
			continue;
		case 'h':
			print_help_screen();
			return 0;
		default:
			continue;
		}
	}
	args.in = process_to_file(args.in);
	if (args.in == NULL) {
		fprintf(stderr, "ERROR: process to file: %s\n", strerror(errno));
		ret = errno;
		goto error;
	}
	if(fseek(args.in, 0L, SEEK_END) == -1) {
		fprintf(stderr, "ERROR: seek end: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	if((size = ftell(args.in)) == -1) {
		fprintf(stderr, "ERROR: ftell failed: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	if(fseek(args.in, 0L, SEEK_SET) == -1) {
		fprintf(stderr, "ERROR: seek start: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	buf = malloc(size * sizeof(char));
	if(buf == NULL) {
		fprintf(stderr, "ERROR: allocating source buffer: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	out = malloc(size * sizeof(char));
	if(out == NULL) {
		fprintf(stderr, "ERROR: allocating output buffer: %s\n", strerror(errno));
		ret = errno;
		goto error_buf;
	}
	memset(out, 0, size * sizeof(char));
	read = fread(buf, 1, size, args.in);
	if (read != size || read == 0) {
		fprintf(stderr, "ERROR: fread bytes read mismatch. expected to read %d bytes but read %d\n", size, read);		
		ret = -1;
		goto error_out;
	}
	src.begin = buf;
	src.cursor = buf;
	src.end = buf + size;
	minimizer_minimze_css(&src, out);
	fprintf(args.out, "%s", out);
error_out:
	free(out);
error_buf:
	free(buf);
error_close:
	fclose(args.in);
	if (args.out != NULL && args.out != stdout) {
		fclose(args.out);
	}
error:
	return ret;
}
