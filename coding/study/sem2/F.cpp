#include <vector>
#include <iostream>
#include <algorithm>

typedef long long int64;

#define DEBUG

#ifdef DEBUG
struct
{
	std::vector <int64> b;
	int64 sum(int l, int r)
	{
		int64 res(0);
		for (int i(l); i <= r; ++i)
			res += b[i];
		return res;
	}
	void insert(int64 x, int pos)
	{
		b.push_back(0);
		for (int i(b.size() - 1); i > pos; --i)b[i] = b[i - 1];
		b[pos] = x;
	}
	void erase(int pos)
	{
		for (int i(pos); i < b.size() - 1; ++i)
			b[i] = b[i + 1];
		b.pop_back();
	}
	void assign(int64 x, int l, int r)
	{
		for (int i(l); i <= r; ++i)
			b[i] = x;
	}
	void add(int64 x, int l, int r)
	{
		for (int i(l); i <= r; ++i)
			b[i] += x;
	}
	void next_perm(int l, int r)
	{
		std::next_permutation(b.begin() + l, b.begin() + r + 1);
	}
	void prev_perm(int l, int r)
	{
		std::prev_permutation(b.begin() + l, b.begin() + r + 1);
	}
	int64 debug(int t, int x, int l, int r, int pos)
	{
		if (t == 1) return debugger.sum(l, r);
		if (t == 2) debugger.insert(x, pos);
		if (t == 3) debugger.erase(pos);
		if (t == 4) debugger.assign(x, l, r);
		if (t == 5) debugger.add(x, l, r);
		if (t == 6) debugger.next_perm(l, r);
		if (t == 7) debugger.prev_perm(l, r);
	}
} debugger;
#endif

namespace casrtesian_tree {

	struct node
	{
		//ptrs
		node *Left, *Right;
		//base information
		int sz, y;
		//elements
		int64 lval, val, rval;
		//statistic
		bool no_dec, no_inc;
		int64 sum;
		//for update
		int64 add;
		bool assign;
		bool rev;

		node(ll x) :
			Left(0), Right(0),
			sz(1), y(rand()),
			lval(x), val(x), rval(x),
			no_dec(true), no_inc(true),
			sum(x),
			add(0),
			assign(false),
			rev(false)
		{}

		void upd_add(int64 x)
		{
			lval += x, val += x, rval += x;
			sum += sz * x;
			add += x;
		}

		void upd_assign(int64 x)
		{
			lval = val = rval = x;
			no_inc = no_dec = true;
			sum = sz * x;
			add = 0;
			assign = true;
		}

		void upd_rev()
		{
			std::swap(Left, Right);
			std::swap(lval, rval);
			std::swap(no_dec, no_inc);
			rev ^= true;
		}

		void recalc()
		{
			sz = 1;
			lval = rval = val;
			no_dec = no_inc = true;
			sum = val;

			if (Left)
			{
				sz += Left->sz;
				lval = Left->lval;
				no_dec &= Left->no_dec && Left->rval <= val;
				no_inc &= Left->no_inc && Left->rval >= val;
				sum += Left->sum;
			}

			if (Right)
			{
				sz += Right->sz;
				rval = Right->rval;
				no_dec &= Right->no_dec && Right->lval >= val;
				no_inc &= Right->no_inc && Right->lval <= val;
				sum += Right->sum;
			}
		}

		void push()
		{
			if (add)
			{
				if (Left)
					Left->upd_add(add);
				if (Right)
					Right->upd_add(add);
				add = 0;
			}

			if (assign)
			{
				if (Left)
					Left->upd_assign(val);
				if (Right)
					Right->upd_assign(val);
				assign = false;
			}

			if (rev)
			{
				if (Left)
					Left->upd_rev();
				if (Right)
					Right->upd_rev();
				rev = false;
			}
		}
	};

	typedef node *treap;

