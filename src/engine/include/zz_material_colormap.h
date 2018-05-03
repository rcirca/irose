/** 
 * @file zz_material_colormap.h
 * @brief material colormap class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    23-jul-2002
 *
 * $Header: /engine/include/zz_material_colormap.h 12    04-07-26 11:43a Zho $
 * $History: zz_material_colormap.h $
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-07-26   Time: 11:43a
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-07-24   Time: 10:01p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-07-08   Time: 7:10p
 * Updated in $/engine/include
 * tfactor related
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-07-06   Time: 6:24p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-06-08   Time: 4:50p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-05-19   Time: 8:14p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-17   Time: 2:10p
 * Updated in $/engine/include
 * Updated for specular material to be transparent by setVisibility.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-06   Time: 1:25p
 * Updated in $/engine/include
 * Added 2pass lightmap shadowed.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-01-27   Time: 7:00p
 * Updated in $/engine/include
 * Support material_sky.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-01-10   Time: 10:41a
 * Updated in $/engine/include
 * Added irradiancemap
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
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
 * *****************  Version 13  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef __ZZ_MATERIAL_COLORMAP_H__
#define __ZZ_MATERIAL_COLORMAP_H__

#ifndef __ZZ_MATERIAL_H__
#include "zz_material.h"
#endif

class zz_renderer;
class zz_shader;

//--------------------------------------------------------------------------------
class zz_material_colormap : public zz_material {
protected:
	float lightmap_transform[4]; // 0 : U translation index, 1 : V translation index, 2 : 1/width, 3 : 1/height

	void _set_colormap_specularmap (void);
	void _set_tfactor_texture_alpha ();
	void _set_texture_texture_alpha (void);
	void _set_texture_x_tfactor_alpha (void);
	void _set_diffuse_alpha (void);
	void _set_texture_alpha (void);
	void _set_tfactor_alpha (void);
	void _set_nop_alpha (void);
	void _set_texture_x_diffuse_alpha (void);
	void _set_colormap (void);
	void _set_shadowmap (void);
	void _set_tfactor_colormap (void);
	void _set_colormap_lightmap (void);
	void _set_colormap_shadowmap (void);
	void _set_colormap_lightmap_shadowmap (void);
	void _set_irradiancemap (void);
	void _set_glow (void);
	void _set_glow_lit (void);
	void _set_glow_texture (void);
	void _set_glow_texture_lit (void);
	void _set_glow_alpha (void);
	void _set_target_shadowmap (void);
	void _set_target_glow (void);

	// set each pass
	void _set_pass_fogged (void);
	void _set_pass_diffusemap (void);
	void _set_pass_diffusemap_specularmap (void);
	void _set_pass_diffusemap_lightmap (void);
	void _set_pass_diffusemap_lightmap_shadowmap (void);
	void _set_pass_diffusemap_shadowmap (void);
	void _set_pass_lightmap (void);
	void _set_pass_lightmap_shadowmap (void);
	void _set_pass_shadowmap (void);
	
	void bind_texture (void);

public:

	zz_material_colormap (void);
	virtual ~zz_material_colormap (void);

	virtual bool set (int pass);

	virtual int get_shader_format () const;

	void set_lightmap_transform (int x, int y, int width, int height)
	{
		lightmap_transform[0] = float(x);
		lightmap_transform[1] = float(y);
		lightmap_transform[2] = 1.0f/float(width);
		lightmap_transform[3] = 1.0f/float(height);
	}

	ZZ_DECLARE_DYNAMIC(zz_material_colormap)
};

#endif // __ZZ_MATERIAL_COLORMAP_H__