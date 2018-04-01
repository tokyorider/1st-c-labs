#define _CRT_SECURE_NO_WARNINGS
#define CHARSIZE 256
#define MAXLENGTH CHARSIZE - 1 
#define l 1
#define r 2
#define byte 8
#define f_err "File opening error"
#define m_err "Memory allocation error"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"

typedef unsigned int ui;
typedef unsigned char uc;
typedef struct Tree {
	uc sym;
	uc is_sym;
	ui freq;
	struct Tree* left;
	struct Tree* right;
} Tree;
typedef struct List {
	uc sym;
	_Bool is_sym;
	ui freq;
	struct List* next;
	Tree* node;
} List;

void close_files(FILE* a, FILE* b);

void compress(uc* s, FILE* outp);

List* push(List*, ui fr, uc sym);

Tree* build_tree(List* list_head, size_t ln);

List* cpy_t_to_l(Tree* ptr);

Tree* cpy_l_to_t(List* ptr, Tree* left, Tree* right);

void write_codes(Tree* root, int ind, uc[CHARSIZE][MAXLENGTH], uc* code, uc el);

FILE* print(uc[CHARSIZE][MAXLENGTH], uc* s, FILE* outp);

void print_tree(Tree* root, FILE* data);

void free_list(List* ptr);

void decompress(uc* s, FILE* outp);

void recreate_tree(Tree* root, FILE* data);

void free_tree(Tree* root);

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
	else decompress(s, outp);
	free(s);
	close_files(inp, outp);
	return 0;
}


void close_files(FILE* a, FILE* b) {
	if (a) fclose(a);
	if (b) fclose(b);
}


void compress(uc* s, FILE* outp) {
	FILE* data;
	ui frequences[CHARSIZE] = { 0 }, i;
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
	uc codes[256][MAXLENGTH], code[MAXLENGTH];
	if (!root->is_sym) {
		write_codes(root->left, 0, codes, code, l);
		write_codes(root->right, 0, codes, code, r);
	}
	else {
		codes[root->sym][0] = 2;
		codes[root->sym][1] = 0;
	}
	if ((data = print(codes, s, outp))) print_tree(root, data);
	free_list(head);
}


List* push(List* head, ui fr, uc sym) {
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
	tmp1->is_sym = 1;
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
	if (!list_head->next) return list_head->node;
	do {
		root = (Tree*)malloc(sizeof(Tree));
		if (!root) return NULL;
		root->sym = 0;
		root->left = tmp1->node;
		root->is_sym = 0;
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
	el->is_sym = ptr->is_sym;
	return el;
}


Tree* cpy_l_to_t(List* ptr, Tree* left, Tree* right) {
	Tree* el = (Tree*)malloc(sizeof(Tree));
	if (!el) return NULL;
	el->freq = ptr->freq;
	el->sym = ptr->sym;
	el->is_sym = ptr->is_sym;
	el->left = left;
	el->right = right;
	return el;
}


void write_codes(Tree* root, int i, uc codes[CHARSIZE][MAXLENGTH], uc* code, uc el) {
	code[i++] = el;
	code[i] = 0;
	if (root->is_sym) {
		strcpy(codes[root->sym], code);
		return;
	}
	write_codes(root->left, i, codes, code, l);
	write_codes(root->right, i, codes, code, r);
	return;
}


FILE* print(uc codes[CHARSIZE][MAXLENGTH], uc* s, FILE* outp) {
	FILE* data = fopen("tree.txt", "wt");
	if (!data) {
		fprintf(outp, f_err);
		return NULL;
	}
	int bits_c = 0;
	uc code = 0, i;
	for (; *s; s++) {
		i = 0;
		while (codes[*s][i]) {
			for (; codes[*s][i] && bits_c < byte; i++, bits_c++) code = code * 2 + codes[*s][i] - 1;
			if (bits_c == byte) {
			    fwrite(&code, sizeof(uc), 1, outp);
			    bits_c = 0;
			    code = 0;
			}
		}
	}
	if (i) {
		code <<= 8 - i;
		fwrite(&i, sizeof(uc), 1, data);
		fwrite(&code, sizeof(uc), 1, outp);
	}
	return data;
}


void print_tree(Tree* root, FILE* data) {
	if (!root) return;
	fwrite(&(root->is_sym), sizeof(uc), 1, data);
	if (root->is_sym) fwrite(&(root->sym), sizeof(uc), 1, data);
	print_codes(root->left, data);
	print_codes(root->right, data);
}


void free_list(List* ptr) {
	for (List* tmp = ptr->next; tmp; ptr = tmp, tmp = tmp->next) {
		free(ptr->node);
		free(ptr);
	}
	free(ptr->node);
	free(ptr);
}


void decompress(uc* s, FILE* outp) {
	FILE* data = fopen("tree.txt", "rt");
	if (!data) {
		fprintf(outp, f_err);
		return;
	}
	uc num_extra_bits;
	fread(&num_extra_bits, sizeof(uc), 1, data);
}


void free_tree(Tree* root) {
	if (!root) return;
	free_tree(root->left);
	free_tree(root->right);
	free(root);
}
