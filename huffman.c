#define _CRT_SECURE_NO_WARNINGS
#define LINESIZE 100000
#define CHARSIZE 256
#define MAXLENGTH CHARSIZE - 1
#define l 1
#define r 2
#define byte 8
#define f_err "\nFile opening error. Please, make sure that file names are correct and files are located in this directory.\n"
#define m_err "\nMemory allocation error.\n"
#define f_err_occ "\nA file error has occured.\n"
#define help "\nExamples of input: \nhuffman.exe setting file1 file2\nor\nhuffman.exe -h.\nSettings:\n-c to compress file1 into file2;\n-d to decompress file1 into file2.\
\nPrint -h to display help.\n"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

typedef unsigned char uc;
typedef struct Tree {
	uc sym;
	uc is_sym;
	size_t freq;
	struct Tree* left;
	struct Tree* right;
} Tree;
typedef struct List {
	uc sym;
	_Bool is_sym;
	size_t freq;
	struct List* next;
	Tree* node;
} List;

void close_files(FILE* a, FILE* b);

void compress(FILE* inp, FILE* outp, int argc);

List* push(List*, size_t fr, uc sym);

Tree* build_tree(List* list_head, size_t ln);

List* cpy_t_to_l(Tree* ptr);

Tree* cpy_l_to_t(List* ptr, Tree* left, Tree* right);

void write_codes(Tree* root, int ind, uc[CHARSIZE][MAXLENGTH], uc[MAXLENGTH], uc el);

void print(uc[CHARSIZE][MAXLENGTH], uc* s, FILE* inp, FILE* outp);

void print_tree(Tree* root, FILE* outp, uc[byte], size_t ln);

void free_list(List* ptr);

void decompress(FILE* inp, FILE* outp, int argc);

FILE* recreate_tree(FILE* inp, Tree* node);

void free_tree(Tree* root);

int main(int argc, char* argv[]) {
	FILE *inp, *outp;
	if (argc > 1) {
		if (argc != 4 || argv[1][0] != '-' || argv[1][1] != 'c' && argv[1][1] != 'd' || argv[1][2] != '\0') {
			if (argc < 4 && (argc != 2 || argv[1][0] == '-' && (argv[1][1] == 'c' || argv[1][1] == 'd') && argv[1][2] == '\0')) printf("\nToo few arguments.\n");
			else if (argc > 4 || argv[1][1] == 'h' && argv[1][0] == '-' && argv[1][2] == '\0' && argc > 2) printf("\nToo many arguments.\n");
			else if (argv[1][0] != '-' || argv[1][1] != 'c' && argv[1][1] != 'd' && argv[1][1] != 'h' || argv[1][2] != '\0') printf("\nWrong setting.\n");
			printf(help);
		}
		else if (!strcmp(argv[2], argv[3])) printf("\nYou can't use the same file for input and output.\n");
		else {
			inp = fopen(argv[2], "rb");
			outp = fopen(argv[3], "wb");
			if (!(inp && outp)) {
				if (outp) printf(f_err);
				close_files(inp, outp);
				return 1;
			}
			if (argv[1][1] == 'c') compress(inp, outp, argc);
			else decompress(inp, outp, argc);
			clock_t time = clock();
			if (time == -1) printf("Time calculation error.");
			else printf("Execution time: %.4lf seconds.", (((double)time) / ((double)CLOCKS_PER_SEC)));
		}
		return 0;
	}
	inp = fopen("in.txt", "rb");
    outp = fopen("out.txt", "wb");
	if (!(inp && outp)) {
		if (outp) fprintf(outp, f_err);
		close_files(inp, outp);
		return 1;
	}
	uc c[3];
	fread(c, sizeof(uc), 3, inp);
	if (c[0] != 'c' && c[0] != 'd' || c[1] != '\r' || c[2] !='\n') {
		close_files(inp, outp);
		return 0;
	}
	fseek(inp, 0, SEEK_END);
	size_t ln = ftell(inp);
	fseek(inp, 3, SEEK_SET);
	if (ln != 1L && ln != 3) {
		if (c[0] == 'c') compress(inp, outp, argc);
		else decompress(inp, outp, argc);
	}
	else if (ln == 1L) fprintf(outp, "file_err_occ");
	close_files(inp, outp);
	return 0;
}


