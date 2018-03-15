#define _CRT_SECURE_NO_WARNINGS 
#define infinity -1
#define INFINITY  2147483648
#define test(a, b, c, d) ((a) < 0 || (a) > 5000 || (b) < 1 || (b) > (a) || (c) < 1 || (c) > (a) || (d) > (a)/2 * ((a) - 1))?0:1

#include "stdio.h" 
#include "stdlib.h" 

typedef unsigned int ui;

void create_graph(int num_vertices, int num_edges, int begin, int end, FILE* fi, FILE* fo);

_Bool fill_graph(int** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo);

void free_graph(int** graph, int num_vertices);

void search_paths(int** graph, int num_vertices, int begin, int end, FILE* fo);

int main() {
	FILE* fi = fopen("in.txt", "rt"), *fo = fopen("out.txt", "wt");
	if (!(fi && fo)) {
		if (fi) fclose(fi);
		if (fo) {
			fprintf(fo, "File opening error");
			fclose(fo);
			return 1;
		}
	}
	int N, S, F, M;
	fscanf(fi, "%d%d%d%d", &N, &S, &F, &M);
	if (!test(N, S, F, M)) {
		if (N < 0 || N > 5000) fprintf(fo, "bad number of vertices");
		else if (S < 1 || S > N || F < 1 || F > N) fprintf(fo, "bad vertex");
		else fprintf(fo, "bad number of edges");
		fclose(fi);
		fclose(fo);
		return 0;
	}
	create_graph(N, M, S, F, fi, fo);
	fclose(fi);
	fclose(fo);
	return 0;
}


void create_graph(int num_vertices, int num_edges, int begin, int end, FILE* fi, FILE* fo) {
	int i, j;
	int** graph = (int**)malloc(sizeof(int*) * num_vertices);
	if (!graph) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	for (i = 0; i < num_vertices; i++) {
		graph[i] = (int*)malloc(sizeof(int) * num_vertices);
		if (!graph[i]) {
			fprintf(fo, "Memory allocation error");
			free_graph(graph, i);
			return;
		}
	}
	for (i = 0; i < num_vertices; i++) {
		for (j = 0; j < num_vertices; j++) graph[i][j] = infinity;
		graph[i][i] = 0;
	}
	if(!fill_graph(graph, num_vertices, num_edges, fi, fo)) return;
	search_paths(graph, num_vertices, begin, end, fo);
	free_graph(graph, num_vertices);
}


_Bool fill_graph(int** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo) {
	int i, j;
    long long ln;
	while (!feof(fi) && num_edges-- > 0) {
		fscanf(fi, "%d", &i);
		fscanf(fi, "%d", &j);
		fscanf(fi, "%lld", &ln);
		if (i < 1 || i > num_vertices || j < 1 || j > num_vertices || ln >= infinity || ln < 0) {
			if (ln >= infinity || ln < 0) fprintf(fo, "bad length");
			else fprintf(fo, "bad vertex");
			free_graph(graph, num_vertices);
			return 0;
		}
		i--;
		j--;
		graph[i][j] = graph[j][i] = (ui)ln;
	}
	if (num_edges > -1) {
		fprintf(fo, "bad number of lines");
		free_graph(graph, num_vertices);
		return 0;
	}
	return 1;
}


void free_graph(int** graph, int num_vertices) {
	for (int i = 0; i < num_vertices; i++) free(graph[i]);
	free(graph);
}


void search_paths(int** graph, int num_vertices, int begin, int end, FILE* fo) {
	ui* distances = (ui*)malloc(sizeof(ui) * num_vertices);
	ui* parents = (ui*)malloc(sizeof(ui) * num_vertices);
	_Bool* isv = (_Bool*)malloc(sizeof(_Bool) * num_vertices);
	if (!(isv && distances && parents)) {
		fprintf(fo, "Memory allocation error");
		if (distances) free(distances);
	    if (isv) free(isv);
		if (parents) free(parents);
		return;
	}
	int i, j, min = begin - 1, n = num_vertices, min1, f = end - 1, s = begin - 1;
	ui min2;
	long long path;
	for (i = 0; i < num_vertices; i++) {
		distances[i] = infinity;
		isv[i] = 0;
		parents[i] = INFINITY;
	}
	distances[min] = 0;
	while (n-- > 0) {
		isv[min] = 1;
		min2 = INFINITY;
		for (i = 0; i < num_vertices; i++) {
			if (!isv[i] && graph[min][i] != infinity) {
				path = graph[min][i] + distances[min];
				if (path < (long long)distances[i]) distances[i] = path;
				if (path < (long long)min2) {
					min1 = i;
					min2 = path;
				}
			}
		}
		if (min1 == min) break;
        parents[min1] = min;
	    min = min1;
	}
	for (i = 0; i < num_vertices; i++) {
		if (!isv[i]) fprintf(fo, "oo ");
		else if (distances[i] == infinity) fprintf(fo, "INT_MAX+ ");
		else  fprintf(fo, "%d ", distances[i]);
	}
	fprintf(fo, "\n");
	for (j = f; isv[j] && j != s; j = parents[j]);
	if (j != s) fprintf(fo, "no path");
	else if (distances[f] != infinity) {
		for (j = f; isv[j] && j != s; j = parents[j]) fprintf(fo, "%d ", j + 1);
	}
	else fprintf(fo, "overflow");
	free(isv);
	free(distances);
	free(parents);
}
