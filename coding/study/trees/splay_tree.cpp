#include <iostream>
#include <vector>
#include <algorithm>

template <typename key_type, class comparator_type = std::less <key_type> >
class splay_tree
{
public:
	comparator_type compare;

	enum direction_type : bool { left_direction, right_direction };

	struct node_type
	{
		node_type *left, *right, *parent;
		key_type key;

		node_type(const key_type &key) :
			left(nullptr),
			right(nullptr),
			parent(nullptr),
			key(key)
		{}

		node_type(key_type &&key) :
			left(nullptr),
			right(nullptr),
			parent(nullptr),
			key(key)
		{}
	};

	splay_tree() :
		root_(nullptr)
	{}

	splay_tree(const splay_tree <key_type, comparator_type> &tree)
	{
		root_ = deep_copy(tree.root_);
	}

	splay_tree(splay_tree <key_type, comparator_type> &&tree) :
		root_(tree.root_)
	{
		tree.root_ = nullptr;
	}

	~splay_tree()
	{
		destruct_(root_);
	}

	bool empty() const { return nullptr == root_; }

	void clear()
	{
		destruct_(root_);
		root_ = nullptr;
	}

	const key_type &minimum() const
	{
		node_type *node = root_;
		while (nullptr != node && nullptr != node->left)
			node = node->left;
		splay_(node);
		if (root_ != nullptr)
			return root_->key;
	}

	const key_type &maximum() const
	{
		node_type *node = root_;
		while (nullptr != node && nullptr != node->right)
			node = node->right;
		splay_(node);
		if (root_ != nullptr)
			return root_->key;
	}

	void merge(splay_tree <key_type, comparator_type> &right_tree)
	{
		root_ = merge_(right_tree.root_);
		right_tree.root_ = nullptr;
	}

	void merge(splay_tree <key_type, comparator_type> &&right_tree)
	{
		root_ = merge(right_tree.root_);
		right_tree.right_ = nullptr;
	}

	bool count(const key_type &key)
	{
		if (empty())
			return false;
		find_key_(key);
		return eq_(key, root_->key);
	}

	void split(splay_tree <key_type, comparator_type> &right_tree, const key_type &key)
	{
		std::pair <node_type *, node_type *> result(split_(key));
		right_tree.clear();
		right_tree.root_ = result.second;
		root_ = result.first;
	}

	void insert(const key_type &key)
	{
		if (count(key))
			return;
		std::pair <node_type *, node_type *> tmp(split_(key));
		root_ = tmp.first;
		root_ = merge_(new node_type(key));
		root_ = merge_(tmp.second);
	}

	void __cout_tree() const
	{
		static std::vector <key_type> data;
		data.clear();
		__write_tree(root_, data);
		std::cout << "{";
		if (!data.empty())
		{
			std::cout << ' ' << data[0];
			for (int i(1); i < data.size(); ++i)
				std::cout << ", " << data[i];
		}
		std::cout << " }" << std::endl;
	}

	void erase(const key_type &key)
	{
		if (empty() || !count(key))
			return;
		find_key_(key);
		node_type *deleted = root_;
		if (nullptr == deleted->left)
		{
			root_ = root_->right;
			if (nullptr != root_)
				root_->parent = nullptr;
			delete deleted;
		}
		else
		{
			root_ = root_->left;
			root_->parent = nullptr;
			if (deleted->right != nullptr)
				deleted->right->parent = nullptr;
			merge_(deleted->right);
			delete deleted;
		}
	}

private:
	mutable node_type *root_;

	bool eq_(const key_type &first_key, const key_type &second_key) const
	{
		return !compare(first_key, second_key) && !compare(second_key, first_key);
	}

	direction_type direction_(node_type *const node) const
	{
		if (node->parent != nullptr && node->parent->left == node)
			return left_direction;
		return right_direction;
	}

	void __write_tree(node_type *node, std::vector <key_type> &target) const
	{
		if (node == nullptr)
			return;
		__write_tree(node->left, target);
		target.push_back(node->key);
		__write_tree(node->right, target);
	}