void close_files(FILE* a, FILE* b) {
	if (a) fclose(a);
	if (b) fclose(b);
}


void compress(FILE* inp, FILE* outp, int argc) {
	size_t i, ln = 0, count, frequences[CHARSIZE] = { 0 };
	List* head = NULL;
	Tree* root = NULL;
	uc* s = (uc*)malloc(sizeof(uc) * LINESIZE);
	if (!s) {
		if (argc == 1) fprintf(outp, m_err);
		else printf(m_err);
		return;
	}
	count = fread(s, sizeof(uc), LINESIZE, inp);
	while (count) {
		for (i = 0; i < count; i++) frequences[s[i]]++;
		ln += count;
		count = fread(s, sizeof(uc), LINESIZE, inp);
	}
	if (argc == 1) fseek(inp, 3, SEEK_SET);
	else fseek(inp, 0, SEEK_SET);
	for (i = 0; i < CHARSIZE; i++) {
		if (frequences[i]) {
			head = push(head, frequences[i], (uc)i);
			if (!head) {
				fprintf(outp, m_err);
				return;
			}
		}
	}
	if (!(root = build_tree(head, ln))) {
		if (argc == 1) fprintf(outp, m_err);
		else printf(m_err);
		free_list(head);
		free(s);
		return;
	}
	uc codes[CHARSIZE][MAXLENGTH], code[MAXLENGTH], syms[byte];
	if (!root->is_sym) {
		write_codes(root->left, 0, codes, code, l);
		write_codes(root->right, 0, codes, code, r);
	}
	else {
		codes[root->sym][0] = 2;
		codes[root->sym][1] = 0;
	}
    print_tree(root, outp, syms, ln);
	print(codes, s, inp, outp);
	if (argc > 1) {
		fseek(inp, 0, SEEK_END);
		fseek(outp, 0, SEEK_END);
		i = ftell(inp);
		ln = ftell(outp);
		if (i == 1L || ln == 1L) printf("Calculation error. But compressing is ok :)");
		else printf("Size before compressing: %zd bytes.\nSize after compressing: %zd bytes.\nCompressing ratio: %.3lf.\n", i, ln, ((double)ln) / ((double)i));
	}
	free_list(head);
}


