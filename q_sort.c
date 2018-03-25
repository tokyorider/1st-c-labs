#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"

#define median(x, y, z) (((x) >= (y)) && ((x) <= (z))||((x)<=(y))&&(x>=(z))?1:0)

void qsort_my(int* a, int b, int c);

void swap(int* a, int i, int j);

void main() {
	int N, i;
	FILE *file, *file0;
	file = fopen("in.txt", "rt");
	file0 = fopen("out.txt", "wt");
	fscanf(file, "%d", &N);
	int *a = (int*)malloc(sizeof(int)*N);
	for (i = 0; i < N; i++) fscanf(file, "%d", &(a[i]));
	for (i = 0; i < N - 1; i++) {
		if (a[i] > a[i + 1]) {
			qsort_my(a, 0, N - 1);
			break;
		}
	}
	for (i = 0; i < N ;i++) fprintf(file0, "%d ", a[i]);
	free(a);
	fclose(file);
	fclose(file0);
}


void qsort_my(int* a, int left, int right) {
	int i, last = left, med, mid;
	if (left >= right) return;
    mid = (left + right) / 2;
	if (median(a[left], a[mid], a[right])) med = left;
	else if (median(a[mid], a[left], a[right])) med = mid;
	else med = right;
	swap(a, left, med);
	for (i = last + 1; i <= right; i++) {
		if (a[i] < a[left]) swap(a, ++last, i);
	}
	swap(a, left, last);
	qsort_my(a, left, last - 1);
	qsort_my(a, last + 1, right);
}


void swap(int *a, int i, int j) {
	int tmp;
	tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}
