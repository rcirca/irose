/** 
 * @file zz_material_ocean.h
 * @brief ocean material class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    30-jun-2003
 *
 * $Header: /engine/include/zz_material_ocean.h 3     05-05-12 10:18a Choo0219 $
 * $History: zz_material_ocean.h $
 * 
 * *****************  Version 3  *****************
 * User: Choo0219     Date: 05-05-12   Time: 10:18a
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-27   Time: 8:03p
 * Updated in $/engine/include
 * material-texture relation redefined.
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
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_MATERIAL_OCEAN_H__
#define __ZZ_MATERIAL_OCEAN_H__

#ifndef __ZZ_MATERIAL_H__
#include "zz_material.h"
#endif

//--------------------------------------------------------------------------------
class zz_material_ocean : public zz_material {
private:
	void set_first();
	void set_first_light();
	int current_texture_index;
public:

	zz_material_ocean (void);
	virtual ~zz_material_ocean (void);
	virtual bool set (int pass);
	
	// manipulate current texture index
	void set_current_texture_index (int index);
	int get_current_texture_index (void);
	void increase_current_texture_index ();
	void decrease_current_texture_index ();

	ZZ_DECLARE_DYNAMIC(zz_material_ocean)
};

inline void zz_material_ocean::increase_current_texture_index ()
{
	current_texture_index = (++current_texture_index >= get_num_textures()) ? 0 : current_texture_index;
}

inline void zz_material_ocean::decrease_current_texture_index ()
{
	current_texture_index = (--current_texture_index < 0) ? get_num_textures()-1 : current_texture_index;
}

inline void zz_material_ocean::set_current_texture_index (int index)
{
	int size = get_num_textures();
	assert(index < get_num_textures());
	current_texture_index = index;
}

inline int zz_material_ocean::get_current_texture_index (void)
{
	return current_texture_index;
}

#endif __ZZ_MATERIAL_OCEAN_H__