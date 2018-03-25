#define _CRT_SECURE_NO_WARNINGS 

#include "stdio.h" 
#include "stdlib.h" 
#include "string.h" 
#include "math.h" 
#include "ctype.h" 

long long perevod_v_10_i(int x, char* s);

double perevod_v_10_d(int x, char* s);

int test(char* s, int b2);

void perevod_v_b2(char* s, int b1, int b2);
  
void reverse(char* s);
  

void main() {
	int b1, b2, i;
	char s[52];
	freopen("in.txt", "r", stdin);
	freopen("out.txt", "w", stdout);
	scanf("%d %d\n", &b1, &b2);
	gets(s);
	if (s[0] == '.' || strlen(s) > 13 || b1 < 2 || b1>16 || b2 < 2 || b2>16 || test(s, b1) == 0)
		printf("Bad input");
	else {
		if (b1 != b2) {
			perevod_v_b2(s, b1, b2);
			for (i = 0; i <= strlen(s) - 1; i++)
				printf("%c", s[i]);
		}
		else for (i = 0; i <= strlen(s) - 1; i++)
			printf("%c", s[i]);
	}
}


long long perevod_v_10_i(int b1, char* s) {
	int i = 0, t, k = -1;
	long long n;
	while (i <= strlen(s) - 1) {
		if (s[i] != '.') {
			k++;
			i++;
		}
		else break;
	}
	t = i;
	for (i = n = 0; i < t; i++, k--) {
		if (isdigit(s[i]))
			n = n + (int)(s[i] - '0')*pow(b1, k);
		else if(s[i]>='a' && s[i]<='f')
			n = n + ((int)(s[i] - 'a') + 10)*pow(b1, k);
		else 
			n = n + ((int)(s[i] - 'A') + 10)*pow(b1, k);
	}
	return n;
}


double perevod_v_10_d(int b1, char* s) {
	int i, r, k;
	double m = 0;
	for (i = 0; i <= strlen(s) - 1; i++) {
		if (s[i] != '.')
			continue;
		else break;
	}
	k = i;
	for (i = k + 1, r = -1; i <= strlen(s) - 1; i++, r--) {
		if (isdigit(s[i]))
			m = m + (int)(s[i] - '0')* pow(b1, r);
		else if (s[i] >= 'a' && s[i] <= 'f')
			m = m + ((int)(s[i] - 'a') + 10) * pow(b1, r);
		else 
			m = m + ((int)(s[i] - 'A') + 10) * pow(b1, r);
		}
	return m;
}


void perevod_v_b2(char *s, int b1, int b2) {
	int k, i, e, j;
	char t[48], w[48];
	long long n = perevod_v_10_i(b1, s);
	double m = perevod_v_10_d(b1, s), l, g;
	if (n == 0) {
		t[0] = '0';
		t[1] = '\0';
	}
	else {
		for (e = 0; n != 0; e++) {
			k = n%b2;
			n = floor(n / b2);
			if (k <= 9)
				t[e] = k + '0';
			else t[e] = k - 10 + 'a';
		}
		t[e] = '\0';
		reverse(t);
	}
	g = m;
	for (j = 0; (m != 0) && (j <= 11); j++) {
		m = m*b2;
		l = floor(m);
		m = m - l;
		if (l <= 9)
			w[j] = l + '0';
		else w[j] = l - 10 + 'a';
	}
	w[j] = '\0';
	for (i = 0; i <= strlen(t) - 1; i++)
		s[i] = t[i];
	if (g != 0) {
		s[i] = '.';
		for (j = 0, i += 1; j <= strlen(w); i++, j++)
			s[i] = w[j];
	}
	s[i] = '\0';
	
}


void reverse(char *t) {
	int i, j;
	char c;
	for (i = 0, j = strlen(t) - 1; i<j; i++, j--) {
		c = t[i];
		t[i] = t[j];
		t[j] = c;
	}
}


int test(char *s, int b1) {
	int i, k = 0;
	for (i = 0; i <= strlen(s) - 1; i++) {
		if (k > 1)
			return 0;
		if (s[i] == '.')
			k++;
		if (s[i] == '.' && (i == strlen(s) - 1 || s[i + 1] == '.'))
			return 0;
		if (isdigit(s[i])) {
			if (s[i] - '0' >= b1)
				return 0;
		}
		else if (s[i] != '.') {
			if (s[i] >= 'a' && s[i] <= 'f') {
				if ((s[i] - 'a') + 10 >= b1)
					return 0;
			}
			else if (s[i] - 'A' + 10 >= b1)
				return 0;
		   }
		}
	return 1;
}
