/** 
 * @file zz_pool.h
 * @brief pool class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2002
 *
 * $Header: /engine/include/zz_pool.h 5     04-08-23 5:21p Zho $
 * $History: zz_pool.h $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-08-23   Time: 5:21p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-07-30   Time: 8:50p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 4:22p
 * Updated in $/engine/include
 * erase() and some comments added.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 1:52p
 * Updated in $/engine/include
 * added erase()
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
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
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-29   Time: 6:26a
 * Updated in $/znzin11/engine/include
 * added iterator mechanism
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_POOL_H__
#define __ZZ_POOL_H__

#include <map>
#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_LOG_H__
#include "zz_log.h"
#endif

#define ZZ_POOL_MAX_SIZE 10000

template<class T>
class zz_pool {
private:
	std::map<zz_handle, T> running_map;
	std::map<zz_handle, T> free_map;

	typedef typename std::map<zz_handle, T>::iterator it_type;

public:
	class iterator {
		friend zz_pool;
	private:
		typename std::map<zz_handle, T>::iterator _it;

	public:
		iterator& operator++()
		{
			_it++;
			return *this;
		}

		iterator& operator++(int i)
		{
			_it++;
			return *this;
		}

		iterator& operator--()
		{
			_it--;
			return *this;
		}

		iterator& operator--(int i)
		{
			_it->operator--(i);
			return *this;
		}

		bool operator!=(iterator& it)
		{
			return (it._it != _it);
		}

		bool operator==(iterator& it)
		{
			return (it._it == _it);
		}	

		T& operator*()
		{
			return (_it->second);
		}
	};

	zz_pool ()
	{
	}

	~zz_pool ()
	{
	}

	iterator begin ()
	{
		iterator it;
		it._it = running_map.begin();
		return it;
	}

	iterator end ()
	{
		iterator it;
		it._it = running_map.end();
		return it;
	}

	// add an element
	zz_handle add (T data)
	{
		zz_handle index;

		// if has no free_map, then create one by using new index
		if (free_map.empty()) {
            index = get_num_total();
		}
		else {
			// if has free node, then re-use the index that
			// free node have used
			it_type it = free_map.begin();
			index = (*it).first;
			free_map.erase(index);
		}
		assert(index <= ZZ_POOL_MAX_SIZE);
		assert(!find(index));
		running_map[index] = data;

		return index;
	}

	bool find (zz_handle index_to_find)
	{
		return (running_map.find(index_to_find) != running_map.end());
	}

	// invalidate the index
	void del (zz_handle index_to_remove)
	{
		assert(ZZ_HANDLE_IS_VALID(index_to_remove));
		assert(this->find(index_to_remove));
		assert(free_map.find(index_to_remove) == free_map.end());
		
		free_map[index_to_remove] = running_map[index_to_remove];
		running_map.erase(index_to_remove);
	}

	// delete by iterator
	void erase (iterator it)
	{
		// *it* is of running_map's
		assert((it._it) != running_map.end()); // confirm valid iterator
		zz_handle index = (it._it)->first; // get index
		assert(ZZ_HANDLE_IS_VALID(index)); // confirm valid handle
		free_map[index] = (it._it)->second; // copy data from running_map to free_map
		running_map.erase(index); // delete running_map item with index
	}

	T& operator[] (zz_handle index)
	{
		return running_map[index];
	}

	const T& operator[] (zz_handle index) const
	{
		return running_map[index];
	}

	unsigned int get_num_running () const
	{
		return running_map.size();
	}

	unsigned int get_num_total () const
	{
		return running_map.size() + free_map.size();
	}

	void clear ()
	{
		it_type it = running_map.begin();
		zz_handle index;
		while (it != running_map.end()) {
			index = (*it).first;
			free_map[index] = (*it).second;
			running_map.erase(index);
			it = running_map.begin();
		}
		assert(running_map.empty());
	}

	// Check if any items of these maps were lost or overlapped.
	bool check ()
	{
		int num_of_items = (int)free_map.size() + (int)running_map.size();
		
		for (int i = 0; i < num_of_items; i++) {
			bool free_found = (free_map.find(i) == free_map.end());
			bool run_found = (running_map.find(i) == running_map.end());
			if (free_found == run_found) return false;
		}
		return true;
	}

	// Dump content
	void dump ()
	{
		it_type it;
		ZZ_LOG("[%x/%d]->running_map = {", this, running_map.size());
		for (it = running_map.begin(); it != running_map.end(); it++) {
			ZZ_LOG("%d ", (*it).first);
		}
		ZZ_LOG("}\n");
		ZZ_LOG("[%x/%d]->free_map= {", this, free_map.size());
		int num = (int)free_map.size();
		int count = 0;
		for (count = 0, it = free_map.begin(); it != free_map.end(); it++, count++) {
			ZZ_LOG("%d ", (*it).first);
			if (count > num) {
				assert(0);
			}
		}
		ZZ_LOG("}\n");
		assert(check());
	}
};

/*
	zz_pool<int> zho_pool;

	int k1 = zho_pool.add(10);
	int k2 = zho_pool.add(20);
	int r1 = zho_pool[k1];
	int r2 = zho_pool[k2];
	r1 = r1;
	r2 = r2;
	zho_pool.del(k1);
	r1 = zho_pool.add(30);
	r2 = zho_pool.add(40);
	r1 = r1;
	r2 = r2;
*/

#endif // __ZZ_POOL_H__