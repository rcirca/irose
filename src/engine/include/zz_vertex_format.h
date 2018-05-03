/** 
 * @file zz_vertex_format.h
 * @brief vertex format class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-jan-2004
 *
 * $Header: /engine/include/zz_vertex_format.h 3     04-01-20 10:06a Zho $
 * $History: zz_vertex_format.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-20   Time: 10:06a
 * Updated in $/engine/include
 * Mesh refactoring first version.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-19   Time: 10:40a
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-15   Time: 4:26p
 * Created in $/engine/include
 * Redesigning mesh structure.
 */

#ifndef __ZZ_VERTEX_FORMAT_H__
#define __ZZ_VERTEX_FORMAT_H__

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#define ZZ_VF_NONE         (1 << 0)
#define ZZ_VF_POSITION     (1 << 1)
#define ZZ_VF_NORMAL       (1 << 2)
#define ZZ_VF_COLOR        (1 << 3)
#define ZZ_VF_BLEND_WEIGHT (1 << 4)
#define ZZ_VF_BLEND_INDEX  (1 << 5)
#define ZZ_VF_TANGENT      (1 << 6)
#define ZZ_VF_UV0          (1 << 7)
#define ZZ_VF_UV1          (1 << 8)
#define ZZ_VF_UV2          (1 << 9)
#define ZZ_VF_UV3          (1 << 10)

#define ZZ_VERTEX_FORMAT_TERRAIN (ZZ_VF_POSITION | ZZ_VF_UV0 | ZZ_VF_UV1 | ZZ_VF_UV2)
#define ZZ_VERTEX_FORMAT_OCEAN (ZZ_VF_POSITION | ZZ_VF_UV0)

class zz_vertex_format {
private:
	int format; // vertex format info

public:
	int size; // vertex size in bytes

	int pos_offset;
	int normal_offset;
	int color_offset;
	int blend_weight_offset;
	int blend_index_offset;
	int tangent_offset;
	int uv_offset[4];

public:

	zz_vertex_format (const zz_vertex_format& format_in) :
	  size(format_in.size),
		  pos_offset(format_in.pos_offset),
		  normal_offset(format_in.normal_offset),
		  color_offset(format_in.color_offset),
		  blend_weight_offset(format_in.blend_weight_offset),
		  blend_index_offset(format_in.blend_index_offset),
		  tangent_offset(format_in.tangent_offset)
	  {
		  memset(uv_offset, 0, sizeof(int)*4);
		  set_format(format_in.format);
	  }

	zz_vertex_format (int format_in = ZZ_VF_NONE) :
	  size(0),
	  pos_offset(0),
	  normal_offset(0),
	  color_offset(0),
	  blend_weight_offset(0),
	  blend_index_offset(0),
	  tangent_offset(0)
	{
		memset(uv_offset, 0, sizeof(int)*4);
		set_format(format_in);
	}

	int get_format () const
	{
		return format;
	}

	bool set_format (int format_in)
	{
		format = format_in;

		pos_offset = 0;
		size = sizeof(vec3); // 12

		normal_offset = size; // include position
		size += (use_normal()) ? sizeof(vec3) : 0; // 12

		color_offset = size;
		size += (use_color()) ? sizeof(zz_color) : 0; // 4

		blend_weight_offset = size;
		blend_index_offset = size + ((use_skin()) ? sizeof(vec4) : 0); // 16
		size += (use_skin()) ? 32 : 0; // vec4 + uivec4
		tangent_offset = size;

		size += (use_tangent()) ? sizeof(vec3) : 0; // 12
		uv_offset[0] = size;

		size += (get_num_mapchannel() >= 1) ? 8 : 0;
		uv_offset[1] = size;

		size += (get_num_mapchannel() >= 2) ? 8 : 0;
		uv_offset[2] = size;

		size += (get_num_mapchannel() >= 3) ? 8 : 0;
		uv_offset[3] = size;
		return true;
	}

	bool use_skin (void) const 
	{
		return
			((format & ZZ_VF_BLEND_WEIGHT) && 
			(format & ZZ_VF_BLEND_INDEX));
	}

	bool use_tangent (void) const 
	{
		return (format & ZZ_VF_TANGENT) > 0;
	}

	int get_num_mapchannel (void) const
	{
		int num = 0;
		num = (format & ZZ_VF_UV0) ? num+1 : num;
		num = (format & ZZ_VF_UV1) ? num+1 : num;
		num = (format & ZZ_VF_UV2) ? num+1 : num;
		num = (format & ZZ_VF_UV3) ? num+1 : num;
		return num;
	}

	bool use_normal (void) const
	{
		return (format & ZZ_VF_NORMAL) > 0;
	}

	bool use_color (void) const
	{
		return (format & ZZ_VF_COLOR) > 0;
	}

	bool use_uv (int index) const
	{
		switch (index)
		{
		case 0:
			return (format & ZZ_VF_UV0) > 0;
		case 1:
			return (format & ZZ_VF_UV1) > 0;
		case 2:
			return (format & ZZ_VF_UV2) > 0;
		case 3:
			return (format & ZZ_VF_UV3) > 0;
		}
		return false;
	}
};

#endif // __ZZ_VERTEX_FORMAT_H__
