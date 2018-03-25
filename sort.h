void seed(Edge* a, int right, int i);

void swap(Edge* a, int i, int j);

void sort(Edge *a, int right) {
	seed(a, right, 0);
	swap(a, 0, right);
	if (right > 1) sort(a, right - 1);
}


void tree(Edge *a, int right) {
	int i;
	for (i = (right + 1) / 2 - 1; i >= 0; i--) seed(a, right, i);
	swap(a, 0, right);
	if (right>1) sort(a, right - 1);
}


void swap(Edge *a, int i, int j) {
	Edge tmp = a[i];
	a[i] = a[j];
	a[j] = tmp;
}


void seed(Edge *a, int right, int i) {
	int m, br1, br2;
	m = i;
	br1 = i * 2 + 1;
	br2 = br1 + 1;
	if (br1 <= right && a[i].weight < a[br1].weight) m = br1;
	if (br2 <= right && a[m].weight < a[br2].weight) m = br2;
	if (m == i) return;
	swap(a, i, m);
	i = m;
	seed(a, right, i);
}
