#include <bits/stdc++.h>

using namespace std;

typedef long long ll;

#define DEBUG

#ifdef DEBUG
struct
{
	vector <ll> b;
	ll sum(int l, int r)
	{
		ll res(0);
		for (int i(l); i <= r; ++i)
			res += b[i];
		return res;
	}
	void insert(ll x, int pos)
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
	void next_perm(int l, int r)
	{
		next_permutation(b.begin() + l, b.begin() + r + 1);
	}
	void prev_perm(int l, int r)
	{
		prev_permutation(b.begin() + l, b.begin() + r + 1);
	}
} debugger;

ll do_operation(int t, int x, int l, int r, int pos)
{
	if (t == 1) return debugger.sum(l, r);
	if (t == 2) debugger.insert(x, pos);
	if (t == 3) debugger.erase(pos);
	if (t == 4) debugger.assign(x, l, r);
	if (t == 5) debugger.add(x, l, r);
	if (t == 6) debugger.next_perm(l, r);
	if (t == 7) debugger.prev_perm(l, r);
}

#endif

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
	ll res;
	for (int t, x, l, r, pos; q; --q)
	{
		cin >> t;
		if (t == 1 || t == 6 || t == 7) cin >> l >> r;
		else if (t == 3) cin >> pos;
		else if (t == 2) cin >> x >> pos;
		else cin >> x >> l >> r;
#ifdef DEBUG
		res = do_operation(t, x, l, r, pos);
		if (t == 1) cout << res << endl;
#endif
	}
#ifdef DEBUG
	for (ll i : debugger.b) cout << i << ' ';
#endif
}
