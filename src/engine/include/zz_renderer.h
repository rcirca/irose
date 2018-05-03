/** 
 * @file zz_renderer.h
 * @brief renderer interface.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/include/zz_renderer.h 49    05-12-16 8:11p Choo0219 $
 */

#ifndef __ZZ_RENDERER_H__
#define __ZZ_RENDERER_H__

#ifndef __ZZ_POOL_H__
#include "zz_pool.h"
#endif

#ifndef __ZZ_GAMMA_H__
#include "zz_gamma.h"
#endif

#ifndef __ZZ_RENDER_STATE_H__
#include "zz_render_state.h"
#endif

#ifndef __ZZ_NODE_H__
#include "zz_node.h"
#endif

#ifndef __ZZ_DEVICE_RESOURCE_H__
#include "zz_device_resource.h"
#endif

#ifndef __ZZ_ALGEBRA_H__
#include "zz_algebra.h"
#endif

#ifndef __ZZ_VERTEX_FORMAT_H__
#include "zz_vertex_format.h"
#endif

// vertex shader constant
// CAUTION: following macros have to syncronize with "Makefile"
#define ZZ_VSC_WORLDVIEW_TM									0
#define ZZ_VSC_PROJECTION_TM									4
#define ZZ_VSC_LIGHT_TEX										8
#define ZZ_VSC_LIGHT_DIFFUSE								12
#define ZZ_VSC_LIGHT_AMBIENT								13
#define ZZ_VSC_LIGHT_DIRECTION							14
#define ZZ_VSC_CAMERA_POSITION							15
#define ZZ_VSC_ONE_HALF_ZERO_MAXPOWER			16       // [1.0f, .5f, 0.0f, 127.9961f]
#define ZZ_VSC_ALPHA_FOG_START_END_FACTOR	17
#define ZZ_VSC_LIGHTMAP_TRANSFORM					18
#define ZZ_VSC_FOG_START_END_FACTOR				19
#define ZZ_VSC_LIGHT_POSITION								22
#define ZZ_VSC_LIGHT_ATTENUATION						23
#define ZZ_VSC_WORLD_TM										24 // 24-27
//28
//29
#define ZZ_VSC_BONE_TM										30       // upto 95th reg. 3 rows * 22 bones = 66 registers

typedef enum _ZZ_TEXTURESTAGESTATETYPE {
    ZZ_TSS_COLOROP = 1,
    ZZ_TSS_COLORARG1 = 2,
    ZZ_TSS_COLORARG2 = 3,
    ZZ_TSS_ALPHAOP = 4,
    ZZ_TSS_ALPHAARG1 = 5,
    ZZ_TSS_ALPHAARG2 = 6,
    ZZ_TSS_BUMPENVMAT00 = 7,
    ZZ_TSS_BUMPENVMAT01 = 8,
    ZZ_TSS_BUMPENVMAT10 = 9,
    ZZ_TSS_BUMPENVMAT11 = 10,
    ZZ_TSS_TEXCOORDINDEX = 11,
    ZZ_TSS_BUMPENVLSCALE = 22,
    ZZ_TSS_BUMPENVLOFFSET = 23,
    ZZ_TSS_TEXTURETRANSFORMFLAGS = 24,
    ZZ_TSS_COLORARG0 = 26,
    ZZ_TSS_ALPHAARG0 = 27,
    ZZ_TSS_RESULTARG = 28,
    ZZ_TSS_CONSTANT = 32,
    ZZ_TSS_FORCE_DWORD = 0x7fffffff
} ZZ_TEXTURESTAGESTATETYPE;

