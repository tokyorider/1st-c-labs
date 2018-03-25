#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "assert.h"

typedef struct vertex {
	int key;
	char height;
	struct vertex* left;
	struct vertex* right;
} vertex;

vertex* add_elem(vertex* tree, int k);

vertex* create_v(int key);

char height(vertex* tree);

char bf(vertex* tree);

void free_tree(vertex** tree);

vertex* balance(vertex* tree);

vertex* turn_left(vertex* tree);

vertex* turn_right(vertex* tree);

void fixh(vertex* tree);

int main() {
	FILE *fi = fopen("in.txt", "rt"), *fo = fopen("out.txt", "wt");
	if (!fi || !fo) {
		if (fi)fclose(fi);
		if (fo)fclose(fo);
		return 1;
	}
	int n, N;
	vertex* tree = 0;
	fscanf(fi, "%d", &N);
	while (N-- > 0) {
		fscanf(fi, "%d", &n);
		tree = add_elem(tree, n);
	}
	fprintf(fo, "%d", height(tree));
	free_tree(&tree);
	fclose(fi);
	fclose(fo);
	return 0;
}


vertex* add_elem(vertex* tree, int key) {
	if (!tree) tree = create_v(key);
	else if (key <= tree->key) {
		tree->left = add_elem(tree->left, key);
		fixh(tree);
		if (bf(tree) == -2) tree = balance(tree);
	}
	else {
		tree->right = add_elem(tree->right, key);
		fixh(tree);
		if (bf(tree) == 2) tree = balance(tree);
	}
	return tree;
}


void fixh(vertex* tree) {
	char h1 = height(tree->left), h2 = height(tree->right);
	tree->height = max(h1, h2) + 1;
}


vertex* create_v(int key) {
	vertex* tree = (vertex*)malloc(sizeof(vertex));
	assert(tree);
	tree->key = key;
	tree->height = 1;
	tree->left = tree->right = 0;
	return tree;
}


char height(vertex* tree) {
	return !tree ? 0 : tree->height;
}


char bf(vertex* tree) {
	return height(tree->right) - height(tree->left);
}


vertex* balance(vertex* tree) {
	if (bf(tree) < 0) {
		if (bf(tree->left) > 0) tree->left = turn_left(tree->left);
		tree = turn_right(tree);
	}
	else {
		if (bf(tree->right) < 0) tree->right = turn_right(tree->right);;
		tree = turn_left(tree);
	}
	return tree;
}


void free_tree(vertex** tree) {
	if (*tree) {
		free_tree(&((*tree)->left));
		free_tree(&((*tree)->right));
		free(*tree);
	}
}


vertex* turn_left(vertex* tree) {
	vertex* temp = tree->right;
	tree->right = temp->left;
	temp->left = tree;
	fixh(tree);
	fixh(temp);
	return temp;
}


vertex* turn_right(vertex* tree) {
	vertex* temp = tree->left;
	tree->left = temp->right;
	temp->right = tree;
	fixh(tree);
	fixh(temp);
	return temp;
}
