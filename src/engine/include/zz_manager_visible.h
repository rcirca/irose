/** 
 * @file zz_manager_visible.h
 * @brief visible manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jun-2004
 *
 * $Header: /engine/include/zz_manager_visible.h 3     04-07-19 12:15p Zho $
 */

#ifndef __ZZ_MANAGER_VISIBLE_H__
#define __ZZ_MANAGER_VISIBLE_H__

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

#ifndef __ZZ_VISIBLE_H__
#include "zz_visible.h"
#endif

class zz_manager_visible : public zz_manager {
protected:

public:
	zz_manager_visible ();
	virtual ~zz_manager_visible ();
	
	ZZ_DECLARE_DYNAMIC(zz_manager_visible);
};

#endif // _ZZ_MANAGER_VISIBLE_H__