typedef enum _ZZ_TEXTUREOP {
    ZZ_TOP_DISABLE = 1,
    ZZ_TOP_SELECTARG1 = 2,
    ZZ_TOP_SELECTARG2 = 3,
    ZZ_TOP_MODULATE = 4,
    ZZ_TOP_MODULATE2X = 5,
    ZZ_TOP_MODULATE4X = 6,
    ZZ_TOP_ADD = 7,
    ZZ_TOP_ADDSIGNED = 8,
    ZZ_TOP_ADDSIGNED2X = 9,
    ZZ_TOP_SUBTRACT = 10,
    ZZ_TOP_ADDSMOOTH = 11,
    ZZ_TOP_BLENDDIFFUSEALPHA = 12,
    ZZ_TOP_BLENDTEXTUREALPHA = 13,
    ZZ_TOP_BLENDFACTORALPHA = 14,
    ZZ_TOP_BLENDTEXTUREALPHAPM = 15,
    ZZ_TOP_BLENDCURRENTALPHA = 16,
    ZZ_TOP_PREMODULATE = 17,
    ZZ_TOP_MODULATEALPHA_ADDCOLOR = 18,
    ZZ_TOP_MODULATECOLOR_ADDALPHA = 19,
    ZZ_TOP_MODULATEINVALPHA_ADDCOLOR = 20,
    ZZ_TOP_MODULATEINVCOLOR_ADDALPHA = 21,
    ZZ_TOP_BUMPENVMAP = 22,
    ZZ_TOP_BUMPENVMAPLUMINANCE = 23,
    ZZ_TOP_DOTPRODUCT3 = 24,
    ZZ_TOP_MULTIPLYADD = 25,
    ZZ_TOP_LERP = 26,
    ZZ_TOP_FORCE_DWORD = 0x7fffffff
} ZZ_TEXTUREOP;

typedef enum _ZZ_CMPFUNC {
    ZZ_CMP_NEVER                = 1,
    ZZ_CMP_LESS                 = 2,
    ZZ_CMP_EQUAL                = 3,
    ZZ_CMP_LESSEQUAL            = 4,
    ZZ_CMP_GREATER              = 5,
    ZZ_CMP_NOTEQUAL             = 6,
    ZZ_CMP_GREATEREQUAL         = 7,
    ZZ_CMP_ALWAYS               = 8,
    ZZ_CMP_FORCE_DWORD          = 0x7fffffff, /* force 32-bit size enum */
} ZZ_CMPFUNC;

typedef enum _ZZ_BLEND {
    ZZ_BLEND_ZERO = 1,
    ZZ_BLEND_ONE = 2,
    ZZ_BLEND_SRCCOLOR = 3,
    ZZ_BLEND_INVSRCCOLOR = 4,
    ZZ_BLEND_SRCALPHA = 5,
    ZZ_BLEND_INVSRCALPHA = 6,
    ZZ_BLEND_DESTALPHA = 7,
    ZZ_BLEND_INVDESTALPHA = 8,
    ZZ_BLEND_DESTCOLOR = 9,
    ZZ_BLEND_INVDESTCOLOR = 10,
    ZZ_BLEND_SRCALPHASAT = 11,
    ZZ_BLEND_BOTHSRCALPHA = 12,
    ZZ_BLEND_BOTHINVSRCALPHA = 13,
    ZZ_BLEND_BLENDFACTOR = 14,
    ZZ_BLEND_INVBLENDFACTOR = 15,
    ZZ_BLEND_FORCE_DWORD = 0x7fffffff
} ZZ_BLEND;

typedef enum _ZZ_BLENDOP {
    ZZ_BLENDOP_ADD = 1,
    ZZ_BLENDOP_SUBTRACT = 2,
    ZZ_BLENDOP_REVSUBTRACT = 3,
    ZZ_BLENDOP_MIN = 4,
    ZZ_BLENDOP_MAX = 5,
    ZZ_BLENDOP_FORCE_DWORD = 0x7fffffff
} ZZ_BLENDOP;

typedef enum _ZZ_SAMPLERSTATETYPE {
    ZZ_SAMP_ADDRESSU = 1,
    ZZ_SAMP_ADDRESSV = 2,
    ZZ_SAMP_ADDRESSW = 3,
    ZZ_SAMP_BORDERCOLOR = 4,
    ZZ_SAMP_MAGFILTER = 5,
    ZZ_SAMP_MINFILTER = 6,
    ZZ_SAMP_MIPFILTER = 7,
    ZZ_SAMP_MIPMAPLODBIAS = 8,
    ZZ_SAMP_MAXMIPLEVEL = 9,
    ZZ_SAMP_MAXANISOTROPY = 10,
    ZZ_SAMP_SRGBTEXTURE = 11,
    ZZ_SAMP_ELEMENTINDEX = 12,
    ZZ_SAMP_DMAPOFFSET = 13,
    ZZ_SAMP_FORCE_DWORD = 0x7fffffff
} ZZ_SAMPLERSTATETYPE;

