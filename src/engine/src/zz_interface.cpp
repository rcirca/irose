/** 
 * @file zz_interface.cpp
 * @brief znzin external interface class 
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    28-nov-2002
 *
 * $Header: /engine/src/zz_interface.cpp 288   07-04-06 9:17p Choo0219 $
 */

//-----------------------------------------------------------------------------------------------
//#define FORCE_LOGGING
//#define PROFILE_INTERFACE // activate this, if you want to log interface profiling information
//#define LOG_SWAPTIME
//#define PROFILE_NORMAL
//-----------------------------------------------------------------------------------------------

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_system.h"
#include "zz_motion.h"
#include "zz_motion_tool.h"
#include "zz_model.h"
#include "zz_camera_follow.h"
#include "zz_render_state.h"
#include "zz_ik_limb.h"
#include "zz_visible.h"
#include "zz_bvolume.h"
#include "zz_script_simple.h"
#include "zz_skeleton.h"
#include "zz_gamma.h"
#include "zz_terrain_block.h"
#include "zz_ocean_block.h"
#include "zz_morpher.h"
#include "zz_sky.h"
#include "zz_profiler.h"
#include "zz_particle_emitter.h"
#include "zz_trail.h"
#include "zz_texture.h"
#include "zz_material_ocean.h"
#include "zz_material_colormap.h"
#include "zz_material_terrain.h"
#include "zz_material_sky.h"
#include "zz_os.h"
#include "zz_vfs.h"
#include "zz_vfs_pkg.h"
#include "zz_vfs_local.h"
#include "zz_vfs_mem.h"
#include "zz_manager.h"
#include "zz_font.h"
#include "zz_font_d3d.h"
#include "zz_renderer.h"
#include "zz_renderer_d3d.h"
#include "zz_script_lua.h"
#include "zz_view.h"
#include "zz_light_direct.h"
#include "zz_light_point.h"
#include "zz_interface.h"
#include "zz_bone.h"
#include "zz_shader.h"
#include "zz_mesh.h"
#include "zz_mesh_terrain.h"
#include "zz_mesh_ocean.h"
#include "zz_mesh_tool.h"
#include "zz_manager_sound.h"
#include "zz_manager_font.h"
#include "zz_sound.h"
#include "zz_path.h"
#include "zz_vfs_thread.h"
#include "zz_sfx.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_channel_xy.h"
#include "zz_channel_x.h"
#include "zz_assert.h"
#include "zz_dx_version.h"
#include "zz_cursor.h"
#include "zz_primitive.h"

#include <stdarg.h>
#include <algorithm> // checkThreadTextures

#define ISTRUE(int_val) (int_val != 0)

#pragma warning(disable : 4312) // to disable warning reinterpret_cast HNODE to greater size
#pragma warning(disable : 4311) // to disable pointer truncation

#ifdef PROFILE_INTERFACE
#define CHECK_INTERFACE(interface_name) ZZ_PROFILER_INSTALL(interface_name)
#else
#define CHECK_INTERFACE(interface_name)
#endif

#ifdef PROFILE_MATERIAL
#define CHECK_INTERFACE_MATERIAL(interface_name) CHECK_INTERFACE(interface_name)
#else
#define CHECK_INTERFACE_MATERIAL(interface_name)
#endif

#ifdef PROFILE_FONT
#define CHECK_INTERFACE_FONT(interface_name) CHECK_INTERFACE(interface_name)
#else
#define CHECK_INTERFACE_FONT(interface_name)
#endif

#ifdef PROFILE_SPRITE
#define CHECK_INTERFACE_SPRITE(interface_name) CHECK_INTERFACE(interface_name)
#else
#define CHECK_INTERFACE_SPRITE(interface_name)
#endif

#define CHECK_INTERFACE_FORCE(interface_name) ZZ_PROFILER_INSTALL(interface_name)

//=================================
// system global variables setting
//=================================

#define MAX_FLOAT_ARRAY 256
#define ZZ_INFINITE (999999999.f)

#define FLOAT3_TO_VEC3(PFLT) (*(reinterpret_cast<vec3*>( PFLT )))

#define SPECULAR_SPHEREMAP_PATH "etc/specular_spheremap.dds"

float float_array[MAX_FLOAT_ARRAY];

long frame_count = 0; // debugging frame count. increased by swapbuffer

// for intersect query
// first test *include* and, then *exclude*
//int include_classes = 
//	ZZ_TYPE_VISIBLE | ZZ_TYPE_ANIMATABLE
//    | ZZ_TYPE_MORPHER | ZZ_TYPE_SKELETON | ZZ_TYPE_MODEL
//    | ZZ_TYPE_PARTICLE | ZZ_TYPE_TRAIL | ZZ_TYPE_TERRAIN; // initially include all classes
int include_classes = ZZ_TYPE_TERRAIN; // initially include all classes
int exclude_classes = ZZ_TYPE_NONE; // initially exclude all classes

bool save_screenshot_once = false; // for save screenshot

zz_render_state * state = NULL;

#ifdef _WIN32
extern HINSTANCE g_hinstDLL;
void setWindowInstance ( HINSTANCE hInstance )
{
	g_hinstDLL = hInstance;
}
#endif

ZZ_SCRIPT
int setScreen ( int Width, int Height, int Depth, int bUseFullScreen )
{
	CHECK_INTERFACE(setScreen);

	state->screen_width = Width;
	state->screen_height = Height;
	state->screen_depth = Depth;

	state->buffer_width = Width;
	state->buffer_height = Height;
	state->buffer_depth = Depth;

	state->use_fullscreen = ISTRUE(bUseFullScreen);
	znzin->view->set_depth(Depth);
	znzin->view->set_fullscreen(ISTRUE(bUseFullScreen));
	znzin->view->set_width(Width);
	znzin->view->set_height(Height);
	return 1;
}

ZZ_SCRIPT
void setBuffer ( int Width, int Height, int Depth )
{
	CHECK_INTERFACE(setBuffer);

	state->buffer_width = Width;
	state->buffer_height = Height;
	state->buffer_depth = Depth;
}

ZZ_SCRIPT
int useHardwareVertexProcessing ( int bUse )
{
	CHECK_INTERFACE(useHardwareVertexProcessing);
	state->use_hw_vertex_processing_support = ISTRUE(bUse);
	return 0;
}
//
//ZZ_SCRIPT
//void useTexture ( int bUse )
//{
//	zz_render_state * state = znzin->renderer->get_state();
//	state->use_texture = ISTRUE(bUse);
//}
//
//ZZ_SCRIPT
//void useHAL ( int bUse )
//{
//	zz_render_state * state = znzin->renderer->get_state();
//	state->use_HAL = ISTRUE(bUse);
//}
//

ZZ_SCRIPT
int useVertexShader ( int bUse )
{
	CHECK_INTERFACE(useVertexShader);
	static bool old;
	old = state->use_vertex_shader;
	state->use_vertex_shader = ISTRUE(bUse);
	return (old) ? 1 : 0;
}

ZZ_SCRIPT
int usePixelShader ( int bUse )
{
	CHECK_INTERFACE(usePixelShader);
	static bool old;
	old = state->use_pixel_shader;
	state->use_pixel_shader = ISTRUE(bUse);
	return (old) ? 1 : 0;
}

//ZZ_SCRIPT
//void useBumpmap (int bUse)
//{
//	zz_render_state * state = znzin->renderer->get_state();
//	state->use_bumpmap = ISTRUE(bUse);
//}
//

ZZ_SCRIPT
void useShadowmap ( int bUse )
{
	CHECK_INTERFACE(useShadowmap);
	state->use_shadowmap = ISTRUE(bUse);
}

ZZ_SCRIPT
void setShadowmapColor ( float fColorR, float fColorG, float fColorB )
{
	CHECK_INTERFACE(setShadowmapColor);
	state->shadowmap_color = vec3(fColorR, fColorG, fColorB);
}

ZZ_SCRIPT
void useGlow ( int bUse )
{
	state->use_glow = ISTRUE(bUse);
}

ZZ_SCRIPT
void useFullSceneGlow ( int bUse )
{
	state->use_glow_fullscene = ISTRUE(bUse);
}

ZZ_SCRIPT
void setGlowmapSize ( int iSize )
{
	state->glowmap_size = iSize;
}


ZZ_SCRIPT
void setGlowColor ( float fColorR, float fColorG, float fColorB )
{
	state->glow_color = vec3(fColorR, fColorG, fColorB);
}

ZZ_SCRIPT
void setFullSceneGlowColor ( float fColorR, float fColorG, float fColorB )
{
	state->glow_color_fullscene = vec3(fColorR, fColorG, fColorB);
}

ZZ_SCRIPT
void setFullSceneGlowType ( int iType )
{
	state->fullscene_glow_type = iType;
}

ZZ_SCRIPT
int useWireMode ( int bUse )
{
	CHECK_INTERFACE(useWireMode);
	static bool old;
	old = state->use_wire_mode;
	state->use_wire_mode = ISTRUE(bUse);

	return (old) ? 1 : 0;
}

ZZ_SCRIPT
void useFog ( int bUse )
{
	CHECK_INTERFACE(useFog);

	state->use_fog = ISTRUE(bUse);
}

//ZZ_SCRIPT
//void useFlatShading (int bUse)
//{
//	zz_render_state * state = znzin->renderer->get_state();
//	state->use_flat_shading = ISTRUE(bUse);
//}

ZZ_SCRIPT
int useCull ( int bCull )
{
	CHECK_INTERFACE(useCull);
	static int old = state->use_cull;
	old = (state->use_cull) ? 1 : 0;
	state->use_cull = ISTRUE(bCull);
	return old;
}

ZZ_SCRIPT
int useDrawBoundingVolume ( int bDraw )
{
	CHECK_INTERFACE(useDrawBoundingVolume);
	static int old = state->use_draw_bounding_volume;
	old = (state->use_draw_bounding_volume) ? 1 : 0;
	state->use_draw_bounding_volume = ISTRUE(bDraw);

	return old;
}

ZZ_SCRIPT
void setClearColor ( float fClearColorX, float fClearColorY, float fClearColorZ )
{
	CHECK_INTERFACE(setClearColor);
	state->clear_color = vec3(fClearColorX, fClearColorY, fClearColorZ);
}

ZZ_SCRIPT
int setShadowmapSize ( int Size )
{
	CHECK_INTERFACE(setShadowmapSize);
	static int old;
	old = state->shadowmap_size;
	state->shadowmap_size = Size;
	return old;
}

ZZ_SCRIPT
int useDrawViewFrustum ( int bDraw )
{
	CHECK_INTERFACE(useDrawViewFrustum);
	static int old;
	old = state->use_draw_viewfrustum ? 1 : 0;
	state->use_draw_viewfrustum = ISTRUE(bDraw);
	return old;
}

ZZ_SCRIPT
int setDrawShadowmapViewport ( int bDraw )
{
	CHECK_INTERFACE(setDrawShadowmapViewport);
	static int old;
	old = state->draw_shadowmap_viewport ? 1 : 0;
	state->draw_shadowmap_viewport = ISTRUE(bDraw);
	return old;
}

ZZ_SCRIPT
int useDrawTextRect ( int bDraw )
{
	static int old;
	old = state->use_draw_text_rect ? 1 : 0;
	state->use_draw_text_rect = ISTRUE(bDraw);
	return old;
}

ZZ_SCRIPT
int useRefreshCull ( int bRefresh )
{
	CHECK_INTERFACE(useRefreshCull);
	static int old;
	old = state->use_refresh_cull ? 1 : 0;
	state->use_refresh_cull = ISTRUE(bRefresh);
	return old;
}

ZZ_SCRIPT
int useObjectSorting ( int bUse )
{
	CHECK_INTERFACE(useObjectSorting);
	static int old;
	old = state->use_object_sorting ? 1 : 0;
	state->use_object_sorting = ISTRUE(bUse);
	return old;
}

ZZ_SCRIPT
int usePolygonSorting ( int bUse )
{
	CHECK_INTERFACE(usePolygonSorting);
	static int old;
	old = state->use_polygon_sorting ? 1 : 0;
	state->use_polygon_sorting = ISTRUE(bUse);
	return old;
}

ZZ_SCRIPT
int setTimeDelay ( int Delay )
{
	CHECK_INTERFACE(setTimeDelay);
	static int old;
	old = state->time_delay;
	state->time_delay = Delay;
	return old;
}

ZZ_SCRIPT
int setTextureLoadingScale ( int iScale )
{
	CHECK_INTERFACE(setTextureLoadingScale);
	static int old;
	old = state->texture_loading_scale;
	if (iScale < 0) iScale = 0; // not te be negative or zero
	state->texture_loading_scale = iScale;
	return old;
}

ZZ_SCRIPT
int setMipmapFilter ( int FilterType ) 
{
	CHECK_INTERFACE(setMipmapFilter);
	int old = int(znzin->get_rs()->mipmap_filter);
	znzin->get_rs()->mipmap_filter = zz_render_state::zz_texture_filter_type(FilterType);
	return old;
}

ZZ_SCRIPT
int setMipmapLevel ( int Level )
{
	CHECK_INTERFACE(setMipmapLevel);
	int old = znzin->get_rs()->mipmap_level;
	znzin->get_rs()->mipmap_level = Level;
	return old;
}

ZZ_SCRIPT
int setMinFilter ( int FilterType )
{
	CHECK_INTERFACE(setMinFilter);
	int old = int(znzin->get_rs()->min_filter);
	znzin->get_rs()->min_filter = zz_render_state::zz_texture_filter_type(FilterType);
	return old;
}

ZZ_SCRIPT
int setMagFilter ( int FilterType )
{
	CHECK_INTERFACE(setMagFilter);
	int old = znzin->get_rs()->mag_filter;
	znzin->get_rs()->mag_filter = zz_render_state::zz_texture_filter_type(FilterType);
	return old;
}

ZZ_SCRIPT
int setFullSceneAntiAliasing ( int Type )
{
	CHECK_INTERFACE(setFullSceneAntiAliasing);
	int old = znzin->get_rs()->fsaa_type;
	znzin->get_rs()->fsaa_type = zz_render_state::zz_fsaa_type(Type);
	return old;
}

//==========================
// object loading & setting
//==========================

ZZ_SCRIPT
HNODE findNode ( ZSTRING pNodeName )
{
	CHECK_INTERFACE(findNode);
	if (pNodeName == 0) return 0;

	zz_node * node = znzin->find(pNodeName, zz_system::ZZ_SC_ALL);
	if (!node) {
		//ZZ_LOG("interface: findNode(%s) node not found\n", pNodeName);
		return 0;
	}
	return reinterpret_cast<HNODE>(node);
}

ZZ_SCRIPT
HNODE loadMesh ( ZSTRING pMeshName, ZSTRING pMeshPath )
{
	CHECK_INTERFACE(loadMesh);

	zz_mesh * mesh;
	
	zz_assertf(!znzin->meshes->find(pMeshName), "loadMesh(%s) failed. already exists", pMeshName);

	mesh = (zz_mesh *)znzin->meshes->spawn(pMeshName, ZZ_RUNTIME_TYPE(zz_mesh), false /* not to autoload */);
	zz_assert(mesh);
	
	mesh->set_path(pMeshPath);

	// set mesh min/max default
	const float DEFAULT_MAX = 10.0f*ZZ_SCALE_IN;

	mesh->set_min(vec3(0, 0, 0));
	mesh->set_max(vec3(DEFAULT_MAX, DEFAULT_MAX, DEFAULT_MAX));

	if (!zz_mesh_tool::load_mesh_minmax(mesh->get_path(), mesh)) {
		ZZ_LOG("interface: loadMesh(%s) failed. no min-max info\n", mesh->get_name());
	}

	znzin->meshes->load(reinterpret_cast<zz_node*>(mesh));

	//ZZ_LOG("interface: loadMesh(%s, %s)\n", pMeshName, pMeshPath);
	return reinterpret_cast<HNODE>(mesh);
}

ZZ_SCRIPT
HNODE loadColormapMaterial ( 
							ZSTRING pMaterialName,
							HNODE hShader,
							ZSTRING pMapFileName )
{
	CHECK_INTERFACE(loadColormapMaterial);

	zz_assert(pMapFileName);

	// zhotest
	if (strcmp(pMapFileName, "end") == 0) return 0;

	zz_assertf(znzin->file_system.exist(pMapFileName),
        "[%s] not found.\n", pMapFileName);

	zz_material_colormap * color_map = 
		(zz_material_colormap *)znzin->materials->find_or_spawn(pMaterialName, ZZ_RUNTIME_TYPE(zz_material_colormap));
	
	zz_shader * shader = reinterpret_cast<zz_shader *>(hShader);

	zz_assert(color_map);
	if (!color_map) {
		ZZ_LOG("interface: loadColormapMaterial() failed. no map\n");
		return 0;
	}

	if (shader) color_map->set_shader(shader);

	// set texture
	if (!color_map->set_texture(0, pMapFileName)) {
		// maybe, texture loading failed
		return 0;
	}

#ifdef ZZ_MATERIAL_LOADTEST
	color_map->loadtest_increase();
#endif

	//ZZ_LOG("interface: loadColormapMaterial(%s, %s, %s) done.\n", pMaterialName, shader->get_name(), pMapFileName);
	return reinterpret_cast<HNODE>(color_map);
}

ZZ_SCRIPT
HNODE loadNullColormapMaterial ( 
						ZSTRING pMaterialName,
						HNODE hShader,
						int iWidth, 
						int iHeight
						)
{
	CHECK_INTERFACE(loadNullColormapMaterial);
	
	zz_material_colormap * color_map = 
		(zz_material_colormap *)znzin->materials->find_or_spawn(pMaterialName, ZZ_RUNTIME_TYPE(zz_material_colormap));
	
	zz_shader * shader = reinterpret_cast<zz_shader *>(hShader);

	zz_assert(color_map);
	if (!color_map) {
		ZZ_LOG("interface: loadNullColormapMaterial() failed. no map");
		return 0;
	}
	if (shader) color_map->set_shader(shader);

	// set texture
	if (!color_map->set_texture(0, iWidth, iHeight)) {
		// maybe, texture loading failed
		return 0;
	}

#ifdef ZZ_MATERIAL_LOADTEST
	color_map->loadtest_increase();
#endif

	//ZZ_LOG("interface: loadNullColormapMaterial(%s, %dx%d, tex(%s)) done.\n",
	//	pMaterialName, iWidth, iHeight, color_map->get_texture(0)->get_name());
	return reinterpret_cast<HNODE>(color_map);
}

//
//ZZ_SCRIPT
//HNODE loadBumpmapMaterial (ZSTRING pMaterialName,
//                           HNODE hShader,
//                           ZSTRING pColormapFileName,
//						   ZSTRING pNormalmapFileName)
//{
//	if (znzin->materials->find(pMaterialName)) {
//		ZZ_LOG("interface: loadBumpmapMaterial(%s) already exists\n", pMaterialName);
//		return 0;
//	}
//	zz_material_bumpmap * bump_map = (zz_material_bumpmap *)znzin->materials->spawn(pMaterialName, ZZ_RUNTIME_TYPE(zz_material_bumpmap));
//	zz_shader * shader = reinterpret_cast<zz_shader *>(hShader);
//	zz_assert(bump_map && shader);
//	if (!bump_map || !shader) return 0;
//	bump_map->load(shader, pColormapFileName, pNormalmapFileName);
//	//ZZ_LOG("interface: loadBumpmapMaterial(%s, %s, %s, %s) done.\n",
//	//	pMaterialName, shader->get_name(), pColormapFileName, pNormalmapFileName);
//	return reinterpret_cast<HNODE>(shader);
//}
//
//ZZ_SCRIPT
//HNODE loadMultisubMaterial (ZSTRING pMultisubMaterialName,
//                            HNODE hSubmaterial)
//{
//	return 0;
//	//if (znzin->materials->find(pMultisubMaterialName)) {
//	//	ZZ_LOG("interface: loadMultisubMaterial(%s) already exists\n", pMultisubMaterialName);
//	//	return;
//	//}
//
//	//zz_material_multisub * multisub = (zz_material_multisub *)znzin->materials->find(pMultisubMaterialName);
//	//zz_material * sub = (zz_material *)znzin->materials->find(pSubmaterialName);
//
//	//zz_assert(multisub && sub);
//	//if (!multisub || !sub) return;
//
//	//if (!multisub) {
// //       multisub = (zz_material_multisub *)znzin->materials->spawn(pMultisubMaterialName, ZZ_RUNTIME_TYPE(zz_material_multisub));
//	//	zz_assert(multisub);
//	//	if (!multisub) return;
//	//}
//	//multisub->add_sub_material(sub);
//	//ZZ_LOG("interface: loadMultisubMaterial(%s, %s) done.\n",
//	//	pMultisubMaterialName, pSubmaterialName);
//}
//
//ZZ_SCRIPT
//HNODE loadMaterialMixer (ZSTRING pMaterialName,
//						 HNODE hMat1,
//						 HNODE hMat2,
//						 int OpCode)
//{
//	if (znzin->materials->find(pMaterialName)) {
//		ZZ_LOG("interface: loadMaterialMixer(%s) already exists\n", pMaterialName);
//		return 0;
//	}
//
//	zz_material_mixer * mixer;
//	zz_material_colormap * mat1, * mat2;
//
//	mixer = (zz_material_mixer *)znzin->materials->spawn(pMaterialName, ZZ_RUNTIME_TYPE(zz_material_mixer));
//	mat1 = reinterpret_cast<zz_material_colormap *>(hMat1);
//	mat2 = reinterpret_cast<zz_material_colormap *>(hMat2);
//	zz_assert(mixer && mat1 && mat2);
//	
//	if (!mixer || !mat1 || !mat2) return 0;
//	mixer->set_mixer(mat1, mat2, zz_material_mixer::zz_material_operation(OpCode));
//	//ZZ_LOG("interface: loadMaterialMixer(%s, %s, %s, %d) done.\n",
//	//	pMaterialName, mat1->get_name(), mat2->get_name(), OpCode);
//	return reinterpret_cast<HNODE>(mixer);
//}

//ZZ_SCRIPT
//HNODE loadMaterialTerrain (
//	ZSTRING pMaterialName,
//	HNODE hShader,
//	ZSTRING pDefaultMaterialName,
//	ZSTRING pSecondMaterialName)
//{
	//if (znzin->materials->find(pMaterialName)) {
	//	ZZ_LOG("interface: loadMaterialTerrain(%s) already exists\n", pMaterialName);
	//	return 0;
	//}

	//zz_material_terrain * tmat;
	//zz_material_colormap * mat1, * mat2;
	//zz_shader * shader;

	//tmat = (zz_material_terrain *)znzin->materials->spawn(
	//	pMaterialName, ZZ_RUNTIME_TYPE(zz_material_terrain));
	//shader = (zz_shader *)znzin->shaders->find(pShaderName);
	//mat1 = (zz_material_colormap *)znzin->materials->find(pDefaultMaterialName);
	//mat2 = (zz_material_colormap *)znzin->materials->find(pSecondMaterialName);
	//zz_assert(tmat && shader && mat1);
	//if (!tmat || !shader || !mat1) return 0;
	//tmat->load(shader, mat1, mat2);
	//ZZ_LOG("interface: loadMaterialTerrain(%s, %s, %s, %s) done.\n",
	//	pMaterialName, pShaderName, pDefaultMaterialName, pSecondMaterialName);
	//return 1;
//}

ZZ_SCRIPT
int setMaterialUseAlpha ( HNODE hMaterial, int bUseAlpha )
{
	CHECK_INTERFACE_MATERIAL(setMaterialUseAlpha);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	if (!mat) return 0;
	mat->set_texturealpha(ISTRUE(bUseAlpha));

	//ZZ_LOG("interface: setMaterialUseAlpha(%s, %d)\n", mat->get_name(), bUseAlpha);
	return 1;
}

ZZ_SCRIPT
int setMaterialUseAlphaTest ( HNODE hMaterial, int bUseAlphaTest )
{
	CHECK_INTERFACE_MATERIAL(setMaterialUseAlphaTest);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	if (!mat) return 0;
	mat->set_alpha_test(ISTRUE(bUseAlphaTest));
	return 1;
}

ZZ_SCRIPT
int setMaterialAlphaRef ( HNODE hMaterial, int iAlphaRef )
{
	CHECK_INTERFACE_MATERIAL(setMaterialAlphaRef);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	if (!mat) return 0;
	mat->set_alpha_ref(iAlphaRef);
	return 1;
}

ZZ_SCRIPT
int setMaterialUseTwoSide ( HNODE hMaterial, int bUseTwoSide )
{
	CHECK_INTERFACE_MATERIAL(setMaterialUseTwoSide);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	if (!mat) return 0;
	mat->set_twoside(ISTRUE(bUseTwoSide));

	//ZZ_LOG("setMaterialUseTwoSide(%s, %d)\n", mat->get_name(), bUseTwoSide);

	return 1;
}


ZZ_SCRIPT
int setMaterialGlow (
	  HNODE hMaterial,
	  int iGlowType,
	  float fRed, float fGreen, float fBlue
)
{
	zz_material * mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	if (!mat) return 0;
	
	assert(fRed <= 1.0f);
	assert(fGreen <= 1.0f);
	assert(fBlue <= 1.0f);
	assert(fRed >= 0);
	assert(fGreen >= 0);
	assert(fBlue >= 0);

	mat->set_glow_type( zz_glow_type(iGlowType) );
	mat->set_glow_color( vec3(fRed, fGreen, fBlue) );

	return 1;
}

ZZ_SCRIPT
int setVisibleGlow (
	  HNODE hVisible,
	  int iGlowType,
	  float fRed, float fGreen, float fBlue
)
{
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;
	
	assert(fRed <= 1.0f);
	assert(fGreen <= 1.0f);
	assert(fBlue <= 1.0f);
	assert(fRed >= 0);
	assert(fGreen >= 0);
	assert(fBlue >= 0);

	vis->set_glow( zz_glow_type(iGlowType), vec3(fRed, fGreen, fBlue) );

	return 1;
}

ZZ_SCRIPT
int setVisibleGlowRecursive (
	  HNODE hVisible,
	  int iGlowType,
	  float fRed, float fGreen, float fBlue
)
{
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;
	
	assert(fRed <= 1.0f);
	assert(fGreen <= 1.0f);
	assert(fBlue <= 1.0f);
	assert(fRed >= 0);
	assert(fGreen >= 0);
	assert(fBlue >= 0);

	vis->set_glow_recursive( zz_glow_type(iGlowType), vec3(fRed, fGreen, fBlue) );

	return 1;
}

ZZ_SCRIPT
int setVisibleRenderUnitGlow ( HNODE hVisible, int iRenderUnit, int iGlowType, float fRed, float fGreen, float fBlue )
{
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;
	
	assert(fRed <= 1.0f);
	assert(fGreen <= 1.0f);
	assert(fBlue <= 1.0f);
	assert(fRed >= 0);
	assert(fGreen >= 0);
	assert(fBlue >= 0);
	assert(iRenderUnit < (int)vis->get_num_runits());

	vis->set_glow_runit( iRenderUnit, zz_glow_type(iGlowType), vec3(fRed, fGreen, fBlue) );

	return 1;
}

ZZ_SCRIPT
HNODE loadVisible ( ZSTRING pVisibleName,
				  HNODE hMesh,
				  HNODE hMaterial,
				  HNODE hLight )
{
	CHECK_INTERFACE(loadVisible);
	zz_visible * vis = (zz_visible *)(znzin->visibles->find(pVisibleName));
	
	if (vis) {
		ZZ_LOG("interface: loadVisible(%s) already exists\n", pVisibleName);
		return 0;
	}
	else {
		vis = (zz_visible *)znzin->visibles->spawn(pVisibleName, ZZ_RUNTIME_TYPE(zz_visible));
	}

	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_material * mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(vis); // light and mesh can be NULL
	if (!vis) return 0;

	vis->set_bvolume_type(ZZ_BV_OBB);

	if (mesh) {
		vis->add_runit(mesh, mat, light);
	}
	vis->set_collision_level(ZZ_CL_NONE);
	//ZZ_LOG("interface: loadVisible(%s, %s, %s, %s) done.\n",
	//	pVisibleName,
	//	mesh ? mesh->get_name() : NULL,
	//	mat ? mat->get_name() : NULL,
	//	light ? light->get_name() : NULL);
	return reinterpret_cast<HNODE>(vis);
}

ZZ_DLL
HNODE loadVisibleEx ( ZSTRING pVisibleName,
				  HNODE hMesh,
				  HNODE hMaterial,
				  HNODE hLight )
{
	CHECK_INTERFACE(loadVisible);
	zz_visible * vis = (zz_visible *)(znzin->visibles->find(pVisibleName));
	
	if (vis) {
		return reinterpret_cast<HNODE>(vis);;
	}
	else {
		vis = (zz_visible *)znzin->visibles->spawn(pVisibleName, ZZ_RUNTIME_TYPE(zz_visible));
	}

	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_material * mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(vis); // light and mesh can be NULL
	if (!vis) return 0;

	vis->set_bvolume_type(ZZ_BV_OBB);

	if (mesh) {
		vis->add_runit(mesh, mat, light);
	}
	vis->set_collision_level(ZZ_CL_NONE);
	//ZZ_LOG("interface: loadVisible(%s, %s, %s, %s) done.\n",
	//	pVisibleName,
	//	mesh ? mesh->get_name() : NULL,
	//	mat ? mat->get_name() : NULL,
	//	light ? light->get_name() : NULL);
	return reinterpret_cast<HNODE>(vis);
}



ZZ_SCRIPT
int setObbox ( HNODE hVisible,
			   float LocalCenterX, float LocalCenterY, float LocalCenterZ,
			   float HalfLengthX, float HalfLengthY, float HalfLengthZ )
{
	CHECK_INTERFACE(setObbox);
	zz_visible * visible = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(visible);
	if (!visible) return 0;

	visible->set_bvolume_type(ZZ_BV_OBB);
	zz_bvolume * bv = visible->get_bvolume();
	LocalCenterX *= ZZ_SCALE_IN;
	LocalCenterY *= ZZ_SCALE_IN;
	LocalCenterZ *= ZZ_SCALE_IN;
	bv->set_local_center(vec3(LocalCenterX, LocalCenterY, LocalCenterZ));
	HalfLengthX *= ZZ_SCALE_IN;
	HalfLengthY *= ZZ_SCALE_IN;
	HalfLengthZ *= ZZ_SCALE_IN;
	bv->set_obb(vec3(HalfLengthX, HalfLengthY, HalfLengthZ));
	visible->invalidate_transform();
	return 1;
}

ZZ_SCRIPT
HNODE loadMorpher ( ZSTRING pMorpherName,
				  HNODE hMesh,
				  HNODE hMotion,
				  HNODE hMaterial,
				  HNODE hLight )
{
	CHECK_INTERFACE(loadMorpher);
	zz_assert(!znzin->visibles->find(pMorpherName));

	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_motion * motion = reinterpret_cast<zz_motion *>(hMotion);
	zz_material * material = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(mesh && material);
	if (!mesh || !material) {
		ZZ_LOG("interface: loadMorpher(%s) failed. not found one or more resource file(s)\n",
			pMorpherName);
		return 0;
	}

	zz_morpher * morpher = (zz_morpher *)znzin->visibles->spawn(pMorpherName, ZZ_RUNTIME_TYPE(zz_morpher));
	
	morpher->add_runit(mesh, material, light);
	
	if (motion) {
		morpher->attach_motion(motion);
	}

	//ZZ_LOG("interface: loadMorpher(%s, %s, %s, %s, %s) done.\n",
	//	morpher->get_name(), mesh->get_name(), 
	//	(motion) ? motion->get_name() : "null", material->get_name(), light ? light->get_name() : "none");

	return reinterpret_cast<HNODE>(morpher);
}

ZZ_SCRIPT
HNODE loadSkeleton (
	ZSTRING pSkeletonName,
	ZSTRING pSkeletonPath )
{
	CHECK_INTERFACE(loadSkeleton);
	static zz_skeleton * skel;
	skel = static_cast<zz_skeleton*>(znzin->find(pSkeletonName, zz_system::ZZ_SC_SKELETON));
	if (skel) {
		ZZ_LOG("interface: loadSkeleton(%s) failed. already exists. return current.\n", pSkeletonName);
		return 0;
	}
	skel = static_cast<zz_skeleton *>
		(znzin->skeletons->spawn(pSkeletonName, ZZ_RUNTIME_TYPE(zz_skeleton)));
	zz_assert(skel);
	if (!skel) {
		ZZ_LOG("interface: loadSkeleton(%s) failed. cannot create skeleton\n",
			pSkeletonName);
		return 0;
	}
	bool ret = skel->load_skeleton(pSkeletonPath);
	if (!ret) {
		ZZ_LOG("interface: loadSkeleton(%s:[%s]) failed.\n", pSkeletonName, pSkeletonPath);
		return 0;
	}
	//ZZ_LOG("interface: loadSkeleton(%s, %s) done.\n",
	//	pSkeletonName, pSkeletonPath);
	return reinterpret_cast<HNODE>(skel);
}


ZZ_SCRIPT
HNODE loadModel ( ZSTRING pModelName,
				HNODE hSkeleton,
				HNODE hMotion,
				float ScaleInLoad )
{
	CHECK_INTERFACE(loadModel);

	zz_assertf(!znzin->visibles->find(pModelName), "loadModel(%s) failed. already exists", pModelName);
	
	zz_model * model;
	zz_motion * motion;
	zz_skeleton * skel;
	model = static_cast<zz_model *>(znzin->visibles->spawn(pModelName, ZZ_RUNTIME_TYPE(zz_model)));
	motion = reinterpret_cast<zz_motion *>(hMotion);
	skel = reinterpret_cast<zz_skeleton *>(hSkeleton);
	
	zz_assert(model);
	if (!model) {
		ZZ_LOG("interface: loadModel(%s) failed. no model\n",
			pModelName);
		return 0;
	}
	if (!skel) {
		ZZ_LOG("interface: loadModel(%s) failed. no skeleton\n",
			pModelName);
		return 0;
	}
	model->attach_skeleton(skel); // ignore ScaleInLoad);

	//ZZ_LOG("interface: attach_skeleton() done\n");
	if (motion) {
		model->attach_motion(motion);
		model->play();
	}
	//ZZ_LOG("interface: loadModel(%s, %s, %s, %f) done.\n",
	//	pModelName, 
	//	(skel) ? skel->get_name() : "[null]",
	//	(motion) ? motion->get_name() : "[null]", ScaleInLoad);
	return reinterpret_cast<HNODE>(model);
}


ZZ_SCRIPT
int addRenderUnit ( HNODE hVisible,
					HNODE hMesh,
					HNODE hMaterial,
					HNODE hLight )
{
	CHECK_INTERFACE(addRenderUnit);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_material * material = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(vis);
	zz_assert(mesh);
	if (!vis) return 0;
	if (!mesh) return 0;

	vis->set_bvolume_type(ZZ_BV_OBB);
	vis->add_runit(mesh, material, light);
	//ZZ_LOG("interface: addrunit(%s, %s, %s, %s) done.\n",
	//	vis->get_name(), mesh->get_name(), material->get_name(), light->get_name());
	return 1;
}

ZZ_SCRIPT
int clearRenderUnit ( HNODE hVisible )
{
	CHECK_INTERFACE(clearRenderUnit);
	zz_visible * vis = reinterpret_cast<zz_model *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;

	vis->clear_runit();
	vis->set_bvolume_type(ZZ_BV_NONE);

	//ZZ_LOG("interface: clearrunit(%s) done.\n", vis->get_name());
	return 1;
}

// sync with linkVisibleWorld
ZZ_SCRIPT
int linkNode ( HNODE hParent, HNODE hNode )
{
	CHECK_INTERFACE(linkNode);
	zz_node * child = reinterpret_cast<zz_node *>(hNode);
	zz_node * parent = reinterpret_cast<zz_node *>(hParent);

	zz_assert(child && "interface: linkNode():");
	zz_assert(parent && "interface: linkNode():");
	zz_assert((child != parent) && "interface: linkNode(): cannot link to same node");
	if (!child) {
		ZZ_LOG("interface: linkNode(%d, %d) failed. invalid handle\n", hParent, hNode);
		return 0;
	}

	if (hParent == hNode) {
		ZZ_LOG("interface: linkNode(%d, %d) failed. cannot link between same nodes\n", hParent, hNode);
		return 0;
	}

	if (child->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		zz_visible * vis = static_cast<zz_visible*>(child);
	}

	if (parent) {
		parent->link_child(child);;
	}
	else {
		zz_node * root = child->get_root();
		root->link_child(child); // make top-level
	}

	return 1;
}

// sync with unlinkNodeWorld
ZZ_SCRIPT
int unlinkNode ( HNODE hNode )
{
	CHECK_INTERFACE(unlinkNode);
	zz_node * child = reinterpret_cast<zz_node *>(hNode);
	
	if (!child) return 0; // invalid hnode

	zz_node * parent = child->get_parent();
	
	if (!parent) return 0; // already unlinked

	if (!child || !parent) return 0;
	//parent->traverse_node(); // for debugging
	parent->unlink_child(child);
	parent = child->get_root();
	zz_assert(parent);
	parent->link_child(child); // relink to manager

	return 1;
}

ZZ_SCRIPT
int linkBone ( HNODE hParentModel, HNODE hNode, int iBone )
{
	CHECK_INTERFACE(linkBone);
	zz_visible * child = reinterpret_cast<zz_visible *>(hNode);
	zz_model * parent = reinterpret_cast<zz_model *>(hParentModel);

	zz_assert(parent);
	zz_assert(child && parent);

	parent->link_bone(child, iBone);

	//ZZ_LOG("interface: linkBone(%s->%s) done.\n",
	//	bone->get_name(), child->get_name());
	return 1;
}

ZZ_SCRIPT
int linkDummy ( HNODE hParentModel, HNODE hNode, int iDummy )
{
	CHECK_INTERFACE(linkDummy);
	zz_visible * child = reinterpret_cast<zz_visible *>(hNode);
	zz_model * parent = reinterpret_cast<zz_model *>(hParentModel);

	zz_assert(child && parent);
	if (!child || !parent) return 0;
	zz_assert(parent->is_a(ZZ_RUNTIME_TYPE(zz_model)));
	zz_assert(child->is_a(ZZ_RUNTIME_TYPE(zz_visible)));

	parent->link_dummy(child, iDummy);

	return 1;
}

