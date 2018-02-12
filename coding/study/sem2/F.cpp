#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

struct node
{
	/* base */
	node *L, *R; //children

	ll x; //data
	int y; //priority
	int sz; //size

	/* assign */
	bool need_assign;

	/* add */
	ll add;

	/* sum */
	ll sum;

	/* permutations */
	bool need_reverse;
	bool no_dec, no_inc; //inc_eq, dec_eq
	ll l, r; //left and right elements of segment

	node(ll x) : x(x)
	{
		L = R = 0;
		y = rand();
		sz = 1;

		need_assign = false;

		add = 0;

		sum = x;

		need_reverse = false;
		no_dec = no_inc = true;
		l = r = x;
	}

	void update_assign(ll new_x)
	{
		l = r = x = new_x;
		sum = new_x * sz;
		add = 0;
		need_assign = true;
		no_dec = no_inc = true;
	}

	void update_add(ll k)
	{
		l += k, r += k;
		x += k;
		sum += k * sz;
		add += k;
	}

	void update_reverse()
	{
		need_reverse ^= 1;
		swap(L, R);
		swap(no_inc, no_dec);
		swap(l, r);
	}

	void push()
	{
		if (need_assign)
		{
			if (L) L->update_assign(x);

			if (R) R->update_assign(x);

			need_assign = false;
		}

		if (need_reverse)
		{
			if (L) L->update_reverse();

			if (R) R->update_reverse();

			need_reverse = false;
		}

		if (add)
		{
			if (L) L->update_add(add);

			if (R) R->update_add(add);

			add = 0;
		}
	}

	void recalc()
	{
		sz = 1;
		l = r = x;
		sum = x;
		no_inc = no_dec = true;

		if (L)
		{
			sz += L->sz;
			sum += L->sum;
			l = L->l;
			no_inc &= L->r >= x && L->no_inc;
			no_dec &= L->r <= x && L->no_dec;
		}

		if (R)
		{
			sz += R->sz;
			sum += R->sum;
			r = R->r;
			no_inc &= R->l <= x && R->no_inc;
			no_dec &= R->l >= x && R->no_dec;
		}
	}
};

typedef node *treap;
typedef pair <treap, treap> ptt;

inline int sz(treap T) { return T ? T->sz : 0; }

ptt split(treap T, int key)
{
	if (!T) return { 0, 0 };

	T->push();

	if (sz(T->L) + 1 <= key)
	{
		ptt tmp(split(T->R, key - 1 - sz(T->L)));

		T->R = tmp.first;
		T->recalc();

		return { T, tmp.second };
	}
	else
	{
		ptt tmp(split(T->L, key));

		T->L = tmp.second;
		T->recalc();

		return { tmp.first, T };
	}
}

treap merge(treap L, treap R)
{
	if (!L || !R) return L ? L : R;

	L->push();
	R->push();

	if (L->y > R->y)
	{
		L->R = merge(L->R, R);
		L->recalc();

		return L;
	}
	else
	{
		R->L = merge(L, R->L);
		R->recalc();

		return R;
	}
}

template <typename T, typename ...arg_list>
T merge(T a, arg_list b) { return merge(a, merge(b...)); }

inline void split_segment(treap T, treap &L, treap &M, treap &R, int l, int r)
{
	ptt tmp1(split(T, r));
	ptt tmp2(split(tmp1.first, l - 1));
	L = tmp2.first;
	R = tmp1.second;
	M = tmp2.second;
}

inline ll sum_query(treap &T, int l, int r)
{
	treap L, M, R;
	split_segment(T, L, R, M, l, r);
	ll res(M->sum);
	T = merge(L, M, R);
	return res;
}

inline void insert_query(treap &T, ll x, int pos)
{
	ptt tmp(split(T, pos - 1));
	T = merge(tmp.first, new node(x), tmp.second);
}

/*
void destruct(treap T)
{
	if (!T) return;
	destruct(T->L);
	destruct(T->R);
	delete T;
}
*/

inline void erase_query(treap &T, int pos)
{
	treap L, M, R;
	split_segment(T, L, M, R, pos, pos);
	T = merge(L, R);
	//destruct(M);
}

inline void assign_query(treap &T, int l, int r, ll x)
{
	treap L, M, R;
	split_segment(T, L, M, R, l, r);
	M->update_assign(x);
	T = merge(L, M, R);
}

inline void add_query(treap &T, int l, int r, ll x)
{
	treap L, M, R;
	split_segment(T, L, M, R, l, r);
	M->update_add(x);
	T = merge(L, M, R);
}

int get_no_inc_suff_len(treap T) // But it calc pref...
{
	int sum(0);
	while (T && !T->no_inc)
	{
		if (!T->R->no_inc)
		{
			sum += sz(T->L) + 1;
			T = T->R;
		}
		else if (T->R->no_inc)
			if (T->x < T->R->l)
			{
				sum += 1 + sz(T->L);
				T = T->R;
			}
			else T = T->L;
	}
	return sum;
}

int get_pos_greater(treap T, int k)
{
	while (true)
		if (T->R && T->R->l > k)
			T = T->R;
		else if (T->x > k)
			return T->x;
		else
			T = T->L;
}

int get_pos_less(treap T, int k)
{
	while (true)
		if (T->R && T->R->l < k)
			T = T->R;
		else if (T->x < k)
			return T->x;
		else
			T = T->L;
}

inline void next_permutation(treap &T, int l, int r)
{
	treap L, M, R;
	split_segment(T, L, M, R, l, r);
	if (M->no_inc)
	{
		M->update_reverse();
		T = merge(L, M, R);
		return;
	}
	int pref_len = get_no_inc_suff_len(M);
	ptt tmp(split(M, pref_len - 1));
}