typedef enum _ZZ_TEXTUREADDRESS {
    ZZ_TADDRESS_WRAP = 1,
    ZZ_TADDRESS_MIRROR = 2,
    ZZ_TADDRESS_CLAMP = 3,
    ZZ_TADDRESS_BORDER = 4,
    ZZ_TADDRESS_MIRRORONCE = 5,
    ZZ_TADDRESS_FORCE_DWORD = 0x7fffffff
} ZZ_TEXTUREADDRESS;

typedef enum _ZZ_RENDERSTATETYPE {
    ZZ_RS_ZENABLE = 7,
    ZZ_RS_FILLMODE = 8,
    ZZ_RS_SHADEMODE = 9,
    ZZ_RS_ZWRITEENABLE = 14,
    ZZ_RS_ALPHATESTENABLE = 15,
    ZZ_RS_LASTPIXEL = 16,
    ZZ_RS_SRCBLEND = 19,
    ZZ_RS_DESTBLEND = 20,
    ZZ_RS_CULLMODE = 22,
    ZZ_RS_ZFUNC = 23,
    ZZ_RS_ALPHAREF = 24,
    ZZ_RS_ALPHAFUNC = 25,
    ZZ_RS_DITHERENABLE = 26,
    ZZ_RS_ALPHABLENDENABLE = 27,
    ZZ_RS_FOGENABLE = 28,
    ZZ_RS_SPECULARENABLE = 29,
    ZZ_RS_FOGCOLOR = 34,
    ZZ_RS_FOGTABLEMODE = 35,
    ZZ_RS_FOGSTART = 36,
    ZZ_RS_FOGEND = 37,
    ZZ_RS_FOGDENSITY = 38,
    ZZ_RS_RANGEFOGENABLE = 48,
    ZZ_RS_STENCILENABLE = 52,
    ZZ_RS_STENCILFAIL = 53,
    ZZ_RS_STENCILZFAIL = 54,
    ZZ_RS_STENCILPASS = 55,
    ZZ_RS_STENCILFUNC = 56,
    ZZ_RS_STENCILREF = 57,
    ZZ_RS_STENCILMASK = 58,
    ZZ_RS_STENCILWRITEMASK = 59,
    ZZ_RS_TEXTUREFACTOR = 60,
    ZZ_RS_WRAP0 = 128,
    ZZ_RS_WRAP1 = 129,
    ZZ_RS_WRAP2 = 130,
    ZZ_RS_WRAP3 = 131,
    ZZ_RS_WRAP4 = 132,
    ZZ_RS_WRAP5 = 133,
    ZZ_RS_WRAP6 = 134,
    ZZ_RS_WRAP7 = 135,
    ZZ_RS_CLIPPING = 136,
    ZZ_RS_LIGHTING = 137,
    ZZ_RS_AMBIENT = 139,
    ZZ_RS_FOGVERTEXMODE = 140,
    ZZ_RS_COLORVERTEX = 141,
    ZZ_RS_LOCALVIEWER = 142,
    ZZ_RS_NORMALIZENORMALS = 143,
    ZZ_RS_DIFFUSEMATERIALSOURCE = 145,
    ZZ_RS_SPECULARMATERIALSOURCE = 146,
    ZZ_RS_AMBIENTMATERIALSOURCE = 147,
    ZZ_RS_EMISSIVEMATERIALSOURCE = 148,
    ZZ_RS_VERTEXBLEND = 151,
    ZZ_RS_CLIPPLANEENABLE = 152,
    ZZ_RS_POINTSIZE = 154,
    ZZ_RS_POINTSIZE_MIN = 155,
    ZZ_RS_POINTSPRITEENABLE = 156,
    ZZ_RS_POINTSCALEENABLE = 157,
    ZZ_RS_POINTSCALE_A = 158,
    ZZ_RS_POINTSCALE_B = 159,
    ZZ_RS_POINTSCALE_C = 160,
    ZZ_RS_MULTISAMPLEANTIALIAS = 161,
    ZZ_RS_MULTISAMPLEMASK = 162,
    ZZ_RS_PATCHEDGESTYLE = 163,
    ZZ_RS_DEBUGMONITORTOKEN = 165,
    ZZ_RS_POINTSIZE_MAX = 166,
    ZZ_RS_INDEXEDVERTEXBLENDENABLE = 167,
    ZZ_RS_COLORWRITEENABLE = 168,
    ZZ_RS_TWEENFACTOR = 170,
    ZZ_RS_BLENDOP = 171,
    ZZ_RS_POSITIONDEGREE = 172,
    ZZ_RS_NORMALDEGREE = 173,
    ZZ_RS_SCISSORTESTENABLE = 174,
    ZZ_RS_SLOPESCALEDEPTHBIAS = 175,
    ZZ_RS_ANTIALIASEDLINEENABLE = 176,
    ZZ_RS_MINTESSELLATIONLEVEL = 178,
    ZZ_RS_MAXTESSELLATIONLEVEL = 179,
    ZZ_RS_ADAPTIVETESS_X = 180,
    ZZ_RS_ADAPTIVETESS_Y = 181,
    ZZ_RS_ADAPTIVETESS_Z = 182,
    ZZ_RS_ADAPTIVETESS_W = 183,
    ZZ_RS_ENABLEADAPTIVETESSELATION = 184,
    ZZ_RS_TWOSIDEDSTENCILMODE = 185,
    ZZ_RS_CCW_STENCILFAIL = 186,
    ZZ_RS_CCW_STENCILZFAIL = 187,
    ZZ_RS_CCW_STENCILPASS = 188,
    ZZ_RS_CCW_STENCILFUNC = 189,
    ZZ_RS_COLORWRITEENABLE1 = 190,
    ZZ_RS_COLORWRITEENABLE2 = 191,
    ZZ_RS_COLORWRITEENABLE3 = 192,
    ZZ_RS_BLENDFACTOR = 193,
    ZZ_RS_SRGBWRITEENABLE = 194,
    ZZ_RS_DEPTHBIAS = 195,
    ZZ_RS_WRAP8 = 198,
    ZZ_RS_WRAP9 = 199,
    ZZ_RS_WRAP10 = 200,
    ZZ_RS_WRAP11 = 201,
    ZZ_RS_WRAP12 = 202,
    ZZ_RS_WRAP13 = 203,
    ZZ_RS_WRAP14 = 204,
    ZZ_RS_WRAP15 = 205,
    ZZ_RS_SEPARATEALPHABLENDENABLE = 206,
    ZZ_RS_SRCBLENDALPHA = 207,
    ZZ_RS_DESTBLENDALPHA = 208,
    ZZ_RS_BLENDOPALPHA = 209,
    ZZ_RS_FORCE_DWORD = 0x7fffffff
} ZZ_RENDERSTATETYPE;

