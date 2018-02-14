#include <iostream>
#include <algorithm>
#include <cstdlib>

namespace cartesian_tree
{

	typedef long long ll;

	struct node
	{
		node *left, *right;
		int sz, y;
		ll lval, val, rval;
		ll sum, add;
		int linc, rinc;
		int ldec, rdec;
		bool assign, rev;

		node() {}

		node(ll x) :
			left(0), right(0),
			sz(1), y(rand()),
			lval(x), val(x), rval(x),
			sum(x), add(0),
			linc(1), rinc(1),
			ldec(1), rdec(1),
			assign(false), rev(false)
		{}

		void upd_add(ll x)
		{
			lval += x;
			val += x;
			rval += x;
			sum += x * sz;
			add += x;
		}

		void upd_rev()
		{
			std::swap(left, right);
			std::swap(lval, rval);
			std::swap(linc, rinc);
			std::swap(ldec, rdec);
			rev ^= 1;
		}

		void upd_assign(ll x)
		{
			lval = val = rval = x;
			sum = x * sz;
			linc = rinc = sz;
			ldec = rdec = sz;
			add = 0;
		}

		void push()
		{
			if (assign)
			{
				if (left)
					left->upd_assign(val);
				if (right)
					right->upd_assign(val);
				assign = false;
			}
			if (rev)
			{
				if (left)
					left->upd_rev();
				if (right)
					right->upd_rev();
				rev = false;
			}
			if (add)
			{
				if (left)
					left->upd_add(add);
				if (right)
					right->upd_add(add);
				add = 0;
			}
		}

		void recalc()
		{
			//sz, vals, sum
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
			linc = ldec = 1;
			if (left)
			{
				linc = left->linc + left->rval <= val;
				ldec = left->ldec + left->rval >= val;
			}
			if (right)
			{
				if (!left || (left->sz + 1 == linc))
					if (val <= right->lval)
						linc += right->linc;
				if (!left || (left->sz + 1 == ldec))
					if (val >= right->lval)
						ldec += right->ldec;
			}
			rinc = rdec = 1;
			if (right)
			{
				rinc = right->rinc + right->lval >= val;
				ldec = right->rdec + right->lval <= val;
			}
			if (left)
			{
				if (!right || (right->sz + 1 == rinc))
					if (val >= left->rval)
						rinc += left->rinc;
				if (!right || (right->sz + 1 == rdec))
					if (val <= left->rval)
						rdec += left->rdec;
			}
		}

		/*
		node *left, *right; children
		int sz, y; size, priority
		ll lval, val, rval; left_value, this_value, right_value
		ll sum, add;
		int lninc, rninc; left_inc, right_inc
		int ldec, rdec;
		bool assign, rev;
		*/
	};

	typedef node *treap;

	struct alloc_type{ //naive memory manager
		node *buff;
		int nxtfr; //next free block
		alloc_type()
		{
			nxtfr = 0;
			buff = new node[1 << 19];
		}
		~alloc_type()
		{
			delete [] buff;
		}
		treap get_new_node(ll x)
		{
			buff[nxtfr] = node(x);
			return &buff[nxtfr++];
		}
	} mem_alloc;

	int sz(treap T) { return T ? T->sz : 0; }

	void split(treap T, treap &L, treap &R, int key)
	{
		if (!T) { L = R = 0; return; }
		T->push();
		if (sz(T->left) + 1 <= key)
		{
			split(T->right, T->right, R, key - sz(T->left) - 1);
			L = T;
		}
		else
		{
			split(T->left, L, T->left, key);
			R = T;
		}
		T->recalc();
	}

	//L = [1..l-1], M = [l, r], R = [r+1, ...]
	inline void split(treap T, treap &L, treap &M, treap &R, int l, int r)
	{
		split(T, T, R, r);
		split(T, L, M, l - 1);
	}

	void merge(treap L, treap R, treap &T)
	{
		if (!L || !R) { T = L ? L : R; return; }
		L->push(), R->push();
		if (L->y > R->y)
		{
			T = L;
			merge(L->right, R, L->right);
		}
		else
		{
			T = R;
			merge(L, R->left, R->left);
		}
		T->recalc();
	}

