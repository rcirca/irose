/** 
 * @file zz_script.h
 * @brief script interface.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    29-nov-2002
 *
 * $Header: /engine/include/zz_script.h 1     03-11-30 8:06p Zho $
 * $History: zz_script.h $
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
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_SCRIPT_H__
#define __ZZ_SCRIPT_H__

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif


class zz_script : public zz_node {
public:
	zz_script (const char * filename = NULL);
	virtual ~zz_script () {}

	virtual bool do_script (const char * filename = NULL, const char * buffer = NULL) = 0;

	virtual void set_param_int (int val) = 0;
	virtual void set_param_float (float val) = 0;
	virtual void set_param_float3 (const float * val) = 0;
	virtual void set_param_string (const char * val) = 0;
	virtual bool call (const char * function_name, va_list arg_list) = 0;

	// get return value after lua call for callXXX interface
	virtual int get_return_int (void) = 0;
	virtual unsigned int get_return_uint (void) = 0;
	virtual float get_return_float (void) = 0;
	virtual const float * get_return_float3 (void) = 0;
	virtual const char * get_return_string (void) = 0;

	ZZ_DECLARE_DYNAMIC(zz_script);
};

#endif // __ZZ_SCRIPT_H__