ZZ_DLL
void InputSceneModelDummyAxis(HNODE hNode, int Index, float Size)
{
	zz_model * model = reinterpret_cast<zz_model *>(hNode);
   		
	zz_dummy * dummy = model->get_dummy(Index);
	znzin->scene.input_scene_axis((zz_visible*)dummy, Size * ZZ_SCALE_IN);
}


ZZ_SCRIPT
HNODE loadLight ( ZSTRING pLightName )
{
	CHECK_INTERFACE(loadLight);
	
	zz_assert(!znzin->lights->find(pLightName));
	
	zz_light_direct * light = (zz_light_direct *)znzin->lights->spawn(pLightName, ZZ_RUNTIME_TYPE(zz_light_direct));
	
	zz_assert(light);
	if (!znzin->get_light()) {
		znzin->set_light(light);
	}

	//ZZ_LOG("interface: loadLight(%s) done.\n", pLightName);
	return reinterpret_cast<HNODE>(light);
}

ZZ_SCRIPT
void setDefaultLight ( HNODE hLight )
{
	zz_light * light = reinterpret_cast<zz_light *>(hLight);

	zz_assert(light);

	znzin->set_light(light);
}

ZZ_SCRIPT
HNODE getDefaultLight ( HNODE hLight )
{
	return reinterpret_cast<HNODE>(znzin->get_light());
}

ZZ_SCRIPT
HNODE loadLightPoint ( ZSTRING pLightName )
{
	CHECK_INTERFACE(loadLightPoint);
	if (znzin->lights->find(pLightName)) {
		return 0;
	}
	zz_light_point * light = (zz_light_point *)znzin->lights->spawn(pLightName, ZZ_RUNTIME_TYPE(zz_light_point));
	zz_assert(light);
	return reinterpret_cast<HNODE>(light);
}

ZZ_SCRIPT
int setLight ( HNODE hLight, ZSTRING pProperty, float fValue1, float fValue2, float fValue3 )
{
	CHECK_INTERFACE(setLight);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);

	zz_assert(light);
	if (!light) return 0;

	if (strcmp("ambient", pProperty) == 0) {
		fValue1 = (fValue1 < 0) ? 0 : ((fValue1 > 1.0f) ? 1.0f : fValue1);
		fValue2 = (fValue2 < 0) ? 0 : ((fValue2 > 1.0f) ? 1.0f : fValue2);
		fValue3 = (fValue3 < 0) ? 0 : ((fValue3 > 1.0f) ? 1.0f : fValue3);

		light->ambient = vec3(fValue1, fValue2, fValue3);
		light->ambient.w = 1.0f;
	}
	else if (strcmp("diffuse", pProperty) == 0) {
		fValue1 = (fValue1 < 0) ? 0 : ((fValue1 > 1.0f) ? 1.0f : fValue1);
		fValue2 = (fValue2 < 0) ? 0 : ((fValue2 > 1.0f) ? 1.0f : fValue2);
		fValue3 = (fValue3 < 0) ? 0 : ((fValue3 > 1.0f) ? 1.0f : fValue3);

		light->diffuse = vec3(fValue1, fValue2, fValue3);
		light->diffuse.w = 1.0f;
	}
	else if (strcmp("specular", pProperty) == 0) {
		fValue1 = (fValue1 < 0) ? 0 : ((fValue1 > 1.0f) ? 1.0f : fValue1);
		fValue2 = (fValue2 < 0) ? 0 : ((fValue2 > 1.0f) ? 1.0f : fValue2);
		fValue3 = (fValue3 < 0) ? 0 : ((fValue3 > 1.0f) ? 1.0f : fValue3);

		light->specular = vec3(fValue1, fValue2, fValue3);
		light->specular.w = 1.0f;
	}
	else if (strcmp("direction", pProperty) == 0) {
		if (!light->is_a(ZZ_RUNTIME_TYPE(zz_light_direct))) {
			ZZ_LOG("interface: setLight(%s:direction) failed. It is not a directional light.\n", light->get_name());
			return 0;
		}

		zz_light_direct * directional_light = static_cast<zz_light_direct*>(light);

		vec3 direction(fValue1, fValue2, fValue3);
		direction.normalize();
		directional_light->direction = direction;
		directional_light->direction.w = 1.0f;
		directional_light->diffuse.w = 1.0f;
	}
	else if (strcmp("position", pProperty) == 0) {
		fValue1 *= ZZ_SCALE_IN;
		fValue2 *= ZZ_SCALE_IN;
		fValue3 *= ZZ_SCALE_IN;
		light->position = vec3(fValue1, fValue2, fValue3);
		light->position.w = 1.0f;
	}
	else if (strcmp("attenuation", pProperty) == 0) {
		if (!light->is_a(ZZ_RUNTIME_TYPE(zz_light_point))) {
			ZZ_LOG("interface: setLight(%s:attenuation) failed. It is not a point light.\n", light->get_name());
			return 0;
		}
		zz_light_point * point_light = static_cast<zz_light_point*>(light);
		fValue1 *= ZZ_SCALE_IN;
		fValue2 *= ZZ_SCALE_IN;
		fValue3 *= ZZ_SCALE_IN;
		float& start = fValue1;
		float& end = fValue2;
		float& scale = fValue3;
		point_light->attenuation.set(1.0f - scale*start/(end - start), scale/(end - start), 0, 0);
	}

	return 1;
}

ZZ_SCRIPT
int setQuaternion ( HNODE hVisible, float qW, float qX, float qY, float qZ )
{
	CHECK_INTERFACE(setQuaternion);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;
	quat q(qX, qY, qZ, qW);
	vis->set_rotation(q);
	vis->invalidate_transform();
	return 1;
}

ZZ_SCRIPT
int setAxisAngle (
	HNODE hVisible,
	float AngleDegree,
	float AxisX,
	float AxisY,
	float AxisZ)
{
	CHECK_INTERFACE(setAxisAngle);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	if (!vis) return 0;
	quat q(vec3(AxisX, AxisY, AxisZ), AngleDegree*ZZ_TO_RAD);
	vis->set_rotation(q);
	vis->invalidate_transform();
	//ZZ_LOG("setAxisAngle() -> quaternion(%f, %f, %f, %f)(w,x,y,z)\n", q.w, q.x, q.y, q.z);
	return 1;
}

ZZ_DLL
int setPositionVec3 ( HNODE hVisible, float vPosition[3] )
{
	CHECK_INTERFACE(setPositionVec3);

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	
	if (!vis) return 0;
	
	vec3 pos(vPosition);
	pos *= ZZ_SCALE_IN;
	const vec3& last_pos = vis->get_position();
	if (last_pos == pos) return 1; // already same

	vis->set_position(pos);
	vis->invalidate_transform();

	return 1;
}

ZZ_SCRIPT
int setPosition (
	HNODE hVisible,
	float PositionX,
	float PositionY,
	float PositionZ )
{
	CHECK_INTERFACE(setPosition);
	float position[3];
	position[0] = PositionX; position[1] = PositionY; position[2] = PositionZ;
	return setPositionVec3( hVisible, position );
}

ZZ_SCRIPT 
int setScale (
	HNODE hVisible,
	float ScaleX,
	float ScaleY,
	float ScaleZ )
{
	CHECK_INTERFACE(setScale);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
//	zz_assert(vis);                           //임시로 제거 2006 0623 버그 잡을때 까지...
	if (!vis) return 0;
	vis->set_scale(vec3(ScaleX, ScaleY, ScaleZ));
	vis->invalidate_transform();
	return 1;
}

ZZ_DLL
int setRotationQuat ( HNODE hVisible, float vRotation[4] )
{
	CHECK_INTERFACE(setRotationQuat);

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	
	if (!vis) return 0;
	
	quat rot;

	rot.w = vRotation[0];
	rot.x = vRotation[1];
	rot.y = vRotation[2];
	rot.z = vRotation[3];
	
	vis->set_rotation_local(rot);
	vis->invalidate_transform();

	return 1;
}


ZZ_DLL
int setRotationQuat2 ( HNODE hVisible, float vRotation[4] )
{
	CHECK_INTERFACE(setRotationQuat);

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	
	if (!vis) return 0;
	
	quat rot;

	rot.x = vRotation[0];
	rot.y = vRotation[1];
	rot.z = vRotation[2];
	rot.w = vRotation[3];

	vis->set_rotation_local(rot);
	vis->invalidate_transform();

	return 1;
}



ZZ_SCRIPT
int setShaderFormat (
				   HNODE hShader,
				   ZSTRING pVertexShaderPath,
                   ZSTRING pPixelShaderPath,
                   int iFormat
				   )
{
	CHECK_INTERFACE(setShader);

	zz_shader * shader = reinterpret_cast<zz_shader*>(hShader);

	if (!shader) return 0;

	int vshader(ZZ_HANDLE_NULL), pshader(ZZ_HANDLE_NULL);

	vshader = shader->create_vshader(pVertexShaderPath, iFormat);
	
	if (znzin->get_rs()->use_pixel_shader) {
		pshader = shader->create_pshader(pPixelShaderPath, iFormat);
	}

	return 1;
}


ZZ_SCRIPT
HNODE loadShader (
				  ZSTRING pShaderName,
                  ZSTRING pVertexShaderPath,
                  ZSTRING pPixelShaderPath,
                  int bUseBinary,
                  int iVertexFormat
				  )
{
	CHECK_INTERFACE(loadShader);
	if (znzin->shaders->find(pShaderName)) {
		//ZZ_LOG("interface: loadShader(%s) already exists\n", pShaderName);
		return 0;
	}

	//ZZ_LOG("interface: loadShader(%s, %s, %s, %d, %d) done.\n",
	//	pShaderName, pVertexShaderPath, pPixelShaderPath, bUseBinary, iVertexFormat);

	zz_shader * shader = (zz_shader *)znzin->shaders->spawn(pShaderName, ZZ_RUNTIME_TYPE(zz_shader));
	zz_assert(shader);

	if (!shader) return 0;

	shader->set_is_binary(ISTRUE(bUseBinary));
	shader->set_vertex_format(iVertexFormat);

	HNODE hShader = reinterpret_cast<HNODE>(shader);
	setShaderFormat(hShader, pVertexShaderPath, pPixelShaderPath, SHADER_FORMAT_DEFAULT);

	// set default shader
	if (strcmp(pShaderName, "shader_shadowmap_skin") == 0) {
		zz_shader::shadow_shader_skin = shader;
	}
	else if (strcmp(pShaderName, "shader_shadowmap") == 0) {
		zz_shader::shadow_shader = shader;
	}
	else if (strcmp(pShaderName, "shader_glow_skin") == 0) {
		zz_shader::glow_shader_skin = shader;
	}
	else if (strcmp(pShaderName, "shader_glow") == 0) {
		zz_shader::glow_shader = shader;
	}
	else if (strcmp(pShaderName, "shader_terrain") == 0) {
		zz_shader::terrain_shader = shader;
	}

	return hShader;
}

ZZ_SCRIPT
HNODE loadCamera ( ZSTRING pCameraName,
				 ZSTRING pCameraPath,
				 HNODE hCameraMotion )
{
	CHECK_INTERFACE(loadCamera);
	if (znzin->cameras->find(pCameraName)) {
		ZZ_LOG("interface: loadCamera(%s) already exists\n", pCameraName);
		return 0;
	}

	//*******************************************
	// CAUTION: WE CREATE FOLLOW CAMERA CURRENTLY
	//*******************************************
	zz_camera * camera;
	if (hCameraMotion) {
		camera = (zz_camera *)znzin->cameras->find_or_spawn(pCameraName, ZZ_RUNTIME_TYPE(zz_camera));
	}
	else {
		camera = (zz_camera *)znzin->cameras->find_or_spawn(pCameraName, ZZ_RUNTIME_TYPE(zz_camera_follow));
	}
	zz_assert(camera);

	if (!camera) return 0;
	camera->load(pCameraPath);
	
	// ignore source camera aspect ratio, and use screen rect's
	camera->set_aspect_ratio((float)znzin->view->get_width() / (float)znzin->view->get_height());

	zz_motion * motion = reinterpret_cast<zz_motion *>(hCameraMotion);
	if (motion) {
		camera->attach_motion(motion);
	}

	//ZZ_LOG("interface: loadCamera(%s, %s, %s) done.\n",
	//	pCameraName, pCameraPath, (motion) ? motion->get_name() : "");
	return reinterpret_cast<HNODE>(camera);
}


ZZ_SCRIPT
int saveCamera ( HNODE hCamera, ZSTRING pCameraPath )
{
	CHECK_INTERFACE(saveCamera);
	zz_camera * camera = reinterpret_cast<zz_camera *>(hCamera);

	if (!camera) {
		ZZ_LOG("interface: saveCamera() failed\n");
		return 0;
	}
	if (!pCameraPath) {
		ZZ_LOG("interface: saveCamera(%s) failed\n", pCameraPath);
		return 0;
	}
	return camera->save(pCameraPath) ? 1 : 0;
}

ZZ_SCRIPT
int setCameraAspectRatio ( HNODE hCamera, float AspectRatio )
{
	CHECK_INTERFACE(setCameraAspectRatio);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);

	zz_assert(cam);

	if (cam) {
		if (AspectRatio == 0.0f) {
			AspectRatio = (float)znzin->view->get_width()/(float)znzin->view->get_height();
		}
		cam->set_aspect_ratio(AspectRatio);
	}
	else {
		return 0;
	}
	return 1;
}

ZZ_SCRIPT
HNODE setCameraDefault ( HNODE hCamera )
{
	zz_camera * old = znzin->get_camera();
	znzin->set_camera(reinterpret_cast<zz_camera*>(hCamera));
	return reinterpret_cast<HNODE>(old);
}

ZZ_SCRIPT
HNODE getCameraDefault ( void )
{
	return reinterpret_cast<HNODE>(znzin->get_camera());
}


ZZ_SCRIPT
HNODE getCameraLight ( void )
{
	return reinterpret_cast<HNODE>(znzin->get_camera_light());
}

ZZ_SCRIPT
HNODE setCameraLight ( HNODE hCamera )
{
	zz_camera * old = znzin->get_camera_light();
	znzin->set_camera_light(reinterpret_cast<zz_camera*>(hCamera));
	return reinterpret_cast<HNODE>(old);
}


ZZ_SCRIPT
HNODE loadMotion ( ZSTRING pMotionName,
				 ZSTRING pMotionFileName,
				 int bUseLoop,
				 int InterpTypePosition,
				 int InterpTypeRotation,
				 float Scale,
				 int bForModel )
{
	CHECK_INTERFACE(loadMotion);
	zz_assert(pMotionFileName);

	//ZZ_LOG("interface: loadMotion(%s, %s)\n", pMotionName, pMotionFileName);

	zz_assertf(!znzin->motions->find(pMotionName), "interface: loadMotion(%s) already exists\n", pMotionName);

	zz_motion * motion = (zz_motion *)znzin->motions->spawn(pMotionName, ZZ_RUNTIME_TYPE(zz_motion));
	
	zz_assert(motion);

	znzin->motion_tool.load_motion(motion, pMotionFileName, ISTRUE(bUseLoop), InterpTypePosition, InterpTypeRotation, Scale);
	//ZZ_LOG("interface: loadMotion(%s, %s, %d, %d, %d, %f, %d) done.\n",
	//	pMotionName, pMotionFileName, bUseLoop, InterpTypePosition, InterpTypeRotation,
	//	Scale, bForModel);
	return reinterpret_cast<HNODE>(motion);
}

ZZ_SCRIPT
HNODE loadMotionMixer (ZSTRING pMixMotionName,
					  HNODE hMotionA,
					  HNODE hMotionB)
{
	CHECK_INTERFACE(loadMotionMixer);
	if (znzin->motions->find(pMixMotionName)) {
		ZZ_LOG("interface: loadMixMotionName(%s) already exists\n", pMixMotionName);
		return 0;
	}
	
	zz_motion * motion1, * motion2;
	motion1 = reinterpret_cast<zz_motion *>(hMotionA);
	motion2 = reinterpret_cast<zz_motion *>(hMotionB);
	zz_assert(motion1 && motion2);
	if (!motion1 || !motion2) return 0;
	zz_motion_mixer * mix_motion = znzin->motion_tool.create_blend_motion(pMixMotionName, motion1, motion2, 1.0f);
	zz_assert(mix_motion);
	//ZZ_LOG("interface: loadMotionMixer(%s, %s, %s) done.\n",
	//	pMixMotionName, motion1->get_name(), motion2->get_name());
	return reinterpret_cast<HNODE>(mix_motion);
}

ZZ_SCRIPT
int attachMotion (HNODE hNode, HNODE hMotion)
{
	CHECK_INTERFACE(attachMotion);	zz_animatable * ani = reinterpret_cast<zz_animatable *>(hNode);
	zz_assert(ani);
	if (!ani) return 0;
	zz_motion * motion = reinterpret_cast<zz_motion *>(hMotion);
	
	ani->attach_motion(motion);

	//ZZ_LOG("interface: attach(%s, %s) done.\n", ani->get_name(), motion ? motion->get_name() : "none");
	return 1;
}


ZZ_SCRIPT
int doScript (ZSTRING pScriptName)
{
	CHECK_INTERFACE(doScript);
	if (!znzin->file_system.exist(pScriptName)) {
		ZZ_LOG("interface: doScript(%s) failed. file not found\n", pScriptName);
		return 0;
	}

	return znzin->script->do_script(pScriptName) ? 1 : 0;
}

//void doMainLoop ()
//{
//	znzin->view->main_loop();
//}

ZZ_SCRIPT
void doLog (ZSTRING pLogMessage)
{
	CHECK_INTERFACE(doLog);
	ZZ_LOG(pLogMessage);
}

ZZ_DLL
void doLogf ( const char * pMsgFormat, ... )
{
	static va_list _va;
	static char formatted_string[ZZ_MAX_STRING];
	
	va_start(_va, pMsgFormat);
	vsprintf(formatted_string, pMsgFormat, _va);
	va_end(_va);

	ZZ_LOG(formatted_string);
}

ZZ_SCRIPT
int turnModel (HNODE hModel, float Angle)
{
	CHECK_INTERFACE(turnModel);
	zz_model * model = reinterpret_cast<zz_model *>(hModel);
	zz_assert(model);
	if (!model) {
		return 0;
	}
	//ZZ_LOG("interface: turnModel(%f)\n", Angle);
    model->turn(Angle);
	return 1;
}

//void destroyWindow ()
//{
//	znzin->view->destroy_window();
//}

ZZ_SCRIPT
int dollyCamera (HNODE hCamera, float StepSize)
{
	CHECK_INTERFACE(dollyCamera);
	zz_camera * camera = reinterpret_cast<zz_camera	 *>(hCamera);
	zz_assert(camera);
	if (!camera) {
		return 0;
	}
	camera->dolly(StepSize);
	return 1;
}

ZZ_SCRIPT
float getFps (void)
{
	CHECK_INTERFACE(getFps);
	return znzin->get_fps();
}

ZZ_SCRIPT
void setWindowText (ZSTRING pTitle)
{
	CHECK_INTERFACE(setWindowText);
	znzin->view->set_window_text(pTitle);
}

ZZ_SCRIPT
int traverseNode (HNODE hNode)
{
	CHECK_INTERFACE(traverseNode);
	zz_node * node = reinterpret_cast<zz_node *>(hNode);
	if (!node) {
		return 0;
	}
	node->traverse_node();
	return 1;
}

ZZ_SCRIPT
int rotateCamera (HNODE hCamera, int AxisType, float AngleDegree)
{
	CHECK_INTERFACE(rotateCamera);
	zz_camera * camera = reinterpret_cast<zz_camera *>(hCamera);

	zz_assert(camera);
	if (!camera) return 0;

	switch (AxisType) {
		case 0: // x
			camera->rotate_x(AngleDegree);
			break;
		case 1: // y
			camera->rotate_y(AngleDegree);
			break;
		case 2: // z
			camera->rotate_z(AngleDegree);
			break;
	}
	return 1;
}

//ZZ_SCRIPT
//void createIkChain (ZSTRING pIkName, ZSTRING pRootName, ZSTRING pMiddleName, ZSTRING pEndName)
//{
//	zz_bone * root, * middle, * end;
//	root = (zz_bone *)znzin->visibles->find(pRootName);
//	middle = (zz_bone *)znzin->visibles->find(pMiddleName);
//	end = (zz_bone *)znzin->visibles->find(pEndName);
//	zz_assert(root && middle && end);
//	if (!root || !middle || !end) return;
//	
//	zz_ik_limb * ik = (zz_ik_limb *)znzin->helpers->spawn(pIkName, ZZ_RUNTIME_TYPE(zz_ik_limb));
//	zz_assert(ik);
//	if (!ik) return;
//	
//	ik->set_chain(root, middle, end);
//}
//
//ZZ_SCRIPT
//void setIkTarget (ZSTRING pIkName, const float * TargetPosWorld)
//{
//	zz_ik_limb * ik = (zz_ik_limb *)znzin->helpers->find(pIkName);
//	zz_assert(ik);
//	if (!ik) return;
//
//	ik->set_target(vec3(TargetPosWorld[0], TargetPosWorld[1], TargetPosWorld[2]));
//}

ZZ_SCRIPT
int beginTrackball (HNODE hCamera, float ScreenX, float ScreenY)
{
	CHECK_INTERFACE(beginTrackball);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->trackball_begin(ScreenX, ScreenY);
	return 1;
}

ZZ_SCRIPT
int rotateTrackball (HNODE hCamera, float ScreenX, float ScreenY)
{
	CHECK_INTERFACE(rotateTrackball);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->trackball_rotate(ScreenX, ScreenY);
	return 1;
}

ZZ_SCRIPT
int endTrackball (HNODE hCamera, float ScreenX, float ScreenY)
{
	CHECK_INTERFACE(endTrackball);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->trackball_end(ScreenX, ScreenY);
	return 1;
}

ZZ_SCRIPT
int beginPan (HNODE hCamera, float ScreenX, float ScreenY)
{
	CHECK_INTERFACE(beginPan);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->pan_begin(ScreenX, ScreenY);
	return 1;
}

ZZ_SCRIPT
int doPan (HNODE hCamera, float ScreenX, float ScreenY, float PanSize)
{
	CHECK_INTERFACE(doPan);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->pan_move(ScreenX, ScreenY, PanSize);
	return 1;
}

ZZ_SCRIPT
int endPan (HNODE hCamera, float ScreenX, float ScreenY)
{
	CHECK_INTERFACE(endPan);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	cam->pan_end(ScreenX, ScreenY);
	return 1;
}

ZZ_SCRIPT
void buildScene (void)
{
	CHECK_INTERFACE(buildScene);
	znzin->scene.build();
	//ZZ_LOG("interface: buildScene() done.\n");
}

ZZ_SCRIPT
void cullScene (void)
{
	CHECK_INTERFACE(cullScene);
	znzin->scene.cull();
	//ZZ_LOG("interface: cullScene() done.\n");
}

ZZ_SCRIPT
int insertToScene (HNODE hVisible)
{
	CHECK_INTERFACE( insertToScene );

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	
	zz_assert(vis);

#if (0) // close-beta test code. can be deleted anytime
	{
		const vec3& pos = vis->get_position();
		if (_isnan(pos.x) || _isnan(pos.y) || _isnan(pos.z)) {
			ZZ_LOG("insertToScene(%s, %f, %f, %f) failed.\n", vis->get_name(), pos.x, pos.y, pos.z);
			vis->dump_hierarchy();
			zz_assertf(0, "invalid position");
		}
	}
#endif
	
	vis->insert_scene();
	
	//ZZ_LOG("insertToScene(%s)\n", vis->get_name());

	return 1;
}

ZZ_SCRIPT
int removeFromScene (HNODE hVisible)
{
	CHECK_INTERFACE( removeFromScene );

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	
	zz_assertf(vis, "removeFromScene() failed. null handle");

#ifdef _DEBUG
	if (!vis) return 0;
#endif

	//ZZ_LOG("removeFromScene(%s)\n", vis->get_name());

	vis->remove_scene();

	return 1;
}

ZZ_SCRIPT
int moveCamera (HNODE hCamera, float deviationX, float deviationY, float deviationZ)
{
	CHECK_INTERFACE(moveCamera);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);
	zz_assert(cam);
	if (!cam) return 0;
	vec3 deviation(deviationX, deviationY, deviationZ);
	deviation *= ZZ_SCALE_IN;
	cam->move(deviation);
	return 1;
}

ZZ_DLL
void getDefaultCameraMatrix( float *d3d_tm_4x4)
{
	CHECK_INTERFACE_SPRITE(getDefaultCameraMatrix);
    zz_camera *cam = znzin->get_camera();
	mat4 camera_m;
    
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
    
	memcpy(d3d_tm_4x4, camera_m.mat_array, sizeof(float)*16);
}

ZZ_DLL
void getDefaultProjectionMatrix( float *d3d_tm_4x4)
{
	CHECK_INTERFACE_SPRITE(getDefaultCameraMatrix);
    zz_camera *cam = znzin->get_camera();
	mat4 projection_m;
    
	cam->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_m);
    
	memcpy(d3d_tm_4x4, projection_m.mat_array, sizeof(float)*16);
}

ZZ_SCRIPT
int getScreenWidth (void)
{
	CHECK_INTERFACE(getScreenWidth);
	return znzin->view->get_width();
}

ZZ_SCRIPT
int getScreenHeight (void)
{
	CHECK_INTERFACE(getScreenHeight);
	return znzin->view->get_height();
}

ZZ_SCRIPT
int getFullScreen ( void )
{
	CHECK_INTERFACE(getFullScreen);
	return znzin->view->get_fullscreen() ? 1 : 0;
}

ZZ_SCRIPT
void setFogColor (float colorR, float colorG, float colorB)
{
	CHECK_INTERFACE(setFogColor);
	znzin->get_rs()->fog_color = vec3(colorR, colorG, colorB);
}

ZZ_SCRIPT
void setFogRange (float rangeStart, float rangeEnd)
{
	CHECK_INTERFACE(setFogRange);
	znzin->get_rs()->fog_start = rangeStart * ZZ_SCALE_IN;
	znzin->get_rs()->fog_end = rangeEnd * ZZ_SCALE_IN;
}

ZZ_SCRIPT
void setAlphaFogRange ( float rangeStart, float rangeEnd )
{
	CHECK_INTERFACE(setAlphaFogRange);
	znzin->get_rs()->alpha_fog_start = rangeStart * ZZ_SCALE_IN;
	znzin->get_rs()->alpha_fog_end = rangeEnd * ZZ_SCALE_IN;
}

ZZ_SCRIPT
int getUseFog (void)
{
	CHECK_INTERFACE(getUseFog);
	return znzin->get_rs()->use_fog ? 1 : 0;
}

ZZ_SCRIPT
int getUseWireMode (void)
{
	CHECK_INTERFACE(getUseWireMode);
	return znzin->get_rs()->use_wire_mode ? 1 : 0;
}


// This is not lua interface
ZZ_DLL
void initZnzin (void)
{
	//ZZ_LOG("interface: initZnzin()\n");

	ulong dxversion = get_dx_version();

	if (dxversion < DX_VERSION_9B) {
		
/*		char bufferString[256];
		sprintf(bufferString, "DirectX Version mismatch! [0x%x]", dxversion);
		MessageBox( NULL, bufferString, "Warning", MB_OK | MB_TOPMOST);
		//exit(EXIT_FAILURE);*/
	}

	if (!znzin) {
		znzin = zz_new zz_system;
		znzin->initialize();
	}

	if (znzin->renderer) {
		state = znzin->renderer->get_state();
	}
}

ZZ_DLL
void destZnzin (void)
{
	//ZZ_LOG("interface: destZnzin()\n");

	zz_profiler::statistics(true); // by time
	zz_profiler::statistics(false); // by count

	if (znzin) {
		ZZ_SAFE_DELETE(znzin);
		state = NULL;
	}
}

ZZ_DLL
int callScriptFunc (ZSTRING funcName, ...)
{
	CHECK_INTERFACE(callScriptFunc);
	va_list va;
	va_start(va, funcName);

	int ret = (znzin->script->call(funcName, va)) ? 1 : 0;

	va_end(va);
	//ZZ_LOG("interface: callScriptFunc(%s) done\n", funcName);
	return ret;
}

ZZ_DLL
int attachWindow (const void * window_handle)
{
	zz_assertf(znzin, "엔진이 초기화되지 않았습니다.");
	zz_assert(znzin->view);
	zz_assert(znzin->sfxs);

	znzin->view->set_handle((const void*)window_handle);
	if (!znzin->view->attach_window()) return 0;
	if (!znzin->sfxs->initialize()) return 0;

	return 1;
}

ZZ_DLL
int detachWindow (void)
{
	if (!znzin->sfxs->cleanup()) return 0;
	if (!znzin->view->detach_window()) return 0;

	return 1;
}

// not used in anywhere
ZZ_SCRIPT
int loadCharacter (ZSTRING characterFileName)
{
	CHECK_INTERFACE(loadCharacter);
	zz_script_simple zch;
	if (!zch.open(characterFileName)) return 0;
	char noname[ZZ_MAX_STRING];
	zch.read_string("version", noname);
	zch.read_string(NULL, noname);

	zz_model * model = (zz_model *)znzin->visibles->spawn(noname,
		ZZ_RUNTIME_TYPE(zz_model));	
	zz_assert(model);
	if (!model) return 0;
	zch.read_string("{");
	
	zch.read_string("skel", noname);
	zz_skeleton * skel = (zz_skeleton *)znzin->visibles->find(noname);
	model->attach_skeleton(skel);
	uint32 num_skin_object;
	zch.read_uint32("num_skins", num_skin_object);
	zz_mesh * mesh;
	zz_material * mat;
	zz_light * light = (zz_light *)znzin->lights->get_current();

	for (uint32 i = 0; i < num_skin_object; i++) {
		zch.read_string("{");
		zch.read_string("mesh", noname);
		mesh = (zz_mesh *)znzin->meshes->find(noname);
		zz_assert(mesh);
		zch.read_string("material", noname);
		mat = (zz_material *)znzin->materials->find(noname);
		zz_assert(mat);
		model->add_runit(mesh, mat, light);
		model->get_bvolume()->set_local_center(vec3_null);
		model->invalidate_transform();
		zch.read_string("}");
	}
	//zch.read_string("}");
	//ZZ_LOG("interface: loadCharacter(%s) done\n", characterFileName);
	return 1;
}

ZZ_SCRIPT
int callInt (ZSTRING funcString)
{
	znzin->script->do_script(NULL, funcString);
	return znzin->script->get_return_int();
}

ZZ_SCRIPT
HNODE callHNODE (ZSTRING funcString)
{
	znzin->script->do_script(NULL, funcString);
	return znzin->script->get_return_uint();
}

ZZ_SCRIPT
ZSTRING callString (ZSTRING funcString)
{
	znzin->script->do_script(NULL, funcString);
	return znzin->script->get_return_string();
}

ZZ_SCRIPT
float callFloat (ZSTRING funcString)
{
	znzin->script->do_script(NULL, funcString);
	return znzin->script->get_return_float();
}

// callFloat3...

ZZ_SCRIPT
ZSTRING getName (HNODE hNode)
{
	CHECK_INTERFACE(getName);
	zz_node * node = reinterpret_cast<zz_node *>(hNode);
	ZSTRING name = NULL;
	if (node) {
		name = node->get_name();
	}
	else {
		ZZ_LOG("interface: getName(%l) failed. node not found\n", hNode);
		name = NULL;
	}
	return name;
}

ZZ_SCRIPT
int setName ( HNODE hNode, ZSTRING pNewName )
{
	CHECK_INTERFACE(setName);
	zz_node * node = reinterpret_cast<zz_node *>(hNode);

	if (!node) return 0;
	
	if (node->set_name(pNewName)) return 1;
	return 0;
}


ZZ_SCRIPT
int cameraAttachTarget (HNODE hCamera, HNODE hModel)
{
	CHECK_INTERFACE(cameraAttachTarget);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	if (!cam) return 0;

	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: cameraAttachTarget() failed. not a follow camera\n");
		return 0;
	}
	zz_model * target = reinterpret_cast<zz_model *>(hModel);
	if (!target->is_a(ZZ_RUNTIME_TYPE(zz_model))) {
		ZZ_LOG("interface: cameraAttachTarget() failed. not a model class\n");
		return 0;
	}
	return cam->attach_target(target) ? 1 : 0;
}

ZZ_SCRIPT
int cameraDetachTarget (HNODE hCamera)
{
	CHECK_INTERFACE(cameraDetachTarget);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	if (!cam) return 0;

	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: cameraDetachTarget() failed. not a follow camera\n");
		return 0;
	}
	return cam->detach_target() ? 1 : 0;
}

ZZ_SCRIPT
int cameraUpdate (HNODE hCamera)
{
	CHECK_INTERFACE(cameraUpdate);
	
	return 1;
}

//ZZ_SCRIPT
//int cameraRotateAroundTarget (HNODE hCamera)
//{
//	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
//	if (!cam) return 0;
//
//	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
//		ZZ_LOG("interface: cameraRotateAroundTarget() failed. not a follow camera\n");
//		return 0;
//	}
//	return cam->rotate_around_target() ? 1 : 0;
//}

ZZ_SCRIPT
int lookAt (HNODE hCamera,
			 float eyeX, float eyeY, float eyeZ,
			 float centerX, float centerY, float centerZ,
			 float upX, float upY, float upZ)
{
	CHECK_INTERFACE(lookAt);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);

	if (cam == NULL) {
		ZZ_LOG("interface: lookAt() failed. camera not found.\n");
		return 0;
	}
	vec3 eye(eyeX, eyeY, eyeZ);
	vec3 center(centerX, centerY, centerZ);
	vec3 up(upX, upY, upZ);
	eye *= ZZ_SCALE_IN;
	center *= ZZ_SCALE_IN;
	up.normalize();
	cam->look_at(eye, center, up);
	return 1;
}

ZZ_DLL
HNODE loadTerrainMesh (
					   ZSTRING pMeshName,
					   float fOrigX, float fOrigY,
					   int iDetailLevel,
					   int iUVType0, int iUVType1,
					   int iWidth,
					   float * pHeightList,
					   int iMapSize,
					   int iBlockSize
					   )
{
	CHECK_INTERFACE(loadTerrainMesh);

	zz_assert(!znzin->terrain_meshes->find(pMeshName));

	zz_mesh_terrain * mesh = (zz_mesh_terrain*)znzin->terrain_meshes->spawn(
		pMeshName,
		ZZ_RUNTIME_TYPE(zz_mesh_terrain),
		false /* do_load */);

	zz_assert(mesh);

	mesh->set_property(
		ZZ_SCALE_IN, fOrigX, fOrigY, iDetailLevel, iUVType0, iUVType1, iWidth,
		pHeightList, float(iMapSize), float(iBlockSize));

	//mesh->set_lod(zz_lod::LOD_NEAR);
	//mesh->apply_lod();
	znzin->terrain_meshes->load((zz_node*)mesh);

	//ZZ_LOG("interface: loadTerrainMesh(%s) done.\n", pMeshName);
	return reinterpret_cast<HNODE>(mesh);
}

ZZ_DLL
HNODE loadTerrainMaterial (
	ZSTRING pMatName,
	HNODE hShader,
	HNODE pFirstTexture,
	HNODE pSecondTexture,
	HNODE pLightTexture
)
{
	CHECK_INTERFACE(loadTerrainMaterial);

	assert(!znzin->materials->find(pMatName));

	zz_shader * shader = reinterpret_cast<zz_shader*>(hShader);

	zz_assert(pFirstTexture);
	zz_assert(shader);

	if (!pFirstTexture || !shader) return 0;

	zz_material_terrain * terrainmap;

	terrainmap = (zz_material_terrain*)znzin->materials->spawn(
		pMatName,
		ZZ_RUNTIME_TYPE(zz_material_terrain));

	assert(terrainmap);
	
	terrainmap->set_shader(shader);

	zz_texture * tex_first, * tex_second, * tex_light;
	tex_first = reinterpret_cast<zz_texture*>(pFirstTexture);
	tex_second = reinterpret_cast<zz_texture*>(pSecondTexture);
	tex_light = reinterpret_cast<zz_texture*>(pLightTexture);

	//ZZ_LOG("interface: loadTerrainMaterial(%s)...\n\t1(%s, #%d)\n\t2(%s, #%d)\n\t3(%s, #%d)\n",
	//	terrainmap->get_name(),
	//	tex_first->get_name(), tex_first->get_refcount(),
	//	tex_second->get_name(), tex_second->get_refcount(),
	//	tex_light->get_name(), tex_light->get_refcount());
	
	terrainmap->set_texture(0, tex_first);
	terrainmap->set_texture(1, tex_second);
	terrainmap->set_texture(2, tex_light);

	terrainmap->set_texturealpha(false);
	terrainmap->set_alpha_test(false);

#ifdef ZZ_MATERIAL_LOADTEST
	terrainmap->loadtest_increase();
#endif

	return reinterpret_cast<HNODE>(terrainmap);
}

ZZ_SCRIPT
float getVisibility ( HNODE hVisible )
{
	CHECK_INTERFACE(getVisibility);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: getVisibility() failed\n");
		return 0.0f;
	}
	return vis->get_visibility();
}

ZZ_SCRIPT
int setVisibility ( HNODE hVisible, float fVisibility )
{
	CHECK_INTERFACE(setVisibility);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: setVisibility() failed\n");
		return 0;
	}
	vis->set_visibility(fVisibility);
	return 1;
}

ZZ_SCRIPT
int setVisibilityRecursive ( HNODE hVisible, float fVisibility )
{
	CHECK_INTERFACE(setVisibilityRecursive);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: setVisibilityRecursive() failed\n");
		return 0;
	}

	vis->set_visibility_recursive(fVisibility);

	return 1;
}

ZZ_DLL
void setLightingRecursive( HNODE hVisible, HNODE hLight)
{
	CHECK_INTERFACE(setLightingRecursive);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: setLightingRecursive() failed\n");
		return;
	}

	vis->set_lighting_recursive(light);
}

ZZ_DLL
int setShadowOnOff( HNODE hVisible, bool bShadowOnOff )
{
	CHECK_INTERFACE(setShadowOnOffRecursive);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: setVisibilityRecursive() failed\n");
		return 0;
	}

	vis->set_shadow_onoff(bShadowOnOff);

	return 1;
}


ZZ_DLL
int setShadowOnOffRecursive ( HNODE hVisible, bool bShadowOnOff )
{
	CHECK_INTERFACE(setShadowOnOffRecursive);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		// error
		ZZ_LOG("interface: setVisibilityRecursive() failed\n");
		return 0;
	}

	vis->set_shadow_onoff_recursive(bShadowOnOff);

	return 1;
}



