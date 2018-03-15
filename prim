#define _CRT_SECURE_NO_WARNINGS 

#include "stdio.h" 
#include "stdlib.h" 
#include "assert.h" 
#include "limits.h" 

void create_graph(int num_vertices, int num_edges, FILE* fi, FILE* fo);

void frame(unsigned int** graph, int num_vertices, FILE* fo);

_Bool conn(unsigned int** graph, int num, _Bool* s, int n);

int fixkeys(unsigned int* keys, unsigned int* parents, unsigned int** graph, int num_v, int num, _Bool* isv);

int main() {
	FILE *fi = fopen("in.txt", "rt"), *fo = fopen("out.txt", "wt");
	if (!fi || !fo) {
		if (fi) fclose(fi);
		else if (fo) fclose(fo);
		return 1;
	}
	int N, M;
	fscanf(fi, "%d", &N);
	if (N < 0 || N > 5000) {
		fprintf(fo, "bad number of vertices");
		fclose(fi);
		fclose(fo);
		return 0;
	}
	fscanf(fi, "%d", &M);
	if (M < 0 || M > N*(N + 1) / 2) {
		fprintf(fo, "bad number of edges");
		fclose(fi);
		fclose(fo);
		return 0;
	}
	create_graph(N, M, fi, fo);
	fclose(fi);
	fclose(fo);
	return 0;
}


void create_graph(int num_vertices, int num_edges, FILE* fi, FILE* fo) {
	unsigned int infinity = INT_MAX + 1;
	int i, j;
	long long ln = 0;;
	unsigned int** graph = (unsigned int**)malloc(sizeof(int*) * num_vertices);
	assert(graph);
	for (i = 0; i < num_vertices; i++) {
		graph[i] = (unsigned int*)malloc(sizeof(int) * num_vertices);
		if (!graph[i]) {
			for (j = 0; j < i; j++) free(graph[i]);
			free(graph);
			return;
		}
	}
	for (i = 0; i < num_vertices; i++) {
		for (j = 0; j < num_vertices; j++) graph[i][j] = infinity;
	}
	while (!feof(fi) && num_edges-- > 0) {
		fscanf(fi, "%d", &i);
		fscanf(fi, "%d", &j);
		fscanf(fi, "%lld", &ln);
		if (i < 1 || i > num_vertices || j < 1 || j > num_vertices) {
			fprintf(fo, "bad vertex");
			for (i = 0; i < num_vertices; i++) free(graph[i]);
			free(graph);
			return;
		}
		if (ln > INT_MAX || ln < 0) {
			fprintf(fo, "bad length");
			for (i = 0; i < num_vertices; i++) free(graph[i]);
			free(graph);
			return;
		}
		i--;
		j--;
		graph[i][j] = graph[j][i] = (unsigned int)ln;
	}
	if (num_edges > -1) {
		fprintf(fo, "bad number of lines");
		for (i = 0; i < num_vertices; i++) free(graph[i]);
		free(graph);
		return;
	}
	frame(graph, num_vertices, fo);
	for (i = 0; i < num_vertices; i++) free(graph[i]);
	free(graph);
}


void frame(unsigned int** graph, int num_vertices, FILE* fo) {
	if (!num_vertices) {
		fprintf(fo, "no spanning tree");
		return;
	}
	_Bool* isv = (_Bool*)malloc(sizeof(_Bool) * num_vertices);
	assert(isv);
	int i, n = num_vertices - 1, ind_min, j;
	for (i = 1; i < num_vertices; i++) isv[i] = 0;
	isv[0] = 1;
	if (!conn(graph, num_vertices, isv, 0)) {
		fprintf(fo, "no spanning tree");
		return;
	}
	for (i = 0; i < num_vertices; i++) isv[i] = 0;
	isv[0] = 1;
	unsigned int min, infinity = INT_MAX + 1;
	unsigned int* keys = (unsigned int*)malloc(sizeof(int) * num_vertices),
		*parents = (unsigned int*)malloc(sizeof(int)*num_vertices);
	if (!keys && parents) free(parents);
	else if (keys && !parents) free(keys);
	assert(keys && parents);
	for (i = 1; i < num_vertices; i++) keys[i] = infinity;
	keys[0] = ind_min = 0;
	while (n-- > 0) {
		ind_min = fixkeys(keys, parents, graph, num_vertices, ind_min, isv);
		fprintf(fo, "%d %d\n", parents[ind_min] + 1, ind_min + 1);
		isv[ind_min] = 1;
	}
	free(isv);
	free(keys);
	free(parents);
}


_Bool conn(unsigned int** graph, int num_v, _Bool* isv, int num) {
	static unsigned int infinity = INT_MAX + 1;
	static int counter = 1;
	_Bool l = 0;
	if (counter == num_v) return 1;
	for (int i = 0; i < num_v && !l; i++) {
		if (graph[num][i] != infinity && !isv[i]) {
			isv[i] = 1;
			counter++;
			l = conn(graph, num_v, isv, i);
		}
	}
	return l;
}



int fixkeys(unsigned int* keys, unsigned int* parents, unsigned int** graph, int num_v, int num, _Bool* isv) {
	unsigned int min = INT_MAX + 1, ind_min;
	for (int i = 0; i < num_v; i++) {
		if (graph[num][i] < keys[i] && !isv[i]) {
			keys[i] = graph[num][i];
			parents[i] = num;
		}
		if (keys[i] < min && !isv[i]){
			ind_min = i;
			min = keys[i];
		}
	}
	return ind_min;
}