typedef enum _ZZ_TEXTURETRANSFORMFLAGS {
    ZZ_TTFF_DISABLE = 0,
    ZZ_TTFF_COUNT1 = 1,
    ZZ_TTFF_COUNT2 = 2,
    ZZ_TTFF_COUNT3 = 3,
    ZZ_TTFF_COUNT4 = 4,
    ZZ_TTFF_PROJECTED = 256,
    ZZ_TTFF_FORCE_DWORD = 0x7fffffff
} ZZ_TEXTURETRANSFORMFLAGS;


typedef enum _ZZ_RENDERWHERE {
	ZZ_RW_NONE      = 0,       // not in begin_scene() end_scene()
	ZZ_RW_SCENE     = 1,       // normal begin_scene() end_scene()
	ZZ_RW_SHADOWMAP = 2,       // in shadow rendering
	ZZ_RW_GLOW = 3, // render glow
	ZZ_RW_TEXTURE = 4, // render to texture
	ZZ_RW_ENVMAP = 5
} ZZ_RENDERWHERE;

typedef enum _ZZ_BLENDTYPE {
	ZZ_BT_NORMAL = 0,
	ZZ_BT_LIGHTEN = 1,
	ZZ_BT_CUSTOM = 3,
	ZZ_BT_NONE = 0xff
} ZZ_BLEND_TYPE;

/* Flags to construct D3DRS_COLORWRITEENABLE */
#define ZZ_COLORWRITEENABLE_RED     (1L<<0)
#define ZZ_COLORWRITEENABLE_GREEN   (1L<<1)
#define ZZ_COLORWRITEENABLE_BLUE    (1L<<2)
#define ZZ_COLORWRITEENABLE_ALPHA   (1L<<3)