// get clas node type
zz_node_type * getNodeType (int iType)
{
	switch (iType) {
		case ZZ_TYPE_MORPHER:
			return ZZ_RUNTIME_TYPE(zz_morpher);
		case ZZ_TYPE_SKELETON:
			return ZZ_RUNTIME_TYPE(zz_skeleton);
		case ZZ_TYPE_MODEL:
			return ZZ_RUNTIME_TYPE(zz_model);
		case ZZ_TYPE_PARTICLE:
			return ZZ_RUNTIME_TYPE(zz_particle_emitter);
		case ZZ_TYPE_TRAIL:
			return ZZ_RUNTIME_TYPE(zz_trail);
		case ZZ_TYPE_TERRAIN:
			return ZZ_RUNTIME_TYPE(zz_terrain_block);
		case ZZ_TYPE_OCEAN:
			return ZZ_RUNTIME_TYPE(zz_ocean_block);
		case ZZ_TYPE_VISIBLE:
			return ZZ_RUNTIME_TYPE(zz_visible);
		case ZZ_TYPE_ANIMATABLE:
			return ZZ_RUNTIME_TYPE(zz_animatable);
	}
	return NULL;
};

int getTypeOf ( HNODE hNode )
{
	zz_node * node = reinterpret_cast<zz_node*>(hNode);

	if (!node) return ZZ_TYPE_NONE;
	
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_morpher)))
		return ZZ_TYPE_MORPHER;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_skeleton)))
		return ZZ_TYPE_SKELETON;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_model)))
		return ZZ_TYPE_MODEL;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_particle_emitter)))
		return ZZ_TYPE_PARTICLE;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_trail)))
		return ZZ_TYPE_TRAIL;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)))
		return ZZ_TYPE_TERRAIN;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_ocean_block)))
		return ZZ_TYPE_OCEAN;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_visible)))
		return ZZ_TYPE_VISIBLE;
	if (node->is_a(ZZ_RUNTIME_TYPE(zz_animatable)))
		return ZZ_TYPE_MORPHER;
	return ZZ_TYPE_NONE;
}

ZZ_SCRIPT
int isA (HNODE hNode, int iType)
{
	CHECK_INTERFACE(isA);
	zz_node * node = reinterpret_cast<zz_node *>(hNode);
	if (!node) {
		// error
		ZZ_LOG("interface: isA(%d) failed\n", iType);
		return 0;
	}
	return node->is_a(getNodeType(iType));
}

ZZ_SCRIPT
int getNumSceneNode ( void )
{
	CHECK_INTERFACE(getNumSceneNode);
	return znzin->scene.get_num_viewfrustum_node();
}

ZZ_SCRIPT
HNODE getSceneNode ( int iIndex )
{
	CHECK_INTERFACE(getSceneNode);
	return reinterpret_cast<HNODE>(znzin->scene.get_viewfrustum_node(iIndex));
}

ZZ_SCRIPT
int inViewfrustum ( HNODE hVisible )
{
	CHECK_INTERFACE(inViewfrustum);
	if (hVisible == 0) return 0;
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	int ret = vis->get_infrustum() ? 1 : 0;

	//ZZ_LOG("interface: inViewfrustum(%s, %s) %d\n", vis->get_name(), vis->get_mesh(0)->get_path(), ret);
	return ret;
}

ZZ_SCRIPT
int getMotionTotalTime ( HNODE hMotion )
{
	CHECK_INTERFACE(getMotionTotalTime);
	zz_motion * motion = reinterpret_cast<zz_motion *>(hMotion);
	if (!motion) {
		ZZ_LOG("interface: getMotionTotalTime() failed\n");
		return 0;
	}
	return (int)motion->get_total_time();
}

ZZ_SCRIPT
int getMotionTotalFrame ( HNODE hMotion )
{
	CHECK_INTERFACE(getMotionTotalFrame);
	zz_motion * motion = reinterpret_cast<zz_motion *>(hMotion);
	if (!motion) {
		ZZ_LOG("interface: getMotionTotalFrame() failed\n");
		return 0;
	}
	return (int)motion->get_num_frames();
}

ZZ_SCRIPT
int renderNode (HNODE hNode)
{
	CHECK_INTERFACE(renderNode);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hNode);

	if (!vis) {
		ZZ_LOG("interface: renderNode() failed\n");
		return 0;
	}

	//vis->update(0);
	vis->render(false);

	return 1;
}

void executeFileCommand ( const char * file_name )
{
	static uint64 old_time = 0;
	static int count = 0;
	zz_vfs_local fs;
	
	if (count++ < 60) {
		return;
	}
	count = 0;

	const char * filepath = file_name;

	if (!fs.exist(filepath)) {
		char otherpath [256];
		sprintf(otherpath, "scripts/%s", file_name);
		filepath = otherpath;
		if (!fs.exist(filepath)) {
			return;
		}
	}

	uint64 current_time = 0;
	fs.get_mtime(filepath, &current_time);
	if (current_time != old_time) {
		znzin->script->do_script(filepath);
		old_time = current_time;
	}
}

ZZ_SCRIPT
void updateScene ( void )
{
#ifdef PROFILE_NORMAL
	CHECK_INTERFACE_FORCE(updateScene);
#else
	CHECK_INTERFACE(updateScene);
#endif

	znzin->tick_time();
	znzin->scene.update(znzin->get_diff_time());

	executeFileCommand("command.lua");
}

ZZ_DLL
void updateSceneEx ( void )
{
#ifdef PROFILE_NORMAL
	CHECK_INTERFACE_FORCE(updateScene);
#else
	CHECK_INTERFACE(updateScene);
#endif

	znzin->tick_time();
    znzin->camera_sfx.calculate_steal_camera();  
	znzin->scene.update_exp_camera(znzin->get_diff_time());
   
	executeFileCommand("command.lua");
}

ZZ_DLL
void updateSceneMovingCamera()
{
//	znzin->scene.updateEx(

}



ZZ_SCRIPT
void setDeltaTime ( int mSecDelta )
{
	assert(znzin);

	znzin->set_diff_time( ZZ_MSEC_TO_TIME( mSecDelta ) );
}

ZZ_SCRIPT
void updateSceneTransform ( void )
{
	znzin->scene.update_transform(znzin->get_diff_time());
}

ZZ_DLL
void updateSceneExAfter (void)
{
	znzin->scene.update_exp_camera_after(znzin->get_diff_time());
}

ZZ_SCRIPT
int beginScene ( void )
{
	CHECK_INTERFACE(beginScene);
    
	return znzin->view->begin_scene();
}

ZZ_SCRIPT
int endScene ( void )
{
	  	
	CHECK_INTERFACE(endScene);
    znzin->camera_sfx.return_camera();
	znzin->screen_sfx.post_render();  
	znzin->sprite_sfx.post_render();
	
	
	return znzin->view->end_scene();
}

ZZ_SCRIPT
void preProcessScene ( void )
{
	CHECK_INTERFACE(preProcessScene);

	znzin->renderer->pre_process();
}

// For now, this is embedded in renderScene.
ZZ_SCRIPT
void postProcessScene ( void )
{
	CHECK_INTERFACE(postProcessScene);

	znzin->renderer->post_process();
}

ZZ_SCRIPT
void renderScene ( void )
{
#ifdef PROFILE_NORMAL
	CHECK_INTERFACE_FORCE(renderScene);
#else
	CHECK_INTERFACE(renderScene);
#endif
	
	znzin->view->render();
	
	// this should be out of renderScene() and should be in front of beginSprite()
	znzin->renderer->post_process();
}

ZZ_SCRIPT
void clearScreen ( void )
{
	CHECK_INTERFACE(clearScreen);
	
	if(znzin->screen_sfx.get_widescreen_mode())
		znzin->screen_sfx.pre_clear_wide();

	znzin->renderer->clear_screen();

    if(znzin->screen_sfx.get_widescreen_mode())
		znzin->screen_sfx.post_clear_wide();

  	znzin->camera_sfx.steal_camera();
	znzin->sprite_sfx.pre_render();
	znzin->screen_sfx.pre_render();

}

// for internal use
void display_debug_message ()
{
	static size_t count = 0;
	static size_t num_delayed_resources_to_be_loaded;
	static size_t num_delayed_resources_to_be_unloaded;

	static size_t num_l_tex;
	static size_t num_l_nmesh; // normal mesh
	static size_t num_l_tmesh;
	static size_t num_l_omesh;

	static size_t num_u_tex;
	static size_t num_u_nmesh;
	static size_t num_u_tmesh;
	static size_t num_u_omesh;

	static int num_textures;
	static int num_polygons;
	static int num_meshes;

	static int num_frames = 0; // avatar num frames
	static int current_frames = 0; // avatar current frame

	static char msg[256] = "";

	char msg_total[256];
	char msg_texture[256];
	char msg_normal_mesh[256];
	char msg_terrain_mesh[256];
	char msg_ocean_mesh[256];

	zz_camera * cam = znzin->get_camera();

	if (cam->get_target()) {
		zz_model * avatar = cam->get_target();
		zz_motion * motion;
		if (avatar) {
			motion = avatar->get_motion();
			if (motion) {
				current_frames = avatar->get_motion_frame();
				num_frames = motion->get_num_frames();
			}
		}
	}
	else {
		cam = 0;
	}

	num_l_tex = znzin->textures->get_entrance_size();
	num_l_nmesh = znzin->meshes->get_entrance_size();
	num_l_tmesh = znzin->terrain_meshes->get_entrance_size();
	num_l_omesh = znzin->ocean_meshes->get_entrance_size();
	num_delayed_resources_to_be_loaded = num_l_tex + num_l_nmesh + num_l_tmesh + num_l_omesh;

	num_u_tex = znzin->textures->get_exit_size();
	num_u_nmesh = znzin->meshes->get_exit_size();
	num_u_tmesh = znzin->terrain_meshes->get_exit_size();
	num_u_omesh = znzin->ocean_meshes->get_exit_size();
	num_delayed_resources_to_be_unloaded = num_u_tex + num_u_nmesh + num_u_tmesh + num_u_omesh;

	num_textures = znzin->renderer->get_num_textures();
	num_polygons = znzin->renderer->get_num_polygons();
	num_meshes = znzin->renderer->get_num_meshes();

	if (num_delayed_resources_to_be_loaded + num_delayed_resources_to_be_unloaded > 0) {
		sprintf(msg_total, "d/u(% 3d/% 3d)=", num_delayed_resources_to_be_loaded, num_delayed_resources_to_be_unloaded);
	}
	else {
		sprintf(msg_total, "d/u(   /   )=");
	}
	if (num_l_tex + num_u_tex > 0) {
		sprintf(msg_texture, "t% 3d/% 3d", num_l_tex, num_u_tex);
	}
	else {
		sprintf(msg_texture, "t   /   ");
	}
	if (num_l_nmesh + num_u_nmesh > 0) {
        sprintf(msg_normal_mesh, "+nm% 3d/% 3d", num_l_nmesh, num_u_nmesh);
	}
	else {
		sprintf(msg_normal_mesh, "+nm   /   ");
	}
	if (num_l_tmesh + num_u_tmesh > 0) {
        sprintf(msg_terrain_mesh, "+tm% 3d/% 3d", num_l_tmesh, num_u_tmesh);
	}
	else {
		sprintf(msg_terrain_mesh, "+tm   /   ");
	}
	if (num_l_omesh + num_u_omesh > 0) {
        sprintf(msg_ocean_mesh, "+om% 3d/% 3d", num_l_omesh, num_u_omesh);
	}
	else {
		sprintf(msg_ocean_mesh, "+om   /   ");
	}

#ifdef _DEBUG
	drawFontLater(0, 0, 0, "DEBUG");
#endif

	sprintf(msg, "%s. m% 3d, t% 3d, p% 6d, (%s%s%s%s)\n",
		(znzin->get_rs()->use_delayed_loading) ? "On" : "Off",
		num_meshes, num_textures, num_polygons,
		msg_total, msg_texture, msg_normal_mesh, msg_terrain_mesh, msg_ocean_mesh);

	drawFontLater(0, 10, 120, msg); // by system font. and not use sprite

	int num_mesh_total, num_mesh_opaque, num_mesh_transparent, num_mesh_terrain, num_mesh_ocean;

	znzin->scene.get_statistics(&num_mesh_total, &num_mesh_transparent, &num_mesh_opaque, &num_mesh_terrain, &num_mesh_ocean);

	sprintf(msg, "mesh: [% 4d] = % 3d(op) + % 03d(tr) + % 03d(te) + % 03d(oc)",
		num_mesh_total, num_mesh_opaque, num_mesh_transparent, num_mesh_terrain, num_mesh_ocean);
	drawFontLaterf(0, 10, 140, "%s", msg);

	// draw texture mem usage
	unsigned int max_texmem, avail_texmem;
	max_texmem = znzin->renderer->get_max_texmem() >> 20; // byte -> mega byte
	avail_texmem = znzin->renderer->get_available_texmem() >> 20;

	drawFontLaterf(0, 10, 160, "texmem = [% 3d/% 3d, #% 3d]",
		max_texmem - avail_texmem,
		max_texmem,
		znzin->textures->get_texlist_size());

	drawFontLaterf(0, 200, 160, "MOTION = (%d/#%d)", current_frames, num_frames);
	
	drawFontLaterf(0, 10, 180, "FPS = [% 3d]", (int)getFps());

#ifdef LOG_SWAPTIME
	drawFontLaterf(0, 150, 180, "FC[#%05d]", frame_count);
#endif

#ifdef ZZ_MATERIAL_LOADTEST
	drawFontLaterf(0, 10, 200, "num_mat = %d", zz_material::loadtest_size());
#endif

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	int num_fill, num_total;
	znzin->vfs_thread->get_num_items(num_fill, num_total);
	drawFontLaterf(0, 10, 200, "thread_tex = %d/%d", num_fill, num_total);
#endif

	if (!cam) return;

	zz_model * model = reinterpret_cast<zz_model*>(cam->get_target());
	
	int node_count = 0;
	
	if (model) {
		node_count = ::collectByNodeBBox( reinterpret_cast<HNODE>(model) );
	}

	int start_posy = 220;
	zz_string meshpath, texpath;
	zz_visible * vis;
	for (int i = 0; i < node_count ; i++) {
		vis = reinterpret_cast<zz_visible*>(::getCollectNode(i));
		if (!vis || (vis->get_num_runits() == 0)) {
			drawFontLaterf(0, 10, start_posy + i * 15, "%03d: no render_unit\n", i);
			continue;
		}
		if (vis->get_mesh(0) && vis->get_mesh(0)->get_path()) {
			zz_path::extract_name(zz_string(vis->get_mesh(0)->get_path()), meshpath);
		}
		else {
			meshpath.set("no_mesh");
		}
		if (vis->get_material(0) && vis->get_material(0)->get_texture(0) && vis->get_material(0)->get_texture(0)->get_path()) {
			zz_path::extract_name(zz_string(vis->get_material(0)->get_texture(0)->get_path()), texpath);
		}
		else {
			texpath.set("no_tex");
		}
		
		if (vis && vis->get_name() && meshpath.get() && texpath.get()) {
			drawFontLaterf(0, 10, start_posy + i * 15, "%03d: %s:%s:%s\n", 
				i, vis->get_name(), meshpath.get(), texpath.get());
		}
		else {
			drawFontLaterf(0, 10, start_posy + i * 15, "%03d: Invalid\n", i);
		}
	}
}

struct zz_screenshots {
	bool started;
	int filetype;
	int num_skip_frame;

	zz_screenshots () : started(false), filetype(0), num_skip_frame(0)
	{
	}
};

zz_screenshots g_screenshots;

ZZ_SCRIPT
void saveScreenshots ( int bStart, int iFileType, int iNumSkipFrame )
{
	g_screenshots.started = (bStart != 0);
	g_screenshots.filetype = iFileType;
	g_screenshots.num_skip_frame = iNumSkipFrame;
}

ZZ_SCRIPT
void swapBuffers ( void  )
{
#ifdef PROFILE_NORMAL
	CHECK_INTERFACE_FORCE(swapBuffers);
#else
	CHECK_INTERFACE(swapBuffers);
#endif
	
	if (g_screenshots.started) { // zhotest
		static char save_filename[256];
		static int count = 0;
		static int skipframe = 0;
		static char exts[3][4] = {"dds", "bmp", "jpg"};

		if (++skipframe > g_screenshots.num_skip_frame) {
			skipframe = 0;

			sprintf(save_filename, "rose%05d.%s", count++, exts[g_screenshots.filetype]);
			znzin->renderer->save_to_file(save_filename, g_screenshots.filetype);
		}
	}

	if (state->use_debug_display) {
		display_debug_message();
	}

	zz_assert(znzin && znzin->renderer);
	znzin->renderer->swap_buffers(NULL);
	znzin->sleep();
	znzin->textures->update_delayed();

#ifdef LOG_SWAPTIME

	static uint64 start = 0, end = 0, diff = 0;
	static uint64 ticks_per_second;
	zz_os::get_ticks(end);
	diff = end - start;
	zz_os::get_ticks(start);
	zz_os::get_ticks_per_second(ticks_per_second);
	ZZ_LOG("interface: [#%05d]swap buffer done. time between swap = %lfms. fps = %d )--------------------\n\n",
		frame_count,
		double(diff)/ticks_per_second*1000.0,
		int(1.0 / (double(diff)/ticks_per_second)));
#endif
	frame_count++;
}
ZZ_DLL
void swapBuffersEx ( HWND hwnd )
{
#ifdef PROFILE_NORMAL
	CHECK_INTERFACE_FORCE(swapBuffers);
#else
	CHECK_INTERFACE(swapBuffers);
#endif
	
	if (g_screenshots.started) { // zhotest
		static char save_filename[256];
		static int count = 0;
		static int skipframe = 0;
		static char exts[3][4] = {"dds", "bmp", "jpg"};

		if (++skipframe > g_screenshots.num_skip_frame) {
			skipframe = 0;

			sprintf(save_filename, "rose%05d.%s", count++, exts[g_screenshots.filetype]);
			znzin->renderer->save_to_file(save_filename, g_screenshots.filetype);
		}
	}

	if (state->use_debug_display) {
		display_debug_message();
	}

	zz_assert(znzin && znzin->renderer);
	znzin->renderer->swap_buffers(hwnd);
	znzin->sleep();
	znzin->textures->update_delayed();

#ifdef LOG_SWAPTIME

	static uint64 start = 0, end = 0, diff = 0;
	static uint64 ticks_per_second;
	zz_os::get_ticks(end);
	diff = end - start;
	zz_os::get_ticks(start);
	zz_os::get_ticks_per_second(ticks_per_second);
	ZZ_LOG("interface: [#%05d]swap buffer done. time between swap = %lfms. fps = %d )--------------------\n\n",
		frame_count,
		double(diff)/ticks_per_second*1000.0,
		int(1.0 / (double(diff)/ticks_per_second)));
#endif
	frame_count++;
}



ZZ_SCRIPT
HNODE getDevice ( void )
{
	CHECK_INTERFACE(getDevice);
	zz_assert(znzin && znzin->renderer);
	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	static zz_renderer_d3d * d3d_renderer;
	d3d_renderer = static_cast<zz_renderer_d3d*>(znzin->renderer);
	return reinterpret_cast<HNODE>(d3d_renderer->get_device());
}


ZZ_SCRIPT
float getPositionX (HNODE hVisible)
{
	CHECK_INTERFACE(getPositionX);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		//ZZ_LOG("interface: getPositionX() failed\n");
		return 0;
	}
	float world_position_x = vis->get_worldTM().get_position().x;
	//ZZ_LOG("interface: getPositionX(%s) = %f\n", vis->get_name(), world_position_x);
	return world_position_x*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getPositionY (HNODE hVisible)
{
	CHECK_INTERFACE(getPositionY);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		//ZZ_LOG("interface: getPositionY() failed\n");
		return 0;
	}
	float world_position_y = vis->get_worldTM().get_position().y;
	//ZZ_LOG("interface: getPositionY(%s) = %f\n", vis->get_name(), world_position_y);
	return world_position_y*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getPositionZ (HNODE hVisible)
{
	CHECK_INTERFACE(getPositionZ);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		//ZZ_LOG("interface: getPositionZ() failed\n");
		return 0;
	}
	float world_position_z = vis->get_worldTM().get_position().z;
	//ZZ_LOG("interface: getPositionZ(%s) = %f\n", vis->get_name(), world_position_z);
	return world_position_z*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
int getPositionScript ( HNODE hVisible )
{
	CHECK_INTERFACE(getPositionScript);
	return getPosition(hVisible, float_array);
}

// 성공하면 1, 실패하면 0을 리턴.
ZZ_DLL
int getPosition ( HNODE hVisible, float fPositionXYZ[3] )
{
	CHECK_INTERFACE(getPosition);
	
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) {
		ZZ_LOG("interface: getPosition() failed\n");
		fPositionXYZ[0] = ZZ_INFINITE;
		fPositionXYZ[1] = ZZ_INFINITE;
		fPositionXYZ[2] = ZZ_INFINITE;
		return 0;
	}
	vec3& pos_world = FLOAT3_TO_VEC3(fPositionXYZ);
	vis->get_position_world(pos_world);
	pos_world *= ZZ_SCALE_OUT;
	//ZZ_LOG("interface: getPosition(%s, %f, %f, %f)\n", vis->get_name(), fPositionXYZ[0], fPositionXYZ[1], fPositionXYZ[2]);
	return 1;
}

ZZ_SCRIPT
void fadeIn ( float zeroToOne )
{
	CHECK_INTERFACE(fadeIn);
	if (!znzin->get_rs()->use_gamma) {
		//ZZ_LOG("interface: fadeIn(%f) failed. does not support gamma control\n", zeroToOne);
		return;
	}
	
    static zz_gamma gamma;
	
	gamma.interpolate(znzin->gamma_black, znzin->gamma_normal, zeroToOne);

	if (!znzin->renderer->set_gamma(gamma)) {
		ZZ_LOG("interface: fadeIn(%f) set_gamma() failed\n", zeroToOne);
	}
	else {
		//ZZ_LOG("interface: fadeIn(%f) done\n", zeroToOne);
	}
}

ZZ_SCRIPT
void setGammaValue ( float fGammaValue )
{
	CHECK_INTERFACE(setGammaValue);
	if (!znzin->get_rs()->use_gamma) {
		return;
	}
	ZZ_LOG("interface: setGammaValue(%f) done\n");
	znzin->renderer->set_gamma_by_value(fGammaValue);
}


ZZ_SCRIPT
int setModelMoveVelocity (HNODE hModel, float fVelocity)
{
	CHECK_INTERFACE(setModelMoveVelocity);
	static zz_model * model;
	
	model = reinterpret_cast<zz_model*>(hModel);

	if (!model) {
		ZZ_LOG("setModelMoveVelocity() failed.\n");
		return 0;
	}
	
	//ZZ_LOG("setModelMoveVelocity(%s, %f)\n", model->get_name(), fVelocity);
	model->set_move_velocity(fVelocity*ZZ_SCALE_IN);
	return 1;
}

ZZ_SCRIPT
float getModelMoveVelocity (HNODE hModel)
{
	CHECK_INTERFACE(getModelVelocity);
	static zz_model * model;
	model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("getModelVelocity() failed.\n");
		return 0.0f;
	}
	return model->get_move_velocity()*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
int setModelDirection (HNODE hModel, float fAngleDegree, int bImmediate)
{
	CHECK_INTERFACE(setModelDirection);
	static zz_model * model;
	model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("setModelDirection() failed.\n");
		return 0;
	}
	model->set_direction(fAngleDegree, ISTRUE(bImmediate));
	return 1;
}


ZZ_DLL
void SetModelVirtualTransform(HNODE hModel, float x, float y, float z)
{

    static zz_model * model;
	model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("SetModelVirtualTransform() failed.\n");
		return;
	}
	
	vec4 pos(ZZ_SCALE_IN * x ,ZZ_SCALE_IN * y, ZZ_SCALE_IN * z, 0.0f);
	model->set_virtual_Transform(pos);
}

ZZ_SCRIPT
float getModelDirection (HNODE hModel)
{
	CHECK_INTERFACE(getModelDirection);
	static zz_model * model;
	model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("getModelDirection() failed.\n");
		return 0.0f;
	}
	return model->get_direction();
}

ZZ_SCRIPT
int setModelDirectionByPosition ( HNODE hModel, float fX, float fY )
{
	CHECK_INTERFACE(setModelDirectionByPosition);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("getModelDirectionByPosition() failed.\n");
		return 0;
	}
	model->set_look_at(vec3(fX*ZZ_SCALE_IN, fY*ZZ_SCALE_IN, 0), 0 /* b_immediate */ );
	return 1;
}

ZZ_DLL
void setModelBlinkCloseMode(HNODE hModel, bool onoff)
{
	CHECK_INTERFACE(setModelBlinkCloseMode);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("setModelBlinkCloseMode() failed.\n");
		return;
	}
	model->set_blink_close_mode(onoff);
	
}

ZZ_DLL
bool getModelBlinkCloseMode(HNODE hModel)
{
    CHECK_INTERFACE(getModelBlinkCloseMode);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) {
		ZZ_LOG("getModelBlinkCloseMode() failed.\n");
		return 0;
	}
	return model->get_blink_close_mode();
}

ZZ_SCRIPT
int rotateAxis (HNODE hVisible, float fAngleDegree, float fAxisX, float fAxisY, float fAxisZ)
{
	CHECK_INTERFACE(rotateAxis);
	static zz_visible * vis;
	vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		return 0;
	}
	static vec3 axis;
	axis.set(fAxisX, fAxisY, fAxisZ);
	vis->rotate_by_axis(fAngleDegree*ZZ_TO_RAD, axis);
	vis->invalidate_transform();
	return 1;
}

ZZ_DLL
int rotateByVectorAbsolute ( HNODE hVisible, const float * fStartXYZ, const float * fEndXYZ )
{
	CHECK_INTERFACE(rotateByVector);
	static zz_visible * vis;
	vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		return 0;
	}
	vec3 start(fStartXYZ), end(fEndXYZ);
	start.normalize();
	end.normalize();
	vis->rotate_by_vector_absolute(start, end);
	vis->invalidate_transform();
	return 1;
}

ZZ_SCRIPT
int rotateByVectorAbsoluteScript (
	HNODE hVisible, 
	float fStartX, float fStartY, float fStartZ, 
	float fEndX, float fEndY, float fEndZ )
{
	CHECK_INTERFACE(rotateByVectorScript);
	float start[3], end[3];
	start[0] = fStartX; start[1] = fStartY; start[2] = fStartZ;
	end[0] = fEndX; end[1] = fEndY; end[2] = fEndZ;
	return rotateByVectorAbsolute(hVisible, start, end);
}

/// TODO: Optimize this!!
ZZ_DLL
int rotateByVectorRelative ( HNODE hVisible, const float fLocalStartAxisXYZ[3],
							const float fWorldEndTargetXYZ[3] )
{
	CHECK_INTERFACE(rotateByVectorRelative);
	static zz_visible * vis;
	vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		return 0;
	}
	vec3 local_start(fLocalStartAxisXYZ), world_end_target(fWorldEndTargetXYZ);
	vec3 world_start, world_origin;
	vec3 local_origin(vec3_null);
	world_end_target *= ZZ_SCALE_IN;

	// We assumes that the localStartAxisXYZ is already normalized vector
	vec3 world_end = world_end_target - vis->get_position();
	const mat4 wtm = vis->get_worldTM();
	mult(world_start, wtm, local_start); // this may be simply calculated by wtm.xyzw
	mult(world_origin, wtm, local_origin);
	world_start = world_start - world_origin;
	world_start.normalize();
	world_end.normalize();
	vis->rotate_by_vector_relative(world_start, world_end);
	vis->invalidate_transform();
	return 1;
}

/// TODO: Optimize this!!
ZZ_DLL
int rotateByVectorRelativeVec ( HNODE hVisible, const float fLocalStartAxisXYZ[3],
							const float fWorldEndTargetXYZ[3] )
{
	CHECK_INTERFACE(rotateByVectorRelativeVec);
	static zz_visible * vis;
	vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		return 0;
	}
	vec3 local_start(fLocalStartAxisXYZ), world_end(fWorldEndTargetXYZ);
	vec3 world_start, world_origin;
	vec3 local_origin(vec3_null);

	const mat4 wtm = vis->get_worldTM();
	mult(world_start, wtm, local_start);
	mult(world_origin, wtm, local_origin);
	world_start = world_start - world_origin;
	world_start.normalize();
	world_end.normalize();
	vis->rotate_by_vector_relative(world_start, world_end);
	vis->invalidate_transform();
	return 1;
}

ZZ_SCRIPT
int rotateByVectorRelativeScript (
	HNODE hVisible,
	float fLocalStartAxisX, float fLocalStartAxisY, float fLocalStartAxisZ,
	float fWorldEndTargetX, float fWorldEndTargetY, float fWorldEndTargetZ )
{
	CHECK_INTERFACE(rotateByVectorRelativeScript);
	float local_start_axis[3], world_end_target[3];
	local_start_axis[0] = fLocalStartAxisX;
	local_start_axis[1] = fLocalStartAxisY;
	local_start_axis[2] = fLocalStartAxisZ;
	world_end_target[0] = fWorldEndTargetX;
	world_end_target[1] = fWorldEndTargetY;
	world_end_target[2] = fWorldEndTargetZ;
	return rotateByVectorRelative(hVisible, local_start_axis, world_end_target);
}


ZZ_SCRIPT
int activateLog (int bTrueFalse)
{
	CHECK_INTERFACE(activateLog);
#ifdef FORCE_LOGGING
	return 1;
#else
	return zz_log::activate(ISTRUE(bTrueFalse)) ? 1 : 0;
#endif
}

ZZ_SCRIPT
int unloadMesh ( HNODE hMesh )
{
	CHECK_INTERFACE(unloadMesh);

	return unloadNode(hMesh);
}

ZZ_SCRIPT
int unloadMaterial ( HNODE hMaterial )
{
	CHECK_INTERFACE(unloadMaterial);

	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!mat) return 0;

#if (0)
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);
	if (mat) {
		ZZ_LOG("interface: unloadMaterial(%s)...refcount(%d)\n", mat->get_name(), mat->get_refcount());
		for (int i = 0; i < mat->get_num_textures(); i++) {
			zz_texture * tex =  mat->get_texture(i);
			if (tex) {
				ZZ_LOG("\t#d: %s. refcount(%d)\n", tex->get_name(), tex->get_refcount());
			}
		}
	}
#endif

#ifdef ZZ_MATERIAL_LOADTEST
	mat->loadtest_decrease();
#endif

	return unloadNode(hMaterial);
}

ZZ_SCRIPT
int unloadVisible ( HNODE hVisible)
{
	CHECK_INTERFACE(unloadVisible);
	return unloadNode(hVisible);
}

ZZ_SCRIPT
int unloadAnimatable ( HNODE hAnimatable )
{
	CHECK_INTERFACE(unloadAnimatable);
	return unloadNode(hAnimatable);
}


ZZ_SCRIPT
int unloadMorpher ( HNODE hMorpher )
{
	//zz_morpher * morpher = reinterpret_cast<zz_morpher*>(hMorpher);
	//ZZ_LOG("interface: unloadMorpher(%s, %s, %s)\n", morpher->get_name(), morpher->get_material(0)->get_name(), morpher->get_material(0)->get_texture(0)->get_path());

	CHECK_INTERFACE(unloadMorpher);
	return unloadNode(hMorpher);
}

ZZ_SCRIPT
int unloadSkeleton ( HNODE hSkeleton )
{
	CHECK_INTERFACE(unloadSkeleton);
	return unloadNode(hSkeleton);
}

ZZ_SCRIPT
int unloadModel ( HNODE hModel )
{
	CHECK_INTERFACE(unloadModel);

	return unloadNode(hModel);
}

ZZ_SCRIPT
int unloadLight ( HNODE hLight )
{
	CHECK_INTERFACE(unloadLight);
	return unloadNode(hLight);
}

ZZ_SCRIPT
int unloadShader ( HNODE hShader )
{
	CHECK_INTERFACE(unloadShader);
	return unloadNode(hShader);
}

ZZ_SCRIPT
int unloadMotion ( HNODE hMotion )
{
	CHECK_INTERFACE(unloadMotion);
	return unloadNode(hMotion);
}

ZZ_SCRIPT
int unloadCamera ( HNODE hCamera )
{
	CHECK_INTERFACE(unloadCamera);
	return unloadNode(hCamera);
}

ZZ_SCRIPT
int unloadTrail ( HNODE hTrail )
{
	CHECK_INTERFACE(unloadTrail);
	return unloadNode( hTrail );
}

ZZ_SCRIPT
int unloadNode ( HNODE hNode )
{
	CHECK_INTERFACE(unloadNode);

	zz_node * node = reinterpret_cast<zz_node *>(hNode);
	
	zz_assert(node);

	//ZZ_LOG("interface: unloadNode(%s:%s:%x) done.\n", node->get_name(), node->get_node_type()->type_name, node);

	zz_node * root = node->get_root();
	
	if (root->is_a(ZZ_RUNTIME_TYPE(zz_manager))) {
		zz_manager * man = static_cast<zz_manager *>(root);
		bool ret = man->kill(node);
		return static_cast<int>(ret);
	}
	zz_assert(!"no manager");
	return 0;
}

ZZ_SCRIPT
int reloadTextures ( void )
{
	CHECK_INTERFACE(reloadTextures);

	assert(znzin->textures);

	znzin->textures->unbind_device();

	return 1;
}

ZZ_SCRIPT
int getTimeDiff ( void )
{
	CHECK_INTERFACE(getTimeDiff);
	return int(ZZ_TIME_TO_MSEC(znzin->get_diff_time()));
}

ZZ_DLL
float getTimeDiffEx( void)
{
	return (float)znzin->get_diff_time();
}

ZZ_DLL
HNODE loadTerrainBlock (
	ZSTRING pTerrainBlockName,
	float fOrigX, float fOrigY,
	int iDetailLevel,
	int iUVType0, int iUVType1,
	float * pHeightList,
	int iMapSize, int iBlockSize,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadTerrainBlock);

	zz_mesh_terrain * tmesh = (zz_mesh_terrain*)znzin->terrain_meshes->spawn(
		NULL,
		ZZ_RUNTIME_TYPE(zz_mesh_terrain),
		false /* do_load */);

	tmesh->set_property(
		ZZ_SCALE_IN, 
		fOrigX, fOrigY,
		iDetailLevel,
		iUVType0, iUVType1, iDetailLevel+1,
		pHeightList, float(iMapSize), float(iBlockSize));

	znzin->terrain_meshes->load((zz_node*)tmesh);

	//HNODE hShader = findNode("shader_terrain");
	zz_material_colormap * firstmap, * secondmap, * lightmap;

	firstmap = reinterpret_cast<zz_material_colormap*>(hFirstMat);
	secondmap = reinterpret_cast<zz_material_colormap*>(hSecondMat);
	lightmap = reinterpret_cast<zz_material_colormap*>(hLightMat);

	zz_texture * first_tex, * second_tex, * light_tex;
	first_tex = (firstmap) ? firstmap->get_texture(0) : NULL;
	second_tex = (secondmap) ? secondmap->get_texture(0) : NULL;
	light_tex = (lightmap) ? lightmap->get_texture(0) : NULL;

	HNODE hMat = loadTerrainMaterial(
		0,
		hTerrainShader,
		reinterpret_cast<HNODE>(first_tex),
		reinterpret_cast<HNODE>((first_tex != second_tex) ? second_tex : 0),
		reinterpret_cast<HNODE>(light_tex)
		);		

#ifdef _DEBUG
	zz_terrain_block * tblock = (zz_terrain_block *)(znzin->terrain_blocks->find(pTerrainBlockName));
	
	if (tblock) {
		ZZ_LOG("interface: loadTerrainBlock(%s) already exists\n", pTerrainBlockName);
	}
	else {
		tblock = (zz_terrain_block *)znzin->terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block));
	}
#else
	zz_terrain_block * tblock = (zz_terrain_block *)znzin->terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block));
#endif
 
	zz_material * mat = reinterpret_cast<zz_material *>(hMat);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(tblock && tmesh && mat); // light can be NULL
	
	tblock->add_runit(tmesh, mat, light);
	// no to call invalidate_transform(). add_runits() does not affects transform

	//ZZ_LOG("interface: loadTerrainBlock(%s) done.\n", pTerrainBlockName);
	return reinterpret_cast<HNODE>(tblock);
}

ZZ_DLL
HNODE loadTerrainBlockEx (
	ZSTRING pTerrainBlockName,
	float fOrigX, float fOrigY,
	int iDetailLevel,
	int iUVType0, int iUVType1,
	float * pHeightList,
	float HeightMinMax[2],
	int iMapSize, int iBlockSize,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadTerrainBlock);

	zz_mesh_terrain * tmesh = (zz_mesh_terrain*)znzin->terrain_meshes->spawn(
		NULL,
		ZZ_RUNTIME_TYPE(zz_mesh_terrain),
		false /* do_load */);

	tmesh->set_property_ex(
		ZZ_SCALE_IN, 
		fOrigX, fOrigY,
		iDetailLevel,
		iUVType0, iUVType1, iDetailLevel+1,
		pHeightList, HeightMinMax,float(iMapSize), float(iBlockSize));

	znzin->terrain_meshes->load((zz_node*)tmesh);

	//HNODE hShader = findNode("shader_terrain");
	zz_material_colormap * firstmap, * secondmap, * lightmap;

	firstmap = reinterpret_cast<zz_material_colormap*>(hFirstMat);
	secondmap = reinterpret_cast<zz_material_colormap*>(hSecondMat);
	lightmap = reinterpret_cast<zz_material_colormap*>(hLightMat);

	zz_texture * first_tex, * second_tex, * light_tex;
	first_tex = (firstmap) ? firstmap->get_texture(0) : NULL;
	second_tex = (secondmap) ? secondmap->get_texture(0) : NULL;
	light_tex = (lightmap) ? lightmap->get_texture(0) : NULL;

	HNODE hMat = loadTerrainMaterial(
		0,
		hTerrainShader,
		reinterpret_cast<HNODE>(first_tex),
		reinterpret_cast<HNODE>((first_tex != second_tex) ? second_tex : 0),
		reinterpret_cast<HNODE>(light_tex)
		);		