List* push(List* head, size_t fr, uc sym) {
	List* tmp1 = NULL, *tmp2 = NULL;
	if (head && fr >= head->freq) {
		for (tmp1 = head; tmp1->next && fr >= tmp1->next->freq; tmp1 = tmp1->next) ;
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


void write_codes(Tree* root, int i, uc codes[CHARSIZE][MAXLENGTH], uc code[MAXLENGTH], uc el) {
	code[i++] = el;
	code[i] = '\0';
	if (root->is_sym) {
		strcpy(codes[root->sym], code);
		codes[root->sym][i] = '\0';
		return;
	}
	write_codes(root->left, i, codes, code, l);
	write_codes(root->right, i, codes, code, r);
}


void print(uc codes[CHARSIZE][MAXLENGTH], uc* s, FILE* inp, FILE* outp) {
	uc code = 0, i, bits_c = 0;
	size_t count = fread(s, sizeof(uc), LINESIZE, inp), j;
	while (count) {
		for (j = 0; j < count; j++) {
			i = 0;
			while (codes[s[j]][i]) {
				for (; codes[s[j]][i] && bits_c < byte; i++, bits_c++) code = (code << 1) + codes[s[j]][i] - 1;
				if (bits_c == byte) {
					fwrite(&code, sizeof(uc), 1, outp);
					bits_c = 0;
					code = 0;
				}
			}
		}
		count = fread(s, sizeof(uc), LINESIZE, inp);
	}
	if (bits_c) {
		bits_c = byte - bits_c;
		code <<= bits_c;
		fwrite(&code, sizeof(uc), 1, outp);
	}
	fwrite(&(bits_c), sizeof(uc), 1, outp);
}


void print_tree(Tree* root, FILE* outp, uc s[byte], size_t ln) {
	static uc code = 0, counter = 0, syms_counter = 0;
	if (counter == byte) {
		fwrite(&code, sizeof(uc), 1, outp);
		if (syms_counter) {
			s[syms_counter] = '\0';
			fwrite(s, sizeof(uc), syms_counter, outp);
		}
		code = counter = syms_counter = 0;
	}
	code = code | (root->is_sym << (7 - counter));
	counter++;
	if (root->is_sym) {
		s[syms_counter++] = root->sym;
		if (root->freq == ln) {
			s[syms_counter] = '\0';
			fwrite(&code, sizeof(uc), 1, outp);
			fwrite(s, sizeof(uc), syms_counter, outp);
		}
		return;
	}
	print_tree(root->left, outp, s, ln);
	print_tree(root->right, outp, s, ln);
	if (root->freq == ln && counter) {
		s[syms_counter] = '\0';
		fwrite(&code, sizeof(uc), 1, outp);
		fwrite(s, sizeof(uc), syms_counter, outp);
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


void decompress(FILE* inp, FILE* outp, int argc) {
	uc num_extra_bits, degrees[byte], *s = (uc*)malloc(sizeof(uc) * LINESIZE);
	if (!s) {
		if (argc == 1) fprintf(outp, m_err);
		else printf(m_err);
		return;
	}
	size_t j = 0, count, ln;
    int i;
	fseek(inp, -1, SEEK_END);
	ln = ftell(inp) - 1;
	fread(&num_extra_bits, sizeof(uc), 1, inp);
	if (argc == 1) fseek(inp, 3, SEEK_SET);
	else fseek(inp, 0, SEEK_SET);
	Tree* root = (Tree*)malloc(sizeof(Tree)), *tmp = root;
	if (!root) {
		if (argc == 1) fprintf(outp, m_err);
		else printf(m_err);
		free(s);
		return;
	}
	inp = recreate_tree(inp, root);
	ln -= ftell(inp);
	if (!inp) {
		free_tree(root);
		free(s);
		if (argc == 1) fprintf(outp, m_err);
		else printf(m_err);
		return;
	}
	count = fread(s, sizeof(uc), LINESIZE, inp);
	if (root->is_sym) {
		for (ln = ln * 8 + (byte - num_extra_bits); j < ln; j++) fwrite(&(root->sym), sizeof(uc), 1, outp);
		free(s);
		free_tree(root);
		return;
	}
	for (i = 0; i < byte; i++) degrees[i] = 1 << i;
	while (count) {
		while (j < ln && count-- > 0) {
			i = 7;
			while (i >= 0) {
				if (tmp->is_sym) {
					fwrite(&(tmp->sym), sizeof(uc), 1, outp);
					tmp = root;
				}
				if (s[j % LINESIZE] & degrees[i--]) tmp = tmp->right;
				else tmp = tmp->left;
			}
			j++;
		}
		count = fread(s, sizeof(uc), LINESIZE, inp);
	}
	s[j % LINESIZE] >>= num_extra_bits;
	i = byte - num_extra_bits;
	while (i-- >= 0) {
		if (tmp->is_sym) {
			fwrite(&(tmp->sym), sizeof(uc), 1, outp);
			tmp = root;
		}
	    if (s[j % LINESIZE] & degrees[i]) tmp = tmp->right;
		else tmp = tmp->left;
	}
	free_tree(root);
	free(s);
}


FILE* recreate_tree(FILE* inp, Tree* node) {
	static uc code, counter = 0;
	if (counter == byte) counter = 0;
	if (!counter) fread(&code, sizeof(uc), 1, inp);
	if (code & (1 << (7 - counter++))) {
		node->is_sym = 1;
		fread(&(node->sym), sizeof(uc), 1, inp);
		node->left = node->right = NULL;
		return inp;
	}
	else node->is_sym = 0;
	node->left = (Tree*)malloc(sizeof(Tree));
	if (!node->left) return NULL;
	inp = recreate_tree(inp, node->left);
	node->right = (Tree*)malloc(sizeof(Tree));
	if (!node->right) return NULL;
	inp = recreate_tree(inp, node->right);
	if (!(node->left && node->right)) {
		free_tree(node);
		return NULL;
	}
	return inp;
}


void free_tree(Tree* root) {
	if (!root) return;
	free_tree(root->left);
	free_tree(root->right);
	free(root);
}
