#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"

int test(char *a, int i);

void swap(char *a, int i, int j);

int create_perm(char *a, int c);

int main() {
	int i, N, c, l = 1;
	char a[12];
	FILE *f = fopen("in.txt", "r"), *fp = fopen("out.txt", "w");
	if (!f || !fp) {
		if (fp) fclose(fp);
		else if (f) fclose(f);
		return 11;
	}
	fgets(a, 12, f);
	if (!a) return 2;
	a[strlen(a) - 1] = '\0';
	c = strlen(a);
	for (i = 0; i < c; i++) {
		if (!isdigit(a[i]) || !test(a, i)) {
			fprintf(fp, "bad input");
			return;
		}
	}
	fscanf(f, "%d", &N);
	l = create_perm(a,c);
	for (i = 0; i < N && l;i++) {
		fprintf(fp, "%s\n", a);
		l = create_perm(a, c);
     }
	fclose(f);
	fclose(fp);
	return 0;
}


int test(char *a, int i) {
	int j;
	for (j = i+1; j < strlen(a); j++) {
		if (a[j] == a[i]) return 0;
	}
	return 1;
}


void swap(char *a, int i, int j) {
	char tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}

	
int create_perm(char *a, int c) {
	int  i = 0, k, j, min;
	for (k = c-2 ; k >= 0 && a[k] > a[k + 1]; k--) ;
	if (k == -1) return 0;
	min = k + 1;
	for (j = k + 1; j < c - 1; j++) {
		if (a[j] > a[j + 1] && a[j + 1] > a[k]) min = j + 1;
	}
	swap(a, k, min);
	for (i = k + 1; i < c - 1; i++) {
		for (j = i + 1; j < c; j++) {
			if (a[i] > a[j]) swap(a, i, j);
		}
	}
	return 1;
}