#ifdef _DEBUG
	zz_terrain_block * tblock = (zz_terrain_block *)(znzin->terrain_blocks->find(pTerrainBlockName));
	
	if (tblock) {
		ZZ_LOG("interface: loadTerrainBlock(%s) already exists\n", pTerrainBlockName);
	}
	else {
		tblock = (zz_terrain_block *)znzin->terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block));
	}
#else
	zz_terrain_block * tblock = (zz_terrain_block *)znzin->terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block));
#endif
 
	zz_material * mat = reinterpret_cast<zz_material *>(hMat);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(tblock && tmesh && mat); // light can be NULL
	
	tblock->add_runit(tmesh, mat, light);
	// no to call invalidate_transform(). add_runits() does not affects transform

	//ZZ_LOG("interface: loadTerrainBlock(%s) done.\n", pTerrainBlockName);
	return reinterpret_cast<HNODE>(tblock);
}



ZZ_DLL
HNODE loadTerrainBlockExt (
	ZSTRING pTerrainBlockName,
	float fMinMax[2][3],
	int iDetailLevel,
	int iUVType0, int iUVType1,
	int iWidth,
	float * pHeightList,
	int iMapSize,
	int iBlockSize,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadTerrainBlockExt);

	zz_mesh_terrain * tmesh = (zz_mesh_terrain*)znzin->terrain_meshes->spawn(
		pTerrainBlockName,
		ZZ_RUNTIME_TYPE(zz_mesh_terrain),
		false /* do_load */);

	vec3 minmax[2];

	minmax[0].x = fMinMax[0][0]; 
	minmax[0].y = fMinMax[0][1]; 
	minmax[0].z = fMinMax[0][2]; 
	minmax[1].x = fMinMax[1][0]; 
	minmax[1].y = fMinMax[1][1]; 
	minmax[1].z = fMinMax[1][2]; 

	tmesh->set_property2(
		ZZ_SCALE_IN, 
		minmax,
		iUVType0, iUVType1,
		iWidth,
		0, // numskip
		pHeightList, float(iMapSize), float(iBlockSize));

	//tmesh->set_property(
	//	ZZ_SCALE_IN, 
	//	fOrigX, fOrigY,
	//	iDetailLevel,
	//	iUVType0, iUVType1, iDetailLevel+1,
	//	pHeightList, float(iMapSize), float(iBlockSize));

	znzin->terrain_meshes->load((zz_node*)tmesh);

	//HNODE hShader = findNode("shader_terrain");
	zz_material_colormap * firstmap, * secondmap, * lightmap;

	firstmap = reinterpret_cast<zz_material_colormap*>(hFirstMat);
	secondmap = reinterpret_cast<zz_material_colormap*>(hSecondMat);
	lightmap = reinterpret_cast<zz_material_colormap*>(hLightMat);

	zz_texture * first_tex, * second_tex, * light_tex;
	first_tex = (firstmap) ? firstmap->get_texture(0) : NULL;
	second_tex = (secondmap) ? secondmap->get_texture(0) : NULL;
	light_tex = (lightmap) ? lightmap->get_texture(0) : NULL;

	HNODE hMat = loadTerrainMaterial(
		0,
		hTerrainShader,
		reinterpret_cast<HNODE>(first_tex),
		reinterpret_cast<HNODE>(second_tex),
		reinterpret_cast<HNODE>(light_tex)
		);		

	zz_terrain_block * tblock = (zz_terrain_block *)znzin->terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block));
 
	zz_material * mat = reinterpret_cast<zz_material *>(hMat);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(tblock && tmesh && mat); // light can be NULL
	
	tblock->add_runit(tmesh, mat, light);
	// no to call invalidate_transform(). add_runits() does not affects transform

	//ZZ_LOG("interface: loadTerrainBlock(%s) done.\n", pTerrainBlockName);
	return reinterpret_cast<HNODE>(tblock);
}

ZZ_DLL
HNODE loadTerrainBlockRough (
	ZSTRING pTerrainBlockName,
	float fMinMax[2][3],
	int iWidth,
	float * pHeightList,
	ZSTRING pTexturePath,
	HNODE hRoughTerrainShader,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadTerrainBlockRough);

	zz_mesh_terrain_rough * rtmesh = (zz_mesh_terrain_rough*)znzin->rough_terrain_meshes->spawn(
		pTerrainBlockName,
		ZZ_RUNTIME_TYPE(zz_mesh_terrain_rough),
		false /* do_load */ );

	vec3 minmax[2];

	// dirty: for now, client sends wrong minmax info.
	minmax[0].x = (fMinMax[0][0] < fMinMax[1][0]) ? fMinMax[0][0] : fMinMax[1][0]; 
	minmax[0].y = (fMinMax[0][1] < fMinMax[1][1]) ? fMinMax[0][1] : fMinMax[1][1]; 
	minmax[0].z = (fMinMax[0][2] < fMinMax[1][2]) ? fMinMax[0][2] : fMinMax[1][2];
	minmax[1].x = (fMinMax[0][0] > fMinMax[1][0]) ? fMinMax[0][0] : fMinMax[1][0];
	minmax[1].y = (fMinMax[0][1] > fMinMax[1][1]) ? fMinMax[0][1] : fMinMax[1][1];
	minmax[1].z = (fMinMax[0][2] > fMinMax[1][2]) ? fMinMax[0][2] : fMinMax[1][2];

	// simple:
	//minmax[0].x = fMinMax[0][0]; 
	//minmax[0].y = fMinMax[0][1]; 
	//minmax[0].z = fMinMax[0][2]; 
	//minmax[1].x = fMinMax[1][0]; 
	//minmax[1].y = fMinMax[1][1]; 
	//minmax[1].z = fMinMax[1][2]; 

	float blocksize = minmax[1].x - minmax[0].x;
	int iNumSkip = 3; // 64 -> 32

	rtmesh->set_property2(
		ZZ_SCALE_IN, 
		minmax,
		zz_mesh_tool::ZZ_UV_NORMAL, zz_mesh_tool::ZZ_UV_NORMAL,
		iWidth,
		iNumSkip,
        pHeightList,
		blocksize, blocksize );

	znzin->rough_terrain_meshes->load((zz_node*)rtmesh);

	zz_texture * first_tex = (zz_texture *)loadTexture(NULL, pTexturePath, 1 /* miplevel */, 1 /* use filter */ );

	HNODE hMat = loadTerrainMaterial(
		0,
		hRoughTerrainShader,
		reinterpret_cast<HNODE>(first_tex),
		0, // second tex
		0 // light tex
		);		

	zz_terrain_block_rough * rtblock = static_cast<zz_terrain_block_rough *>(
		znzin->rough_terrain_blocks->spawn(pTerrainBlockName, ZZ_RUNTIME_TYPE(zz_terrain_block_rough)));
 
	rtblock->set_receive_shadow(false);

	zz_material * mat = reinterpret_cast<zz_material *>(hMat);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	
	zz_assert(rtblock && rtmesh && mat); // light can be NULL

	if (!rtblock || !rtmesh || !mat) return 0;

	rtblock->add_runit(rtmesh, mat, light);
	// no to call invalidate_transform(). add_runits() does not affects transform

	//ZZ_LOG("interface: loadTerrainBlock(%s) done.\n", pTerrainBlockName);
	return reinterpret_cast<HNODE>(rtblock);
}

ZZ_DLL
void unloadTerrainBlockRough ( HNODE hTerrainBlockRough )
{
	CHECK_INTERFACE(unloadTerrainBlockRough);
	
	zz_terrain_block_rough * rtblock = reinterpret_cast<zz_terrain_block_rough*>(hTerrainBlockRough);
	
	zz_assert(rtblock);

	zz_assert(rtblock->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)));
	zz_assert(rtblock->get_mesh(0));
	
	zz_mesh * mesh = rtblock->get_mesh(0); // get first mesh
	zz_material * mat = rtblock->get_material(0); // get first material

	zz_assert(mesh);
	zz_assert(mat);
	
	zz_texture * tex = mat->get_texture(0);
	zz_assert(tex);

	// unload terrain block
	unloadVisible(hTerrainBlockRough);

	// unload terrain mesh, born in loadTerrainBlock()
	unloadMesh(reinterpret_cast<HNODE>(mesh));

	unloadMaterial(reinterpret_cast<HNODE>(mat));

	unloadTexture(reinterpret_cast<HNODE>(tex));
}

ZZ_SCRIPT
HNODE loadTerrainBlockTest (
	ZSTRING pTerrainBlockName,
	int iX,
	int iY,
	int iType,
	HNODE hFirstMat,
	HNODE hSecondMat,
	HNODE hLightMat,
	HNODE hTerrainShader,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadTerrainBlockTest);
	const int iDetailLevel = 4;
	const int iWidth = 5;

	assert(iDetailLevel < iWidth);

	int iUVType0 = 0, iUVType1 = 1;
	static float pHeightList[iWidth][iWidth];

	int iMapSize = 16000;
	int iBlockSize = 1000;
	
	const int iWidth2 = 65;

	static float pHeightList2[iWidth2][iWidth2];
	if (iType == 1) {
		iMapSize = 16000;
		iBlockSize = 16000;
	}

	int num = 16;
	float fOrigX = float(iX*iBlockSize);
	float fOrigY = float(iY*iBlockSize + iBlockSize);
	
	srand(iX*iY);

	int k, l;
	float height;
	float min_height, max_height;
	int width;

	if (iType == 0) {
		width = iWidth;
	}
	else {
		width = iWidth2;
	}

	for (k = 0; k < width; k++) {
		for (l = 0; l < width; l++) {
			//height = iY*iY*(1.0f - float(k)/(width-1)) + (iY+1)*(iY+1)*(float(k)/(width-1)); 
			height = 0;
			if ((k == 0) && (l == 0)) {
				min_height = height;
				max_height = height;
			}
			else {
				if (height < min_height) min_height = height;
				if (height > max_height) max_height = height;
			}

			if (iType == 0) {
				pHeightList[width - 1 - k][l] = height;
			}
			else {
				pHeightList2[width - 1 - k][l] = height;
			}
		}
	}

	float fMinMax[2][3];

	fMinMax[0][0] = fOrigX;
	fMinMax[0][1] = fOrigY - iBlockSize;
	fMinMax[0][2] = min_height;
	fMinMax[1][0] = fOrigX + iBlockSize;
	fMinMax[1][1] = fOrigY;
	fMinMax[1][2] = max_height;

	if (iType == 0) {
		return loadTerrainBlockExt(pTerrainBlockName, fMinMax, iDetailLevel, iUVType0, iUVType1, iWidth, 
			(float*)pHeightList, iMapSize, iBlockSize, hFirstMat, hSecondMat, hLightMat,
			hTerrainShader, hLight);
	}
	else { // rough
		return loadTerrainBlockRough(pTerrainBlockName, fMinMax, iWidth2,
			(float*)pHeightList2, "etc/lightmap.dds",
			hTerrainShader, hLight);
	}
}

ZZ_DLL
void setTerrainBlockIndexOrder ( HNODE hTerrain, int iType )
{
	zz_terrain_block * tblock = reinterpret_cast<zz_terrain_block *>(hTerrain);

	zz_assert(iType >= 0);
	zz_assert(tblock);

	if (znzin->get_rs()->use_terrain_lod)
		tblock->set_index_order(iType);
}

ZZ_SCRIPT
int selectNode ( HNODE hNode )
{
	CHECK_INTERFACE(selectNode);
	zz_node * node = reinterpret_cast<zz_node*>(hNode);
	if (!node) return 0;

	if (node->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		znzin->visibles->set_current(node);
		return 1;
	}
	return 0;
}

ZZ_SCRIPT
float getGlobalFloat ( ZSTRING pName )
{
	CHECK_INTERFACE(getGlobalFloat);
	float fvalue;
	if (!znzin->script->get_global(pName, fvalue)) {
		ZZ_LOG("interface: getGlobalFloat(%s) failed\n", pName);
		return 0.0f;
	}
	return fvalue;
}


ZZ_SCRIPT
int setCameraFollowYaw ( HNODE hCamera, float fYawDeg )
{
	CHECK_INTERFACE(setCameraFollowYaw);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	if (!cam) return 0;

	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: setCameraFollowYaw() failed. not a follow camera\n");
		return 0;
	}

	cam->set_yaw(fYawDeg*ZZ_TO_RAD);    

/*////////////////////////////////////////////////////////////////           test 11-25
	if(fabsf(fYawDeg)<0.0001f)
    cam->set_yaw(0.0f); 
	else
	cam->set_yaw(180.0f*fYawDeg/(fabsf(fYawDeg))*ZZ_TO_RAD); 
///////////////////////////////////////////////////////////////	*/
	return 1;
}

ZZ_SCRIPT
int setCameraFollowPitch ( HNODE hCamera, float fPitch )
{
	CHECK_INTERFACE(setCameraFollowPitch);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	if (!cam) return 0;

	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: setCameraFollowPitch() failed. not a follow camera\n");
		return 0;
	}

	cam->set_pitch(fPitch); 
	return 1;
}

ZZ_SCRIPT
int setCameraFollowDistance ( HNODE hCamera, float fDistance )
{
	CHECK_INTERFACE(setCameraFollowDistance);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	zz_assert(cam);

	zz_assert(cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow)));

	cam->set_distance(fDistance*ZZ_SCALE_IN);
	return 1;
}

ZZ_SCRIPT
void setCameraFollowDistanceRange ( HNODE hCamera, float fMinDistance, float fMaxDistance )
{
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);
	zz_assert(cam);

	zz_assert(cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow)));
	cam->set_distance_range(fMinDistance*ZZ_SCALE_IN, fMaxDistance*ZZ_SCALE_IN);
}

int setReceiveFog ( HNODE hVisible, int bReceiveFog )
{
	CHECK_INTERFACE(setReceiveFog);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: setReceiveFog() failed.\n");
		return 0;
	}
	vis->set_receive_fog(ISTRUE(bReceiveFog));
	return 1;
}

int setReceiveShadow ( HNODE hVisible, int bReceiveShadow )
{
	CHECK_INTERFACE(setReceiveShadow);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: setReceiveShadow() failed.\n");
		return 0;
	}
	vis->set_receive_shadow(ISTRUE(bReceiveShadow));
	return 1;
}

int setCastShadow ( HNODE hVisible, int bCastShadow )
{
	CHECK_INTERFACE(setCastShadow);
	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: setCastShadow() failed.\n");
		return 0;
	}
	vis->set_cast_shadow(ISTRUE(bCastShadow));
	return 1;
}

ZZ_SCRIPT
int setCameraFollowMode ( HNODE hCamera, int mode_Look0_Back1 )
{
	CHECK_INTERFACE(setCameraFollowMode);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow*>(hCamera);
	if (!cam || !cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: setCameraFollowMode() failed.\n");
		return 0;
	}
	cam->set_follow_mode(zz_camera_follow::zz_follow_mode(mode_Look0_Back1));
	return 1;
}

ZZ_SCRIPT
float getCameraFollowYaw ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraFollowYaw);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow*>(hCamera);

	if (!cam || !cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: getCameraFollowYaw() failed.\n");
		return 0.0f;
	}
	return cam->get_yaw()*ZZ_TO_DEG;
}

ZZ_SCRIPT
float getCameraFollowPitch ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraFollowPitch);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow*>(hCamera);

	if (!cam || !cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: getCameraFollowPitch() failed.\n");
		return 0.0f;
	}
	return cam->get_pitch();
}

ZZ_SCRIPT
float getCameraFollowDistance ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraFollowDistance);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow*>(hCamera);

	if (!cam || !cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: getCameraFollowDistance() failed.\n");
		return 0.0f;
	}
	return cam->get_distance()*ZZ_SCALE_OUT;
}

ZZ_DLL
int getCameraEye ( HNODE hCamera, float posEye[3] )
{
	CHECK_INTERFACE(getCameraEye);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraEye() failed.\n");
		return 0;
	}
	vec3 eye = cam->get_eye();
	posEye[0] = eye.x*ZZ_SCALE_OUT;
	posEye[1] = eye.y*ZZ_SCALE_OUT;
	posEye[2] = eye.z*ZZ_SCALE_OUT;
	return 1;
}

ZZ_DLL
int getCameraDir ( HNODE hCamera, float dirFront[3] )
{
	CHECK_INTERFACE(getCameraDir);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraDir() failed.\n");
		return 0;
	}
	vec3 front = cam->get_dir();
	dirFront[0] = front.x;
	dirFront[1] = front.y;
	dirFront[2] = front.z;
	return 1;
}

ZZ_DLL
int getCameraUp ( HNODE hCamera, float dirUp[3] )
{
	CHECK_INTERFACE(getCameraUp);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraUp() failed.\n");
		return 0;
	}
	vec3 up = cam->get_up();
	dirUp[0] = up.x;
	dirUp[1] = up.y;
	dirUp[2] = up.z;
	return 1;
}

ZZ_DLL
int getCameraViewfrustum ( HNODE hCamera, float fViewfrustum_Out[6][4] )
{
	CHECK_INTERFACE(getCameraViewfrustum);
	
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) return 0;

	//cam->update_frustum(4000.0f*ZZ_SCALE_IN);

	const zz_viewfrustum& vf = cam->get_frustum();

	fViewfrustum_Out[0][0] = vf.np.x;
	fViewfrustum_Out[0][1] = vf.np.y;
	fViewfrustum_Out[0][2] = vf.np.z;
	fViewfrustum_Out[0][3] = ZZ_SCALE_OUT*vf.np.w;

	fViewfrustum_Out[1][0] = vf.fp.x;
	fViewfrustum_Out[1][1] = vf.fp.y;
	fViewfrustum_Out[1][2] = vf.fp.z;
	fViewfrustum_Out[1][3] = ZZ_SCALE_OUT*vf.fp.w;

	fViewfrustum_Out[2][0] = vf.lp.x;
	fViewfrustum_Out[2][1] = vf.lp.y;
	fViewfrustum_Out[2][2] = vf.lp.z;
	fViewfrustum_Out[2][3] = ZZ_SCALE_OUT*vf.lp.w;

	fViewfrustum_Out[3][0] = vf.rp.x;
	fViewfrustum_Out[3][1] = vf.rp.y;
	fViewfrustum_Out[3][2] = vf.rp.z;
	fViewfrustum_Out[3][3] = ZZ_SCALE_OUT*vf.rp.w;

	fViewfrustum_Out[4][0] = vf.tp.x;
	fViewfrustum_Out[4][1] = vf.tp.y;
	fViewfrustum_Out[4][2] = vf.tp.z;
	fViewfrustum_Out[4][3] = ZZ_SCALE_OUT*vf.tp.w;

	fViewfrustum_Out[5][0] = vf.bp.x;
	fViewfrustum_Out[5][1] = vf.bp.y;
	fViewfrustum_Out[5][2] = vf.bp.z;
	fViewfrustum_Out[5][3] = ZZ_SCALE_OUT*vf.bp.w;

	return 1;
}

ZZ_SCRIPT
int setUseTimeWeight ( int bTrueOrFalse )
{
	CHECK_INTERFACE(setUseTimeWeight);
	bool last_value = znzin->get_use_time_weight();
	znzin->set_use_time_weight(ISTRUE(bTrueOrFalse));
	return (last_value) ? 1 : 0;
}

ZZ_SCRIPT
int setUseFixedFramerate ( int bTrueOrFalse )
{
	CHECK_INTERFACE(setUseFixedFramerate);
	bool last_value = znzin->get_use_fixed_framerate();
	znzin->set_use_fixed_framerate(ISTRUE(bTrueOrFalse));
	return (last_value) ? 1 : 0;
}

ZZ_SCRIPT
int enableRenderState ( int bTrueOrFalse )
{
	CHECK_INTERFACE(enableRenderState);
	bool last_value = znzin->get_rs()->use_render_state;
	znzin->get_rs()->use_render_state = (ISTRUE(bTrueOrFalse));
	return (last_value) ? 1 : 0;
}

ZZ_SCRIPT
HNODE loadSky ( ZSTRING pSkyName, HNODE hMesh, HNODE hMaterial, HNODE hLight )
{
	CHECK_INTERFACE(loadSky);

	zz_sky * sky;
	
	zz_assert(!znzin->visibles->find(pSkyName));
	
	sky = (zz_sky *)znzin->visibles->spawn(pSkyName, ZZ_RUNTIME_TYPE(zz_sky));

	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_material_colormap * mat = reinterpret_cast<zz_material_colormap *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);

	zz_assert(sky && mesh && mat); // light can be NULL

	sky->add_runit(mesh, mat, light);

	// sky must not in the scene
	znzin->sky = sky;
	return reinterpret_cast<HNODE>(sky);
}

ZZ_SCRIPT
int unloadSky ( HNODE hSky )
{
	CHECK_INTERFACE(unloadSky);
	return unloadNode(hSky);
}

ZZ_SCRIPT
int setSkyRotationSpeed ( HNODE hSky, int iSkyUnit, float fAngleDegreePerSecond )
{
	CHECK_INTERFACE(setSkyRotationSpeed);
	zz_sky * sky = reinterpret_cast<zz_sky*>(hSky);
	
	if (!sky) return 0;

	return sky->set_rotation_deltas(iSkyUnit, fAngleDegreePerSecond) ? 1 : 0;
}

ZZ_SCRIPT
int getMotionFrame ( HNODE hNode )
{
	CHECK_INTERFACE(getMotionFrame);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hNode);
	
	if (!ani) return -1;

	if (!ani->is_a(ZZ_RUNTIME_TYPE(zz_animatable))) return -1;

	return (int)ani->get_motion_frame();
}

// TODO: optimize these calls by stack interface
ZZ_SCRIPT
float pickScreenX ( int screenX, int screenY )
{
	CHECK_INTERFACE(pickScreenX);
	vec3 picked_pos;
	zz_camera * cam = znzin->get_camera();
	zz_assert(cam);
	vec2 pos_cur;
	pos_cur.x = (float)screenX;
	pos_cur.y = (float)screenY;
	cam->pick(pos_cur, picked_pos, 0 /* z_value */);
	return picked_pos.x*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float pickScreenY ( int screenX, int screenY )
{
	CHECK_INTERFACE(pickScreenY);
	vec3 picked_pos;
	zz_camera * cam = znzin->get_camera();
	zz_assert(cam);
	vec2 pos_cur;
	pos_cur.x = (float)screenX;
	pos_cur.y = (float)screenY;
	cam->pick(pos_cur, picked_pos, 0 /* z_value */);
	return picked_pos.y*ZZ_SCALE_OUT;
}

ZZ_DLL
bool pickScreenXY ( int screenX, int screenY, float * worldX, float * worldY )
{
	CHECK_INTERFACE(pickScreenXY);
	vec3 picked_pos;
	zz_camera * cam = znzin->get_camera();
	if (!cam) return false;
	vec2 pos_cur;
	pos_cur.x = (float)screenX;
	pos_cur.y = (float)screenY;
	cam->pick(pos_cur, picked_pos, 0 /* z_value */);
	*worldX = picked_pos.x*ZZ_SCALE_OUT;
	*worldY = picked_pos.y*ZZ_SCALE_OUT;
	return true;
}

ZZ_DLL
int intersectRay (
	HNODE hNode,
	float originX, float originY, float originZ,
	float dirX, float dirY, float dirZ,
	float * pContactPointX, float * pContactPointY, float * pContactPointZ,
	float * pDistance
)
{
	CHECK_INTERFACE(intersectRay);
	// sphere-ray check
	zz_visible * vis = reinterpret_cast<zz_visible*>(hNode);

	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		ZZ_LOG("interface: intersectRay() failed. no visible\n");
		return 0;
	}

	if( vis->get_visibility() < 0.01f)
	{
		
		return 0;
	}
	
	
	vec3 origin_in(originX, originY, originZ), origin;
	vec3 dir(dirX, dirY, dirZ);
	vec3 contact_point;
	vec3 contact_normal;
	origin = ZZ_SCALE_IN*origin_in;

	bool ret;


	if (vis->is_a(ZZ_RUNTIME_TYPE(zz_model))) {
#if (0) 
		// if too close, does not detail collision test
		const float max_distance_square_from_camera = ZZ_SCALE_IN*ZZ_SCALE_IN*10000.0f*10000.0f;
		if (vis->get_camdist_square() < max_distance_square_from_camera) // If this is close enough
		{
			ret = vis->get_intersection_ray_level(origin, dir, contact_point, contact_normal, ZZ_CL_OBB, zz_mesh_tool::ZZ_SM_ANY);
		}
		else
		{
			ret = vis->get_intersection_ray_level(origin, dir, contact_point, contact_normal, ZZ_CL_SPHERE, zz_mesh_tool::ZZ_SM_ANY);
		    
		}
#else
		// always detail collision test
		ret = vis->get_intersection_ray_level(origin, dir, contact_point, contact_normal, ZZ_CL_OBB, zz_mesh_tool::ZZ_SM_ANY);
        
#endif
	}
	else {
		ret = vis->get_intersection_ray(origin, dir, contact_point, contact_normal, zz_mesh_tool::ZZ_SM_NEAREST);
	}

	if (ret) {
		*pContactPointX = contact_point.x*ZZ_SCALE_OUT;
		*pContactPointY = contact_point.y*ZZ_SCALE_OUT;
		*pContactPointZ = contact_point.z*ZZ_SCALE_OUT;
		if (pDistance) 
			*pDistance = origin.distance(contact_point)*ZZ_SCALE_OUT;
		return 1;
	}
	return 0;
}

ZZ_DLL
int intersectRay2OBB (float *pOrigin, float *pDirection, float *pCenter, float *pRotation, float xLength, float yLength, float zLength)
{
	zz_bounding_obb obb;
	quat rotation;

	obb.center.x = pCenter[0]; obb.center.y = pCenter[1]; obb.center.z = pCenter[2];
	rotation.x = pRotation[0]; rotation.y = pRotation[1]; rotation.z = pRotation[2]; rotation.w = pRotation[3];
	rotation.to_matrix(obb.rotation);
    obb.half_length.x = xLength;
	obb.half_length.y = yLength;
	obb.half_length.z = zLength;
    
    vec3 origin, direction;

	origin.x = pOrigin[0]; origin.y = pOrigin[1]; origin.z = pOrigin[2];
	direction.x = pDirection[0]; direction.y = pDirection[1]; direction.z = pDirection[2]; 

	if(intersect (obb, origin, direction))
	{
		return 1;
	}
	
	return 0;
	
}





ZZ_SCRIPT
int intersectRayScript ( HNODE hNode, float originX, float originY, float originZ, float dirX, float dirY, float dirZ )
{
	CHECK_INTERFACE(intersectRayScript);
	return (intersectRay(hNode,
		originX, originY, originZ, dirX, dirY, dirZ,
		&float_array[0], &float_array[1], &float_array[2], &float_array[3]));
}

ZZ_SCRIPT
int intersectNodeSphereSphere ( HNODE hNodeA, HNODE hNodeB )
{
	CHECK_INTERFACE(intersectNodeSphereSphere);
	zz_visible * visA = reinterpret_cast<zz_visible*>(hNodeA);
	zz_visible * visB = reinterpret_cast<zz_visible*>(hNodeB);

	if (!visA || !visA->is_a(ZZ_RUNTIME_TYPE(zz_visible)) ||
		!visA || !visA->is_a(ZZ_RUNTIME_TYPE(zz_visible)))
	{
		ZZ_LOG("interface: intersectNodeSphereSphere() failed. no visible\n");
		return 0;
	}
	int result = visA->test_intersection_node(visB, ZZ_CL_SPHERE);
	
	//ZZ_LOG("intersectNodeSphereSphere(%d, %d) = %d\n", (int)hNodeA, (int)hNodeB, result);
	return result;
}

ZZ_SCRIPT
int intersectNode ( HNODE hNodeA, HNODE hNodeB )
{
	CHECK_INTERFACE(intersectNode);
	zz_visible * visA = reinterpret_cast<zz_visible*>(hNodeA);
	zz_visible * visB = reinterpret_cast<zz_visible*>(hNodeB);

	if (!visA || !visA->is_a(ZZ_RUNTIME_TYPE(zz_visible)) ||
		!visA || !visA->is_a(ZZ_RUNTIME_TYPE(zz_visible)))
	{
		ZZ_LOG("interface: intersectNode() failed. no visible\n");
		return 0;
	}
	int result = visA->test_intersection_node(visB, ZZ_CL_OBB);
	
	return result;
}

ZZ_DLL
int intersectNodeTriSphere ( HNODE hNodeByTri_In, float vSphereCenter_In[3], float fSphereRadius_In )
{
	CHECK_INTERFACE(intersectNodeTriSphere);
	zz_visible * visTri = reinterpret_cast<zz_visible*>(hNodeByTri_In);

	if (!visTri || !visTri->is_a(ZZ_RUNTIME_TYPE(zz_visible)))
	{
		ZZ_LOG("interface: intersectNodeSphere() failed. no visible\n");
		return 0;
	}

	float radius = fSphereRadius_In * ZZ_SCALE_IN;
	vec3 center(vSphereCenter_In);
	center *= ZZ_SCALE_IN;

	zz_bounding_sphere sp(radius, center);
	int result;

	result = visTri->test_intersection_sphere(sp);

	return result;
}

ZZ_DLL
int intersectNodeTriSphereMoving ( HNODE hNodeByTri_In, float vSphereCenter_In[3], float fSphereRadius_In,  float vPrevCenter_In[3], float vClosestCenter_Out[3], float fMaxDistanceSquare_In )
{
	CHECK_INTERFACE(intersectNodeTriSphere);
	zz_visible * visTri = reinterpret_cast<zz_visible*>(hNodeByTri_In);

	if (!visTri || !visTri->is_a(ZZ_RUNTIME_TYPE(zz_visible)))
	{
		ZZ_LOG("interface: intersectNodeSphere() failed. no visible\n");
		return 0;
	}

	vec3 prev_center(vPrevCenter_In), closest_center;
	float max_distance_square = fMaxDistanceSquare_In*ZZ_SCALE_IN;
	prev_center *= ZZ_SCALE_IN;

	float radius = fSphereRadius_In * ZZ_SCALE_IN;
	vec3 center(vSphereCenter_In);
	center *= ZZ_SCALE_IN;

	zz_bounding_sphere sp(radius, center);
	int result;

	result = visTri->test_intersection_sphere_moving( sp, prev_center, &closest_center, max_distance_square );
	
	if (result) {
		if (vClosestCenter_Out) {
			vClosestCenter_Out[0] = closest_center.x*ZZ_SCALE_OUT;
			vClosestCenter_Out[1] = closest_center.y*ZZ_SCALE_OUT;
			vClosestCenter_Out[2] = closest_center.z*ZZ_SCALE_OUT;
		}
	}

	//ZZ_LOG("interface: intersectNodeTriSphereMoving(%s) = %d\n", visTri->get_mesh(0)->get_path(), result);

	return result;
}

ZZ_DLL
bool intersectRayNodeAABB ( HNODE hNode,float originX, float originY, float originZ,
	float dirX, float dirY, float dirZ)
{
	vec3 origin_in(originX, originY, originZ), origin;
	vec3 dir(dirX, dirY, dirZ);
    
	origin = ZZ_SCALE_IN * origin_in;
    
	zz_visible * vis = reinterpret_cast<zz_visible*>(hNode);

	if (!vis || !vis->is_a(ZZ_RUNTIME_TYPE(zz_visible))) {
		ZZ_LOG("interface: intersectRay() failed. no visible\n");
		return false;
	}
     
    zz_bvolume *bv = vis->get_bvolume();
	
	return ::intersect(*(bv->get_aabb()),origin,dir);
}

ZZ_DLL
bool intersectRayAABB ( float vMin[3], float vMax[3],float originX, float originY, float originZ,
	float dirX, float dirY, float dirZ)
{
	vec3 origin(originX, originY, originZ);
	vec3 dir(dirX, dirY, dirZ);
    vec3 pmin(vMin[0], vMin[1], vMin[2]);
	vec3 pmax(vMax[0], vMax[1], vMax[2]);


	return ::intersect_aabbbox(pmin, pmax, origin, dir);
    	
}




ZZ_SCRIPT
float getCameraEyeX ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraEyeX);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraEyeX() failed. no camera\n");
		return 0.0f;
	}
	return cam->get_eye().x*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getCameraEyeY ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraEyeY);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraEyeY() failed. no camera\n");
		return 0.0f;
	}
	return cam->get_eye().y*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getCameraEyeZ ( HNODE hCamera )
{
	CHECK_INTERFACE(getCameraEyeZ);
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (!cam) {
		ZZ_LOG("interface: getCameraEyeZ() failed. no camera\n");
		return 0.0f;
	}
	return cam->get_eye().z*ZZ_SCALE_OUT;
}

ZZ_DLL
void getCameraPerspectiveElement(HNODE hCamera, float *_fov, float *_ratio, float *_near, float *_far)
{
	zz_camera * cam = reinterpret_cast<zz_camera*>(hCamera);

	if (cam) {

		*_fov = cam->get_fov();
		*_ratio = cam->get_aspect_ratio();
		*_near = cam->get_near_plane();
		*_far = cam->get_far_plane();
	
	}
}


bool is_one_of ( int iOne, int iAll )
{
	if ((iOne & iAll) > 0) return true;
	return false;
}

ZZ_DLL
HNODE pickNode ( int screenX, int screenY, float * pContactPointX, float * pContactPointY, float * pContactPointZ, float * pDistance )
{
	CHECK_INTERFACE(pickNode);
	// get current camera
	zz_camera * cam = znzin->get_camera();
	zz_assert(cam);

	// get camera ray component(origin, direction)
	vec3 origin, direction;
	cam->get_ray(vec2(float(screenX), float(screenY)), origin, direction);
	
	// for each visibles in the scene
	int num_nodes = znzin->scene.get_num_viewfrustum_node();
	
	// info. for current comparison node
	float cur_distance;
	vec3 cur_contact_point;
	vec3 cur_contact_normal; // not used here
	zz_visible * cur_vis = NULL;
	
	// info. for closest node
	float min_distance;
	vec3 min_contact_point;
	zz_visible * min_vis = NULL;

	bool first = true;
	int cur_node_type;

	for (int i = 0; i < num_nodes; i++) {
		cur_vis = znzin->scene.get_viewfrustum_node(i);
		cur_node_type = getTypeOf(reinterpret_cast<HNODE>(cur_vis));	
		
		// include test
		if (!is_one_of(cur_node_type, include_classes)) continue;
		if (is_one_of(cur_node_type, exclude_classes)) continue;

		if (cur_vis->get_intersection_ray(origin, direction, cur_contact_point, cur_contact_normal)) {
			cur_distance = origin.distance_square(cur_contact_point);
			if (first || (cur_distance < min_distance)) {
				min_distance = cur_distance;
				min_contact_point = cur_contact_point;
				min_vis = cur_vis;
				first = false;
			}
		}
	}

	if (min_vis) {
		*pContactPointX = min_contact_point.x*ZZ_SCALE_OUT;
		*pContactPointY = min_contact_point.y*ZZ_SCALE_OUT;
		*pContactPointZ = min_contact_point.z*ZZ_SCALE_OUT;
		if (pDistance)
			*pDistance = sqrtf(min_distance)*ZZ_SCALE_OUT;

        return reinterpret_cast<HNODE>(min_vis);
	}
	return 0;
}

ZZ_SCRIPT
HNODE pickNodeScript ( int screenX, int screenY )
{
	CHECK_INTERFACE(pickNodeScript);
	float ret[4] = {0, 0, 0, 0};

	HNODE picked_node = pickNode(screenX, screenY, &ret[0], &ret[1], &ret[2], &ret[3]);

	if (picked_node) {
		setFloatArray(0, ret[0]);
		setFloatArray(1, ret[1]);
		setFloatArray(2, ret[2]);
		setFloatArray(3, ret[3]);
		return picked_node;
	}
	return 0;
}

// fHeight : ³oAI°ª(cm)
ZZ_SCRIPT
int setCameraTargetHeight ( HNODE hCamera, float fHeight )
{
	CHECK_INTERFACE(setCameraTargetHeight);
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(hCamera);

	fHeight *= ZZ_SCALE_IN;

	if (!cam) {
		ZZ_LOG("interface: setCameraTargetHeight() failed. no camera found\n");
		return 0;
	}

	if (!cam->is_a(ZZ_RUNTIME_TYPE(zz_camera_follow))) {
		ZZ_LOG("interface: setCameraTargetHeight() failed. not a follow_camera\n");
		return 0;
	}

	cam->set_height_added(fHeight);
	return 1;
}

ZZ_SCRIPT
float getFloatArray ( int iIndex )
{
	CHECK_INTERFACE(getFloatArray);
	zz_assert(iIndex < MAX_FLOAT_ARRAY);
	return float_array[iIndex];
}

ZZ_SCRIPT
float setFloatArray ( int iIndex, float fNewFloat )
{
	CHECK_INTERFACE(setFloatArray);
	zz_assert(iIndex < MAX_FLOAT_ARRAY);
	float saved = float_array[iIndex];
	float_array[iIndex] = fNewFloat;
	return saved;
}

ZZ_SCRIPT
int unloadTerrainBlock ( HNODE hTerrainBlock )
{
	CHECK_INTERFACE(unloadTerrainBlock);
	
	zz_terrain_block * block = reinterpret_cast<zz_terrain_block*>(hTerrainBlock);
	
	zz_assert(block);

	zz_assert(block->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block)));
	zz_assert(block->get_mesh(0));
	
	zz_mesh * mesh = block->get_mesh(0); // get first mesh
	zz_material * mat = block->get_material(0); // get first material

	zz_assert(mesh);
	zz_assert(mat);

	//ZZ_LOG("unloadTerrainBlock(%s) done\n", block->get_name());

	// unload terrain block
	int ret_vis = unloadVisible(hTerrainBlock);

	// unload terrain mesh, born in loadTerrainBlock()
	int ret_mesh = unloadMesh(reinterpret_cast<HNODE>(mesh));

	int ret_mat = unloadMaterial(reinterpret_cast<HNODE>(mat));

	if (ret_vis == 0 || ret_mesh == 0) return 0;
	return 1;
}

