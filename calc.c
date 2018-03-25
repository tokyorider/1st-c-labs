#define _CRT_SECURE_NO_WARNINGS
#define MAXSIZE 1000
#define prior(c) ((c) == '*' || (c) == '/'?3:(c) == '+' || (c) == '-'?2:(c) == '(' || (c) == ')'?1:isdigit((c))?0:-1)

#include "stdio.h"
#include "stdlib.h"
#include "ctype.h"
#include "string.h"
#include "math.h"

typedef struct stack {
	char symbol;
	struct stack* next;
} stack;

int test(char* c);

char* create_line(char* c);

char pop(stack** s);

void push(stack** s, char c);

int calculate(char* c);

int main() {
	FILE *fi = fopen("in.txt", "r"), *fo = fopen("out.txt", "w");
	char c[MAXSIZE];
	int i = 0, result;
	if (!fi || !fo) {
		if (fi)  fclose(fi);
		else if (fo) fclose(fo);
		return -1;
	}
	while (!feof(fi) && i < MAXSIZE) fscanf(fi, "%c", &c[i++]);
	if (c[i - 1] == '\n') c[i - 1] = 0;
	else if (c[i - 2] == '\n') c[i - 2] = 0;
	if(!test(c)) {
		fprintf(fo, "syntax error");
		return 0;
	}
	result = calculate(create_line(c));
	fscanf(fo, "%c", c[0]);
	if (c[0] == 'd') return 0;
	else fprintf(fo, "%d", result);
	fclose(fi);
	fclose(fo);
	return 0;
}


int test(char* c) {
	int i = 0, k = 0, length = strlen(c);
	if (!isdigit(c[0]) && c[0] != '(')
		return 0;
	if (c[0] == '(') k++;
	for (i = 1;i < length;i++){
		if (prior(c[i]) == -1) return 0;
		if (c[i - 1] == '(' && prior(c[i]) > 1) return 0;
		if (prior(c[i - 1]) > 1 && c[i] == ')') return 0;
		if (c[i - 1] == '(' && c[i] == ')') return 0;
		if (prior(c[i - 1]) > 1 && prior(c[i]) > 1) return 0;
		if (c[i] == '(') k++;
		else if (c[i] == ')') k--;
		if (k < 0) return 0;
		}
	if (prior(c[i-1]) > 1 || c[i - 1] == '(') return 0;
	return 1;
}


char* create_line(char* c) {
	int i, j;
	char* a = (char*)malloc(sizeof(char)*(MAXSIZE + 1));
	stack* stack1 = 0;
	for (i = 0, j = 0; c[i] != 0;i++) {
		if (prior(c[i]) == 0) {
			if (a[j] == ',') j++;
			while (prior(c[i]) == 0) a[j++] = c[i++];
			a[j] = ',';
			i--;
		}
		else if (c[i] == '(') push(&stack1, c[i]);
		else if (c[i] == ')') {
			while (stack1->symbol != '(') a[j++] = pop(&stack1);
			pop(&stack1);
		}
		else {
			while (stack1 &&  prior(c[i]) <= prior(stack1->symbol)) a[j++] = pop(&stack1);
			push(&stack1, c[i]);
		}
	}
	while (stack1) a[j++] = pop(&stack1);
    a[j] = 0;
	return a;
}


void push(stack** st, char c) {
	stack* add = (stack*)malloc(sizeof(stack));
	add->symbol = c;
	add->next = *st;
	*st = add;
}


char pop(stack** st) {
	if (!(*st)) return 0;
	char c = (*st)->symbol;
	stack* tmp = *st;
	*st = (*st)->next;
	free(tmp);
	return c;
}


int calculate(char* a) {
	FILE *fo = fopen("out.txt", "w");
	int i = 0, length = strlen(a), t = 0, k, l, m = 0;
	char c;
	char s[12];
	stack* stack2 = 0;
	while (i < length) {
		if (isdigit(a[i])) {
			while (isdigit(a[i])) push(&stack2, a[i++]);
		}
		else if (a[i] != ',') {
			t = 0;
			while (isdigit(c = pop(&stack2))) s[t++] = c;
			--t;
			for (l = 0; t >= 0;t--) l = l * 10 + s[t] - '0';
			if (c == '-') {
				l *= -1;
				pop(&stack2);
			}
			while (isdigit(c = pop(&stack2))) s[++t] = c;
			for (k = 0;t >= 0;t--) k = k * 10 + s[t] - '0';
			if (c == '-') {
				k *= -1;
				pop(&stack2);
			}
			switch (a[i]) {
			case '+': {
				k += l;
				break;
			}
			case '-': {
				k -= l;
				break;
			}
			case '*': {
				k *= l;
				break;
			}
			case '/': {
				if (l == 0) {
					fprintf(fo, "division by zero");
					return 0;
				}
				k /= l;
				break;
			}
			}
			i++;
			if (stack2) push(&stack2, ',');
			if (k == 0) push(&stack2, '0');
			if (k < 0) push(&stack2, '-');
			for (t = 0; k; t++) {
				s[t] = abs(k) % 10 + '0';
				k /= 10;
			}
			t--;
			for (; t >= 0; t--) push(&stack2, s[t]);
			if (isdigit(a[i])) push(&stack2, ',');
		}
		else {
			i++;
			push(&stack2, ',');
		}
	}
	t = 0;
	while(isdigit(c = pop(&stack2))) s[t++] = c;
	--t;
	for (; t >= 0;t--) m = m * 10 + s[t] - '0';
	if (c == '-') m *= -1;
	pop(&stack2);
	free(a);
	return m;
}  
