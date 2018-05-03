/** 
 * @file zz_tree.h
 * @brief red-black tree.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-jun-2002
 *
 * $Header: /engine/include/zz_tree.h 2     04-01-17 2:11p Zho $
 * $History: zz_tree.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-17   Time: 2:11p
 * Updated in $/engine/include
 * return NULL when try to insert already existing one.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

/*
  [Red-Black Tree Reference]

  - for easy tutorial
  http://www.cs.mcgill.ca/~cs251/OldCourses/1997/topic18/ 
  http://kuic.kyonggi.ac.kr/~skpaik/DS_text/niemann/s_man.htm

  - for clean code reference
  http://kuic.kyonggi.ac.kr/~skpaik/DS_text/niemann/s_rbt.txt
  
  - for perpect implementation
  http://times8.postech.ac.kr/doc/libstdc++3-doc/libstdc++/html_user/stl__tree_8h-source.html

  - etc
  http://www.cs.utk.edu/~plank/plank/rbtree/rbtree.html
  http://www.seanet.com/users/arsen/avltree.html
*/

#ifndef __ZZ_TREE_H__
#define __ZZ_TREE_H__

#include <assert.h>
#include <stdio.h>

#define _key_compare(a, b) ((a) < (b))

typedef enum { BLACK, RED } _node_color;
template <class T_key, class T_value>
class zz_tree {
private:
	struct _node {
		_node * left;
		_node * right;
		_node * parent;
		_node_color color;
		T_key key;
		T_value value;

		_node () : left(NULL), right(NULL), parent(NULL), color(RED) {}
		~_node () {}
	};

	_node * _root;
	_node * _head, * _tail;
	int _count;

	// just for test
	bool _is_leaf (_node * x) {
		if (!x) return false;
		if (!x->left && !x->right) return true;
		return false;
	}
	int _count_depth (_node * x) {
		if (!x) return 0;
		int count = 0;
		count = _count_depth(x->parent);
		return (x->color == BLACK) ? ++count : count;
	}

	// rotate node x to left
	void _rotate_left (_node * x) {
		_node * y = x->right;

		assert(y);
		// establish x->right link
		x->right = y->left;
		if (y->left) y->left->parent = x;

		// y's new parent was x's parent
		y->parent = x->parent;

		// Set the parent to point to y instead of x
		// First see whether we're at the root */
		if (x->parent == NULL) _root = y;
		else
			if (x == (x->parent)->left)
				// x was on the left of its parent
				x->parent->left = y;
			else
				// x must have been on the right
				x->parent->right = y;
		// Finally, put x on y's left
		y->left = x;
		x->parent = y;
	}

	// rotate node x to right
	void _rotate_right (_node * x) {
		_node * y = x->left;

		assert(y);
		// establish x->left link
		x->left = y->right;
		if (y->right) y->right->parent = x;

		// y's new parent was x's parent
		y->parent = x->parent;

		// Set the parent to point to y instead of x
		// First see whether we're at the root */
		if (x->parent == NULL) _root = y;
		else
			if (x == (x->parent)->right)
				// x was on the right of its parent
				x->parent->right = y;
			else
				// x must have been on the left
				x->parent->left = y;
		// Finally, put x on y's left
		y->right = x;
		x->parent = y;
	}

	//  maintain Red-Black tree balance
	//  after inserting node x       
	void _insert_fixup (_node * x) {
		// check Red-Black properties
		while (x->parent && x->parent->color == RED) {
			// we have a violation
			if (x->parent == x->parent->parent->left) {
				_node * y = x->parent->parent->right;
				if (y && (y->color == RED)) {
					// uncle is RED
					x->parent->color = BLACK;
					y->color = BLACK;
					x->parent->parent->color = RED;
					x = x->parent->parent;
				} else {
					// uncle is BLACK
					if (x == x->parent->right) {
						// make x a left child
						x = x->parent;
						_rotate_left(x);
					}
					// recolor and rotate
					x->parent->color = BLACK;
					x->parent->parent->color = RED;
					_rotate_right(x->parent->parent);
				}
			} else {
				// mirror image of above code
				_node * y = x->parent->parent->left;
				if (y && (y->color == RED)) {
					// uncle is RED
					x->parent->color = BLACK;
					y->color = BLACK;
					x->parent->parent->color = RED;
					x = x->parent->parent;
				} else {
					// uncle is BLACK
					if (x == x->parent->left) {
						x = x->parent;
						_rotate_right(x);
					}
					x->parent->color = BLACK;
					x->parent->parent->color = RED;
					_rotate_left(x->parent->parent);
				}
			}
		}
		_root->color = BLACK;
	}


