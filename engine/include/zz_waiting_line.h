/** 
 * @file zz_waiting_line.h
 * @brief waiting line class for zz_manager.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    27-oct-2003
 *
 * $Header: /engine/include/zz_waiting_line.h 9     04-09-20 1:39p Zho $
 */

#ifndef __ZZ_WAITING_LINE_H__
#define __ZZ_WAITING_LINE_H__

#include <list>
#include <assert.h>

template<class T>
class zz_waiting_line {
public:
	enum zz_usage {
		FOR_NONE = 0,
		FOR_ENTRANCE = 1,
		FOR_EXIT = 2
	};

private:
	typedef std::list<T *> zz_line_list;
	zz_line_list _line;
	zz_usage _usage;
	size_t _max_size;
	
public:

	zz_waiting_line (zz_usage usage_in = FOR_NONE, size_t max_size_in = 0)
		: _usage(usage_in), _max_size(max_size_in) {}
	~zz_waiting_line () {}

	void set (zz_usage usage_in, size_t max_size_in);

	bool remove (T * node);
	T * pop ();
	// return true if really pushed into list
	bool push (T * node);

	bool empty ();
	size_t size ();
	size_t flush_n_pop (size_t num_update = 0); // flush nodes and pop
	bool flush_node (T * node); // direct flush
	bool flush_all ();
	
	bool find (T * node_to_find);

	bool to_front (T * node_to_front);
	bool to_back (T * node_to_back);

	typedef bool (zz_sort_compare_func) (const T * A, const T * B);

	class zz_waiting_line_compare_func : public std::binary_function<T *, T *, bool> 
	{
	public:
		bool operator() (const T * lhs, const T * rhs) const 
		{
			return lhs->get_load_priority() > rhs->get_load_priority();
		}
	};

	void sort_line ()
	{
		zz_waiting_line_compare_func cf;
		_line.sort(cf);
	}

	T * back ();

	void dump (void) // for debug only
	{
		zz_line_list::iterator it;
		ZZ_LOG("waiting_line: dump()\n");
		for (it = _line.begin(); it != _line.end(); ++it) {
			ZZ_LOG("[%s]-", (*it)->get_name());
		}
	}
};

template<class T>
bool zz_waiting_line<T>::empty ()
{
	return _line.empty();
}

template<class T>
bool zz_waiting_line<T>::flush_all ()
{
	size_t num_line = _line.size();
	return (flush_n_pop(size_t(0)) == num_line);
}

template<class T>
size_t zz_waiting_line<T>::size ()
{
	return _line.size();
}

template<class T>
T * zz_waiting_line<T>::back ()
{
	if (_line.empty()) return NULL;
	return _line.back();
}


template<class T>
void zz_waiting_line<T>::set (zz_usage usage_in, size_t max_size_in)
{
	if ((_usage != usage_in) && (_usage != zz_waiting_line<T>::FOR_NONE)) {
		ZZ_LOG("waiting_line: set(%d, %d) failed. cannot reset\n", usage_in, max_size_in);
		return; // cannot reset 
	}
	_usage = usage_in;
	_max_size = max_size_in;
}

template<class T>
T * zz_waiting_line<T>::pop ()
{
	if (_line.empty()) return NULL;

	T * popped = _line.back();
	_line.pop_back();
	return popped;
}

// return true if flushing was succeeded
template<class T>
bool zz_waiting_line<T>::flush_node (T * node)
{
	assert(node);
	if (!node) return false;
	if (_usage == FOR_ENTRANCE) {
		//ZZ_LOG("waiting_line: flush(%s) load()\n", node->get_name());
		return node->load(); // <-> unload()
	}
	else if (_usage == FOR_EXIT) { // for exit
		//ZZ_LOG("waiting_line: flush(%s) unload()\n", node->get_name());
		return node->unload_and_release(); // <-> load()
	}
	return true;
}

template<class T>
bool zz_waiting_line<T>::push (T * node)
{
	assert(node);
	if (_line.size() >= _max_size) {
		// flush directly
		if (_max_size > 0) {
			//assert(!"push exceeded max size");
		}
		flush_node(node);
		return false; // not pushed into the line
	}
	else {
		if (find(node)) { // already here
			//ZZ_LOG("waiting_line: %s push(%s). found already\n", (_usage == FOR_ENTRANCE) ? "ent" : "ext", node->get_name());
			return true;
		}
		_line.push_front(node);
		//ZZ_LOG("waiting_line: %s push_front(%s)\n", (_usage == FOR_ENTRANCE) ? "ent" : "ext", node->get_name());
		if ((_usage) == FOR_EXIT) {
			// clean up manager related
			//ZZ_LOG("waiting_line:push(%s):exit removed\n", node->get_name());
			node->remove_name(); // not to be able to find this by zz_node::find()
		}
	}
	return true;
}

// flush *num_update* nodes
// if num_update = 0, then flush all
template<class T>
size_t zz_waiting_line<T>::flush_n_pop (size_t num_update)
{
	T * node;
	size_t count = 0, i;
	if (num_update == 0) num_update = _line.size();

	for (i = 0; i < num_update; ++i) {
		if (_line.empty()) break;
		node = _line.back();
		assert(node);
		if (flush_node(node)) {
			pop();
			++count;
		}
		else {
			zz_assert(0); // "waiting_line: flush failed");
			return count; // blocked
		}
	}
	return count;
}

// Move this node to the top-front position
template<class T>
bool zz_waiting_line<T>::to_front (T * node_to_front)
{
	zz_line_list::iterator it;
	for (it = _line.begin(); it != _line.end(); ++it) {
		if (*it == node_to_front) {
			_line.erase(it);
			_line.push_front(node_to_front);
			return true;
		}
	}
	return false;
}

// Move this node to the top-back position
template<class T>
bool zz_waiting_line<T>::to_back (T * node_to_back)
{
	zz_line_list::iterator it;

	//ZZ_LOG("waiting_line:to_back(%s)\n", node_to_back->get_name());
	for (it = _line.begin(); it != _line.end(); ++it) {
		if (*it == node_to_back) {
			_line.erase(it);
			_line.push_back(node_to_back);
			return true;
		}
	}
	return false;
}

template<class T>
bool zz_waiting_line<T>::remove (T * node)
{
	zz_line_list::iterator it, it_end;

	for (it = _line.begin(), it_end = _line.end(); it != it_end; ++it) {
		if (*it == node) {
			_line.erase(it);
			return true;
		}
	}
	return false; // not found
}

template<class T>
bool zz_waiting_line<T>::find (T * node_to_find)
{
	zz_line_list::iterator it, it_end;

	for (it = _line.begin(), it_end = _line.end(); it != it_end; ++it) {
		if (*it == node_to_find) {
			return true;
		}
	}
	return false; // not found
}

#endif // __ZZ_WAITING_LINE_H__