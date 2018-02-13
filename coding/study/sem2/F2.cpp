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
					right->upd_add(right);
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
				if (!right || (right->sz + 1 == r/dec))
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
}
