#define _CRT_SECURE_NO_WARNINGS
#define SIZE 1024
#define CHARSIZE 256
#define f_err "File opening error"
#define m_err "Memory allocation error"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned int ui;
typedef unsigned char uc;
typedef struct Tree {
	uc sym;
	size_t freq;
	struct Tree* left;
	struct Tree* right;
} Tree;
typedef struct List {
	uc sym;
	size_t freq;
	struct List* next;
	Tree* node;
} List;

void close_files(FILE* a, FILE* b);

void compress(uc* s, FILE* outp);

List* push(List*, size_t fr, uc sym);

Tree* build_tree(List* list_head, size_t ln);

List* cpy_t_to_l(Tree* ptr);

Tree* cpy_l_to_t(List* ptr, Tree* l, Tree* r);

_Bool write_codes(Tree* root, int ind, uc[256][24], uc* code, uc el);

void print(uc[256][24], uc* s, FILE* outp);

void free_list(List* ptr);

int main() {
	FILE *inp = fopen("in.txt", "rt"), *outp = fopen("out.txt", "wt");
	if (!(inp && outp)) {
		if (outp) fprintf(outp, f_err);
		close_files(inp, outp);
		return 1;
	}
	fseek(inp, 0, SEEK_END);
	size_t ln = ftell(inp);
	if (ln == 1L) {
		fprintf(outp, "File error");
		close_files(inp, outp);
		return 1;
	}
	fseek(inp, 0, SEEK_SET);
	uc c[2];
	fread(c, sizeof(uc), 2, inp);
	if (c[0] != 'c' && c[0] != 'd' || c[1] != '\n') {
		close_files(inp, outp);
		return 0;
	}
	uc* s = (uc*)malloc(sizeof(uc) * ln);
	if (!s) {
		fprintf(outp, m_err);
		close_files(inp, outp);
	}
	s[fread(s, sizeof(uc), ln, inp)] = 0;
	if (c[0] == 'c' && *s) compress(s, outp);
	free(s);
	close_files(inp, outp);
	return 0;
}


void close_files(FILE* a, FILE* b) {
	if (a) fclose(a);
	if (b) fclose(b);
}


void compress(uc* s, FILE* outp) {
	size_t frequences[CHARSIZE] = { 0 }, i;
	List* head = NULL;
	Tree* root = NULL;
	for (i = 0; s[i]; i++) frequences[s[i]]++;
	size_t j = i;
	for (i = 0; i < CHARSIZE; i++) {
		if (frequences[i]) {
			head = push(head, frequences[i], (uc)i);
			if (!head) {
				fprintf(outp, m_err);
				return;
			}
		}
	}
	if (!(root = build_tree(head, j))) {
		fprintf(outp, m_err);
		free_list(head);
		return;
	}
	uc codes[256][24], code[24];
	if (!codes || !code || !write_codes(root, 0, codes, code, 1)) fprintf(outp, m_err);
	print(codes, s, outp);
	free_list(head);
}


List* push(List* head, size_t fr, uc sym) {
	List* tmp1 = NULL, *tmp2 = NULL;
	if (head && fr >= head->freq) {
		for (tmp1 = head; tmp1->next && fr >= tmp1->next->freq; tmp1 = tmp1->next)
			;
		tmp2 = tmp1->next;
		tmp1->next = (List*)malloc(sizeof(List));
		tmp1 = tmp1->next;
	}
	else if (head && fr < head->freq) {
		tmp1 = (List*)malloc(sizeof(List));
		tmp2 = head;
		head = tmp1;
	}
	else {
		tmp1 = (List*)malloc(sizeof(List));
		head = tmp1;
		tmp2 = NULL;
	}
	if (!tmp1) {
		free_list(head);
		return NULL;
	}
	tmp1->next = tmp2;
	tmp1->freq = fr;
	tmp1->sym = sym;
	tmp1->node = cpy_l_to_t(tmp1, NULL, NULL);
	if (!tmp1->node) {
		free_list(head);
		return NULL;
	}
	return head;
}


Tree* build_tree(List* list_head, size_t ln) {
	Tree *root = NULL;
	List *tmp1 = list_head, *tmp2, *tmp3;
	do {
		root = (Tree*)malloc(sizeof(Tree));
		if (!root) return NULL;
		root->sym = 0;
		root->left = tmp1->node;
		tmp2 = tmp1->next;
		root->right = tmp2->node;
		root->freq = tmp1->freq + tmp2->freq;
		tmp1 = tmp3 = tmp2;
		tmp2 = tmp1->next;
		while (tmp2 && tmp2->freq < root->freq) {
			tmp1 = tmp1->next;
			tmp2 = tmp2->next;
		}
		if (!(tmp1->next = cpy_t_to_l(root))) return NULL;
		tmp1->next->node = root;
		tmp1->next->next = tmp2;
		tmp1 = tmp3->next;
	} while (root->freq != ln);
	return root;
}


List* cpy_t_to_l(Tree* ptr) {
	List* el = (List*)malloc(sizeof(List));
	if (!el) return NULL;
	el->freq = ptr->freq;
	el->sym = ptr->sym;
	return el;
}


Tree* cpy_l_to_t(List* ptr, Tree* left, Tree* right) {
	Tree* el = (Tree*)malloc(sizeof(Tree));
	if (!el) return NULL;
	el->freq = ptr->freq;
	el->sym = ptr->sym;
	el->left = left;
	el->right = right;
	return el;
}


_Bool write_codes(Tree* root, int i, uc codes[256][24], uc* code, uc el) {
	if (!root) return 1;
	code[i++] = el;
	code[i] = 0;
	if (root->sym) {
		strcpy(codes[root->sym], code);
		return 1;
	}
	if (!write_codes(root->left, i, codes, code, 1)) return 0;
	if (!write_codes(root->right, i, codes, code, 2)) return 0;
	return 1;
}


void print(uc codes[256][24], uc* s, FILE* outp) {
	int bits_c = 0, i;
	uc code = 0;
	for (; *s; s++) {
		i = 0;
		while (codes[*s][i]) {
			for (; codes[*s][i] && bits_c < 8; i++, bits_c++) code = code * 2 + codes[*s][i] - 1;
			if (bits_c == 8) {
				fwrite(&code, sizeof(uc), 1, outp);
			    bits_c = 0;
				code = 0;
			}
		}
	}
}


void free_list(List* ptr) {
	for (List* tmp = ptr->next; tmp; ptr = tmp, tmp = tmp->next) {
		free(ptr->node);
		free(ptr);
	}
	free(ptr->node);
	free(ptr);
}
