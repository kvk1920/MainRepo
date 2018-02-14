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
		ll lval, val, rval;
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
			lval = val = rval = x;
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
	};
}