	//  allocate node for data and insert in tree
	_node * _insert_node (T_key key, T_value value) {
		_node * current, * parent, * x;

		assert(find(key) == end());
		if (find(key) != end()) {
			return NULL;
		}

		// find where node belongs
		current = _root;
		parent = NULL;
		while (current != NULL) {
			parent = current;
			current = _key_compare(key, current->key) ?
				current->left : current->right;
		}

		// setup new node
		x = new _node;
		x->key = key;
		x->value = value;
		x->parent = parent;
		x->left = x->right = NULL;
		x->color = RED;

		// insert node in tree
		if (parent) {
			if(_key_compare(key, parent->key))
				parent->left = x;
			else
				parent->right = x;
		} else {
			_root = x;
		}

		// head/tail setting
		if (!_head || _key_compare(key, _head->key)) { // if x < _head
			_head = x;
		}
		if (!_tail || _key_compare(_tail->key, key)) { // if x > _tail
			_tail = x;
		}

		_insert_fixup(x);
		++_count;

		return(x);
	}
	
	// maintain Red-Black tree balance
	// after deleting node x
	void _delete_fixup (_node *x, _node * x_parent) {
		if (!x) return;

		while (x != _root && (!x || x->color == BLACK)) {
			if (x == x_parent->left) {
				_node * w = x_parent->right;
				if (w && w->color == RED) {
					w->color = BLACK;
					x_parent->color = RED;
					_rotate_left(x_parent);
					w = x_parent->right;
				}
				if (w == NULL) { // dirty case. It made me crazy for 2 days...
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else if ((!w->left || w->left->color == BLACK) && (!w->right || w->right->color == BLACK)) {
					w->color = RED;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else {
					if (!w->right || w->right->color == BLACK) {
						if (w->left) w->left->color = BLACK;
						w->color = RED;
						_rotate_right(w);
						w = x_parent->right;
					}
					w->color = x_parent->color;
					x_parent->color = BLACK;
					if (w->right) w->right->color = BLACK;
					_rotate_left(x_parent);
					break; // x = _root;
				}
			} else {
				_node * w = x_parent->left;
				if (w && w->color == RED) {
					w->color = BLACK;
					x_parent->color = RED;
					_rotate_right(x_parent);
					w = x_parent->left;
				}
				if (w == NULL) {
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else if ((!w->right || w->right->color == BLACK) && (!w->left || w->left->color == BLACK)) {
					w->color = RED;
					x = x_parent;
					x_parent = x_parent->parent;
				}
				else {
					if (!w->left || w->left->color == BLACK) {
						if (w->right) w->right->color = BLACK;
						w->color = RED;
						_rotate_left(w);
						w = x_parent->left;
					}
					w->color = x_parent->color;
					x_parent->color = BLACK;
					if (w->left) w->left->color = BLACK;
					_rotate_right(x_parent);
					break; // x = _root;
				}
			}
		}
		if (x)
			x->color = BLACK;
	}

	// delete node z from tree
	void _delete_node (_node * z) {
		_node * y = z;
		_node * x = 0;
		_node * x_parent = 0;
		
		if (!z) return;

		// set y
		if (z->left == NULL || z->right == NULL)
			// y has a NULL node as a child
			y = z;
		else {
			// find tree successor with a NULL node as a child
			assert((z != _head) && (z != _tail));
			y = z->right;
			while (y->left != NULL) y = y->left;
		}

		// set x
		// x is y's only child
		if (y->left != NULL)
			x = y->left;
		else
			x = y->right;

		if (y == z) {
			// head/tail setting
			if (_head == z) {
				assert(!z->left);
				if (z->right) {
					_node * left_most = z->right;
					while (left_most->left) {
						left_most = left_most->left;
					}
					_head = left_most;
				}
				else {
					_head = z->parent;
					//if (_head == NULL) {
					//	assert(z == _tail);
					//}
					assert(!z->parent || (z->parent->left == z));
				}
			}
			if (_tail == z) {
				assert(!z->right);
				if (z->left) {
					_node * right_most = z->left;
					while (right_most->right) {
						right_most = right_most->right;
					}
					_tail = right_most;
				}
				else {
					_tail = z->parent;
					assert(!z->parent || (z->parent->right == z));
				}
			}
		}

		// remove y from the parent chain
		x_parent = y->parent; // saving for the case [x == NULL]
		if (x) 
			x->parent = y->parent;

		if (y->parent)
			if (y == y->parent->left)
				y->parent->left = x;
			else
				y->parent->right = x;
		else
			_root = x;

		if (y != z) {
			z->key = y->key;
			z->value = y->value;
			if (y == _tail)
				_tail = z;
			assert(y != _head);
		}

		if (y->color != RED) {
			_delete_fixup(x, x_parent);
		}

		--_count;
		//_dump("delete");
		assert(y != _tail);
		delete y;
	}

	// for debugging. delete this!
	void _dump (char * mode) {
		iterator it;
		it = begin();
		_node * current = _head;
		int left, right, parent;
		int color;
		while (it != end()) {
			current = it._current;
			if (current->left)
				left = current->left->key;
			else left = -1;
			if (current->right)
				right = current->right->key;
			else right = -1;
			if (current->parent)
				parent = current->parent->key;
			else parent = -1;
			color = current->color;
			printf("[%s]%d : left(%d), right(%d), parent(%d), color(%d)\n",
				mode,
				current->key, left, right, parent, color);
			it++;
		}
	}		

public:
	class iterator {
		friend zz_tree;
	private:
		_node * _current;
		bool _reverse;

		void _increment () {
			_node * save_node = _current;
			if (_current->right) { // have right node
				_current = _current->right;
				while (_current->left) {
					_current = _current->left;
				}
			}
			else { // does not have right node
				_node * parent_of_current = _current->parent;				
				while (parent_of_current && (_current == parent_of_current->right)) {
					_current = parent_of_current;
					parent_of_current = parent_of_current->parent;
				}
				_current = parent_of_current;
			}
		}

		void _decrement () {
			if (!_current) // empty!
				return;
			if (_current->left) {
				_current = _current->left;
				while (_current->right) {
					_current = _current->right;
				}
			}
			else {
				_node * parent_of_current = _current->parent;
				while (parent_of_current && (_current == parent_of_current->left)) {
					_current = parent_of_current;
					parent_of_current = parent_of_current->parent;
				}
				_current = parent_of_current;
			}
		}

	public:
		iterator() : _current(NULL), _reverse(false) {}
		~iterator() {}

		iterator& operator++() {
			if (_reverse) _decrement();
			else _increment();
			return *this;
		}

		iterator& operator++(int i) {
			if (_reverse) _decrement();
			else _increment();
			return *this;
		}

		iterator& operator--() {
			if (_reverse) _increment();
			else _decrement();
			return *this;
		}

		iterator& operator--(int i) {
			if (_reverse) _increment();
			else _decrement();
			return *this;
		}

		bool operator!=(iterator& it) {
			return (it._current != _current);
		}

		bool operator==(iterator& it) {
			// return ((it._current == _current) && (it._reverse == _reverse));
			return (it._current == _current);
		}

		const T_key * key() {
			if (_current)
				return &_current->key;
			return NULL;
		}

		const T_value * value() {
			if (_current)
				return &_current->value;
			return NULL;
		}
	};

public:
	void erase (iterator it) {
		_delete_node(it._current);
	}

	iterator insert (T_key key, T_value value) {
		iterator it;
		it._current = _insert_node(key, value);
		return it;
	}

	iterator find (T_key key) {
		iterator it;
		it = _find_node(key);
		return it;
	}

	int size () {
		return _count;
	}

	iterator begin () {
		iterator it;
		it._current = _head;
		it._reverse = false;
		return it;
	}
	
	iterator rbegin () {
		iterator it;
		it._current = _tail;
		it._reverse = true;
		return it;
	}

	iterator end (void) {
		iterator it;
		it._current = NULL;
		it._reverse = false;
		return it;
	}

	iterator rend (void) {
		iterator it;
		it._current = NULL;
		it._reverse = true;
		return it;
	}


	//iterator last (void) { // get final node
	//	iterator it;
	//	it._current = _tail;
	//	return it;
	//}

	void clear (void) {
		iterator it = begin();
		
		while (it != end()) {
			erase(it);
			it = begin();
		}
		assert(_count == 0);
		assert(!_tail && !_head);
	}

	iterator upper_bound (T_key key) {
		_node * y = NULL; // last node which is greater than the key
		_node * x = _root; // current node

		while (x)
			if (_key_compare(key, x->key)) // x->key > key
				y = x, x = x->left;
			else
				x = x->right;

		iterator it;
		it._current = y;
		return it;
	}

	iterator lower_bound (T_key key) {
		_node * y = NULL; // last node which is not less than the key
		_node * x = _root; // current node

		while (x)
			if (!_key_compare(x->key, key)) // !(x->key < k) == x->key >= k
				y = x, x = x->left;
			else 
				x = x->right;

		iterator it;
		it._current = y;
		return it;
	}

	zz_tree() : _root(NULL), _head(NULL), _tail(NULL), _count(0) {
	}

	~zz_tree() {
		clear();
	}

private:
	iterator _find_node (T_key key) {
		iterator it = lower_bound(key);

		return (it == end() || _key_compare(key, it._current->key)) ? end() : it;
	}
};

// usage :
/*
	zz_tree<int, int> pine;

	for (int i = 0; i < 100; i+=10) {
		pine.insert(i, i);
	}

	zz_tree<int, int>::iterator it;
	for (int i = 0; i < 100; i++) {
		it = pine.upper_bound(i);
		it--;
		ZZ_LOG("main::AppInit() %d = %d\n", i, it.value());
	}
*/
#endif // __ZZ_TREE_H__
