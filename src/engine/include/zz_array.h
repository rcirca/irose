//not used anymore
/** 
 * @file zz_array.h
 * @brief simple array template.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-feb-2002
 *
 * $Id: zz_array.h,v 1.1.1.1 2002/09/06 00:44:04 Jiho Choi Exp $
 */

#ifndef __ZZ_ARRAY_H__
#define __ZZ_ARRAY_H__

#include "zz_mem.h"
#include "zz_log.h"

#define ZZ_ARRAY_DEFAULT_SIZE 100 // setting too big can bring heap memory overflow.
#define ZZ_ARRAY_MAXIMUM_SIZE 1000000 // maximum size to allocate

// array which starts from 0 to MAX-1
template <class T>
class zz_array {
private:
	T * _data;
	long _size;
	long _size_reserved;
	
	void _resize (long new_size) { // It doubles the size.
		assert(new_size > _size_reserved); // always big!

		long old_size = _size_reserved;
		_size_reserved = new_size;
		
		if (_size_reserved == 0)
			_size_reserved = ZZ_ARRAY_DEFAULT_SIZE; // minimum
		
		T * new_data = zz_new T [_size_reserved];
		assert(new_data);
		// do not memcpy!!! we have to call copy operator.
		//memcpy(new_data, _data, old_size*sizeof(T)); // buggy code
		for (int i = 0; i < old_size; ++i) {
			new_data[i] = _data[i];
		}
		
		zz_delete[] _data;
		_data = new_data;
	}

public:
	zz_array (long reserved_array_size = ZZ_ARRAY_DEFAULT_SIZE) :
	  _size(0), _size_reserved(reserved_array_size), _data(NULL)
	{
		assert(reserved_array_size < ZZ_ARRAY_MAXIMUM_SIZE);
		if (reserved_array_size) {
			_data = zz_new T[reserved_array_size];
		}
		//else
		//	_data = NULL;
	}
	
	// empty all items
	void reset ()
	{
		if (_data) 
			zz_delete[] _data;
		_size_reserved = 0;
		_data = NULL;
		_size = 0;
	}

	// clear for recycling
	void clear ()
	{
		_size = 0;
	}

	~zz_array ()
	{
		reset();
	}
	
	// operator [] doest not support auto-increment
	T& operator[] (long index_to_access)
	{
		assert(index_to_access < ZZ_ARRAY_MAXIMUM_SIZE);
		assert(index_to_access >= 0);

		if (index_to_access > _size_reserved - 1) {
			ZZ_LOG("array:: index exceeded(%d)\n", index_to_access);
			_resize((index_to_access+1)*2);
			//if (index_to_access == 0) {
			//	_resize(1);
			//}
			//else {
			//	_resize(index_to_access+1);
			//}
		}
		if (index_to_access > _size - 1) {
			_size = index_to_access + 1;
		}
		return _data[index_to_access];
	}

	long size ()
	{
		return _size;
	}

	void reserve (long size_to_reserve)
	{
		reset();
		_data = zz_new T [size_to_reserve];
		_size_reserved = size_to_reserve;
	}

	// add empty item
	long add (void)
	{
		if (!_data) { // empty, then allocate
			if (_size_reserved == 0) {
				_size_reserved = ZZ_ARRAY_DEFAULT_SIZE;
			}
			_data = zz_new T[_size_reserved];
		}
		if (_size >= _size_reserved) { // exceed maximum size
			_resize(_size_reserved*2); // make double size
		}
		return ++_size;
	}

	// support auto-increment by doubling size
	// CAUTION: do not insert the item which does not have its own copy operator!
	// exam) structures
	long add (T item_to_add)
	{
		if (!_data) { // empty, then allocate
			if (_size_reserved == 0) {
				_size_reserved = ZZ_ARRAY_DEFAULT_SIZE;
			}
			_data = zz_new T[_size_reserved];
		}
		if (_size >= _size_reserved) { // exceed maximum size
			_resize(_size_reserved*2);
		}

		_data[_size++] = item_to_add;		
		return _size;
		// else error! cannot add item
	}

	T& last ()
	{
		return _data[_size-1];
	}