	void __debug_dfs(treap T, std::vector <int64> &result)
	{
		if (!T) return;
		__debug_dfs(T->Left, result);
		result.push_back(T->val);
		__debug_dfs(T->Right, result);
	}

	inline int sz(treap T) { return T ? T->sz : 0; }

	//split T to L = [1..pref_end], R = [pref_end + 1, ...]
	void split(treap T, treap &L, treap &R, int pref_end)
	{
		if (!T) { L = R = 0; return; }

		T->push();

		if (sz(T->Left) + 1 <= pref_end)
		{
			split(T->Right, T->Right, R, pref_end - sz(T->Left) - 1);
			L = T;
		}
		else
		{
			split(T->Left, L, T->Left, pref_end);
			R = T;
		}

		T->recalc();
	}

	inline void get_segment(treap T, treap &L, treap &M, treap &R, int l, int r)
	{
		split(T, M, R, r);
		split(M, L, M, l - 1);
	}

	//merge L, R to T
	void merge(treap L, treap R, treap &T)
	{
		if (!L || !R) { T = L ? L : R; return; }

		L->push(), R->push();

		if (L->y > R->y)
		{
			merge(L->Right, R, L->Right);
			T = L;
		}
		else
		{
			merge(L, R->Left, R->Left);
			T = R;
		}

		T->recalc();
	}

	inline void merge(treap A, treap B, treap C, treap &T)
	{
		merge(A, B, T);
		merge(T, C, T);
	}

	void swap(treap &T, int i, int j)
	{
		if (i > j) std::swap(i, j);
		treap A, B, C, D, E;
		//A - B - C - D - E -> A - D - C - B - E
		get_segment(T, A, B, C, i, i);
		get_segment(C, C, D, E, j, j);
		merge(C, B, E, C);
		merge(A, D, C, T);
	}

	int no_inc_len(treap T)
	{
		int pref(0);
		while (true)

	}

	void next_permutation(treap &T)
	{
		if (T->no_inc)
		{
			T->upd_rev();
			return;
		}

	}

	inline void operation(treap &root, int t, int64 x, int pos, int l, int r)
	{
		if (t == 1 || t >= 4)
		{
			treap L, M, R;
			get_segment(root, L, M, R, l + 1, r + 1);
			if (t == 1)
				std::cout << M->sum << '\n';
			if (t == 4)
				M->upd_assign(x);
			if (t == 5)
				M->upd_add(x);
			if (t == 6)
				next_permutation(M);
			if (t == 7)
				prev_permutation(M);
			root = merge(L, M, R);
		}
		if (t == 2)
		{
			treap L, R;
			split(root, L, R, pos);
			merge(L, new node(x), R, root);
		}
		if (t == 3)
		{
			treap L, M, R;
			get_segment(root, L, M, R, pos + 1, pos + 1);
			delete M;
			merge(L, R, root);
		}
	}
}

//------------------------------

int main()
{
#ifdef DEBUG
	freopen("debug", "w", stdout);
#else
	freopen("output.txt", "w", stdout);
#endif
	freopen("input.txt", "r", stdin);
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	int n;
	cin >> n;
	int input[n];
	for (int i(0); i < n; ++i) cin >> input[i];
#ifdef DEBUG
	for (int i(0); i < n; ++i)
		debugger.b.push_back(input[i]);
#endif
	int q;
	cin >> q;
	int64 res;
	for (int t, x, l, r, pos; q; --q)
	{
		cin >> t;
		if (t == 1 || t == 6 || t == 7) cin >> l >> r;
		else if (t == 3) cin >> pos;
		else if (t == 2) cin >> x >> pos;
		else cin >> x >> l >> r;
#ifdef DEBUG
		res = debugger.debug(t, x, l, r, pos);
		if (t == 1) cout << res << endl;
#endif
	}
#ifdef DEBUG
	for (int64 i : debugger.b) cout << i << ' ';
#endif
}
