#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <stack>

using namespace std;

template <typename K, typename V>
class AVLNode {
public:
	K key;
	V value;
	AVLNode *left, *right;
	int height;

	AVLNode()
		:left(NULL), right(NULL), height(1)
	{}
	AVLNode(const K &key, const V &value)
		:key(key), value(value), left(NULL), right(NULL), height(1)
	{}
};

template <typename K, typename V>
class AVLTree {
private:
	AVLNode<K,V> *vRoot;
	AVLNode<K,V> *root;
	int nodeCnt;
	
public:
	AVLTree() {
		this->vRoot = new AVLNode<K,V>();
		this->root = NULL;
		this->nodeCnt = 0;
	}

	~AVLTree() {
		delete this->vRoot;
	}

	/* General Methods of AVL Tree. */

public:
	const AVLNode<K,V>* insert(const K &key, const V &value) {
		stack<AVLNode<K,V>*> route;
		auto pNew = new AVLNode<K,V>(key, value);

		if(root == NULL) {
			vRoot->right = root = pNew;
		} else {
			auto parent = vRoot, cur = root;

			route.push(vRoot);

			while(cur) {
				route.push(cur);
				
				parent = cur;

				if(key < cur->key)
					cur = cur->left;
				else if(key > cur->key)
					cur = cur->right;
				else {
					delete pNew;
					return NULL;
				}
			}

			if(key < parent->key)
				parent->left = pNew;
			else
				parent->right = pNew;
		}
		
		balance(route);

		++nodeCnt;
		return pNew;
	}

	bool remove(const K &key) {
		stack<AVLNode<K,V>*> route1, route2;
		auto parent = vRoot, cur = root;

		route1.push(vRoot);

		while(cur) {
			auto tmp = cur;

			if(key < cur->key)
				cur = cur->left;
			else if(key > cur->key)
				cur = cur->right;
			else
				break;
			
			route1.push(tmp);

			parent = tmp;
		}

		if(!cur) {
			return false;
		} else {
			bool r2Flag = false;
			auto *pDel = cur;
			decltype(pDel) pRep;

			if(!pDel->right) {
				pRep = pDel->left;
			} else {
				auto *p = pDel, *c = pDel->right;

				route2.push(pDel);

				while(c->left) {
					route2.push(c);
					p = c;
					c = c->left;
				}
				route1.push(c);

				if(pDel->right == c) {
					c->left = pDel->left;
				} else {
					r2Flag = true;
					p->left = c->right;
					c->left = pDel->left;
					c->right = pDel->right;
				}
				pRep = c;
			}

			if(parent->left == pDel)
				parent->left = pRep;
			else
				parent->right = pRep;

			if(pDel == root) {
				vRoot->right = root = pRep;
			}

			if(r2Flag) {
				balance(route2);
				pRep->right = pDel->right;
			}
			balance(route1);
			
			delete pDel;
			--nodeCnt;
			
			return true;
		}
	}

	const AVLNode<K,V>* search(const K &key) {
		auto cur = root;
		while(cur) {
			if(key < cur->key)
				cur = cur->left;
			else if(key > cur->key)
				cur = cur->right;
			else
				break;
		}
		return cur;
	}

	inline int count(void) {
		return nodeCnt;
	}

	inline int getHeight(void) {
		return getHeight(root);
	}

private:
	inline int getHeight(AVLNode<K,V> *node) {
		return (node ? node->height : 0);
	}


	/* Methods that balance AVL Tree. */

private:
	void balance(stack<AVLNode<K,V>*> &route) {
		if(route.empty())
			return;

		AVLNode<K,V> *parent, *cur = route.top(); route.pop();

		while(!route.empty()) {
			updateHeight(cur);
			parent = route.top(); route.pop();

			if(parent->left == cur)
				parent->left = balance(cur);
			else
				parent->right = balance(cur);

			if(cur == root)
				root = vRoot->right;

			cur = parent;
		}
	}

	AVLNode<K,V>* balance(AVLNode<K,V> *node) {
		int bf = balanceFactor(node);
		if(bf > 1) {
			if(balanceFactor(node->left) > 0) {
				node = rotateLL(node);
			} else {
				node = rotateLR(node);
			}
		} else if(bf < -1) {
			if(balanceFactor(node->right) > 0) {
				node = rotateRL(node);
			} else {
				node = rotateRR(node);
			}
		}
		return node;
	}

	void updateHeight(AVLNode<K,V> *node) {
		int h1 = getHeight(node->left);
		int h2 = getHeight(node->right);
		node->height = (h1 > h2 ? h1 : h2) + 1;
	}

	inline int balanceFactor(AVLNode<K,V> *node) {
		return getHeight(node->left) - getHeight(node->right);
	}