	void destruct_(node_type *node)
	{
		if (node == nullptr)
			return;
		destruct_(node->left);
		destruct_(node->right);
		delete node;
	}

	node_type *deep_copy_(node_type *const node, node_type *const parent = nullptr) const
	{
		if (nullptr == node)
			return nullptr;
		node_type *copy = new node_type(node->key);
		copy->left = deep_copy_(node->left, copy);
		copy->right = deep_copy_(node->right, copy);
		copy->parent = parent;
		return copy;
	}

	void make_child_(node_type *parent, node_type *child, direction_type direction) const
	{
		if (parent != nullptr)
			if (left_direction == direction)
				parent->left = child;
			else
				parent->right = child;
		else {}
		if (child != nullptr)
			child->parent = parent;
	}

	void rotate_(node_type *node) const
	{
		node_type *parent = node->parent;
		if (parent == nullptr)
			return;
		make_child_(parent->parent, node, direction_(parent));
		if (direction_(node) == left_direction)
		{
			make_child_(parent, node->right, left_direction);
			make_child_(node, parent, right_direction);
		}
		else
		{
			make_child_(parent, node->left, right_direction);
			make_child_(node, parent, left_direction);
		}
	}

	void zig_(node_type *node) const
	{
		rotate_(node);
	}

	void zig_zig_(node_type *node) const
	{
		rotate_(node->parent);
		rotate_(node);
	}

	void zig_zag_(node_type *node) const
	{
		rotate_(node);
		rotate_(node);
	}

	void splay_(node_type *node) const
	{
		root_ = node;
		if (nullptr == node)
			return;
		while (node->parent != nullptr)
		{
			static node_type *parent, *grandparent;
			parent = node->parent;
			grandparent = parent->parent;
			if (nullptr == grandparent)
				zig_(node);
			else if (direction_(node) == direction_(parent))
				zig_zig_(node);
			else
				zig_zag_(node);
		}
	}

	void find_key_(const key_type &key)
	{
		node_type *current = root_, *parent = nullptr;
		while (nullptr != current && !eq_(key, current->key))
		{
			parent = current;
			if (compare(current->key, key))
				current = current->right;
			else
				current = current->left;
		}
		if (current == nullptr)
			current = parent;
		splay_(current);
	}

	node_type *merge_(node_type *right_tree)
	{
		if (nullptr == root_ || nullptr == right_tree)
			return root_ != nullptr ? root_ : right_tree;
		maximum();
		make_child_(root_, right_tree, right_direction);
		return root_;
	}

	std::pair <node_type *, node_type *> split_(const key_type &key)
	{
		if (nullptr == root_)
			return { nullptr, nullptr };
		find_key_(key);
		if (compare(root_->key, key))
		{
			node_type *right_tree = root_->right;
			if (nullptr != right_tree)
				right_tree->parent = nullptr;
			root_->right = nullptr;
			return { root_, right_tree };
		}
		else
		{
			node_type *left_tree = root_->left;
			if (nullptr != left_tree)
				left_tree->parent = nullptr;
			root_->left = nullptr;
			return { left_tree, root_ };
		}
	}
};

/*
 * use 1 <tree_num> <number> for inserting <number> into <tree_num>
 * 2 - erase
 * 3 - a[0] = merge(a[0], a[1])
 * 4 <k> - split a[0] to a[0], a[1]
 */

int main()
{
	using namespace std;
	int n;
	cin >> n;
	splay_tree <int> a[2];
	for (int i(0); i < n; ++i)
	{
		int t;
		cin >> t;
		if (t == 1)
		{
			int x, y;
			cin >> x >> y;
			a[x - 1].insert(y);
		}
		else if (t == 2)
		{
			int x, y;
			cin >> x >> y;
			a[x - 1].erase(y);
		}
		else if (t == 3)
			a[0].merge(a[1]);
		else
		{
			int k; cin >> k;
			a[0].split(a[1], k);
		}
		cout << "a[0] = ";
		a[0].__cout_tree();
		cout << "a[1] = ";
		a[1].__cout_tree();
	}
}

