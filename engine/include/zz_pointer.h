/** 
 * @file zz_pointer.h
 * @brief smart pointer.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    08-feb-2002
 *
 * $Header: /engine/include/zz_pointer.h 6     04-05-19 8:14p Zho $
 * $History: zz_pointer.h $
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-05-19   Time: 8:14p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Updated in $/engine/include
 * Redesigning mesh structure.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-14   Time: 12:19p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-14   Time: 12:00p
 * Updated in $/engine/include
 * Upgraded smart pointer interface.
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-27   Time: 4:50p
 * Updated in $/znzin11/engine/include
 * - removed zz_resource class, added full-support of resource management
 * into zz_node, and cleaned up some codes.
 */

#ifndef	__ZZ_POINTER_H__
#define __ZZ_POINTER_H__

#include <cassert>

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif


#pragma warning(disable : 4150 )

// smart pointer template which offers automatic reference counting
// [REFERENCE]
//   http://www.flipcode.com/cgi-bin/msg.cgi?showThread=COTD-SmartPointerTemplateClass&forum=cotd&id=-1
//   http://web.ftech.net/~honeyg/articles/smartp.htm
//   http://www.awl.com/cseng/titles/0-201-70431-5 : �ڡڡڡڡ�
template <class T>
class zz_pointer {
	// friend functions are used not to confuse "->release()" and ".release()".
	template<class T>
		friend void
		set_impl (zz_pointer<T>& sp, T * p);

	// is it a valid pointer? (data valification)
	template<class T>
		friend bool
		is_valid (const zz_pointer<T>& sp);

	// get internal pointer
	template<class T>
		friend T *
		get_impl (zz_pointer<T>& sp);

	// get internal constant pointer
	template<class T>
		friend const T *
		get_impl (const zz_pointer<T>& sp);

	// get internal pointer reference
	template<class T>
		friend T *&
		get_implref (zz_pointer<T>& sp);

	// get internal constant pointer reference
	template<class T>
		friend const T *&
		get_implref (const zz_pointer<T>& sp);

	// get reference count
	template<class T>
		friend int
		get_count (const zz_pointer<T>& sp);

	// add reference count
	template<class T>
		friend void
		addref (zz_pointer<T>& sp);

	// release pointer
	template<class T>
		friend void
		release (zz_pointer<T>& sp);

private:
	T *	pointee_;
	int * count_;

public:
	zz_pointer() : pointee_(0), count_(0)
	{
	}
	
	explicit zz_pointer(T * p) : pointee_(0), count_(0)
	{
		set_impl<T>(*this, p);
	}
	
	~zz_pointer()
	{
		release(*this);
	}

public:

	// receive pointer value from pure pointer or smart pointer
	void operator=(T * p)
	{
		set_impl(*this, p);
	}

	void operator=(const zz_pointer &p)
	{
		release(*this);

		pointee_ = p.pointee_;
		count_ = p.count_;
		(*count_)++;
	}

	// get the real pointer value
	T * operator->() const
	{
		return pointee_;
	}

	// get the real pointer's content
	T & operator* () const
	{
		return *pointee_;
	}

	operator T * () const
	{
		return pointee_;
	}

	// not to allow "delete zz_poinetr type object".
	operator void * () const
	{
		return pointee_;
	}
};


// friend functions are used not to confuse "->release()" and ".release()".
template<class T>
inline void
//--------------------------------------------------------------------------------
set_impl (zz_pointer<T>& sp, T * p)
//--------------------------------------------------------------------------------
{
	release(sp);
	sp.pointee_ = p;
	sp.count_ = zz_new int;
	*sp.count_ = 1;
}

// is it a valid pointer? (data valification)
template<class T>
inline bool
//--------------------------------------------------------------------------------
is_valid (const zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	if (sp.pointee_) {
		return true;
	}
	return false;
}

// get internal pointer
template<class T>
inline T *
//--------------------------------------------------------------------------------
get_impl (zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	return sp.pointee_;
}

// get internal constant pointer
template<class T>
inline const T *
//--------------------------------------------------------------------------------
get_impl (const zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	return sp.pointee_;
}

// get internal pointer reference
template<class T>
inline T *&
//--------------------------------------------------------------------------------
get_implref (zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	return sp.pointee_;
}

// get internal constant pointer reference
template<class T>
inline const T *&
//--------------------------------------------------------------------------------
get_implref (const zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	return sp.pointee_;
}

template<class T>
inline int
//--------------------------------------------------------------------------------
get_count (const zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	if (sp.count_) {
		return (*sp.count_);
	}
	return 0;
}

template<class T>
inline void
//--------------------------------------------------------------------------------
addref (zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	if (sp.count_) {
		(*sp.count_)++;
	}
}

template<class T>
inline void
//--------------------------------------------------------------------------------
release (zz_pointer<T>& sp)
//--------------------------------------------------------------------------------
{
	if (!sp.count_) return;

	(*sp.count_)--;

	if (0 == (*sp.count_)) {
		if (sp.pointee_) {
			zz_delete sp.pointee_;
			sp.pointee_ = 0;
		}
		zz_delete sp.count_;
		sp.count_ = 0;
	}
}

//template<class T>
////--------------------------------------------------------------------------------
//void reset (zz_pointer<T>& sp, T* source)
////--------------------------------------------------------------------------------
//{
//	T* temp = source;
//	source = sp.pointee_;
//	sp.pointee_ = source;
//}

#endif // __ZZ_POINTER_H__