	// It is not efficent so, it's better to use list class for removable objec.
	void remove (long index_to_access = -1) {
		assert(index_to_access < _size);
		
		// if index_to_access is not defined, then pop last element.
		if (index_to_access == -1)
			index_to_access = _size-1;

		if (_size == 0) return;

		for (long i = index_to_access; i < (_size-1); ++i) {
			_data[i] = _data[i+1];
		}
		--_size;
	}

	void copy_operator (zz_array<T>& from)
	{
		int from_size = from.size();
		this->reset();
		if (from_size <= 0) return;
		this->reserve(from_size);
		for (int i = 0; i < from_size; i++) {
			this->add(from[i]);
		}
		
	}

	void operator= (zz_array<T>& from)
	{
		copy_operator(from);
	}

private:
	// for sort()
	bool (*_compare) (T, T);
	void quick_sort (int low_in, int high_in);

public:
	void sort (bool (*compare)(T elem1, T elem2))
	{
		_compare = compare;
		quick_sort(0, this->size()-1);
	}
};

// traditional recursive quick sort
// [REFERENCE]
//    http://www.devapprentice.com/exqsort.htm
// This quick sort implementation performance equals to std::algorithim's.
// But, for over 10,000,000 elements, the standard library's is over 10 times faster.
template<class T>
void zz_array<T>::quick_sort (int low_in, int high_in)
{
	// return if finished
	if (low_in >= high_in) { return; }

	if (low_in+1 == high_in)
	{
		// just swap if they are out of order to sort
		// 2 elements
		//if (!(_data[low_in] <= _data[high_in]))
		if (!_compare(_data[low_in], _data[high_in]))
		{
			T _temp = _data[high_in];
			_data[high_in] = _data[low_in];
			_data[low_in] = _temp;
		}
		return;
	}

	// temporary copies that will change as elements
	// are swapped
	int i_low = low_in;
	int i_high = high_in;

	int i_mid = (i_low + i_high)/2;

	// We will make 2 probably unequal sections of _data.
	// The first, from low_in to i_low will have values less
	// than or equal to the value of pivot_val.
	// The second, from i_high to high_in will have values greater
	// than or equal to pivot_val.
	T pivot_val = _data[i_mid];

	// Move the pivot value to an end, since we don't yet
	// know were the "middle" of the array will be.
	_data[i_mid] = _data[i_high];
	_data[i_high] = pivot_val;

	while (i_low < i_high)
	{
		// find first canidate in "low" part for swapping
		//while((_data[i_low] <= pivot_val) & (i_low < i_high))
		while (_compare(_data[i_low], pivot_val) & (i_low < i_high))
		{
			i_low = i_low+1;
		}

		// find first canidate in "high" part for swapping
		//while((pivot_val <= _data[i_high]) & (i_low < i_high))
		while (_compare(pivot_val, _data[i_high]) & (i_low < i_high))
		{
			i_high = i_high-1;
		}

		// if not crossing in the middle, swap them
		if (i_low < i_high)
		{
			T sT = _data[i_high];
			_data[i_high] = _data[i_low];
			_data[i_low] = sT;
		}
	}

	// Now put pivot value where it goes
	_data[high_in] = _data[i_high];
	_data[i_high] = pivot_val;

	// and sort the two pieces on both sides of the
	// pivot element.
	quick_sort(low_in, i_low-1);
	quick_sort(i_high+1, high_in);
}

// usage 1 : add member
/*

zz_array<int> myarr;

myarr.reserve(100);

for (int i = 0; i < 10; i++) {
	myarr.add(i);
}

for (i = 0; i < myarr.size(); i++) {
	ZZ_LOG("arr: %d\n", myarr[i]);
}
*/

// usage 2 : sort
/*
// sort descending order
bool my_greater_than (int elem1, int elem2)
{
	return (elem1 > elem2);
}

{
	zz_array<int> test_sort;
	for (int i = 0; i < 100; ++i) {
		test_sort.add(rand());
	}

	for (int i = 0; i < test_sort.size(); ++i) {
		ZZ_LOG("view_d3d: attach_window(). sort(%d)\n", test_sort[i]);
	}
	ZZ_LOG("view_d3d: sort() before <-> after\n");
	test_sort.sort(my_greater_than);

	for (int i = 0; i < test_sort.size(); ++i) {
		ZZ_LOG("view_d3d: attach_window(). sort(%d)\n", test_sort[i]);
	}
}
*/

#endif // __ZZ_ARRAY_H__