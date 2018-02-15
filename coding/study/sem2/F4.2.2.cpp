#include <iostream>
#include <vector>
#include <cstdlib>

#include <algorithm>

/*
 * 4.0.0
 * split, merge, size recalc, write
 * segmet split, multi merge, insert, erase
 */

/*
 * 4.1.0
 * reverse
 */

/*
 * 4.2.0
 * sum, recalc sum
 */

/*
 * 4.2.1
 * left_value, right_value
 */

/*
 * 4.2.2
 * no_dec_pref_len...
 */

class cartesian_tree
{
private:

	struct node_type
	{
		//4.0.0
		node_type *left_child, *right_child;
		uint32_t size, priority;

		int64_t value;//4.0.0

		//4.1.0
		bool reversed;//4.1.0

		//4.2.0
		int64_t sum; //4.2.0

		//4.2.1
		int64_t left_value, right_value; //4.2.1

		//4.2.2
		uint32_t no_dec_pref, no_dec_suff;
		uint32_t no_inc_pref, no_inc_suff; //4.2.2

		//4.0.0
		node_type(int64_t);
		void push();
		void recalc();
		void recalc_size();//4.0.0

		//4.1.0
		void reverse();
		void reverse_push();//4.1.0

		//4.2.0
		void recalc_sum(); //4.2.0

		//4.2.1
		void recalc_left_right_values(); //4.2.1

		//4.2.2
		void recalc_pref_suff(); //4.2.2
	};

	mutable node_type *root;

	//4.0.0
	uint32_t size(node_type *) const;

	void write_node(node_type *, std::vector <int64_t> &) const;

	void delete_node(node_type *);

	void split(node_type *, node_type *(&), node_type *(&), uint32_t) const;

	void merge(node_type *, node_type *, node_type *(&)) const;//4.0.0

	void merge(node_type *, node_type *, node_type *, node_type *(&)) const;

	void split(node_type *, node_type *(&), node_type *(&), node_type *(&), uint32_t, uint32_t) const;//4.0.0

public:
	//4.0.0
	cartesian_tree(const std::vector <int64_t> &);

	~cartesian_tree();

	void write_tree(std::vector <int64_t> &) const;

	void insert(int64_t, uint32_t);

	void erase(uint32_t);//4.0.0

	//4.1.0
	void reverse(uint32_t, uint32_t); //4.1.0

	//4.2.0
	int64_t sum(uint32_t, uint32_t) const; //4.2.0

	//4.2.1
	void __dbg_get_left_right_values(uint32_t, uint32_t, int64_t &, int64_t &); //4.2.1

	//4.2.2 I hope it 'll work good without debugging
	//void __dbg_get_left_right_lens(uint32_t, uint32_t, int64_t &, int64_t &, int64_t &, int64_t &); //4.2.2
};


//4.0.0-----------------------------------------------------------------------
cartesian_tree::node_type::node_type(int64_t value) :
	left_child(nullptr), right_child(nullptr), //4.0.0
	size(1), priority(rand()), //4.0.0
	value(value), //4.0.0
	reversed(false), //4.1.0
	sum(value), //4.2.0
	left_value(value), right_value(value), //4.2.1
	no_dec_pref(1), no_dec_suff(1), no_inc_pref(1), no_inc_suff(1) //4.2.2
{}

void cartesian_tree::node_type::push()
{
	reverse_push(); //4.1.0
}

void cartesian_tree::node_type::recalc()
{
	recalc_size(); //4.0.0
	recalc_sum(); //4.2.0
	recalc_left_right_values(); //4.2.1
	recalc_pref_suff(); //4.2.2
}

void cartesian_tree::node_type::recalc_size()
{
	size = 1;
	if (left_child != nullptr)
		size += left_child->size;
	if (right_child != nullptr)
		size += right_child->size;
}

cartesian_tree::cartesian_tree(const std::vector<int64_t> &data)
{
	root = nullptr;
	for (int64_t value : data)
		merge(root, new node_type(value), root);
}

cartesian_tree::~cartesian_tree()
{
	delete_node(root);
}

uint32_t cartesian_tree::size(node_type *node) const
{
	return node == nullptr ? 0U : node->size;
}