	AVLNode<K,V>* rotateLL(AVLNode<K,V> *node) {
		auto *child = node->left;
		node->left = child->right;
		child->right = node;

		updateHeight(node);
		updateHeight(child);

		return child;
	}
	AVLNode<K,V>* rotateRR(AVLNode<K,V> *node) {
		auto *child = node->right;
		node->right = child->left;
		child->left = node;

		updateHeight(node);
		updateHeight(child);
		
		return child;
	}
	AVLNode<K,V>* rotateLR(AVLNode<K,V> *node) {
		node->left = rotateRR(node->left);
		return rotateLL(node);
	}
	AVLNode<K,V>* rotateRL(AVLNode<K,V> *node) {
		node->right = rotateLL(node->right);
		return rotateRR(node);
	}

	/* Methods that visualize AVL Tree. */

public:
	void visualizeTree(void) {
		int h = getHeight();
		bool *flag = new bool[h];
		visualizeTree(root, flag, 0);
		delete[] flag;
	}

private:
	void visualizeTree(AVLNode<K,V> *root, bool *flag, int level) {
		for(int i=0; i<level-1; ++i) {
			if(flag[i])	cout << "│  ";
			else		cout << "   ";
		}
		if(level > 0) {
			if(root) {
				if(flag[level-1]) cout << "├──" << root->key << endl;
				else			  cout << "└──" << root->key << endl;
			} else {
				if(flag[level-1]) cout << "├──" << "nil" << endl;
				else			  cout << "└──" << "nil" << endl;
				return;
			}
		} else {
			if(root) cout << root->key << endl;
			else return;
		}
		flag[level] = true;
		visualizeTree(root->left, flag, level+1);
		flag[level] = false;
		visualizeTree(root->right, flag, level+1);
	}
};

template <typename T>
void print_array(T arr[], int N) {
	if(N > 1000) {
		cout << "Omission of print_array for preventing tainted stdout." << endl;
		return;
	}
	for(int i=0; i<N; ++i)
		cout << arr[i] << " ";
	cout << endl;
}

int main(void)
{
	int N;
	int mode, detail;
	int *data;
	AVLTree<int,int> tree;
	time_t start_time, end_time;

	ios::sync_with_stdio(false);

	cout << endl;
	cout << "*******************************************" << endl;
	cout << "****** AVL Tree and Tester By taeguk ******" << endl;
	cout << "*******************************************" << endl;
	cout << endl;

	cout << "Enter # of data >> ";
	cin >> N;

	cout << "Select mode * 1(random), 0(worst) >> ";
	cin >> mode;

	cout << "Detail Information * 1(yes), 0(no) >> ";
	cin >> detail;

	data = new int[N];
	for(int i=0; i<N; ++i)
		data[i] = i+1;

	if(mode) {
		unsigned seed = chrono::system_clock::now().time_since_epoch().count();
		shuffle(data, data+N, default_random_engine(seed));
	}

	cout << endl;
	if(detail) {
		cout << "Array of data -------------" << endl;
		print_array(data, N);
		cout << endl;
	}

	cout << "Insert Test ------------" << endl;
	start_time = clock();
	for(int i=0; i<N; ++i)
		tree.insert(data[i], data[i]);
	end_time = clock();
	if(detail) tree.visualizeTree();
	cout << "[I] Count of node : " << tree.count() << endl;
	cout << "[I] Height of tree : " << tree.getHeight() << endl;
	cout << "[I] Progress Time : " << (float)(end_time-start_time)/CLOCKS_PER_SEC << "sec" << endl << endl;

	cout << "Remove Test (odd number) ------------" << endl;
	start_time = clock();
	for(int i=1; i<=N; i+=2)
		tree.remove(i);
	end_time = clock();
	if(detail) tree.visualizeTree();
	cout << "[I] Count of node : " << tree.count() << endl;
	cout << "[I] Height of tree : " << tree.getHeight() << endl;
	cout << "[I] Progress Time : " << (float)(end_time-start_time)/CLOCKS_PER_SEC << "sec" << endl << endl;

	cout << "Search Test ------------" << endl;
	bool searchSuccess = true;
	start_time = clock();
	for(int i=1; i<=N; ++i) {
		if(i%2 == (tree.search(i)?1:0)) {
			searchSuccess = false;
			break;
		}
	}
	end_time = clock();
	if(searchSuccess)
		cout << "[*] Search Test PASS!" << endl;
	else 
		cout << "[!] Search Test FAIL!" << endl;
	cout << "[I] Progress Time : " << (float)(end_time-start_time)/CLOCKS_PER_SEC << "sec" << endl << endl;

	cout << "            Thank you!" << endl << endl;

	return 0;
}

