/** 
* @file zz_list.h
* @brief list class.
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    20-jun-2002
*
* $Header: /engine/include/zz_list.h 7     04-07-07 3:29a Zho $
* $History: zz_list.h $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-07-07   Time: 3:29a
 * Updated in $/engine/include
* 
* *****************  Version 6  *****************
* User: Zho          Date: 04-07-02   Time: 3:12p
* Updated in $/engine/include
* delayed_load
* camera collision
* terrain index buffer
* 
* *****************  Version 5  *****************
* User: Zho          Date: 04-06-30   Time: 6:57p
* Updated in $/engine/include
* 6.30.4498
* 
* *****************  Version 4  *****************
* User: Zho          Date: 04-05-19   Time: 8:13p
* Updated in $/engine/include
* 
* *****************  Version 3  *****************
* User: Zho          Date: 04-02-27   Time: 5:48p
* Updated in $/engine/include
* 
* *****************  Version 2  *****************
* User: Zho          Date: 04-01-17   Time: 1:24p
* Updated in $/engine/include
* nil in iterator added
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 8:05p
* Created in $/engine/include
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 7:57p
* Created in $/engine_1/include
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 7:53p
* Created in $/engine/engine/include
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 7:47p
* Created in $/engine/msvc71/include
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 6:19p
* Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
* 
* *****************  Version 1  *****************
* User: Zho          Date: 03-11-30   Time: 6:08p
* Created in $/Engine/INCLUDE
* 
* *****************  Version 10  *****************
* User: Zho          Date: 03-11-28   Time: 2:55p
* Updated in $/znzin11/engine/include
* cleanup #include structure
* 
* *****************  Version 9  *****************
* User: Zho          Date: 03-11-27   Time: 4:50p
* Updated in $/znzin11/engine/include
* - removed zz_resource class, added full-support of resource management
* into zz_node, and cleaned up some codes.
*/

/* features:
- sequential access
- binary search
- push_back(), remove()
*/

#ifndef	__ZZ_LIST_H__
#define __ZZ_LIST_H__

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif

#ifndef __ZZ_LOG_H__
#include "zz_log.h"
#endif

#include <assert.h>

//--------------------------------------------------------------------------------
template <class T>
class zz_list {
private:
	struct _node {
		_node * prev, * next;
		T data;

		_node () : prev(NULL), next(NULL) {}
		~_node ()
		{
			if (prev) {
				prev->next = next;
			}
			if (next) {
				next->prev = prev;
			}
		}

		// insert to back
		// this->next => this->new_node->next
		void insert (_node * new_node)
		{
			assert(new_node);

			if (next) {
				next->prev = new_node;
				new_node->next = next;
			}
			next = new_node;
			new_node->prev = this;
		}
	};

	_node * _head, * _tail;
	int _count;

public:
	class iterator {
		friend zz_list;
	private:
		_node * _current;
		T _nil;

	public:
		iterator() : _current(NULL) {}
		iterator(_node * current_in) : _current(current_in) {}
		iterator(const iterator& rhs) : _current(rhs._current) {}
		~iterator() {}

		iterator& operator=(const iterator& rhs)
		{
			if (this == &rhs) return *this;
			this->_current = rhs._current;
			return *this;
		}

		iterator& operator++()
		{
			if (_current) {
				_current = _current->next;
			}
			return *this;
		}

		iterator& operator++(int i)
		{
			if (_current) {
				_current = _current->next;
			}
			return *this;
		}

		iterator& operator--()
		{
			if (_current) {
				_current = _current->prev;
			}
			return *this;
		}

		iterator& operator--(int i)
		{
			if (_current) {
				_current = _current->prev;
			}
			return *this;
		}

		bool operator!=(iterator& it)
		{
			return (it._current != _current);
		}

		bool operator==(iterator& it)
		{
			return (it._current == _current);
		}	

		T operator*()
		{
			assert(_current);

			return _current->data;
			// We should throw exception?
			//throw "list: nil data refered";
			//return _nil; // We cannot provide *operator for nil iterator. But, we should provide any one for now.
		}
	};

private:

	// from -> next
	//   |
	//   V
	// from -> new_node -> next
	_node * insert (_node * from, T& data)
	{
		// build content
		_node * new_node = zz_new _node;
		new_node->data = data;

		++_count;
		// if this is the first
		if (from == NULL) { // it become head node
			_head = _tail = new_node;
			return new_node;
		}

		from->insert(new_node);

		if (_tail == from) {
			_tail = new_node;
		}
		return new_node;
	}


public:
	zz_list () : _head(NULL), _tail(NULL), _count(0) 
	{
	}

	~zz_list ()
	{
		clear();
	}

	void erase (iterator it)
	{
		_node * node_to_remove = it._current;

		if (_head == node_to_remove) {
			_head = node_to_remove->next;
		}
		if (_tail == node_to_remove) {
			_tail = node_to_remove->prev;
		}
		if (node_to_remove) {
			zz_delete (node_to_remove);
		}
		--_count;
	}

	void clear (void)
	{
		iterator it = begin();

		while (it != end()) {
			erase(it);
			it = begin();
		}
		assert(_count == 0);
		assert(!_tail && !_head);
	}

	int size () const
	{
		return _count;
	}

	iterator push_back (T data)
	{
		return iterator(insert(_tail, data));
	}

	iterator begin ()
	{
		return iterator(_head);
	}

	iterator begin () const
	{
		return iterator(_head);
	}

	iterator end (void)
	{
		return iterator(NULL);
	}

	iterator end (void) const
	{
		return iterator(NULL);
	}

	iterator find (T data)
	{
		for (iterator it = begin(), it_end = end(); it != it_end; ++it) {
			if (data == *it) {
				return it;
			}
		}
		return end();
	}

	bool empty () const
	{
		return (_count == 0);
	}
};

#endif // __ZZ_LIST_H__