void cartesian_tree::write_node(node_type *node, std::vector <int64_t> &data) const
{
	if (node == nullptr)
		return;
	node->push();
	write_node(node->left_child, data);
	data.push_back(node->value);
	write_node(node->right_child, data);
}

void cartesian_tree::write_tree(std::vector<int64_t> &data) const
{
	data.clear();
	write_node(root, data);
}

void cartesian_tree::delete_node(node_type *node)
{
	if (node == nullptr)
		return;
	delete_node(node->left_child);
	delete_node(node->right_child);
	delete node;
}

void cartesian_tree::split(node_type *T, node_type *&L, node_type *&R, uint32_t k) const
{
	if (T == nullptr)
	{
		L = R = nullptr;
		return;
	}
	T->push();
	if (size(T->left_child) >= k)
	{
		split(T->left_child, L, T->left_child, k);
		R = T;
	}
	else
	{
		split(T->right_child, T->right_child, R, k - 1 - size(T->left_child));
		L = T;
	}
	T->recalc();
}

void cartesian_tree::merge(node_type *L, node_type *R, node_type *&T) const
{
	if (L == nullptr || R == nullptr)
	{
		T = L == nullptr ? R : L;
		return;
	}
	L->push();
	R->push();
	if (L->priority > R->priority)
	{
		merge(L->right_child, R, L->right_child);
		T = L;
	}
	else
	{
		merge(L, R->left_child, R->left_child);
		T = R;
	}
	T->recalc();
}

void cartesian_tree::split(node_type *T, node_type *&L, node_type *&M, node_type *&R, uint32_t l, uint32_t r) const
{
	split(T, M, R, r);
	split(M, L, M, l - 1);
}

void cartesian_tree::merge(node_type *L, node_type *M, node_type *R, node_type *&T) const
{
	merge(L, M, M);
	merge(M, R, T);
}

void cartesian_tree::insert(int64_t value, uint32_t position)
{
	node_type *L, *R;
	split(root, L, R, position - 1);
	merge(L, new node_type(value), R, root);
}

void cartesian_tree::erase(uint32_t position)
{
	node_type *L, *M, *R;
	split(root, L, M, R, position, position);
	delete_node(M);
	merge(L, R, root);
}
//4.0.0-----------------------------------------------------------------------

//4.1.0-----------------------------------------------------------------------
void cartesian_tree::node_type::reverse()
{
	reversed ^= 1;
	std::swap(left_child, right_child);
	std::swap(left_value, right_value); //4.2.1
	std::swap(no_dec_pref, no_inc_suff); //4.2.2
	std::swap(no_inc_pref, no_dec_suff); //4.2.2
}

void cartesian_tree::node_type::reverse_push()
{
	if (reversed)
	{
		reversed = false;
		if (left_child != nullptr)
			left_child->reverse();
		if (right_child != nullptr)
			right_child->reverse();
	}
}

void cartesian_tree::reverse(uint32_t l, uint32_t r)
{
	node_type *L, *M, *R;
	split(root, L, M, R, l, r);
	M->reverse();
	merge(L, M, R, root);
}
//4.1.0-----------------------------------------------------------------------

//4.2.0-----------------------------------------------------------------------
void cartesian_tree::node_type::recalc_sum()
{
	sum = value;
	if (left_child != nullptr)
		sum += left_child->sum;
	if (right_child != nullptr)
		sum += right_child->sum;
}

int64_t cartesian_tree::sum(uint32_t l, uint32_t r) const
{
	node_type *L, *M, *R;
	split(root, L, M, R, l, r);
	int64_t result(M->sum);
	merge(L, M, R, root);
	return result;
}
//4.2.0-----------------------------------------------------------------------

//4.2.1-----------------------------------------------------------------------
void cartesian_tree::node_type::recalc_left_right_values()
{
	left_value = right_value = value;
	if (left_child != nullptr)
		left_value = left_child->left_value;
	if (right_child != nullptr)
		right_value = right_child->right_value;
}

void cartesian_tree::__dbg_get_left_right_values(uint32_t l, uint32_t r, int64_t &lv, int64_t &rv)
{
	node_type *L, *M, *R;
	split(root, L, M, R, l, r);
	lv = M->left_value;
	rv = M->right_value;
	merge(L, M, R, root);
}
//4.2.1-----------------------------------------------------------------------

