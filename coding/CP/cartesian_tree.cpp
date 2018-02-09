#include <bits/stdc++.h>

using namespace std;

struct node
{
	node *L, *R;
	int x, y, sz;
	node(int x) : L(0), R(0), x(x), y(rand()), sz(1) {}
};

typedef node *treap;

inline int sz(treap T) { return T ? T->sz : 0; }

void recalc(treap T) { T->sz = 1 + sz(T->L) + sz(T->R); }

void merge(treap L, treap R, treap &T)
{
	if (!L or !R) { T = L ? L : R; return; }
	if (L->y > R->y) { T = L; merge(T->R, R, T->R); }
	else { T = R; merge(L, T->L, T->L); }
}

void split(treap T, treap &L, treap &R, int key)
{
	if (!T) { L = R = 0; return; }
	if (T->key < key) { L = T; split(T->R, T->R, R, key); }
	else { R = T; split(T->L, L, T->L, key); }
}

int main()
{

}
