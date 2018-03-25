#define _CRT_SECURE_NO_WARNINGS 
#define test(a, b) ((a) > 5000 || (a) < 0 || (b) > ((a) * (a+1))/2 || (b) < 0?0:1)

typedef struct Edge {
	short v1;
	short v2;
	int weight;
} Edge;

#include "stdio.h" 
#include "stdlib.h"
#include "sort.h"

void create_graph(int num_v, int num_e, FILE* inp, FILE* outp);

_Bool fill_graph(Edge* Graph, int num_v, int num_e, FILE* inp, FILE* outp);

_Bool conn(Edge* Graph, int num_v, int num_e, _Bool* isv);

void create_frame(Edge* Graph, int num_v, int num_e, FILE* outp);

short* init_set(int num_v, FILE* outp);

void build(Edge* graph, short* vertices, short* colours, int num_v, FILE* outp);

short fixcolours(short* vertices, short* colours, int num_v, short i, short j);

int main() {
	FILE* inp = fopen("in.txt", "rt"), *outp = fopen("out.txt", "wt");
	if (!(inp && outp)) {
		if (inp) fclose(inp);
		else if (outp) {
			fprintf(outp, "File opening error");
			fclose(outp);
			return 1;
		}
	}
	int N, M;
	if (fscanf(inp, "%d%d", &N, &M) != 2) {
		fprintf(outp, "bad number of lines");
		fclose(inp);
		fclose(outp);
		return 0;
	}
	if (!test(N, M)) {
		if (N < 0 || N > 5000) fprintf(outp, "bad number of vertices");
		else fprintf(outp, "bad number of edges");
		fclose(inp);
		fclose(outp);
		return 0;
	}
	if (N != 1 || M != 0) create_graph(N, M, inp, outp);
	fclose(inp);
	fclose(outp);
	return 0;
}


void create_graph(int num_v, int num_e, FILE* inp, FILE* outp) {
	Edge* Graph = (Edge*)malloc(sizeof(Edge) * num_e);
	int n = num_v;
	if (!Graph) {
		fprintf(outp, "Memory allocation error");
		return;
	}
	if (!fill_graph(Graph, num_v, num_e, inp, outp)) {
		free(Graph);
		return;
	}
	create_frame(Graph, num_v, num_e, outp);
	free(Graph);
}


_Bool fill_graph(Edge* Graph, int num_v, int num_e, FILE* inp, FILE* outp) {
	int i, j, k = 0;
	long long w;
	while (num_e-- > 0) {
		if (fscanf(inp, "%d%d%lld", &i, &j, &w) != 3) {
			fprintf(outp, "bad number of lines");
			return 0;
		}
		if (i < 1 || i-- > num_v || j < 1 || j-- > num_v || w > INT_MAX || w < 0) {
			if (w > INT_MAX || w < 0) fprintf(outp, "bad length");
			else fprintf(outp, "bad vertex");
			return 0;
		}
		Graph[k].v1 = i;
		Graph[k].v2 = j;
		Graph[k++].weight = (int)w;
	}
	return 1;
}


void create_frame(Edge* Graph, int num_v, int num_e, FILE* outp) {
	_Bool* isv = (_Bool*)malloc(sizeof(_Bool) * num_v);
	int i;
	short* vertices;
	if (!isv) {
		fprintf(outp, "Memory allocation error");
		return;
	}
	for (i = 0; i < num_v; i++) isv[i] = 0;
	isv[Graph[0].v1] = 1;
	if (!conn(Graph, num_v, num_e, isv)) {
		fprintf(outp, "no spanning tree");
		return;
	}
	free(isv);
	for (i = (num_e) / 2 - 1; i >= 0; i--) seed(Graph, num_e - 1, i);
	sort(Graph, num_e - 1);
	short* colours = (short*)malloc(sizeof(short) * num_v);
	if (!colours) {
		fprintf(outp, "Memory allocation error");
		return;
	}
	for (i = 0; i < num_v; i++) colours[i] = 1;
	if (vertices = init_set(num_v, outp)) {
		build(Graph, vertices, colours, num_v, outp);
		free(vertices);
	}
	free(colours);
	return;
}


_Bool conn(Edge* Graph, int num_v, int num_e, _Bool* isv) {
	static int counter = 1;
	int i;
	for (i = 0; i < num_e; i++) {
		if (isv[Graph[i].v1] && !isv[Graph[i].v2]) {
			counter++;
			isv[Graph[i].v2] = 1;
			return conn(Graph, num_v, num_e, isv);
		}
		else if (isv[Graph[i].v2] && !isv[Graph[i].v1]) {
			counter++;
			isv[Graph[i].v1] = 1;
			return conn(Graph, num_v, num_e, isv);
		}
	}
	if (counter != num_v) return 0;
	else return 1;
}


short* init_set(int num_v, FILE* outp) {
	short* vertices = (short*)malloc(sizeof(short) * num_v);
	int i;
	if (!vertices) fprintf(outp, "Memory allocation error");
	else {
		for (i = 0; i < num_v; i++) vertices[i] = i;
	}
	return vertices;
}


void build(Edge* Graph, short* vertices, short* colours, int num_v, FILE* outp) {
	int i = 0;
	short v1, v2, max = 0;
	while (max != num_v) {
		v1 = Graph[i].v1;
		v2 = Graph[i].v2;
		if (vertices[v1] != vertices[v2]) {
			fprintf(outp, "%d %d\n", v1 + 1, v2 + 1);
			max = fixcolours(vertices, colours, num_v, v1, v2);
		}
		i++;
	}
}


short fixcolours(short* vertices, short* colours, int num_v, short i, short j) {
	short k, min, max, counter = 0;
	if (colours[i] <= colours[j]) {
		max = vertices[j];
		min = vertices[i];
	}
	else {
		max = vertices[i];
		min = vertices[j];
	}
	for (k = 0; counter < colours[min]; k++) {
		if (vertices[k] == min) {
			vertices[k] = max;
			counter++;
		}
	}
	colours[max] += colours[min];
	return colours[max];
}