ZZ_DLL
int getRay (
	int screenX, int screenY,
	float * pOriginX, float * pOriginY, float * pOriginZ,
	float * pDirectionX, float * pDirectionY, float * pDirectionZ )
{
	CHECK_INTERFACE(getRay);
	zz_camera * cam = znzin->get_camera();

	//zz_assert(cam);
	if (!cam) return 0;

	vec3 origin, direction;
	
	cam->get_ray(vec2(float(screenX), float(screenY)), origin, direction);

	zz_assert(pOriginX);
	zz_assert(pOriginY);
	zz_assert(pOriginZ);

	zz_assert(pDirectionX);
	zz_assert(pDirectionY);
	zz_assert(pDirectionZ);

	origin *= ZZ_SCALE_OUT;
	*pOriginX = origin.x; *pOriginY = origin.y; *pOriginZ = origin.z;
	*pDirectionX = direction.x; *pDirectionY = direction.y; *pDirectionZ = direction.z;

	//ZZ_LOG("interface: getRay() -> Origin(%f, %f, %f)-Dir(%f, %f, %f)\n",
	//	*pOriginX, *pOriginY, *pOriginZ,
	//	*pDirectionX, *pDirectionY, *pDirectionZ);

	return 1;
}

ZZ_SCRIPT
int useMultiPass ( int bUseMultiPass )
{
	CHECK_INTERFACE(useMultiPass);
	static bool old;
	old = state->use_multipass;
	state->use_multipass = ISTRUE(bUseMultiPass);
	return (old) ? 1 : 0;
}

ZZ_SCRIPT
int useLightmap ( int bUseLightmap )
{
	CHECK_INTERFACE(useLightmap);
	static bool old;
	old = state->use_lightmap;
	state->use_lightmap = ISTRUE(bUseLightmap);
	return (old) ? 1 : 0;
}

ZZ_SCRIPT
void useTerrainLOD ( int bUseLOD )
{
	state->use_terrain_lod = ISTRUE(bUseLOD);
}

ZZ_SCRIPT
int weldMesh ( HNODE hMesh )
{
	CHECK_INTERFACE(weldMesh);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;

	return zz_mesh_tool::weld_vertices(mesh);
}

ZZ_SCRIPT
int clearMeshMatid ( HNODE hMesh )
{
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;

	mesh->set_num_matids(0);

	return 1;
}


ZZ_SCRIPT
int generateMeshStrip ( HNODE hMesh )
{
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);

	if (!mesh) return 0;
	
	return ISTRUE(mesh->generate_strip());
}


ZZ_SCRIPT
int saveMesh ( HNODE hMesh, ZSTRING pSavePath, int bTextMode )
{
	CHECK_INTERFACE(saveMesh);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;
	if (!pSavePath) return 0;
	return zz_mesh_tool::save_mesh(pSavePath, mesh, ISTRUE(bTextMode) /* text mode */);
}

ZZ_SCRIPT
float getWorldHeight ( HNODE hVisible, float fPointX, float fPointY )
{
	CHECK_INTERFACE(getWorldHeight);
	const vec3 up_dir(0, 0, 1), down_dir(0, 0, -1);
	vec3 origin(fPointX, fPointY, 0);
	vec3 contact_point;
	vec3 contact_normal;

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0.0f;

	// check both up and down
	if (vis->get_intersection_ray(origin, up_dir, contact_point, contact_normal, zz_mesh_tool::ZZ_SM_FARTHEST)) {
		return contact_point.z*ZZ_SCALE_OUT;
	}
	else if (vis->get_intersection_ray(origin, down_dir, contact_point, contact_normal, zz_mesh_tool::ZZ_SM_NEAREST)) {
		return contact_point.z*ZZ_SCALE_OUT;
	}
	return -ZZ_INFINITE;
}

ZZ_SCRIPT
float shootRay ( HNODE hVisible, int iNearest,
				float fOriginX, float fOriginY, float fOriginZ, 
				float fDirX, float fDirY, float fDirZ )
{
	CHECK_INTERFACE(shootRay);
	float vOrigin[3], vDirection[3];
	vOrigin[0] = fOriginX;
	vOrigin[1] = fOriginY;
	vOrigin[2] = fOriginZ;
	vDirection[0] = fDirX;
	vDirection[1] = fDirY;
	vDirection[2] = fDirZ;
	return shootRayVec3( hVisible, iNearest, vOrigin, vDirection );
}

ZZ_DLL
float shootRayVec3 ( HNODE hVisible, int iNearest, float vOrigin[3], float vDirection[3] )
{
	CHECK_INTERFACE(shootRay);
	vec3 dir(vDirection);
	vec3 origin(vOrigin);
	vec3 contact_point;
	vec3 contact_normal;

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0.0f;

	zz_mesh_tool::zz_select_method method = ISTRUE(iNearest) ? zz_mesh_tool::ZZ_SM_NEAREST : zz_mesh_tool::ZZ_SM_FARTHEST;
	
	origin *= ZZ_SCALE_IN;
	if (vis->get_intersection_ray(origin, dir, contact_point, contact_normal, method)) {
		return origin.distance(contact_point)*ZZ_SCALE_OUT;
	}
	return ZZ_INFINITE;
}

ZZ_DLL
float shootRayVec3Ex ( HNODE hVisible, int iNearest, float vOrigin[3], float vDirection[3], float contactPoint[3], float contactNormal[3], float meshPoint1[3], float meshPoint2[3], float meshPoint3[3])
{
	CHECK_INTERFACE(shootRay);
	vec3 dir(vDirection);
	vec3 origin(vOrigin);
	vec3& contact_point = FLOAT3_TO_VEC3(contactPoint);;
	vec3& contact_normal = FLOAT3_TO_VEC3(contactNormal);
	vec3& mesh_point1 = FLOAT3_TO_VEC3(meshPoint1);;
	vec3& mesh_point2 = FLOAT3_TO_VEC3(meshPoint2);
    vec3& mesh_point3 = FLOAT3_TO_VEC3(meshPoint3);;
	
 
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0.0f;

	zz_mesh_tool::zz_select_method method = ISTRUE(iNearest) ? zz_mesh_tool::ZZ_SM_NEAREST : zz_mesh_tool::ZZ_SM_FARTHEST;
	
	origin *= ZZ_SCALE_IN;
	if (vis->get_intersection_ray_ex(origin, dir, contact_point, contact_normal, mesh_point1, mesh_point2, mesh_point3, method)) {
		
		contact_point *= ZZ_SCALE_OUT;
        origin *= ZZ_SCALE_OUT;
		mesh_point1 *= ZZ_SCALE_OUT;
        mesh_point2 *= ZZ_SCALE_OUT;
		mesh_point3 *= ZZ_SCALE_OUT;
  
		return origin.distance(contact_point);
	}
	return ZZ_INFINITE;
}


ZZ_SCRIPT
int isDescendantOf ( HNODE hChild, HNODE hParent )
{
	CHECK_INTERFACE(isDescendantOf);
	zz_node * child = reinterpret_cast<zz_node*>(hChild);
	zz_node * parent = reinterpret_cast<zz_node*>(hParent);

	if (!child || !parent) return 0;

	if (child->is_descendant_of(parent)) return 1;
	return 0;
}

ZZ_SCRIPT
int setCollisionLevel ( HNODE hVisible, int iLevel )
{
	CHECK_INTERFACE(setCollisionLevel);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (vis) {
		vis->set_collision_level(zz_collision_level(iLevel));
		return 1;
	}
	return 0;
}

ZZ_SCRIPT
int getCollisionLevel ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	return static_cast<int>(vis->get_collision_level());
}

ZZ_DLL
bool getHeightCollisionLevelOnOff ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
    zz_collision_level level = vis->get_collision_level();
	
    return ZZ_IS_HEIGHTONOY(level);
	
}




ZZ_SCRIPT
int setMaterialLightmap ( HNODE hMaterial, ZSTRING pLightmapFilename, int iX, int iY, int iWidth, int iHeight )
{
	CHECK_INTERFACE_MATERIAL(setMaterialLightmap);
	zz_material_colormap * mat = reinterpret_cast<zz_material_colormap*>(hMaterial);
	
	if (!mat) {
		ZZ_LOG("interface: setLightmap() failed. no material found.\n");
		return 0;
	}

	int lightmap_slot = 2; // LIGHTMAP_SLOT in zz_material_colormap.cpp
	mat->set_texture(lightmap_slot, pLightmapFilename);
	mat->set_lightmap_transform(iX, iY, iWidth, iHeight);
	mat->set_receive_shadow(true);
	
	return 1;
}

ZZ_SCRIPT
int isDescendantOfType ( HNODE hChild, int iType )
{
	CHECK_INTERFACE(isDescendantOfType);
	zz_node * node = reinterpret_cast<zz_node*>(hChild);

	if (!node) return 0;

	return node->is_descendant_of_type(getNodeType(iType)) ? 1 : 0;
}

ZZ_SCRIPT
HNODE loadParticleFromMem ( ZSTRING pParticleName, ZSTRING pParticleMem, int iMemSize )
{
	CHECK_INTERFACE(loadParticleFromMem);
	zz_particle_emitter * part = (zz_particle_emitter*)znzin->visibles->spawn(pParticleName, ZZ_RUNTIME_TYPE(zz_particle_emitter));

	//ZZ_LOG("interface: loadParticleFromMem(%s, %s)\n", pParticleName, pParticleMem);

	zz_vfs_mem fs;
	if (!fs.open(pParticleMem, iMemSize)) return 0;

	if (!part->load(&fs, false /* for_now */ )) return 0;

	part->set_bvolume_type(ZZ_BV_OBB);
	part->set_collision_level(ZZ_CL_NONE);
	part->set_cast_shadow(false);

	return reinterpret_cast<HNODE>(part);
}

ZZ_SCRIPT
HNODE loadParticle ( ZSTRING pParticleName, ZSTRING pParticleFilename )
{
	CHECK_INTERFACE(loadParticle);
	zz_particle_emitter * part = (zz_particle_emitter*)znzin->visibles->spawn(pParticleName, ZZ_RUNTIME_TYPE(zz_particle_emitter));

	//ZZ_LOG("interface: loadParticle(%s, %s)\n", pParticleName, pParticleFilename);

	bool nowload = !state->use_delayed_loading;
	if (!part->load(pParticleFilename, nowload)) {
		ZZ_LOG("interface: loadParticle(%s, %s) failed.\n", pParticleName, pParticleFilename);
		return 0;
	}

	return reinterpret_cast<HNODE>(part);
}

ZZ_SCRIPT
int unloadParticle ( HNODE hParticle )
{
	//ZZ_LOG("unloadParticle...\n");
	CHECK_INTERFACE(unloadParticle);
	return unloadNode(hParticle);
}

ZZ_SCRIPT
int controlParticle ( HNODE hParticle, int iControl )
{
	CHECK_INTERFACE(controlParticle);
	return controlAnimatable( hParticle, iControl );
}

ZZ_SCRIPT
int setCameraPerspective ( HNODE hCamera, float fFovY, float fAspectRatio, float fNear, float fFar )
{
	CHECK_INTERFACE(setCameraPerspective);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);

	if (!cam) return 0;

	if (fAspectRatio == 0.0f) {
		fAspectRatio = (float)znzin->view->get_width()/(float)znzin->view->get_height();
	}
	cam->set_perspective(fFovY, fAspectRatio, fNear*ZZ_SCALE_IN, fFar*ZZ_SCALE_IN);

	return 1;
}

ZZ_SCRIPT
int setCameraOrthogonal ( HNODE hCamera, float fWidth, float fHeight, float fNear, float fFar )
{
	CHECK_INTERFACE(setCameraOrthogonal);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);

	if (!cam) return 0;

	cam->set_orthogonal(fWidth*ZZ_SCALE_IN, fHeight*ZZ_SCALE_IN, fNear*ZZ_SCALE_IN, fFar*ZZ_SCALE_IN);

	return 1;
}

ZZ_SCRIPT
void setCameraFov ( HNODE hCamera, float fFovY )
{
	CHECK_INTERFACE(setCameraFov);
	zz_camera * cam = reinterpret_cast<zz_camera *>(hCamera);

	zz_assert(cam);

	cam->set_fov(fFovY);
}

ZZ_SCRIPT
int getParticleState ( HNODE hParticle )
{
	CHECK_INTERFACE(getParticleState);
	zz_particle_emitter * part = reinterpret_cast<zz_particle_emitter*>(hParticle);
	
//	zz_assert(part);
	
	if(part)
		return (part->IsRunning()) ? 1 : 0;
	else
		return 0;
}

ZZ_DLL
int collectBySphere ( float vCenter[3], float fDistance )
{
	vec3 minmax[2];
	minmax[0].set(vCenter[0] - fDistance, vCenter[1] - fDistance, vCenter[2] - fDistance);
	minmax[1].set(vCenter[0] + fDistance, vCenter[1] + fDistance, vCenter[2] + fDistance);
	minmax[0] *= ZZ_SCALE_IN;
	minmax[1] *= ZZ_SCALE_IN;
	return znzin->scene.collect_by_minmax(0 /* pack_index */, minmax, true /* skip_no_collision */);
}

ZZ_SCRIPT
int collectByMinMax ( float fMinX, float fMinY, float fMinZ, float fMaxX, float fMaxY, float fMaxZ )
{
	CHECK_INTERFACE(collectByMinMax);
	vec3 minmax[2];
	minmax[0].set(fMinX, fMinY, fMinZ);
	minmax[1].set(fMaxX, fMaxY, fMaxZ);
	minmax[0] *= ZZ_SCALE_IN;
	minmax[1] *= ZZ_SCALE_IN;
	return znzin->scene.collect_by_minmax(0 /* pack_index */, minmax, true);
}

ZZ_DLL
int collectByMinMaxVec3 ( float fMin[3], float fMax[3] )
{
	CHECK_INTERFACE(collectByMinMaxVec3);
	vec3 minmax[2];
	minmax[0] = fMin;
	minmax[1] = fMax;
	minmax[0] *= ZZ_SCALE_IN;
	minmax[1] *= ZZ_SCALE_IN;
	return znzin->scene.collect_by_minmax(0 /* pack_index */, minmax, true);
}

ZZ_SCRIPT
int collectByNodeBBox ( HNODE hVisible )
{
	CHECK_INTERFACE(collectByNodeBBox);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	const float margin = 100.0f*ZZ_SCALE_IN;

	if (!vis) {
		ZZ_LOG("interface: collectByNodeBBox() failed. not a valid visible\n");
		return 0;
	}
	const zz_bvolume * bv = vis->get_bvolume();
	zz_assert(bv);
	const zz_bounding_aabb * aabb = bv->get_aabb();
	zz_assert(aabb);
	vec3 minmax[2];
	minmax[0].set(aabb->pmin.x - margin, aabb->pmin.y - margin, aabb->pmin.z - margin);
	minmax[1].set(aabb->pmax.x + margin, aabb->pmax.y + margin, aabb->pmax.z + margin);
	
	return znzin->scene.collect_by_minmax(0 /* pack_index */, minmax, true);
}

ZZ_SCRIPT
HNODE getCollectNode ( int iNode )
{
	CHECK_INTERFACE(getCollectNode);
	return reinterpret_cast<HNODE>(znzin->scene.get_collect_node(0 /* pack_index */, iNode));
}

ZZ_SCRIPT
int setMaterialShader ( HNODE hMaterial, HNODE hShader )
{
	CHECK_INTERFACE_MATERIAL(setMaterialShader);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);
	zz_shader * sha = reinterpret_cast<zz_shader*>(hShader);
	if (!mat || !sha) {
		ZZ_LOG("interface: setShader() failed. no material or shader\n");
		return 0;
	}
	mat->set_shader(sha);
	return 1;
}

ZZ_SCRIPT
HNODE getMaterialShader ( HNODE hMaterial )
{
	CHECK_INTERFACE(getMaterialShader);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!mat) {
		return 0;
	}
	return reinterpret_cast<HNODE>(mat->get_shader());
}

ZZ_SCRIPT
int setVisibleMaterial ( HNODE hVisible, int irunit, HNODE hMaterial )
{
	CHECK_INTERFACE(setVisibleMaterial);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!vis) return 0;

	vis->set_material(irunit, mat);
	return 1;
}

ZZ_SCRIPT
HNODE getVisibleMaterial ( HNODE hVisible, int irunit )
{
	CHECK_INTERFACE(getVisibleMaterial);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) return 0;

	zz_material * mat = vis->get_material(static_cast<unsigned int>(irunit));

	return reinterpret_cast<HNODE>(mat);
}

ZZ_SCRIPT
int getNumRenderUnit ( HNODE hVisible )
{
	CHECK_INTERFACE(getNumRenderUnit);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) return 0;

	return vis->get_num_runits();
}

ZZ_SCRIPT
HNODE getVisibleMesh ( HNODE hVisible, int irunit )
{
	CHECK_INTERFACE(getVisibleMesh);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) return 0;

	zz_mesh * mesh = vis->get_mesh(irunit);

	return reinterpret_cast<HNODE>(mesh);
}

ZZ_SCRIPT
int setVisibleMesh ( HNODE hVisible, int irunit, HNODE hMesh )
{
	CHECK_INTERFACE(setVisibleMesh);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);

	if (!vis) return 0;

	zz_assert(vis->get_material(irunit) && "setVisibleMesh() no material");

	vis->set_mesh(irunit, mesh);

	return 1;
}


ZZ_SCRIPT
HNODE getVisibleLight ( HNODE hVisible, int irunit )
{
	CHECK_INTERFACE(getVisibleLight);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) return 0;

	zz_light * lit = vis->get_light(static_cast<unsigned int>(irunit));

	return reinterpret_cast<HNODE>(lit);
}

ZZ_SCRIPT
int setVisibleLight ( HNODE hVisible, int irunit, HNODE hLight )
{
	CHECK_INTERFACE(setVisibleLight);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	zz_light * lit = reinterpret_cast<zz_light*>(hLight);

	if (!vis) return 0;

	vis->set_light(static_cast<unsigned int>(irunit), lit);
	return 1;
}

ZZ_SCRIPT
int setLightmapBlendStyle ( int iBlendStyle )
{
	CHECK_INTERFACE(setLightmapBlendStyle);
	znzin->get_rs()->lightmap_blend_style = iBlendStyle;
	return 1;
}

ZZ_SCRIPT
int setMaterialZTest ( HNODE hMaterial, int bZTest )
{
	CHECK_INTERFACE_MATERIAL(setMaterialZTest);
	static zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);
	mat->set_ztest(ISTRUE(bZTest));
	return 1;
}

ZZ_SCRIPT
int setMaterialZWrite ( HNODE hMaterial, int bZWrite )
{
	CHECK_INTERFACE_MATERIAL(setMaterialZWrite);
	static zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);
	mat->set_zwrite(ISTRUE(bZWrite));
	return 1;
}

ZZ_SCRIPT
int setMaterialObjectAlpha ( HNODE hMaterial, float fAlpha )
{
	CHECK_INTERFACE_MATERIAL(setMaterialObjectAlpha);
	static zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);

	mat->set_objectalpha(fAlpha);
	return 1;
}

ZZ_SCRIPT
int setMaterialAlphaValue ( HNODE hMaterial, float fAlpha )
{
	CHECK_INTERFACE_MATERIAL(setMaterialAlphaValue);
	zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);

	mat->set_materialalpha(fAlpha);
	return 1;
}

ZZ_SCRIPT
int setCameraTransparency ( float fTransparency )
{
	CHECK_INTERFACE(setCameraTransparency);
	znzin->get_rs()->camera_transparency = fTransparency;
    return 1;
}

ZZ_SCRIPT
int setMaterialBlendType ( HNODE hMaterial, int iBlendType )
{
	CHECK_INTERFACE_MATERIAL(setMaterialBlendType);
	static zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);

	mat->set_blendtype(iBlendType);
    return 1;
}

ZZ_SCRIPT
int setMaterialBlendTypeCustom ( HNODE hMaterial, int iBlendSrc, int iBlendDest, int iBlendOp )
{
	CHECK_INTERFACE_MATERIAL(setMaterialBlendTypeCustom);
	static zz_material * mat;
	mat = (zz_material*)reinterpret_cast<zz_material*>(hMaterial);
	zz_assert(mat);

	mat->set_blendtype(iBlendSrc, iBlendDest, iBlendOp);
	return 1;
}   

ZZ_SCRIPT
int setAnimatableSpeed ( HNODE hAnimatable, float fSpeed )
{
	CHECK_INTERFACE(setAnimatableSpeed);
	static zz_animatable * ani;
	ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	zz_assert(ani);
	ani->set_motion_speed(fSpeed);
	//ZZ_LOG("interface: setAnimatableSpeed(%s, %f)\n", ani->get_name(), fSpeed);

	return 1;
}

ZZ_SCRIPT
float getAnimatableSpeed ( HNODE hAnimatable )
{
	CHECK_INTERFACE(getAnimatableSpeed);
	static zz_animatable * ani;
	ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) {
		ZZ_LOG("interface: getMotionSpeed() failed. not valid animatable\n");
		return -1.0f;
	}
	return ani->get_motion_speed();
}

ZZ_SCRIPT
HNODE loadTrailScript ( ZSTRING pName, 
				 float fDistancePerPoint,
				 int iDurationInMilliSecond,
				 int bUseCurve,
				 ZSTRING pTexturePath, 
				 float fColorRed, float fColorGreen, float fColorBlue, float fColorAlpha,
				 float fSP_X, float fSP_Y, float fSP_Z,
				 float fEP_X, float fEP_Y, float fEP_Z )
{
	ZZ_COLOR Color = ZZ_COLOR32_ARGB(int(255.0f*fColorAlpha), int(255.0f*fColorRed), int(255.0f*fColorGreen), int(255.0f*fColorBlue));
	ZZ_VECTOR vStart, vEnd;
	vStart.x = fSP_X;
	vStart.y = fSP_Y;
	vStart.z = fSP_Z;
	vEnd.x = fEP_X;
	vEnd.y = fEP_Y;
	vEnd.z = fEP_Z;

	return loadTrail(pName, fDistancePerPoint, iDurationInMilliSecond, bUseCurve, pTexturePath, Color, 
		vStart, vEnd );
}

ZZ_DLL
HNODE loadTrail ( ZSTRING pName, 
				 float fDistancePerPoint,
				 int iDurationInMilliSecond,
				 int bUseCurve,
				 ZSTRING pTexturePath, 
				 ZZ_COLOR Color,
				 ZZ_VECTOR vStart,
				 ZZ_VECTOR vEnd )
{
	CHECK_INTERFACE(loadTrail);
	zz_trail * trail = static_cast<zz_trail*>(znzin->visibles->spawn(pName, ZZ_RUNTIME_TYPE(zz_trail)));

	if (!trail) return 0;
	
	bool ret;
	
	zz_color color(Color);

	ret = trail->load(
		vec4(color.r, color.g, color.b, color.a),
		fDistancePerPoint*ZZ_SCALE_IN,
		iDurationInMilliSecond,
		ISTRUE(bUseCurve),
		pTexturePath,
        vec3(vStart.x*ZZ_SCALE_IN, vStart.y*ZZ_SCALE_IN, vStart.z*ZZ_SCALE_IN),
		vec3(vEnd.x*ZZ_SCALE_IN, vEnd.y*ZZ_SCALE_IN, vEnd.z*ZZ_SCALE_IN));

	if (ret == NULL) return 0;

	return reinterpret_cast<HNODE>(trail);
}

ZZ_SCRIPT
int controlTrail ( HNODE hTrail, int iControl )
{
	CHECK_INTERFACE(controlTrail);
	zz_trail * trail = reinterpret_cast<zz_trail*>(hTrail);
	
	if (!trail) return 0;

	trail->set_control_state(static_cast<zz_trail::zz_control_state>(iControl));
	
	return 1;
}

ZZ_SCRIPT
int getTrailState ( HNODE hTrail )
{
	CHECK_INTERFACE(getTrailState);
	zz_trail * trail = reinterpret_cast<zz_trail*>(hTrail);

	if (!trail) return 0; // CAUTION: not right return value

	return static_cast<int>(trail->get_control_state());
}

ZZ_SCRIPT
int getAnimatableState ( HNODE hAnimatable )
{
	CHECK_INTERFACE(getAnimatableState);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	zz_assert(ani->is_a(ZZ_RUNTIME_TYPE(zz_animatable)));
	if (!ani) return ZZ_STOPPED;

	return ani->get_motion_state();
}

ZZ_SCRIPT
int controlAnimatable ( HNODE hAnimatable, int iControl )
{
	CHECK_INTERFACE(controlAnimatable);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;
	zz_assert(ani->is_a(ZZ_RUNTIME_TYPE(zz_animatable)));

	//ZZ_LOG("controlAnimatable(%s:#%d, %d)\n", ani->get_name(), ani->get_node_type()->type_name, iControl);
	
	switch (iControl) {
		case 0 : // stop
            ani->stop();
			break;
		case 1 : // Start
			ani->play();
			break;
		case 2 : // Pause
			ani->pause();
			break;
	}
	return 1;
}

ZZ_SCRIPT
int setShadowmapBlurType ( int iBlurType )
{
	CHECK_INTERFACE(setShadowmapBlurType);
	int old = znzin->get_rs()->shadowmap_blur_type;
	znzin->get_rs()->shadowmap_blur_type = iBlurType;
	return old;
}

ZZ_SCRIPT
void setDataPath ( ZSTRING pDataPath )
{
	CHECK_INTERFACE(setDataPath);
	return; // obsolete interface
}

ZZ_SCRIPT
int getDrawPriority ( HNODE hVisible )
{
	CHECK_INTERFACE(getDrawPriority);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return -1;
	
	return vis->get_draw_priority();
}

ZZ_SCRIPT
int setDrawPriority ( HNODE hVisible, int iPriority )
{
	CHECK_INTERFACE(setDrawPriority);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	if (iPriority < 0 || iPriority > 10) return 0;

	vis->set_draw_priority(iPriority);
	return 1;
}

ZZ_SCRIPT
int getNumDummies ( HNODE hModel )
{
	CHECK_INTERFACE(getNumDummies);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	return model->get_num_dummies();
}

ZZ_SCRIPT
int getNumBones ( HNODE hModel )
{
	CHECK_INTERFACE(getNumBones);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	return model->get_num_bones();
}

ZZ_SCRIPT
int getStartDelay ( HNODE hAnimatable )
{
	CHECK_INTERFACE(getStartDelay);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	return ani->get_start_delay();
}

ZZ_SCRIPT
int setStartDelay ( HNODE hAnimatable, int iDelayMilliSecond )
{
	CHECK_INTERFACE(setStartDelay);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	ani->set_start_delay(ZZ_MSEC_TO_TIME(iDelayMilliSecond));
	
	return 1;
}

ZZ_SCRIPT
int setRepeatCount ( HNODE hAnimatable, int iRepeatCount )
{
	CHECK_INTERFACE(setRepeatCount);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	zz_assert(ani->is_a(ZZ_RUNTIME_TYPE(zz_animatable)) && "setRepeatCount()");
	ani->set_repeat_count(iRepeatCount);

	return 1;
}

ZZ_SCRIPT
int getRepeatCount ( HNODE hAnimatable )
{
	CHECK_INTERFACE(getRepeatCount);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	return ani->get_repeat_count();
}

ZZ_SCRIPT
HNODE getBone ( HNODE hModel, int iBoneIndex )
{
	CHECK_INTERFACE(getBone);
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;
	int num_bones = model->get_num_bones();
	zz_assert(iBoneIndex < num_bones && "getBone");
	return reinterpret_cast<HNODE>(model->get_bone(iBoneIndex));
}

ZZ_DLL
void inputForcedBoneRotation( HNODE hBone, float vPosition[3])
{
	zz_bone *bone = reinterpret_cast<zz_bone*>(hBone);
    vec4 position;
    
	position.x = vPosition[0] * ZZ_SCALE_IN; position.y = vPosition[1] * ZZ_SCALE_IN; position.z = vPosition[2] * ZZ_SCALE_IN; position.w = 1.0f;

	bone->input_forced_rotation_mode(position);
}

// sync with linkNode
ZZ_SCRIPT
int linkVisibleWorld ( HNODE hParent, HNODE hChild )
{
	CHECK_INTERFACE(linkVisibleWorld);
	zz_visible * child = reinterpret_cast<zz_visible *>(hChild);
	zz_visible * parent = reinterpret_cast<zz_visible *>(hParent);

	zz_assert(child && child->is_a(ZZ_RUNTIME_TYPE(zz_visible)));
	if (!child || !child->is_a(ZZ_RUNTIME_TYPE(zz_visible))) return 0;

	child->send_to_local(parent);

	if (parent) {
		parent->link_child(child);;
	}
	else {
		zz_node * root = child->get_root();
		root->link_child(child); // make top-level
	}
	    
	return 1;
}


// sync with unlinkNode
ZZ_SCRIPT 
int unlinkVisibleWorld ( HNODE hNode )
{
	CHECK_INTERFACE(unlinkVisibleWorld);
	zz_visible * child = reinterpret_cast<zz_visible *>(hNode);
	
	zz_assert(child && child->is_a(ZZ_RUNTIME_TYPE(zz_visible)));

	zz_node * parent = child->get_parent();
	
	zz_assert(child && parent);
	if (!child || !parent) return 0;

	// keypoint!
	child->send_to_world();

	parent->unlink_child(child);
	parent = child->get_root();
	zz_assert(parent);
	parent->link_child(child); // relink to manager

	return 1;
}

ZZ_SCRIPT
int setAnimatableFrame ( HNODE hAnimatable, int iFrame )
{
	CHECK_INTERFACE(setAnimatableFrame);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	return ani->set_motion_frame(iFrame) ? 1 : 0;
}


ZZ_SCRIPT
int setAnimatableTime ( HNODE hAnimatable, int iTimeMilliSecond )
{
	CHECK_INTERFACE(setAnimatableTime);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	return ani->set_motion_time(ZZ_MSEC_TO_TIME(iTimeMilliSecond)) ? 1 : 0;
}

ZZ_SCRIPT
int applyMotion ( HNODE hAnimatable )
{
	CHECK_INTERFACE(applyMotion);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	ani->apply_motion();

	return 1;
}

ZZ_SCRIPT
HNODE getMotion ( HNODE hAnimatable )
{
	CHECK_INTERFACE(getMotion);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);

	if (!ani) return 0;

	zz_motion * mot = ani->get_motion();

	return reinterpret_cast<HNODE>(mot);
}

ZZ_SCRIPT
HNODE loadAnimatable (ZSTRING pAnimatableName,
				  HNODE hMesh,
				  HNODE hMotion,
				  HNODE hMaterial,
				  HNODE hLight)
{
	CHECK_INTERFACE(loadAnimatable);
	zz_animatable * ani = (zz_animatable *)(znzin->visibles->find(pAnimatableName));
	
	if (ani) {
		ZZ_LOG("interface: loadAnimatable(%s) already exists\n", pAnimatableName);
	}
	else {
		ani = (zz_animatable *)znzin->visibles->spawn(pAnimatableName, ZZ_RUNTIME_TYPE(zz_animatable));
	}

	zz_mesh * mesh = reinterpret_cast<zz_mesh *>(hMesh);
	zz_material * mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);
	zz_motion * motion = reinterpret_cast<zz_motion *>(hMotion);
	
	zz_assert(ani); // light and mesh can be NULL
	if (!ani) return 0;

	ani->set_bvolume_type(ZZ_BV_OBB);

	if (mesh) {
		ani->add_runit(mesh, mat, light);
	}
	if (motion) {
		ani->attach_motion(motion);
	}
	ani->set_collision_level(ZZ_CL_NONE);

	//ZZ_LOG("interface: loadAnimatable(%s, %s, %s, %s, %s) done.\n",
	//	pAnimatableName, mesh->get_name(), motion ? motion->get_name() : NULL, mat->get_name(), light->get_name());

	return reinterpret_cast<HNODE>(ani);
}

ZZ_SCRIPT
int popRenderUnit ( HNODE hVisible )
{
	CHECK_INTERFACE(poprunit);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->pop_runit();

	return 1;
}

ZZ_SCRIPT
int setIncludeClasses ( int iIncludeClasses )
{
	CHECK_INTERFACE(setIncludeClasses);
	int old = include_classes;
	include_classes = iIncludeClasses;
	return old;
}

ZZ_SCRIPT
int getIncludeClasses ( void )
{
	CHECK_INTERFACE(getIncludeClasses);
	return include_classes;
}

ZZ_SCRIPT
int setExcludeClasses ( int iExcludeClasses )
{
	CHECK_INTERFACE(setExcludeClasses);
	int old = exclude_classes;
	exclude_classes = iExcludeClasses;
	return old;
}

ZZ_SCRIPT
int getExcludeClasses ( void )
{
	CHECK_INTERFACE(getExcludeClasses);
	return exclude_classes;
}

ZZ_SCRIPT
void worldToScreenScript ( float fWorldX, float fWorldY, float fWorldZ )
{
	CHECK_INTERFACE(worldToScreenScript);

	worldToScreen(fWorldX, fWorldY, fWorldZ, &float_array[0], &float_array[1], &float_array[2]);
}

ZZ_DLL
void worldToScreen ( float fWorldX, float fWorldY, float fWorldZ, float * fScreenX, float * fScreenY, float * fScreenZ)
{
	CHECK_INTERFACE(worldToScreen);
	zz_camera * cam = znzin->get_camera();
	vec3 world_pos(fWorldX, fWorldY, fWorldZ);
	vec3 screen_pos;
	world_pos *= ZZ_SCALE_IN;
	cam->world2screen(world_pos, screen_pos);

	// not floor() and using this values as sprite transform coordinates can cause blurred text.
	*fScreenX = floor(screen_pos.x);
	*fScreenY = floor(screen_pos.y);
	*fScreenZ = screen_pos.z;
}

// 모델의 높이는 자식노드들까지 포함해서 다시 계산한다.
ZZ_SCRIPT
float getModelHeight ( HNODE hModel )
{
	CHECK_INTERFACE(getModelHeight);
	zz_assert(hModel);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	zz_assert(model->is_a(ZZ_RUNTIME_TYPE(zz_model)));

	return model->get_height()*ZZ_SCALE_OUT;
}

/// 바다 객체 생성
ZZ_SCRIPT
HNODE loadOceanBlock (
	ZSTRING pOceanBlockName,
	float fOrigX, float fOrigY, float fOrigZ,
	float fWidth, float fHeight,
	int iRepeatU, int iRepeatV,
	HNODE hMaterial,
	HNODE hLight
)
{
	CHECK_INTERFACE(loadOceanBlock);
	
	zz_mesh_ocean * mesh = 
		(zz_mesh_ocean*)znzin->ocean_meshes->spawn(0, ZZ_RUNTIME_TYPE(zz_mesh_ocean),
		false /* do_load */);
	
	if (!mesh) return 0;

	fWidth *= ZZ_SCALE_IN;
	fHeight *= ZZ_SCALE_IN;
	fOrigX *= ZZ_SCALE_IN;
	fOrigY *= ZZ_SCALE_IN;
	fOrigZ *= ZZ_SCALE_IN;

	mesh->set_property(fWidth, fHeight, iRepeatU, iRepeatV);

	znzin->ocean_meshes->load(mesh);
	
	zz_ocean_block * ocean_block = (zz_ocean_block *)znzin->ocean_blocks->spawn(pOceanBlockName,
		ZZ_RUNTIME_TYPE(zz_ocean_block));
 
	zz_material * mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_light * light = reinterpret_cast<zz_light *>(hLight);

	zz_assert(ocean_block && mesh && mat); // light can be NULL
	if (!ocean_block || !mesh || !mat) return 0;

	ocean_block->set_position(vec3(fOrigX, fOrigY, fOrigZ));
	ocean_block->add_runit(mesh, mat, light);
	
	return reinterpret_cast<HNODE>(ocean_block);
}

ZZ_SCRIPT
HNODE loadOceanMaterial (
	ZSTRING pMatName,
	HNODE hShader,
	ZSTRING pFirstTexture
)
{
	CHECK_INTERFACE(loadOceanMaterial);

	if (!pFirstTexture) {
		return 0;
	}

	zz_material_ocean * ocean_map = (zz_material_ocean *)znzin->materials->find(pMatName);
	
/*	if (ocean_map) {
		ZZ_LOG("interface: loadOceanMaterial() failed. %s already exists\n", pMatName);
		return 0;
	} */
	
	if(ocean_map == NULL)
		ocean_map = (zz_material_ocean *)znzin->materials->spawn(pMatName, ZZ_RUNTIME_TYPE(zz_material_ocean));

	// copy materia texture index to ocean block
	ocean_map->set_current_texture_index(ocean_map->get_current_texture_index());

	zz_shader * shader = reinterpret_cast<zz_shader *>(hShader);
	
	if (!ocean_map) {
		ZZ_LOG("interface: loadOceanMaterial() failed. no map");
		return 0;
	}
	
	if (shader) ocean_map->set_shader(shader);
	ocean_map->set_texture(0, pFirstTexture);
	ocean_map->set_zwrite(false);
	ocean_map->set_alpha_test(false);
	ocean_map->set_texturealpha(true);

//	zz_texture * tex = ocean_map->get_texture(0);
//	tex->lock_texture();

#ifdef ZZ_MATERIAL_LOADTEST
	ocean_map->loadtest_increase();
#endif

	return reinterpret_cast<HNODE>(ocean_map);
}

ZZ_DLL
void resetOceanMaterial(HNODE hMaterial)
{
	zz_material_ocean * ocean_map = reinterpret_cast<zz_material_ocean *>(hMaterial);
    assert(ocean_map);
	ocean_map->reset_texture();
	ocean_map->set_current_texture_index(0);

}


ZZ_SCRIPT
int unloadOceanBlock ( HNODE hOceanBlock )
{
	CHECK_INTERFACE(unloadOceanBlock);
	
	zz_visible * block = reinterpret_cast<zz_visible *>(hOceanBlock);
	zz_assert(block);
	if (!block || !block->is_a(ZZ_RUNTIME_TYPE(zz_visible))) return 0;
	
	zz_assert(block->get_mesh(0));
	zz_mesh * mesh = block->get_mesh(0); // get first mesh
	zz_material * mat = block->get_material(0); // get first material

	// unload ocean block
	int ret_vis = unloadVisible(hOceanBlock);

	// unload terrain mesh, born in loadTerrainBlock()
	int ret_mesh = unloadMesh(reinterpret_cast<HNODE>(mesh));
//	int ret_mat = unloadMaterial(reinterpret_cast<HNODE>(mat));

	if (ret_vis == 0 || ret_mesh == 0) return 0;
	return 1;
}

ZZ_SCRIPT
int pushTexture ( HNODE hMaterial, ZSTRING pTexturePath )
{
	CHECK_INTERFACE(pushTexture);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!mat) return 0;
	if (!pTexturePath) return 0;

	mat->push_texture(pTexturePath);
    
	return 1;
}

