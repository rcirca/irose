/** 
 * @file zz_autolock.h
 * @brief Auto Lock template for critical section and mutex....
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-dec-2003
 *
 * $Header: /engine/include/zz_autolock.h 3     04-01-21 2:55p Zho $
 * $History: zz_autolock.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-21   Time: 2:55p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-13   Time: 3:54p
 * Updated in $/engine/include
 * oknernel and thread class added
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-12-13   Time: 2:28p
 * Created in $/engine/include
 * 
 */

#ifndef __ZZ_AUTOLOCK_H__
#define __ZZ_AUTOLOCK_H__

#include "zz_critical_section.h"

// Every T class should have methods lock() and unlock().
template<class T>
class zz_autolock {
private:
	T * obj_;
	
public:
	zz_autolock(T& obj) : obj_(&obj)
	{
		obj_->lock();
	}
	zz_autolock(T* pobj) : obj_(pobj)
	{
		obj_->lock();
	}
	~zz_autolock()
	{
		obj_->unlock();
	}
};

// zz_autolock template class specilization of critical section
template<>
class zz_autolock<zz_critical_section> {
private:
	zz_critical_section * cs_;

public:
	zz_autolock(zz_critical_section& cs) : cs_(&cs)
	{
		cs_->enter();
	}
	zz_autolock(zz_critical_section * cs) : cs_(cs)
	{
		cs_->enter();
	}
	~zz_autolock()
	{
		cs_->leave();
	}
};

#endif // __ZZ_AUTOLOCK_H__