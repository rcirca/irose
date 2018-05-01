/** 
 * @file zz_hash_table.h
 * @brief hash table class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    20-jun-2002
 *
 * $Header: /engine/include/zz_hash_table.h 9     04-07-02 3:12p Zho $
 * $History: zz_hash_table.h $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-02   Time: 3:12p
 * Updated in $/engine/include
 * delayed_load
 * camera collision
 * terrain index buffer
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-03-06   Time: 7:11p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-03-05   Time: 1:41p
 * Updated in $/engine/include
 * Redesigned vfs_thread.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-20   Time: 12:13p
 * Updated in $/engine/include
 * Some assertion added.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-10   Time: 8:27p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-10   Time: 5:10p
 * Updated in $/engine/include
 * In ~hash_table(), not-all-members-are-deleted bug was fixed.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-17   Time: 1:24p
 * Updated in $/engine/include
 * null hashkey zz_assert added
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
 */

#ifndef	__ZZ_HASH_TABLE_H__
#define __ZZ_HASH_TABLE_H__

#define ZZ_HASH_DEFAULT_SIZE 64000

#include <string.h>

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif

#ifndef __ZZ_ASSERT_H__
#include "zz_assert.h"
#endif

/*
bucket#0 : slot#1<->slot#2<->slot#3<->....<->slot#n->NULL
bocket#1 : slot#1<->....
.....
bucket#n : ....

bucket# = hash_function(string);
*/

template <class T>
class zz_hash_table {
private:
	struct _slot {
		char * key; // string name

		_slot * prev; // link in slot list
		_slot * next;

		_slot * list_prev; // double link list to traverse
		_slot * list_next;

		T data;

		_slot () : key(NULL), next(NULL), prev(NULL), list_prev(NULL), list_next(NULL) {}

		~_slot () {
			if (key) { // if allocated name exist, delete it.
				zz_delete[] key; // newed in set_key()
			}
			if (prev) {
				prev->next = next;
			}
			if (next) {
				next->prev = prev;
			}
			if (list_prev) {
				list_prev->list_next = list_next;
			}
			if (list_next) {
				list_next->list_prev = list_prev;
			}
		}

		void set_key (const char * key_to_set) {
			key = zz_new char[strlen(key_to_set)+1]; // will be deleted in ~_slot()
			strcpy(key, key_to_set);
		}
	};

	_slot ** _bucket; // bucket. comprises slot pointers
	int _bucket_size; // bucket size. the number of slot lists

	_slot * _head, * _tail; // for sequential access

	int _count; // total allocated slot number

#ifdef _DEBUG
	int _max_iteration_count;
	int _iteration_count;
#endif

	// find slot with same key
	_slot * _find_slot (const char * key) {
		assert(key && "NULL hash key is not allowed.");
		
		int hkey = _hash(key);
		_slot * found_slot = _bucket[hkey];

#ifdef _DEBUG
		_iteration_count = 0; // clear iteration count
#endif
		while (found_slot) {

#ifdef ZZ_HASH_TABLE_DOLOG
			ZZ_LOG("this[%x]. size(%d). found_slot[%x]. key_find(%s:%s->[%d]. head(%x), tail(%x), head(%s), tail(%s)\n",
					this,
					_count,
					found_slot,
					found_slot->key,
					key,
					_hash(key),
					_head,
					_tail,
					(_head) ? _head->key : "null",
					(_tail) ? _tail->key : "null");
#endif

			if (strcmp(found_slot->key, key) == 0) { // same key found
				return found_slot;
			}
			found_slot = found_slot->next;

#ifdef _DEBUG
			if (++_iteration_count > _max_iteration_count) {
				_max_iteration_count = _iteration_count;
				// print maximum iteration count
				//ZZ_LOG("hash_table: max_iteration_count = %d\n", _max_iteration_count);
			}
#endif
		}
		return NULL;
	}

	// slot initialize
	void _init (void) {
		_head = NULL;
		_tail = NULL;
		_count = 0;
		_bucket = zz_new _slot * [_bucket_size]; // * means pointer. not multiplication

#ifdef _DEBUG
		//ZZ_LOG("hash_table: _init(%d->%x)\n", _bucket_size, _bucket);
		_max_iteration_count = 0;
#endif
		memset(_bucket, 0, _bucket_size*sizeof(_slot *));
		//for (int i = 0; i < _bucket_size; i++) {
		//	_bucket[i] = NULL;
		//}
	}

	int _hash (const char * key_to_hash) {
		assert(key_to_hash);

		// [ref] djb2 hash algorithm
		// http://www.cs.yorku.ca/~oz/hash.html
		unsigned long hash_code = 5381;
		int c;
		int ret;

		while (c = *key_to_hash++)
			hash_code = ((hash_code << 5) + hash_code) + c; // hash * 33 + c

		ret = (hash_code % _bucket_size);

		return ret;

		//unsigned char c;
		//int i = 0;
		//long hash_code = 0;
		//while (c = (unsigned char)key_to_hash[i++]) {
		//	hash_code += c*i; // i >= 1
		//}
		//return (hash_code % _bucket_size);
	}

public:
	zz_hash_table(void) : _bucket_size(ZZ_HASH_DEFAULT_SIZE) {
		_init();
	}
	