ZZ_SCRIPT
int setOceanBlockInterval ( HNODE hOceanBlock, int iIntervalMilliSecond )
{
	CHECK_INTERFACE(setOceanBlockInterval);
	zz_ocean_block * block = reinterpret_cast<zz_ocean_block*>(hOceanBlock);

	if (!block) return 0;

	block->set_interval(ZZ_MSEC_TO_TIME(iIntervalMilliSecond));

	return 1;
}

ZZ_SCRIPT
int setMeshNumClipFaces ( HNODE hMesh, int iNumClipFaces )
{
	CHECK_INTERFACE(setMeshNumClipFaces);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;

	if (iNumClipFaces < 0 || iNumClipFaces > mesh->get_num_faces()) return 0;

	mesh->set_num_clip_faces(iNumClipFaces);
	return 1;
}

ZZ_SCRIPT
int getMeshNumClipFaces ( HNODE hMesh )
{
	CHECK_INTERFACE(getMeshNumClipFaces);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;

	return mesh->get_num_clip_faces();
}

ZZ_SCRIPT
int setVisibleClipFace ( HNODE hVisible, int iClipFace )
{
	CHECK_INTERFACE(setVisibleClipFace);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->set_clip_face((zz_clip_face)iClipFace);
	return 1;
}

ZZ_SCRIPT
int getVisibleClipFace ( HNODE hVisible )
{
	CHECK_INTERFACE(getVisibleClipFace);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	return (int)vis->get_clip_face();
}

ZZ_SCRIPT
int saveToDDS ( ZSTRING pFileNameDDS )
{
	CHECK_INTERFACE(saveToDDS);
	return (znzin->renderer->save_to_file(pFileNameDDS, 0)) ? 1 : 0;
}

ZZ_SCRIPT
int saveToBMP ( ZSTRING pFileNameBMP )
{
	CHECK_INTERFACE(saveToBMP);
	znzin->scene.trace_tree();
	return (znzin->renderer->save_to_file(pFileNameBMP, 1)) ? 1 : 0;
}

ZZ_SCRIPT
int setUserData ( HNODE hVisible, HNODE hData )
{
	CHECK_INTERFACE(setUserData);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->set_userdata(reinterpret_cast<void*>(hData));

	return 1;
}

ZZ_SCRIPT
HNODE getUserData ( HNODE hVisible )
{
	CHECK_INTERFACE(getUserData);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	void * data = const_cast<void*>(vis->get_userdata());

	return reinterpret_cast<HNODE>(data);
}

ZZ_SCRIPT
float getTerrainHeight ( float fPositionX, float fPositionY )
{
	CHECK_INTERFACE(getTerrainHeight);

    float max_height = -ZZ_INFINITE;
    float height;
    bool no_object = true;
    HNODE node;
    
	int num_nodes = getNumSceneNode();

	for(int i = 0; i < num_nodes; i++) {
		node = getSceneNode(i);

		if (isA(node, ZZ_TYPE_TERRAIN) != 0) {
			height = getWorldHeight(node, fPositionX, fPositionY);
			if (height > max_height) {
				max_height = height;
			}
			no_object = false;
		}
	}
	if (no_object) return -ZZ_INFINITE;

	return height*ZZ_SCALE_OUT;
}

/// CAUTION: for internal use only!
float findAndGetPointHeight ( int num_nodes, const vec3& pos )
{
    float min_distance = 999999999.f;
    float distance;
    bool no_object = true;
    float knee_y = 130.0f; // knee position. consider objects placed under the knee position
	HNODE node;
    
	for(int i = 0; i < num_nodes; i++) {
		node = getCollectNode(i);

		if (isA(node, ZZ_TYPE_TERRAIN) != 0) {
			distance = shootRay( node, 1, // iNearest
				pos.x, pos.y, pos.z + knee_y, // origin
				0, 0, -1 // direction
				);
			if (distance < min_distance) {
				min_distance = distance;
			}
			no_object = false;
		}
	}
	return pos.z + knee_y - min_distance;
}

ZZ_SCRIPT
float getSlidingScript ( HNODE hVisible, float fMin, float fMax, float fNeighborDistance, float fVelocity )
{
	CHECK_INTERFACE(getSlidingScript);

	return getSliding(hVisible, fMin, fMax, fNeighborDistance, fVelocity, float_array);
}


ZZ_DLL
float getSliding ( HNODE hVisible, float fMin, float fMax, float fNeighborDistance, float fVelocity, float vSliding[3] )
{
	CHECK_INTERFACE(getSliding);

	int num_nodes = collectByNodeBBox( hVisible );
	if (num_nodes == 0) {
		return 0.f;
	}

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	vec3 position = vis->get_position();
	vec3 front(position.x, position.y + fNeighborDistance, position.z);
	vec3 right(position.x + fNeighborDistance, position.y, position.z);

	position.z = findAndGetPointHeight(num_nodes, position); // same effect as getTerrainHeight()
	front.z = findAndGetPointHeight(num_nodes, front);
	right.z = findAndGetPointHeight(num_nodes, right);
	
	vec3 sliding;
	float magnitude = get_ground_sliding_vector(sliding, position, front, right);

	if (magnitude >= fMax) {
		magnitude = 1.0f;
	}
	else if (magnitude <= fMin) {
		magnitude = 0.0f;
	}
	else {
		magnitude -= fMin;
		magnitude /= (fMax - fMin);
	}

	sliding *= ZZ_TIME_TO_SEC((float)getTimeDiff())*fVelocity*magnitude;
	
	vSliding[0] = sliding[0];
	vSliding[1] = sliding[1];
	vSliding[2] = sliding[2];

	return magnitude;
}

ZZ_DLL
int getDummyPosition ( HNODE hModel, int iDummyIndex, float fPosXYZ[3] )
{
	CHECK_INTERFACE(getDummyPosition);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;
	vec3 world_pos;
	model->get_dummy_position_world(world_pos, iDummyIndex);
	fPosXYZ[0] = world_pos.x*ZZ_SCALE_OUT;
	fPosXYZ[1] = world_pos.y*ZZ_SCALE_OUT;
	fPosXYZ[2] = world_pos.z*ZZ_SCALE_OUT;
	return 1;
}

ZZ_SCRIPT
int getDummyPositionScript ( HNODE hModel, int iDummyIndex )
{
	CHECK_INTERFACE(getDummyPositionScript);
	return getDummyPosition( hModel, iDummyIndex, float_array);
}

ZZ_SCRIPT
float setBlendFactor ( float fFactor )
{
	CHECK_INTERFACE(setBlendFactor);
	float old = znzin->get_rs()->blend_factor;
	znzin->get_rs()->blend_factor = fFactor;
	return old;
}

ZZ_SCRIPT
int getMeshInfo ( HNODE hMesh, int iInfoType )
{
	CHECK_INTERFACE(getMeshInfo);
	int ret = 0;
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	
	if (!mesh) return 0;

	const zz_vertex_format& format = mesh->get_vertex_format();

	switch (iInfoType) {
		case 1: // num face
			ret = mesh->get_num_faces();
			break;
		case 2: // num vertex
			ret = mesh->get_num_verts();
			break;
		case 3: // vertex format
			ret = format.get_format();
			break;
		case 4: // num matid
			ret = mesh->get_num_matids();
			break;
		case 5: // num mapchannel
			ret = format.get_num_mapchannel();
			break;
		case 6: // num bone indices
			ret = mesh->get_num_bone_indices();
			break;
	}
	return ret;
}

ZZ_SCRIPT
int buildNormal ( HNODE hMesh )
{
	CHECK_INTERFACE(buildNormal);
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);
	if (!mesh) return 0;

	zz_mesh_tool::build_normal(mesh);

	return 1;
}

// for loadEffect
void read_string_from_effect (zz_vfs& fs, char * str)
{
	int32 length;
	if (fs.read_int32(length)) 
		fs.read_string(str, length);
	str[length] = '\0';
}


ZZ_SCRIPT
int unloadNodeRecursive ( HNODE hNode )
{
	zz_node * node = reinterpret_cast<zz_node*>(hNode);

	if (!node) return 0;

	int num_children = node->get_num_children();

	zz_node * child;
	for (int i = 0; i < num_children; ++i) {
		child = node->get_child_by_index(i);
		unloadNodeRecursive( reinterpret_cast<HNODE>(child) );
	}
	if (IS_A(node, zz_visible)) {
		removeFromScene( hNode );
	}
	unloadNode( hNode );
	return 1;
}

ZZ_SCRIPT
int unloadEffect ( HNODE hNode )
{
	zz_node * node = reinterpret_cast<zz_node*>(hNode);

	if (!node) return 0;

	int num_children = node->get_num_children();

	zz_node * child;
	for (int i = 0; i < num_children; ++i) {
		child = node->get_child_by_index(i);
		unloadEffect( reinterpret_cast<HNODE>(child) );
	}
	if (IS_A(node, zz_animatable)) {
		zz_animatable * ani = static_cast<zz_animatable*>(node);
		zz_mesh * mesh = ani->get_mesh(0);
		zz_material * mat = ani->get_material(0);
		zz_motion * motion = ani->get_motion();
		if (mesh) {
			unloadMesh( reinterpret_cast<HNODE>(mesh) );
		}
		if (mat) {
			unloadMaterial( reinterpret_cast<HNODE>(mat) );
		}
		if (motion) {
			unloadMotion( reinterpret_cast<HNODE>(motion) );
		}
	}
	if (IS_A(node, zz_visible)) {
		removeFromScene( hNode );
	}
	unloadNode( hNode );
	return 1;
}

ZZ_SCRIPT
HNODE loadEffect ( ZSTRING pEffectName, ZSTRING pEffectFilePath )
{
	CHECK_INTERFACE(loadEffect);
	
	if (!pEffectFilePath) return 0;

	HNODE effect = findNode(pEffectName);

	if (effect) { // use old
		return effect;
	}

	int i;
	zz_vfs fs;
	if (!fs.open(pEffectFilePath)) return 0; // open error

	char effect_name[256];
	int32 length;
	read_string_from_effect(fs, effect_name);
	
	// sound stuff
	char sound_name[256];
	fs.read_int32(length); // sound file use or not
	read_string_from_effect(fs, sound_name); // sound file name
	fs.read_int32(length); // loop count

	// particle list
	int32 num_particles;
	fs.read_int32(num_particles); // number of particles

	// create effect object
	effect = loadVisible(pEffectName, NULL, NULL, NULL);

	// particle list loop
	char particle_effect_name[256];
	char particle_effect_name_unique[256];
	char particle_path[256];
	char particle_motion_name[256];
	int32 use_motion;
	vec3 position;
	quat rotation;
	int32 start_delay;
	int32 is_link;
	float yaw, pitch, roll, notused;
	quat q;
		
	for (i = 0; i < num_particles; i++) {
		// particle effect name
		read_string_from_effect(fs, particle_effect_name);

		// paritcle unique name in STB table
		read_string_from_effect(fs, particle_effect_name_unique);

		// paritle index in STB table
		fs.read_int32(length);
		
        // 게임용 실제 경로
		read_string_from_effect(fs, particle_path);
		
		// 애니매이션 파일 사용할꺼냐 말꺼냐..
		fs.read_int32(use_motion);

        // Particle Animation Name
		read_string_from_effect(fs, particle_motion_name);
		
		// ani loop count 
		fs.read_int32(length);

        // Particle Index in STB Table..
		fs.read_int32(length);
		
        // position
		fs.read_float(position.x);
		fs.read_float(position.y);
		fs.read_float(position.z);
	
		// rotation(pitch, yaw, roll in D3D coordinates)
		// For now, this is (pitch, yaw, roll, notused) in radians.
		// yaw,pitch,roll are in D3D coordinates.
		fs.read_float(pitch);
		fs.read_float(yaw);
		fs.read_float(roll);
		fs.read_float(notused);
		euler_2_quat_d3d(q, ZZ_TO_RAD*yaw, ZZ_TO_RAD*pitch, ZZ_TO_RAD*roll);
		rotation.x = q.x;
		rotation.y = q.z; // d3d coordinates(right-hand) vs engine coordinates(left-hand)
		rotation.z = q.y;
		rotation.w = -q.w; // d3d rotation vs engine rotation

		//Delay
		fs.read_int32(start_delay);

		// is link
		fs.read_int32(is_link); // whether this object align to world or to root object

		//ZZ_LOG("particle_path = %s\n", particle_path);
		//ZZ_LOG("particle_anim = %s\n", particle_motion_name);
		//ZZ_LOG("particle_pos = %f, %f, %f\n", position.x, position.y, position.z);
		//ZZ_LOG("particle_rot = %f, %f, %f, %f\n", rotation.x, rotation.y, rotation.z, rotation.w);
		//ZZ_LOG("particle_delay = %d\n", start_delay);
		//ZZ_LOG("particle_islink = %d\n", is_link);

		// load particle
		HNODE part_node;
		HNODE part = loadParticle( NULL, particle_path );
		if (part) {
			controlParticle( part, 1 );	// start
			if (use_motion && strcmp(particle_motion_name, "NULL")) {
				HNODE mot = loadMotion(NULL, particle_motion_name, 1, ZZ_INTERP_LINEAR, ZZ_INTERP_LINEAR, 1.0f, 0);
				zz_assert(mot);
				part_node = loadAnimatable(NULL, NULL, mot, NULL, NULL);
			}
			else {
				part_node = loadVisible(NULL, NULL, NULL, NULL);
			}
			setPosition(part_node, position.x, position.y, position.z);
			setQuaternion(part_node, rotation.w, rotation.x, rotation.y, rotation.z);
			linkNode( part_node, part );
			if (is_link) {
				linkNode( effect, part_node );
			}
		}
	}

	int32 num_morph; // mesh animation count
	char morph_name_unique[256]; // mesh animation unique name
	char morph_mesh_path[256]; // mesh path 
	char morph_motion_path[256]; // mesh animation path
	char morph_texture_path[256]; // texture path
	char morph_animation_path[256]; // mesh object animation path
	int32 use_alpha, use_twoside, use_alphatest, use_ztest, use_zwrite;
	int32 use_animation; // object animation (not morph animation)
	int32 loop_count;
	int32 src_blend, dest_blend, blend_op;
	fs.read_int32(num_morph);
	HNODE shader = findNode("shader_nolit");
	HNODE light = findNode("light_01");
	for (i = 0; i < num_morph; i++) {
		// Particle effect name
		read_string_from_effect(fs, particle_effect_name);

        // Mesh ani Unique Name in STB table..
		read_string_from_effect(fs, morph_name_unique);

		// Particle Index in STB Table..
		fs.read_int32(length);

		// mesh path
		read_string_from_effect(fs, morph_mesh_path);
		read_string_from_effect(fs, morph_motion_path);
		read_string_from_effect(fs, morph_texture_path);

		// material stuff
		fs.read_int32(use_alpha);
		fs.read_int32(use_twoside);
		fs.read_int32(use_alphatest);
		fs.read_int32(use_ztest);
		fs.read_int32(use_zwrite);

		// blend op
		fs.read_int32(src_blend);
		fs.read_int32(dest_blend);
		fs.read_int32(blend_op);

        // 6/2일 추가.. 애니매이션 파일 사용할꺼냐 말꺼냐..
		fs.read_int32(use_animation);
	
        // Meshani Animation Name
		read_string_from_effect(fs, morph_animation_path);

		// aniloop cnt
		fs.read_int32(length);

        // Meshani Index in STB Table..
		fs.read_int32(length);
		
        // position
		fs.read_float(position.x);
		fs.read_float(position.y);
		fs.read_float(position.z);
	
		// rotation(pitch, yaw, roll in D3D coordinates)
		// For now, this is (pitch, yaw, roll, notused) in radians
		fs.read_float(pitch);
		fs.read_float(yaw);
		fs.read_float(roll);
		fs.read_float(notused);
		euler_2_quat_d3d(q, ZZ_TO_RAD*yaw, ZZ_TO_RAD*pitch, ZZ_TO_RAD*roll);
		rotation.x = q.x;
		rotation.y = q.z; // d3d coordinates(right-hand) vs engine coordinates(left-hand)
		rotation.z = q.y;
		rotation.w = -q.w; // d3d rotation vs engine rotation

		// Delay
		fs.read_int32(start_delay);
	
		// Loop Count
		fs.read_int32(loop_count);

		// is link
		fs.read_int32(is_link);

		//ZZ_LOG("morph[particle_effect_name] = %s\n", particle_effect_name);
		//ZZ_LOG("morph[mesh] = %s\n", morph_mesh_path);
		//ZZ_LOG("morph[tex] = %s\n", morph_texture_path);
		//ZZ_LOG("morph[zmo] = %s\n", morph_motion_path);
		//ZZ_LOG("morph[is_link] = %d\n", is_link);
		//ZZ_LOG("morph[position] = (%f, %f, %f)\n", position.x, position.y, position.z);
		//ZZ_LOG("morph[rotation] = (%f, %f, %f, %f)\n", rotation.x, rotation.y, rotation.z, rotation.w);

		HNODE hmesh = NULL, hmat = NULL, hmo = NULL;
		HNODE hmorph = NULL;
		hmesh = loadMesh( NULL, morph_mesh_path );
		hmat = loadColormapMaterial( NULL, shader, morph_texture_path);
		if (morph_motion_path) {
			hmo = loadMotion( NULL, morph_motion_path, 1, ZZ_INTERP_NONE, ZZ_INTERP_NONE, 1, 0 );
		}
		hmorph = loadMorpher( NULL, hmesh, hmo, hmat, light );
		if (hmorph) {			
			HNODE htm = loadAnimatable(NULL, NULL, NULL, NULL, NULL);
			linkNode( htm,    hmorph );
			linkNode( effect, htm );

			setPosition(htm, position.x, position.y, position.z);
			setQuaternion(htm, rotation.w, rotation.x, rotation.y, rotation.z);

			//controlAnimatable( htm, 0 ); // stop
			controlAnimatable( hmorph, 0 ); // stop
			
			setMaterialUseAlpha( hmat, use_alpha );
			setMaterialUseTwoSide( hmat, use_twoside );
			setMaterialUseAlphaTest( hmat, use_alphatest );
			setMaterialZTest( hmat, use_ztest );
			setMaterialZWrite( hmat, use_zwrite );
			//setDrawPriority( hmorph, i );
			setMaterialBlendTypeCustom ( hmat, src_blend, dest_blend, blend_op );
			setRepeatCount( hmorph, loop_count );
			setStartDelay( hmorph, start_delay );
						
			controlAnimatable( hmorph, 1 ); // start			
			//controlAnimatable( htm, 1 ); // start
		}
	}

	insertToScene( effect );
	return effect;
}

ZZ_SCRIPT
ZSTRING getRealPath ( ZSTRING pPath )
{
	CHECK_INTERFACE(getRealPath);
	//static char new_path[ZZ_MAX_STRING];
	//if (!znzin->file_system.check_and_get_path(new_path, pPath)) return NULL;
	//return new_path;
	return NULL;
}

ZZ_SCRIPT
int setMaterialUseSpecular ( HNODE hMaterial, int bUseSpecular )
{
	CHECK_INTERFACE_MATERIAL(setMaterialUseSpecular);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!mat) return 0;

	mat->set_use_specular(ISTRUE(bUseSpecular));
	if (bUseSpecular) {
		mat->set_texturealpha(true); // TODO: modify model merger or .txt file
		mat->push_texture(SPECULAR_SPHEREMAP_PATH);
	}
	return 1;
}

ZZ_SCRIPT
int setBillboardOneAxis ( HNODE hVisible, int bUseBillboard, int iRotationAxis, int iDirectionAxis )
{
	CHECK_INTERFACE(setBillboardOneAxis);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	
	if (!vis) return 0;
	
	vis->set_billboard_one_axis(ISTRUE(bUseBillboard), iRotationAxis, iDirectionAxis);
	
	return 1;
}

ZZ_SCRIPT
int setBillboard ( HNODE hVisible, int bUseBillboard )
{
	CHECK_INTERFACE(setBillboardOneAxis);
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	
	if (!vis) return 0;
	
	vis->set_billboard_axis(ISTRUE(bUseBillboard), 2);
	
	//ZZ_LOG("interface: setBillboard(%s, %d)\n", vis->get_name(), bUseBillboard);
	return 1;
}


ZZ_SCRIPT
HNODE getTexture ( HNODE hMaterial, int iTextureIndex )
{
	CHECK_INTERFACE(getTexture);
	zz_material * mat = reinterpret_cast<zz_material*>(hMaterial);

	if (!mat) return 0;

	zz_texture * tex = mat->get_texture(iTextureIndex);
	zz_assert(znzin && znzin->renderer);
	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	zz_renderer_d3d * ren_d3d = static_cast<zz_renderer_d3d*>(znzin->renderer);

	if (!ren_d3d) {
		return 0;
	}
	
	zz_assert(tex->get_device_updated());

	tex->lock_texture();

	zz_handle tex_handle = tex->get_texture_handle();
	if (!ZZ_HANDLE_IS_VALID(tex_handle)) {
		return 0;
	}
	return reinterpret_cast<HNODE>(ren_d3d->get_texture(tex_handle));
}

ZZ_SCRIPT
void resetScreen ( void )
{
	CHECK_INTERFACE(resetScreen);
	if (znzin) {
		zz_renderer_d3d * r = (zz_renderer_d3d*)(znzin->renderer);
		if (r) {
			r->cleanup();
			r->initialize();
		}
	}
}

ZZ_SCRIPT
float getVisibleRadius ( HNODE hVisible, int bSelectSmaller )
{
	CHECK_INTERFACE(getVisibleRadius);

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0.0f;

	return vis->get_radius(ISTRUE(bSelectSmaller))*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getVisibleHeight ( HNODE hVisible )
{
	CHECK_INTERFACE(getVisibleHeight);

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) return 0.0f;
	
	return vis->get_height()*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
int getModelFrontScript ( HNODE hModel )
{
	CHECK_INTERFACE(getModelFrontScript);
	return getModelFront(hModel, float_array);
}

ZZ_DLL
int getModelFront ( HNODE hModel, float fPositionXYZ_Out[3] )
{
	CHECK_INTERFACE(getModelFront);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	vec3 direction;
	direction = model->get_direction_vector();

	direction.normalize();

	zz_assert(direction.norm() > .5f); // not allow zero-length

	vec3 pos;
	model->get_position_world(pos);

	float radius;
	radius = model->get_radius();

	direction *= .5f*radius; // get front vector by radius
	pos += direction; // get front position
	
	fPositionXYZ_Out[0] = pos.x*ZZ_SCALE_OUT;
	fPositionXYZ_Out[1] = pos.y*ZZ_SCALE_OUT;
	fPositionXYZ_Out[2] = pos.z*ZZ_SCALE_OUT;

	return 1;
}

ZZ_SCRIPT
int getModelDirectionVector ( HNODE hModel, float fDirectionXYZ[3] )
{
	CHECK_INTERFACE(getModelDirectionVector);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	vec3 direction = model->get_direction_vector();
	fDirectionXYZ[0] = direction.x;
	fDirectionXYZ[1] = direction.y;
	fDirectionXYZ[2] = direction.z;

	return 1;
}

ZZ_DLL
int getModelVelocityDirectionVector ( HNODE hModel, float fDirectionXYZ[3] )
{
	CHECK_INTERFACE(getModelDirectionVector);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	vec3 direction = model->get_look_at();
	fDirectionXYZ[0] = direction.x;
	fDirectionXYZ[1] = direction.y;
	fDirectionXYZ[2] = direction.z;
	return 1;
}

ZZ_SCRIPT
int getModelDirectionVectorScript ( HNODE hModel )
{
	CHECK_INTERFACE(getModelDirectionVectorScript);

	return getModelDirectionVector(hModel, float_array);
}

ZZ_SCRIPT
int getVisibleSeethruMode ( HNODE hVisible )
{
	CHECK_INTERFACE(getVisibleSeethruMode);

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	zz_assert(vis);
	if (!vis) return 0;

	return vis->is_seethru();
}

ZZ_SCRIPT
void beginProfiler ( void )
{
	zz_profiler_begin();
}

ZZ_SCRIPT
void endProfiler ( ZSTRING pProfilerName )
{
	zz_profiler_end(pProfilerName);
}

ZZ_SCRIPT
float quaternionToModelDirection ( float fQuatW, float fQuatX, float fQuatY, float fQuatZ )
{
	CHECK_INTERFACE(quaternionToModelDirection);
	quat q(fQuatX, fQuatY, fQuatZ, fQuatW);
	float yaw, pitch, roll;
	quat_2_euler_ogl(q, yaw, pitch, roll);
	return ZZ_TO_DEG * yaw;
}

ZZ_SCRIPT
int setDelayedLoad ( int iDelayedLoad )
{
	CHECK_INTERFACE(setDelayedLoad);

	//ZZ_LOG("interface: setDelayedLoad(%d)\n", iDelayedLoad);

	if (iDelayedLoad == 2) {
		znzin->flush_delayed(true, true);
	}
	else {
		if ((iDelayedLoad == 0) && (state->use_delayed_loading)) { // auto flush at changing state(true->false)
			znzin->flush_delayed(true, true);
		}
		state->use_delayed_loading = (iDelayedLoad == 1);
	}
	
#ifdef ZZ_MATERIAL_LOADTEST
	zz_material::loadtest_dump();
#endif

	return 1;
}

ZZ_SCRIPT
int setTextureLoadingFormat ( int iFormat )
{
	CHECK_INTERFACE(setTextureLoadingFormat);
	zz_assert(iFormat <= 2 && "setTextureLoadingFormat");
	zz_assert(iFormat >= 0 && "setTextureLoadingFormat");
	state->texture_loading_format = iFormat;
	return 1;
}

ZZ_SCRIPT
int setMaxSimultaneousBone ( int iMaxBone )
{
	CHECK_INTERFACE(setMaxSimultaneousBone);
	zz_assert(iMaxBone <= 4 && "setMaxSimultaneousBone");
	zz_assert(iMaxBone >= 1 && "setMaxSimultaneousBone" );
	state->max_simultaneous_bone = iMaxBone;
	return 1;
}

ZZ_SCRIPT
int setDisplayQualityLevel ( int iLevel )
{
	CHECK_INTERFACE(setDisplayQualityLevel);
	zz_assert(iLevel <= 5 && "setDisplayQualityLevel");
	zz_assert(iLevel >= 0 && "setDisplayQualityLevel");
	
	ZZ_LOG("interface: dispqual(%d)\n", iLevel);

	state->set_display_quality_level(iLevel);
	return 1;
}

ZZ_SCRIPT
int setFramerateRange ( int iMinFramerate, int iMaxFramerate )
{
	zz_assert(iMinFramerate > 0);
	zz_assert(iMaxFramerate > 0);
	zz_assert(iMinFramerate <= iMaxFramerate);
	state->min_framerate = iMinFramerate;
	state->max_framerate = iMaxFramerate;
	return 1;
}

ZZ_SCRIPT
int useFileTimeStamp ( int bUse )
{
	state->use_file_time_stamp = ISTRUE(bUse);
	return 1;
}

ZZ_SCRIPT
int useMotionInterpolation ( int bUse )
{
	znzin->set_use_motion_interpolation(ISTRUE(bUse));
	return 1;
}

ZZ_SCRIPT
int setMotionInterpolationRange ( float fDistanceFromCamera )
{
	fDistanceFromCamera *= 100.0f*ZZ_SCALE_IN; // 1meter = 100 centimeter
	znzin->set_motion_interp_range(fDistanceFromCamera*fDistanceFromCamera);
	return 1;
}

ZZ_SCRIPT
int useVSync ( int bUseVSync )
{
	state->use_vsync = ISTRUE(bUseVSync);
	return 1;
}

ZZ_SCRIPT
int setMaterialTextureAddress ( HNODE hMaterial, int iStage, int iTextureAddress )
{
	CHECK_INTERFACE_MATERIAL(setMaterialTextureAddress);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	mat->set_texture_address(iStage, iTextureAddress);

	//ZZ_LOG("interface: setMaterialTextureAddress(%s, %d)\n", mat->get_name(), iTextureAddress);
	return 1;
}
	
ZZ_SCRIPT
int setMaterialUseLight ( HNODE hMaterial, int bUseLight )
{
	CHECK_INTERFACE_MATERIAL(setMaterialUseLight);
	static zz_material * mat;
	mat = reinterpret_cast<zz_material *>(hMaterial);
	zz_assert(mat);
	mat->set_use_light(ISTRUE(bUseLight));

	return 1;
}

ZZ_SCRIPT
HNODE getParent ( HNODE hChild )
{
	CHECK_INTERFACE(getParent);
	zz_node * child = reinterpret_cast<zz_node*>(hChild);
	if (!child) return 0;

	zz_node * parent = child->get_parent();

	if (parent->is_a(ZZ_RUNTIME_TYPE(zz_manager))) {
		return 0;
	}
	return reinterpret_cast<HNODE>(parent);
}

ZZ_SCRIPT
int useDebugDisplay ( int bUseDebugDisplay )
{
	state->use_debug_display = ISTRUE(bUseDebugDisplay);
	return 1;
}

ZZ_SCRIPT
int setLazyBufferSize ( int iTextureSize, int iNormalMeshSize, int iTerrainMeshSize, int iOceanMeshSize )
{
	zz_assert(znzin);
	zz_assert(state);
	zz_assert(znzin->textures);
	zz_assert(znzin->meshes);
	zz_assert(znzin->terrain_meshes);
	zz_assert(znzin->ocean_meshes);

	znzin->textures->set_lazy(state->lazy_texture_size = iTextureSize);
	znzin->meshes->set_lazy(state->lazy_mesh_size = iNormalMeshSize);
	znzin->terrain_meshes->set_lazy(state->lazy_terrain_mesh_size = iTerrainMeshSize);
	znzin->ocean_meshes->set_lazy(state->lazy_ocean_mesh_size = iOceanMeshSize);
	
	return 1;
}

ZZ_SCRIPT
HNODE loadTexture ( 	ZSTRING pTextureName, ZSTRING pTextureFileName, int iMipLevels, int bUseFilter )
{
	return loadTextureWithPool( pTextureName, pTextureFileName, iMipLevels, bUseFilter, 
		zz_device_resource::zz_resource_pool::ZZ_POOL_MANAGED );
}

ZZ_SCRIPT
HNODE loadTextureWithPool ( ZSTRING pTextureName, ZSTRING pTextureFileName, int iMipLevels, int bUseFilter, int iPoolType )
{
	CHECK_INTERFACE(loadTextureWithPool);

	zz_texture * tex = NULL;

	if (!pTextureFileName) {
		ZZ_LOG("interface: loadTexture(%s) failed. no texture file name specified\n", pTextureFileName);
		return 0;
	}

	if (!znzin->file_system.exist(pTextureFileName)) {
		ZZ_LOG("interface: loadTexture() failed. file [%s] not found\n", pTextureFileName);
		return 0;
	}

	if (pTextureName) {
		// find existing
		tex = (zz_texture*)znzin->textures->find(pTextureName);
		if (tex) {
			ZZ_LOG("interface: loadTexture(%s, %s) failed. already exist\n", pTextureName, pTextureFileName);
			return NULL;
		}
	}

	tex = (zz_texture*)znzin->textures->spawn(pTextureName, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
	
	zz_assert(tex);

	bool for_image = true;
	char texfilename[ZZ_MAX_STRING] = "";
	zz_path::to_dos(texfilename, pTextureFileName);

	tex->set_property(texfilename, 0, 0, false /* non-dynamic */, iMipLevels, ISTRUE(bUseFilter), 
		static_cast<zz_device_resource::zz_resource_pool>(iPoolType), ZZ_FMT_UNKNOWN,
		for_image);

	//ZZ_LOG("loadTextureWithPool: [%s], miplevel(%d), filter(%d), pool(%d), image(%d)\n",
	//	texfilename, iMipLevels, bUseFilter, iPoolType, for_image);

	//zz_assert(state->use_delayed_loading);

	tex->lock_texture(); // not to auto unload

	//znzin->textures->load(static_cast<zz_node*>(tex));
	// direct load
	tex->load(); // zhotest

	return reinterpret_cast<HNODE>(tex);
}

ZZ_SCRIPT
int unloadTexture ( HNODE hTexture )
{
	CHECK_INTERFACE(unloadTexture);

	return unloadNode(hTexture);
}

ZZ_SCRIPT
HNODE getTexturePointer ( HNODE hTexture )
{
	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);

	if (!tex) return 0;

	zz_assert(znzin->renderer);
	if (!znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d))) {
		return 0;
	}

	if (!tex->get_device_updated()) {
		tex->load();
	}

	tex->lock_texture();

	LPDIRECT3DTEXTURE9 d3d_tex = ((zz_renderer_d3d*)znzin->renderer)->get_texture(tex->get_texture_handle());
	return reinterpret_cast<HNODE>(d3d_tex);
}


ZZ_SCRIPT
HNODE loadFont ( ZSTRING pFontIDName, ZSTRING pFontName, int iFontCharset, int iFontSize, int iBold, int iItalic, int iColorR, int iColorG, int iColorB, int iColorA )
{
	CHECK_INTERFACE_FONT(loadFont);
	zz_assert(pFontIDName);
	zz_assert(pFontName);
	
	zz_font * font = NULL;
	if (pFontIDName) {
		if (znzin->fonts->find(pFontIDName)) {
			ZZ_LOG("interface: loadFont(%s, %s) failed. already exists.\n", pFontIDName, pFontName);
			return NULL;
		}
	}

	font = (zz_font *)znzin->fonts->spawn(pFontIDName, ZZ_RUNTIME_TYPE(zz_font_d3d), false /* not to autoload */ );

	zz_assert(font);
	zz_assert(iFontSize > 0);

	font->set_font_property(
		pFontName, 
		(unsigned char)iFontCharset,
		iFontSize, 
		ZZ_COLOR32_ARGB(iColorA, iColorR, iColorG, iColorB),
		ZZ_COLOR32_ARGB(255, 0, 0, 0),
		//ISTRUE(iItalic), ISTRUE(iBold), zz_font::OUTLINE_TYPE_SIMPLE); // zhotest
		ISTRUE(iItalic), ISTRUE(iBold), zz_font::OUTLINE_TYPE_NONE);

	znzin->fonts->flush_entrance(font); // load immediately
	return reinterpret_cast<HNODE>(font);
}

ZZ_DLL
HNODE loadFontOutline ( ZSTRING pFontIDName, ZSTRING pFontName, int iFontCharset, int iFontSize, int iBold, int iItalic, int iOutlineType, ZZ_COLOR ColorText, ZZ_COLOR ColorOutline )
{
	CHECK_INTERFACE_FONT(loadFont);

	zz_font * font = NULL;

	if (pFontIDName) {
		if (znzin->fonts->find(pFontIDName)) {
			ZZ_LOG("interface: loadFontOutline(%s, %s) failed. already exists.\n", pFontIDName, pFontName);
			return NULL;
		}
	}

	font = (zz_font *)znzin->fonts->spawn(pFontIDName, ZZ_RUNTIME_TYPE(zz_font_d3d), false /* not to autoload */ );

	zz_assert(font);

	font->set_font_property(
		pFontName,
		(unsigned char)iFontCharset,
		iFontSize, 
		ColorText,
		ColorOutline,
		ISTRUE(iItalic), ISTRUE(iBold), static_cast<zz_font::zz_outline_type>(iOutlineType));
	
	font->set_cache_property(1, 256, 256); // zhotest

	znzin->fonts->flush_entrance(font); // load immediately

	return reinterpret_cast<HNODE>(font);
}



ZZ_SCRIPT
int unloadFont ( HNODE hFont )
{
	CHECK_INTERFACE_FONT(unloadFont);
	return unloadNode(hFont);
}

#define ZZ_CHECKFONT_MACRO(hFont, font, ret) \
	zz_font * font; \
	if (hFont == NULL) { \
		font = znzin->font_sys; \
	} \
	else { \
		font = reinterpret_cast<zz_font*>(hFont); \
		if (!font) return ret; \
	}


ZZ_DLL
int setFontCache ( HNODE hFont, int iNumTextures, int iTextureWidth, int iTextureHeight )
{
	CHECK_INTERFACE_FONT(setFontCache);
	
	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	font->set_cache_property(iNumTextures, iTextureWidth, iTextureHeight);

	return 1;
}

ZZ_SCRIPT
int setFontColor ( HNODE hFont, int iRed, int iGreen, int iBlue, int iAlpha )
{
	CHECK_INTERFACE_FONT(setFontColor);

	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	font->set_color(ZZ_COLOR32_ARGB(iAlpha, iRed, iGreen, iBlue));
	return 1;
}

/// 폰트에 아웃라인 색상 설정
/// @hFont 폰트 핸들
/// @iRed/iGreen/iBlue/iAlpha 0-255 사이의 정수값
/// @return 성공하면 1, 실패하면 0
ZZ_SCRIPT
int setFontOutlineColor ( HNODE hFont, int iRed, int iGreen, int iBlue, int iAlpha )
{
	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	font->set_color_outline(ZZ_COLOR32_ARGB(iAlpha, iRed, iGreen, iBlue));

	return 1;
}

ZZ_DLL
ZZ_SIZE getFontTextExtent ( HNODE hFont, ZSTRING pText )
{
	CHECK_INTERFACE_FONT(getFontStringWidth);

	ZZ_SIZE extent;
	extent.cx = 0;
	extent.cy = 0;
	
	ZZ_CHECKFONT_MACRO(hFont, font, extent);

	font->get_text_extent(pText, &extent);

	return extent;
}

ZZ_SCRIPT
int getFontHeight ( HNODE hFont )
{
	CHECK_INTERFACE_FONT(getFontStringWidth);

	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	return (int)font->get_height();
}

ZZ_SCRIPT
HNODE loadText ( HNODE hFont, int iX, int iY, ZSTRING pMessage )
{
	CHECK_INTERFACE_FONT(loadText );

	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	return static_cast<HNODE>(font->add_text(false /* static */, iX, iY, pMessage));
}

ZZ_SCRIPT
int unloadText ( HNODE hFont, HNODE hText )
{
	CHECK_INTERFACE_FONT(unloadText );

	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	return font->del_text(static_cast<zz_handle>(hText));
}

ZZ_SCRIPT
int unloadAllText ( HNODE hFont )
{
	CHECK_INTERFACE_FONT(unloadAllText);

	ZZ_CHECKFONT_MACRO(hFont, font, 0);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	font->clear_text(false); // clear all static texts
	return 1;
}

ZZ_SCRIPT
int drawFontScript ( HNODE hFont, int iX, int iY, ZSTRING pMessage )
{
	CHECK_INTERFACE_FONT(drawFontScript);

	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	zz_assert(pMessage);
	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	if (((zz_renderer_d3d*)znzin->renderer)->sprite_began()) { // is in begin/end scene section
		font->draw(true, iX, iY, pMessage);
	}
	else {
		font->draw(false, iX, iY, pMessage);
	}

	return 1;
}

