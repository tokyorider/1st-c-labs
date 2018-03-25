#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1024 
#define CHARSIZE 256

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned char uc;

void search(uc* c, int n);

int main() {
	FILE *fi = fopen("in.txt", "r"), *fo = fopen("out.txt", "w");
	if (!fi || !fo) {
		if (fi) fclose(fi);
		else if (fo) fclose(fo);
		return -1;
	}
	int ln;
	uc c[18];
	fgets(c, 18, fi);
	if (!feof(fi)) c[strlen(c) - 1] = 0;
	ln = strlen(c);
	search(c, ln);
	fclose(fi);
	fclose(fo);
}


void search(uc* c, int ln) {
	FILE *fi = fopen("in.txt", "r"), *fo = fopen("out.txt", "w");
	int shift[CHARSIZE], i, last = ln - 1, n, count, j, k, pos, next = SIZE - ln, nline;
	uc buf[SIZE];
	for (i = 0; i < CHARSIZE; i++) shift[i] = ln;
	for (i = 0, n = last; n > 0; i++, n--) shift[c[i]] = n;
	fseek(fi, ln + 2, SEEK_SET);
	n = ln;
	do {
		i = last;
		count = fread(buf, sizeof(uc), SIZE, fi);
		while (i < count) {
			if (buf[i] != c[last] && i < next) {
				fprintf(fo, "%d ", n);
				n += shift[buf[i]];
				i += shift[buf[i]];
			}
			else if (i < next){
				fprintf(fo, "%d ", n);
				fprintf(fo, "%d ", n - 1);
				for (j = i - 1, k = last - 1, pos = n - 1; j > i - last && buf[j] == c[k]; j--, k--)
					fprintf(fo, "%d ", --pos);
				if (j == i - ln) {
					i++;
					n++;
				}
				else {
					i += shift[c[last]];
					n += shift[c[last]];
				}
			}
			else i = SIZE;
		}
		for (j = next, nline = 0; j < SIZE; j++) {
			if (buf[j] == '\n') nline--;
		}
		fseek(fi, nline - ln, SEEK_CUR);
	} while (count == SIZE);
}
