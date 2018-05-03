/** 
 * @file zz_manager_font.h
 * @brief font manager class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    26-aug-2004
 *
 * $Header: /engine/include/zz_manager_font.h 1     04-08-31 4:46p Zho $
 */

#ifndef __ZZ_MANAGER_FONT_H__
#define __ZZ_MANAGER_FONT_H__

#ifndef __ZZ_MANAGER_H__
#include "zz_manager.h"
#endif

class zz_manager_font : public zz_manager {
protected:
public:
	zz_manager_font ();
	virtual ~zz_manager_font ();
	
	void prepare_font (); // this function should be called out of begin_scene/end_scene block.

	ZZ_DECLARE_DYNAMIC(zz_manager_font);
};

#endif // _ZZ_MANAGER_TEXTURE_H__