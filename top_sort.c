#define _CRT_SECURE_NO_WARNINGS 
#define test(a, b) ((a) > 1000 || (a) < 0 || (b) > ((a) * (a+1))/2 || (b) < 0?0:1)

#include "stdio.h" 
#include "stdlib.h"

typedef struct Vertex {
	short int num_adj_in;
	short int num_adj_out;
	short int* adj;
} Vertex;

void create_graph(int num_vertices, int num_edges, FILE* fi, FILE* fo);

_Bool fill_graph(Vertex* Graph, int num_vertices, int num_edges, FILE* fi, FILE* fo);

void free_graph(Vertex* Graph, int num_vertices);

void top_sort(Vertex* Graph, int num_vertices, FILE* fi, FILE* fo);

void fix_edges(Vertex* Graph, int num_vertices, short int vertex);

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
	int N, M;
	if (fscanf(fi, "%d%d", &N, &M) < 2) {
		fprintf(fo, "bad number of lines");
		fclose(fi);
		fclose(fo);
		return 0;
	}
	if (!test(N, M)) {
		if (N < 0 || N > 1000) fprintf(fo, "bad number of vertices");
		else fprintf(fo, "bad number of edges");
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
	Vertex* Graph = (Vertex*)malloc(sizeof(Vertex) * num_vertices);
	if (!Graph) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	int i;
	for (i = 0; i < num_vertices; i++) {
		Graph[i].num_adj_in = Graph[i].num_adj_out = 0;
		Graph[i].adj = 0;
	}
	if (!fill_graph(Graph, num_vertices, num_edges, fi, fo)) {
		free_graph(Graph, num_vertices);
		return;
	}
	top_sort(Graph, num_vertices, fi, fo);
	free_graph(Graph, num_vertices);
}


_Bool fill_graph(Vertex* Graph, int num_vertices, int num_edges, FILE* fi, FILE* fo) {
	int i, j;
	while (num_edges-- > 0) {
		if (fscanf(fi, "%d%d", &i, &j) < 2) {
			fprintf(fo, "bad number of lines");
			return 0;
		}
		if (i < 1 || j < 1 || i-- > num_vertices || j-- > num_vertices) {
			fprintf(fo, "bad vertex");
			return 0;
		}
		Graph[i].num_adj_out++;
		Graph[j].num_adj_in++;
		Graph[i].adj = realloc(Graph[i].adj, (Graph[i].num_adj_out * sizeof(short int)));
		if (!Graph[i].adj) {
			fprintf(fo, "Memory allocation error");
			return 0;
		}
		(Graph[i].adj)[Graph[i].num_adj_out - 1] = j;
	}
	return 1;
}


void free_graph(Vertex* Graph, int num_vertices) {
	for (int i = 0; i < num_vertices; i++) {
		if (Graph[i].num_adj_out) free(Graph[i].adj);
	}
	free(Graph);
}


void top_sort(Vertex* Graph, int num_vertices, FILE* fi, FILE* fo) {
	short int i = 0, j; 
	short int v_min = -1;
	_Bool* isd = (_Bool*)malloc(sizeof(_Bool) * num_vertices);
	if (!isd) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	for (j = 0; j < num_vertices; j++) isd[j] = 0;
	while (i++ < num_vertices) {
		for (j = 0; j < num_vertices; j++) {
			if (!(Graph[j].num_adj_in || isd[j])) {
				v_min = j;
				fix_edges(Graph, num_vertices, v_min);
				break;
			}
		}
		if (v_min == -1) {
			fclose(fo);
			fo = fopen("out.txt", "wt");
			if (fo) fprintf(fo, "impossible to sort");
			return;
		}
		isd[v_min] = 1;
		fprintf(fo, "%d ", v_min + 1);
		v_min = -1;
	}
	free(isd);
}


void fix_edges(Vertex* Graph, int num_vertices, short int vertex) {
	short int ln = Graph[vertex].num_adj_out, j;
	short int* neighbours = Graph[vertex].adj;
	for (j = 0; j < ln; j++) Graph[neighbours[j]].num_adj_in--;
}