//////////////////////////////////////////////////////////////////////////////
// D3DXSPRITE flags:
// -----------------
// D3DXSPRITE_DONOTSAVESTATE
//   Specifies device state is not to be saved and restored in Begin/End.
// D3DXSPRITE_DONOTMODIFY_RENDERSTATE
//   Specifies device render state is not to be changed in Begin.  The device
//   is assumed to be in a valid state to draw vertices containing POSITION0, 
//   TEXCOORD0, and COLOR0 data.
// D3DXSPRITE_OBJECTSPACE
//   The WORLD, VIEW, and PROJECTION transforms are NOT modified.  The 
//   transforms currently set to the device are used to transform the sprites 
//   when the batch is drawn (at Flush or End).  If this is not specified, 
//   WORLD, VIEW, and PROJECTION transforms are modified so that sprites are 
//   drawn in screenspace coordinates.
// D3DXSPRITE_BILLBOARD
//   Rotates each sprite about its center so that it is facing the viewer.
// D3DXSPRITE_ALPHABLEND
//   Enables ALPHABLEND(SRCALPHA, INVSRCALPHA) and ALPHATEST(alpha > 0).
//   ID3DXFont expects this to be set when drawing text.
// D3DXSPRITE_SORT_TEXTURE
//   Sprites are sorted by texture prior to drawing.  This is recommended when
//   drawing non-overlapping sprites of uniform depth.  For example, drawing
//   screen-aligned text with ID3DXFont.
// D3DXSPRITE_SORT_DEPTH_FRONTTOBACK
//   Sprites are sorted by depth front-to-back prior to drawing.  This is 
//   recommended when drawing opaque sprites of varying depths.
// D3DXSPRITE_SORT_DEPTH_BACKTOFRONT
//   Sprites are sorted by depth back-to-front prior to drawing.  This is 
//   recommended when drawing transparent sprites of varying depths.
//////////////////////////////////////////////////////////////////////////////
#define ZZ_SPRITE_DONOTSAVESTATE               (1 << 0)
#define ZZ_SPRITE_DONOTMODIFY_RENDERSTATE      (1 << 1)
#define ZZ_SPRITE_OBJECTSPACE                  (1 << 2)
#define ZZ_SPRITE_BILLBOARD                    (1 << 3)
#define ZZ_SPRITE_ALPHABLEND                   (1 << 4)
#define ZZ_SPRITE_SORT_TEXTURE                 (1 << 5)
#define ZZ_SPRITE_SORT_DEPTH_FRONTTOBACK       (1 << 6)
#define ZZ_SPRITE_SORT_DEPTH_BACKTOFRONT       (1 << 7)

// texture argument constants from d3d9types.h
/*
 * Values for COLORARG0,1,2, ALPHAARG0,1,2, and RESULTARG texture blending
 * operations set in texture processing stage controls in D3DRENDERSTATE.
 */
#define ZZ_TA_SELECTMASK        0x0000000f  // mask for arg selector
#define ZZ_TA_DIFFUSE           0x00000000  // select diffuse color (read only)
#define ZZ_TA_CURRENT           0x00000001  // select stage destination register (read/write)
#define ZZ_TA_TEXTURE           0x00000002  // select texture color (read only)
#define ZZ_TA_TFACTOR           0x00000003  // select ZZ_RS_TEXTUREFACTOR (read only)
#define ZZ_TA_SPECULAR          0x00000004  // select specular color (read only)
#define ZZ_TA_TEMP              0x00000005  // select temporary register color (read/write)
#define ZZ_TA_CONSTANT          0x00000006  // select texture stage constant
#define ZZ_TA_COMPLEMENT        0x00000010  // take 1.0 - x (read modifier)
#define ZZ_TA_ALPHAREPLICATE    0x00000020  // replicate alpha to color components (read modifier)

#define NUM_STAGES 8

struct zz_viewport { // same as D3DVIEWPORT9
	dword x;
	dword y;
	dword width;
	dword height;
    float minz; // 0.0f
    float maxz; // 1.0f
};

