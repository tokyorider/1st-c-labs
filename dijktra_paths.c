#define _CRT_SECURE_NO_WARNINGS 
#define INFINITY  2147483648
#define test(a, b, c, d) ((a) < 0 || (a) > 5000 || (b) < 1 || (b) > (a) || (c) < 1 || (c) > (a) || (d) > (a) * ((a)-1)/2) ? 0 : 1


#include "stdio.h" 
#include "stdlib.h" 

typedef unsigned int ui;

void create_graph(int num_vertices, int num_edges, int start, int end, FILE* fi, FILE* fo);

_Bool fill_graph(ui** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo);

void free_graph(ui** graph, int num_vertices);

void dijktra_path(ui** graph, int num_vertices, int start, int end, FILE* fo);

int search_paths(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int start, int finish);

int mark_neighbours(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int finish, int l);

void print_paths(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int s, int f, FILE* fo, int l);

int count_paths(ui** graph, _Bool* isv, int num_vertices, int start, int end);

int count(ui** graph, _Bool* isv, int* num_paths, int num_vertices, int vertice);

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


void create_graph(int num_vertices, int num_edges, int start, int finish, FILE* fi, FILE* fo) {
	int i, j;
	ui** graph = (ui**)malloc(sizeof(ui*) * num_vertices);
	if (!graph) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	for (i = 0; i < num_vertices; i++) {
		graph[i] = (ui*)malloc(sizeof(ui) * num_vertices);
		if (!graph[i]) {
			fprintf(fo, "Memory allocation error");
			free_graph(graph, i);
			return;
		}
	}
	for (i = 0; i < num_vertices; i++) {
		for (j = 0; j < num_vertices; j++) graph[i][j] = INFINITY;
		graph[i][i] = 0;
	}
	if (!fill_graph(graph, num_vertices, num_edges, fi, fo)) return;
	dijktra_path(graph, num_vertices, start, finish, fo);
	free_graph(graph, num_vertices);
}


_Bool fill_graph(ui** graph, int num_vertices, int num_edges, FILE* fi, FILE* fo) {
	int i, j;
	long long ln;
	while (!feof(fi) && num_edges-- > 0) {
		fscanf(fi, "%d%d%lld", &i, &j, &ln);
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
	if (num_edges > 0) {
		fprintf(fo, "bad number of lines");
		free_graph(graph, num_vertices);
		return 0;
	}
	return 1;
}


void free_graph(ui** graph, int num_vertices) {
	for (int i = 0; i < num_vertices; i++) free(graph[i]);
	free(graph);
}


void dijktra_path(ui** graph, int num_vertices, int start, int end, FILE* fo) {
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
	int i, f = end - 1, s = start - 1;
	for (i = 0; i < num_vertices; i++) {
		distances[i] = INFINITY;
		isv[i] = 0;
	}
	distances[s] = 0;
	int l = search_paths(graph, isv, distances, parents, num_vertices, s, f);
	print_paths(graph, isv, distances, parents, num_vertices, s, f, fo, l);
	free(isv);
	free(distances);
	free(parents);
}


int search_paths(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int start, int finish) {
	ui min2;
	int min = start, min1 = start, i, n = num_vertices, l = 0;
	long long path;
	while (n-- > 0) {
		isv[min] = 1;
		min2 = INFINITY;
		for (i = 0; i < num_vertices; i++) {
			if (!isv[i]) {
				if (graph[min][i] != INFINITY) {
					path = graph[min][i] + distances[min];
					if (path < (long long)distances[i]) {
						distances[i] = path;
						parents[i] = min;
					}
					if (path >= INFINITY && distances[i] == INFINITY) {
						parents[i] = min;
						isv[i] = 1;
						if (i == finish) l++;
					}
				}
				if (distances[i] < min2 && !isv[i]) {
					min1 = i;
					min2 = distances[i];
				}
			}
		}
		if (min1 == min) {
			l = mark_neighbours(graph, isv, distances, parents, num_vertices, finish, l);
			if (distances[finish] < INFINITY) l = 0;
			return l;
		}
		min = min1;
	}
	if (distances[finish] < INFINITY) l = 0;
	return l;
}


int mark_neighbours(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int finish, int l) {
	int i, j;
	for (i = 0; i < num_vertices; i++) {
		if (distances[i] >= INFINITY && isv[i]) {
			for (j = 0; j < num_vertices; j++) {
				if (graph[i][j] != INFINITY && !isv[j]) {
					isv[j] = 1;
					distances[j] = INFINITY;
					parents[j] = i;
					if (j == finish) l++;
				}
			}
		}
	}
	return l;
}


void print_paths(ui** graph, _Bool* isv, ui* distances, ui* parents, int num_vertices, int s, int f, FILE* fo, int l) {
	int i;
	for (i = 0; i < num_vertices; i++) {
		if (!isv[i]) fprintf(fo, "oo ");
		else if (distances[i] >= INFINITY) fprintf(fo, "INT_MAX+ ");
		else  fprintf(fo, "%d ", distances[i]);
	}
	fprintf(fo, "\n");
	if (!isv[f]) fprintf(fo, "no path");
	else if (distances[f] < INFINITY) {
		for (i = f; i != s; i = parents[i]) fprintf(fo, "%d ", i + 1);
		fprintf(fo, "%d", i + 1);
	}
	else if (l > 1) fprintf(fo, "overflow");
	else {
		l = count_paths(graph, isv, num_vertices, s, f);
		if (l == 2) fprintf(fo, "overflow");
		else if (l == 1) {
			for (i = f; i != s; i = parents[i]) fprintf(fo, "%d ", i + 1);
			fprintf(fo, "%d", i + 1);
		}
		else fprintf(fo, "Memory allocation error");
	}
}


int count_paths(ui** graph, _Bool* isv, int num_vertices, int start, int finish) {
	int* num_paths = (int*)malloc(sizeof(int) * num_vertices), i;
	if (!num_paths) return 0;
	for (i = 0; i < num_vertices; i++) num_paths[i] = isv[i] = 0;
	num_paths[start] = isv[start] = 1;
	count(graph, isv, num_paths, num_vertices, finish);
	return num_paths[finish] > 1 ? 2 : 1;
}


int count(ui** graph, _Bool* isv, int* num_paths, int num_vertices, int vertice) {
	int i, c;
	if (isv[vertice]) return num_paths[vertice];
	else {
		for (i = 0; i < num_vertices; i++) {
			if (graph[vertice][i] != INFINITY && vertice != i) {
				graph[i][vertice] = INFINITY;
				c = count(graph, isv, num_paths, num_vertices, i);
				num_paths[vertice] += c;
				if (c >= 2) return 2;
				isv[i] = 1;
			}
		}
	}
	return c;
}
