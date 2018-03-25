#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "math.h"
#include "string.h"

typedef unsigned char uc;

int hash(uc* c, int ln);

void search(uc* c, int h);

int main() {
	FILE *fi = fopen("in.txt", "r"), *fo = fopen("out.txt", "w");
	int h, ln;
	if (!fi || !fo) {
		if (fi)
			fclose(fi);
		else if (fo)
			fclose(fo);
		return -1;
	}
	uc c[18];
	fgets(c, 18, fi);
	if (!feof(fi))
		c[strlen(c) - 1] = 0;
	ln = strlen(c);
    h = hash(c, ln);
	search(c, h);
	fclose(fi);
	fclose(fo);
}


int hash(uc* c, int ln) {
	int i, h;
	for (i = 0, h = 0; i < ln; i++)
		h += (c[i] % 3) * pow(3, i);
	return h;
}


void search(uc* c, int h) {
	FILE *fi = fopen("in.txt", "r"), *fo = fopen("out.txt", "w");
	int ln = strlen(c), i, j, hl, first, k, count, n = 2, nlcount;
	int degree = pow(3, ln - 1);
	uc buf[1024];
	fprintf(fo, "%d", h);
	fseek(fi, ln + 2, SEEK_SET);
	count = fread(buf, sizeof(uc), 1024, fi);
	if (count < ln)
		return;
	hl = hash(buf, ln);
	if (hl == h) {
		fprintf(fo, " %d", 1);
		for (i = 0; buf[i] == c[i] && i < ln - 1;)
			fprintf(fo, " %d", ++i + 1);
	}
	while (1) {
		for (i = ln, first = 0; i < count; i++, first++, n++) {
			hl = (hl - buf[first] % 3) / 3 + (buf[i] % 3) * degree;
			if (hl == h) {
				fprintf(fo, " %d", n);
				for (j = first + 1, k = 0; k < ln - 1 && c[k] == buf[j]; j++)
					fprintf(fo, " %d", ++k + n);
			}
		}
		for (i = count - ln, nlcount = 0; i < count; i++) {
			if (buf[i] == '\n')
				nlcount--;
		}
		if (!feof(fi)) {
			fseek(fi, nlcount - ln, SEEK_CUR);
			count = fread(buf, sizeof(uc), 1024, fi);
		}
		else return;
	}
}