//--------------------------------------------------------------------------------
struct zz_renderer_cached_info {
//--------------------------------------------------------------------------------
	enum e_type {
		TEXTURE0 = 0,
		TEXTURE1 = 1,
		TEXTURE2 = 2,
		TEXTURE3 = 3,
		TEXTURE4 = 4,
		TEXTURE5 = 5,
		TEXTURE6 = 6,
		TEXTURE7 = 7, // NUM_STAGES - 1
		VERTEX_SHADER = 100,
		PIXEL_SHADER = 101,
		VERTEX_BUFFER = 102,
		INDEX_BUFFER = 103,
		LIGHT = 104,
		ALL = 255
	};

	bool texture[NUM_STAGES];
	bool vertex_shader;
	bool pixel_shader;
	bool vertex_buffer;
	bool index_buffer;
	bool light;

	zz_renderer_cached_info ()
	{
		invalidate();
	}

	void invalidate_texture (int stage = NUM_STAGES)
	{
		if (stage == NUM_STAGES) { // invalidate all texture stages
			memset(texture, 0, NUM_STAGES);
		}
		else {
			texture[stage] = false;
		}
	}

	void invalidate (e_type type = ALL)
	{
		if (type == ALL) {
			vertex_shader = false;
			pixel_shader = false;
			vertex_buffer = false;
			index_buffer = false;
			memset(texture, 0, NUM_STAGES);
			light = false;
			return;
		}
		switch (type)
		{
		case TEXTURE0:
			texture[0] = false; break;
		case TEXTURE1:
			texture[1] = false; break;
		case TEXTURE2:
			texture[2] = false; break;
		case TEXTURE3:
			texture[3] = false; break;
		case TEXTURE4:
			texture[4] = false; break;
		case TEXTURE5:
			texture[5] = false; break;
		case TEXTURE6:
			texture[6] = false; break;
		case TEXTURE7:
			texture[7] = false; break;
		case VERTEX_SHADER:
			vertex_shader = false; break;
		case PIXEL_SHADER:
			pixel_shader = false; break;
		case VERTEX_BUFFER:
			vertex_buffer = false; break;
		case INDEX_BUFFER:
			index_buffer = false; break;
		case LIGHT:
			light = false; break;
		}
	}
};

// renderer interface
class zz_scene;
class zz_camera;
class zz_mesh;
class zz_material;
class zz_light;
class zz_light_direct;
class zz_shader;
class zz_texture;
class zz_view;

//--------------------------------------------------------------------------------
class zz_renderer : public zz_node {
public:
	enum zz_renderer_light_property {
		ZZ_RLP_AMBIENT,
		ZZ_RLP_DIFFUSE,
		ZZ_RLP_SPECULAR,
		ZZ_RLP_POSITION,
		ZZ_RLP_DIRECT
	};

	enum zz_renderer_light_type {
		ZZ_RLT_OMNI,
		ZZ_RLT_DIRECTIONAL,
		ZZ_RLT_SPOT
	};

	zz_renderer () : zz_node() {};
	virtual ~zz_renderer () {};

	// render
	virtual void render (zz_mesh * mesh, zz_material * material, zz_light * light) = 0;
	virtual bool draw_indexed_trilist (zz_mesh * mesh) = 0;
	virtual bool draw_indexed_tristrip (zz_mesh * mesh) = 0;
	virtual bool set_stream_buffer (zz_mesh * mesh) = 0;
	virtual void set_light (zz_light * light) = 0;
	virtual bool set_material (zz_material * material) = 0;

	// rendering
	virtual void swap_buffers (HWND hwnd) = 0;
	virtual void clear_screen () = 0;
	virtual void clear_zbuffer () = 0;
	virtual void begin_scene (ZZ_RENDERWHERE render_where) = 0;
	virtual void end_scene () = 0;
	virtual bool scene_began () = 0;
	virtual void init_render_state () = 0;
	virtual void init_scene () = 0;
	virtual void init_textures () = 0;

	// init
	virtual bool initialize () = 0;
	virtual void cleanup () = 0;
	virtual bool is_active () = 0;

	// relates to vertex buffer & index buffer
	virtual zz_handle create_vertex_buffer (const zz_device_resource& vres, int buffer_size) = 0;
	virtual zz_handle create_index_buffer (const zz_device_resource& ires, int buffer_size) = 0;
	virtual bool update_vertex_buffer (const zz_device_resource& vres, zz_handle vhandle, const void * buf, int size, int offset) = 0;
	virtual bool update_index_buffer (const zz_device_resource& ires, zz_handle ihandle, const void * buf, int size, int offset) = 0;
	virtual void destroy_vertex_buffer (zz_handle handle_in) = 0;
	virtual void destroy_index_buffer (zz_handle handle_in) = 0;

