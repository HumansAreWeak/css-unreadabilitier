#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "minimizer.h"

#define BUF_SZ 8192

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

int main(int argc, char **argv) {
	FILE *in;
	struct css_source src = {0};
	char *buf, *out;
	int size, read;
	int ret = 0;
	in = process_to_file(stdin);
	if (in == NULL) {
		fprintf(stderr, "ERROR: process to file: %s\n", strerror(errno));
		ret = errno;
		goto error;
	}
	if(fseek(in, 0L, SEEK_END) == -1) {
		fprintf(stderr, "ERROR: seek end: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	if((size = ftell(in)) == -1) {
		fprintf(stderr, "ERROR: ftell failed: %s\n", strerror(errno));
		ret = errno;
		goto error_close;
	}
	if(fseek(in, 0L, SEEK_SET) == -1) {
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
	read = fread(buf, 1, size, in);
	if (read != size || read == 0) {
		fprintf(stderr, "ERROR: fread bytes read mismatch. expected to read %d bytes but read %d\n", size, read);		
		ret = -1;
		goto error_out;
	}
	src.begin = buf;
	src.cursor = buf;
	src.end = buf + size;
	minimizer_minimze_css(&src, out);
error_out:
	free(out);
error_buf:
	free(buf);
error_close:
	fclose(in);
error:
	return ret;
}