//4.2.2-----------------------------------------------------------------------
void cartesian_tree::node_type::recalc_pref_suff()
{//lefts
	if (nullptr != left_child)
	{
		no_dec_pref = left_child->no_dec_pref;
		no_inc_pref = left_child->no_inc_pref;
		if (no_dec_pref == left_child->size && left_child->right_value <= value)
			++no_dec_pref;
		if (no_inc_pref == left_child->size && left_child->right_value >= value)
			++no_inc_pref;
	}
	else no_inc_pref = no_dec_pref = 1;
	if (nullptr != right_child)
	{
		if (size - right_child->size == no_dec_pref && value <= right_child->left_value)
			no_dec_pref += right_child->no_dec_pref;
		if (size - right_child->size == no_inc_pref && value >= right_child->left_value)
			no_inc_pref += right_child->no_inc_pref;
	}
	//now rights
	if (nullptr != right_child)
	{
		no_dec_suff = right_child->no_dec_suff;
		no_inc_suff = right_child->no_inc_suff;
		if (no_dec_suff == right_child->size && value <= right_child->left_value)
			++no_dec_suff;
		if (no_inc_suff == right_child->size && value >= right_child->left_value)
			++no_inc_suff;
	}
	else no_inc_suff = no_dec_suff = 1;
	if (nullptr != left_child)
	{
		if (size - left_child->size == no_dec_suff && left_child->right_value <= value)
			no_dec_suff += left_child->no_dec_suff;
		if (size - left_child->size == no_inc_suff && left_child->right_value >= value)
			no_inc_suff += left_child->no_inc_suff;
	}
}

/*void cartesian_tree::__dbg_get_left_right_lens(uint32_t l, uint32_t r, int64_t &linc, int64_t &rinc, int64_t &ldec, int64_t &rdec)
{
	node_type *L, *M, *R;
	split(root, L, M, R, l, r);
	linc = M->no_dec_pref;
	ldec = M->no_dec_suff;
	rinc = M->no_
}*/
//4.2.2-----------------------------------------------------------------------

#define DEBUG_MODE
void solve()
{
	std::ios_base::sync_with_stdio(false);
	//std::cin.tie(0);
	//std::cout.tie(0);

	uint32_t n;
	std::cin >> n;
	std::vector <int64_t> input(n), output;
	for (int64_t &x : input)
		std::cin >> x;

	cartesian_tree array(input);
#ifdef DEBUG_MODE
	array.write_tree(output);
	for (int64_t x : output)
		std::cout << x << ' ';
	std::cout << '\n';
#endif
	uint32_t q;
	std::cin >> q;
	int64_t x;
	uint32_t t, pos, l, r;
	for (uint32_t counter(0); counter < q; ++counter)
	{
		std::cin >> t;
		if (t == 1)
		{
			std::cin >> l >> r;
			++l, ++r;
			std::cout << array.sum(l, r) << '\n';
		}
		if (t == 2)
		{
			std::cin >> x >> pos;
			++pos;
			array.insert(x, pos);
		}
		if (t == 3)
		{
			std::cin >> pos;
			++pos;
			array.erase(pos);
		}
/*		if (t == 4)
		{
			std::cin >> x >> l >> r;
			++l, ++r;
			array.assign(x, l, r);
		}
		if (t == 5)
		{
			std::cin >> x >> l >> r;
			++l, ++r;
			array.add(x, l, r);
		}
		if (t == 6 || t == 7)
		{
			std::cin >> l >> r;
			++l, ++r;
			if (t == 6)
				array.next_permutation(l, r);
			else
				array.prev_permutation(l, r);
		}*/
#ifdef DEBUG_MODE
		if (t == 8)
		{
			std::cin >> l >> r;
			++l, ++r;
			array.reverse(l, r);
		}
		if (t == 9)
		{
			std::cin >> l >> r;
			++l, ++r;
			int64_t lv, rv;
			array.__dbg_get_left_right_values(l, r, lv, rv);
			std::cout << lv << ' ' << rv << '\n';
		}
		array.write_tree(output);
		for (int64_t a : output)
			std::cout << a << ' ';
		std::cout << '\n';
#endif
	}
	array.write_tree(output);
	for (int64_t a : output)
		std::cout << a << ' ';
	std::cout << '\n';
}
#undef DEBUG_MODE

int main()
{
	solve();
	return 0;
}
