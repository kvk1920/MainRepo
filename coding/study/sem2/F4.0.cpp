#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <vector>

namespace tree
{
	typedef long long ll;

	struct node
	{
		node *left, *right;
		int y, sz;
		ll lval, val, rval, new_val;
		ll sum, add;
		bool assigned, reversed;
		int linc, rinc;
		int ldec, rdec;
		node(ll x) :
			y(rand()), sz(1),
			lval(x), val(x), rval(x),
			sum(x), add(0),
			assigned(false), reversed(false),
			linc(1), rinc(1),
			ldec(1), rdec(1)
		{}

		void add_x(ll x)
		{
			lval += x;
			val += x;
			rval += x;
			sum += x * sz;
			add += x;
		}

		void assign_x(ll x)
		{
			lval = val = rval = new_val = x;
			sum = x * sz;
			add = 0;
			assigned = true;
			linc = rinc = ldec = rdec = sz;
		}

		void reverse()
		{
			std:swap(left, right);
			std::swap(lval, rval);
			std::swap(linc, rinc);
			std::swap(rdec, rdec);
			reversed ^= 1;
		}

		void push()
		{
			if (assigned)
			{
				if (left) left->assign_x(new_val);
				if (right) right->assign_x(new_val);
				assigned = false;
			}
			if (add)
			{
				if (left) left->add_x(add);
				if (right) right->add_x(add);
				add = 0;
			}
			if (reversed)
			{
				reverse = 0;
				if (left) left->reverse();
				if (right) right->reverse();
			}
		}

		void recalc()
		{
			push();
			sz = 1;
			lval = rval = val;
			sum = val;
			if (left)
			{
				sz += left->sz;
				lval = left->lval;
				sum += left->sum;
			}
			if (right)
			{
				sz += right->sz;
				rval = right->rval;
				sum += right->sum;
			}
			if (left)
			{
				linc = left->linc;
				if (left->sz == linc && val >= left->rval)
					++linc;
				ldec = left->ldec;
				if (left->sz == ldec && val <= left->rval)
					++ldec;
			} else linc = ldec = 1;
			if (right)
			{
				if (linc == sz - right->sz && right->lval >= val)
					linc += right->linc;
				if (ldec == sz - right->sz && right->lval <= val)
					ldec += right->ldec;
			}
			if (right)
			{
				rinc = right->rinc;
				if (right->sz == rinc && val <= right->lval) ++rinc;
				rdec = right->rdec;
				if (right->sz == rdec && val >= right->lval) ++rdec;
			} else rdec = rinc = 1;
			if (left)
			{
				if (rinc == sz - left->sz && left->rval <= val)
					rinc += left->rinc;
				if (rdec == sz - left->sz && left->rval >= val)
					rdec += left->rdec;
			}
			/*int y, sz;
			ll lval, val, rval, new_val;
			ll sum, add;
			bool assigned, reversed;
			int linc, rinc;
			int ldec, rdec;*/
		}
	};

	typedef node *treap;

	inline int sz(treap T) { return T ? T->sz : 0; }

	void split(treap T, treap &L, treap &R, int k)
	{
		if (!T) { L = R = 0; return; }
		T->push();
		if (sz(T->left) + 1 <= k)
		{
			split(T->right, T->right, R, k - 1 - sz(T->left));
			L = T;
		}
		else
		{
			split(T->left, L, T->left, k);
			R = T;
		}
		T->recalc();
	}

	inline void split(treap T, treap &L, treap &M, treap &R, int l, int r)
	{
		split(T, M, R, r);
		split(M, L, M, l - 1);
	}

	void merge(treap L, treap R, treap &T)
	{
		if (!L || !R) { T = L ? L : R; if (T) T->push(); return; }
		L->push();
		R->push();
		if (L->y >= R->y)
		{
			merge(L->right, R, L->right);
			T = L;
		}
		else
		{
			merge(L, R->left, R->left);
			T = R;
		}
		T->recalc();
	}

	inline void merge(treap L, treap M, treap R, treap &T)
	{
		merge(L, M, M);
		merge(M, R, T);
	}

	inline void reverse_query(treap &T)
	{
		T->reverse();
	}

	ll sum_query(treap &T)
	{
		return T->sum;
	}

	inline void insert_query(treap &T, ll x, int pos)
	{
		treap L, R;
		split(T, L, R, pos - 1);
		merge(L, new node(x), R, T);
	}

	inline void erase_query(treap &T, int pos)
	{
		treap L, D, R;
		split(T, L, D, R, pos, pos);
		merge(L, R, T);
	}

	inline void add_query(treap T, ll x)
	{
		T->add_x(x);
	}

	inline void assign_query(treap T, ll x)
	{
		T->assign_x(x);
	}

	int find_greater_pos(treap T, ll x)
	{
		T->push();
		if (T->right && T->right->lval > x)
			return sz(T->left) + 1 + find_greater_pos(T->right, x);
		if (T->val > x)
			return sz(T->left) + 1;
		return find_greater_pos(T->left, x);
	}

	int find_less_pos(treap T, ll x)
	{
		T->push();
		if (T->right && T->right->lval < x)
			return sz(T->left) + 1 + find_less_pos(T->right, x);
		if (T->val < x)
			return 1 + sz(T->left);
		return find_less_pos(T->left, x);
	}

	inline void swap(treap &L, treap &R, int pos)
	{
		treap A, B, C;
		split(R, A, B, C, pos, pos);
		merge(A, L, C, R);
		L = B;
	}


}