	// vertex shader
	virtual bool set_vertex_shader (zz_handle shader_index) = 0;
	virtual int create_vertex_shader (const char * shader_file_name,
        int vertex_format, bool is_binary = false) = 0;
	virtual void destroy_vertex_shader (int vertex_shader_handle) = 0;
	virtual bool support_vertex_shader () = 0;
	virtual void set_vertex_shader_constant (int register_index, const void * constant_data, int coustant_count) = 0;
	virtual void set_vertex_shader_constant_matrix (int index, const mat4& matrix_to_set_modelview, int count = 4) = 0;

	// pixel shader
	virtual bool set_pixel_shader (zz_handle shader_index) = 0;
	virtual bool support_pixel_shader () = 0;
	virtual int create_pixel_shader (const char * shader_file_name, bool is_binary = false) = 0;
	virtual void destroy_pixel_shader (int pixel_shader_handle) = 0;
	virtual void set_pixel_shader_constant (int register_index, const void * constant_data, int coustant_count) = 0;

	virtual void set_view (zz_view * view_to_set) = 0;
	virtual zz_view * get_view () = 0;

	// texture
	virtual zz_handle create_texture (zz_texture * tex) = 0; // returns texture id
	virtual zz_handle update_texture (zz_texture * tex) = 0; // returns texture id
	virtual void destroy_texture (zz_texture * tex) = 0;

	virtual bool set_texture (zz_handle texture_handle, int stage, const char * texture_name) = 0;

	//virtual void set_texture_stage_mixmode (
	//	zz_material_mixer::zz_material_operation mixmode) = 0;
	virtual void set_texture_shadowmap (int shadowmap_stage) = 0;

	//virtual zz_scene * get_scene () = 0;
	//virtual void set_scene (zz_scene * new_scene) = 0;

	virtual const mat4& get_modelview_matrix () = 0;
	virtual const mat4& get_projection_matrix () = 0;
	virtual const mat4& get_world_matrix () = 0;
	virtual void set_modelview_matrix (const mat4& matrix_to_set) = 0;
	virtual void set_projection_matrix (const mat4& matrix_to_set) = 0;
	virtual void set_world_matrix (const mat4& matrix_to_set) = 0;
	virtual void set_texture_matrix (int texture_stage, int shader_constant_index, const mat4& matrix_to_set) = 0;

	// render state
	virtual bool enable_zbuffer       (bool true_or_false) = 0;
	virtual bool enable_alpha_blend    (bool true_or_false, int blend_type) = 0;
	virtual bool enable_zwrite        (bool true_or_false) = 0;
	virtual void enable_fog           (bool true_or_false) = 0;
	virtual bool enable_alpha_test (bool true_or_false, ulong alpha_ref = 0x80, ZZ_CMPFUNC cmp_func = ZZ_CMP_GREATEREQUAL) = 0;
	virtual void set_fog_range (float fog_start, float fog_end) = 0;
	virtual void set_alpha_fog_range (float fog_start, float fog_end) = 0;

	virtual void set_alpha_ref (int alpha_ref_in) = 0;
	virtual void set_cullmode (zz_render_state::zz_cull_mode_type cullmode) = 0;
	
	virtual zz_render_state * get_state (void) = 0;

	virtual void matrix_push (const mat4& in_push) = 0;
	virtual mat4 matrix_pop (void) = 0;

	virtual void draw_line (vec3 from, vec3 to, vec3 rgb) = 0;
    virtual void draw_arrow(float size, int color) = 0;
	virtual void draw_axis(float size) = 0;
	virtual void draw_axis(float *q, float *v, float size) = 0;
	virtual void draw_camera() = 0;
	virtual void draw_camera_frustum() = 0; 
    virtual void draw_camera_ex(const mat4& view_mat) = 0;

	virtual bool set_normalization_cubemap_texture (int stage) = 0;

	virtual void shadowmap_texturetm_setup (void) = 0;
	virtual void begin_shadowmap (void) = 0;
	virtual void end_shadowmap (void) = 0;
	virtual void blur_shadowmap (int repeat_count = 1) = 0;
	virtual void draw_shadowmap_viewport (void) = 0;