ZZ_SCRIPT
int beginSprite ( int iFlag )
{
	CHECK_INTERFACE_SPRITE(beginSprite);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	zz_renderer_d3d * r = (zz_renderer_d3d*)znzin->renderer;

	if (!r->begin_sprite(iFlag, "beginSprite")) 
		return 0;
	r->init_sprite_transform(state->buffer_width, state->buffer_height);
	return 1;
}


ZZ_SCRIPT
int endSprite ( void )
{
	CHECK_INTERFACE_SPRITE(endSprite);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	return ((zz_renderer_d3d*)znzin->renderer)->end_sprite() ? 1 : 0;
}

ZZ_DLL
int drawSprite ( HNODE hTexture, 
    const ZZ_RECT * pSrcRect,
    const ZZ_VECTOR *pCenter,
    const ZZ_VECTOR *pPosition,
    const ZZ_COLOR color
)
{
	CHECK_INTERFACE_SPRITE(drawSprite);

	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);

	if (!tex) return 0;

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	bool ret = ((zz_renderer_d3d*)znzin->renderer)->draw_sprite(tex, (const zz_rect *)(pSrcRect),
		(const vec3 *)(pCenter), (const vec3 *)(pPosition),
		(color32)color);

	return ret ? 1 : 0;
}

ZZ_DLL
int drawSpriteCover ( HNODE hTexture, 
    const ZZ_RECT * pSrcRect,
    const ZZ_VECTOR *pCenter,
    const ZZ_VECTOR *pPosition,
    const ZZ_COLOR origin_color, 
	const ZZ_COLOR cover_color,
	float value
)
{
	CHECK_INTERFACE_SPRITE(drawSprite);

	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);

	if (!tex) return 0;

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	bool ret = ((zz_renderer_d3d*)znzin->renderer)->draw_sprite_cover(tex, (const zz_rect *)(pSrcRect),
		(const vec3 *)(pCenter), (const vec3 *)(pPosition),
		(color32)origin_color,(color32)cover_color,value);

	return ret ? 1 : 0;
}

ZZ_DLL
int getSpriteTextureColor(HNODE hTexture,  
	int iMouseX, int iMouseY,
	const ZZ_RECT * pSrcRect,
    const ZZ_VECTOR *pCenter,
    const ZZ_VECTOR *pPosition,
    D3DXCOLOR *color) 
{
	CHECK_INTERFACE_SPRITE(drawSprite);
	
	color->r = color->g = color->b = color->a = 0;
	
	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);
	if (!tex) return 0;

	zz_renderer_d3d * r = (zz_renderer_d3d*)(znzin->renderer);
    LPD3DXSPRITE sprite = r->get_sprite();
	D3DXMATRIX m;
	D3DXVECTOR2 buffer_v, buffer_length;
	D3DXVECTOR2 center, length;
    int xx, yy;
	zz_rect rect_;
	ZZ_VECTOR center_;
	ZZ_VECTOR position_;	

	if(pSrcRect == NULL)
	{
		rect_.left = 0;
		rect_.right = tex->get_width();
		rect_.top = 0;
		rect_.bottom = tex->get_height();
	}
	else
	{
		rect_ = *pSrcRect;
	}

	if( pCenter == NULL)
	{
		center_.x = 0.0f;
		center_.y = 0.0f;
        center_.z = 0.0f;
	}
	else
	{
		center_ = *pCenter;
	}
	

	if( pPosition == NULL)
	{
		position_.x = 0.0f;
		position_.y = 0.0f;
		position_.z = 0.0f;
	}
	else
	{
		position_ = *pPosition;
	}

	sprite->GetTransform(&m);

    buffer_length.x =  (-rect_.left + rect_.right) * 0.5f;
	buffer_length.y =  (-rect_.top + rect_.bottom) * 0.5f; 
	
	buffer_v.x = position_.x + ( buffer_length.x - center_.x);
	buffer_v.y = position_.y + ( buffer_length.y - center_.y);

	center.x = buffer_v.x * m._11 + m._41;
	center.y = buffer_v.y * m._22 + m._42;
    
	length.x = buffer_length.x * m._11;
	length.y = buffer_length.y * m._22;
     
	xx =  int( (iMouseX- center.x) * (buffer_length.x / length.x) +  (rect_.left + rect_.right) * 0.5f); 
    yy =  int( (iMouseY- center.y) * (buffer_length.y / length.y) +  (rect_.top + rect_.bottom) * 0.5f);

	
	if( (xx < rect_.left) || (xx > rect_.right) || (yy < rect_.top) || (yy > rect_.bottom))
		return 0;
	
	zz_handle tex_handle = tex->get_texture_handle();
	LPDIRECT3DTEXTURE9 d3d_tex = r->get_texture(tex_handle);
		
	D3DSURFACE_DESC d3dsd;
	d3d_tex->GetLevelDesc( 0, &d3dsd );

	D3DLOCKED_RECT rect;
	d3d_tex->LockRect( 0, &rect, NULL, D3DLOCK_READONLY );

	int width, height;

    width = d3dsd.Width;
	height = d3dsd.Height;
	
	
    
	if(d3dsd.Format == D3DFMT_A8B8G8R8)
	{
		BYTE* Bits = (BYTE*)rect.pBits;
		
		color->r = (float)Bits[ (yy*width + xx)*4];
        color->g = (float)Bits[ (yy*width + xx)*4 + 1];
		color->b = (float)Bits[ (yy*width + xx)*4 + 2];
		color->a = (float)Bits[ (yy*width + xx)*4 + 3];

	}
	else if(d3dsd.Format == D3DFMT_X8R8G8B8)
	{
		BYTE* Bits = (BYTE*)rect.pBits;
		
		color->b = (float)Bits[ (yy*width + xx)*4];
        color->g = (float)Bits[ (yy*width + xx)*4 + 1];
		color->r = (float)Bits[ (yy*width + xx)*4 + 2];
		color->a = (float)Bits[ (yy*width + xx)*4 + 3];
    }
	else if(d3dsd.Format == D3DFMT_A4R4G4B4)
	{
		WORD * Bits = (WORD*)rect.pBits;
	    WORD buffer_color[4];
		WORD buffer_b;
	     
	    buffer_color[0] = (Bits[yy*width + xx] >> 12);
		
		buffer_b = (Bits[yy*width + xx] >> 8);
		buffer_color[1] = (buffer_b & 0x000f);

		buffer_b = (Bits[yy*width + xx] >> 4);
		buffer_color[2] = (buffer_b & 0x000f);

		buffer_b = (Bits[yy*width + xx]);
		buffer_color[3] = (buffer_b & 0x000f);
	    
		color->b = (float)buffer_color[3];
        color->g = (float)buffer_color[2];
		color->r = (float)buffer_color[1];
		color->a = (float)buffer_color[0];

	
	}
	else if( d3dsd.Format == D3DFMT_A1R5G5B5 )
	{
		WORD * Bits = (WORD*)rect.pBits;

        WORD buffer_color[4];
		WORD buffer_b;
	     
	    buffer_color[0] = (Bits[yy*width + xx] >> 15);
		
		buffer_b = (Bits[yy*width + xx] >> 10);
		buffer_color[1] = (buffer_b & 0x001f);

		buffer_b = (Bits[yy*width + xx] >> 5);
		buffer_color[2] = (buffer_b & 0x001f);

		buffer_b = (Bits[yy*width + xx]);
		buffer_color[3] = (buffer_b & 0x001f);
	    
		color->b = (float)buffer_color[3];
        color->g = (float)buffer_color[2];
		color->r = (float)buffer_color[1];
		color->a = (float)buffer_color[0];

	
	}
	else if( d3dsd.Format == D3DFMT_R5G6B5)
	{
		WORD * Bits = (WORD*)rect.pBits;
	
	    WORD buffer_color[4];
		WORD buffer_b;
	     
	    buffer_color[0] = (Bits[yy*width + xx] >> 11);
		
		buffer_b = (Bits[yy*width + xx] >> 5);
		buffer_color[1] = (buffer_b & 0x003f);

		buffer_b = (Bits[yy*width + xx]);
		buffer_color[2] = (buffer_b & 0x001f);
	    
		color->b = (float)buffer_color[2];
        color->g = (float)buffer_color[1];
		color->r = (float)buffer_color[0];
	}	
	else if( d3dsd.Format == D3DFMT_X1R5G5B5)
	{
		WORD * Bits = (WORD*)rect.pBits;
	
	    WORD buffer_color[4];
		WORD buffer_b;
	     
	    buffer_b = (Bits[yy*width + xx] >> 10);
		buffer_color[0] = (buffer_b & 0x001f); 

		buffer_b = (Bits[yy*width + xx] >> 5);
		buffer_color[1] = (buffer_b & 0x001f);

		buffer_b = (Bits[yy*width + xx]);
		buffer_color[2] = (buffer_b & 0x001f);
	    
		color->b = (float)buffer_color[2];
        color->g = (float)buffer_color[1];
		color->r = (float)buffer_color[0];
	}	
	
	
	
	d3d_tex->UnlockRect(0);
}



ZZ_DLL
int inputSpriteSFX ( HNODE hTexture, 
    const ZZ_RECT * pSrcRect,
    const ZZ_VECTOR *pCenter,
    const ZZ_VECTOR *pPosition,
    const ZZ_COLOR color, 
	float fade_time1,
	float fade_time2,
	float max_time
)
{
	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);

	if (!tex) return 0;

	if( (fade_time1 > fade_time2) || (fade_time1 > max_time) || (fade_time2 > max_time))
		return 0;
	
	znzin->sprite_sfx.input_sprite_sfx_element(tex, (const zz_rect *)(pSrcRect),
		(const vec3 *)(pCenter), (const vec3 *)(pPosition), (color32)color, fade_time1,fade_time2, max_time);
    return 1;
}

ZZ_DLL
void stopSpriteSFX()
{
	zz_assert(znzin);
	znzin->sprite_sfx.stop_sprite_sfx();

}

ZZ_DLL
bool getSpriteSFXPlayOnOff()
{
	zz_assert(znzin);
	return 	znzin->sprite_sfx.get_play_onoff();
}

ZZ_DLL
void pauseSpriteSFX(bool onoff)
{
	zz_assert(znzin);
	
	if(onoff)
		znzin->sprite_sfx.pause_on();
	else
		znzin->sprite_sfx.pause_off();

}

ZZ_DLL
bool getSpriteSFXPauseOnOff()
{
	zz_assert(znzin);
	
	return znzin->sprite_sfx.get_pause_onoff();
}


ZZ_DLL
HNODE getSpriteSFXCurrentTexNode()
{
	zz_assert(znzin);
    if(!(znzin->sprite_sfx.get_play_onoff()))
		return NULL;
	else
		return reinterpret_cast<HNODE>(znzin->sprite_sfx.sfx_tex);
}

ZZ_SCRIPT
int flushSprite ( void )
{
	CHECK_INTERFACE_SPRITE(flushSprite);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	return ((zz_renderer_d3d*)znzin->renderer)->flush_sprite() ? 1 : 0;
}

ZZ_DLL
int getTransformSprite ( float * d3d_tm_4x4 )
{
	CHECK_INTERFACE_SPRITE(getTransformSprite);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));
	return ((zz_renderer_d3d*)znzin->renderer)->get_sprite_transform(d3d_tm_4x4) ? 1 : 0;
}

ZZ_DLL
int setTransformSprite ( const float * d3d_tm_4x4 )
{
	CHECK_INTERFACE_SPRITE(setTransformSprite);

	zz_assert(znzin->renderer->is_a(ZZ_RUNTIME_TYPE(zz_renderer_d3d)));

	return ((zz_renderer_d3d*)znzin->renderer)->set_sprite_transform(d3d_tm_4x4) ? 1 : 0;
}

ZZ_SCRIPT
int drawFontLater ( HNODE hFont, int iX, int iY, ZSTRING pMsg)
{
	CHECK_INTERFACE_FONT(drawFontLater);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->add_text(true /* dynamic */, iX, iY, pMsg);
	return 1;
}

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, int iX, int iY, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->draw(ISTRUE(bUseSprite), iX, iY, pMsg);
	return 1;
}

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, int iX, int iY, ZZ_COLOR Color, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->set_color(Color);
	font->draw(ISTRUE(bUseSprite), iX, iY, pMsg);
	return 1;
}

ZZ_DLL
int drawFontOutLine ( HNODE hFont, int bUseSprite, int iX, int iY, ZZ_COLOR Color, ZZ_COLOR OutLineColor, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->set_color(Color);
	font->set_color_outline(OutLineColor);
	font->draw(ISTRUE(bUseSprite), iX, iY, pMsg);
	return 1;
}

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	DWORD dwFormat = DT_WORDBREAK | DT_LEFT | DT_TOP | DT_NOCLIP;
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, pMsg);
	return 1;
}

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_DWORD dwFormat, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, pMsg);
	return 1;
}

ZZ_DLL
int drawFont ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_COLOR Color, ZZ_DWORD dwFormat, const char * pMsg)
{
	CHECK_INTERFACE_FONT(drawFont);
	ZZ_CHECKFONT_MACRO(hFont, font, 0);
	if (!pMsg) return 0;
	font->set_color(Color);
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, pMsg);
	return 1;
}

#define ZZ_DRAWFONTF_MACRO(hfont, msg_format) \
	if (msg_format == NULL) return 0; \
	zz_font * font = reinterpret_cast<zz_font*>(hfont); \
	if (font == NULL) font = znzin->font_sys; \
	static va_list _va; \
	static char formatted_string[ZZ_MAX_STRING]; \
	va_start(_va, msg_format); \
	vsprintf(formatted_string, msg_format, _va); \
	va_end(_va);

ZZ_DLL
int drawFontLaterf ( HNODE hFont, int iX, int iY, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontLaterf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	font->add_text(true /* dynamic */, iX, iY, formatted_string);
	return 1;
}

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, int iX, int iY, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	font->draw(ISTRUE(bUseSprite), iX, iY, formatted_string);
	return 1;
}

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, int iX, int iY, ZZ_COLOR Color, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	font->set_color(Color);
	font->draw(ISTRUE(bUseSprite), iX, iY, formatted_string);
	return 1;
}

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	DWORD dwFormat = DT_WORDBREAK | DT_LEFT | DT_TOP | DT_NOCLIP;
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, formatted_string);
	return 1;
}

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_DWORD dwFormat, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, formatted_string);
	return 1;
}

ZZ_DLL
int drawFontf ( HNODE hFont, int bUseSprite, ZZ_RECT * pRect, ZZ_COLOR Color, ZZ_DWORD dwFormat, const char * pMsgFormat, ... )
{
	CHECK_INTERFACE_FONT(drawFontf);
	ZZ_DRAWFONTF_MACRO(hFont, pMsgFormat);
	if (!pMsgFormat) return 0;
	font->set_color(Color);
	font->draw(ISTRUE(bUseSprite), pRect, dwFormat, formatted_string);
	return 1;
}

ZZ_SCRIPT
int setFileSytem ( HNODE hVFS )
{
	// not used anymore
	return 0;
}

ZZ_SCRIPT
int openFileSystem ( ZSTRING pIndexFileName)
{
#ifndef ZZ_IGNORE_TRIGGERVFS
	if (zz_system::set_pkg_system( pIndexFileName ) == NULL) return 0;
#endif
	return 1;
}

ZZ_SCRIPT
int closeFileSystem ( void )
{
	// Since zz_system does this automaticall, no need to call.
	return 1;
}

ZZ_SCRIPT
HNODE fileReadFrom ( ZSTRING pFilename )
{
	zz_vfs* vfstream = zz_new zz_vfs; // will be deallocated in fileClose()
	if (!vfstream->open(pFilename)) return 0;
	return reinterpret_cast<HNODE>(vfstream);
}

ZZ_SCRIPT
ZSTRING fileReadWord ( HNODE hFile )
{
	zz_vfs* vfstream = reinterpret_cast<zz_vfs*>(hFile);
	static char strbuffer[ZZ_MAX_STRING];
	if (vfstream) {
		vfstream->read_string_without_whitespace(strbuffer);
	}
	else {
		strbuffer[0] = '\0'; // make null-string
	}
	return strbuffer;
}

ZZ_SCRIPT
int fileClose ( HNODE hFile )
{
	zz_vfs* vfstream = reinterpret_cast<zz_vfs*>(hFile);
	if (!vfstream) return 0;
	vfstream->close();
	zz_delete vfstream; // allocated from fileReadFrom
	return 1;
}

ZZ_SCRIPT
int setMotionInterporationInterval ( HNODE hMotion, int iMilliSeconds )
{
	zz_motion * motion = reinterpret_cast<zz_motion*>(hMotion);

	if (!motion) return 0; // no motion was set

	if (iMilliSeconds < 0) return 0; // not acceptable range

	motion->set_interp_interval(ZZ_MSEC_TO_TIME(iMilliSeconds));

	return 1;
}

ZZ_SCRIPT
int savePrevPosition ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->save_prev_position_world();

//	ZZ_LOG("savePrevPosition(%s)\n", vis->get_name());
	return 1;
}

ZZ_DLL
int savePrevRotation( HNODE hVisible)
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->save_prev_rotation_world();

//	ZZ_LOG("savePrevPosition(%s)\n", vis->get_name());
	return 1;
}

ZZ_DLL
int getPrevPosition ( HNODE hVisible, float fPositionXYZ[3] )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevPosition() failed\n");
		fPositionXYZ[0] = ZZ_INFINITE;
		fPositionXYZ[1] = ZZ_INFINITE;
		fPositionXYZ[2] = ZZ_INFINITE;
		return 0;
	}
	vec3 pos;
	pos = vis->get_prev_position_world();
	fPositionXYZ[0] = pos.x*ZZ_SCALE_OUT;
	fPositionXYZ[1] = pos.y*ZZ_SCALE_OUT;
	fPositionXYZ[2] = pos.z*ZZ_SCALE_OUT;
	return 1;
}

ZZ_SCRIPT
float getPrevPositionX ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevPositionX() failed\n");
		return ZZ_INFINITE;
	}
	vec3 pos;
	pos = vis->get_prev_position_world();
	return pos.x*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getPrevPositionY ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevPositionY() failed\n");
		return ZZ_INFINITE;
	}
	vec3 pos;
	pos = vis->get_prev_position_world();
	return pos.y*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
float getPrevPositionZ ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevPositionZ() failed\n");
		return ZZ_INFINITE;
	}
	vec3 pos;
	pos = vis->get_prev_position_world();
	return pos.z*ZZ_SCALE_OUT;
}

ZZ_DLL 
int getPrevRotation ( HNODE hVisible, float fRotationWXYZ[4])
{

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevRotationWXYZ() failed\n");
		return 0;
	}
	quat rot;
	rot = vis->get_prev_rotation_world();
	fRotationWXYZ[0] = rot.w;
	fRotationWXYZ[1] = rot.x;
    fRotationWXYZ[2] = rot.y;
    fRotationWXYZ[3] = rot.z;

	return 1;

}

ZZ_DLL 
int getRotationQuad ( HNODE hVisible, float fRotationWXYZ[4])
{

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevRotationWXYZ() failed\n");
		return 0;
	}
	quat rot;
	vis->get_rotation_world(rot);
	fRotationWXYZ[0] = rot.w;
	fRotationWXYZ[1] = rot.x;
    fRotationWXYZ[2] = rot.y;
    fRotationWXYZ[3] = rot.z;

	return 1;
}

ZZ_DLL
int getRotationQuadLocal( HNODE hVisible, float fRotationWXYZ[4])
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
	if (!vis) {
		ZZ_LOG("interface: getPrevRotationWXYZ() failed\n");
		return 0;
	}
	quat rot;
	rot = vis->get_rotation();
	fRotationWXYZ[0] = rot.w;
	fRotationWXYZ[1] = rot.x;
    fRotationWXYZ[2] = rot.y;
    fRotationWXYZ[3] = rot.z;

	return 1;
}

ZZ_DLL
int setRotationQuadLocal( HNODE hVisible, float fRotationWXYZ[4])
{
	CHECK_INTERFACE(setRotationQuat);

	zz_visible * vis = reinterpret_cast<zz_visible *>(hVisible);
	zz_assert(vis);
	
	if (!vis) return 0;
	
	quat rot;

	rot.w = fRotationWXYZ[0];
	rot.x = fRotationWXYZ[1];
	rot.y = fRotationWXYZ[2];
	rot.z = fRotationWXYZ[3];
	
	vis->set_rotation_local(rot);
	vis->invalidate_transform();
	
	return 1;
}



//ZZ_DLL
//int getModelRightPosition( HNODE hModel, float fRightDistance, float fWorldPosOut[3] )
//{
//	zz_model * model = reinterpret_cast<zz_model*>(hModel);
//
//	if (!model) return 0;
//
//	vec3 up(0, 0, 1.0f);
//	vec3 front = model->get_direction_vector();
//	vec3 right;
//	right.cross(front, up);
//
//	right.normalize();
//	right *= fRightDistance;
//
//	vec3 model_pos;
//	model->get_position_world(model_pos);
//	fWorldPosOut[0] = model_pos.x + right.x;
//	fWorldPosOut[1] = model_pos.y + right.y;
//	fWorldPosOut[2] = model_pos.z + right.z;
//
//	return 1;
//}


//ZZ_DLL
//int getModelRightPosition( HNODE hModel, float fRightDistance, float fWorldPosOut[3] )
//{
//	zz_model * model = reinterpret_cast<zz_model*>(hModel);
//
//	if (!model) return 0;
//
//	vec3 front = model->get_direction_vector();
//	front.normalize();
//
//	vec3 rotation_axis_z(0, 0, 1);
//	mat3 matrix_rotation(mat3_id), matrix_result;
//	float angle_degree = (fRightDistance < 0) ? 90.0f : -90.0f;
//	matrix_rotation.set_rot(ZZ_TO_RAD*angle_degree, rotation_axis_z);
//	vec3 right;
//	
//	mult(right, matrix_rotation, front);
//	
//	right.normalize();
//	right *= fabs(fRightDistance);
//
//	vec3 model_pos;
//	model->get_position_world(model_pos);
//	fWorldPosOut[0] = model_pos.x + right.x;
//	fWorldPosOut[1] = model_pos.y + right.y;
//	fWorldPosOut[2] = model_pos.z + right.z;
//
//	return 1;
//}

ZZ_DLL
int getModelShiftPosition ( HNODE hModel, float fCmPerSecond, float fWorldPosOut[3] )
{
	CHECK_INTERFACE( getModelShiftPosition );

	zz_assert(fCmPerSecond > 0);

	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	vec3 front = model->get_direction_vector();
	front.normalize();

	vec3 rotation_axis_z(0, 0, 1);
	mat3 matrix_rotation(mat3_id), matrix_result;
	float angle_degree = (model->get_dir_left_first()) ? 80.0f : -80.0f;
	matrix_rotation.set_rot(ZZ_TO_RAD*angle_degree, rotation_axis_z);
	vec3 right;
	
	mult(right, matrix_rotation, front);
	
	right.normalize();
	float distance = fCmPerSecond * ZZ_SCALE_IN * 0.001f * ZZ_TIME_TO_MSEC(znzin->get_diff_time());

	right *= distance;

	vec3 model_pos;
	model->get_position_world(model_pos);
	fWorldPosOut[0] = (model_pos.x + right.x)*ZZ_SCALE_OUT;
	fWorldPosOut[1] = (model_pos.y + right.y)*ZZ_SCALE_OUT;
	fWorldPosOut[2] = (model_pos.z)*ZZ_SCALE_OUT;//  + right.z;

	return 1;
}

ZZ_DLL
int flipModelShiftDirection ( HNODE hModel )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	model->flip_dir_left_first();

	return 1;
}
	
/// 비져블에 중력을 적용한 위치를 리턴한다.
/// 현재 속도 및 질량은 적용되지 않음.
ZZ_DLL
float applyGravity ( HNODE hVis )
{
	CHECK_INTERFACE( applyGravity );

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVis);

	if (!vis) return 0.0f;

	vec3 pos;

	{ // engine space
		vis->get_position_world(pos);
		float distancez = znzin->get_linear_gravity() * ZZ_TIME_TO_MSEC(znzin->get_diff_time());
		pos.z -= distancez;
	} 
	// game space
	pos *= ZZ_SCALE_OUT;

	//ZZ_LOG("applyGravity() %f\n", pos.z);

	setPositionVec3( hVis, pos.vec_array );
	return pos.z;
}

ZZ_SCRIPT
float getWorldObjectHeightInScene ( float fWorldX, float fWorldY, float fDefaultHeight )
{
	CHECK_INTERFACE(getWorldObjectHeightInScene);
	bool first = true;
	zz_visible * vis = NULL;
    const zz_bounding_aabb * aabb = NULL;
	
	fDefaultHeight *= ZZ_SCALE_IN;

	float max_height = fDefaultHeight;
	float current_height = fDefaultHeight;
	const vec3 up_dir(0, 0, 1), down_dir(0, 0, -1);
	
	fWorldX *= ZZ_SCALE_IN;
	fWorldY *= ZZ_SCALE_IN;

	vec3 origin(fWorldX, fWorldY, 0);
	vec3 contact_point;
	vec3 contact_normal;
	
	//znzin->scene.update(zz_time(0));

	vec3 minmax[2];

	minmax[0].set(fWorldX, fWorldY, ZZ_OCTREE_MINZ);
	minmax[1].set(fWorldX, fWorldY, ZZ_OCTREE_MAXZ);

	int pack_index = 0;
	int num_nodes = znzin->scene.collect_by_minmax(pack_index, minmax, true);

	for (int i = 0; i < num_nodes; i++) {
		vis = znzin->scene.get_collect_node(pack_index, i);
	
		zz_assert(vis);
		zz_assert(vis->is_a(ZZ_RUNTIME_TYPE(zz_visible)));
        
		if (vis->is_a(ZZ_RUNTIME_TYPE(zz_terrain_block))) {
			continue; // skip terrain block
		}
		if (vis->is_a(ZZ_RUNTIME_TYPE(zz_ocean_block))) {
			continue; // skip ocean block
		}
		if (vis->is_descendant_of_type(ZZ_RUNTIME_TYPE(zz_model))) {
			continue; // skip model sub components
		}
		zz_collision_level level = vis->get_collision_level();

		if (ZZ_IS_NOTMOVEABLE( level )) {
			continue; // skip if not moveable
		}

		zz_assert(vis->get_bvolume());
		aabb = vis->get_bvolume()->get_aabb();
		zz_assert(aabb);

		// if not included in 2D-bounding aabb, then skip this node.
		if (fWorldX < aabb->pmin.x) continue;
		if (fWorldX > aabb->pmax.x) continue;
		if (fWorldY < aabb->pmin.y) continue;
		if (fWorldY > aabb->pmax.y) continue;

		// from getWorldHeight()
		// check both up and down
		if (vis->get_intersection_ray(origin, up_dir, contact_point, contact_normal, zz_mesh_tool::ZZ_SM_FARTHEST)) {
			current_height = contact_point.z;
		}
		else if (vis->get_intersection_ray(origin, down_dir, contact_point, contact_normal, zz_mesh_tool::ZZ_SM_NEAREST)) {
			current_height = contact_point.z;
		}
		else {
			continue; // skip if not intersected
		}

		if (first) {
			first = false;
			max_height = current_height;
		}
		else {
			if (current_height > max_height) {
                max_height = current_height; // update max_height
			}
		}
	}
	return max_height*ZZ_SCALE_OUT;
}

ZZ_SCRIPT
HNODE loadSound ( ZSTRING strSoundName, ZSTRING strWaveFileName, int iNumBuffers )
{
#ifdef ZZ_USE_SOUND
	CHECK_INTERFACE(loadSound);

	zz_sound * sound = (zz_sound *)znzin->sounds->find(strSoundName);
	
	if (sound) {
		ZZ_LOG("interface: loadSound(%s) failed. already exists\n", strSoundName);
		return 0;
	}
	
	sound = (zz_sound *)znzin->sounds->spawn(strSoundName, ZZ_RUNTIME_TYPE(zz_sound), false /* not to autoload */);
	zz_assert(sound);
	
	if (!sound) {
		ZZ_LOG("interface: loadSound(%s) failed. cannot spawn sound\n", strSoundName);
		return 0;
	}
	
	if (!sound->set_property(strWaveFileName, iNumBuffers)) {
		ZZ_LOG("interface: loadSound(%s) failed. invalid path(%s) or num_buffers(%d)\n", strSoundName, strWaveFileName, iNumBuffers);
		unloadSound(HNODE(sound));
		return 0;
	}
	
	znzin->sounds->load(reinterpret_cast<zz_node*>(sound));

	return reinterpret_cast<HNODE>(sound);
#else
	return 0;
#endif
}

ZZ_SCRIPT
int unloadSound ( HNODE hSound )
{
#ifdef ZZ_USE_SOUND
	return unloadNode( hSound );
#else
	return 0;
#endif
}

ZZ_DLL
int setSoundPosition ( HNODE hSound, float fPosition[3] )
{
#ifdef ZZ_USE_SOUND
	zz_sound * sound = reinterpret_cast<zz_sound *>(hSound);

	if (!sound) return 0;

	sound->set_position(fPosition);

	return 1;
#else
	return 0;
#endif
}

ZZ_SCRIPT
int playSound ( HNODE hSound, int bLoop )
{
#ifdef ZZ_USE_SOUND
	zz_sound * sound = reinterpret_cast<zz_sound *>(hSound);

	if (!sound) return 0;

	return sound->play(ISTRUE(bLoop)) ? 1 : 0;
#else
	return 0;
#endif
}

ZZ_SCRIPT
int stopSound ( HNODE hSound )
{
#ifdef ZZ_USE_SOUND
	zz_sound * sound = reinterpret_cast<zz_sound *>(hSound);

	if (!sound) return 0;

	return sound->stop() ? 1 : 0;
#else
	return 0;
#endif
}

// @return : 0(Stopped), 1(Started)
ZZ_SCRIPT
int getSoundState ( HNODE hSound )
{
#ifdef ZZ_USE_SOUND
	zz_sound * sound = reinterpret_cast<zz_sound *>(hSound);

	if (!sound) return 0;

	return sound->is_playing() ? 1 : 0;
#else
	return 0;
#endif
}

ZZ_SCRIPT
int initSoundSystem ( void )
{
#ifdef ZZ_USE_SOUND
	if (!znzin) return false;
	if (!znzin->sounds) return false;

	return znzin->sounds->initialize();
#else
	return 0;
#endif
}

ZZ_SCRIPT
int destSoundSystem ( void )
{
#ifdef ZZ_USE_SOUND
	if (!znzin) return false;
	if (!znzin->sounds) return false;
#endif
	// will be destructed in ~system()
	return 1;
}

ZZ_SCRIPT
int updateSoundSystem ( void )
{
#ifdef ZZ_USE_SOUND
	if (!znzin) return false;
	if (!znzin->sounds) return false;
	
	return znzin->sounds->update_listener() ? 1 : 0;
#else
	return 0;
#endif
}

ZZ_DLL
float getVectorAngle ( float vVector1[3], float vVector2[3] )
{
	vec3 v1(vVector1), v2(vVector2);
	float angle_radian;
	v1.normalize();
	v2.normalize();
	get_angle(angle_radian, v1, v2);
	return ZZ_TO_DEG*angle_radian;
}

ZZ_SCRIPT 
HNODE loadSkyMaterial ( ZSTRING pMaterialName, HNODE hShader, ZSTRING pMapFileName1, ZSTRING pMapFileName2 )
{
	CHECK_INTERFACE(loadSkyMaterial);

	if (!pMapFileName1) {
		return 0;
	}

	if (!znzin->file_system.exist(pMapFileName1)) {
		ZZ_LOG("interface: loadSkyMaterial(%s, %d, %s, %s) failed. [%s] not found\n",
			pMaterialName, hShader, pMapFileName1, pMapFileName2, pMapFileName1);
		return 0;
	}

	zz_material_sky * skymat = 
		(zz_material_sky *)znzin->materials->find_or_spawn(pMaterialName, ZZ_RUNTIME_TYPE(zz_material_sky));
	
	zz_shader * shader = reinterpret_cast<zz_shader *>(hShader);

	zz_assert(skymat);
	if (!skymat) {
		ZZ_LOG("interface: loadSkyMaterial() failed. no map");
		return 0;
	}
	if (shader) skymat->set_shader(shader);

	// set texture
	if (!skymat->set_texture(0, pMapFileName1)) {
		// maybe, texture loading failed
		return 0;
	}

	if (pMapFileName2) {
		if (!skymat->set_texture(1, pMapFileName2)) {
			// maybe, texture loading failed
			return 0;
		}
	}

	skymat->set_texture_address(0, ZZ_TADDRESS_WRAP);

	return reinterpret_cast<HNODE>(skymat);
}

ZZ_SCRIPT 
int setSkyMaterialBlendRatio ( HNODE hMat, float fBlendRatio )
{
	CHECK_INTERFACE( setSkyMaterialBlendRatio );

	zz_material_sky * skymat = reinterpret_cast<zz_material_sky*>(hMat);

	if (!skymat) return 0;

	zz_assert(fBlendRatio >= 0);
	zz_assert(fBlendRatio <= 1.0f);

	if (fBlendRatio < 0) {
		fBlendRatio = 0;
	}
	else if (fBlendRatio > 1.0f) {
		fBlendRatio = 1.0f;
	}

	skymat->set_blend_ratio(fBlendRatio);

	return true;
}

ZZ_DLL
int getVisibleAABB ( HNODE hVisible_In, float fMin_Out[3], float fMax_Out[3] )
{
	CHECK_INTERFACE( getVisibleAABB );
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible_In);

	if (!vis) return 0;

	const zz_bounding_aabb * aabb;
	zz_bvolume * bv = vis->get_bvolume();
	if (!bv) return 0;
	aabb = bv->get_aabb();
	if (aabb) {
		fMin_Out[0] = aabb->pmin.x*ZZ_SCALE_OUT;
		fMin_Out[1] = aabb->pmin.y*ZZ_SCALE_OUT;
		fMin_Out[2] = aabb->pmin.z*ZZ_SCALE_OUT;
		fMax_Out[0] = aabb->pmax.x*ZZ_SCALE_OUT;
		fMax_Out[1] = aabb->pmax.y*ZZ_SCALE_OUT;
		fMax_Out[2] = aabb->pmax.z*ZZ_SCALE_OUT;
		return 1;
	}
	return 0;
}

ZZ_DLL
int getVisibleSphere ( HNODE hVisible_In, float vCenter_Out[3], float * fRadius_Out )
{
	CHECK_INTERFACE( getVisibleSphere );
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible_In);

	if (!vis) return 0;

	const zz_bounding_sphere * sphere;
	zz_bvolume * bv = vis->get_bvolume();
	if (!bv) return 0;
	sphere = bv->get_sphere();
	if (sphere) {
		vCenter_Out[0] = sphere->center.x;
		vCenter_Out[1] = sphere->center.y;
		vCenter_Out[2] = sphere->center.z;
		if (fRadius_Out) {
			*fRadius_Out = sphere->radius;
		}
		return 1;
	}
	return 0;
}

ZZ_DLL
int intersectRayNormal ( HNODE hVisible_In, float vRayOrigin_In[3], float vRayDirection_In[3], float vContactPoint_Out[3], float vContactNormal_Out[3] )
{
	CHECK_INTERFACE(intersectRayNormal);

	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible_In);

	if (!vis) return 0;

	vec3 origin(vRayOrigin_In), direction(vRayDirection_In), contact_point, contact_normal;
	origin *= ZZ_SCALE_IN;
	bool ret = vis->get_intersection_ray(origin, direction, contact_point, contact_normal);
	if (!ret) return 0;

	if (vContactPoint_Out) {
		vContactPoint_Out[0] = contact_point.x*ZZ_SCALE_OUT;
		vContactPoint_Out[1] = contact_point.y*ZZ_SCALE_OUT;
		vContactPoint_Out[2] = contact_point.z*ZZ_SCALE_OUT;
	}
	if (vContactNormal_Out) {
		vContactNormal_Out[0] = contact_normal.x;
		vContactNormal_Out[1] = contact_normal.y;
		vContactNormal_Out[2] = contact_normal.z;
	}
	return 1;
}

ZZ_SCRIPT
int logTextures ( void )
{
	zz_list<zz_texture *> child_list;

	zz_manager * textures = znzin->textures;

	if (!textures) return 0;

	textures->get_all_children( child_list );

	ZZ_LOG("interface: logTextures(%d)..\n", child_list.size());
	int count = 0;
	int size = 0;
	zz_vfs vfs;

	for (zz_list<zz_texture *>::iterator it = child_list.begin(); it != child_list.end(); ++it) {
		zz_assert(*it);
		if ((*it)->get_path()) {
			size = vfs.get_size((*it)->get_path());
		}
		else {
			size = 4*(*it)->get_width()*(*it)->get_height();
		}

		ZZ_LOG("%03d: %6dKB-[%3dx%3d]-[%s]\n",
			count++,
			size / 1024,
			(*it)->get_width(),
			(*it)->get_height(),
			(*it)->get_path()
		);
	}
	return count;
}

ZZ_DLL
int getModelCOMPositionWorld ( HNODE hModel, float vCenterOfMass[3] )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) return 0;
	vec3& pos_world = FLOAT3_TO_VEC3(vCenterOfMass);
	pos_world = model->get_com_position_world();
	pos_world *= ZZ_SCALE_OUT;
	return 1;
}

ZZ_DLL
int getModelCOMPositionLocal ( HNODE hModel, float vCenterOfMass[3] )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) return 0;
	vec3& pos_world = FLOAT3_TO_VEC3(vCenterOfMass);
	pos_world = model->get_com_position_local();
	pos_world *= ZZ_SCALE_OUT;
	return 1;
}

ZZ_DLL
int getModelBonePositionWorld ( HNODE hModel, int BoneIndex, float vPosition[3] )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
	if (!model) return 0;
	vec3& pos_world = FLOAT3_TO_VEC3(vPosition);
	pos_world = model->get_bone_position_world(BoneIndex);
	pos_world *= ZZ_SCALE_OUT;
	return 1;
}



ZZ_SCRIPT
int setVisibleRangeFromCamera ( HNODE hVisible, float fDistanceFromCameraStart, float fDistanceFromCameraEnd )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	fDistanceFromCameraStart *= ZZ_SCALE_IN;
	fDistanceFromCameraEnd *= ZZ_SCALE_IN;

	vis->set_range_square( fDistanceFromCameraStart*fDistanceFromCameraStart, fDistanceFromCameraEnd*fDistanceFromCameraEnd);
	return 1;
}

