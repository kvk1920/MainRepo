#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <cassert>
#include <map>

#define DEBUG_MODE

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
			rev ^= 1;
		}

		void upd_assign(ll x)
		{
			lval = val = rval = x;
			sum = x * sz;
			linc = rinc = sz;
			ldec = rdec = sz;
			add = 0;
			assign = true;
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
				std::swap(left, right);
				std::swap(lval, rval);
				std::swap(linc, rinc);
				std::swap(ldec, rdec);
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
				linc = left->linc + (left->rval <= val);
				ldec = left->ldec + (left->rval >= val);
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
				rinc = right->rinc + (right->lval >= val);
				rdec = right->rdec + (right->lval <= val);
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

		void dbg_output() const;
	};

	typedef node *treap;

	/*struct alloc_type{ //naive memory manager
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
	} mem_alloc;*/

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
		T->push();
		return T ? T->sum : 0;
	}

	inline void insert_query(treap &T, ll x, int pos)
	{
		treap L, R;
		split(T, L, R, pos - 1);
		merge(L, new node(x) /*mem_alloc.get_new_node(x)*/, R, T);
	}

	inline void erase_query(treap &T, int pos)
	{
		treap L, M, R;
		split(T, L, M, R, pos, pos);
		merge(L, R, T);
	}

	inline void assign_query(treap T, ll x)
	{
		T->push();
		T->upd_assign(x);
	}

	inline void add_query(treap T, ll x)
	{
		T->push();
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
		L->recalc();
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
		int j(find_less(C, B->val));
		swap(B, C, j);
		C->upd_rev();
		merge(A, B, C, T);
	}

	inline void query(treap &T, int t, int pos, int l, int r, ll x)
	{
		if (l > r) std::swap(l, r);
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

	namespace ptr_writer {
		std::map <treap, int> dict;
		int counter = 1;
		std::ostream &operator<<(std::ostream &out, const treap &t)
		{
			if (!dict.count(t))
				dict[t] = counter++;
			return out << dict[t];
		}
	}

#define dbg(a) std::cerr << #a << " = " << a << std::endl
	void node::dbg_output() const {
		using namespace ptr_writer;
		std::cerr << "-------------\n";
		dbg((node *)this);
		dbg(left);
		dbg(right);
		dbg(sz);
		dbg(val);
		dbg(lval);
		dbg(rval);
		dbg(sum);
		dbg(linc);
		dbg(rinc);
		dbg(ldec);
		dbg(rdec);
		std::cerr << "-------------\n";
	}
#undef dbg

	void print(treap T, std::vector <ll> &nums)
	{
		if (!T) return;
		T->push();
#ifdef DEBUG_MODE
		//T->dbg_output();
#endif
		print(T->left, nums);
		nums.push_back(T->val);
		print(T->right, nums);
	}

	struct {
		std::vector <ll> b;
		ll get_sum(int l, int r)
		{
			ll res(0);
			for (int i(l); i <= r; ++i)
				res += b[i];
			return res;
		}
		void insert(ll x, int pos)
		{
			b.push_back(x);
			for (int i(b.size() - 1); i != pos; --i)
				std::swap(b[i], b[i - 1]);
		}
		void erase(int pos)
		{
			for (int i(pos); i < b.size() - 1; ++i)
				std::swap(b[i], b[i + 1]);
			b.pop_back();
		}
		void assign(ll x, int l, int r)
		{
			for (int i(l); i <= r; ++i)
				b[i] = x;
		}
		void add(ll x, int l, int r)
		{
			for (int i(l); i <= r; ++i)
				b[i] += x;
		}
		void next_permutation(int l, int r)
		{
			std::next_permutation(b.begin() + l, b.begin() + r + 1);
		}
		void prev_permutation(int l, int r)
		{
			std::prev_permutation(b.begin() + l, b.begin() + r + 1);
		}
		ll query(int t, int pos, int l, int r, ll x)
		{
			if (l > r) std::swap(l, r);
			if (t == 1) return get_sum(l, r);
			if (t == 2) insert(x, pos);
			if (t == 3) erase(pos);
			if (t == 4) assign(x, l, r);
			if (t == 5) add(x, l, r);
			if (t == 6) next_permutation(l, r);
			if (t == 7) prev_permutation(l, r);
		}
		std::vector <ll> print() { return b; }
	} debugger;
}

int main()
{
	using namespace std;
	using cartesian_tree::query;
	using cartesian_tree::node;
	//using cartesian_tree::mem_alloc;
	using cartesian_tree::print;
	//freopen("input.txt", "r", stdin);

#ifdef DEBUG_MODE
	using cartesian_tree::debugger;
	freopen("debug", "w", stderr);
#endif
	ios_base::sync_with_stdio(false);
	cin.tie(0);
	cout.tie(0);
	int n;
	cin >> n;
	//n = rand() % 10000;
	node *root = 0;
	for (int a; n; --n)
	{
		//cin >> a;
		a = rand() % 25;
#ifdef DEBUG_MODE
		cerr << a << ' ';
		debugger.b.push_back(a);
#endif
		query(root, 2, (root ? root->sz : 0) + 1, 0, 0, a);
	}
	int q; cin >> q;
	//q = rand() % 100000;
	while (q--)
	{
		int t, pos, l, r;
		long long x;
		//cin >> t;
		int sz = cartesian_tree::sz(root);
		t = 1 + rand() % 7;
		if (t == 2) x = static_cast <long long>(rand()) * rand(), pos = rand() % sz;//cin >> x >> pos;
		else if (t == 3) pos = rand() % sz; //cin >> pos;
		else if (t == 4 || t == 5) l = rand() % sz, r = rand() % sz, x = (long long) rand() * rand();//cin >> x >> l >> r;
		else l = rand() % sz, r = rand() % sz;//cin >> l >> r;
		query(root, t, pos, l, r, x);
#ifdef DEBUG_MODE
		static vector <long long> data;
		data.clear();
		debugger.query(t, pos, l, r, x);
		print(root, data);
		cerr << endl << q << endl;
		for (long long x : data) cerr << x << ' ';
		cerr << endl;
		for (long long x : debugger.b) cerr << x << ' ';
		cerr << endl;
		if (data != debugger.b)
		{
			cout << "ERROR!!!" << endl;
			cout << q << endl;
			cout << "type: " << t << endl;

			for (int i(0); i < sz; ++i)
				//if (debugger.b[i] != data[i])
				{
					cout << i << " : " << endl;
					cout << "treap: " << data[i] << endl;
					cout << "debug: " << debugger.b[i] << endl;
					cout << endl;
				}
			return 0;
		}
		cerr << "================\n";
		cout << endl;
#endif
	}
	vector <long long> ans;
	print(root, ans);
	for (long long x : ans)
		cout << x << ' ';
	return 0;
}