	// resize back buffer by state.screen_width/height
	virtual bool resize (void) = 0;

	virtual void set_depthbias (int bias) = 0;

	// gamma control section
	virtual bool set_gamma (const zz_gamma& gamma) = 0;
	virtual bool get_gamma (zz_gamma& gamma) = 0;
	virtual bool set_gamma_by_value (float gamma_value) = 0;

	// set texture state (from d3d)
	virtual bool set_texture_stage_state (ulong stage, ZZ_TEXTURESTAGESTATETYPE type, ulong value) = 0;

	virtual bool set_blend_type (ZZ_BLEND src, ZZ_BLEND dest, ZZ_BLENDOP = ZZ_BLENDOP_ADD) = 0;

	virtual void render_shadowmap_overlay() = 0;

	virtual void set_sampler_state (int stage, ZZ_SAMPLERSTATETYPE state_type, ulong value) = 0;

	virtual void set_render_where (ZZ_RENDERWHERE rwhere_in) = 0;
	virtual ZZ_RENDERWHERE get_render_where () = 0;

	virtual bool set_texture_factor (const uivec4& factor) = 0;

	virtual bool set_alphafunc (ZZ_CMPFUNC val) = 0;
	virtual bool set_zfunc (ZZ_CMPFUNC val) = 0;

	virtual bool set_render_state (ZZ_RENDERSTATETYPE state, ulong value) = 0;

	virtual bool save_to_file (const char * filename, int format_dds0_bmp1_jpg2) = 0;

	virtual unsigned int get_max_texmem () = 0; // get max texture memory in bytes.
	virtual unsigned int get_available_texmem () = 0; // get currently available texture memory in bytes.

	virtual bool begin_sprite (int flag, const char * spritename) = 0;
	virtual bool end_sprite () = 0;
	virtual void init_sprite_state () = 0;
	virtual void init_sprite_transform (int surface_width, int surface_height) = 0;
	virtual bool flush_sprite () = 0;
	virtual bool sprite_began () = 0;
	virtual bool draw_sprite (zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color) = 0;
    virtual bool draw_sprite_ex ( zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color) = 0;
	virtual bool draw_sprite_cover (zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 origin_color, color32 cover_color, float value) = 0;
    virtual void draw_wire_sphere(float x, float y, float z, float r) = 0;
    virtual void draw_wire_cylinder(float x, float y, float z, float length, float r) = 0;
	virtual void draw_visible_boundingbox(const mat4& matrix, float min_vec[3], float max_vec[3],DWORD color) = 0;
    virtual void draw_axis_object(mat4& object_matrix, float size) = 0;

	// get renderer cached info
	virtual zz_renderer_cached_info& get_cached_info () = 0;

	// for statistics. call after end_scene() or before begin_scene()
	virtual int get_num_meshes () = 0;
	virtual int get_num_polygons () = 0;
	virtual int get_num_textures () = 0;
	
	// render final blurred glowmap into the backbuffer
	virtual void overlay_glow (bool object_glow, bool fullscene_glow) = 0;

	// checks current device states, and reset device if it is necessary.
	// returns device_lost
	virtual bool reset_device () = 0;

#ifdef _DEBUG
	// only for debugging
	virtual void test() = 0;
#endif

	virtual void begin_glow () = 0;
	virtual void end_glow () = 0;

	virtual void pre_process () = 0;
	virtual void post_process () = 0; // post processing

	virtual bool has_device () = 0;

	virtual bool check_glowable () = 0; // check if device support glow effect or not.
	virtual bool check_shadowable () = 0; // check if device support shadowmap or not.

	virtual bool support_hw_mouse () = 0;

	virtual bool validate_device () = 0;

	virtual void invalidate_texture ( int stage ) = 0;

	virtual void get_viewport (zz_viewport& viewport) = 0;

	virtual void set_viewport (const zz_viewport& viewport) = 0;

	virtual bool support_color_write () = 0;

	virtual void enable_color_write (dword flag) = 0;

	virtual int get_num_simultaneous_render_target () = 0;

	virtual bool get_device_lost () const = 0;

	ZZ_DECLARE_DYNAMIC(zz_renderer);
};

#endif // __ZZ_RENDERER_H__