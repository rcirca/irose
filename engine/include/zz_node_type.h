/** 
 * @file zz_node_type.h
 * @brief node type class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    29-may-2002
 *
 * $Header: /engine/include/zz_node_type.h 4     04-07-10 4:31p Zho $
 * $History: zz_node_type.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-07-10   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-03-19   Time: 11:27a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-20   Time: 12:20p
 * Updated in $/engine/include
 * Added IS_A macro.
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
 * User: Zho          Date: 03-11-30   Time: 7:53p
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
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_NODE_TYPE_H__
#define __ZZ_NODE_TYPE_H__

#ifndef __ZZ_MEM_H__
#include "zz_mem.h"
#endif


class zz_node;

#define ZZ_RUNTIME_TYPE(type_name) ((zz_node_type*)(&type_name::type_##type_name))
#define ZZ_DECLARE_DYNAMIC(type_name) \
public: \
	static const zz_node_type type_##type_name; \
	virtual zz_node_type * get_node_type() const; \
	static zz_node * create_instance(); \

#define ZZ_IMPLEMENT_RUNTIME_TYPE(type_name, base_type_name, pfn_create) \
	const zz_node_type type_name::type_##type_name = { \
#type_name, ZZ_RUNTIME_TYPE(base_type_name), pfn_create }; \
	zz_node_type * type_name::get_node_type() const \
		{ return ZZ_RUNTIME_TYPE(type_name); } \

#define ZZ_IMPLEMENT_DYNAMIC(type_name, base_type_name) \
	ZZ_IMPLEMENT_RUNTIME_TYPE(type_name, base_type_name, NULL) \

#define ZZ_IMPLEMENT_DYNCREATE(type_name, base_type_name) \
	zz_node * type_name::create_instance() \
		{ zz_node * new_type = zz_new type_name; return new_type; } \
	ZZ_IMPLEMENT_RUNTIME_TYPE(type_name, base_type_name, type_name::create_instance) \

struct zz_node_type {
	char * type_name;

	zz_node_type * base_type;

	zz_node * (* pfn_create_instance)();

	bool is_a (const zz_node_type * const base_type) const;

	zz_node * create_instance () const
	{
		return (pfn_create_instance)();
	}
};

inline bool zz_node_type::is_a (const zz_node_type * const base_type) const
{
	const zz_node_type * start_type = this;
	while (start_type) {
		if (start_type == base_type) {
			return true;
		}
		start_type = start_type->base_type;
	}
	return false;
}

#define IS_A(INSTANCEPOINTER, CLASSNAME) (INSTANCEPOINTER->is_a(ZZ_RUNTIME_TYPE(CLASSNAME)))

// zz_visible *any_node->create_instance("zz_visible");
//
#endif // __ZZ_NODE_TYPE_H__