	// size : bucket size
	zz_hash_table(int size) : _bucket_size(size) {
		_init();
	}

	~zz_hash_table(void) {
		clear();
		
		assert(_count == 0);
		
#ifdef _DEBUG
		for (int i = 0; i < _bucket_size; i++) {
			assert(_bucket[i] == NULL);
		}
#endif

		zz_delete [] _bucket;
	}

	class iterator {
		friend zz_hash_table;
	private:
		_slot * _current;

	public:
		iterator() : _current(NULL) {}
		~iterator() {}

		iterator& operator++() {
			if (_current) {
				_current = _current->list_next;
			}
			return *this;
		}

		iterator& operator++(int i) {
			if (_current) {
				_current = _current->list_next;
			}
			return *this;
		}

		iterator& operator--() {
			if (_current) {
				_current = _current->list_prev;
			}
			return *this;
		}

		iterator& operator--(int i) {
			if (_current) {
				_current = _current->list_prev;
			}
			return *this;
		}

		bool operator!=(iterator& it) {
			return (it._current != _current);
		}

		bool operator==(iterator& it) {
			return (it._current == _current);
		}	

		T& operator*() {
			assert(_current);
			return _current->data;
		}
	};

	iterator begin (void) {
		iterator it;
		it._current = _head;
		return it;
	}

	iterator end (void) {
		iterator it;
		it._current = NULL;
		return it;
	}

	iterator erase (iterator it);

	void clear (void) {
		iterator it = begin();
		
		while (it != end()) {
			erase(it);
			it = begin();
		}
		assert(_count == 0);
		assert(!_tail && !_head);
	}

	iterator find (const char * key) {
		assert(key);

		_slot * found_slot = _find_slot(key);
		iterator it;

		if (!found_slot) {
			return end();
		}
		it._current = found_slot;
		return it;
	}

	iterator insert (const char * key, T value);

	iterator remove (const char * key)
	{
		iterator it;
		it._current = _find_slot(key);
		return erase(it);
	}
	
	int size (void) {
		return _count;
	}
};

template <class T>
typename zz_hash_table<T>::iterator zz_hash_table<T>::insert (const char * key, T value)
{
	assert(key);

	_slot * start_slot, * new_slot;
	int hash_code;

	// we do not check to have same key for performance
	// then, assure that we do not insert same key
	assert(find(key) == end());

	hash_code = _hash(key);
	start_slot = _bucket[hash_code];
	new_slot = zz_new _slot;

	assert(new_slot);

	new_slot->set_key(key);
	new_slot->data = value;

	// setup linkage
	new_slot->next = start_slot;
	//new_slot->prev = assumes null
	if (start_slot) {
		start_slot->prev = new_slot;
		//start_slot->next = do not touch
	}

	_bucket[hash_code] = new_slot;

	// for list, add to tail slot
	if (_tail) {
		new_slot->list_prev = _tail;
		_tail->list_next = new_slot;
		_tail = new_slot;
	}
	if (!_head) { // if head does not exist
		assert(!_tail);
		_head = new_slot;
		_tail = new_slot;
	}

	//ZZ_LOG("hash_table[%x]: insert(%s/%d)\n", this, key, _count);

	++_count;

	assert(find(key) != this->end());

	iterator it;
	it._current = new_slot;
	return it;
}

template < class T >
typename zz_hash_table< T >::iterator zz_hash_table< T >::erase (typename zz_hash_table< T >::iterator it)
{
	if (it._current == NULL) return it;
	
	_slot * this_slot = it._current;

	//ZZ_LOG("hash_table:: remove(%s)\n", key);
	if (!this_slot) 
		return end(); // not found

	int hash_code = _hash(this_slot->key);

	if (!strcmp(this_slot->key, _bucket[hash_code]->key)) { // if it is the head of the slot
		_bucket[hash_code] = this_slot->next;
	}

	if (_head == this_slot) { // if it is the head of the list
		_head = this_slot->list_next;
	}
	if (_tail == this_slot) {
		_tail = this_slot->list_prev;
	}

	it._current = this_slot->list_next;
	
	zz_delete this_slot;

	--_count;

	return it;
}

// usage:
/*
zz_hash_table<int> tab(100);

tab.insert("one", 1);
tab.insert("two", 2);
tab.insert("three", 3);
tab.insert("four", 4);
tab.insert("five", 5);

int zho;

if (tab.find("five", zho)) {
	ZZ_LOG("main: tab.find (three = %d)\n", zho);
}
tab.remove("five");

if (!tab.find("five", zho)) {
	ZZ_LOG("main: tab.find failed.\n");
}
*/	
#endif // __ZZ_HASH_TABLE_H__