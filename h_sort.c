#define _CRT_SECURE_NO_WARNINGS 

#include "stdio.h" 
#include "stdlib.h" 

void heap_sort(int *a, int right);

void swap(int *a, int i, int j);

void tree(int *a, int right);

void seed(int *a, int right, int i);

int main() {
	int N, i;
	FILE *file = fopen("in.txt", "rt"), *file0 = fopen("out.txt", "wt");
	fscanf(file, "%d", &N);
	int *a = (int*)malloc(sizeof(int)*N);
	if (a == NULL)
		return -1;
	for (i = 0; i < N; i++)
		fscanf(file, "%d", &(a[i]));
	if (N > 1)
		tree(a, N - 1);
	for (i = 0; i < N; i++)
		fprintf(file0, "%d ", a[i]);
	free(a);
	fclose(file);
	fclose(file0);
	return 0;
}


void heap_sort(int *a, int right) {
	seed(a, right, 0);
	swap(a, 0, right);
	if (right > 1) 
		heap_sort(a, right - 1);
}


void tree(int *a, int right) {
	int i;
	for (i = (right + 1) / 2 - 1; i >= 0; i--) 
		seed(a, right, i);
	swap(a, 0, right);
	if (right>1)
		heap_sort(a, right - 1);
}


void swap(int *a, int i, int j) {
		int btw=a[i];
		a[i] = a[j];
		a[j] = btw;
}


void seed(int *a, int right, int i) {
	int m, br1, br2;
	m = i;
    br1 = i * 2 + 1;
	br2 = br1 + 1;
	if (br1 <= right && a[i] < a[br1])
			m = br1;
	if (br2 <= right && a[m] < a[br2])
			m = br2;
	if (m == i)
			return;
	swap(a, i, m);
	i = m;
	seed(a, right, i);
}