	inline void merge(treap L, treap M, treap R, treap &T)
	{
		merge(M, R, M);
		merge(L, M, T);
	}

	inline void merge(treap A, treap B, treap C, treap D, treap &T)
	{
		merge(A, B, C, T);
		merge(T, D, T);
	}

	inline void merge(treap A, treap B, treap C, treap D, treap E, treap &T)
	{
		merge(A, B, C, C);
		merge(C, D, E, T);
	}

	inline ll sum_query(treap T)
	{
		return T ? T->sum : 0;
	}

	inline void insert_query(treap &T, ll x, int pos)
	{
		treap L, R;
		split(T, L, R, pos - 1);
		merge(L, mem_alloc.get_new_node(x), R, T);
	}

	inline void erase_query(treap &T, int pos)
	{
		treap L, M, R;
		split(T, L, M, R, pos, pos);
		merge(L, R, T);
	}

	inline void assign_query(treap T, ll x)
	{
		T->upd_assign(x);
	}

	inline void add_query(treap T, ll x)
	{
		T->upd_add(x);
	}

	//T - sorted, this function return index of element
	// for dec
	int find_greater(treap T, ll x)
	{
		T->push();
		if (T->right && T->right->lval > x)
			return sz(T->left) + 1 + find_greater(T->right, x);
		if (T->val > x)
			return 1 + sz(T->left);
		return find_greater(T->left, x);
	}

	// for inc
	int find_less(treap T, ll x)
	{
		T->push();
		if (T->right && T->right->lval < x)
			return sz(T->left) + 1 + find_less(T->right, x);
		if (T->val < x)
			return 1 + sz(T->left);
		return find_less(T->left, x);
	}

	//swap elements L and R[pos]
	void swap(treap &L, treap &R, int pos)
	{
		treap A, B, C;
		split(R, A, B, C, pos, pos);
		merge(A, L, C, R);
		L = B;
	}

	inline void next_permutation(treap &T)
	{
		if (T->rdec == sz(T))
		{
			T->upd_rev();
			return;
		}
		int i(sz(T) - T->rdec);
		treap A, B, C; //pref, [i, i] ,suf
		split(T, A, B, C, i, i);
		int j(find_greater(C, B->val));
		swap(B, C, j);
		C->upd_rev();
		merge(A, B, C, T);
	}

	inline void prev_permutation(treap &T)
	{
		if (T->rinc == sz(T))
		{
			T->upd_rev();
			return;
		}
		int i(sz(T) - T->rinc);
		treap A, B, C;
		split(T, A, B, C, i, i);
		int j(find_greater(C, B->val));
		swap(B, C, j);
		C->upd_rev();
		merge(A, B, C, T);
	}

	inline void query(treap &T, int t, int pos, int l, int r, ll x)
	{
		++l, ++r, ++pos;
		ll res;
		if (t == 2)
			insert_query(T, x, pos);
		else if (t == 3)
			erase_query(T, pos);
		else
		{
			treap A, B, C;
			split(T, A, B, C, l, r);
			if (t == 1) res = sum_query(B);
			if (t == 4) assign_query(B, x);
			if (t == 5) add_query(B, x);
			if (t == 6) next_permutation(B);
			if (t == 7) prev_permutation(B);
			merge(A, B, C, T);
		}
		if (t == 1) std::cout << res << '\n';
	}
}

int main()
{
	using namespace std;
	using cartesian_tree::query;
	using cartesian_tree::node;
	using cartesian_tree::mem_alloc;
	freopen("input.txt", "r", stdin);
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	int n;
	cin >> n;
	node *root = 0;
	for (int a; n; --n)
	{
		cin >> a;
		query(root, 2, root->sz + 1, 0, 0, a);
	}
	int q; cin >> q;
	while (q--)
	{
		int t, pos, l, r;
		long long x;
		cin >> t;
		if (t == 2) cin >> x >> pos;
		else if (t == 3) cin >> pos;
		else if (x == 4 || x == 5) cin >> x >> l >> r;
		else cin >> l >> r;
		query(root, t, pos, l, r, x);
	}
	return 0;
}
