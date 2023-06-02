#include <iostream>

class IntTree {
public:
	IntTree() : root(nullptr), size(0) { }
	IntTree(std::initializer_list<int> data) : IntTree() {
		for (auto i : data) { Insert(i); }
	}
	IntTree(const IntTree& other) : size(other.size) {
		Copy(root, nullptr, other.root);
	}
	IntTree(IntTree&& other) noexcept : IntTree() {
		Swap(*this, other);
	}
	~IntTree() {
		Clear(root);
	}

	IntTree& operator = (const IntTree& other) {
		IntTree new_tree{ other };
		Swap(*this, new_tree);
		return *this;
	}
	IntTree& operator = (IntTree&& other) noexcept {
		Swap(*this, other);
		return *this;
	}

	void Insert(int value) {
		if (size == 0) {
			root = new Node;
			root->data = value;
			size++;
			return;
		}
		Node* leaf = new Node;
		leaf->data = value;
		Insert(leaf);
		if (!leaf->parent) { delete leaf; return; }
		size++;
	}
	void Erase(int value) {
		Node* temp = Find(value);
		if (temp->data != value) return;
		size--;
		Erase(temp);
	}
	bool IsContained(int value) const {
		if (size == 0) return false;
		if (Find(value)->data == value) return true;
		else return false;
	}
	void Print() const {
		Print(root);
	}
	static void Swap(IntTree& t_tree, IntTree& o_tree) {
		std::swap(t_tree.size, o_tree.size);
		std::swap(t_tree.root, o_tree.root);
	}
	std::size_t GetSize() const {
		return size;
	}
private:
	struct Node {
		int data = 0;
		Node* l_child = nullptr;
		Node* r_child = nullptr;
		Node* parent = nullptr;
	};
	static void Erase(Node* root) {
		Node* temp = root;
		Node* parent = temp->parent;
		// no child case
		if (!temp->l_child && !temp->r_child) {
			if (parent->l_child == temp) parent->l_child = nullptr;
			else parent->r_child = nullptr;
			delete temp;
			return;
		}
		// one child case
		if (!temp->l_child || !temp->r_child) {
			if (parent->l_child == temp) parent->l_child = !temp->l_child ? temp->r_child : temp->l_child;
			else parent->r_child = !temp->r_child ? temp->l_child : temp->r_child;
			delete temp;
			return;
		}
		// two children case
		temp->data = temp->l_child->data;
		Erase(temp->l_child);
	}
	static void Clear(Node* node) {
		if (!node) return;
		Clear(node->l_child);
		Clear(node->r_child);
		delete node;
	}
	static void Copy(Node*& self_node, Node* self_node_parent, const Node* other_node) {
		if (!other_node) return;
		self_node = new Node;
		self_node->data = other_node->data;
		self_node->parent = self_node_parent;
		Copy(self_node->l_child, self_node, other_node->l_child);
		Copy(self_node->r_child, self_node, other_node->r_child);
	}
	static void Print(Node* node) {
		if (node->l_child) Print(node->l_child);
		std::cout << node->data << " ";
		if (node->r_child) Print(node->r_child);
	}
	void Insert(Node* child) {
		if (!child) return;
		Node* parent = Find(child->data);
		if (child->data == parent->data) return;
		if (child->data < parent->data) parent->l_child = child;
		else parent->r_child = child;
		child->parent = parent;
	}
	Node* Find(int child) const {
		Node* temp = root;
		while (temp->l_child || temp->r_child) {
			if (child < temp->data && temp->l_child) temp = temp->l_child;
			else if (child > temp->data && temp->r_child) temp = temp->r_child;
			else break;
		}
		return temp;
	}
	Node* root;
	std::size_t size;
};

int main() {
	//Constructor test section
	IntTree tree0, tree0_1, tree_test; //Default constructor test
	IntTree tree_c{ 50, 78, 34, 80, 79, 67, 20, 45, 17 }; //Constructor test + Insert function test (root setting and branch constructing)
	IntTree tree_copy1{ tree_c }, tree_copy2{ tree_c }, tree_copy3{ tree0 }; // Copy constructor test
	tree_copy1.Insert(88);
	tree_c.Erase(80);
	tree_c.Print(); std::cout << '\n';
	tree_copy1.Print(); std::cout << '\n';
	IntTree tree_move{ (IntTree&&)tree_copy2 }; //Move constructor test
	//Operators test section
	tree0 = tree_c; //Copy operator = test
	tree0_1 = (IntTree&&)tree_copy1; //Move operator = test
	//Functions test section
	tree_c.Insert(79); //Dublicate insertion
	tree_c.IsContained(41); //IsContained - false;
	tree_c.IsContained(20); //IsContained - true;
	tree_test.IsContained(0); //IsContained - empty container 0 test
	tree_c.Erase(41); //41 cannot be deleted
	tree_c.Erase(78); //78 can be deleted
	std::cout << "Tree size: " << tree_c.GetSize(); //GetSize function test
	std::cout << "\nTree components: "; tree_c.Print();
	return 0;
}
