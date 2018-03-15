#define _CRT_SECURE_NO_WARNINGS
#define SIZE 100000
#define CHARSIZE 256

#include "stdio.h"
#include "stdlib.h"

typedef unsigned char uc;
typedef struct tree {
	uc symbol;
	int freq;
	struct tree* left;
	struct tree* right;
	struct tree* next;
}Tree;

void compress(uc* line, FILE* fo);

void decompress(uc* s, FILE* fo);

uc** create_codes(uc** codes, Tree* root, Tree** leafs, int* symbols, FILE* fo);

void print_tree(FILE* data, Tree* root);

void sort(Tree** freq);

void swap(Tree** freq, Tree* el1, Tree* el2);

Tree* create_list(int* arr, int n);

Tree* prev(Tree* head, Tree* elem);

Tree* create_tree(Tree* freq);

void recreate_tree(Tree* root, FILE* data);

void free_tree(Tree* root);

void reverse(uc* arr, int ln);

int main() {
	FILE *fi = fopen("in.txt", "rt"), *fo = fopen("out.txt", "wt");
	if (!fi || !fo) {
		if (fi) fclose(fi);
		if (fo) {
			fprintf(fo, "File opening error");
			fclose(fo);
		}
		return 1;
	}
	uc c, d;
	uc *s = 0;
	int n = 0, count;
	fscanf(fi, "%c", &c);
	fscanf(fi, "%c", &d);
	if (feof(fi)) return 0;
	if (c != 'c' && c != 'd' || d != '\n') {
		fprintf(fo, "Wrong setting");
		return 0;
	}
	while (!feof(fi)) {
		n++;
		s = (uc*)realloc(s, SIZE * n);
		if (!s) {
			fprintf(fo, "Memory allocation error");
			fclose(fi);
			fclose(fo);
			return 1;
		}
		count = fread(s + SIZE * (n - 1), sizeof(char), SIZE, fi);
	}
	*(s + SIZE * (n - 1) + count) = 0;
	if (c == 'c') compress(s, fo);
	else decompress(s, fo);
	free(s);
	fclose(fi);
	fclose(fo);
	return 0;
}


void compress(uc* s, FILE* fo) {
	FILE* data = fopen("tree.txt", "wt");
	if (!data) {
		fprintf(fo, "File opening error");
		return;
	}
	if (!(*s)) return;
	int symbols[CHARSIZE], i = 0, j, k;
	Tree* tmp;
	uc *t = s, code = 0, **codes = (uc*)malloc(sizeof(uc*) * CHARSIZE);
	if (!codes) {
		fprintf(fo, "File opening error");
		fclose(data);
		return;
	}
	for (i = 0; i < CHARSIZE; i++) symbols[i] = 0;
	for (; *t; t++) symbols[*t]++;
	Tree* freq = create_list(symbols, 0), *leafs[CHARSIZE];
	if (!freq) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	sort(&freq);
	for (tmp = freq; tmp; tmp = tmp->next) leafs[tmp->symbol] = tmp;
	Tree* root = create_tree(freq);
	if (!root) {
		fprintf(fo, "Memory allocation error");
		return;
	}
	codes = create_codes(codes, root, leafs, symbols, fo);
	if (!codes) {
		fclose(data);
		free(codes);
		free_tree(root);
	}
	i = 0;
	for (; *s; s++) {
		j = 0;
		while (codes[*s][j]) {
			for (; codes[*s][j] && i < 8; j++, i++) code = code * 2 + codes[*s][j] - 1;
			if (i == 8) {
				fprintf(fo, "%c", code);
				i = 0;
				code = 0;
			}
		}
	}
	if (i) {
		fprintf(fo, "%c", code << (8 - i));
		fprintf(data, "%d\n", 8 - i);
	}
	else fprintf(data, "%d\n", 0);
	fprintf(data, "%d\n", root->symbol);
	print_tree(data, root);
	for (k = 0; k < i; k++) {
		if (symbols[k]) free(codes[k]);
	}
	free_tree(root);
}


Tree* create_list(int* arr, int n) {
	static _Bool is_error = 0;
	Tree* freq;
	while (n++ < CHARSIZE && !arr[n]);
	if (n == CHARSIZE) return 0;
	freq = (Tree*)malloc(sizeof(Tree));
	if (!freq) {
		is_error = 1;
		return 0;
	}
	freq->right = freq->left = 0;
	freq->symbol = n;
	freq->freq = arr[n];
	freq->next = create_list(arr, n);
	if (is_error) return 0;
	return freq;
}


void sort(Tree** freq) {
	Tree* tmp1, *tmp2, *tmp3;
	for (tmp1 = *freq; tmp1->next;) {
		for (tmp2 = tmp1->next; tmp2;) {
			if (tmp1->freq > tmp2->freq) {
				swap(freq, tmp1, tmp2);
				tmp3 = tmp1;
				tmp1 = tmp2;
				tmp2 = tmp3->next;
			}
			else tmp2 = tmp2->next;
		}
		tmp1 = tmp1->next;
	}
}