ZZ_SCRIPT
int pushSpecialEffect ( int iEffectID )
{
	if (!znzin) return 0;
	return znzin->sfxs->push_sfx( static_cast<zz_manager_sfx::e_type>(iEffectID) ) ? 1 : 0;
}

ZZ_SCRIPT
int popSpecialEffect ( void )
{
	if (!znzin) return 0;
	znzin->sfxs->pop_sfx();
	return 1;
}

ZZ_SCRIPT
int clearSpecialEffect ( void )
{
	if (!znzin) return 0;
	znzin->sfxs->clear_sfx();
	return 1;
}

ZZ_DLL
int getTextureSize ( HNODE hTexture, int& iWidth_Out, int& iHeight_Out )
{
	zz_texture * tex = reinterpret_cast<zz_texture*>(hTexture);

	if (!tex) return 0;

	if (!tex->get_device_updated()) {
		ZZ_LOG("gettexturesize(%s) failed. not read yet\n", tex->get_path());
		return 0; // not buffered
	}

	iWidth_Out = tex->get_width();
	iHeight_Out = tex->get_height();

	return 1;
}

ZZ_DLL
int setModelNormal ( HNODE hModel, float vNormal[3] )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	model->set_normal(vec3(vNormal));

	return 1;
}

ZZ_DLL
int shakeCamera ( HNODE hCamera, float vMin[3], float vMax[3], int iTimeMSEC )
{
	zz_camera_follow * cam = reinterpret_cast<zz_camera_follow*>(hCamera);

	if (!cam || !IS_A(cam, zz_camera_follow)) return 0;

	cam->set_shake(ZZ_MSEC_TO_TIME(iTimeMSEC), ZZ_SCALE_IN*vec3(vMin), ZZ_SCALE_IN*vec3(vMax));

	return 1;
}

ZZ_DLL
int getVisibleVelocity ( HNODE hVisible, float vVelocityVector[3] )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis || !IS_A(vis, zz_visible)) return 0;

	const vec3& velocity = vis->get_velocity();
	vVelocityVector[0] = velocity.x;  // * ZZ_SCALE_OUT;         //test 12-7
	vVelocityVector[1] = velocity.y;  //* ZZ_SCALE_OUT;
	vVelocityVector[2] = velocity.z;  //* ZZ_SCALE_OUT;
	
	return 1;
}

ZZ_DLL
int setVisibleVelocity ( HNODE hVisible, float vVelocityVector[3] )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis || !IS_A(vis, zz_visible)) return 0;

	vis->set_velocity(vec3(vVelocityVector));

	return 1;
}

ZZ_SCRIPT
int zeroVisibleVelocityZ ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis || !IS_A(vis, zz_visible)) return 0;

	vec3 velocity = vis->get_velocity();
	velocity.z = 0;
	vis->set_velocity(velocity);
	return 1;
}

ZZ_DLL
int setGravity ( float vGravityVector[3] )
{
	vec3 gravity(vGravityVector);

	gravity *= ZZ_SCALE_IN;

	zz_visible::set_gravity(gravity);
	
	return 1;
}

ZZ_SCRIPT
int setGravityScript ( float fGravityX, float fGravityY, float fGravityZ )
{
	vec3 gravity(fGravityX, fGravityY, fGravityZ);

	gravity *= ZZ_SCALE_IN;
	zz_visible::set_gravity(gravity);
	
	return 1;
}

ZZ_DLL
int getModelInitialCOMPosition ( HNODE hModel, float vPositionOut[3] )
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);

	if (!model) return 0;

	zz_motion * motion = model->get_motion();
	if (!motion) return 0;

	const vec3& initial_position = motion->get_initial_position();

	vPositionOut[0] = ZZ_SCALE_OUT*initial_position.x;
	vPositionOut[1] = ZZ_SCALE_OUT*initial_position.y;
	vPositionOut[2] = ZZ_SCALE_OUT*initial_position.z;

	return 1;
}

ZZ_DLL 
void InputModelCollisionBlock(HNODE hModel, HNODE hBlock)
{
	zz_model * model = reinterpret_cast<zz_model*>(hModel);
    zz_animatable *block = reinterpret_cast<zz_model*>(hBlock);
	
	model->input_collision_block(block);

}

ZZ_DLL
HNODE OutPutModelCollisionBlock(HNODE hModel)
{
   zz_model * model = reinterpret_cast<zz_model*>(hModel);
   zz_animatable *block = NULL;
   
   block = model->output_collision_block();
   model->reset_collision_block();
   if(block != NULL)
   return reinterpret_cast<HNODE>(block);
   else
   return NULL;
}

ZZ_DLL
void InputModelGravity(HNODE hModel)
{
	 zz_model * model = reinterpret_cast<zz_model*>(hModel);
     model->set_apply_gravity();
}


ZZ_DLL
void stopModelRotation (HNODE hModel)
{
	 zz_model * model = reinterpret_cast<zz_model*>(hModel);
     model->stop_rotation();
}

ZZ_SCRIPT
int useGravity ( HNODE hVisible, int bUseOrNot )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vis->set_use_gravity( ISTRUE(bUseOrNot) );

	return 1;
}

ZZ_DLL
int setVisibleForce ( HNODE hVisible, float vForceVector[3] )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	vForceVector[0] *= ZZ_SCALE_IN;
	vForceVector[1] *= ZZ_SCALE_IN;
	vForceVector[2] *= ZZ_SCALE_IN;

	vis->set_force( vForceVector );
	
	return 1;
}

ZZ_DLL
bool IsAnimatable(HNODE hVisible)
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);
    
    if(vis->is_a(ZZ_RUNTIME_TYPE(zz_animatable)))
		return true;
	return false;


}


ZZ_SCRIPT
int setVisibleJump ( HNODE hVisible, float fJumpForce )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0;

	const float force_coef = 1.0f/ZZ_TICK_PER_SEC;
	vis->set_force( vec3(0, 0, ZZ_SCALE_IN*fJumpForce*force_coef) );
	 
	return 1;
}

ZZ_SCRIPT
float getGravityVelocityZ ( HNODE hVisible )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return 0.0f;

	float distancez;
	{ // engine space
		distancez = znzin->get_linear_gravity() * ZZ_TIME_TO_MSEC(znzin->get_diff_time());
	} 
	// game space
	distancez *= ZZ_SCALE_OUT;

	return distancez;
}

ZZ_SCRIPT
int setAdapter ( int iAdapterIndex )
{
	if (iAdapterIndex < state->num_adapters) {
		state->adapter = iAdapterIndex;
	}
	else {
		return 0;
	}
	return 1;
}

ZZ_SCRIPT
int getNumAdapter ( void )
{
	return state->num_adapters;
}

ZZ_SCRIPT
int setMonitorRefreshRate ( int iRefreshRate )
{
	if ((iRefreshRate < 0) || (iRefreshRate > 100)) return 0;
	state->refresh_rate = iRefreshRate;
	return 1;
}

ZZ_SCRIPT
int getMonitorRefreshRate ( void )
{
	return state->refresh_rate;
}

ZZ_SCRIPT
int dumpResourcesInViewstrum ( int bTerrain, int bOcean, int bOpaque, int bTransparent )
{
	znzin->scene.set_dump_view(ISTRUE(bTerrain), ISTRUE(bOcean), ISTRUE(bOpaque), ISTRUE(bTransparent));
	return 1;
}

ZZ_SCRIPT
ZSTRING readLogString ( int iNumLatest, ZSTRING pStrLineBreakingMsg )
{
	if (znzin && znzin->renderer && znzin->renderer->is_active()) {
		ZZ_LOG("readLogString: availtexmem = [%d/%d]\n",
			znzin->renderer->get_available_texmem()/1000000,
			znzin->renderer->get_max_texmem()/1000000);
	}
	return zz_log::read_latest_log(iNumLatest, pStrLineBreakingMsg);
}

ZZ_SCRIPT
HNODE returnHNODE ( HNODE hNode )
{
	return hNode;
}

ZZ_SCRIPT
ZSTRING getEngineVersion ( void )
{
	zz_assert(znzin);
	return znzin->sysinfo.engine_string;
}

ZZ_DLL
void setRenderState ( ZZ_DWORD dwState, ZZ_DWORD dwValue )
{
	zz_assert(znzin && znzin->renderer);

	zz_renderer * r = znzin->renderer;
	r->set_render_state(static_cast<ZZ_RENDERSTATETYPE>(dwState), dwValue);
}

ZZ_DLL
void setAlphaTest ( int iRefValue )
{
	if ((iRefValue < 0) || (iRefValue > 255)) return;

	zz_assert(znzin && znzin->renderer);

	zz_renderer * r = znzin->renderer;
	r->enable_alpha_test(true, iRefValue);
}

ZZ_SCRIPT
void setMeshType ( HNODE hMesh, int iMeshType )
{
	zz_mesh * mesh = reinterpret_cast<zz_mesh*>(hMesh);

	if (iMeshType == 0) { // static mesh
		mesh->set_dynamic(false);
		mesh->set_bindable(true);
	}
	else if (iMeshType == 1) { // dynamic mesh
		mesh->set_dynamic(true);
		mesh->set_bindable(true);
	}
	else {
		mesh->set_dynamic(false);
		mesh->set_bindable(false);
	}
}

ZZ_DLL
HNODE loadCursor ( ZSTRING pCursorName, HCURSOR hCursorWIN32 )
{
	zz_cursor * cursor =(zz_cursor *)znzin->cursors->spawn(pCursorName, ZZ_RUNTIME_TYPE(zz_cursor), false /* not to autoload */ );

	zz_assert(cursor);
	zz_assert(hCursorWIN32);

	cursor->set_property( hCursorWIN32 );

	znzin->cursors->flush_entrance(cursor); // load immediately

	return reinterpret_cast<HNODE>(cursor);
}

ZZ_DLL
int unloadCursor ( HNODE hCursor )
{
	zz_cursor * cursor = reinterpret_cast<zz_cursor*>(hCursor);
	zz_assert(cursor);

	if (!cursor) return 0;

	znzin->cursors->kill(cursor);

	return 1;
}

ZZ_DLL
int showCursor ( HNODE hCursor )
{
	zz_cursor * cursor = reinterpret_cast<zz_cursor*>(hCursor);

	if (0 == cursor) {
		zz_cursor::hide_cursor();
		return 1;
	}

	cursor->show_cursor(true);

	return 1;
}

ZZ_DLL
int setCursorPosition ( HNODE hCursor, int iX, int iY )
{
	zz_cursor * cursor = reinterpret_cast<zz_cursor*>(hCursor);
	zz_assert(cursor);

	if (!cursor) return 0;

	cursor->set_position(iX, iY);

	return 1;
}

/// 라인 그리기
ZZ_DLL
int drawLine ( float vStart[3], float vEnd[3], ZZ_COLOR Color )
{
	zz_color rgba(Color);
	vec3 pstart(vStart), pend(vEnd);

	pstart *= ZZ_SCALE_IN;
	pend *= ZZ_SCALE_IN;

	static zz_renderer * r = znzin->renderer;
	
	r->draw_line(pstart, pend, vec3(rgba.r, rgba.g, rgba.b));

	return 1;
}

/// 축정렬 박스 그리기
ZZ_DLL
int drawAABB ( float vMin[3], float vMax[3], ZZ_COLOR Color )
{
	vec3 pmin(vMin), pmax(vMax);

	pmin *= ZZ_SCALE_IN;
	pmax *= ZZ_SCALE_IN;

	zz_color rgba(Color);
	draw_aabb(pmin, pmax, vec3(rgba.r, rgba.g, rgba.b));

	return 1;
}

ZZ_DLL
void ScreenFadeInStart(float fade_in_t,float fade_m_t,float fade_out_t,int color_r,int color_g,int color_b)
{

   znzin->screen_sfx.start_fade_inout(fade_in_t,fade_m_t,fade_out_t,color_r,color_g,color_b);

}

ZZ_DLL
void ScreenTransition(int state,float time)
{
   znzin->screen_sfx.start_screen_sfx(state,time);
}


ZZ_DLL
void ObserverCameraTransform(int mouse_xx,int mouse_yy)
{
   znzin->camera_sfx.update_angle(mouse_xx,mouse_yy);   
}

ZZ_DLL
void ObserverCameraZoomInOut(int delta)
{
   znzin->camera_sfx.update_length(delta);
}


ZZ_DLL
void SetObserverCameraOnOff()
{
  znzin->camera_sfx.play_onoff();
}

ZZ_DLL
void SetObserverCameraOnOff2(float xPos, float yPos, float zPos)
{
  znzin->camera_sfx.play_onoff();
  xPos *= ZZ_SCALE_IN;
  yPos *= ZZ_SCALE_IN;
  zPos *= ZZ_SCALE_IN;

  znzin->camera_sfx.Init_observer_Camera(xPos, yPos, zPos);
}

ZZ_DLL
void DrawArrow(float size,int color)
{
	static zz_renderer * r = znzin->renderer;
    r->draw_arrow(size,color);
}

ZZ_DLL
void DrawAxis(float size)
{
	static zz_renderer * r = znzin->renderer;
    r->draw_axis(size);
}

ZZ_DLL
void DrawAxisEx(float *q, float *v, float size)
{
	static zz_renderer_d3d * d3d_renderer;
	d3d_renderer = static_cast<zz_renderer_d3d*>(znzin->renderer);
    
	float vPos[3];
    float vSize;

	vPos[0] = v[0] * ZZ_SCALE_IN;
	vPos[1] = v[1] * ZZ_SCALE_IN;
	vPos[2] = v[2] * ZZ_SCALE_IN;
   
	vSize = size * ZZ_SCALE_IN;

	d3d_renderer->draw_axis(q, vPos, vSize);
}

ZZ_DLL
void DrawOBBBox(float *q, float *v, float xx, float yy, float zz)
{
	static zz_renderer_d3d * r;
	r = static_cast<zz_renderer_d3d*>(znzin->renderer);
    
	vec3 position;
	quat rotation;
    mat4 model_m;
    
	float min_vec[3], max_vec[3];
	
	position.x = v[0] * ZZ_SCALE_IN;
	position.y = v[1] * ZZ_SCALE_IN;
    position.z = v[2] * ZZ_SCALE_IN;

	rotation.x = q[0]; rotation.y = q[1]; rotation.z = q[2]; rotation.w = q[3];

	model_m.set(position, rotation);
    
	min_vec[0] = -xx * ZZ_SCALE_IN; min_vec[1] = -yy * ZZ_SCALE_IN; min_vec[2] = -zz * ZZ_SCALE_IN;
	max_vec[0] = xx * ZZ_SCALE_IN;  max_vec[1] = yy * ZZ_SCALE_IN; max_vec[2] = zz * ZZ_SCALE_IN;

    r->draw_visible_boundingbox(model_m, min_vec, max_vec, D3DCOLOR_COLORVALUE(1.0f,1.0f,0.2f,1.0f));
	


}


ZZ_DLL
void SetOceanSFXOnOff(bool onoff)
{
   znzin->sfx_onoff=onoff;
}

ZZ_DLL
bool GetObserverCameraOnOff()
{
	return  znzin->camera_sfx.get_play_onoff();
}

ZZ_DLL
void StopScreenFadeInOut()
{
	znzin->screen_sfx.stop();	  
}

ZZ_DLL
void PlayWideScreen(float screen_ratio)
{
	if(!znzin->screen_sfx.get_widescreen_mode())
	znzin->screen_sfx.play_widescreen_mode(screen_ratio);
}

ZZ_DLL
void PlayWideScreenEx(int x,int y ,int width,int height)
{
	if(!znzin->screen_sfx.get_widescreen_mode())
	znzin->screen_sfx.play_widescreen_mode(x,y,width,height);
}

ZZ_DLL
void StopWideScreen()
{
	if(znzin->screen_sfx.get_widescreen_mode())
	znzin->screen_sfx.stop_widescreen_mode();	
}

ZZ_DLL
void UserObserverCamera(int bUse)
{

	switch(bUse)
	{
	case 0:
		if(znzin->camera_sfx.get_play_onoff())
		znzin->camera_sfx.play_onoff();
		break;


	case 1:
		if(!(znzin->camera_sfx.get_play_onoff()))
		znzin->camera_sfx.play_onoff();
		
		break;


	default:
		
		
		break;

	}

}

ZZ_DLL
void CameraEffectTest(float Yaw, float Pitch ,float Distance ,float Time)
{
   zz_camera_follow * cam = reinterpret_cast<zz_camera_follow *>(znzin->get_camera());
   
   cam->play_camera_effect(ZZ_TO_RAD*Yaw,ZZ_TO_RAD*Pitch,ZZ_SCALE_IN*Distance,Time);  
}

ZZ_DLL
void InputSceneSphere(float x, float y, float z, float r)
{
  	
    znzin->scene.input_scene_sphere(x * ZZ_SCALE_IN, y * ZZ_SCALE_IN, z * ZZ_SCALE_IN, r * ZZ_SCALE_IN);
}

ZZ_DLL
void InputSceneOBBNode(HNODE hVisible)
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return;
	{
	 
		zz_bvolume *bv = vis->get_bvolume();
		znzin->scene.input_scene_obb(bv->get_obb());
	/*   
		vec3 *minmax;
        float min[3],max[3];
		minmax = vis->get_minmax();
		min[0] = ZZ_SCALE_OUT * minmax[0].x; min[1] = ZZ_SCALE_OUT * minmax[0].y; min[2] = ZZ_SCALE_OUT * minmax[0].z;
		max[0] = ZZ_SCALE_OUT * minmax[1].x; max[1] = ZZ_SCALE_OUT * minmax[1].y; max[2] = ZZ_SCALE_OUT * minmax[1].z;

		znzin->scene.input_scene_aabb(min,max, 0);  //test 12-7
      */           	    
	} 
}

ZZ_DLL
void InputSceneAABB(float vMin[3], float vMax[3], ZZ_COLOR Color)
{
	znzin->scene.input_scene_aabb(vMin, vMax, Color);
}

ZZ_DLL
void InputSceneCylinder(float x, float y, float z, float length, float r)
{
	znzin->scene.input_scene_cylinder(x, y, z, length, r);
}

ZZ_DLL
void InputSceneLine(float Vec1[3], float Vec2[3])
{
	vec3 vec1(Vec1);
	vec3 vec2(Vec2);
	
	znzin->scene.input_scene_line(vec1, vec2);
}

ZZ_DLL
void InputSceneOBB2(float Center[3], float RotationWXYZ[4], float xLength, float yLength, float zLength)
{
	vec3 center(Center);  
    quat rotation;

	rotation.w = RotationWXYZ[0];
	rotation.x = RotationWXYZ[1];
	rotation.y = RotationWXYZ[2];
	rotation.z = RotationWXYZ[3];
    
	znzin->scene.input_scene_obb2(center, rotation, xLength, yLength, zLength); 
}

ZZ_DLL
void InputSceneAxis(HNODE hVisible, float Size)
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hVisible);

	if (!vis) return;
	{
		znzin->scene.input_scene_axis(vis,Size * ZZ_SCALE_IN);
	} 
}


ZZ_DLL
void ResetSceneSphere()
{
	 znzin->scene.reset_scene_sphere();
}

ZZ_DLL
void ResetSceneOBB()
{
	znzin->scene.reset_scene_obb();
}

ZZ_DLL
void ResetSceneAABB()
{

	znzin->scene.reset_scene_aabb();
}

ZZ_DLL
void ResetSceneCylinder()
{
	znzin->scene.reset_scene_cylinder(); 
}

ZZ_DLL
void ResetSceneLine()
{
	znzin->scene.reset_scene_line(); 
}

ZZ_DLL
void ResetSceneAxis()
{
	znzin->scene.reset_scene_axis();
}

ZZ_DLL
void DrawAnimatableMovingPath( HNODE hNODE )
{
	zz_visible * vis = reinterpret_cast<zz_visible*>(hNODE);
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer);

    
    if(!vis->is_a(ZZ_RUNTIME_TYPE(zz_animatable)))
		return;

	zz_animatable *ani = reinterpret_cast<zz_animatable*>(hNODE);
    
	
	if( ani != NULL)
	{
		zz_motion *motion = ani->get_motion();
        uint32 channel_type;
        int position_channel;
		
		if(motion != NULL)
		{
			mat4 parentTM = ani->get_worldTM();
			
			int num_channels = motion->get_num_channels();
            
			if(num_channels == 0)
				return;
			
			int  num_frames = motion->get_num_frames();

			if(num_frames == 0 )
				return;

			position_channel = -1;
			for(int channel_index = 0; channel_index < num_channels; channel_index++)
			{
				channel_type = motion->get_channel_type(channel_index);
				if(channel_type == ZZ_CTYPE_POSITION)
					position_channel = channel_index;
			}

			if(position_channel == -1)
				return;
		
			for(int channel_index = 0; channel_index < num_channels; channel_index++ )
			{
				channel_type = motion->get_channel_type(channel_index);
   

				if(channel_type == ZZ_CTYPE_POSITION)
				{
					vec3 position1(0.0f, 0.0f, 0.0f), position2(0.0f, 0.0f, 0.0f);
					quat rotation(0.0f, 0.0f, 0.0f, 1.0f);
							
					
					for(int frame_index = 0; frame_index < (num_frames - 1); frame_index ++)
					{
						motion->get_channel_data(channel_index, frame_index, (void *)&position1);
						motion->get_channel_data(channel_index, frame_index + 1, (void *)&position2);
                        
						position1.x += 5200.0f;	position2.x += 5200.0f;
						position1.y += 5200.0f;	position2.y += 5200.0f;
						r->draw_line(position1, position2, vec3(0.0f, 0.5f, 1.0f));
					
					    
					}

				}
				else if(channel_type == ZZ_CTYPE_ROTATION)
				{			
				/*	vec3 position(0.0f, 0.0f, 0.0f);
					quat rotation(0.0f, 0.0f, 0.0f, 1.0f);
					mat4 mat;
			
					
					for(int frame_index = 0; frame_index < num_frames ; frame_index ++)
					{
						motion->get_channel_data(channel_index, frame_index, (void *)&rotation);
						motion->get_channel_data(position_channel, frame_index, (void *)&position);
                        
						position.x += 5200.0f;	position.y += 5200.0f;	
			
						mat.set(position, rotation);
						r->draw_axis_object(mat, 10.0f);
					}
				*/	
				}


			}
		}


	}

}

ZZ_DLL
void DrawCameraMovingPath( HNODE hMotion, float vPosition[3] , float OutPosition[3], float OutRotation[4])
{
	zz_motion *motion = reinterpret_cast<zz_motion *>(hMotion);
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer);

	vec3 position1, position2, position;
	
    position.x = vPosition[0] * 0.01f;
	position.y = vPosition[1] * 0.01f;
	position.z = vPosition[2] * 0.01f;

	if(!motion)
	{
		ZZ_LOG("interface: DrawCameraMovingPath() failed\n");
		return;
	}
	
	int num_channels = motion->get_num_channels();
    if(num_channels == 0)
		return;
	
	int	num_frames = motion->get_num_frames();
    if(num_frames == 0 )
		return;

	for(int frame_index = 0; frame_index < (num_frames - 1) ; frame_index += 1)
	{
		motion->get_channel_data(0, frame_index, (void *)&position1);
        motion->get_channel_data(0, frame_index + 1, (void *)&position2);

		position1 += position;
		position2 += position;

		r->draw_line(position1, position2, vec3(0.0f, 0.5f, 1.0f));
	
	
	   if(frame_index == 0 )
	   {
		   r->draw_wire_sphere(position1.x, position1.y, position1.z, 0.2f);
	   }

	   if(frame_index ==  (num_frames - 2))
	   {
		    r->draw_wire_sphere(position2.x, position2.y, position2.z, 0.2f);
	   }
	
	}

	static zz_time current_time = 0;
    zz_time total_time = (4800 *(motion->get_num_frames() - 1))/(motion->get_fps()); 
    int fps = motion->get_fps();
	vec3 eye, center, up;
    vec3 direction, right;
    mat4 mat;	
	
	motion->get_channel_data(0, current_time, (void *)&eye, fps);
    motion->get_channel_data(1, current_time, (void *)&center, fps);
    motion->get_channel_data(2, current_time, (void *)&up, fps);
    
	direction = center - eye;
	normalize(direction);
    
	cross(right, direction, up);
	normalize(right);

    cross(up, right, direction);
	normalize(up);

    mat._11 = right.x; mat._21 = right.y; mat._31 = right.z; mat._41 = 0.0f;
    mat._12 = up.x; mat._22 = up.y; mat._32 = up.z; mat._42 = 0.0f;
    mat._13 = -direction.x; mat._23 = -direction.y; mat._33 = -direction.z; mat._43 = 0.0f;
    mat._14 = eye.x + position.x; mat._24 = eye.y + position.y; mat._34 = eye.z + position.z; mat._44 = 1.0f;

	r->draw_camera_ex(mat);
	current_time += znzin->get_diff_time();
	
	if(current_time > total_time)
	{
		current_time -= total_time;
	}

    vec3 buffer_position;
	quat buffer_rotation;
	
	buffer_position = mat.get_position();
	buffer_rotation = mat.get_rotation();

	OutPosition[0] = buffer_position.x; OutPosition[1] = buffer_position.y; OutPosition[2] = buffer_position.z;
    OutRotation[0] = buffer_rotation.x; OutRotation[1] = buffer_rotation.y; OutRotation[2] = buffer_rotation.z; OutRotation[3] = buffer_rotation.w;

}


ZZ_DLL
void DrawCameraMovingPathOnly( HNODE hMotion, float vPosition[3])
{
	zz_motion *motion = reinterpret_cast<zz_motion *>(hMotion);
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer);

	vec3 position1, position2, position;
	
    position.x = vPosition[0] * 0.01f;
	position.y = vPosition[1] * 0.01f;
	position.z = vPosition[2] * 0.01f;

	if(!motion)
	{
		ZZ_LOG("interface: DrawCameraMovingPath() failed\n");
		return;
	}
	
	int num_channels = motion->get_num_channels();
    if(num_channels == 0)
		return;
	
	int	num_frames = motion->get_num_frames();
    if(num_frames == 0 )
		return;

	for(int frame_index = 0; frame_index < (num_frames - 1) ; frame_index += 1)
	{
		motion->get_channel_data(0, frame_index, (void *)&position1);
        motion->get_channel_data(0, frame_index + 1, (void *)&position2);

		position1 += position;
		position2 += position;

		r->draw_line(position1, position2, vec3(0.5f, 0.0f, 1.0f));
	
	
	   if(frame_index == 0 )
	   {
		   r->draw_wire_sphere(position1.x, position1.y, position1.z, 0.2f);
	   }

	   if(frame_index ==  (num_frames - 2))
	   {
		    r->draw_wire_sphere(position2.x, position2.y, position2.z, 0.2f);
	   }
	
	}

}

ZZ_SCRIPT
void setRendererMinFilter ( int iFilter )
{
	zz_assert(znzin);
	zz_renderer * r = znzin->renderer;
	zz_assert(r);

	r->set_sampler_state(0, ZZ_SAMP_MINFILTER, zz_render_state::zz_texture_filter_type(iFilter));
}

ZZ_SCRIPT
void setRendererMagFilter ( int iFilter )
{
	zz_assert(znzin);
	zz_renderer * r = znzin->renderer;
	zz_assert(r);

	r->set_sampler_state(0, ZZ_SAMP_MAGFILTER, zz_render_state::zz_texture_filter_type(iFilter));
}

ZZ_DLL
void drawSpriteSFX()
{
	zz_assert(znzin);
	if(znzin->sfx_onoff)
	znzin->sfxs->render_sfx();
}

ZZ_DLL
void setAvatarViewPort(float x, float y, float width, float height)
{
	zz_assert(znzin);
	znzin->avatar_selection_sfx.set_avatar_selection_viewport(x, y, width, height);
    znzin->avatar_selection_sfx.change_avatar_viewport();
	znzin->avatar_selection_sfx.clear_scene();
    znzin->avatar_selection_sfx.calculate_avatar_projection_matrix();
	znzin->avatar_selection_sfx.calculate_avatar_view_matrix();
	znzin->avatar_selection_sfx.change_avatar_graphicpipeline();
}

ZZ_DLL
void setMovingCameraViewPort(int x, int y, int width, int height)
{
	zz_assert(znzin);
	znzin->moving_camera_sfx.set_camera_screen_viewport(x, y, width, height);
    znzin->moving_camera_sfx.change_camera_viewport();
	znzin->moving_camera_sfx.clear_scene();
	znzin->moving_camera_sfx.calculate_camera_projection_matrix();
	znzin->moving_camera_sfx.change_camera_graphicpipeline();
}

ZZ_DLL
void setMovingCameraDefaultViewPort()
{
	zz_assert(znzin);
	znzin->moving_camera_sfx.change_default_graphicpipeline();
	znzin->moving_camera_sfx.change_default_viewport();
}

ZZ_DLL
void InputMovingCameraMatrix(float vPosition[3], float vRotation[4])
{
	zz_assert(znzin);
    vec3 position;
	quat rotation;

	position.x = vPosition[0]; position.y = vPosition[1]; position.z = vPosition[2];
	rotation.x = vRotation[0]; rotation.y = vRotation[1]; rotation.z = vRotation[2]; rotation.w = vRotation[3];

	znzin->moving_camera_sfx.Input_camera_matrix(position, rotation);
}

ZZ_DLL
void setDefaultViewPort()
{
	zz_assert(znzin);
	znzin->avatar_selection_sfx.change_default_graphicpipeline();
	znzin->avatar_selection_sfx.change_default_viewport();
}

ZZ_DLL
void updateAvatarSelectionCameraLength(float step)
{
	zz_assert(znzin);
	znzin->avatar_selection_sfx.view_length += step;
}

ZZ_DLL
void updateAvatarSelectionCameraSeta(float step)
{
	zz_assert(znzin);
	znzin->avatar_selection_sfx.view_seta += step;
}

ZZ_DLL
void updateAvatarSelectionCameraHeight(float step)
{
	zz_assert(znzin);
	znzin->avatar_selection_sfx.view_height += step;
}

ZZ_DLL
void RenderSelectedAvatar(HNODE hModel)
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	zz_model * model = reinterpret_cast<zz_model *>(hModel);
	
	zz_assert(model);
	if (!model) {
		return ;
	}
	
	vec3 pos1, pos2, pos3;
	quat quat1, quat2;
	
	pos2.x = pos2.y = 0.0f; pos2.z = -1.4f; 
    quat2.x = quat2.y = quat2.z =0.0f; quat2.w = 1.0f;

	pos1 = model->get_position();
	quat1 = model->get_rotation();
	
	if (false/*znzin->sky*/) {
		
		pos3 = znzin->sky->get_position();
		znzin->sky->set_position(pos2);
		znzin->sky->invalidate_transform();
		znzin->sky->set_ztest(false);
		znzin->sky->render(true);
	    znzin->sky->set_position(pos3);
        znzin->sky->invalidate_transform();
	}
	
//	znzin->avatar_selection_sfx.draw_background();
	
	zz_time diff_time = znzin->get_diff_time();
	
	
	model->invalidate_transform();
	model->set_position(pos2);
	model->set_rotation_local(quat2);
	model->set_infrustum(true);
	model->update_animation(true, diff_time);
	model->render(true);
    model->set_position(pos1);
	model->set_rotation_local(quat1);
	model->invalidate_transform();
    model->set_infrustum(false);
}

ZZ_DLL
void LoadSelectedAvatarBackGroundTexture(ZSTRING pTexFileName)
{
	znzin->avatar_selection_sfx.set_texture(pTexFileName);
}

ZZ_DLL
void InputForcedMovingCameraElement(float *vPosition, float *vRotation, float time)
{
	zz_camera * cam = znzin->get_camera();
    
	vec3 position;
    quat rotation;

	position.x = vPosition[0] * ZZ_SCALE_IN;
	position.y = vPosition[1] * ZZ_SCALE_IN;
	position.z = vPosition[2] * ZZ_SCALE_IN;

    rotation.x = vRotation[0];
	rotation.y = vRotation[1];
	rotation.z = vRotation[2];
    rotation.w = vRotation[3];


	cam->input_forced_transformation_element(position, rotation, time);
}

ZZ_DLL
void setForcedMoveCamera(float *vPosition, float *vRotation)
{
	zz_camera * cam = znzin->get_camera();
    
	vec3 position;
    quat rotation;

	position.x = vPosition[0] * ZZ_SCALE_IN;
	position.y = vPosition[1] * ZZ_SCALE_IN;
	position.z = vPosition[2] * ZZ_SCALE_IN;

    rotation.x = vRotation[0];
	rotation.y = vRotation[1];
	rotation.z = vRotation[2];
    rotation.w = vRotation[3];


	cam->set_forced_transformation(position, rotation);
}



ZZ_DLL
void InputForcedMovingCameraSystemElement(float *vPosition, float *vRotation, float *vElement, float time)
{
	zz_camera * cam = znzin->get_camera();
    
	vec3 position;
    quat rotation;

	position.x = vPosition[0] * ZZ_SCALE_IN;
	position.y = vPosition[1] * ZZ_SCALE_IN;
	position.z = vPosition[2] * ZZ_SCALE_IN;

    rotation.x = vRotation[0];
	rotation.y = vRotation[1];
	rotation.z = vRotation[2];
    rotation.w = vRotation[3];


	cam->input_forced_transformation_system_element(position, rotation, vElement, time);

}

ZZ_DLL
bool IsForcedMovingCameraState()
{
	zz_camera * cam = znzin->get_camera();
    
	return cam->forced_transformation;
}

ZZ_DLL
void CulledModelAnimationRevision(HNODE hModel)
{
	zz_model * model = reinterpret_cast<zz_model *>(hModel);
	zz_time diff_time = znzin->get_diff_time();
	
	if(!model->get_infrustum())
	{
		model->set_infrustum(true);
		model->update_animation(true, diff_time);
		model->set_infrustum(false);
	}
    
}

ZZ_DLL
bool GetMotionInterpolation ()
{
	return znzin->get_use_motion_interpolation();
}

ZZ_DLL
void getAnimatableMotionTime ( HNODE hAnimatable, float *currentTime, float *totalTime )
{
	CHECK_INTERFACE(getAnimatableState);
	zz_animatable * ani = reinterpret_cast<zz_animatable*>(hAnimatable);
    zz_motion *motion;
    
	
	*currentTime = 0.0f;
    *totalTime = -1.0f;

	if(ani->get_motion_state() == 0)
		return;
    
	motion = ani->get_motion();

	*totalTime = motion->get_total_time() / 4800.0f;
    *currentTime = ani->get_motion_time() / 4800.0f;
}

ZZ_DLL
void SetFogOffMode(bool fogState)
{
	znzin->scene.fog_onoff_mode = fogState;
}

ZZ_DLL
int getLight (HNODE hLight, ZSTRING pProperty, float *fValue )
{	
	zz_light * light = reinterpret_cast<zz_light *>(hLight);

	zz_assert(light);
	if (!light) return 0;

	if (strcmp("ambient", pProperty) == 0) {
		
		fValue[0] = light->ambient.r;
        fValue[1] = light->ambient.g;
		fValue[2] = light->ambient.b;
	}
	else if (strcmp("diffuse", pProperty) == 0) {
		
		fValue[0] = light->diffuse.r;
        fValue[1] = light->diffuse.g;
		fValue[2] = light->diffuse.b;
	}
	else if (strcmp("specular", pProperty) == 0) {
		
		fValue[0] = light->specular.r;
        fValue[1] = light->specular.g;
		fValue[2] = light->specular.b;
	}
	else if (strcmp("direction", pProperty) == 0) {
		if (!light->is_a(ZZ_RUNTIME_TYPE(zz_light_direct))) {
			ZZ_LOG("interface: setLight(%s:direction) failed. It is not a directional light.\n", light->get_name());
			return 0;
		}

		zz_light_direct * directional_light = static_cast<zz_light_direct*>(light);

		fValue[0] = directional_light->direction.x;
        fValue[1] = directional_light->direction.y;
		fValue[2] = directional_light->direction.z;
		
			}
	else if (strcmp("position", pProperty) == 0) {
		
		
		fValue[0] = light->position.x * ZZ_SCALE_OUT;
        fValue[1] = light->position.y * ZZ_SCALE_OUT;
		fValue[2] = light->position.z * ZZ_SCALE_OUT;
	}
	else if (strcmp("attenuation", pProperty) == 0) {
		if (!light->is_a(ZZ_RUNTIME_TYPE(zz_light_point))) {
			ZZ_LOG("interface: setLight(%s:attenuation) failed. It is not a point light.\n", light->get_name());
			return 0;
		}
		
	}

	return 1;

}

ZZ_DLL
void GetObserverCameraPosXY(float *xPos, float *yPos)
{
	znzin->camera_sfx.update_time(xPos, yPos);
    *xPos *= ZZ_SCALE_OUT;
	*yPos *= ZZ_SCALE_OUT;

}

ZZ_DLL
void SetObserverCameraPosZ(float zPos)
{
	zPos *= ZZ_SCALE_IN;
	znzin->camera_sfx.set_observer_zpos(zPos);
}

ZZ_DLL
void SetObserverCameraSpeed(float fSpeed)
{
	znzin->camera_sfx.update_speed(fSpeed);
}

ZZ_DLL
void SetObserverCameraBaseRotation(float fAngle)
{
	znzin->camera_sfx.update_base_angle(fAngle);
}

ZZ_DLL

void SetObserverCameraRotationPhi(float fAngle)
{
	znzin->camera_sfx.update_view_angle(fAngle);
}

ZZ_DLL
void SetObserverCameraSideSpeed(float fSpeed)
{
	znzin->camera_sfx.update_side_speed(fSpeed);
}

ZZ_DLL
void SetObserverCameraHeightSpeed(float fSpeed)
{
	znzin->camera_sfx.update_height_speed(fSpeed);
}

ZZ_DLL
void SetModelCameraCollisionOnOff(HNODE hModel, bool bCollisionOnOff)
{
	zz_model * model = reinterpret_cast<zz_model *>(hModel);
	model->camera_collision_onoff = bCollisionOnOff;
}

ZZ_DLL
void SetForcedMotionMixOff(HNODE hModel, bool bOnOff)
{
	zz_model * model = reinterpret_cast<zz_model *>(hModel);
	model->fored_motion_blend0ff = bOnOff;
}

