/** 
 * @file zz_refcount.h
 * @brief reference count.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    08-feb-2002
 *
 * $Header: /engine/include/zz_refcount.h 2     04-01-15 4:26p Zho $
 * $History: zz_refcount.h $
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Updated in $/engine/include
 * Redesigning mesh structure.
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_REFCOUNT_H__
#define __ZZ_REFCOUNT_H__

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif


class zz_refcount {
public:
	zz_refcount () : count(0), locked(false) {}
	~zz_refcount () {}

	virtual void lock () { locked = true; }
	virtual void unlock () { locked = false; }
	virtual void addref () { count++; }
	virtual void release () { count = (--count < 0) ? 0 : count; if (0 == count) zz_delete this;}
	virtual void reset_ref () { count = 0; locked = false; }
	int get_count () { return count; }
	void set_count (int new_count) { count = new_count; }

private:
	int count; // reference count
	bool locked;
};

template<class T>
class zz_refcounted {
public:
	zz_refcounted () : count_(0) {}
	~zz_refcounted () {}

	void addref ()
	{
		count_++;
	}

	void release ()
	{
		count_ = (--count_ < 0) ? 0 : count_;
	}
	
	unsigned int count ()
	{
		return count_;
	}
	
	T& get ()
	{
		return data_;
	}

	bool set (T data_in)
	{
		data_ = data_in;
		count_ = 0;
		addref();
		return true;
	}

private:
	T data_; // real data
	unsigned int count_; // reference count
};

#endif // __ZZ_REFCOUNT_H__