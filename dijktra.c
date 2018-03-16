#define _CRT_SECURE_NO_WARNINGS 
#define infinity -1
#define INFINITY  2147483648
#define test(a, b, c, d) ((a) < 0 || (a) > 5000 || (b) < 1 || (b) > (a) || (c) < 1 || (c) > (a) || (d) > (a) * ((a)-1)/2) ? 0 : 1


#include "stdio.h" 
#include "stdlib.h" 

typedef unsigned int ui;

void create_graph(int num_vertices, int num_edges, int begin, int end, FILE* fi, FILE* fo);

_Bool fill_graph(int** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo);

void free_graph(int** graph, int num_vertices);

void dijktra_path(int** graph, int num_vertices, int begin, int end, FILE* fo);

_Bool search_paths(int** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int start, int end);

int mark_neighbours(int** graph, _Bool* isv, ui* distances, ui* parents, ui i, int num_vertices, ui parent, int counter, int end);

void print_paths(_Bool* isv, ui* distances, ui* parents, int num_vertices, int s, int f, _Bool l, FILE* fo);

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
	dijktra_path(graph, num_vertices, begin, end, fo);
	free_graph(graph, num_vertices);
}


_Bool fill_graph(int** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo) {
	int i, j;
    long long ln;
	while (!feof(fi) && num_edges-- > 0) {
		fscanf(fi, "%d", &i);
		fscanf(fi, "%d", &j);
		fscanf(fi, "%lld", &ln);
		if (i < 1 || i > num_vertices || j < 1 || j > num_vertices || ln >= INFINITY || ln < 0) {
			if (ln >= INFINITY || ln < 0) fprintf(fo, "bad length");
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


void dijktra_path(int** graph, int num_vertices, int begin, int end, FILE* fo) {
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
	int i, f = end - 1, s = begin - 1;
	for (i = 0; i < num_vertices; i++) {
		distances[i] = infinity;
		isv[i] = 0;
		parents[i] = INFINITY;
	}
	distances[s] = 0;
	_Bool l = search_paths(graph, isv, distances, parents, num_vertices, s, f);
	print_paths(isv, distances, parents, num_vertices, s, f, l, fo);
	free(isv);
	free(distances);
	free(parents);
}


_Bool search_paths(int** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int start, int end) {
	ui min2;
	int min = start, min1 = start, i, n = num_vertices, paths_counter = 0;
	long long path;
	while (n-- > 0) {
		isv[min] = 1;
		min2 = INFINITY;
		for (i = 0; i < num_vertices; i++) {
			if (!isv[i]) {
				if (graph[min][i] != infinity) {
					path = graph[min][i] + distances[min];
					if (path < (long long)distances[i] || distances[i] == infinity) distances[i] = (ui)path;
					if (distances[i] >= INFINITY) paths_counter = mark_neighbours(graph, isv, distances, parents, i, num_vertices, min, paths_counter, end);
				}
				if (distances[i] < min2 && distances[i] != infinity) {
					min1 = i;
					min2 = distances[i];
				}
			}
		}
		if (min1 == min) {
			if (paths_counter > 1) return 0;
			return 1;
		}
		parents[min1] = min;
		min = min1;
	}
	return 1;
}


int mark_neighbours(int** graph, _Bool* isv, ui* distances, ui* parents,  ui i, int num_vertices, ui parent, int counter, int end){
	isv[i] = 1;
	parents[i] = parent;
	distances[i] = INFINITY;
	for (int j = 0; j < num_vertices; j++) {
		if (graph[i][j] != infinity) {
			if (j == end) counter++;
			if (distances[j] == infinity) counter = mark_neighbours(graph, isv, distances, parents, j, num_vertices, i, counter, end);
		}
	}
	return counter;
}


void print_paths(_Bool* isv, ui* distances, ui* parents, int num_vertices, int s, int f, _Bool l, FILE* fo) {
	int i;
	for (i = 0; i < num_vertices; i++) {
		if (!isv[i]) fprintf(fo, "oo ");
		else if (distances[i] == INFINITY) fprintf(fo, "INT_MAX+ ");
		else  fprintf(fo, "%d ", distances[i]);
	}
	fprintf(fo, "\n");
	for (i = f; isv[i] && i != s; i = parents[i]);
	if (i != s) fprintf(fo, "no path");
	else if (distances[f] != INFINITY || l) {
		for (i = f; isv[i] && i != s; i = parents[i]) fprintf(fo, "%d ", i + 1);
		fprintf(fo, "%d", s + 1);
	}
	else fprintf(fo, "overflow");
}