void swap(Tree** freq, Tree* l1, Tree* l2) {
	Tree *tmp1, *tmp2, *tmp3 = l2->next;
	if (*freq != l1) {
		tmp1 = prev(*freq, l1);
		tmp2 = prev(tmp1, l2);
		if (tmp1->next != tmp2) {
			tmp1->next = l2;
			tmp1->next->next = l1->next;
			l1->next = tmp3;
			tmp2->next = l1;
		}
		else {
			tmp3 = l1;
			tmp1->next = tmp1->next->next;
			tmp3->next = tmp1->next->next;
			tmp1->next->next = tmp3;
		}
	}
	else {
		if ((*freq)->next != l2) {
			tmp2 = prev(*freq, l2);
			tmp2->next = (*freq);
			l2->next = (*freq)->next;
			(*freq)->next = tmp3;
		}
		else {
			l2->next = (*freq);
			(*freq)->next = tmp3;
		}
		(*freq) = l2;
	}
}


Tree* prev(Tree* head, Tree* elem) {
	while (head->next != elem) head = head->next;
	return head;
}


Tree* create_tree(Tree* freq) {
	Tree* root = freq, *tmp1 = freq, *tmp2;
	while (tmp1 && tmp1->next) {
		root = (Tree*)malloc(sizeof(Tree));
		if (!root) return 0;
		root->left = tmp1;
		root->right = tmp1->next;
		root->freq = tmp1->freq + tmp1->next->freq;
		root->symbol = 0;
		tmp2 = root->right;
		tmp1 = root->right->next;
		while (tmp1 && tmp1->freq < root->freq) {
			tmp1 = tmp1->next;
			tmp2 = tmp2->next;
		}
		tmp2->next = root;
		root->next = tmp1;
		tmp1 = root->right->next;
		root->left->next = root->right->next = root;
	}
	return root;
}


uc** create_codes(uc** codes, Tree* root, Tree** leafs, int* symbols, FILE* fo) {
	int i, j, k;
	Tree* tmp;
	for (i = 0; i < CHARSIZE; i++) {
		if (symbols[i]) {
			codes[i] = 0;
			if (!root->symbol) {
				for (tmp = leafs[i], j = 0; tmp->next; tmp = tmp->next, j++) {
					codes[i] = (uc*)realloc(codes[i], j + 1);
					if (!codes[i]) {
						for (k = 0; k < i; k++) {
							if (symbols[k]) free(codes[k]);
						}
						fprintf(fo, "Memory allocation error");
						return 0;
					}
					if (tmp == tmp->next->right) codes[i][j] = 2;
					else codes[i][j] = 1;
				}
				reverse(codes[i], j);
			}
			else {
				codes[i] = (uc*)malloc(sizeof(uc));
				if (!codes[i]) {
					fprintf(fo, "Memory allocation error");
					return 0;
				}
				codes[i][0] = 1;
				codes[i][1] = 0;
				break;
			}
			codes[i][j] = 0;
		}
	}
	return codes;
}

void reverse(uc* arr, int ln) {
	int i, j;
	char tmp;
	for (i = 0, j = ln - 1; i < j; i++, j--) {
		tmp = arr[i];
		arr[i] = arr[j];
		arr[j] = tmp;
	}
}


void print_tree(FILE* data, Tree* root) {
	if (root->left) {
		fprintf(data, "%d %d\n", root->left->symbol, root->right->symbol);
		print_tree(data, root->left);
		print_tree(data, root->right);
	}
}


void decompress(uc* s, FILE* fo) {
	FILE* data = fopen("tree.txt", "rt");
	int num_extra_bits, i = 7;
	uc last_sym;
	Tree* root = (Tree*)malloc(sizeof(Tree)), *tmp;
	if (!data || !root) {
		if (!data) {
			fprintf(fo, "File opening error");
			fclose(data);
		}
		else fprintf(fo, "Memory allocation error");
		return;
	}
	fscanf(data, "%d", &num_extra_bits);
	if (feof(data)) return;
	fscanf(data, "%hhd", &(root->symbol));
	recreate_tree(root, data);
	tmp = root;
	while (*(s + 1)) {
		if (tmp->symbol) {
			fprintf(fo, "%c", tmp->symbol);
			tmp = root;
			continue;
		}
		if (*s & (1 << i)) tmp = tmp->right;
		else tmp = tmp->left;
		i--;
		if (i < 0) {
			s++;
			i = 7;
		}
	}
	last_sym = *s >> num_extra_bits;
	i = 7 - num_extra_bits;
	while (i >= 0) {
		if (tmp->symbol) {
			fprintf(fo, "%c", tmp->symbol);
			tmp = root;
			if (root->symbol) i--;
			continue;
		}
		if (last_sym & (1 << i)) tmp = tmp->right;
		else tmp = tmp->left;
		i--;
	}
	if (tmp->symbol && !root->symbol) fprintf(fo, "%c", tmp->symbol);
	fclose(data);
	free_tree(root);
}


void recreate_tree(Tree* root, FILE* data) {
	if (root->symbol) {
		root->left = root->right = 0;
		return;
	}
	root->left = (Tree*)malloc(sizeof(Tree));
	root->right = (Tree*)malloc(sizeof(Tree));
	fscanf(data, "%hhd", &(root->left->symbol));
	fscanf(data, "%hhd", &(root->right->symbol));
	recreate_tree(root->left, data);
	recreate_tree(root->right, data);
}


void free_tree(Tree* root) {
	if (root) {
		free_tree(root->left);
		free_tree(root->right);
		free(root);
	}
}
