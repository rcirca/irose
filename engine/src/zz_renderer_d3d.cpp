/** 
 * @file zz_renderer_d3d.cpp
 * @brief renderer Direct 3D driver.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    21-feb-2002
 *
 * $Header: /engine/src/zz_renderer_d3d.cpp 223   06-01-03 10:41a Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_font_d3d.h"
#include "zz_renderer_d3d.h"
#include "zz_view_d3d.h"
#include "zz_log.h"
#include "zz_type.h"
#include "zz_mesh.h"
#include "zz_system.h"
#include "zz_material_colormap.h"
#include "zz_manager.h"
#include "zz_vfs.h"
#include "zz_camera.h"
#include "zz_shader.h"
#include "zz_profiler.h"
#include "zz_model.h"
#include "zz_morpher.h"
#include "zz_light_direct.h"
#include "zz_light_point.h"
#include "zz_texture.h"
#include "zz_misc.h"
#include "zz_error.h"
#include "zz_trail.h"

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
#include "zz_vfs_thread.h"
#endif

#include <windows.h>

#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

typedef struct
{
	D3DXVECTOR4 p;
	FLOAT       tu, tv;

} TOVERLAY_VERTEX;

struct MYLINEVERTEX {
			D3DXVECTOR4 pos;
			D3DCOLOR diffuse;
		};

typedef struct 
{
    vec4 position;        
	D3DCOLOR    color;
	vec2 uv;
} VERTEX_SPRITE;
#define D3DFVF_SPRITE (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)


#define TOVERLAY_VERTEX_FVF (D3DFVF_XYZRHW | D3DFVF_TEX1)

#define SHADOWOVER_TEXTURE_BLACK		"etc/shadowover_black.dds"
#define SHADOWOVER_TEXTURE_WHITE		"etc/shadowover.dds"

#define NUM_GLOW_BLUR 8
#define NUM_SHADOW_BLUR 8

#define ADAPTER_FORMAT32 D3DFMT_X8R8G8B8
#define ADAPTER_FORMAT16 D3DFMT_R5G6B5
#define RENDERTARGET_FORMAT32 D3DFMT_A8R8G8B8
#define RENDERTARGET_FORMAT16 D3DFMT_R5G6B5 // D3DFMT_R5G6B5 // D3DFMT_A4R4G4B4, D3DFMT_R5G6B5 
#define BACKBUFFER_FORMAT32 D3DFMT_A8R8G8B8
#define BACKBUFFER_FORMAT16 D3DFMT_R5G6B5 // Most cards does not support D3DFMT_R5G6B5 backbuffer.
#define DEPTH_STENCIL_FORMAT D3DFMT_D16 // D3DFMT_D24S8
#define ZZ_USECACHE // whether use shader/texture caching mechanism

vec3 * shadowmap_pixels = NULL; // for shadowmap

D3DXMATRIX zz_renderer_d3d::s_mat_id_d3d (1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);

// nvidia perfHUD related.
bool s_nvperf_found = false;
int s_nvperf_adapter = -1;

//#define LOG_TEXTURES

#ifdef LOG_TEXTURES
#include <algorithm>
std::vector<zz_texture *> s_textures;
#endif

ZZ_IMPLEMENT_DYNCREATE(zz_renderer_d3d, zz_renderer)

zz_renderer_d3d::zz_renderer_d3d () : zz_renderer()
{
	view = NULL;
	d3d = NULL;
	d3d_device = NULL;
	backbuffer_zsurface = NULL;
	backbuffer_surface = NULL;
	shadowmap = NULL;
	shadowmap_surface = NULL;
	shadowmap_forblur = NULL;
	shadowmap_forblur_surface = NULL;
	light_camera = NULL;
	saved_camera = NULL;
	gamma_saved = false;
	_scene_began = false;
	normalization_cubemap = NULL;
	adapter_format = ADAPTER_FORMAT32;
	rendertarget_format = RENDERTARGET_FORMAT32;
	depthstencil_format = DEPTH_STENCIL_FORMAT;
	backbuffer_format = BACKBUFFER_FORMAT32;
	num_polygons_ = 0;
	num_meshes_ = 0;
	min_texture_memory = 999999999;
	max_texture_memory = 0;	
	line_vertex_buffer = NULL;
	shadowmap_overlay_texture = NULL;
	render_where = ZZ_RW_NONE;
	overlay_vb = NULL;
	shadowmap_vb = NULL;
	glow_vb = NULL;
	_device_lost = false;
	v_backbuffer_texture = NULL;
	v_backbuffer_zsurface = NULL;
	v_backbuffer_surface = NULL;
	glow_backbuffer_surface = NULL;
	glow_backbuffer_texture = NULL;
	glow_downsample_texture = NULL;
	glow_downsample_surface = NULL;
	glow_blur_texture_fullscene = NULL;
	glow_blur_surface_fullscene = NULL;
	glow_blur_texture = NULL;
	glow_blur_surface = NULL;
	_sprite_name[0] = 0;
    sprite_vertexbuffer_cover = NULL;
	sprite_vertexbuffer_origin = NULL;
	sprite_vertexbuffer_origin_ex = NULL;
	boundingbox_vertexbuffer = NULL;
	boundingbox_indexbuffer = NULL;
	
	use_virtual_backbuffer = false;
	d3d_sprite = NULL;
	_sprite_began = false;
	
	sphere_buffer = NULL;
	cylinder_buffer = NULL;

	// d3d must be released in cleanup()
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if (!d3d) {
		ZZ_LOG("r_d3d: Cannot create Direct3D object.!\n");
		zz_msgbox(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::CREATE_D3D_FAIL));
		
		strcat(znzin->sysinfo.video_string," <D3D Device failed>");
		
		zz_msgboxf(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::MSG_EXIT_D3DERROR), znzin->sysinfo.video_string);
		exit(EXIT_FAILURE);
	}

	state.num_adapters = get_num_adapters();
	//ZZ_LOG("r_d3d: zz_renderer_d3d created\n");
}

zz_renderer_d3d::~zz_renderer_d3d ()
{
	cleanup();
	SAFE_RELEASE(d3d); // created in initialize()
	//ZZ_LOG("r_d3d: zz_renderer_d3d destroyed\n");
}

zz_render_state * zz_renderer_d3d::get_state (void)
{
	return &state;
}

void zz_renderer_d3d::log_adapter_identifier ( UINT ordinal_in, const D3DADAPTER_IDENTIFIER9& id )
{
	WORD Product = HIWORD(id.DriverVersion.HighPart);
	WORD Version = LOWORD(id.DriverVersion.HighPart);
	WORD SubVersion = HIWORD(id.DriverVersion.LowPart);
	WORD Build = LOWORD(id.DriverVersion.LowPart);
	if (ordinal_in == 0) {
		sprintf(znzin->sysinfo.video_string, "%s(%d.%d.%d.%d)",
			id.Description, Product, Version, SubVersion, Build);
	}

	ZZ_LOG("r_d3d: adapter(%d): desc(%s), version(%d.%d.%d.%d)\n",
		ordinal_in, id.Description, Product, Version, SubVersion, Build);
}

int zz_renderer_d3d::get_num_adapters ()
{
	if (!d3d) {
		ZZ_LOG("r_d3d: get_num_adapters() failed. d3d not initialized\n");
		return 0;
	}
	return d3d->GetAdapterCount();
}

bool zz_renderer_d3d::find_adapter_ordinal (
	D3DDEVTYPE& d3d_dev_type_out,
	UINT& adapter_ordinal_out,
	bool use_fullscreen_in,
	D3DFORMAT adapter_format_in,
	D3DFORMAT backbuffer_format_in,
	UINT width_in,
	UINT height_in,
	UINT refresh_rate_in)
{
	UINT num_adapters = state.num_adapters;
	D3DADAPTER_IDENTIFIER9 adapter_identifier;

	UINT best_ordinal = 0;
	UINT best_refresh_rate = 0;
	int best_bpp = 0;
	int best_mode = 0;
	D3DDISPLAYMODE checked_display_mode;
	bool found = false;
	HRESULT hresult;
	
	s_nvperf_found = false;
	s_nvperf_adapter = -1;

	for (adapter_ordinal_out = 0; adapter_ordinal_out < num_adapters; adapter_ordinal_out++) {
		hresult = d3d->GetAdapterIdentifier(adapter_ordinal_out, 0, &adapter_identifier);

		if (s_nvperf_found) return found; // all done

		if (FAILED(hresult)) {
			ZZ_LOG("r_d3d: find_adapter_ordinal() faild. %s", get_hresult_string(hresult));
			continue;
		}

		//ZZ_LOG("r_d3d: find_adapter_ordinal(%d/%d). (%dx%d-%s-%dhz). format(%d,%d).\n",
		//	adapter_ordinal_out,
		//	num_adapters,
		//	width_in, height_in,
		//	use_fullscreen_in ? "full" : "window",
		//	refresh_rate_in,
		//	adapter_format_in, backbuffer_format_in);

		log_adapter_identifier(adapter_ordinal_out, adapter_identifier);
		
		UINT num_adapter_modes = d3d->GetAdapterModeCount( adapter_ordinal_out, adapter_format_in );

#if (0) // to not using nVidia PerfHUD
		if (strcmp(adapter_identifier.Description,"NVIDIA NVPerfHUD") == 0)
		{
			s_nvperf_found = true;
			s_nvperf_adapter = adapter_ordinal_out;
			d3d_dev_type_out = D3DDEVTYPE_REF;
			ZZ_LOG("r_d3d: find_adapter_ordinal(%d). NVIDIA NVPerfHUD found.\n", s_nvperf_adapter);
		}
#endif

		for (UINT mode = 0; mode < num_adapter_modes; mode++) {
			if (FAILED(d3d->EnumAdapterModes( adapter_ordinal_out, adapter_format_in, mode, &checked_display_mode ))) {
				ZZ_LOG("r_d3d: find_adapter_ordinal:enum_adapter_modes(%d, %d, %d) failed\n",
					adapter_ordinal, adapter_format_in, mode);
				continue;
			}

			int bpp=0;
			switch (checked_display_mode.Format) {
				case D3DFMT_X1R5G5B5: bpp=16; break;
				case D3DFMT_R5G6B5: bpp=16; break;
				case D3DFMT_A1R5G5B5: bpp=16; break;
				case D3DFMT_A4R4G4B4: bpp=16; break;
				case D3DFMT_X4R4G4B4: bpp=16; break;
				case D3DFMT_R8G8B8: bpp=24; break;
				case D3DFMT_X8R8G8B8: bpp=32; break;
				case D3DFMT_A8R8G8B8: bpp=32; break;
			}			

			zz_assert(checked_display_mode.Format == adapter_format_in);

			//if (mode == 0) { // sample one
			//	ZZ_LOG("r_d3d: check display - adapter(%d)-mode(%d)-(%dx%dx%d)-%dHz\n",
			//		adapter_ordinal_out, mode, checked_display_mode.Width, checked_display_mode.Height, bpp, checked_display_mode.RefreshRate);
			//}

			if ((checked_display_mode.Format != adapter_format_in) ||
				(checked_display_mode.Width != width_in) ||
				(checked_display_mode.Height != height_in))				
			{
				//ZZ_LOG("r_d3d: not matching format(%dx%d-%d)\n", width_in, height_in, adapter_format);
				continue;
			}
			
			if ((refresh_rate_in > 0) && (checked_display_mode.RefreshRate != refresh_rate_in)) {
				//ZZ_LOG("r_d3d: not matching refreshrate(%d)\n", refresh_rate_in);
				continue;
			}
			
			// check backbuffer format
			if (FAILED(d3d->CheckDeviceType(
				adapter_ordinal_out,
				d3d_dev_type_out, 
				adapter_format_in,
				backbuffer_format_in,
				use_fullscreen_in
				)))
			{
				//ZZ_LOG("r_d3d: not supported device.\n");
				continue;
			}

			//ZZ_LOG("r_d3d: found display: adapter(%d)-mode(%d)-(%dx%dx%d-%s)-%dHz\n",
			//	adapter_ordinal_out, mode, checked_display_mode.Width, checked_display_mode.Height, bpp, 
			//	use_fullscreen_in ? "full" : "window",
			//	checked_display_mode.RefreshRate);

			found = true;

			if (s_nvperf_found || (checked_display_mode.RefreshRate >= best_refresh_rate)) {
				best_ordinal = adapter_ordinal_out;
				best_refresh_rate = checked_display_mode.RefreshRate;
				best_bpp = bpp;
				best_mode = mode;

				//ZZ_LOG("r_d3d: best display: adapter(%d)-mode(%d)-(%dx%dx%d-%s)-%dHz\n",
				//	adapter_ordinal_out, mode, checked_display_mode.Width, checked_display_mode.Height, bpp, 
				//	use_fullscreen_in ? "full" : "window",
				//	checked_display_mode.RefreshRate);
			}
		}
	}

	if (found) {
		ZZ_LOG("r_d3d: best adapter ordinal = adapter(%d)-mode(%d)-(%dx%dx%d-%s)-(%d)Hz.\n",
			best_ordinal, best_mode, width_in, height_in, best_bpp, use_fullscreen_in ? "full" : "window",
			best_refresh_rate);
		adapter_ordinal_out = best_ordinal;
	}
	else {
		ZZ_LOG("r_d3d: not found available adapter ordinal. use default.\n");
		adapter_ordinal_out = D3DADAPTER_DEFAULT;
	}
	return found;
}

const char * zz_renderer_d3d::get_hresult_string (HRESULT hr)
{
	switch (hr) {
		case D3DERR_INVALIDCALL:
			return "D3DERR_INVALIDCALL";
			break;
		case D3DERR_NOTAVAILABLE:
			return "D3DERR_NOTAVAILABLE";
			break;
		case D3DERR_DEVICELOST:
			return "DEVICELOST";
			break;
		case D3DERR_DEVICENOTRESET:
			return "D3DERR_DEVICENOTRESET";
			break;
		case D3DERR_DRIVERINTERNALERROR:
			return "DRIVERINTERNALERROR";
			break;
		case D3DERR_OUTOFVIDEOMEMORY:
			return "D3DERR_OUTOFVIDEOMEMORY";
			break;
		case E_OUTOFMEMORY:
			return "E_OUTOFMEMORY";
			break;
		case D3DERR_CONFLICTINGRENDERSTATE:
			return "D3DERR_CONFLICTINGRENDERSTATE";
			break;
		case D3DERR_CONFLICTINGTEXTUREFILTER:
			return "D3DERR_CONFLICTINGTEXTUREFILTER";
			break;
		case D3DERR_TOOMANYOPERATIONS:
			return "D3DERR_TOOMANYOPERATIONS";
			break;
		case D3DERR_UNSUPPORTEDALPHAARG:
			return "D3DERR_UNSUPPORTEDALPHAARG";
			break;
		case D3DERR_UNSUPPORTEDALPHAOPERATION:
			return "D3DERR_UNSUPPORTEDALPHAOPERATION";
			break;
		case D3DERR_UNSUPPORTEDCOLORARG:
			return "D3DERR_UNSUPPORTEDCOLORARG";
			break;
		case D3DERR_UNSUPPORTEDCOLOROPERATION:
			return "D3DERR_UNSUPPORTEDCOLOROPERATION";
			break;
		case D3DERR_UNSUPPORTEDFACTORVALUE:
			return "D3DERR_UNSUPPORTEDFACTORVALUE";
			break;
		case D3DERR_UNSUPPORTEDTEXTUREFILTER:
			return "D3DERR_UNSUPPORTEDTEXTUREFILTER";
			break;
		case D3DERR_WRONGTEXTUREFORMAT:
			return "D3DERR_WRONGTEXTUREFORMAT";
			break;
		default:
			return "UNKNOWN";
	}
}

bool zz_renderer_d3d::check_rendertarget32 ()
{
	if (FAILED(d3d->CheckDeviceFormat(
		adapter_ordinal,
		D3DDEVTYPE_HAL,
		adapter_format,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		RENDERTARGET_FORMAT32)))
	{
		return false;
	}
	return true;
}


bool zz_renderer_d3d::check_shadowable ()
{
	if (device_capability.MaxSimultaneousTextures == 3) { // ex) Matrox G400 millenium
		// forcing not to use shadowmap
		ZZ_LOG("r_d3d: maxsimtex = 3. ignoring shadowmap.\n");
		return false;
	}
	if (!check_rendertarget32()) {
		ZZ_LOG("r_d3d: rendertarget32 not support.\n");
		return false;
	}
	return true;
}

bool zz_renderer_d3d::check_glowable ()
{
	zz_assert(has_device());

	if (get_num_simultaneous_render_target() < 1) {
		ZZ_LOG("r_d3d: num_simultaneous_render_target < 1. check_glowable() failed.\n");
		return false;
	}

	if (FAILED(d3d->CheckDeviceFormat(
		adapter_ordinal,
		D3DDEVTYPE_HAL,
		adapter_format,
		D3DUSAGE_RENDERTARGET,
		D3DRTYPE_TEXTURE,
		RENDERTARGET_FORMAT32)))
	{
		ZZ_LOG("r_d3d: not support 32format rendertarget texture.\n");
		return false;
	}
	
	if ((device_capability.MaxTextureWidth < (WORD)state.buffer_width) ||
		(device_capability.MaxTextureHeight < (WORD)state.buffer_height))
	{
		ZZ_LOG("r_d3d: not support %x% texture. limit(%dx%d)\n",
			state.buffer_width, state.buffer_height, 
			device_capability.MaxTextureWidth, 
			device_capability.MaxTextureHeight);
		return false;
	}

	ZZ_LOG("r_d3d: check_glowable() ok.\n");
	return true;
}


HRESULT zz_renderer_d3d::_create_device (
    UINT Adapter,
    D3DDEVTYPE DeviceType,
    HWND hFocusWindow,
    DWORD BehaviorFlags,
    D3DPRESENT_PARAMETERS *pPresentationParameters,
    IDirect3DDevice9 **ppReturnedDeviceInterface
	)
{
	const int max_retry = 3;
	HRESULT hr;

	for (int retry_count = 0; retry_count < 3; ++retry_count) {
		if (SUCCEEDED(hr = d3d->CreateDevice(
			Adapter,
			DeviceType,
			hFocusWindow, 
			BehaviorFlags,
			pPresentationParameters,
			ppReturnedDeviceInterface)))
		{
			// succeeded
			return hr;
		}
		// createdevice failed

		if (hr != D3DERR_DEVICELOST) { // failed. but not devicelost state. cannot proceed.
			break;
		}
		
		// device lost state, sleep and retry
		ZZ_LOG("r_d3d: _create_device() device lost. sleep.\n");
		Sleep(1000);
	}
	return hr;
}
	


bool zz_renderer_d3d::initialize ()
{
	ZZ_LOG("r_d3d: iNiT...\n");

	if (d3d_device) {
		throw ("renderer_d3d: d3d_device exists already.\n");
	}

	ZeroMemory (&_parameters, sizeof(_parameters));

	assert(view);
	if (!view) {
		throw ("renderer_d3d: no view.\n");
	}

	// set adapter format by view(script)
	if (view->get_depth() == 32) {
        adapter_format = ADAPTER_FORMAT32;
		backbuffer_format = BACKBUFFER_FORMAT32;
		rendertarget_format = RENDERTARGET_FORMAT32;
	}
	else { // 16-bit mode
        adapter_format = ADAPTER_FORMAT16;
		backbuffer_format = BACKBUFFER_FORMAT16;
		rendertarget_format = RENDERTARGET_FORMAT16;
	}

	if (FAILED(d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &display_mode_original))) {
		throw ("renderer_d3d: GetAdapterDisplayMode() failed.\n");
	}

	ZZ_LOG("r_d3d: cUrMoDe (%dx%d)-(%d)-(%dHz)\n",
		display_mode_original.Width, display_mode_original.Height, display_mode_original.Format, display_mode_original.RefreshRate);

	UINT refresh_rate = (state.use_fullscreen) ? state.refresh_rate : 0;
	int check_width = (state.use_fullscreen) ? state.screen_width : display_mode_original.Width;
	int check_height = (state.use_fullscreen) ? state.screen_height : display_mode_original.Height;
	D3DFORMAT check_adapter_format = (state.use_fullscreen) ? adapter_format : display_mode_original.Format;

	D3DDEVTYPE d3d_dev_type = (state.use_HAL) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF;

	if (state.adapter >= 0) { // adapter was already specified explicitly
		adapter_ordinal = static_cast<UINT>(state.adapter);
	}
	else {
		if (!find_adapter_ordinal(d3d_dev_type, adapter_ordinal, state.use_fullscreen, adapter_format, backbuffer_format, 
			check_width, check_height, refresh_rate))
		{
			if (check_width > 1024) {
				// retry default resolution
				ZZ_LOG("r_d3d: find_adapter_ordinal(%d, %s, %d, %d, %dx%d) failed. retry 1024x768.\n",
					adapter_ordinal, state.use_fullscreen ? "full" : "window", adapter_format, backbuffer_format, 
					check_width, check_height);

				check_width = 1024;
				check_height = 768;

				if (!find_adapter_ordinal(d3d_dev_type, adapter_ordinal, state.use_fullscreen, adapter_format, backbuffer_format,
					check_width, check_height, refresh_rate))
				{
					ZZ_LOG("r_d3d: find_adapter_ordinal() retry failed. use default adapter\n");
					adapter_ordinal = D3DADAPTER_DEFAULT;
					//return false;
				}
				state.screen_width = check_width;
				state.screen_height = check_height;
				state.buffer_width = check_width;
				state.buffer_height = check_height;
			}
			else {
				ZZ_LOG("r_d3d: find_adapter_ordinal() failed. use default adapter\n");
				adapter_ordinal = D3DADAPTER_DEFAULT;
				//return false;
			}
		}
	}

	//ZZ_LOG("r_d3d: initialize() check device done\n");

	if (!check_rendertarget32()) {
		ZZ_LOG("r_d3d: not support 32format rendertarget texture.\n");
	}

	// multi-sample check
	if (FAILED(d3d->CheckDeviceMultiSampleType(
		adapter_ordinal,
		d3d_dev_type,
		backbuffer_format,
		!state.use_fullscreen,
		(D3DMULTISAMPLE_TYPE)state.fsaa_type,
		NULL)))
	{
		state.fsaa_type = zz_render_state::ZZ_FSAA_NONE;
		ZZ_LOG("r_d3d: FSAA(%d) failed.\n", state.fsaa_type);
	}

	// backbuffer size = screen size
	_parameters.BackBufferWidth = state.screen_width; // state.buffer_width;
	_parameters.BackBufferHeight = state.screen_height; // state.buffer_height;

	// set default viewport
	default_viewport.X = 0;
	default_viewport.Y = 0;
	default_viewport.MinZ = 0;
	default_viewport.MaxZ = 1.0f;
	default_viewport.Width = state.buffer_width;
	default_viewport.Height = state.buffer_height;

	if (state.use_fullscreen) { // full-screen mode		
		// in fullscreen mode, do not use state.buffer_*
		_parameters.BackBufferCount = 2;
		//_parameters.FullScreen_RefreshRateInHz = display_mode_original.RefreshRate;
		_parameters.FullScreen_RefreshRateInHz = (state.refresh_rate > 0) ? state.refresh_rate : D3DPRESENT_RATE_DEFAULT;
		_parameters.BackBufferFormat = backbuffer_format;
		//_parameters.BackBufferFormat = D3DFMT_R5G6B5; // 16-bit

		_parameters.MultiSampleType = D3DMULTISAMPLE_TYPE(state.fsaa_type);
		_parameters.Windowed = FALSE;

		// if multisample is used, SwapEffect has to be D3DSWAPEFFECT_DISCARD
		if (state.fsaa_type == zz_render_state::ZZ_FSAA_NONE)
			_parameters.SwapEffect = D3DSWAPEFFECT_FLIP;
		else 
			_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		
		_parameters.EnableAutoDepthStencil = TRUE;
		_parameters.AutoDepthStencilFormat = depthstencil_format;
		
		if (state.use_vsync) {
			_parameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE; // same as D3DPRESENT_INTERVAL_DEFAULT
			ZZ_LOG("interface: vsync on.\n");
		}
		else {
			_parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
			ZZ_LOG("interface: vsync off.\n");
		}
		
		ZZ_LOG("r_d3d: fullscreen-mode(%dx%d) enabled.\n", state.buffer_width, state.buffer_height);
		//ShowCursor(FALSE);
	}
	else { // windowed mode
		display_mode = display_mode_original;
		
		// Set render target format by current display mode setting.
		// Reset rendertarget format to support alpha channel.
		if (display_mode.Format == D3DFMT_X8R8G8B8) {
			rendertarget_format = RENDERTARGET_FORMAT32;
			backbuffer_format = BACKBUFFER_FORMAT32;
		}
		else {
			rendertarget_format = RENDERTARGET_FORMAT16;
			backbuffer_format = BACKBUFFER_FORMAT16;
			state.buffer_depth = 16;
		}
		_parameters.BackBufferFormat = backbuffer_format; // display_mode_original.Format;
		_parameters.BackBufferCount = 1;

		_parameters.Windowed = TRUE;
		_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		_parameters.MultiSampleType = D3DMULTISAMPLE_TYPE(state.fsaa_type);
		_parameters.EnableAutoDepthStencil = TRUE;
		_parameters.AutoDepthStencilFormat = depthstencil_format;
		_parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
		ZZ_LOG("r_d3d: WiNmOdE(%dx%d).\n", state.buffer_width, state.buffer_height);
	}

	// d3d_device must be released in cleanup()
	DWORD behavior_flags;
	if (state.use_hw_vertex_processing_support)
		behavior_flags = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else
        behavior_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

	assert(!d3d_device);

	HRESULT hr;

	if (FAILED(hr = _create_device(
		adapter_ordinal,
		d3d_dev_type,
		(HWND)view->get_handle(), 
		behavior_flags,
		&_parameters,
		&d3d_device)))
	{
		if (state.use_hw_vertex_processing_support) {
			// force ignoring use_hw_vertex_processing_support
            state.use_hw_vertex_processing_support = false;
			behavior_flags = D3DCREATE_MIXED_VERTEXPROCESSING;
		}
		else {
			ZZ_LOG("r_d3d: createdevice(SWVP) failed. %s\n", get_hresult_string(hr));
			throw ("renderer_d3d: createdevice(SWVP) failed.\n");
		}
		if (FAILED(_create_device(adapter_ordinal, d3d_dev_type, (HWND)view->get_handle(), 
			behavior_flags, &_parameters, &d3d_device)))
		{
			// check mixed mode
			behavior_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
			if (FAILED(_create_device(adapter_ordinal, d3d_dev_type, (HWND)view->get_handle(), 
				behavior_flags, &_parameters, &d3d_device)))
			{
				ZZ_LOG("r_d3d: createdevice(SWVP2) failed. %s\n", get_hresult_string(hr));
				throw ("renderer_d3d: createdevice(SWVP2) failed.\n");
			}
			else {
				ZZ_LOG("r_d3d: createdevice(force SWVP mode)\n");
			}
		}
		else {
			ZZ_LOG("r_d3d: cdFMXVP\n");
		} 
	}
	else {
		switch (d3d_dev_type) {
			case D3DDEVTYPE_HAL:
				ZZ_LOG("r_d3d: cdHW\n");
				break;
			case D3DDEVTYPE_REF:
				ZZ_LOG("r_d3d: cdREF\n");
				break;
			case D3DDEVTYPE_SW:
				ZZ_LOG("r_d3d: cdSW\n");
				break;
		}
	}

	// get device caps
	d3d_device->GetDeviceCaps(&device_capability);

	// get shader version (ex. 0 = 0.0, 1.1 = 11, 2.0 = 20 ...)
	state.vertex_shader_version = (device_capability.VertexShaderVersion & 0x0F) +
		((device_capability.VertexShaderVersion >> 8) & 0x0F)*10;

	state.max_simultaneous_textures = device_capability.MaxSimultaneousTextures;

	// save initial available texture memory
	max_texture_memory = d3d_device->GetAvailableTextureMem();
	ZZ_LOG("r_d3d: maxtexmem = %d\n", max_texture_memory);

	const unsigned int MIN_TEXTURE_MEMORY = 40;

	if (max_texture_memory < MIN_TEXTURE_MEMORY) {
		ZZ_LOG("r_d3d: too small texture memory(%d)\n", max_texture_memory);
		throw "renderer_d3d: too smal texture memory";
	}

	// cursor check
	if (!support_hw_mouse()) {
		ZZ_LOG("r_d3d: not support hw mouse.\n");
	}

	if (!state.use_multipass) {
		if (device_capability.MaxSimultaneousTextures > 3) {
			state.use_multipass = false; // do all in a single pass
		}
		else if (device_capability.MaxSimultaneousTextures > 1) {
			ZZ_LOG("r_d3d: initialize() force to use_multipass\n");
			state.use_multipass = true; // do multipass
		}
		else { // < 2
			throw ("renderer_d3d: initialize() failed. Device does not support multi-texturing\n");
			return false;
		}
	}

	// check if the device supports dynamic textures
	state.use_dynamic_textures = (device_capability.Caps2 & D3DCAPS2_DYNAMICTEXTURES) ? true : false;
	if (!state.use_dynamic_textures) {
		ZZ_LOG("r_d3d: initialize: not support dynamic textures.\n");
	}

	// check if the device supports special alpha blending operation
	if (device_capability.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP) {
		state.use_blackshadowmap = false;
	}
	else { // not support D3DBLENDOP_REVSUBTRACT
		state.use_blackshadowmap = true; // force to use black shadowmap
	}

	// if the current device is not h/w or mixed mode, but does not support vertex shader,
	// then, we recreate to s/w mode.
	if ((state.vertex_shader_version == 0) && (behavior_flags != D3DCREATE_SOFTWARE_VERTEXPROCESSING)) {
		ZZ_LOG("r_d3d: device does not support vertex shader.\n");
		state.use_hw_vertex_processing_support = false;
		behavior_flags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;

		// re-create d3d_device by software_vertex_processing mode
		if (FAILED(hr = d3d_device->Release())) {
			ZZ_LOG("r_d3d: d3d_device->release() failed. %s\n", get_hresult_string(hr));
			throw ("r_d3d: d3d_device->release() failed\n");
			return false;
		}

		if (FAILED(hr = _create_device(
			adapter_ordinal,
			d3d_dev_type,
			(HWND)view->get_handle(),
			behavior_flags,
			&_parameters,
			&d3d_device)))
		{
			ZZ_LOG("r_d3d: createdevice(swvp3) failed. %s\n", get_hresult_string(hr));
			throw ("r_d3d: createdevice(swvp3) failed.\n");
			return false;
		}
	}

	if (!(device_capability.TextureCaps & D3DPTEXTURECAPS_MIPMAP)) {
		ZZ_LOG("r_d3d: initialize() failed. Device does not support mipmaps!\n");
		//return false;
	}

	if (device_capability.MaxSimultaneousTextures < 2) {
		ZZ_LOG("r_d3d: initialize() failed. Device does not support two simultaneous textuers!\n");
	}

	if ((state.shadowmap_blur_type > 0) && (shadowmap_pixels == NULL)) {
		shadowmap_pixels = zz_new vec3[state.shadowmap_size*state.shadowmap_size]; // deleted in cleanup()
	}

	init_device_objects();
	znzin->init_device_objects();
	
	restore_device_objects();
	znzin->restore_device_objects();

	_cached.invalidate_texture();

	if (!gamma_saved) {
		gamma_saved = true;
		get_gamma(saved_gamma); // save gamma
		current_gamma.copy_from(saved_gamma); // copy gamma
	}
	else {
		set_gamma(current_gamma);
	}

	ZZ_LOG("r_d3d: iNiTdOnE\n");

	return true;
}

// This creates all device-dependent managed objects, such as managed
//       textures and managed vertex buffers.
// Name: InitDeviceObjects()
// Desc: Paired with DeleteDeviceObjects()
//       The device has been created.  Resources that are not lost on
//       Reset() can be created here -- resources in D3DPOOL_MANAGED,
//       D3DPOOL_SCRATCH, or D3DPOOL_SYSTEMMEM.  Image surfaces created via
//       CreateImageSurface are never lost and can be created here.  Vertex
//       shaders and pixel shaders can also be created here as they are not
//       lost on Reset().
bool zz_renderer_d3d::init_device_objects ()
{
	// create shadowmap stuff
	if (state.use_shadowmap && state.use_round_shadow) {
		// not implemented yet
	}

	return true;
}

void zz_renderer_d3d::fill_shadowmap_vb ()
{
	TOVERLAY_VERTEX* dest_vertex;

	// lock here <----------------------------------------
	if (FAILED(shadowmap_vb->Lock( 0, 0, (void**)&dest_vertex,
		D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE)))
		return;

	// vertex positions
	float left[NUM_SHADOW_BLUR], right[NUM_SHADOW_BLUR], top[NUM_SHADOW_BLUR], bottom[NUM_SHADOW_BLUR];
	float size = float(state.shadowmap_size);

	// penumbra vertex position setup
	float dist = 2.0f;
	left[0] = -dist; right[0] = left[0] + size; bottom[0] = 0.0f;  top[0] = bottom[0] + size;
	left[1] = dist;  right[1] = left[1] + size; bottom[1] = 0.0f;  top[1] = bottom[1] + size;
	left[2] = 0.f;  right[2] = left[2] + size; bottom[2] = -dist; top[2] = bottom[2] + size;
	left[3] = 0.f;  right[3] = left[3] + size; bottom[3] = dist;  top[3] = bottom[3] + size;
	
	// umbra vertex position setup
	dist = 1.0f;
	left[4] = -dist; right[4] = left[4] + size; bottom[4] = 0.0f;  top[4] = bottom[4] + size;
	left[5] = dist;  right[5] = left[5] + size; bottom[5] = 0.0f;  top[5] = bottom[5] + size;
	left[6] = 0.f;  right[6] = left[6] + size; bottom[6] = -dist; top[6] = bottom[6] + size;
	left[7] = 0.f;  right[7] = left[7] + size; bottom[7] = dist;  top[7] = bottom[7] + size;
	
	for (int i = 0; i < NUM_SHADOW_BLUR; ++i) {
		// position
		dest_vertex[i*4 + 0].p = D3DXVECTOR4( left[i] - .5f ,  top[i] - .5f,    0.0f, 1.0f );
		dest_vertex[i*4 + 1].p = D3DXVECTOR4( right[i] - .5f, top[i] - .5f ,    0.0f, 1.0f );
		dest_vertex[i*4 + 2].p = D3DXVECTOR4( left[i] - .5f,  bottom[i] - .5f , 0.0f, 1.0f );
		dest_vertex[i*4 + 3].p = D3DXVECTOR4( right[i] - .5f, bottom[i] - .5f, 0.0f, 1.0f );
		// texture coordinate
		dest_vertex[i*4 + 0].tu = 0.0f; dest_vertex[i*4 + 0].tv = 1.0f;
		dest_vertex[i*4 + 1].tu = 1.0f; dest_vertex[i*4 + 1].tv = 1.0f;
		dest_vertex[i*4 + 2].tu = 0.0f; dest_vertex[i*4 + 2].tv = 0.0f;
		dest_vertex[i*4 + 3].tu = 1.0f; dest_vertex[i*4 + 3].tv = 0.0f;
	}	

	// ----------------------------------------> unlock here
	if (FAILED(shadowmap_vb->Unlock())) return;
}

void zz_renderer_d3d::fill_glow_vb ()
{
	TOVERLAY_VERTEX* dest_vertex;

	// lock here <----------------------------------------
	if (FAILED(glow_vb->Lock( 0, 0, (void**)&dest_vertex,
		D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE)))
		return;
	// vertex positions
	float left[NUM_GLOW_BLUR], right[NUM_GLOW_BLUR], top[NUM_GLOW_BLUR], bottom[NUM_GLOW_BLUR];
	float size = float(state.glowmap_size);

	// penumbra vertex position setup
	float dist = 2.0f;
	left[0] = -dist; right[0] = left[0] + size; bottom[0] = 0.0f;  top[0] = bottom[0] + size;
	left[1] = dist;  right[1] = left[1] + size; bottom[1] = 0.0f;  top[1] = bottom[1] + size;
	left[2] = 0.f;  right[2] = left[2] + size; bottom[2] = -dist; top[2] = bottom[2] + size;
	left[3] = 0.f;  right[3] = left[3] + size; bottom[3] = dist;  top[3] = bottom[3] + size;
	
	// umbra vertex position setup
	dist = 1.0f;
	left[4] = -dist; right[4] = left[4] + size; bottom[4] = 0.0f;  top[4] = bottom[4] + size;
	left[5] = dist;  right[5] = left[5] + size; bottom[5] = 0.0f;  top[5] = bottom[5] + size;
	left[6] = 0.f;  right[6] = left[6] + size; bottom[6] = -dist; top[6] = bottom[6] + size;
	left[7] = 0.f;  right[7] = left[7] + size; bottom[7] = dist;  top[7] = bottom[7] + size;
	
	for (int i = 0; i < NUM_GLOW_BLUR; ++i) {
		// position
		dest_vertex[i*4 + 0].p = D3DXVECTOR4( left[i] - .5f ,  top[i] - .5f,    0.0f, 1.0f );
		dest_vertex[i*4 + 1].p = D3DXVECTOR4( right[i] - .5f, top[i] - .5f ,    0.0f, 1.0f );
		dest_vertex[i*4 + 2].p = D3DXVECTOR4( left[i] - .5f,  bottom[i] - .5f , 0.0f, 1.0f );
		dest_vertex[i*4 + 3].p = D3DXVECTOR4( right[i] - .5f, bottom[i] - .5f, 0.0f, 1.0f );

		// texture coordinate
		dest_vertex[i*4 + 0].tu = 0.0f; dest_vertex[i*4 + 0].tv = 1.0f;
		dest_vertex[i*4 + 1].tu = 1.0f; dest_vertex[i*4 + 1].tv = 1.0f;
		dest_vertex[i*4 + 2].tu = 0.0f; dest_vertex[i*4 + 2].tv = 0.0f;
		dest_vertex[i*4 + 3].tu = 1.0f; dest_vertex[i*4 + 3].tv = 0.0f;
	}	

	// ----------------------------------------> unlock here
	if (FAILED(glow_vb->Unlock())) return;
}

bool zz_renderer_d3d::create_glow_textures ()
{
	//------------------------------------------------------
	// Create glow backbuffer texture & surface
	zz_assert(NULL == glow_backbuffer_texture);
	zz_assert(NULL == glow_backbuffer_surface);
	zz_assert(NULL == glow_downsample_texture);
	zz_assert(NULL == glow_downsample_surface);
	zz_assert(NULL == glow_blur_texture_fullscene);
	zz_assert(NULL == glow_blur_surface_fullscene);
	zz_assert(NULL == glow_blur_texture);
	zz_assert(NULL == glow_blur_surface);
	
	HRESULT hr;

	if (state.use_glow_ztest) {
		if (FAILED(hr = d3d_device->CreateTexture(
			state.buffer_width, 
			state.buffer_height, 
			1, // level
			D3DUSAGE_RENDERTARGET,
			RENDERTARGET_FORMAT32,
			D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
			&glow_backbuffer_texture, NULL)))
		{
			ZZ_LOG("r_d3d: createtexture(glow_backbuffer_texture, %dx%d) failed.[%s]", 
				state.buffer_width, state.buffer_height,
				get_hresult_string(hr));
			state.use_glow_ztest = false;
			zz_assert(glow_backbuffer_texture == 0);
			// does not return to create glow fullscene
		}
		// Retrieve top-level surfaces of our shadow buffer (need these for use with SetRenderTarget)
		else if (FAILED(hr = glow_backbuffer_texture->GetSurfaceLevel(0, &glow_backbuffer_surface))) {
			ZZ_LOG("r_d3d: getsurfacelevel(glow_backbuffer_surface) failed.[%s]", get_hresult_string(hr));
			zz_assert(0);
			return false;
		}
	}

	//------------------------------------------------------
	// Create glow downsample texture & surface
	if (FAILED(hr = d3d_device->CreateTexture(
		state.glowmap_size, // default to 128
		state.glowmap_size, 
		1, // level
		D3DUSAGE_RENDERTARGET,
		RENDERTARGET_FORMAT32,
		D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
		&glow_downsample_texture, NULL)))
	{
		ZZ_LOG("r_d3d: CreateTexture(glow_downsample, %dx%d) failed. [%s]",
			state.glowmap_size, state.glowmap_size,
			get_hresult_string(hr) );
		zz_assert(0);
		return false;
	}
	if (FAILED(hr = glow_downsample_texture->GetSurfaceLevel(0, &glow_downsample_surface))) {
		ZZ_LOG("r_d3d: GetSurfaceLevel(glow_downsample_surface) failed.[%s]", get_hresult_string(hr));
		zz_assert(0);
		return false;
	}

	//------------------------------------------------------
	// Create glow blur texture & surface
	if (FAILED(hr = d3d_device->CreateTexture(
		state.glowmap_size,
		state.glowmap_size, 
		1, // level
		D3DUSAGE_RENDERTARGET,
		RENDERTARGET_FORMAT32, // blur texture should have 32-bit accuracy. if not, blurring could be unrecognizible.
		D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
		&glow_blur_texture, NULL)))
	{
		ZZ_LOG("r_d3d: CreateTexture(glow_blur_texture[%d], %dx%d) failed.[%s]",
			state.glowmap_size, state.glowmap_size, get_hresult_string(hr) );
		zz_assert(0);
		return false;
	}
	if (FAILED(hr = glow_blur_texture->GetSurfaceLevel(0, &glow_blur_surface))) {
		ZZ_LOG("r_d3d: GetSurfaceLevel(glow_blur_surface) failed.[%s]", get_hresult_string(hr));
		zz_assert(0);
		return false;
	}

	if (state.use_glow_fullscene) {
		//------------------------------------------------------
		// Create fullscene glow blur texture & surface
		if (FAILED(hr = d3d_device->CreateTexture(
			state.glowmap_size,
			state.glowmap_size, 
			1, // level
			D3DUSAGE_RENDERTARGET,
			RENDERTARGET_FORMAT32, // blur texture should have 32-bit accuracy. if not, blurring could be unrecognizible.
			D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
			&glow_blur_texture_fullscene, NULL)))
		{
			ZZ_LOG("r_d3d: CreateTexture(glow_blur_texture_fulscene[%d], %dx%d) failed.[%s]",
				state.glowmap_size, state.glowmap_size, get_hresult_string(hr) );
			zz_assert(0);
			return false;
		}
		if (FAILED(hr = glow_blur_texture_fullscene->GetSurfaceLevel(0, &glow_blur_surface_fullscene))) {
			ZZ_LOG("r_d3d: GetSurfaceLevel(glow_blur_surface_fullscene) failed.[%s]", get_hresult_string(hr));
			zz_assert(0);
			return false;
		}
	}

	return true;
}

// Restore device-memory objects and state after a device is created or resized.
// Name: RestoreDeviceObjects()
// Desc: Paired with InvalidateDeviceObjects()
//       The device exists, but may have just been Reset().  Resources in
//       D3DPOOL_DEFAULT and any other device state that persists during
//       rendering should be set here.  Render states, matrices, textures,
//       etc., that don't change during rendering can be set once here to
//       avoid redundant state setting during Render() or FrameMove().
bool zz_renderer_d3d::restore_device_objects ()
{
	HRESULT hr;

	assert(NULL == backbuffer_surface);
	//setup buffers; retrieve back buffer surface
	if(FAILED(hr = d3d_device->GetRenderTarget(0, &backbuffer_surface))) {
		zz_assertf(0, "renderer_d3d: getrendertarget() failed.[%s]", get_hresult_string(hr));
		return false;
	}
	
	// pass log started
	//ZZ_LOG("r_d3d: RES");
	
	int progress = 0;

	//ZZ_LOG("%d-", progress++);
	
	assert(NULL == backbuffer_zsurface);
	// retrieve z-buffer surface
	if(FAILED(hr = d3d_device->GetDepthStencilSurface(&backbuffer_zsurface))) {
		zz_assertf(0, "renderer_d3d: getdepthstencilsurface() failed.[%s]", get_hresult_string(hr));
		return false;
	}
	//ZZ_LOG("%d-", progress++);

	if(!backbuffer_surface || !backbuffer_zsurface) {
		zz_assertf(0, "renderer_d3d: backbuffer_surface or backbuffer_zsurface failure.");
		return false;
	}
	//ZZ_LOG("%d-", progress++);

	if (use_virtual_backbuffer) {
		assert(NULL == v_backbuffer_texture);
		if (FAILED(hr = d3d_device->CreateTexture(
			state.buffer_width, 
			state.buffer_height, 
			1, // level
			D3DUSAGE_RENDERTARGET,
			rendertarget_format,
			D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
			&v_backbuffer_texture, NULL)))
		{
			zz_assertf(0, "renderer_d3d: createtexture(v_backbuffer_texture, %dx%d) failed. [%s]", 
				state.buffer_width, state.buffer_height,
				get_hresult_string(hr));
			return false;
		}

		assert(NULL == v_backbuffer_surface);
		// Retrieve top-level surfaces of our shadow buffer (need these for use with SetRenderTarget)
		if (FAILED(hr = v_backbuffer_texture->GetSurfaceLevel(0, &v_backbuffer_surface))) {
			zz_assertf(0, "renderer_d3d: getsurfacelevel(v_backbuffer_surface) failed.[%s]", get_hresult_string(hr));
			return false;
		}

		assert(NULL == v_backbuffer_zsurface);
		if (FAILED(hr = d3d_device->CreateDepthStencilSurface(
			state.buffer_width,
			state.buffer_height,
			DEPTH_STENCIL_FORMAT,
			D3DMULTISAMPLE_NONE , // multisample type
			0, // MultisampleQuality
			FALSE,
			&v_backbuffer_zsurface,
			NULL)))
		{
			zz_assertf(0, "renderer_d3d: createdepthstencilsurface(%dx%d) failed.[%s]", state.buffer_width, state.buffer_height, get_hresult_string(hr));
			return false;
		}
	}
	//ZZ_LOG("%d-", progress++);

	if (state.use_shadowmap) {
		if (!check_shadowable()) {
			state.use_shadowmap = false;
		}
	}

	if (state.use_glow || state.use_glow_fullscene) {
		if (!check_glowable()) {
			state.use_glow = false;
			state.use_glow_fullscene = false;
		}
		else if (!create_glow_textures()) {
			state.use_glow = false;
			state.use_glow_fullscene = false;
		}
	}
	//ZZ_LOG("%d-", progress++);

	// Create shadow map texture and retrieve surface : shadowmap, shadowmap_surface
	if (state.use_shadowmap) {
		assert(shadowmap == NULL);
		if (FAILED(hr = d3d_device->CreateTexture(
			state.shadowmap_size, 
			state.shadowmap_size, 
			1, // level
			D3DUSAGE_RENDERTARGET,
			rendertarget_format,
			D3DPOOL_DEFAULT, // rendertarget must set this to D3DPOOL_DEFAULT
			&shadowmap, NULL)))
		{
			zz_assertf(0, "renderer_d3d: createtexture(color_texture, %dx%d) failed.[%s]", state.shadowmap_size, state.shadowmap_size, get_hresult_string(hr) );
			return false;
		}
		// Retrieve top-level surfaces of our shadow buffer (need these for use with SetRenderTarget)
		if (FAILED(hr = shadowmap->GetSurfaceLevel(0, &shadowmap_surface))) {
			zz_assertf(0, "renderer_d3d: getsurfacelevel(shadowmap_surface) failed. [%s]\n", get_hresult_string(hr));
			return false;
		}

		assert(NULL == shadowmap_forblur);
		// create blurred shadowmap texture
		if (FAILED(hr = d3d_device->CreateTexture(
			state.shadowmap_size,
			state.shadowmap_size,
			1,
			D3DUSAGE_RENDERTARGET, // usage
			rendertarget_format,
			D3DPOOL_DEFAULT,
			&shadowmap_forblur,
			NULL)))
		{
			zz_assertf(0, "renderer_d3d: blurred_shadowmap(%dx%d) creation failed.[%s]",
				state.shadowmap_size, state.shadowmap_size, get_hresult_string(hr));
			return false;
		}
		assert(NULL == shadowmap_forblur_surface);
		if (FAILED(hr = shadowmap_forblur->GetSurfaceLevel(0, &shadowmap_forblur_surface))) {
			zz_assertf(0, "renderer_d3d: getsurfacelevel(shadowmap_forblur_surface) failed.[%s]", get_hresult_string(hr));
			return false;
		}
		//ZZ_LOG("%d-", progress++);
	}

	if (state.fsaa_type > 0) {
		d3d_device->SetRenderState(D3DRS_MULTISAMPLEANTIALIAS, TRUE);
	}

	// prepare for shadowmap viewport
	shadowmap_viewport.Width  = state.shadowmap_size;
	shadowmap_viewport.Height = state.shadowmap_size;
	shadowmap_viewport.MinZ = 0.0f;
	shadowmap_viewport.MaxZ = 1.0f;
	shadowmap_viewport.X = 0;
	shadowmap_viewport.Y = 0;

	overlay_viewport.Width  = state.shadowmap_size;
	overlay_viewport.Height = state.shadowmap_size;
	overlay_viewport.MinZ = 0.0f;
	overlay_viewport.MaxZ = 1.0f;
	overlay_viewport.X = 0;
	overlay_viewport.Y = 0;

	glow_viewport.Width  = state.buffer_width;
	glow_viewport.Height = state.buffer_height;

	glow_viewport.MinZ = 0.0f;
	glow_viewport.MaxZ = 1.0f;
	glow_viewport.X = 0;
	glow_viewport.Y = 0;

	glow_downsample_viewport.Width  = state.glowmap_size;
	glow_downsample_viewport.Height = state.glowmap_size;
	glow_downsample_viewport.MinZ = 0.0f;
	glow_downsample_viewport.MaxZ = 1.0f;
	glow_downsample_viewport.X = 0;
	glow_downsample_viewport.Y = 0;

	d3d_device->SetRenderState(D3DRS_LIGHTING, FALSE);
	//ZZ_LOG("%d-", progress++);
	//d3d_device->LightEnable(0, FALSE);
	
	enable_alpha_test(true);
	enable_zbuffer(true);
	enable_zwrite(true);

	//if (!create_normalization_cubemap(256, 9)) {
	//	ZZ_LOG("r_d3d: normalization cubemap create failed\n");
	//	return false;
	//}

	// fog setting
	d3d_device->SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );  
	d3d_device->SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE );

	if (FAILED(hr = d3d_device->SetRenderState(D3DRS_FOGENABLE, FALSE))) {
		zz_assertf(0, "renderer_d3d: enable_fog() failed.[%s]", get_hresult_string(hr));
		return false;
	}
	enable_fog(state.use_fog);

	//ZZ_LOG("%d-", progress++);

	assert(NULL == overlay_vb);

	// create overlay texture vertex buffer
	DWORD usage_dynamic = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
	if (!state.use_hw_vertex_processing_support) {
		usage_dynamic |= D3DUSAGE_SOFTWAREPROCESSING;
	}
    if (FAILED(hr = d3d_device->CreateVertexBuffer( 4 * sizeof(TOVERLAY_VERTEX), 
		usage_dynamic,
		0,
		D3DPOOL_DEFAULT,
		&overlay_vb,
		NULL)))
	{
		zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for overlay_vb failed. [%s]", get_hresult_string(hr));
		return false;
	}
	//ZZ_LOG("%d-", progress++);

	assert(NULL == shadowmap_vb);
	// create shadowmap blur vertex buffer
    if (FAILED(hr = d3d_device->CreateVertexBuffer( NUM_SHADOW_BLUR * 4 * sizeof(TOVERLAY_VERTEX), 
		usage_dynamic,
		0,
		D3DPOOL_DEFAULT,
		&shadowmap_vb,
		NULL)))
	{
		zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for shadowmap_vb failed. [%s]", get_hresult_string(hr));
		return false;
	}
	else {
		fill_shadowmap_vb();
	}
	//ZZ_LOG("%d-", progress++);

	assert(NULL == glow_vb);
	// create glow blur vertex buffer
	if (FAILED(hr = d3d_device->CreateVertexBuffer( NUM_GLOW_BLUR * 4 * sizeof(TOVERLAY_VERTEX), 
		usage_dynamic,
		0,
		D3DPOOL_DEFAULT,
		&glow_vb,
		NULL)))
	{
		zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for glow_vb failed. [%s]", get_hresult_string(hr));
	}
	else {
		fill_glow_vb();
	}

	//ZZ_LOG("%d-", progress++);

	// create shadowmap overlay texture
	const char * shadowmap_overlay_path;
	if (state.shadowmap_blur_type == 0) { // no blur
		state.use_blackshadowmap = true;
	}
	if (state.use_blackshadowmap) {
		shadowmap_overlay_path = SHADOWOVER_TEXTURE_BLACK;
	}
	else {
        shadowmap_overlay_path = SHADOWOVER_TEXTURE_WHITE;
	}

	zz_vfs fs;
	uint32 size;
	const void * data = fs.open_read_get_data(shadowmap_overlay_path, &size);
	if (size <= 0 || !data) {
		ZZ_LOG("r_d3d: restore_device_objects() failed. shadowmap_overlay texture(%s) not found.", shadowmap_overlay_path);
		zz_assert(0);
		// continue other restoring job
	}
	else {
		assert(NULL == shadowmap_overlay_texture);
		if (FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(
			d3d_device,			// LPDIRECT3DDEVICE9 pDevice,
			data, // pSrcData
			(UINT)size, // SrcDataSize
			0,		// UINT Width,
			0,		// UINT Height,
			1, // UINT MipLevels,
			0,					// DWORD Usage,(0, D3DUSAGE_RENDERTARGET, or D3DUSAGE_DYNAMIC)
			D3DFMT_UNKNOWN, // D3DFMT_DXT5, // D3DFMT_UNKNOWN, // D3DFMT_DXT5, // D3DFMT_UNKNOWN,		// D3DFORMAT Format,
			D3DPOOL_MANAGED,
			D3DX_FILTER_NONE, // DWORD Filter, D3DX_DEFAULT = (D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER)
			D3DX_FILTER_NONE, // DWORD MipFilterw D3DX_DEFAULT = (D3DX_FILTER_BOX)
			0,					// D3DCOLOR ColorKey,
			NULL,				// D3DXIMAGE_INFO* pSrcInfo
			NULL,				// PALETTEENTRY* pPalette,
			&shadowmap_overlay_texture)))
		{
			ZZ_LOG("r_d3d: D3DXCreateTextureFromFileInMemoryEx(%s) failed. [%s]", shadowmap_overlay_path, get_hresult_string(hr));
			zz_assert(0);
		}
	}
	//ZZ_LOG("%d-", progress++);

	d3d_device->SetRenderState( D3DRS_LIGHTING,  FALSE );

	create_default_sprite();

	//ZZ_LOG("eNdP\n");
	return true;
}


// invalidate_device_objects -> delete_device_objects
bool zz_renderer_d3d::invalidate_device_objects ()
{
	SAFE_RELEASE(line_vertex_buffer);

	SAFE_RELEASE(backbuffer_surface);
	SAFE_RELEASE(backbuffer_zsurface);

	SAFE_RELEASE(v_backbuffer_surface);
	SAFE_RELEASE(v_backbuffer_zsurface);
	SAFE_RELEASE(v_backbuffer_texture);

	SAFE_RELEASE(glow_backbuffer_surface);
	SAFE_RELEASE(glow_backbuffer_texture);
	SAFE_RELEASE(glow_downsample_texture);
	SAFE_RELEASE(glow_downsample_surface);

	SAFE_RELEASE(glow_blur_texture_fullscene);
	SAFE_RELEASE(glow_blur_surface_fullscene);

	SAFE_RELEASE(glow_blur_texture);
	SAFE_RELEASE(glow_blur_surface);

	SAFE_RELEASE(shadowmap_surface);
	//SAFE_RELEASE(shadowmap_zsurface);
	SAFE_RELEASE(shadowmap);
	SAFE_RELEASE(shadowmap_forblur);
	SAFE_RELEASE(shadowmap_forblur_surface);
	SAFE_RELEASE(shadowmap_overlay_texture);
	SAFE_RELEASE(overlay_vb);
	SAFE_RELEASE(shadowmap_vb);
	SAFE_RELEASE(glow_vb);
	SAFE_RELEASE(normalization_cubemap);
    SAFE_RELEASE(sprite_vertexbuffer_cover);
	SAFE_RELEASE(sprite_vertexbuffer_origin);
    SAFE_RELEASE(sprite_vertexbuffer_origin_ex);
	SAFE_RELEASE(sphere_buffer);
	SAFE_RELEASE(cylinder_buffer);
	SAFE_RELEASE(boundingbox_vertexbuffer);
    SAFE_RELEASE(boundingbox_indexbuffer);

	destroy_default_sprite();

	return true;
}

// invalidate_device_objects->delete_device_objects
// Name: DeleteDeviceObjects()
// Desc: Paired with InitDeviceObjects()
//       Called when the app is exiting, or the device is being changed,
//       this function deletes any device dependent objects.  
bool zz_renderer_d3d::delete_device_objects ()
{
	zz_assert(d3d_device);

	return true;
}


void zz_renderer_d3d::create_default_sprite ()
{
	assert(d3d_device);
	if (!d3d_device) {
		ZZ_LOG("r_d3d: create_default_sprite() failed. no d3d device.\n");
		zz_assert(0);
	}

	if (d3d_sprite) {
		ZZ_LOG("r_d3d: create_default_sprite() failed. already exist.\n");
		return;
	}

	// create d3d sprite object
	if (FAILED(D3DXCreateSprite(d3d_device, &d3d_sprite))) {
		ZZ_LOG("r_d3d: createsprite() failed.");
		zz_assert(0);
		return;
	}
}

void zz_renderer_d3d::destroy_default_sprite ()
{
	SAFE_RELEASE(d3d_sprite);
}

void zz_renderer_d3d::cleanup ()
{
	//ZZ_LOG("r_d3d: cLeAnUp....\n");

	if (!d3d_device) {
		ZZ_LOG("r_d3d: no d3d_device\n");
		return; // do nothing
	}

	if (!znzin->flush_delayed(true /* entrance */, true /* exit */)) {
		ZZ_LOG("r_d3d: cleanup() failed. flush_delayed() failed.\n");
	}

	// restore gamma saved in initialize()
	set_gamma(saved_gamma);

	//ZZ_LOG("r_d3d: max texture usage = %d MB\n",
	//	(max_texture_memory - min_texture_memory)/1000000);

	invalidate_device_objects();
	znzin->invalidate_device_objects();

	delete_device_objects();
	znzin->delete_device_objects();
	
#ifdef LOG_TEXTURES
	for (std::vector<zz_texture*>::iterator it = s_textures.begin();
		it != s_textures.end(); ++it)
	{
		ZZ_LOG("r_d3d: [%x] not cleared\n", (*it));
	}
#endif

	zz_assert(vertex_buffer_pool.get_num_running() == 0);
	zz_assert(index_buffer_pool.get_num_running() == 0);
	zz_assert(d3d_textures.get_num_running() == 0);
	zz_assert(vertex_decls.get_num_running() == 0);
	zz_assert(vertex_shaders.get_num_running() == 0);
	zz_assert(pixel_shaders.get_num_running() == 0);

	if (shadowmap_pixels) {
		ZZ_SAFE_DELETE(shadowmap_pixels); // created in initialize
	}

	SAFE_RELEASE(d3d_device); // created in initialize()
	ZZ_LOG("r_d3d: cLeAnUpDoNe\n"); 
}

void zz_renderer_d3d::set_cullmode (zz_render_state::zz_cull_mode_type cullmode)
{
	switch (cullmode) {
		case zz_render_state::ZZ_CULLMODE_NONE :
			d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE); 
			break;
		case zz_render_state::ZZ_CULLMODE_CW :
			d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); 
			break;
		case zz_render_state::ZZ_CULLMODE_CCW :
			d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW); 
			break;
	}
}

void zz_renderer_d3d::begin_shadowmap (void)
{
	assert(state.use_shadowmap);

	//ZZ_LOG("r_d3d: begin_shadowmap()\n");

	if (!light_camera) {
		light_camera = znzin->get_camera_light();
		if (!light_camera) return;
	}

	set_cullmode(state.cull_mode);

	if (FAILED(d3d_device->BeginScene())) {
		ZZ_LOG("r_d3d: begin_shadowmap: BeginScene() failed\n");
	}

	// shader TM constants setup for light_camera point of view
	light_camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, modelview_matrix);
	light_camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_matrix);
	set_projection_matrix(projection_matrix);
	set_modelview_matrix(modelview_matrix);

	// swap camera
	saved_camera = znzin->get_camera();    //test
	znzin->set_camera(light_camera);      

	assert(saved_camera);              

	//set render target to shadow map surfaces
	if (state.shadowmap_blur_type > 0) {
		if (FAILED(d3d_device->SetRenderTarget(0, shadowmap_forblur_surface))) {
			ZZ_LOG("r_d3d: begin_shadowmap: SetRenderTarget(shadowmap_forblur_surface) failed\n");
			state.use_shadowmap = false;
		}
	}
	else { // no blur
		if (FAILED(d3d_device->SetRenderTarget(0, shadowmap_surface))) {
			ZZ_LOG("r_d3d: begin_shadowmap: SetRenderTarget(shadowmap_surface) failed\n");
			state.use_shadowmap = false;
		}
	}
	// set shadowmap depth-stencil surface
	if (FAILED(d3d_device->SetDepthStencilSurface(NULL))) { // no z-surface
		ZZ_LOG("r_d3d: begin_shadowmap: SetDepthStencilSurface() failed.\n");
		this->_scene_began = false;
		return;
	}


	enable_zbuffer(false);
	enable_zwrite(false);

	//save old viewport
	d3d_device->GetViewport(&old_viewport);

	//set new, funky viewport
	d3d_device->SetViewport(&shadowmap_viewport);

	// tex_shadowmap setting : identity setting
	mat4 texmat = mat4_id;
	set_texture_matrix(1 /* texture stage */, ZZ_VSC_LIGHT_TEX, texmat);

	// In all cases, uses black background
	d3d_device->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0L );

	set_render_where(ZZ_RW_SHADOWMAP);
}

void zz_renderer_d3d::shadowmap_texturetm_setup (void)    //test  11-18 여러번 계산
{
	assert(state.use_shadowmap);

	assert(light_camera);

	mat4 light_viewTM, light_projTM, light_view_projTM, camera_view_inverseTM;
	mat4 Lproj_Lview_CiviewTM;

	// get the inverse of the camera view (= camera->modelview)
	if(znzin->camera_sfx.get_success())
		invert(camera_view_inverseTM, znzin->camera_sfx.camera_sfx_m);
	else
	invert(camera_view_inverseTM, modelview_matrix);

	// get the sum of projTM * viewTM
	light_camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, light_viewTM);
	light_camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, light_projTM);
	mult(light_view_projTM, light_projTM, light_viewTM);

	// get the TM = LightProjTM * LightViewTM * CameraViewTM^-1
	mult(Lproj_Lview_CiviewTM, light_view_projTM, camera_view_inverseTM);

	//set special texture matrix for shadow mapping
	float offset_x = 0.5f + (0.5f / (float)state.shadowmap_size);
	float offset_y = 0.5f + (0.5f / (float)state.shadowmap_size);
	unsigned int range = 0xFFFFFFFF >> (32 - 24); // 24 bitdepth
	float bias = -(float)range/(light_camera->get_far_plane() - light_camera->get_near_plane());
	//float bias = -(float)range*.001f;

	mat4 tex_scale_bias_mat (
		.5f, 0.0f, 0.0f, offset_x,
		0.0f, -0.5f, 0.0f, offset_y,
		0.0f, 0.0f, (float)range, bias,
		0.0f, 0.0f, 0.0f, 1.0f);

	mat4 texmat;

	// texmat = tex_scale_bias_mat * Lproj_Lview_CiviewTM
	mult(texmat, tex_scale_bias_mat, Lproj_Lview_CiviewTM);

	// set final texture matrix to shader
	set_texture_matrix(1 /* texture stage */, ZZ_VSC_LIGHT_TEX, texmat);
}

void zz_renderer_d3d::init_textures ()
{
	// initialize texture binding
	for (int i = 0; i < NUM_STAGES; i++) {
		set_texture(ZZ_HANDLE_NULL, i, 0);
	}
}

void zz_renderer_d3d::init_render_state ()
{
	set_cullmode(state.cull_mode); // default cullmode set
	
	if (state.use_wire_mode) {
		d3d_device->SetRenderState( D3DRS_FILLMODE, D3DFILL_WIREFRAME );
	}
	else {
		d3d_device->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}

	// mostly, filtering is not changed over frames.
	// no need to change every frame
	// check h/w support for filtering
	
	// anisotropic is only in minfilter
	assert(state.mipmap_filter != zz_render_state::ZZ_TEXF_ANISOTROPIC);
	assert(state.mag_filter != zz_render_state::ZZ_TEXF_ANISOTROPIC);
	if (state.mipmap_filter == zz_render_state::ZZ_TEXF_ANISOTROPIC) {
		state.mipmap_filter = zz_render_state::ZZ_TEXF_LINEAR;
	}
	if (state.mag_filter == zz_render_state::ZZ_TEXF_ANISOTROPIC) {
		state.mag_filter = zz_render_state::ZZ_TEXF_LINEAR;
	}

	if ((state.mipmap_filter == zz_render_state::ZZ_TEXF_LINEAR) &&
		!(device_capability.TextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
	{
		state.mipmap_filter = zz_render_state::ZZ_TEXF_POINT;
	}
	if ((state.mipmap_filter == zz_render_state::ZZ_TEXF_POINT) &&
		!(device_capability.TextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT))
	{
		state.mipmap_filter = zz_render_state::ZZ_TEXF_NONE;
	}

	if ((state.min_filter == zz_render_state::ZZ_TEXF_ANISOTROPIC) &&
		!(device_capability.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC))
	{
		state.min_filter = zz_render_state::ZZ_TEXF_LINEAR;
	}
	// for now, we ignore anisotropic filtering
	// CAUTION: 61.77 driver with high-performance option couldn't use anisotropic option.
	// It caused dropping out all sprites.
	if (state.min_filter == zz_render_state::ZZ_TEXF_ANISOTROPIC) {
		state.min_filter = zz_render_state::ZZ_TEXF_LINEAR;
	}
	if ((state.min_filter == zz_render_state::ZZ_TEXF_LINEAR) &&
		!(device_capability.TextureFilterCaps & D3DPTFILTERCAPS_MINFLINEAR))
	{
		state.min_filter = zz_render_state::ZZ_TEXF_POINT;
	}
	if ((state.mag_filter == zz_render_state::ZZ_TEXF_LINEAR) &&
		!(device_capability.TextureFilterCaps & D3DPTFILTERCAPS_MAGFLINEAR))
	{
		state.mag_filter = zz_render_state::ZZ_TEXF_POINT;
	}

	for (int i = 0; i < 4; i++) {
		set_sampler_state( i, ZZ_SAMP_MIPFILTER, state.mipmap_filter );
		set_sampler_state( i, ZZ_SAMP_MINFILTER, state.min_filter );
		set_sampler_state( i, ZZ_SAMP_MAGFILTER, state.mag_filter );

		// for sprite
		{
			set_sampler_state( i, ZZ_SAMP_ADDRESSU, ZZ_TADDRESS_CLAMP );
			set_sampler_state( i, ZZ_SAMP_ADDRESSV, ZZ_TADDRESS_CLAMP );
			set_sampler_state( i, ZZ_SAMP_MAXMIPLEVEL, 0 );
			set_sampler_state( i, ZZ_SAMP_MAXANISOTROPY, ZZ_MIN(device_capability.MaxAnisotropy, 2) );
			set_sampler_state( i, ZZ_SAMP_MIPMAPLODBIAS, 0 );
			set_sampler_state( i, ZZ_SAMP_SRGBTEXTURE, 0 );
		}
	}

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	enable_zbuffer(true);
	enable_zwrite(true);
	set_zfunc(ZZ_CMP_LESSEQUAL);
	enable_alpha_test(false);

	set_fog_range(state.fog_start, state.fog_end);
	set_alpha_fog_range(state.alpha_fog_start, state.alpha_fog_end);

	set_render_state(ZZ_RS_FOGCOLOR, 
		ZZ_TO_D3DXRGB(state.fog_color.x, state.fog_color.y, state.fog_color.z));

	D3DXMATRIX d3d_mat_id;

	D3DXMatrixIdentity( &d3d_mat_id );
	d3d_device->SetTransform(D3DTS_VIEW, &d3d_mat_id);
	d3d_device->SetTransform(D3DTS_WORLD, &d3d_mat_id);

	enable_alpha_blend(true, ZZ_BT_NORMAL);
	enable_alpha_test(true, 128);
}

void zz_renderer_d3d::init_scene ()
{
	zz_assert(zz_shader::check_system_shaders());

	//--------------------------------------------------------------------------------
	// initial setting
	// get the pointer of camera for light-view to cast shadow
	if (!light_camera) {
		light_camera = znzin->get_camera_light();
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// set_camera_position
	// eye position constant setup. default origin
	float eye_position[4] = {0, 0, 0, 1};
	set_vertex_shader_constant(ZZ_VSC_CAMERA_POSITION, eye_position, 1);
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// set_constant
	// constant value (1.0, 0.5, 0.0, 127.9961f);
	float constant_value[4] = {1.0f, 0.5f, 0.0f, 127.9961f};
	set_vertex_shader_constant(ZZ_VSC_ONE_HALF_ZERO_MAXPOWER, constant_value, 1);
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// set_render_target
	if (use_virtual_backbuffer) {
		if (FAILED(d3d_device->SetRenderTarget(0, v_backbuffer_surface))) {
			ZZ_LOG("r_d3d: begin_scene: SetRenderTarget() failed\n");
			return;
		}
		if (FAILED(d3d_device->SetDepthStencilSurface(v_backbuffer_zsurface))) {
			ZZ_LOG("r_d3d: begin_scene(): SetDepthStencilSurface() failed.\n");
			return;
		}
	}
	else {
		//set render target to normal back buffer / depth buffer
		if (FAILED(d3d_device->SetRenderTarget(0, backbuffer_surface))) {
			ZZ_LOG("r_d3d: begin_scene(): SetRenderTarget() failed\n");
			return;
		}
		if (FAILED(d3d_device->SetDepthStencilSurface(backbuffer_zsurface))) {
			ZZ_LOG("r_d3d: begin_scene(): SetDepthStencilSurface() failed.\n");
			return;
		}
	}
	//--------------------------------------------------------------------------------

	//--------------------------------------------------------------------------------
	// get camera 
	// get the default camera for eye-view
	zz_camera * camera = znzin->get_camera();
	//--------------------------------------------------------------------------------
	
	if (camera) {
		//--------------------------------------------------------------------------------
		// set_camera_modelview
		camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, modelview_matrix);
		set_modelview_matrix(modelview_matrix);   //test
		//--------------------------------------------------------------------------------
		
		//--------------------------------------------------------------------------------
		// set_camera_projection
		camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_matrix);
		set_projection_matrix(projection_matrix);
		//--------------------------------------------------------------------------------

		//--------------------------------------------------------------------------------
		// set_shadowmap_texturetm
		if (state.use_shadowmap) {
			shadowmap_texturetm_setup();
		}
		//--------------------------------------------------------------------------------
	}
	else { // without camera
		set_projection_matrix(mat4_id);
		set_modelview_matrix(mat4_id);
	}

	// invalidate caching system
	_cached.invalidate();

	// initialize object and polygon count
	num_polygons_ = 0;
	num_meshes_ = 0;

	d3d_device->SetViewport(&default_viewport);
}

void zz_renderer_d3d::set_fog_range (float fog_start, float fog_end)
{
	float fog_constant[4] = {fog_start, fog_end, (1.0f/(fog_end - fog_start))};
	set_vertex_shader_constant(ZZ_VSC_FOG_START_END_FACTOR, fog_constant, 1);
}

void zz_renderer_d3d::set_alpha_fog_range (float fog_start, float fog_end)
{
	float fog_constant[4] = {fog_start, fog_end, (1.0f/(fog_end - fog_start))};
	set_vertex_shader_constant(ZZ_VSC_ALPHA_FOG_START_END_FACTOR, fog_constant, 1);
}

void zz_renderer_d3d::begin_scene (ZZ_RENDERWHERE render_where)
{
	assert(!_scene_began);

	if (_scene_began) return;

	if (FAILED(d3d_device->BeginScene())) {
		ZZ_LOG("r_d3d: BeginScene() failed\n");
		return;
	}
	_scene_began = true;
	set_render_where(render_where);
}

void zz_renderer_d3d::end_scene ()
{
	assert(_scene_began);

	if (!_scene_began) return;

	//ZZ_LOG("r_d3d: end_scene\n");

	if (FAILED(d3d_device->EndScene())) {
		ZZ_LOG("r_d3d: end_scene()::EndScene() failed\n");
	}
	_scene_began = false;
	set_render_where(ZZ_RW_NONE);
}

int zz_renderer_d3d::get_num_polygons ()
{
	return num_polygons_;
}

int zz_renderer_d3d::get_num_meshes ()
{
	return num_meshes_;
}

void zz_renderer_d3d::end_shadowmap ()
{
	assert(state.use_shadowmap);

	assert(get_render_where() == ZZ_RW_SHADOWMAP);

	if (FAILED(d3d_device->EndScene())) {
		ZZ_LOG("r_d3d: end_shadowmap()::EndScene() failed\n");
	}

	if (state.shadowmap_blur_type > 0) {
		blur_shadowmap();
	}

	// draw smooth cleanning border
	render_shadowmap_overlay();

	// re-set render target
	//set render target to normal back buffer / depth buffer
	if (FAILED(d3d_device->SetRenderTarget(0, backbuffer_surface))) {
		ZZ_LOG("r_d3d: end_shadowmap::SetRenderTarget() failed\n");
		zz_assert(0);
		return;
	}
	if (FAILED(d3d_device->SetDepthStencilSurface(backbuffer_zsurface))) {
		ZZ_LOG("r_d3d: end_shadowmap::SetDepthStencilSurface() failed.\n");
		zz_assert(0);
		return;
	}

	set_render_where(ZZ_RW_NONE);

	// restore viewport
	d3d_device->SetViewport(&old_viewport);

	// restore camera
	assert(saved_camera);              
	znzin->set_camera(saved_camera);  
}

void zz_renderer_d3d::pre_process ()
{
}

// post_process should be called in begin_scene/end_scene block
void zz_renderer_d3d::post_process ()
{
	if (!use_virtual_backbuffer && !state.use_glow && !state.use_glow_fullscene) return;

	// flush scene
	if (_scene_began) {
		_end_scene("post_process");
	}

	if (use_virtual_backbuffer) {
		// re-set render target
		//set render target to normal back buffer / depth buffer
		if (FAILED(d3d_device->SetRenderTarget(0, backbuffer_surface))) {
			ZZ_LOG("r_d3d: post_process()::SetRenderTarget() failed\n");
			zz_assert(0);
			return;
		}
		if (FAILED(d3d_device->SetDepthStencilSurface(backbuffer_zsurface))) {
			ZZ_LOG("r_d3d: post_process()::SetDepthStencilSurface() failed.\n");
			return;
		}

		clear_screen();
		_begin_scene("virtual_backbuffer");
		draw_texture(0.0f, (float)state.screen_width, 0.0f, (float)state.screen_height, v_backbuffer_texture, ZZ_BT_NONE);
		_end_scene("virtual_backbuffer");

		// restart begin-end section
		d3d_device->BeginScene();
	}

	// save min texture memory
	//unsigned int available_texture_mem = d3d_device->GetAvailableTextureMem(); // costs 0.5msec
	//min_texture_memory = (available_texture_mem < min_texture_memory) ? available_texture_mem : min_texture_memory;

	bool use_object_glow = state.use_glow && znzin->scene.get_num_glows();
	bool use_fullscene_glow = state.use_glow_fullscene;

	if (use_object_glow || use_fullscene_glow) {
		znzin->renderer->overlay_glow(use_object_glow, use_fullscene_glow);
	}

	// initializing setupd for sprite
	enable_zbuffer(true);
	enable_zwrite(true);
	set_zfunc(ZZ_CMP_LESSEQUAL);

	// restore begin scene state
	if (_scene_began) {
		_begin_scene("post_process");
	}
}


void zz_renderer_d3d::begin_glow (void)
{
	assert(!_scene_began);

	if (state.use_glow_ztest && glow_backbuffer_surface) {
		if (FAILED(d3d_device->SetRenderTarget(0, glow_backbuffer_surface))) {
			ZZ_LOG("r_d3d: begin_glow: SetRenderTarget() failed\n");
			return;
		}
	}
	else if (glow_downsample_surface) {
		if (FAILED(d3d_device->SetRenderTarget(0, glow_downsample_surface))) {
			ZZ_LOG("r_d3d: begin_glow: SetRenderTarget() failed\n");
			return;
		}
	}
	else {
		state.use_glow = false;
		ZZ_LOG("r_d3d: begin_glow() failed. no glow surface.\n");
		zz_assert(0);
		return;
	}

	// clear color except z-buffer and stencil.
	d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET, 0x0,	1.0f, 0L);

	//save old viewport
	d3d_device->GetViewport(&old_viewport);

	//set new, funky viewport
	if (state.use_glow_ztest) {
		d3d_device->SetViewport(&glow_viewport);
	}
	else {
		d3d_device->SetViewport(&glow_downsample_viewport);
	}

	if (state.use_glow_ztest) {
		enable_zbuffer( true ); // use z-buffer
	}
	else {
		enable_zbuffer( false );
	}

	enable_zwrite( false ); // not to write to z-buffer
}

void zz_renderer_d3d::end_glow (void)
{
	assert(!_scene_began);

	zz_assert(state.use_glow || state.use_glow_fullscene);

	//set_zfunc( ZZ_CMP_LESSEQUAL );
	if (state.use_glow) {
		if (state.use_glow_ztest) {
			downsample_glow(); // downsample *big* glow_backbuffer -> *small* glow_downsample
		}
		blur_glow(); // render glow_downsample_texture into the final glow_blur_texture
	}

	if (state.use_glow_fullscene) {
		downsample_glow_fullscene();
		blur_glow_fullscene();
	}

	// restore viewport
	d3d_device->SetViewport(&old_viewport);

	// re-set render target
	//set render target to normal back buffer / depth buffer
	zz_assert(backbuffer_surface);
	zz_assert(backbuffer_zsurface);

	if (FAILED(d3d_device->SetRenderTarget(0, backbuffer_surface))) {
		ZZ_LOG("r_d3d: end_glow::SetRenderTarget() failed\n");
		return;
	}

	if(znzin->screen_sfx.get_widescreen_mode())       
		znzin->screen_sfx.post_clear_wide();            
		
	// revert to original
	enable_zbuffer( true );
	enable_zwrite( true );
	set_zfunc(ZZ_CMP_LESSEQUAL);
}

void zz_renderer_d3d::overlay_glow (bool object_glow, bool fullscene_glow)
{	
	assert(object_glow || fullscene_glow);

	enable_alpha_test(false);

	_begin_scene("overlay_glow");
	//draw_texture(0.0f, float(state.screen_width)/3.f, .0f, float(state.screen_height)/3.f, glow_blur_texture, ZZ_BT_NONE); // non alpha blend
	//draw_texture(0.0f, 256.0f, 128.0f, 384.0f, glow_backbuffer_texture, ZZ_BT_NONE); // non alpha blend
	//draw_texture(128.0f, 256.0f, .0f, 128.0f, glow_downsample_texture, ZZ_BT_NONE); // non alpha blend
	//draw_texture(128.0f, 256.0f, .0f, 128.0f, glow_downsample_texture, ZZ_BT_NONE); // non alpha blend
	//draw_texture(256.0f, 384.0f, .0f, 128.0f, glow_blur_texture, ZZ_BT_NONE); // non alpha blend
	//draw_texture(256.0f, 384.0f, .0f, 128.0f, glow_blur_texture_fullscene, ZZ_BT_NONE); // non alpha blend
	
	set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD); // additive blending

	if (object_glow) {
		assert(state.use_glow);
		draw_texture(-.5f, (float)state.screen_width-.5f, -.5f, (float)state.screen_height-.5f, glow_blur_texture, ZZ_BT_CUSTOM); // custum alpha blend
	}

	if (fullscene_glow) {
		assert(state.use_glow_fullscene);
		switch (state.fullscene_glow_type) 
		{
		case 0:
			set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD); // additive blending
			break;
		case 1:
			set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD); // additive blending
			break;
		case 2:
			set_blend_type(ZZ_BLEND_INVDESTCOLOR, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD); // additive blending
			break;
		}

		draw_texture(-.5f, (float)state.screen_width-.5f, 0, (float)state.screen_height-.5f, glow_blur_texture_fullscene, ZZ_BT_CUSTOM); // custum alpha blend
	}
	_end_scene("overlay_glow");
}

void zz_renderer_d3d::draw_shadowmap_viewport (void)
{
	_begin_scene("draw_shadowmap_viewport");
	draw_texture(4.5f, 4.5f+128.f, 4.5f, 4.5f + 128.f, shadowmap, ZZ_BT_NONE);
	//draw_texture(4.5f, 4.5f+128.f, 4.5f, 4.5f + 128.f, shadowmap_forblur, ZZ_BT_NONE);
	_end_scene("draw_shadowmap_viewport");
}

// should be in between beginScene() and endScene() block
void zz_renderer_d3d::draw_texture (float left, float right, float bottom, float top, LPDIRECT3DTEXTURE9 texture, ZZ_BLEND_TYPE blend_type)
{	
	TOVERLAY_VERTEX* dest_vertex;

	if (!texture) return;

	if (!overlay_vb) return;

	if (FAILED(overlay_vb->Lock( 0, 0, (void**)&dest_vertex,
		D3DLOCK_DISCARD ))) // NO D3DLOCK_NOOVERWRITE!
		return;
	
	dest_vertex[0].p = D3DXVECTOR4( left, top, 0.0f, 1.0f );

	dest_vertex[0].tu = 0.0f;
	dest_vertex[0].tv = 1.0f;
	dest_vertex[1].p = D3DXVECTOR4( right, top, 0.0f, 1.0f );

	dest_vertex[1].tu = 1.0f;
	dest_vertex[1].tv = 1.0f;
	dest_vertex[2].p = D3DXVECTOR4( left, bottom, 0.0f, 1.0f );

	dest_vertex[2].tu = 0.0f;
	dest_vertex[2].tv = 0.0f;
	dest_vertex[3].p = D3DXVECTOR4( right, bottom, 0.0f, 1.0f );

	dest_vertex[3].tu = 1.0f;
	dest_vertex[3].tv = 0.0f;

	if (FAILED(overlay_vb->Unlock())) return;

	enable_fog(false);
	enable_zbuffer(false);
	enable_zwrite(false);

	d3d_device->SetTexture( 0, texture );
	_cached.invalidate_texture( 0 );

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	d3d_device->SetFVF(TOVERLAY_VERTEX_FVF);
	d3d_device->SetStreamSource( 0, overlay_vb, 0, sizeof(TOVERLAY_VERTEX));

	zz_renderer_cached_info& cached = znzin->renderer->get_cached_info();
	cached.invalidate(zz_renderer_cached_info::VERTEX_BUFFER);
	
	d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLOROP,   ZZ_TOP_SELECTARG1 );
	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_DISABLE );

	if (blend_type == ZZ_BT_NONE) {
		set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );
		enable_alpha_blend(false, ZZ_BT_NONE);
	}
	else {
		enable_alpha_blend(true, blend_type);
		set_texture_stage_state( 0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE );
		set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_SELECTARG1 );
		set_texture_stage_state( 1, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );
	}

	//set_sampler_state(0, ZZ_SAMP_MINFILTER, zz_render_state::zz_texture_filter_type(1));
	//set_sampler_state(0, ZZ_SAMP_MAGFILTER, zz_render_state::zz_texture_filter_type(1));

	try {
		if (FAILED(d3d_device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 )))
			return;
	}
	catch (...) {
		// do nothing. maybe device lost state.
		ZZ_LOG("r_d3d: draw_texture(). dp exception\n");
	}
}

// sub-function of blur_map()
vec3 read_pixel_rel (unsigned char * p, int x, int y, int width, int pitch)
{
	static float kernel[] =
	{
		1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
		1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f,
		1.0f/9.0f, 1.0f/9.0f, 1.0f/9.0f
	};
	//static float kernel[] =
	//{
	//	.04f, .04f, .04f, .04f, .04f,
	//	.04f, .04f, .04f, .04f, .04f,
	//	.04f, .04f, .04f, .04f, .04f,
	//	.04f, .04f, .04f, .04f, .04f,
	//	.04f, .04f, .04f, .04f, .04f
	//};

#define FILTERSIZE (3)

	vec3 src[FILTERSIZE*FILTERSIZE];
	const int kernel_length = (FILTERSIZE-1)/2; // 3x3
	const int kernel_size = FILTERSIZE;
	int cur_x, cur_y;
	int kernel_index = 0;
	for (int j = -kernel_length; j <= kernel_length; j++) {
		for (int i = -kernel_length; i <= kernel_length; i++) {
			if ((x+i) < 0 || (y+j) < 0 || (x+i) >= width || (y+j) >= width) {
				cur_x = x;
				cur_y = y;
			}
			else {
				cur_x = x + i;
				cur_y = y + j;
			}
			src[kernel_index].x = 
				float(p[pitch*cur_y + cur_x*4])*kernel[kernel_index];
			src[kernel_index].y = 
				float(p[pitch*cur_y + cur_x*4 + 1])*kernel[kernel_index];
			src[kernel_index].z = 
				float(p[pitch*cur_y + cur_x*4 + 2])*kernel[kernel_index];
			kernel_index++;
		}
	}
	vec3 result = vec3_null;
	for (int i = 0; i < kernel_size*kernel_size; i++) {
		result.x += src[i].x;
		result.y += src[i].y;
		result.z += src[i].z;
	}
	return result;
}

void blur_map (unsigned char * bits, int map_size, int pitch)
{
	// read shadowmap_pixelsels
	assert(shadowmap_pixels);

	int x, y;
	for (y = 0; y < map_size; ++y) {
		for (x = 0; x < map_size; ++x) {
			shadowmap_pixels[y*map_size + x] = read_pixel_rel(bits, x, y, map_size, pitch);
		}
	}
	// write shadowmap_pixelsels
	int index = 0;
	for (y = 0; y < map_size; ++y) {
		for (x = 0; x < map_size; ++x) {
			// blue
			bits[index++] = (unsigned char)shadowmap_pixels[y*map_size + x].x;
			// green
			bits[index++] = (unsigned char)shadowmap_pixels[y*map_size + x].y;
			// red
			bits[index++] = (unsigned char)shadowmap_pixels[y*map_size + x].z;
			// alpha
			bits[index++] = 0x00;
		}
		index += pitch - (map_size*4);
	}
}


void zz_renderer_d3d::blur_shadowmap (int repeat_count)
{	
	//ZZ_PROFILER_INSTALL(Pblur_shadowmap);

	assert(shadowmap_vb);

	if (!shadowmap_vb) return;

	// switch render target to shadowmap surface
	if (FAILED(d3d_device->SetRenderTarget(0, shadowmap_surface))) {
		ZZ_LOG("r_d3d: begin_scene:SetRenderTarget() failed\n");
		return;
	}

	if (FAILED(d3d_device->BeginScene())) {
		ZZ_LOG("r_d3d: blur_shadowmap:BeginScene() failed.\n");
		return;
	}

	// shadowmap background color
	if (state.use_blackshadowmap) {
		d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET, 0x0,	1.0f, 0L);
	}
	else {
		d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET, 0xFFFFFFFF,	1.0f, 0L);
	}

	enable_alpha_blend(true, ZZ_BT_LIGHTEN);

	if (state.use_blackshadowmap) {
		set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD);
	}
	else {
		// CAUTION: if D3DPMISCCAPS_BLENDOP is not supported, then ADD is performed.
		assert(device_capability.PrimitiveMiscCaps & D3DPMISCCAPS_BLENDOP);
		set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_REVSUBTRACT);
	}

	enable_alpha_test(false);
	enable_fog(false);
	enable_zwrite(false);

	d3d_device->SetTexture( 0, shadowmap_forblur );
	_cached.invalidate_texture( 0 );
	set_vertex_shader( ZZ_HANDLE_NULL );
	set_pixel_shader( ZZ_HANDLE_NULL );
	d3d_device->SetFVF(TOVERLAY_VERTEX_FVF);
	d3d_device->SetStreamSource( 0, shadowmap_vb, 0, sizeof(TOVERLAY_VERTEX));
	d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	zz_renderer_cached_info& cached = znzin->renderer->get_cached_info();
	cached.invalidate(zz_renderer_cached_info::VERTEX_BUFFER);

	vec3 inverse_color = znzin->get_rs()->shadowmap_color;
	inverse_color = 1.0f - inverse_color;
	set_render_state(ZZ_RS_TEXTUREFACTOR, ZZ_TO_D3DRGBA(inverse_color.x, inverse_color.y, inverse_color.z, 1.0f));
	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE );
	set_texture_stage_state( 0, ZZ_TSS_COLOROP,   ZZ_TOP_MODULATE );
	set_texture_stage_state( 1, ZZ_TSS_COLORARG1, ZZ_TA_CURRENT );
	set_texture_stage_state( 1, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR );
	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_MODULATE );

	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_DISABLE );
	set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );

	// first *2* = two triangle per rectangle
	// second *2* = include degenerated polygons
	// last *2* = skip last degenerated polygon
	try {
		if (FAILED(d3d_device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*NUM_SHADOW_BLUR*2 - 2 )))
			return;
	}
	catch (...) {
		// do nothing. maybe device lost state
		ZZ_LOG("r_d3d: blur_shadowmap(). dp exception\n");
	}

	if (FAILED(d3d_device->EndScene())) {
		ZZ_LOG("r_d3d: blur_shadowmap:EndScene() failed.\n");
		return;
	}
}

void zz_renderer_d3d::downsample_glow ()
{
	assert(state.use_glow_ztest);
	assert(glow_backbuffer_surface);

	HRESULT hr;
	D3DTEXTUREFILTERTYPE tex_filter = D3DTEXF_NONE;

	if (device_capability.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) {
		tex_filter = D3DTEXF_LINEAR;
	}
	else if (device_capability.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFPOINT) {
		tex_filter = D3DTEXF_POINT;
	}

	if (FAILED(hr =d3d_device->StretchRect( glow_backbuffer_surface, NULL /* entire */,
		glow_downsample_surface, NULL/* entire */,
		tex_filter /* filter */
		)))
	{
		ZZ_LOG("r_d3d: downsample_glow() failed.[%s]\n", get_hresult_string(hr));
		return;
	}
}

void zz_renderer_d3d::downsample_glow_fullscene ()
{
	zz_assert(backbuffer_surface);
	zz_assert(glow_downsample_surface);

	HRESULT hr;
	D3DTEXTUREFILTERTYPE tex_filter = D3DTEXF_NONE;

	if (device_capability.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFLINEAR) {
		tex_filter = D3DTEXF_LINEAR;
	}
	else if (device_capability.StretchRectFilterCaps & D3DPTFILTERCAPS_MINFPOINT) {
		tex_filter = D3DTEXF_POINT;
	}

	if (FAILED(hr =d3d_device->StretchRect( backbuffer_surface, NULL /* entire */,
		glow_downsample_surface, NULL/* entire */,
		tex_filter /* filter */
		)))
	{
		ZZ_LOG("r_d3d: downsample_glow_fullscene() failed.[%s]\n", get_hresult_string(hr));
		return;
	}
}

void zz_renderer_d3d::blur_glow ()
{	
	// switch render target to glow blur surface
	if (FAILED(d3d_device->SetRenderTarget(0, glow_blur_surface))) {
		ZZ_LOG("r_d3d: blur_glow()::SetRenderTarget() failed\n");
		zz_assert(0);
		return;
	}

	//set new, funky viewport
	d3d_device->SetViewport(&glow_downsample_viewport);

	// glow background color
	d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET, 0x0, 1.0f, 0L);

	enable_alpha_blend(true, ZZ_BT_CUSTOM);

	set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD);
	
	enable_alpha_test(false);
	enable_fog(false);
	enable_zwrite(false);
	enable_zbuffer(false);

	d3d_device->SetTexture( 0, glow_downsample_texture );
	_cached.invalidate_texture( 0 );
	set_vertex_shader( ZZ_HANDLE_NULL );
	set_pixel_shader( ZZ_HANDLE_NULL );
	d3d_device->SetFVF(TOVERLAY_VERTEX_FVF);
	d3d_device->SetStreamSource( 0, glow_vb, 0, sizeof(TOVERLAY_VERTEX));
	d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	zz_renderer_cached_info& cached = get_cached_info();
	cached.invalidate(zz_renderer_cached_info::VERTEX_BUFFER);

	set_sampler_state(0, ZZ_SAMP_ADDRESSU,  ZZ_TADDRESS_CLAMP);
	set_sampler_state(0, ZZ_SAMP_ADDRESSV,  ZZ_TADDRESS_CLAMP);

	set_render_state(ZZ_RS_TEXTUREFACTOR, ZZ_TO_D3DRGBA(state.glow_color.x, state.glow_color.y, state.glow_color.z, 1.0f));
	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR );
	set_texture_stage_state( 0, ZZ_TSS_COLOROP,   ZZ_TOP_MODULATE );
	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_DISABLE );
	set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );

	_begin_scene("blur_glow");

	// first *2* = two triangle per rectangle
	// second *2* = include degenerated polygons
	// last *2* = skip last degenerated polygon
	try {
		if (FAILED(d3d_device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*NUM_GLOW_BLUR*2 - 2 )))
			return;
	}
	catch (...) {
		// do nothing. maybe device lost state
		ZZ_LOG("r_d3d: blur_glow(). dp exception\n");
	}

	_end_scene("blur_glow");
}


void zz_renderer_d3d::blur_glow_fullscene ()
{	
	zz_assert(glow_blur_surface_fullscene);

	// switch render target to glow blur surface
	if (FAILED(d3d_device->SetRenderTarget(0, glow_blur_surface_fullscene))) {
		ZZ_LOG("r_d3d: blur_glow_fullscreen::SetRenderTarget() failed\n");
		zz_assert(0);
		return;
	}

	//set new, funky viewport
	d3d_device->SetViewport(&glow_downsample_viewport);

	// glow background color
	d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET, 0x0,	1.0f, 0L);

	enable_alpha_blend(true, ZZ_BT_CUSTOM);

	set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD);
	
	enable_alpha_test(false);
	enable_fog(false);
	enable_zwrite(false);
	enable_zbuffer(false);

	d3d_device->SetTexture( 0, glow_downsample_texture );
	_cached.invalidate_texture( 0 );
	set_vertex_shader( ZZ_HANDLE_NULL );
	set_pixel_shader( ZZ_HANDLE_NULL );
	d3d_device->SetFVF(TOVERLAY_VERTEX_FVF);
	d3d_device->SetStreamSource( 0, glow_vb, 0, sizeof(TOVERLAY_VERTEX));
	d3d_device->SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );
	
	zz_renderer_cached_info& cached = get_cached_info();
	cached.invalidate(zz_renderer_cached_info::VERTEX_BUFFER);

	set_sampler_state(0, ZZ_SAMP_ADDRESSU,  ZZ_TADDRESS_CLAMP);
	set_sampler_state(0, ZZ_SAMP_ADDRESSV,  ZZ_TADDRESS_CLAMP);

	D3DCOLOR tfactor = ZZ_TO_D3DRGBA(
		state.glow_color_fullscene.x, state.glow_color_fullscene.y, state.glow_color_fullscene.z, 1.0f);

	set_render_state(ZZ_RS_TEXTUREFACTOR, tfactor);

	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR );
	set_texture_stage_state( 0, ZZ_TSS_COLOROP,   ZZ_TOP_MODULATE );
	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_DISABLE );
	set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );

	//set_sampler_state(0, ZZ_SAMP_MINFILTER, zz_render_state::zz_texture_filter_type(1));
	//set_sampler_state(0, ZZ_SAMP_MAGFILTER, zz_render_state::zz_texture_filter_type(1));

	_begin_scene("blur_glow_fullscene");

	// first *2* = two triangle per rectangle
	// second *2* = include degenerated polygons
	// last *2* = skip last degenerated polygon
	try {
		if (FAILED(d3d_device->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2*NUM_GLOW_BLUR*2 - 2 )))
			return;
	}
	catch (...) {
		// do nothing. maybe device lost state
		ZZ_LOG("r_d3d: blur_glow_fullscreen(). dp exception\n");
	}

	_end_scene("blur_glow_fullscene");
}

void zz_renderer_d3d::set_texture_shadowmap (int shadowmap_stage)
{
	_cached.invalidate_texture(shadowmap_stage);

	//set depth map as texture
	if ((state.use_shadowmap) && (get_render_where() != ZZ_RW_SHADOWMAP)) {
		if (FAILED(d3d_device->SetTexture(shadowmap_stage, shadowmap))) {
			ZZ_LOG("r_d3d: begin_scene()::SetTexture(shadowmap) failed\n");
		}
	}
	else { // no shadowmap support, then disable texture
		d3d_device->SetTexture(shadowmap_stage, 0);
	}
}

D3DTEXTUREOP d3d_textureop_table[] = {
    D3DTOP_DISABLE,
    D3DTOP_SELECTARG1,
    D3DTOP_SELECTARG2,
    D3DTOP_MODULATE,
    D3DTOP_MODULATE2X,
    D3DTOP_MODULATE4X,
    D3DTOP_ADD,
    D3DTOP_ADDSIGNED,
    D3DTOP_ADDSIGNED2X,
    D3DTOP_SUBTRACT,
    D3DTOP_ADDSMOOTH,
    D3DTOP_BLENDDIFFUSEALPHA,
    D3DTOP_BLENDTEXTUREALPHA,
    D3DTOP_BLENDFACTORALPHA,
    D3DTOP_BLENDTEXTUREALPHAPM,
    D3DTOP_BLENDCURRENTALPHA,
    D3DTOP_PREMODULATE,
    D3DTOP_MODULATEALPHA_ADDCOLOR,
    D3DTOP_MODULATECOLOR_ADDALPHA,
    D3DTOP_MODULATEINVALPHA_ADDCOLOR,
    D3DTOP_MODULATEINVCOLOR_ADDALPHA,
    D3DTOP_BUMPENVMAP,
    D3DTOP_BUMPENVMAPLUMINANCE,
    D3DTOP_DOTPRODUCT3,
    D3DTOP_MULTIPLYADD,
    D3DTOP_LERP,
    D3DTOP_FORCE_DWORD,
};

void zz_renderer_d3d::set_light (zz_light * light)
{
	assert(light);

	static zz_light * last_light = NULL;

	if (_cached.light && (last_light == light)) {
		return; // skip same light
	}

	last_light = light; // set new light to saved last light
	_cached.light = true;

	//--------------------------------------------------------------------------------
	// default setting
	//--------------------------------------------------------------------------------
	// diffuse
	set_vertex_shader_constant(ZZ_VSC_LIGHT_DIFFUSE, &light->diffuse.x, 1);

	// ambient
	set_vertex_shader_constant(ZZ_VSC_LIGHT_AMBIENT, &light->ambient.x, 1);

	// position
	set_vertex_shader_constant(ZZ_VSC_LIGHT_POSITION, &light->position.x, 1);

	//--------------------------------------------------------------------------------
	// directional light
	//--------------------------------------------------------------------------------
	if (IS_A(light, zz_light_direct)) {
		zz_light_direct * light_direct = static_cast<zz_light_direct*>(light);

		zz_camera * camera = znzin->get_camera();
		assert(camera);

		camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, modelview_matrix);
		mat3 modelview3x3;
		modelview_matrix.get_rot(modelview3x3);
		vec3 direction_world3;
		mult(direction_world3, modelview3x3, vec3(light_direct->direction));

		direction_world3.normalize(); // normalize
		vec4 direction_world4(direction_world3.x, direction_world3.y, direction_world3.z, 1);

		set_vertex_shader_constant(ZZ_VSC_LIGHT_DIRECTION, &direction_world4.x, 1);
	}

	//--------------------------------------------------------------------------------
	// point light
	//--------------------------------------------------------------------------------
	if (IS_A(light, zz_light_point)) {
		zz_light_point * light_point = static_cast<zz_light_point*>(light);

		//float start = .0f;
		//float end = 30.f;
		//vec4 attenuation(1.0f - 10.0f*start/(end - start), 10.0f / (end - start), 0.f, .0f);
		set_vertex_shader_constant(ZZ_VSC_LIGHT_ATTENUATION, &light_point->attenuation.x, 1);
	}
}

bool zz_renderer_d3d::set_vertex_shader (zz_handle shader_index)
{
	static zz_handle last_shader_index = ZZ_HANDLE_NULL;

	if (_cached.vertex_shader) {
		if (last_shader_index == shader_index) {
			// cached
			//ZZ_LOG("r_d3d: set_vertex_shader(%d) cached\n", shader_index);
			return true;
		}
		else {
			//ZZ_LOG("r_d3d: set_vertex_shader(%d) not cached.\n", shader_index);
		}
	}
	last_shader_index = shader_index;
#ifdef ZZ_USECACHE
	_cached.vertex_shader = true;
#endif

	if (!ZZ_HANDLE_IS_VALID(shader_index)) {
		d3d_device->SetVertexShader(NULL);
		return true;
	}

	if (FAILED(d3d_device->SetVertexDeclaration(vertex_decls[shader_index]))) {
		ZZ_LOG("r_d3d: SetVertexDeclaration() failed\n");
		return false;
	}
	if (FAILED(d3d_device->SetVertexShader(vertex_shaders[shader_index]))) {
		ZZ_LOG("r_d3d: SetVertexShader() failed\n");
		return false;
	}
	return true;
}

bool zz_renderer_d3d::set_pixel_shader (zz_handle shader_index)
{
	static zz_handle last_shader_index = ZZ_HANDLE_NULL;

	if (_cached.pixel_shader) {
		if (last_shader_index == shader_index) {
			// cached
			return true;
		}
	}
	last_shader_index = shader_index;
#ifdef ZZ_USECACHE
	_cached.pixel_shader = true;
#endif

	if (!ZZ_HANDLE_IS_VALID(shader_index)) {
		d3d_device->SetPixelShader(NULL);
		return true;
	}
	if (get_render_where() == ZZ_RW_SHADOWMAP) {
		zz_shader * shadow_shader = (zz_shader*)znzin->shaders->find("shader_shadowmap_skin");
		assert(shadow_shader);
		if (shadow_shader) {
			shader_index = shadow_shader->get_pshader(state.current_pass);
		}	
	}
	if (FAILED(d3d_device->SetPixelShader(pixel_shaders[shader_index]))) {
		ZZ_LOG("r_d3d: SetPixelShader() failed\n");
		return false;
	}
	return true;
}

bool zz_renderer_d3d::set_stream_buffer (zz_mesh * mesh)
{
	static zz_handle last_vhandle, last_ihandle;

	int vertex_size = mesh->get_vertex_size();

	zz_handle vhandle = mesh->get_vbuffer_handle();
	zz_handle ihandle = mesh->get_ibuffer_handle();

	assert(ZZ_HANDLE_IS_VALID(vhandle));
	assert(ZZ_HANDLE_IS_VALID(ihandle));

	static int num_vertex_buffer_call = 0;
	static int num_index_buffer_call = 0;

	if ((_cached.vertex_buffer)&&(last_vhandle == vhandle)) {
	}
	else {
		LPDIRECT3DVERTEXBUFFER9 d3d_buffer = vertex_buffer_pool[vhandle];
		if (FAILED(d3d_device->SetStreamSource(
			0,          /* stream number */
			d3d_buffer, /* pstreamdata */
			0,          /* offset, in bytes */
			vertex_size /* stride, in bytes */
			)))
		{
			ZZ_LOG("r_d3d: SetStreamSource() failed\n");
			return false;
		}
	}

	if ((_cached.index_buffer)&&(last_ihandle == ihandle)) {
	}
	else {
		if (FAILED(d3d_device->SetIndices(
			index_buffer_pool[ihandle])))
		{
			ZZ_LOG("r_d3d: SetIndices() failed\n");
			return false;
		}
	}

#ifdef ZZ_USECACHE
	last_vhandle = vhandle;
#endif

#ifdef ZZ_USECACHE
	last_ihandle = ihandle;
#endif

	return true;
}


bool zz_renderer_d3d::set_material (zz_material * material)
{
	assert(material);

	const zz_shader * shader = material->get_shader();
	assert(shader);

	// check it is invalid path
	int shader_format = material->get_shader_format();
	if (shader_format == SHADER_FORMAT_INVALID)
		return false; // not supported shader or shader pass

	// If the material->set() method was not completed properly, do not draw primitives.
	if (!material->set(state.current_pass)) {
		return false; // if false, do not render
	}

	//ZZ_LOG("r_d3d: render(). mesh(%s), material(%s)\n", mesh->get_name(), material->get_name());

	ZZ_RENDERWHERE render_where = get_render_where();

	zz_handle vshader_index(ZZ_HANDLE_NULL), pshader_index(ZZ_HANDLE_NULL);

	vshader_index = shader->get_vshader(shader_format, render_where);
	pshader_index = shader->get_pshader(shader_format, render_where);

	assert(vshader_index != ZZ_HANDLE_NULL);

	set_vertex_shader(vshader_index);
	set_pixel_shader(pshader_index);

	return true;
}

//-------------------------------------------------------------------------------------
// render
//-------------------------------------------------------------------------------------
void zz_renderer_d3d::render (zz_mesh * mesh, zz_material * material, zz_light * light)
{
	assert(mesh);

	if (material) {
		if (!set_material(material))
			return;
	}
	
	if (light) set_light(light); // light can be null for terrain_block

	set_stream_buffer(mesh);

	// add polygon and object count
	num_polygons_ += mesh->get_num_faces();
	++num_meshes_;
	
	//ZZ_LOG("r_d3d: render #% 6d (%s-%s)\n", mesh->get_num_faces(), mesh->get_path(), mesh->get_name() );
	
#if defined(_DEBUG)
	//validate_device();
#endif
	
	switch (mesh->get_index_type())
	{
	case zz_mesh::TYPE_LIST:
		draw_indexed_trilist(mesh);
		break;
	case zz_mesh::TYPE_STRIP:
		draw_indexed_tristrip(mesh);
		break;
	default:
		assert(!"invalid index type");
		ZZ_LOG("r_d3d: render(%s) invalid index type\n", mesh->get_path());
		break;
	}


}



//--------------------------------------------------------------------------------
// draw indexed triangle strip
//--------------------------------------------------------------------------------
bool zz_renderer_d3d::draw_indexed_tristrip (zz_mesh * mesh)
{
	// get the number of vertices and faces
	UINT num_verts = (UINT)mesh->get_num_verts();
	UINT num_prims = (UINT)mesh->get_num_indices() - 2;

	int base_vertex_index = 0; // mesh->get_vbuffer_block_index()*mesh->get_num_verts();

	HRESULT result;
	try {
		result = d3d_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLESTRIP				/* type              */
			,base_vertex_index /* base vertex index */
			,0								/* min index         */
			,num_verts						/* num vertices      */
			,0					/* start index       */
			,num_prims	/* number of primitives */
			);
	}
	catch (...) {
		ZZ_LOG("r_d3d: drawindexedprimitive(%s:tristrip) failed.\n", mesh ? mesh->get_path() : "n/a");
		ZZ_LOG("...mesh(%s), num_verts(%d), num_prims(%d)\n", mesh ? mesh->get_name() : "n/a", num_verts, num_prims);

		// exception occured
		// this issue from directx_oct_2004sdk:dxreadme.htm#WhatsNew#Direct3D#Issues:
		// maybe mesh's index/vertex buffer is pool_default, but driver created this as pool_system, and
		// internally this resources were released and referenced in the drawindexedprimitive() call.
		// we have to release this resource and load in the next frame
		mesh->unbind_device();
		mesh->bind_device();
		return false;
	}

	if (FAILED(result))
	{
		ZZ_LOG("r_d3d: drawindexedprimitive(%s:tristrip) failed. %s\n", mesh ? mesh->get_path() : "n/a", get_hresult_string(result));
		ZZ_LOG("...mesh(%s), num_verts(%d), num_prims(%d)\n", mesh ? mesh->get_name() : "n/a", num_verts, num_prims);
		return false;
	}

	return true;
}


//--------------------------------------------------------------------------------
// draw indexed triangle list
//--------------------------------------------------------------------------------
bool zz_renderer_d3d::draw_indexed_trilist (zz_mesh * mesh)
{
	// get the number of vertices and faces
	UINT num_verts = (UINT)mesh->get_num_verts();
	UINT num_prims = (UINT)mesh->get_num_faces();

	// code for eye-blinking
	UINT num_prims_subtracted(0), start_index(0);
	zz_clip_face clip_face = mesh->get_clip_face();
	switch (clip_face) {
		case ZZ_CLIP_FACE_FIRST:
			// multiply by 3, because this is just index, not a number of primitives
			start_index = 3*mesh->get_num_clip_faces();
			num_prims_subtracted = mesh->get_num_clip_faces();
			break;
		case ZZ_CLIP_FACE_LAST:
			num_prims_subtracted = mesh->get_num_clip_faces();
			break;
		case ZZ_CLIP_FACE_BOTH:
			start_index = 3*mesh->get_num_clip_faces();
			num_prims_subtracted = mesh->get_num_clip_faces()*2; // 2 means both
			break;
	}

	int base_vertex_index = 0; // mesh->get_vbuffer_block_index()*mesh->get_num_verts();

	HRESULT result;
	try {
		result = d3d_device->DrawIndexedPrimitive(
			D3DPT_TRIANGLELIST				/* type              */
			,base_vertex_index /* base vertex index */
			,0								/* min index         */
			,num_verts						/* num vertices      */
			,start_index					/* start index       */
			,(num_prims - num_prims_subtracted)	/* number of primitives */
			);
	}
	catch (...) {
		ZZ_LOG("r_d3d: drawindexedprimitive(%s:trilist) failed.\n", mesh ? mesh->get_path() : "n/a");
		ZZ_LOG("...mesh(%s), num_verts(%d), num_prims(%d)\n", mesh ? mesh->get_name() : "n/a", num_verts, num_prims);

		// exception occured
		// this issue from directx_oct_2004sdk:dxreadme.htm#WhatsNew#Direct3D#Issues:
		// maybe mesh's index/vertex buffer is pool_default, but driver created this as pool_system, and
		// internally this resources were released and referenced in the drawindexedprimitive() call.
		// we have to release this resource and load in the next frame
		mesh->unbind_device();
		mesh->bind_device();
		return false;
	}

	if (FAILED(result))
	{
		ZZ_LOG("r_d3d: drawindexedprimitive(%s:trilist) failed. %s\n", mesh ? mesh->get_path() : "n/a", get_hresult_string(result));
		ZZ_LOG("...mesh(%s), num_verts(%d), num_prims(%d)\n", mesh ? mesh->get_name() : "n/a", num_verts, num_prims);
		return false;
	}

	return true;
}

void zz_renderer_d3d::clear_screen ()
{
	d3d_device->Clear(0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
		ZZ_TO_D3DRGBA(state.clear_color.x, state.clear_color.y, state.clear_color.z, .0f),
		1.0f, 0L);
}

void zz_renderer_d3d::clear_zbuffer ()
{
	d3d_device->Clear(0L, NULL, D3DCLEAR_ZBUFFER, 0, 1.0f, 0L);
}

void zz_renderer_d3d::wait_device_lost ()
{
	zz_assert(d3d_device);

	HRESULT hr = d3d_device->TestCooperativeLevel();

	do {
		::Sleep( 100 ); // Yield CPU to other process
	}
	while (D3DERR_DEVICELOST == hr);

	if (hr == D3DERR_OUTOFVIDEOMEMORY) {
		throw "out of videomemory";
	}
}

// returns device_lost state value
bool zz_renderer_d3d::reset_device ()
{
	if (_device_lost == false) { // _device_lost variable can be set in swap_buffers
		return false;
	}

	// Yield some CPU time to other processes
	::Sleep( 100 ); // Yield CPU to other process

	HRESULT hr = d3d_device->TestCooperativeLevel();

	assert(_device_lost);
	// If the device was lost, do not render until we get it back
	if ( D3DERR_DEVICELOST == hr ) {
		ZZ_LOG("r_d3d: d3d_device lost\n");
	}
	// Check if the device needs to be reset.
	else if ( D3DERR_DEVICENOTRESET == hr ) {
		ZZ_LOG("r_d3d: d3d_device not reset\n");

		if (!znzin->flush_delayed(true /* entrance */, true /* exit */)) {
			ZZ_LOG("r_d3d: swap_buffers() failed. flush_delayed() failed.\n");
		}
		ZZ_LOG("...1");

		if (!invalidate_device_objects()) { // invalidate default pool objects
			ZZ_LOG("r_d3d: invalidate_device_objects1() failed\n");
			throw "invalidate_device_objects() failed";
		}
		ZZ_LOG("-2");

		if (!znzin->invalidate_device_objects()) { // invalidate default pool objects
			ZZ_LOG("r_d3d: invalidate_device_objects2() failed\n");
			throw "invalidate_device_objects() failed";
		}
		ZZ_LOG("-3");

		// Reset the device
		if( FAILED( hr = d3d_device->Reset( &_parameters ) ) ) {
			ZZ_LOG("r_d3d: d3d_device reset failed. %s\n", get_hresult_string(hr));
			throw "device reset() failed";
		}
		ZZ_LOG("-4");

		if (!restore_device_objects()) { // restore default pool objects
			ZZ_LOG("r_d3d: restore_device_objects1() failed\n");
			throw "restore_device_objcets() failed";
		}
		ZZ_LOG("-5");

		if (!znzin->restore_device_objects()) { // restore default pool objects
			ZZ_LOG("r_d3d: restore_device_objects2() failed\n");
			throw "restore_device_objcets() failed";
		}
		ZZ_LOG("-6");

		ZZ_LOG("r_d3d: d3d_device reset done.\n");
		_device_lost = false; // device returned
	}
	else if ( D3DERR_OUTOFVIDEOMEMORY == hr ) {
		ZZ_LOG("r_d3d: reset_devicer() failed. out of memory!\n");
		throw "out of memory";
	}
	else {
		_device_lost = false; // device returned
	}
	return _device_lost;
}

void zz_renderer_d3d::swap_buffers (HWND hwnd)
{
	HRESULT hr = d3d_device->Present(NULL, NULL, hwnd, NULL);

	if( D3DERR_DEVICELOST == hr ) {
		ZZ_LOG("r_d3d: swap_buffers() device lost.\n");
		_device_lost = true;
	}
}

void zz_renderer_d3d::set_view (zz_view * view_to_set)
{
	view = static_cast<zz_view_d3d *>(view_to_set);
}

zz_view * zz_renderer_d3d::get_view ()
{
	return static_cast<zz_view *>(view);
}

int zz_renderer_d3d::create_vertex_shader (
	const char * shader_file_name,
	int vertex_format,
	bool is_binary)
{
	assert(d3d_device);
	if (!d3d_device) {
		ZZ_LOG("r_d3d: create_vertex_shader(%s) failed. no d3d_device.\n", shader_file_name);
		return 0;
	}

	assert(shader_file_name);
	if (!shader_file_name) {
		ZZ_LOG("r_d3d: create_vertex_shader(%s) failed. no shader_file_name.\n");
		return 0;
	}

	static D3DVERTEXELEMENT9 refer_decl[] =
	{
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0,  4, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0},
		{0, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0},
		{0, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDINDICES, 0},
		{0, 12, D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT , 0},
		{0,  8, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0,  8, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 1},
		{0,  8, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 2},
		{0,  8, D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 3},
		D3DDECL_END()
	};

	LPD3DXBUFFER data_buffer = NULL;
	DWORD * shader_buffer_binary;
	D3DVERTEXELEMENT9 * decl;
	int element_size = 0; // position
	element_size += (vertex_format & ZZ_VF_POSITION) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_NORMAL) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_COLOR) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_BLEND_WEIGHT) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_BLEND_INDEX) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_TANGENT) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_UV0) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_UV1) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_UV2) ? 1 : 0;
	element_size += (vertex_format & ZZ_VF_UV3) ? 1 : 0;
	
	decl = zz_new D3DVERTEXELEMENT9[element_size+1]; // include D3DDECL_END()
	int current_element = 0;
	int current_offset = 0;
	int offset = 0;
	if (vertex_format & ZZ_VF_POSITION) {
        decl[current_element] = refer_decl[0];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_NORMAL) {
		decl[current_element] = refer_decl[1];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_COLOR) {
		decl[current_element] = refer_decl[2];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_BLEND_WEIGHT) {
		decl[current_element] = refer_decl[3];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_BLEND_INDEX) {
		decl[current_element] = refer_decl[4];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_TANGENT) {
		decl[current_element] = refer_decl[5];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_UV0) {
		decl[current_element] = refer_decl[6];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_UV1) {
		decl[current_element] = refer_decl[7];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_UV2) {
		decl[current_element] = refer_decl[8];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	if (vertex_format & ZZ_VF_UV3) {
		decl[current_element] = refer_decl[9];
		offset = decl[current_element].Offset;
		decl[current_element].Offset = current_offset;
		current_offset += offset;
		current_element++;
	}
	decl[current_element] = refer_decl[10]; // end

	int vertex_shader_index = vertex_shaders.add(NULL);
	int vertex_decl_index = vertex_decls.add(NULL);
	assert(vertex_shader_index == vertex_decl_index);
	
	if (FAILED(d3d_device->CreateVertexDeclaration(decl, &(vertex_decls[vertex_decl_index])))) {
		ZZ_LOG("r_d3d: create_vertex_shader()::CreateVertexDeclaration() failed\n");
		ZZ_SAFE_DELETE(decl);
        return -1;
    }
	ZZ_SAFE_DELETE(decl);

	zz_vfs shader_file;
	shader_file.open(shader_file_name);
	shader_file.read(shader_file.get_size());

	if (is_binary) {
		shader_buffer_binary = (DWORD *)shader_file.get_data();
	}
	else {
		if (FAILED(D3DXAssembleShader((LPCTSTR)shader_file.get_data(), shader_file.get_size(), NULL, NULL, 0, &data_buffer, NULL))) {
			state.use_vertex_shader = false;
			ZZ_LOG("r_d3d: create_vertex_shader()::D3DXAssembleShader() failed\n");
			return -1;
		}
		shader_buffer_binary = (DWORD *)data_buffer->GetBufferPointer();
	}

	// vertex_shader_handle must be released in destroy_vertex_shader()
	if (FAILED(d3d_device->CreateVertexShader(shader_buffer_binary, &vertex_shaders[vertex_shader_index]))) {
		state.use_vertex_shader = false;
		ZZ_LOG("r_d3d: create_vertex_shader()::CreateVertexShader() failed\n");
		return -1;
	}

	//ZZ_LOG("r_d3d: create_vertex_shader(%d;%d) ok.\n", vertex_shader_index, vertex_shaders[vertex_shader_index]);

	SAFE_RELEASE(data_buffer);

	return vertex_shader_index;
}

int zz_renderer_d3d::create_pixel_shader (const char * shader_file_name, bool is_binary)
{
	if (!d3d_device) return 0;
	if (!shader_file_name) return 0;

	LPD3DXBUFFER data_buffer = NULL;
	DWORD * shader_buffer_binary;

	zz_vfs shader_file;
	shader_file.open(shader_file_name);
	shader_file.read(shader_file.get_size());
	
	if (is_binary) {
		shader_buffer_binary = (DWORD *)shader_file.get_data();
	}
	else {
		if (FAILED(D3DXAssembleShader((LPCSTR)shader_file.get_data(), shader_file.get_size(), NULL, NULL, 0, &data_buffer, NULL))) {
			state.use_pixel_shader = false;
			ZZ_LOG("r_d3d: D3DXAssembleShader() failed\n");
			return -1;
		}
		shader_buffer_binary = (DWORD *)data_buffer->GetBufferPointer();
	}
	
	// pixel_shader_handle must be released in destroy_pixel_shader()
	int pixel_shader_index = pixel_shaders.add(NULL);
	if (FAILED(d3d_device->CreatePixelShader(shader_buffer_binary, &pixel_shaders[pixel_shader_index]))) {
		state.use_pixel_shader = false;
		ZZ_LOG("r_d3d: create_pixel_shader()::CreatePixelShader() failed\n");
		return -1;
	}
	
	//ZZ_LOG("r_d3d: create_pixel_shader(%d) ok.\n", pixel_shader_index);
	SAFE_RELEASE(data_buffer);

	return pixel_shader_index;
}

void zz_renderer_d3d::destroy_vertex_shader (int vertex_shader_handle)
{
	if (d3d_device) {
		// make sure (vertex decls handle == vertex_shader_handel)
		int vertex_decl_handle = vertex_shader_handle;
		assert(ZZ_HANDLE_IS_VALID(vertex_decl_handle));
		//ZZ_LOG("r_d3d: destroy_vertex_shader(%d;%d) ok.\n", vertex_shader_handle, vertex_shaders[vertex_shader_handle]);
		LPDIRECT3DVERTEXDECLARATION9 p = vertex_decls[vertex_decl_handle];
		SAFE_RELEASE(vertex_decls[vertex_decl_handle]);
		SAFE_RELEASE(vertex_shaders[vertex_shader_handle]);

		vertex_decls.del(vertex_decl_handle);
		vertex_shaders.del(vertex_shader_handle);
	}
}

void zz_renderer_d3d::destroy_pixel_shader (int pixel_shader_handle)
{
	if (d3d_device) {
		assert(ZZ_HANDLE_IS_VALID(pixel_shader_handle));
		LPDIRECT3DPIXELSHADER9 pixel_shader = pixel_shaders[pixel_shader_handle];
		SAFE_RELEASE(pixel_shaders[pixel_shader_handle]);
		pixel_shaders.del(pixel_shader_handle);
		
		//ZZ_LOG("r_d3d: destroy_pixel_shader(%d) ok.\n", pixel_shader_handle);
	}
}

bool zz_renderer_d3d::support_vertex_shader ()
{
	if (D3DSHADER_VERSION_MAJOR(device_capability.VertexShaderVersion) < 1) {
		return false;
	}
	return true;
}


bool zz_renderer_d3d::support_pixel_shader ()
{
	if(device_capability.PixelShaderVersion < D3DPS_VERSION(1,0)) {
		return false;
	}
	return true;
}

zz_handle zz_renderer_d3d::create_vertex_buffer (const zz_device_resource& vres, int buffer_size)
{
	assert(d3d_device);
	assert(vres.get_ready());
	assert(!vres.get_created());
	assert(!vres.get_updated());

	int index_of_vertex_buffer_pool;

	DWORD usage;
	D3DPOOL pool;
	if (vres.get_dynamic() && vres.get_pool() == zz_device_resource::ZZ_POOL_DEFAULT) { // dynamic mesh
		usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		pool = D3DPOOL_DEFAULT; // dynamic mesh should be created with POOL_DEFAULT
	}
	else if (vres.get_pool() == zz_device_resource::ZZ_POOL_DEFAULT) { // for non-dynamic mesh but use default pool
		usage = D3DUSAGE_WRITEONLY;
		pool = D3DPOOL_DEFAULT;
	}
	else {
		// static mesh
		usage = D3DUSAGE_WRITEONLY;
		pool = D3DPOOL_MANAGED;
	}
	if (!state.use_hw_vertex_processing_support) { // if software vertex processing
		usage |= D3DUSAGE_SOFTWAREPROCESSING;
	}

	index_of_vertex_buffer_pool = vertex_buffer_pool.add(NULL);
	
	HRESULT hr;

	// vertex_buffer must be released in vertex_buffer_delete()
	if (FAILED(hr = d3d_device->CreateVertexBuffer(
		buffer_size, /* length */
		usage,  /* usage */
		0,                     /* FVF. if use shader, set to 0 */
		pool,       /* pool */
		&vertex_buffer_pool[index_of_vertex_buffer_pool], /* ppvertexbuffer */
		0                      /* pHandle : reserved */
		)))
	{
		ZZ_LOG("r_d3d: createvertexbuffer(%d, %d, %d) failed. [%s]\n",
			buffer_size, usage, pool, get_hresult_string(hr));
		zz_assert(0);
		return false;
	}

	return index_of_vertex_buffer_pool;
}

// pushes back a float value into the buffer, and increase the buffer pointer.
// This is used only in zz_renderer_d3d::update_vertex_buffer ().
char * push_back_float (char * buffer, float val)
{
	float * fp = (float *)(buffer);
    *fp = val;
    buffer += sizeof(float);
	return buffer;
}
		
bool zz_renderer_d3d::update_vertex_buffer (const zz_device_resource& vres, zz_handle vhandle, const void * buf, int size, int offset)
{
	static LPDIRECT3DVERTEXBUFFER9 last_buffer = NULL;

	assert(znzin);
	assert(vres.get_ready());
	assert(vres.get_created());
	assert(ZZ_HANDLE_IS_VALID(vhandle));
	// can be updated() state

//	// use caching
//#ifdef ZZ_USECACHE
//	zz_renderer_cached_info& cached = znzin->renderer->get_cached_info();
//	if (cached.vertex_buffer && (last_buffer == vertex_buffer_pool[vhandle])) {
//		return true; // use cached
//	}
//	last_buffer = vertex_buffer_pool[vhandle];
//	cached.vertex_buffer = true;
//#endif // ZZ_USE_CACHE

	char * vbuffer;	
	DWORD flags;

	if (vres.get_dynamic() && vres.get_pool() == zz_device_resource::ZZ_POOL_DEFAULT) {
		flags = D3DLOCK_DISCARD; //  | D3DLOCK_NOOVERWRITE;
	}
	else {
		// static mesh
		flags = 0;
	}

	//ZZ_PROFILER_BEGIN(Pupdate_vertex_buffer);

	// lock buffer
	if (FAILED(vertex_buffer_pool[vhandle]->Lock(
		offset, /* offset to lock */
		size, /* size to lock, in bytes. 0 means entire data to lock */
		(void**)&vbuffer, /* ppbdata */
		flags // D3DLOCK_DISCARD /* flags */
		)))
	{
		assert(!"renderer_d3d: vertex_buffer->Lock() failed\n");
		ZZ_LOG("r_d3d: vertex_buffer->Lock() failed\n");
		return false;
	}

	// copy contents
	memcpy(vbuffer, buf, size);
	
	// unlock
	if (FAILED(vertex_buffer_pool[vhandle]->Unlock())) {
		ZZ_LOG("r_d3d: vertex_buffer->Unlock() failed\n");
		return false;
	}

	//ZZ_PROFILER_END(Pupdate_vertex_buffer);

	return true;
}

zz_handle zz_renderer_d3d::create_index_buffer (const zz_device_resource& ires, int buffer_size)
{
	assert(d3d_device);
	assert(ires.get_ready());
	assert(!ires.get_created());
	assert(!ires.get_updated());
	assert(buffer_size > 0);

	int index_of_index_buffer_pool;

	DWORD usage;
	D3DPOOL pool = D3DPOOL_MANAGED;
	D3DFORMAT format = D3DFMT_INDEX16;
	
	if (ires.get_dynamic() && ires.get_pool() == zz_device_resource::ZZ_POOL_DEFAULT) { // dynamic mesh
		usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		pool = D3DPOOL_DEFAULT;
	}
	else { // static mesh
		usage = D3DUSAGE_WRITEONLY;
		pool = D3DPOOL_MANAGED;
	}

	// get new index buffer handle
	index_of_index_buffer_pool = index_buffer_pool.add(NULL);

	if (FAILED(d3d_device->CreateIndexBuffer(
		buffer_size,	/* length, in bytes */
		usage,	/* usage */
		format, /* format */
		pool,	/* pool */
		&index_buffer_pool[index_of_index_buffer_pool], /* ppindexbuffer */
		NULL /* handle : reserved */
		)))
	{
		ZZ_LOG("r_d3d: CreateIndexBuffer failed\n");
		return ZZ_HANDLE_NULL;
	}

	return index_of_index_buffer_pool;
}


bool zz_renderer_d3d::update_index_buffer (const zz_device_resource& ires, zz_handle ihandle, const void * buf, int size, int offset)
{
	static LPDIRECT3DINDEXBUFFER9 last_buffer = NULL;

	assert(buf);
	assert(ZZ_HANDLE_IS_VALID(ihandle));
	assert(d3d_device);
	assert(ires.get_ready());
	assert(ires.get_created());
	// can be updated() state

//	// use caching
//#ifdef ZZ_USECACHE
//	zz_renderer_cached_info& cached = znzin->renderer->get_cached_info();
//	if (cached.index_buffer && (last_buffer == index_buffer_pool[ihandle])) {
//		return true; // use cached
//	}
//	last_buffer= index_buffer_pool[ihandle];
//	cached.index_buffer = true;
//#endif // ZZ_USE_CACHE

	WORD * p_indices;

	DWORD flags;

	if (ires.get_dynamic() && ires.get_pool() == zz_device_resource::ZZ_POOL_DEFAULT) {
		flags = D3DLOCK_DISCARD | D3DLOCK_NOOVERWRITE;
	}
	else {
		// static mesh
		flags = 0;
	}

	if (FAILED(index_buffer_pool[ihandle]->Lock(
		offset,                        /* offset to lock */
		size, /* size to lock, in bytes */
		(void**)&p_indices,       /* ppbdata */
		flags                     /* flags */
		)))
	{
		ZZ_LOG("r_d3d: index_buffer->Lock() failed\n");
		return false;
	}

	zz_assert(p_indices);
	
	memcpy(p_indices, buf, size);
	
	if (FAILED(index_buffer_pool[ihandle]->Unlock())) {
		ZZ_LOG("r_d3d: vertex_buffer->Lock() failed\n");
		return false;
	}

	return true;
}


void zz_renderer_d3d::matrix_convert (D3DMATRIX& matrix_to_d3d, const mat4& matrix_from_ogl)
{
	// doubly transposed matrix = equal order

	memcpy(&matrix_to_d3d.m[0][0], &matrix_from_ogl.mat_array[0], 16*sizeof(float));
}

void zz_renderer_d3d::matrix_convert (mat4& matrix_to_ogl, const D3DMATRIX& matrix_from_d3d)
{
	// doubly transposed matrix = equal order

	memcpy(&matrix_to_ogl.mat_array[0], &matrix_from_d3d.m[0][0], 16*sizeof(float));
}

void zz_renderer_d3d::set_vertex_shader_constant (int register_index, const void * constant_data, int constant_count)
{
	zz_assert(state.use_vertex_shader);

	if (FAILED(d3d_device->SetVertexShaderConstantF(register_index, (const float *)constant_data, constant_count))) {
		ZZ_LOG("r_d3d: set_vertex_shader_constant()::SetVertexShaderConstant() failed\n");
	}
}

void zz_renderer_d3d::set_pixel_shader_constant (int register_index, const void * constant_data, int constant_count)
{
	zz_assert(state.use_pixel_shader);

	if (FAILED(d3d_device->SetPixelShaderConstantF(register_index, (const float *)constant_data, constant_count))) {
		ZZ_LOG("r_d3d: set_pixel_shader_constant()::SetPixelShaderConstant() failed\n");
	}
}

// [REF] http://www.flipcode.org/cgi-bin/fcarticles.cgi?show=4&id=64182
bool is_power_of_2 (UINT x)
{
    if (x<1) return false;
    return (x&(x-1))==0;
}

UINT make_power2_texturesize (UINT power_nonpower)
{
	if (is_power_of_2(power_nonpower))
		return power_nonpower;

	// non power 2
	return 1 << log2ge(power_nonpower);
}


// reload texture if the content was modified from beginning
zz_handle zz_renderer_d3d::download_texture (zz_texture * tex)
{
	LPDIRECT3DTEXTURE9 new_texture = NULL;

	//ZZ_PROFILER_INSTALL(Pdownload_texture);

	zz_assert(!_scene_began);
	zz_assert(tex);

	// get properties from tex pointer
	int width = tex->get_width();
	int height = tex->get_height();
	int miplevels = tex->get_miplevels();
	bool is_dynamic = tex->get_dynamic();
	zz_device_resource::zz_resource_pool pool = tex->get_pool();
	zz_handle texture_handle = tex->get_texture_handle();
	const char * texture_path = tex->get_path();
	bool use_filter = tex->get_filter();

	// re-scale texture size
	int div_scale(state.texture_loading_scale);
	UINT read_width(width), read_height(height);
	
	// read texture file to buffer
	uint32 size;
	void * data = NULL;

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	// vfs_thread: read by worker thread
	zz_vfs_thread::zz_item_it vfs_handle = tex->get_file_handle();

	if (!znzin->vfs_thread->is_valid_handle(vfs_handle)) { // invalid handle(= buffer freed)
		// reopen
		vfs_handle = znzin->vfs_thread->open(texture_path);
		assert(znzin->vfs_thread->is_valid_handle(vfs_handle));
	}
	data = znzin->vfs_thread->read(vfs_handle, &size, true /* force */);
#else
	zz_vfs fs;
	int fs_handle = fs.open(texture_path);
	size = fs.read();
	if (size == 0) {
		ZZ_LOG("r_d3d: download_texture(%s) failed.\n", texture_path);
		return ZZ_HANDLE_NULL;
	}
	data = fs.get_data();
#endif

	assert(data);

	if (size <= 0 || !data) {
		ZZ_LOG("r_d3d: download_texture(%s) failed. file not found\n", texture_path);
		return ZZ_HANDLE_NULL;
	}

	// get image info
	static D3DXIMAGE_INFO image_info;
	if (FAILED(D3DXGetImageInfoFromFileInMemory(data, size, &image_info))) {
		ZZ_LOG("r_d3d: getimginfofrommem(%s) failed.\n", texture_path);
		return ZZ_HANDLE_NULL;
	}

	if ((0 == width) || (0 == height)) { // if not default size
		tex->set_width(image_info.Width);
		tex->set_height(image_info.Height);
		// get/set width/heigth
		if (width == 0) {
			read_width = image_info.Width;
		}
		if (height == 0) {
			read_height = image_info.Height;
		}
	}

	// change 
	if (0 != div_scale) {
		if (!tex->get_for_image() && !tex->get_lock_texture()) // except for image textures
		{
			read_width = (image_info.Width >> div_scale);
			read_height = (image_info.Height >> div_scale);
			// not to be zero-size
			read_width = (0 == read_width) ? 1 : read_width;
			read_height = (0 == read_height) ? 1 : read_height;
		}
	}

	zz_assert(read_width != 0);
	zz_assert(read_height != 0);
	zz_assert(read_width != D3DX_DEFAULT);
	zz_assert(read_height != D3DX_DEFAULT);

	if (device_capability.TextureCaps & D3DPTEXTURECAPS_POW2) {
		// if not support non-power of 2 sized texture, then make power of 2.
		read_width = make_power2_texturesize(read_width);
		read_height = make_power2_texturesize(read_height);
	}

	// adjust width/height by h/w capability
	if (read_width > device_capability.MaxTextureWidth) {
		read_width = device_capability.MaxTextureWidth;
	}
	if (read_height > device_capability.MaxTextureHeight) {
		read_height = device_capability.MaxTextureHeight;
	}

	if (tex->get_for_image()) { // if this is image texture, we need no mipmap
		miplevels = 1;
	}
	else if ((miplevels < 0) && (state.mipmap_level < 0)) {
		miplevels = image_info.MipLevels;
	}
	else 	if (miplevels < 0) {
		miplevels = state.mipmap_level;
	}
	zz_assert(miplevels >= 0);

	// set texture loading format by force
	D3DFORMAT texture_loading_format;

	if (tex->get_lock_texture()) { // if lock texture, we maybe use d3dfmt_a4r4g4b4;
		texture_loading_format = static_cast<D3DFORMAT>(tex->get_format());
	}
	else {
		switch (state.texture_loading_format) {
		case 2: // compressed
			texture_loading_format = D3DFMT_DXT3;
			break;
		case 1: // 16-bit
			texture_loading_format = D3DFMT_A4R4G4B4;
			break;
		default:
			texture_loading_format = static_cast<D3DFORMAT>(tex->get_format());
			break;
		}
	}
	
	//ZZ_PROFILER_BEGIN(Pdownload_texture);
	//ZZ_LOG("\t\t\t\t\t- %s : miplevel(%d), format(%d)\n", texture_path, miplevels, texture_loading_format);

	/*
	512x512 texture
	DXT1 - miplevel1 = 0.729ms, 131KB
	DXT1 - miplevel2 = 57ms, 131KB
	TGA - 24bit = 59ms, 786KB
	TGA - 32bit = 28ms, 1,048KB
	*/

	HRESULT hr;

	if (FAILED(hr = D3DXCreateTextureFromFileInMemoryEx(
		d3d_device,			// LPDIRECT3DDEVICE9 pDevice,
		data,	// pSrcData
		(UINT)size, // SrcDataSize
		read_width, // UINT Width,
		read_height, // UINT Height,
		miplevels, // UINT MipLevels,
		is_dynamic ? D3DUSAGE_DYNAMIC : 0,					// DWORD Usage,(0, D3DUSAGE_RENDERTARGET, or D3DUSAGE_DYNAMIC)
		texture_loading_format, // texture format
		static_cast<D3DPOOL>(pool),
		use_filter ? D3DX_DEFAULT : D3DX_FILTER_NONE, // DWORD Filter, D3DX_DEFAULT = (D3DX_FILTER_TRIANGLE | D3DX_FILTER_DITHER)
		use_filter ? D3DX_DEFAULT : D3DX_FILTER_NONE, // DWORD MipFilterw D3DX_DEFAULT = (D3DX_FILTER_BOX)
		0,					// D3DCOLOR ColorKey,
		NULL,				// D3DXIMAGE_INFO* pSrcInfo
		NULL,				// PALETTEENTRY* pPalette,
		&new_texture)))
	{
		ZZ_LOG("r_d3d: d3dxcreatetexturefromfileinmemoryex(%s, %dx%d) failed.[%s]\n",
			texture_path, read_width, read_height, get_hresult_string(hr));

		zz_assert(0);

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
		znzin->vfs_thread->close(vfs_handle);
		tex->set_file_handle(zz_vfs_thread::zz_item_it()); // reset file handle
#endif
		return ZZ_HANDLE_NULL;
	}
	//ZZ_PROFILER_END(Pdownload_texture);

#ifdef USE_VFS_THREAD_TEXTURE_LOADING
	assert(znzin->vfs_thread->is_valid_handle(vfs_handle));
	znzin->vfs_thread->close(vfs_handle);
	tex->set_file_handle(zz_vfs_thread::zz_item_it()); // reset file handle
#endif
	
	if (ZZ_HANDLE_IS_VALID(texture_handle)) {
		assert(d3d_textures[texture_handle]); // zz_assert already has a texture
		// release old
		SAFE_RELEASE(d3d_textures[texture_handle]);
	}
	else {
		texture_handle = d3d_textures.add(NULL);
	}
	// set new
	d3d_textures[texture_handle] = new_texture;
	
	//ZZ_LOG("r_d3d: d3d_textures['%s'%d/%d] inserted.\n", texture_path, texture_handle, d3d_textures.size());

#ifdef LOG_TEXTURES
	if (std::find(s_textures.begin(), s_textures.end(), tex) == s_textures.end()) // not found
	{
		s_textures.push_back(tex);
	}
	else {
		assert(0);
	}
#endif

	return texture_handle;
}

// create null-texture
zz_handle zz_renderer_d3d::prepare_texture (zz_texture * tex)
{
	if (!tex) return ZZ_HANDLE_NULL;

	LPDIRECT3DTEXTURE9 new_texture;
	
	// get properties from tex pointer
	int width = tex->get_width();
	int height = tex->get_height();
	int miplevels = tex->get_miplevels();
	bool is_dynamic = tex->get_dynamic();
	zz_device_resource::zz_resource_pool pool = tex->get_pool();
	zz_handle texture_handle = tex->get_texture_handle();
	
	assert(is_dynamic);
	assert(pool == zz_device_resource::ZZ_POOL_DEFAULT);
	assert(miplevels == 1);

	if (device_capability.TextureCaps & D3DPTEXTURECAPS_POW2) {
		//not support non-power of 2 texture
		width = make_power2_texturesize(width);
		height = make_power2_texturesize(height);
	}
	
	assert(width > 0);
	assert(height > 0);
	
	HRESULT hr;

	// CAUTION: matrox g400 does not support dynamic texture
	if (FAILED(hr = d3d_device->CreateTexture(
		width,
		height,
		miplevels, //   UINT Levels,
		(is_dynamic) ? D3DUSAGE_DYNAMIC : 0, // DWORD Usage,
		D3DFMT_A8R8G8B8, // D3DFORMAT Format,
		static_cast<D3DPOOL>(pool), // D3DPOOL Pool,
		&new_texture, // IDirect3DTexture9** ppTexture,
		NULL // HANDLE* pSharedHandle
		)))
	{

		ZZ_LOG("r_d3d: createtexture(%s:%s, (%dx%d)->(%dx%d)) (%d, %d, %d) failed. [%s]\n",
			tex->get_name(), tex->get_path(),
			tex->get_width(), tex->get_height(), width, height,
			miplevels, is_dynamic, pool,
			get_hresult_string(hr));

		strcat(znzin->sysinfo.video_string," <Texture failed>");

		zz_msgbox(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::CREATE_TEXTURE_FAIL));
		zz_msgboxf(zz_error::get_text(zz_error::MSG_ERROR), zz_error::get_text(zz_error::MSG_EXIT_D3DERROR), znzin->sysinfo.video_string);
		exit(EXIT_FAILURE);
		return ZZ_HANDLE_NULL;
	}
	
	if (ZZ_HANDLE_IS_VALID(texture_handle)) {
		assert(d3d_textures[texture_handle]); // zz_assert already has a texture
		// release old
		SAFE_RELEASE(d3d_textures[texture_handle]);
	}
	else {
		texture_handle = d3d_textures.add(NULL);
	}
	// set new
	d3d_textures[texture_handle] = new_texture;

	//ZZ_LOG("r_d3d: d3d_textures2['%dx%d',%d/%d] inserted.\n", width, height, texture_handle, d3d_textures.size());

#ifdef LOG_TEXTURES
	if (std::find(s_textures.begin(), s_textures.end(), tex) == s_textures.end()) // not found
	{
		s_textures.push_back(tex);
	}
#endif

	return texture_handle;
}

zz_handle zz_renderer_d3d::create_texture (zz_texture * tex)
{
	return update_texture(tex);
}

zz_handle zz_renderer_d3d::update_texture (zz_texture * tex )
{
	if (!tex) return ZZ_HANDLE_NULL;

	if (tex->get_path()) {
		if (strlen(tex->get_path()) == 0) {
			ZZ_LOG("r_d3d: update_texture(%s) failed. invalide path(%s)\n", tex->get_name(), tex->get_path());
		}
		return download_texture(tex);
	}
	return prepare_texture(tex);
}

void zz_renderer_d3d::destroy_texture (zz_texture * tex)
{
	zz_assert(tex);

	zz_handle texture_handle = tex->get_texture_handle();
//	zz_assert(ZZ_HANDLE_IS_VALID(texture_handle));
	
	assert(d3d_textures.find(texture_handle));

	SAFE_RELEASE(d3d_textures[texture_handle]);
	d3d_textures.del(texture_handle);

#ifdef LOG_TEXTURES
	std::vector<zz_texture*>::iterator it = std::find(s_textures.begin(), s_textures.end(), tex);
	
	assert(it != s_textures.end());
	if (it == s_textures.end()) {
		ZZ_LOG("not created[%x:%d, %x]\n", tex->get_path(), tex->get_handle(), 
			d3d_textures[tex->get_handle()]);
	}
	else {
		s_textures.erase(it);
	}
#endif

	//ZZ_LOG("r_d3d: d3d_textures[%d/%d] removed.\n", texture_handle, d3d_textures.size());
}

int zz_renderer_d3d::get_num_textures ()
{
	return static_cast<int>(d3d_textures.get_num_running());
}

bool zz_renderer_d3d::set_normalization_cubemap_texture (int stage)
{
	if (!normalization_cubemap) return false;

	_cached.invalidate_texture(stage);
	if (state.use_texture) {
		if (FAILED(d3d_device->SetTexture(stage, normalization_cubemap))) {
			ZZ_LOG("r_d3d: set_texture()::SetTexture(normalization_cubemap) failed\n");
			return false;
		}
		return true;
	}
	return false;
}

bool zz_renderer_d3d::set_texture (zz_handle texture_handle, int stage, const char * texture_name)
{
	//ZZ_PROFILER_INSTALL(Pset_texture);
	assert(NUM_STAGES == 8);
	static zz_handle last_handle[NUM_STAGES] = {
			ZZ_HANDLE_NULL, ZZ_HANDLE_NULL, ZZ_HANDLE_NULL, ZZ_HANDLE_NULL, 
			ZZ_HANDLE_NULL, ZZ_HANDLE_NULL, ZZ_HANDLE_NULL, ZZ_HANDLE_NULL };

	assert(stage < NUM_STAGES);

	if (get_render_where() == ZZ_RW_SHADOWMAP) {
		texture_handle = ZZ_HANDLE_NULL; // ignore texture binding
	}
		
	if (_cached.texture[stage]) {
		if (last_handle[stage] == texture_handle) {
			//ZZ_LOG("r_d3d::set_texture() ignored by cache. [%d:%d]\n", texture_handle, stage);
			return true;
		}
	}
	
#ifdef ZZ_USECACHE
	_cached.texture[stage] = true;
#endif

	last_handle[stage] = texture_handle;
	
	unsigned int texture_size = (unsigned long)d3d_textures.get_num_total();

	if (texture_size == 0) return false; // no texture exists

	LPDIRECT3DTEXTURE9 t;
	if (!ZZ_HANDLE_IS_VALID(texture_handle)) {
		//texture_handle %= texture_size;
		t = NULL;
		//if (get_render_where() != ZZ_RW_SHADOWMAP) {
		//	ZZ_LOG("r_d3d: set_texture(%d) failed. not a valid handle\n", texture_handle);
		//}
	}
	else {
		t = d3d_textures[texture_handle];
	}

	bool ret = false;
	//ZZ_PROFILER_BEGIN(Pset_texture);
	if (state.use_texture) {
		if (FAILED(d3d_device->SetTexture(stage, t))) {
			ZZ_LOG("r_d3d: set_texture()::SetTexture() failed\n");
		}
		else ret = true;
	}
	//ZZ_LOG("r_d3d::set_texture(%d:%d, %s)\n", texture_handle, stage, texture_name);
	
	//ZZ_PROFILER_END(Pset_texture);

	return ret;
}

void zz_renderer_d3d::set_vertex_shader_constant_matrix (int index, const mat4& matrix_to_set_modelview, int count)
{
	if (!state.use_vertex_shader)
		return;
	static mat4 transposed_matrix;

	// transpose matrices to transper matrix to shader program.
	// why transpose? set_vertex_shader_constant() uses transposed matrix
	transpose(transposed_matrix, matrix_to_set_modelview);
	set_vertex_shader_constant(index, transposed_matrix.mat_array, count);
}

const mat4& zz_renderer_d3d::get_modelview_matrix ()
{
	//if (state.use_vertex_shader == false) {
	//	D3DMATRIX matd3d;
	//	d3d_device->GetTransform(D3DTS_VIEW, &matd3d);
	//	matrix_convert(modelview_matrix, matd3d);
	//}
	return modelview_matrix;
}

const mat4& zz_renderer_d3d::get_projection_matrix ()
{
	//if (state.use_vertex_shader == false) {
	//	D3DMATRIX matd3d;
	//	d3d_device->GetTransform(D3DTS_PROJECTION, &matd3d);
	//	matrix_convert(projection_matrix, matd3d);
	//}
	return projection_matrix;
}

const mat4& zz_renderer_d3d::get_world_matrix ()
{
	return world_matrix;
}

void zz_renderer_d3d::set_world_matrix (const mat4& matrix_to_set)
{
	world_matrix = matrix_to_set;
	if (state.use_vertex_shader) {
		set_vertex_shader_constant_matrix(ZZ_VSC_WORLD_TM, matrix_to_set, 4);
	}
}

void zz_renderer_d3d::set_modelview_matrix (const mat4& matrix_to_set)
{
	modelview_matrix = matrix_to_set;
	
	if (state.use_vertex_shader) {
		set_vertex_shader_constant_matrix(ZZ_VSC_WORLDVIEW_TM, matrix_to_set, 4);
	}
	d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&matrix_to_set);
}

void zz_renderer_d3d::set_projection_matrix (const mat4& matrix_to_set)
{
	projection_matrix = matrix_to_set;

	if (state.use_vertex_shader) {
		set_vertex_shader_constant_matrix(ZZ_VSC_PROJECTION_TM, matrix_to_set);
	}
	d3d_device->SetTransform(D3DTS_PROJECTION, (const D3DXMATRIX *)&matrix_to_set);
}

bool zz_renderer_d3d::enable_zbuffer (bool true_or_false)
{
	static bool old_value = true;
	static bool ret_value = true;
	static bool first = true;
	
	// changed
	d3d_device->SetRenderState(D3DRS_ZENABLE, (true_or_false) ? D3DZB_TRUE : D3DZB_FALSE);
	ret_value = old_value;
	old_value = true_or_false; // update old
	return ret_value;
}

void zz_renderer_d3d::set_alpha_ref (int alpha_ref_in)
{
	d3d_device->SetRenderState(D3DRS_ALPHAREF, (DWORD)(alpha_ref_in));
}

// return last value
bool zz_renderer_d3d::enable_alpha_test (bool true_or_false, ulong alpha_ref, ZZ_CMPFUNC cmp_func)
{
	static bool first = true;
	static bool old_value = true;
	static bool ret_value = true;

	if (!true_or_false) {
		d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
	else {
		d3d_device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	}
	d3d_device->SetRenderState(D3DRS_ALPHAREF, alpha_ref);
	d3d_device->SetRenderState(D3DRS_ALPHAFUNC, cmp_func);

	ret_value = old_value;
	old_value = true_or_false;
	return ret_value;
}

bool zz_renderer_d3d::enable_alpha_blend (bool true_or_false, int blend_type)
{
	static bool old_value = false; // default : alpha blend not enabled
	static bool ret_value; // return value
	
	state.use_alpha = true_or_false;

	//ZZ_LOG("r_d3d: enable_alpha_blend(%d)\n", true_or_false);
	//if (old_value != true_or_false) { // changed, then update render state
	d3d_device->SetRenderState(D3DRS_ALPHABLENDENABLE, (true_or_false) ? TRUE : FALSE);
	
	ret_value = old_value;
	old_value = true_or_false; // update old

	if (true_or_false == false)	{
		return ret_value;
	}

	//d3d_device->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);

	// blend_type 0 : alpha blend, 1 : dodge blend
	if (blend_type == ZZ_BT_NONE) return ret_value; // skip blend setting
	if (blend_type == ZZ_BT_CUSTOM) return ret_value; // already set by zz_material::set()
	else if (blend_type == ZZ_BT_NORMAL) // normal alpha blending
	{
		set_blend_type(ZZ_BLEND_SRCALPHA, ZZ_BLEND_INVSRCALPHA, ZZ_BLENDOP_ADD);
	}
	else if (blend_type == ZZ_BT_LIGHTEN) // color dodge blend
	{
		// srcColor*1 + destColor*1 = finalColor
		set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD);
		//set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_ONE);
	}
	//else if (blend_type == 2) // shadow and lightmap
	//{
	//	// srcColor*DESTCOLOR + destColor*DESTCOLOR = finalColor
	//	set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_DESTCOLOR);
	//}
	//else if (blend_type == 3) // only shadow
	//{
	//	// srcColor*DESTCOLOR + destColor*INVDESTCOLOR = finalColor
	//	set_blend_type(ZZ_BLEND_DESTCOLOR, ZZ_BLEND_INVDESTCOLOR);
	//}
	return ret_value;
}

void zz_renderer_d3d::matrix_push (const mat4& in_push)
{
	matrix_stack.push_back(in_push);
}

mat4 zz_renderer_d3d::matrix_pop (void)
{
	mat4 out_mat = matrix_stack[matrix_stack.size()-1];
	matrix_stack.pop_back();
	return out_mat;
}

bool zz_renderer_d3d::enable_zwrite (bool true_or_false)
{
	static bool old_value = true;
	static bool ret_value = old_value;
	static bool first = true;

	// changed	
	d3d_device->SetRenderState(D3DRS_ZWRITEENABLE, (true_or_false) ? TRUE : FALSE);	
	ret_value = old_value;
	old_value = true_or_false; // update old
	return ret_value;
}

void zz_renderer_d3d::enable_fog (bool true_or_false)
{
	// adapt use_fog first
	if (state.use_fog == false) true_or_false = false;
	
	if (FAILED(d3d_device->SetRenderState(D3DRS_FOGENABLE, (true_or_false) ? TRUE : FALSE))) {
		ZZ_LOG("r_d3d: enable_fog() failed\n");
	}
}

void zz_renderer_d3d::draw_line (vec3 from, vec3 to, vec3 rgb)
{
	struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};

	//ZZ_PROFILER_BEGIN_CYCLE(Pdraw_line, 100);
	
	static MYLINEVERTEX line_buffer[2];
	const float min_distance_ignore = 10.0f*ZZ_SCALE_IN;

	// if less than minimum distance ignore this
//	if (from.distance_square(to) < min_distance_ignore) return;   //조성현 무시함...
	
	// this seems to be a ATI driver bug
	// if not, in some case line direction can be weird.
	// if from or to value is too big, line position can be ignored to zero.
	// thus, we converted to ->
	// multiply modelview matrix with from and to vector, and just transfer identity
	// matrix to modelview matrix. do not use modelview matrix explicitly
	zz_camera * camera = znzin->get_camera();
	if (!camera) {
		ZZ_LOG("r_d3d: draw_line() failed. no camera\n");
		return;
	}

	// get matrices
	camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, modelview_matrix);
	camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_matrix);
	
	vec3 from_world, to_world;
	mult(from_world, modelview_matrix, from);
	mult(to_world, modelview_matrix, to);

	line_buffer[0].pos = D3DXVECTOR3(from_world.x, from_world.y, from_world.z);
	line_buffer[0].diffuse = ZZ_TO_D3DXRGB(rgb.x, rgb.y, rgb.z);
	line_buffer[1].pos = D3DXVECTOR3(to_world.x, to_world.y, to_world.z);
	line_buffer[1].diffuse = ZZ_TO_D3DXRGB(rgb.x, rgb.y, rgb.z);

	set_projection_matrix(projection_matrix);
	set_modelview_matrix(mat4_id);

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);
	
	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);

	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	try {
		if (FAILED(d3d_device->DrawPrimitiveUP(D3DPT_LINELIST, 2, line_buffer, sizeof(MYLINEVERTEX)))) {
			ZZ_LOG("r_d3d: draw_line()->drawprimitiveup() failed\n");
			return;
		}
	}
	catch (...) {
		// do nothing. maybe device lost state
		ZZ_LOG("r_d3d: draw_line(). dp exception\n");
	}
	
	//ZZ_PROFILER_END(Pdraw_line);
}


void zz_renderer_d3d::draw_arrow(float size, int color)
{

	struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};

	int i;
	float arrow_size;
	MYLINEVERTEX gVertPool[4];
	D3DXMATRIX model_m,t_m,mem_m;
	DWORD lighting_onoff;
	
	arrow_size=size*0.05f;
	gVertPool[0].pos.x=0.0f;gVertPool[0].pos.y=0.0f;gVertPool[0].pos.z=2.5f*arrow_size;gVertPool[0].diffuse=color;

	for(i=0;i<3;i+=1){
	gVertPool[i+1].pos.x=arrow_size*cosf((-30.0f+120.0f*i)*3.141592f/180.0f);gVertPool[i+1].pos.y=arrow_size*sinf((-30.0f+120.0f*i)*3.141592f/180.0f);gVertPool[i+1].pos.z=0.0f;gVertPool[i+1].diffuse=color;
	}

	WORD indexList[12]  =  { 0, 1, 2,
							0, 2, 3,
							0, 3, 1,
							1, 3, 2};


	d3d_device->GetTransform(D3DTS_WORLD,&mem_m);
	D3DXMatrixTranslation (&t_m,0.0f,0.0f,size);
	d3d_device->GetRenderState(D3DRS_LIGHTING, &lighting_onoff);
	d3d_device->SetRenderState(D3DRS_LIGHTING, false);
	model_m=t_m*mem_m;

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);
	enable_alpha_test(false);
	enable_fog(false);
	
	d3d_device->SetTexture(0, NULL);
	d3d_device->SetTexture(1, NULL);
	d3d_device->SetTexture(2, NULL);
	d3d_device->SetTexture(3, NULL);
	
	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
		
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	d3d_device->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST,0,4,4,indexList,D3DFMT_INDEX16,gVertPool,sizeof(MYLINEVERTEX)); 

	gVertPool[0].pos.x=gVertPool[0].pos.y=gVertPool[0].pos.z=0.0f;
	gVertPool[1].pos.x=gVertPool[1].pos.y=0.0f;
	gVertPool[1].pos.z=size;

	d3d_device->SetTransform(D3DTS_WORLD,&mem_m);
	d3d_device->DrawPrimitiveUP(D3DPT_LINELIST,1,gVertPool,sizeof(MYLINEVERTEX));
	d3d_device->SetRenderState(D3DRS_LIGHTING, lighting_onoff);
}


void zz_renderer_d3d::draw_axis(float size)
{

	D3DXMATRIX model_m,r_m,mem_m;
	D3DXVECTOR3 direction;
	
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m);

	direction.x=0.0f;direction.y=1.0f;direction.z=0.0f;
	D3DXMatrixRotationAxis(&r_m,&direction,3.141592f/2.0f); 
	model_m=r_m*mem_m;
	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	draw_arrow(size,D3DCOLOR_COLORVALUE(1.0f,0.0f,0.0f,1.0f));

	direction.x=-1.0f;direction.y=0.0f;direction.z=0.0f;
	D3DXMatrixRotationAxis(&r_m,&direction,3.141592f/2.0f); 
	model_m=r_m*mem_m;
	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	draw_arrow(size,D3DCOLOR_COLORVALUE(0.0f,1.0f,0.0f,1.0f));

	d3d_device->SetTransform(D3DTS_WORLD,&mem_m);
	draw_arrow(size,D3DCOLOR_COLORVALUE(0.0f,0.0f,1.0f,1.0f));
}

void zz_renderer_d3d::draw_axis(float *q, float *v, float size)
{

	D3DXMATRIX mem_m1, mem_m2;
	D3DXMATRIX buffer_m, s_m, object_matrix;
	D3DXQUATERNION quaternion;
	D3DXVECTOR3 vector;
	DWORD lighting_onoff;

	mat4 camera_m;
	D3DXMATRIX projection_m;
	zz_camera * cam = znzin->get_camera();

	quaternion.x = q[0]; quaternion.y = q[1]; quaternion.z = q[2]; quaternion.w = q[3];
    vector.x = v[0]; vector.y = v[1]; vector.z = v[2];
 
	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
	
    d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->GetRenderState(D3DRS_LIGHTING, &lighting_onoff);

	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetTexture(0, NULL);
    d3d_device->SetTexture(1, NULL);
  
	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
	d3d_device->SetRenderState(D3DRS_LIGHTING, false);
	
	
	D3DXMatrixRotationQuaternion(&buffer_m, &quaternion);
	buffer_m._41 = vector.x; buffer_m._42 = vector.y; buffer_m._43 = vector.z;
    D3DXMatrixScaling(&s_m, size, size, size);
	object_matrix = s_m * buffer_m; 
	
	d3d_device->SetTransform(D3DTS_WORLD,&object_matrix);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	draw_axis(size);  

	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetRenderState(D3DRS_LIGHTING, lighting_onoff);

}

void zz_renderer_d3d::draw_camera()
{
 
	D3DXMATRIX projection_m;
	
	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	draw_camera_frustum();
	draw_axis(2.0f);
	d3d_device->SetTransform(D3DTS_PROJECTION,&projection_m);

}

void zz_renderer_d3d::draw_camera_ex(const mat4& view_mat)
{
 
	D3DXMATRIX projection_m;
	D3DXMATRIX mem_m; 
	
	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m);
		
	set_projection_matrix(projection_matrix);
	d3d_device->SetTransform(D3DTS_WORLD, (const D3DXMATRIX *)&view_mat);
	
	draw_camera_frustum();
	draw_axis(2.0f);
	
	d3d_device->SetTransform(D3DTS_PROJECTION,&projection_m);
	d3d_device->SetTransform(D3DTS_WORLD,&mem_m);

}



void zz_renderer_d3d::draw_camera(mat4& camera_matrix)
{
	
	zz_camera *cam = znzin->get_camera();
	zz_camera *lcam = znzin->get_camera_light();
	D3DXMATRIX mem_m1,mem_m2;
	mat4 model_m,camera_m;
	mat4 lcam_m;
	D3DXMATRIX projection_m;

	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	
	model_m=camera_matrix.inverse();
    cam->get_modelviewTM(camera_m);

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);

		
	
	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);
	d3d_device->SetTransform(D3DTS_WORLD,(const D3DXMATRIX *)&model_m);
    
	draw_axis(10.0f);
	
	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1); 
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2); 
    d3d_device->SetTransform(D3DTS_PROJECTION,&projection_m);

}

void zz_renderer_d3d::draw_camera_frustum()
{
    struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};
   
    D3DXMATRIX mem;

	float near_region[2],far_region[2],near_plane,far_plane;
	zz_camera * cam = znzin->get_camera();
	
	near_plane=-1.0f*cam->get_near_plane();
	far_plane=/*-1.0f*cam->get_far_plane()*/-1300.0f;
	znzin->camera_sfx.get_viewing_region(near_plane,near_region);
	znzin->camera_sfx.get_viewing_region(far_plane,far_region);

	MYLINEVERTEX gVertPool[8];

	gVertPool[0].pos.x=-near_region[0];gVertPool[0].pos.y=near_region[1];gVertPool[0].pos.z=near_plane;gVertPool[0].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[1].pos.x=-near_region[0];gVertPool[1].pos.y=-near_region[1];gVertPool[1].pos.z=near_plane;gVertPool[1].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[2].pos.x=near_region[0];gVertPool[2].pos.y=-near_region[1];gVertPool[2].pos.z=near_plane;gVertPool[2].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[3].pos.x=near_region[0];gVertPool[3].pos.y=near_region[1];gVertPool[3].pos.z=near_plane;gVertPool[3].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[4].pos.x=-far_region[0];gVertPool[4].pos.y=far_region[1];gVertPool[4].pos.z=far_plane;gVertPool[4].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[5].pos.x=-far_region[0];gVertPool[5].pos.y=-far_region[1];gVertPool[5].pos.z=far_plane;gVertPool[5].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[6].pos.x=far_region[0];gVertPool[6].pos.y=-far_region[1];gVertPool[6].pos.z=far_plane;gVertPool[6].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);
	gVertPool[7].pos.x=far_region[0];gVertPool[7].pos.y=far_region[1];gVertPool[7].pos.z=far_plane;gVertPool[7].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,0.0f,1.0f);

	WORD indexList[24]  =  { 0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6, 6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
	d3d_device->GetTransform(D3DTS_WORLD,&mem);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	d3d_device->DrawIndexedPrimitiveUP(D3DPT_LINELIST,0,8,12,indexList,D3DFMT_INDEX16,gVertPool,sizeof(MYLINEVERTEX)); 

}

void zz_renderer_d3d::draw_shadowmap()
{
   
	zz_camera * lcam = znzin->get_camera_light();
	zz_camera * cam = znzin->get_camera();
	D3DXMATRIX mem_m1,mem_m2;
	mat4 model_m,camera_m;
	D3DXMATRIX projection_m;

	
	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
		
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);

	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);
	
	model_m.set_identity();
	d3d_device->SetTransform(D3DTS_WORLD,(const D3DXMATRIX *)&model_m);
    
	
	if (!cam) return;
	lcam->update_frustum(0);  //test
	
	const zz_viewfrustum& frustum = lcam->get_frustum();

	VERTEX_TRAIL point[6]; 
	
	point[0].position.x=frustum.p[0].x;point[0].position.y=frustum.p[0].y;point[0].position.z=frustum.p[0].z;point[0].uv.x=1.0f;point[0].uv.y=1.0f;
    point[1].position.x=frustum.p[3].x;point[1].position.y=frustum.p[3].y;point[1].position.z=frustum.p[3].z;point[1].uv.x=0.0f;point[1].uv.y=0.0f;
    point[2].position.x=frustum.p[1].x;point[2].position.y=frustum.p[1].y;point[2].position.z=frustum.p[1].z;point[2].uv.x=0.0f;point[2].uv.y=1.0f;
    point[3].position.x=frustum.p[0].x;point[3].position.y=frustum.p[0].y;point[3].position.z=frustum.p[0].z;point[3].uv.x=1.0f;point[3].uv.y=1.0f;
    point[4].position.x=frustum.p[2].x;point[4].position.y=frustum.p[2].y;point[4].position.z=frustum.p[2].z;point[4].uv.x=1.0f;point[4].uv.y=0.0f;
    point[5].position.x=frustum.p[3].x;point[5].position.y=frustum.p[3].y;point[5].position.z=frustum.p[3].z;point[5].uv.x=0.0f;point[5].uv.y=0.0f;

    enable_fog(false);
	enable_zbuffer(true);
	enable_zwrite(true);
	set_zfunc(ZZ_CMP_LESSEQUAL);

	d3d_device->SetTexture( 0, shadowmap );
	
	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLOROP,   ZZ_TOP_SELECTARG1 );
	set_texture_stage_state( 1, ZZ_TSS_COLOROP,   ZZ_TOP_DISABLE );

	set_texture_stage_state( 0, ZZ_TSS_ALPHAOP,   ZZ_TOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NONE);

   	d3d_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
	d3d_device->DrawPrimitiveUP(D3DPT_TRIANGLELIST,2,point,sizeof(VERTEX_TRAIL));
 
   	d3d_device->SetTexture( 0, NULL ); 

    d3d_device->SetTransform(D3DTS_WORLD,&mem_m1); 
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2); 
    d3d_device->SetTransform(D3DTS_PROJECTION,&projection_m);

}


// from nvSDK's "shader_BRDF.cpp"
bool zz_renderer_d3d::create_normalization_cubemap (int width, int mipmap_level)
{
	HRESULT hr;

	hr = D3DXCreateCubeTexture(d3d_device, width, mipmap_level, 0, D3DFMT_X8R8G8B8, 
		D3DPOOL_MANAGED, &normalization_cubemap);

	if(FAILED(hr))
	{
		return false;
	}

	for (int i = 0; i < 6; i++)
	{
		D3DLOCKED_RECT Locked;
		D3DXVECTOR3 Normal;
		float w,h;
		D3DSURFACE_DESC ddsdDesc;
		
		normalization_cubemap->GetLevelDesc(0, &ddsdDesc);

		normalization_cubemap->LockRect((D3DCUBEMAP_FACES)i, 0, &Locked, NULL, 0);

		for (unsigned int y = 0; y < ddsdDesc.Height; y++)
		{
			h = (float)y / ((float)(ddsdDesc.Height - 1));
			h *= 2.0f;
			h -= 1.0f;

			for (unsigned int x = 0; x < ddsdDesc.Width; x++)
			{
				w = (float)x / ((float)(ddsdDesc.Width - 1));
				w *= 2.0f;
				w -= 1.0f;

				DWORD* pBits = (DWORD*)((BYTE*)Locked.pBits + (y * Locked.Pitch));
				pBits += x;

				switch((D3DCUBEMAP_FACES)i)
				{
					case D3DCUBEMAP_FACE_POSITIVE_X:
						Normal = D3DXVECTOR3(1.0f, -h, -w);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_X:
						Normal = D3DXVECTOR3(-1.0f, -h, w);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Y:
						Normal = D3DXVECTOR3(w, 1.0f, h);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Y:
						Normal = D3DXVECTOR3(w, -1.0f, -h);
						break;
					case D3DCUBEMAP_FACE_POSITIVE_Z:
						Normal = D3DXVECTOR3(w, -h, 1.0f);
						break;
					case D3DCUBEMAP_FACE_NEGATIVE_Z:
						Normal = D3DXVECTOR3(-w, -h, -1.0f);
						break;
					default:
						assert(0);
						break;
				}

				D3DXVec3Normalize(&Normal, &Normal);

				// Scale to be a color from 0 to 255 (127 is 0)
				Normal += D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				Normal *= 127.0f;

				// Store the color
				*pBits = (DWORD)(((DWORD)Normal.x << 16) | ((DWORD)Normal.y << 8) | ((DWORD)Normal.z << 0));

			}
		}
		normalization_cubemap->UnlockRect((D3DCUBEMAP_FACES)i, 0);
	}

	if ((mipmap_level == 0) || (mipmap_level > 1))
		D3DXFilterCubeTexture(normalization_cubemap, NULL, 0, D3DX_FILTER_LINEAR);

	return true;
}

void zz_renderer_d3d::destroy_vertex_buffer (zz_handle handle_in)
{
	assert(d3d_device);
	assert(ZZ_HANDLE_IS_VALID(handle_in));

	if (vertex_buffer_pool.find(handle_in)) {
		assert(vertex_buffer_pool[handle_in]);
		vertex_buffer_pool[handle_in]->Release();
		vertex_buffer_pool.del(handle_in);
	}
	else {
		assert(0);
	}
	//ZZ_LOG("r_d3d: destroy_vertex_buffer(%d) size(%d)\n", handle_in, vertex_buffer_pool.size());
}

void zz_renderer_d3d::destroy_index_buffer (zz_handle handle_in)
{
	assert(d3d_device);
	assert(ZZ_HANDLE_IS_VALID(handle_in));

	if (index_buffer_pool.find(handle_in)) {
		assert(index_buffer_pool[handle_in]);
		index_buffer_pool[handle_in]->Release();
        index_buffer_pool.del(handle_in);
	}
	else {
		assert(0);
	}
	//ZZ_LOG("r_d3d: destroy_index_buffer(%d) size(%d)\n", handle_in, index_buffer_pool.size());
}

bool zz_renderer_d3d::resize (void)
{
	_parameters.BackBufferHeight = state.screen_height;
	_parameters.BackBufferWidth = state.screen_width;
	if (!d3d_device) return false;
	if (FAILED(d3d_device->Reset(&_parameters))) {
		ZZ_LOG("r_d3d: resize(%d, %d) failed\n",
			_parameters.BackBufferWidth, _parameters.BackBufferHeight);
		return false;
	}
	return true;
}

void zz_renderer_d3d::set_depthbias (int bias)
{
	d3d_device->SetRenderState(D3DRS_DEPTHBIAS, bias);
}

bool zz_renderer_d3d::set_gamma_by_value (float gamma_value)
{
	current_gamma.set_gamma_value(gamma_value);
	return set_gamma(current_gamma);
}

bool zz_renderer_d3d::set_gamma (const zz_gamma& gamma)
{
	if (!d3d_device) return false;
	if (!state.use_gamma) return false;

	D3DGAMMARAMP d3d_ramp;

	for (int i = 0; i < 256; ++i) {
		d3d_ramp.red[i]   = gamma.red[i];
		d3d_ramp.green[i] = gamma.green[i];
		d3d_ramp.blue[i]  = gamma.blue[i];

		//if ((i == 0) || (i == 128) || (i == 255))
		//ZZ_LOG("r_d3d: set_gamma() sample [%d] = (%d, %d, %d)\n",
		//	i, d3d_ramp.red[i], d3d_ramp.green[i], d3d_ramp.blue[i]);
	}

	if (state.use_game_gamma && znzin->get_rs()->use_fullscreen) {
		if (device_capability.Caps2 & D3DCAPS2_FULLSCREENGAMMA) {
			UINT iSwapChain = 0;
			d3d_device->SetGammaRamp(iSwapChain, D3DSGR_NO_CALIBRATION, &d3d_ramp);
			ZZ_LOG("r_d3d: set game gamma.\n");
			return true;
		}
		else {
			ZZ_LOG("r_d3d: SetGammaRamp() failed.\n");
		}
	}
	else {
		if (::SetDeviceGammaRamp(GetDC(reinterpret_cast<HWND>(znzin->view->get_handle())), &d3d_ramp)) {
			ZZ_LOG("r_d3d: set device gamma.\n");
			return true;
		}
		else {
			ZZ_LOG("r_d3d: SetDeviceGammaRamp() failed.\n");
		}
	}
	state.use_gamma = false;

	return false;
}

bool zz_renderer_d3d::get_gamma (zz_gamma& gamma)
{
	if (!d3d_device) return false;
	if (!state.use_gamma) return false;

	D3DGAMMARAMP d3d_ramp;

	bool ok = false;
	
	if (state.use_game_gamma && this->state.use_fullscreen) {
		if (device_capability.Caps2 & D3DCAPS2_FULLSCREENGAMMA) {
			UINT iSwapChain = 0;
			d3d_device->GetGammaRamp(iSwapChain, &d3d_ramp);
			ZZ_LOG("r_d3d: get game gamma.\n");
			ok = true;
		}
		else {
			ZZ_LOG("r_d3d: GetGammaRamp() failed.\n");
		}
	}
	else {
		if (::GetDeviceGammaRamp(GetDC(reinterpret_cast<HWND>(znzin->view->get_handle())), &d3d_ramp)) {
			ZZ_LOG("r_d3d: get device gamma.\n");
			ok = true;
		}
		else {
			ZZ_LOG("r_d3d: GetDeviceGammaRamp() failed.\n");
		}
	}
	
	if (!ok) {
		state.use_gamma = false;
		return false;
	}

	for (int i = 0; i < 256; ++i) {
		gamma.red[i]   = d3d_ramp.red[i];
		gamma.green[i] = d3d_ramp.green[i];
		gamma.blue[i]  = d3d_ramp.blue[i];

		//if ((i == 0) || (i == 128) || (i == 255))
		//ZZ_LOG("r_d3d: get_gamma() sample [%d] = (%d, %d, %d)\n",
		//	i, d3d_ramp.red[i], d3d_ramp.green[i], d3d_ramp.blue[i]);
	}

	return true;
}

void zz_renderer_d3d::render_shadowmap_overlay()
{
	_begin_scene("render_shadowmap_overlay");
	draw_texture(0, float(state.shadowmap_size), 0, float(state.shadowmap_size), this->shadowmap_overlay_texture, ZZ_BT_NORMAL);
	_end_scene("render_shadowmap_overlay");
}

bool zz_renderer_d3d::set_zfunc (ZZ_CMPFUNC val)
{
	if (FAILED(d3d_device->SetRenderState(D3DRS_ZFUNC, static_cast<ZZ_CMPFUNC>(val)))) return false;
	return true;
}

bool zz_renderer_d3d::set_alphafunc (ZZ_CMPFUNC val)
{
	if (FAILED(d3d_device->SetRenderState(D3DRS_ALPHAFUNC, static_cast<ZZ_CMPFUNC>(val)))) return false;
	return true;
}

bool zz_renderer_d3d::set_render_state (ZZ_RENDERSTATETYPE state, ulong value)
{
	if (FAILED(d3d_device->SetRenderState(static_cast<D3DRENDERSTATETYPE>(state), (DWORD)value))) return false;
	return true;
}

bool zz_renderer_d3d::save_to_file (const char * filename_dds, int format_dds0_bmp1_jpg2)
{
	D3DXIMAGE_FILEFORMAT image_fileformat;

	//ZZ_PROFILER_BEGIN(Psavetofile);

	if (format_dds0_bmp1_jpg2 == 0) { // dds
		image_fileformat = D3DXIFF_DDS;
	}
	else if (format_dds0_bmp1_jpg2 == 1) {
		image_fileformat = D3DXIFF_BMP;
	}
	else if (format_dds0_bmp1_jpg2 == 2) {
		image_fileformat = D3DXIFF_JPG;
	}

	if (FAILED(D3DXSaveSurfaceToFile(filename_dds,
		image_fileformat,
		backbuffer_surface,
		NULL,
		NULL)))
	{
		return false;
	}
	
	//ZZ_PROFILER_END(Psavetofile);

	return true;
}

// get d3d Flexible Vertex Format
DWORD zz_renderer_d3d::get_fvf (const zz_vertex_format& format)
{
	DWORD fvf = 0;
	// not supported formats
	if (format.use_skin()) return 0;
	if (format.use_tangent()) return 0;
	if (format.get_num_mapchannel() > 4) return 0;

	// set fvf
	fvf = D3DFVF_XYZ;
	fvf |= format.use_color() ? D3DFVF_DIFFUSE : 0;
	fvf |= format.use_normal() ? D3DFVF_NORMAL : 0;
	switch (format.get_num_mapchannel()) {
		case 1:
			fvf |= D3DFVF_TEX1;
			break;
		case 2:
			fvf |= D3DFVF_TEX2;
			break;
		case 3:
			fvf |= D3DFVF_TEX3;
			break;
		case 4:
			fvf |= D3DFVF_TEX4;
			break;
	}
	return fvf;
}

void zz_renderer_d3d::set_texture_matrix (int texture_stage, int shader_constant_index, const mat4& matrix_to_set)
{
	if (state.use_vertex_shader) {
		set_vertex_shader_constant_matrix(shader_constant_index, matrix_to_set, 4);
	}
	else { // fixed pipeline version
		if (0) {
			D3DMATRIX matd3d;
			matrix_convert(matd3d, matrix_to_set);
			set_texture_stage_state(texture_stage,
				ZZ_TSS_TEXTURETRANSFORMFLAGS, ZZ_TTFF_COUNT2 | ZZ_TTFF_PROJECTED);

			// CAUTION: assumes that D3DTS_TEXTURE0-7 is continuous
			//d3d_device->SetTransform(D3DTRANSFORMSTATETYPE(D3DTS_TEXTURE0 + texture_stage), &matd3d);
		}
	}
}

LPDIRECT3DTEXTURE9 zz_renderer_d3d::get_texture (zz_handle texture_handle)
{
	if (!ZZ_HANDLE_IS_VALID(texture_handle)) {
		return 0;
	}
	zz_assert(texture_handle < d3d_textures.get_num_total());
    return this->d3d_textures[texture_handle];
}

//inline
bool zz_renderer_d3d::set_blend_type (ZZ_BLEND src, ZZ_BLEND dest, ZZ_BLENDOP op)
{
	//assert(op >= 1 && op <= 5 && "blendop must be [1, 5]");
	if (FAILED(d3d_device->SetRenderState(D3DRS_SRCBLEND, static_cast<D3DBLEND>(src)))) {
		ZZ_LOG("r_d3d: SetRenderState(D3DRS_SRCBLEND, %d) failed\n", src);
		return false;
	}
	if (FAILED(d3d_device->SetRenderState(D3DRS_DESTBLEND, static_cast<D3DBLEND>(dest)))) {
		ZZ_LOG("r_d3d: SetRenderState(D3DRS_DESTBLEND, %d) failed\n", src);
		return false;
	}
	/// BUG: D3DBLENDOP_MAX blend operation may affect the next alpha blended rendering in drawing with D3DSPRITE.
	///      In that situation, srcblend and destblend become to ONE and ONE.
	///      It is only in ATI radeon 8500 card.
	///      Solution: Before the sprite rendering, do set_blend_type(ONE, ZERO, ADD).
	if (FAILED(d3d_device->SetRenderState(D3DRS_BLENDOP, static_cast<D3DBLENDOP>(op)))) {
		ZZ_LOG("r_d3d: SetRenderState(D3DRS_BLENDOP, %d) failed\n", src);
		return false;
	}
	return true;
}

//const unsigned int TEST_MIN_MEM = 128000000;

unsigned int zz_renderer_d3d::get_max_texmem ()
{
	//return (max_texture_memory > TEST_MIN_MEM) ? max_texture_memory - TEST_MIN_MEM : 0;
	return max_texture_memory;
}

unsigned int zz_renderer_d3d::get_available_texmem ()
{
	if (!d3d_device) return 0;

	//return (d3d_device->GetAvailableTextureMem() > TEST_MIN_MEM) ? (d3d_device->GetAvailableTextureMem() - TEST_MIN_MEM) : 0;

	return d3d_device->GetAvailableTextureMem();
}

bool zz_renderer_d3d::begin_sprite (int flag, const char * spritename)
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(!_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (_sprite_began) return false;
	if (_device_lost) return false;

	HRESULT hr;

	// zhotest
	if (spritename) {
		strcpy(_sprite_name, spritename);
	}
	else {
		_sprite_name[0] = '\0';
	}

	// CAUTION:
	// if d3dsprite's own render states conflicts with performance option in nvidia 61.77 driver.
	// Thus, we use our own render states by (D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE)
	try {
		if (FAILED(hr = d3d_sprite->Begin(flag | D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE))) {
			ZZ_LOG("r_d3d: begin_sprite(%s) failed. [%s]\n", _sprite_name, get_hresult_string(hr));
			zz_assert(0);
			return false;
		}
	}
	catch (...) {
		ZZ_LOG("r_d3d: begin_sprite(%s) failed.\n", _sprite_name);
		throw;
	}
	_sprite_began = true;

	// set initial transform
	d3d_sprite->SetTransform(&s_mat_id_d3d);

	init_sprite_state();

	return true;
}

// setting id3dxsprite's begin states
// ref) ID3DXSprite::Begin() Method
void zz_renderer_d3d::init_sprite_state ()
{
	enable_zbuffer(true);
	enable_zwrite(true);

	// following code is only for radeon. driver bug(?)
	// documents in zz_renderer_d3d::set_blend_type()
	set_blend_type(ZZ_BLEND_ONE, ZZ_BLEND_ZERO, ZZ_BLENDOP_ADD);
	enable_alpha_blend(true, ZZ_BT_NORMAL);
	enable_alpha_test(true, 0, ZZ_CMP_GREATER);
	
	enable_fog(false);

	set_vertex_shader( ZZ_HANDLE_NULL );
	set_pixel_shader( ZZ_HANDLE_NULL );

	set_cullmode(zz_render_state::ZZ_CULLMODE_NONE);
	// wrap0 0
	set_texture_stage_state( 0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE );
	set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_COLORARG2, ZZ_TA_DIFFUSE );

	set_texture_stage_state( 0, ZZ_TSS_ALPHAOP, ZZ_TOP_MODULATE);
	set_texture_stage_state( 0, ZZ_TSS_ALPHAARG1, ZZ_TA_TEXTURE );
	set_texture_stage_state( 0, ZZ_TSS_ALPHAARG2, ZZ_TA_DIFFUSE );

	set_texture_stage_state( 1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE );
	set_texture_stage_state( 1, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE );

	set_sampler_state( 0, ZZ_SAMP_ADDRESSU, ZZ_TADDRESS_CLAMP );
	set_sampler_state( 0, ZZ_SAMP_ADDRESSV, ZZ_TADDRESS_CLAMP );
}

// simulate sprite transform setting
// viewport / view|projection|world transform
void zz_renderer_d3d::init_sprite_transform (int surface_width, int surface_height)
{
	D3DXMATRIX ortho, d3d_mat_id;

	D3DXMatrixIdentity( &d3d_mat_id );
	float width = (float)surface_width;
	float height = (float)surface_height;

	// this orthogonal transform is same as d3dsprite's default projection transform
	D3DXMatrixOrthoOffCenterLH( &ortho,
		.5f,					width + .5f,
		height + .5f,		.5f,
		0, 1.0f );

	zz_viewport viewport;
	viewport.x = 0;
	viewport.y = 0;
	viewport.minz = 0;
	viewport.maxz = 1.0f;
	viewport.width = (DWORD)surface_width;
	viewport.height = (DWORD)surface_height;

	// changing render states
	set_viewport( viewport );
	
	d3d_device->SetTransform(D3DTS_VIEW, &d3d_mat_id);
	d3d_device->SetTransform(D3DTS_PROJECTION, &ortho);
	d3d_device->SetTransform(D3DTS_WORLD, &d3d_mat_id);
}

bool zz_renderer_d3d::end_sprite ()
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	HRESULT hr;

	try {
		if (FAILED(hr = d3d_sprite->End())) {
			ZZ_LOG("r_d3d: end_sprite(%s) failed. [%s]\n", _sprite_name, get_hresult_string(hr));
			recreate_default_sprite();
		}
	}
	catch (...) {
		ZZ_LOG("r_d3d: end_sprite(%s) failed.\n", _sprite_name);
		recreate_default_sprite();
	}

	_sprite_name[0] = 0; // clear name

	_sprite_began = false;
	return true;
}

bool zz_renderer_d3d::flush_sprite ()
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	HRESULT hr;

	try {
		if (FAILED(hr = d3d_sprite->Flush())) {
			ZZ_LOG("r_d3d: flush_sprite(%s) failed.[%s]\n", _sprite_name, get_hresult_string(hr));
			recreate_default_sprite();
		}
	}
	catch (...) {
		ZZ_LOG("r_d3d: flush_sprite(%s) failed.\n", _sprite_name);
		recreate_default_sprite();
	}

	return true;
}

bool zz_renderer_d3d::sprite_began ()
{
	return _sprite_began;
}

bool zz_renderer_d3d::draw_sprite ( zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color)
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	if (!tex) return false;

	if (!tex->get_device_updated()) {
		assert(znzin->textures);
		znzin->textures->bind_next_frame(tex);
		return true;
	}

	tex->update_last_settime();

	zz_handle tex_handle = tex->get_texture_handle();

	if (!ZZ_HANDLE_IS_VALID(tex_handle)) return false; //  invalid texture handle

	LPDIRECT3DTEXTURE9 d3d_tex = d3d_textures[tex_handle];

	HRESULT hr;

	if (FAILED(hr = d3d_sprite->Draw(
		d3d_tex,
		(const RECT *)(src_rect),
		(const D3DXVECTOR3 *)(center),
		(const D3DXVECTOR3 *)(position),
		color)))
	{
		ZZ_LOG("r_d3d: draw_sprite(%s) failed. [%s]\n", tex->get_path(), get_hresult_string(hr));
		return false;
	}

	return true;
}



bool zz_renderer_d3d::set_sprite_transform ( const float * tm )
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	HRESULT hr;
	if (FAILED(hr = d3d_sprite->SetTransform((const D3DXMATRIX*)tm))) {
		ZZ_LOG("r_d3d: set_sprite_transform(%s) failed. [%s]\n", _sprite_name, get_hresult_string(hr));
		zz_assert(0);
		return false;
	}

	return true;
}

bool zz_renderer_d3d::get_sprite_transform ( float * tm )
{
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	HRESULT hr;
	if (FAILED(hr = d3d_sprite->GetTransform((D3DXMATRIX*)tm))) {
		ZZ_LOG("r_d3d: get_sprite_transform(%s) failed. [%s]\n", _sprite_name, get_hresult_string(hr));
		zz_assert(0);
		return false;
	}

	return true;
}

bool zz_renderer_d3d::draw_sprite_cover ( zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 origin_color, color32 cover_color,float value)
{
	
	if( fabsf(value)<0.001f)
	{
	 	bool return_value;
		return_value = draw_sprite(tex,src_rect,center,position,origin_color);
	    return return_value;    
	}
	
	assert(d3d_sprite);
	assert(_scene_began);
	assert(_sprite_began);
	assert(!_device_lost);

	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	if (!_sprite_began) return false;
	if (_device_lost) return false;

	if (!tex) return false;

	if (!tex->get_device_updated()) {
		assert(znzin->textures);
		znzin->textures->bind_next_frame(tex);
		return true;
	}

	zz_rect rect;
	vec3 center_;
	vec3 position_;
	HRESULT hr;
    D3DXVECTOR3 texture_center;
	float length_xy[2];
	DWORD texture_state1, texture_state2, texture_state3;
	float angle[8];
    float angle_value;
	int state;
	float plane[3];
	float vector[2];
    float point[2],t; 
	MYLINEVERTEX *gVertPool;
    VERTEX_SPRITE *gVertPool2;
	float texture_width, texture_height;

	if(sprite_vertexbuffer_cover == NULL)
	{
    
		if (FAILED(hr = d3d_device->CreateVertexBuffer( 24 * sizeof(MYLINEVERTEX),  
			D3DUSAGE_WRITEONLY,
			D3DFVF_XYZRHW | D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED,
			&sprite_vertexbuffer_cover, NULL )))
  	    zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for glow_vb failed. [%s]", get_hresult_string(hr));
	}
	
	if(sprite_vertexbuffer_origin == NULL)
	{

        if (FAILED(hr = d3d_device->CreateVertexBuffer( 6 * sizeof(VERTEX_SPRITE),  
			D3DUSAGE_WRITEONLY,
			D3DFVF_SPRITE,
			D3DPOOL_MANAGED,
			&sprite_vertexbuffer_origin, NULL )))
  	    zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for glow_vb failed. [%s]", get_hresult_string(hr));
	
	}
	
	
	tex->update_last_settime();

	zz_handle tex_handle = tex->get_texture_handle();

	if (!ZZ_HANDLE_IS_VALID(tex_handle)) return false; //  invalid texture handle

	LPDIRECT3DTEXTURE9 d3d_tex = d3d_textures[tex_handle];
	
	
	state = 8;
    angle_value = value * 3.141592f * 2.0f;

	d3d_device->GetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_ALPHAOP), &texture_state1);
	d3d_device->GetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_COLORARG1), &texture_state2);
	d3d_device->GetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_COLOROP), &texture_state3);
	
	
	
	enable_fog(false);
	
	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
	if(src_rect == NULL)
	{
		rect.bottom = tex->get_height();
		rect.top = 0;
		rect.left = 0;
		rect.right = tex->get_width();
	}
	else
	{
        rect = *src_rect;
	}
	
	
	if(center == NULL)
	{
		center_.x = 0.0f;
		center_.y = 0.0f;
		center_.z = 0.0f;
	}
	else
	{
	    center_ = *center;
	}
	
	if(position == NULL)
	{
		position_.x = 0.0f;
		position_.y = 0.0f;
		position_.z = 0.0f;
	}
	else
	{
		position_ = *position;
	}
	
	D3DXMATRIX sm;
	d3d_sprite->GetTransform(&sm);


	texture_center.x =  sm._11 * (position_.x+ ( (-rect.left + rect.right)*0.5f-center_.x)) + sm._41;
    texture_center.y =  sm._22 * (position_.y+ ( (-rect.top + rect.bottom)*0.5f-center_.y)) + sm._42;
    texture_center.z = center_.z;

	length_xy[0] = sm._11 * (-rect.left + rect.right)*0.5f;
	length_xy[1] = sm._22 * (-rect.top + rect.bottom)*0.5f;
   
	angle[0] = atanf(length_xy[0]/length_xy[1]);
	angle[1] = 3.141592f*0.5f;
	angle[2] = 3.141592f-angle[0];
	angle[3] = 3.141592f;
    angle[4] = 3.141592f + angle[0];
	angle[5] = 3.141592f * 1.5f;
	angle[6] = 3.141592f * 2.0f - angle[0];
	angle[7] = 3.141592f * 2.0f; 
		
	vector[0] = sinf(angle_value);
	vector[1] = -cosf(angle_value);
	
	texture_width = (float)tex->get_width();
	texture_height = (float)tex->get_height();
	
	if(angle_value > 0.0f && angle_value < angle[0])
	{
		state = 0;
		plane[0] = 0.0f; plane[1] = -1.0f; plane[2] = -length_xy[1];
	}
	else if(angle_value < angle[1])
	{
		state = 1;
	    plane[0] = 1.0f; plane[1] = 0.0f; plane[2] = -length_xy[0];
	}
	else if(angle_value < angle[2])
	{	
		state = 2;
	    plane[0] = 1.0f; plane[1] = 0.0f; plane[2] = -length_xy[0];
	}
	else if(angle_value < angle[3])
	{	
		state = 3;
	    plane[0] = 0.0f; plane[1] = 1.0f; plane[2] = -length_xy[1];
	}
	else if(angle_value < angle[4])
	{
		state = 4;
	    plane[0] = 0.0f; plane[1] = 1.0f; plane[2] = -length_xy[1];
	}
	else if(angle_value < angle[5])
	{	
		state = 5;
	    plane[0] = -1.0f; plane[1] = 0.0f; plane[2] = -length_xy[0];
	}
	else if(angle_value < angle[6])
	{	
		state = 6;
	    plane[0] = -1.0f; plane[1] = 0.0f; plane[2] = -length_xy[0];
	}
	else if(angle_value < angle[7])
	{
		state = 7;
	    plane[0] = 0.0f; plane[1] = -1.0f; plane[2] = -length_xy[1];
	}
	else 
		state =-1;
	
	if(state != -1)
	{
	
		t = -(plane[2]) / (plane[0]*vector[0] + plane[1]*vector[1]);
		point[0] = texture_center.x + t*vector[0];
		point[1] = texture_center.y + t*vector[1];
		
	    	
		sprite_vertexbuffer_origin->Lock( 0, 0, (void**)&gVertPool2, 0 );

        gVertPool2[0].position.x = texture_center.x - length_xy[0]; gVertPool2[0].position.y = texture_center.y - length_xy[1]; gVertPool2[0].position.z = center_.z; gVertPool2[0].position.w = 1.0f; gVertPool2[0].color = origin_color; gVertPool2[0].uv.x = rect.left/texture_width; gVertPool2[0].uv.y = rect.top/texture_height;
		gVertPool2[1].position.x = texture_center.x - length_xy[0]; gVertPool2[1].position.y = texture_center.y + length_xy[1]; gVertPool2[1].position.z = center_.z; gVertPool2[1].position.w = 1.0f; gVertPool2[1].color = origin_color; gVertPool2[1].uv.x = rect.left/texture_width; gVertPool2[1].uv.y = rect.bottom/texture_height; 
		gVertPool2[2].position.x = texture_center.x + length_xy[0]; gVertPool2[2].position.y = texture_center.y + length_xy[1];	gVertPool2[2].position.z = center_.z; gVertPool2[2].position.w = 1.0f; gVertPool2[2].color = origin_color; gVertPool2[2].uv.x = rect.right/texture_width; gVertPool2[2].uv.y =  rect.bottom/texture_height;
	    
		gVertPool2[3].position.x = texture_center.x - length_xy[0]; gVertPool2[3].position.y = texture_center.y - length_xy[1]; gVertPool2[3].position.z = center_.z; gVertPool2[3].position.w = 1.0f; gVertPool2[3].color = origin_color; gVertPool2[3].uv.x = rect.left/texture_width; gVertPool2[3].uv.y = rect.top/texture_height;
		gVertPool2[4].position.x = texture_center.x + length_xy[0]; gVertPool2[4].position.y = texture_center.y + length_xy[1]; gVertPool2[4].position.z = center_.z; gVertPool2[4].position.w = 1.0f; gVertPool2[4].color = origin_color; gVertPool2[4].uv.x = rect.right/texture_width; gVertPool2[4].uv.y = rect.bottom/texture_height; 
		gVertPool2[5].position.x = texture_center.x + length_xy[0]; gVertPool2[5].position.y = texture_center.y - length_xy[1];	gVertPool2[5].position.z = center_.z; gVertPool2[5].position.w = 1.0f; gVertPool2[5].color = origin_color; gVertPool2[5].uv.x = rect.right/texture_width; gVertPool2[5].uv.y = rect.top/texture_height;
	    
        sprite_vertexbuffer_origin->Unlock();
		
      
		d3d_device->SetStreamSource( 0, sprite_vertexbuffer_origin, 0, sizeof(VERTEX_SPRITE) );
	   	d3d_device->SetTexture(0, d3d_tex);
		d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST,0,2);
		
		
		set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	    set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	    set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
 
		
		sprite_vertexbuffer_cover->Lock( 0, 0, (void**)&gVertPool, 0 ); 
		
		
		
		gVertPool[0].pos.x = texture_center.x; gVertPool[0].pos.y = texture_center.y - length_xy[1]; gVertPool[0].pos.z = center_.z; gVertPool[0].pos.w = 1.0f; gVertPool[0].diffuse = cover_color;
		gVertPool[1].pos.x = texture_center.x; gVertPool[1].pos.y = texture_center.y; gVertPool[1].pos.z = center_.z; gVertPool[1].pos.w = 1.0f; gVertPool[1].diffuse = cover_color; 
		gVertPool[2].pos.x = texture_center.x + length_xy[0]; gVertPool[2].pos.y = texture_center.y - length_xy[1];	gVertPool[2].pos.z =  center_.z; gVertPool[2].pos.w = 1.0f; gVertPool[2].diffuse = cover_color;
	    
		if(state >= 1)
		{
			gVertPool[3].pos.x = texture_center.x + length_xy[0]; gVertPool[3].pos.y = texture_center.y - length_xy[1];	gVertPool[3].pos.z =  center_.z; gVertPool[3].pos.w = 1.0f; gVertPool[3].diffuse = cover_color;
   			gVertPool[4].pos.x = texture_center.x; gVertPool[4].pos.y = texture_center.y; gVertPool[4].pos.z =  center_.z; gVertPool[4].pos.w = 1.0f; gVertPool[4].diffuse = cover_color; 
			gVertPool[5].pos.x = texture_center.x + length_xy[0]; gVertPool[5].pos.y = texture_center.y; gVertPool[5].pos.z =  center_.z; gVertPool[5].pos.w = 1.0f; gVertPool[5].diffuse = cover_color;
		}   
		
		if(state >=2 )
		{
			gVertPool[6].pos.x = texture_center.x + length_xy[0]; gVertPool[6].pos.y = texture_center.y; gVertPool[6].pos.z =  center_.z; gVertPool[6].pos.w = 1.0f; gVertPool[6].diffuse = cover_color;
   			gVertPool[7].pos.x = texture_center.x; gVertPool[7].pos.y = texture_center.y; gVertPool[7].pos.z =  center_.z; gVertPool[7].pos.w = 1.0f; gVertPool[7].diffuse = cover_color; 
			gVertPool[8].pos.x = texture_center.x + length_xy[0]; gVertPool[8].pos.y = texture_center.y + length_xy[1]; gVertPool[8].pos.z =  center_.z; gVertPool[8].pos.w = 1.0f; gVertPool[8].diffuse = cover_color;
		}  
		
		if(state >=3)
		{
			gVertPool[9].pos.x = texture_center.x + length_xy[0]; gVertPool[9].pos.y = texture_center.y + length_xy[1]; gVertPool[9].pos.z =  center_.z; gVertPool[9].pos.w = 1.0f; gVertPool[9].diffuse = cover_color;
   			gVertPool[10].pos.x = texture_center.x; gVertPool[10].pos.y = texture_center.y; gVertPool[10].pos.z =  center_.z; gVertPool[10].pos.w = 1.0f; gVertPool[10].diffuse = cover_color; 
			gVertPool[11].pos.x = texture_center.x; gVertPool[11].pos.y = texture_center.y + length_xy[1]; gVertPool[11].pos.z =  center_.z; gVertPool[11].pos.w = 1.0f; gVertPool[11].diffuse = cover_color;
		}   
		
		if(state >= 4)
		{
			gVertPool[12].pos.x = texture_center.x; gVertPool[12].pos.y = texture_center.y + length_xy[1]; gVertPool[12].pos.z =  center_.z; gVertPool[12].pos.w = 1.0f; gVertPool[12].diffuse = cover_color;
   			gVertPool[13].pos.x = texture_center.x; gVertPool[13].pos.y = texture_center.y; gVertPool[13].pos.z =  center_.z; gVertPool[13].pos.w = 1.0f; gVertPool[13].diffuse = cover_color; 
			gVertPool[14].pos.x = texture_center.x - length_xy[0]; gVertPool[14].pos.y = texture_center.y + length_xy[1]; gVertPool[14].pos.z =  center_.z; gVertPool[14].pos.w = 1.0f; gVertPool[14].diffuse = cover_color;
		}

        if(state >= 5)
		{
			gVertPool[15].pos.x = texture_center.x - length_xy[0]; gVertPool[15].pos.y = texture_center.y + length_xy[1]; gVertPool[15].pos.z = center_.z; gVertPool[15].pos.w = 1.0f; gVertPool[15].diffuse = cover_color;
   			gVertPool[16].pos.x = texture_center.x; gVertPool[16].pos.y = texture_center.y; gVertPool[16].pos.z =  center_.z; gVertPool[16].pos.w = 1.0f; gVertPool[16].diffuse = cover_color; 
			gVertPool[17].pos.x = texture_center.x - length_xy[0]; gVertPool[17].pos.y = texture_center.y; gVertPool[17].pos.z =  center_.z; gVertPool[17].pos.w = 1.0f; gVertPool[17].diffuse = cover_color;
			}  
		
		if(state >= 6)
		{
			gVertPool[18].pos.x = texture_center.x - length_xy[0]; gVertPool[18].pos.y = texture_center.y; gVertPool[18].pos.z =  center_.z; gVertPool[18].pos.w = 1.0f; gVertPool[18].diffuse = cover_color;
   			gVertPool[19].pos.x = texture_center.x; gVertPool[19].pos.y = texture_center.y; gVertPool[19].pos.z =  center_.z; gVertPool[19].pos.w = 1.0f; gVertPool[19].diffuse = cover_color; 
			gVertPool[20].pos.x = texture_center.x - length_xy[0]; gVertPool[20].pos.y = texture_center.y - length_xy[1]; gVertPool[20].pos.z =  center_.z; gVertPool[20].pos.w = 1.0f; gVertPool[20].diffuse = cover_color;
			}  
		
		if(state >=7)
		{
			gVertPool[21].pos.x = texture_center.x - length_xy[0]; gVertPool[21].pos.y = texture_center.y - length_xy[1]; gVertPool[21].pos.z =  center_.z; gVertPool[21].pos.w = 1.0f; gVertPool[21].diffuse = cover_color;
   			gVertPool[22].pos.x = texture_center.x; gVertPool[22].pos.y = texture_center.y; gVertPool[22].pos.z =  center_.z; gVertPool[22].pos.w = 1.0f; gVertPool[22].diffuse = cover_color; 
			gVertPool[23].pos.x = texture_center.x; gVertPool[23].pos.y = texture_center.y - length_xy[1]; gVertPool[23].pos.z =  center_.z; gVertPool[23].pos.w = 1.0f; gVertPool[23].diffuse = cover_color;
		}  
		
		gVertPool[3*state+2].pos.x = point[0];
		gVertPool[3*state+2].pos.y = point[1];
		
		sprite_vertexbuffer_cover->Unlock();
		
		d3d_device->SetStreamSource( 0, sprite_vertexbuffer_cover, 0, sizeof(MYLINEVERTEX) );
	   	d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
		d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST,0,state+1);
		
	    d3d_device->SetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_ALPHAOP), texture_state1);
	    d3d_device->SetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_COLORARG1), texture_state2);
	    d3d_device->SetTextureStageState(0, static_cast<D3DTEXTURESTAGESTATETYPE>(ZZ_TSS_COLOROP), texture_state3);
	}
	
	return true;

}

bool zz_renderer_d3d::draw_sprite_ex ( zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color)
{
	
	assert(d3d_sprite);
	assert(_scene_began);
	
	if (!d3d_sprite) return false;
	if (!_scene_began) return false;
	

	if (!tex) return false;

	if (!tex->get_device_updated()) {
		assert(znzin->textures);
		znzin->textures->bind_next_frame(tex);
		return true;
	}

	D3DXMATRIX buffer_m;

	D3DXMatrixIdentity(&buffer_m);

    tex->update_last_settime();

	zz_handle tex_handle = tex->get_texture_handle();

	if (!ZZ_HANDLE_IS_VALID(tex_handle)) return false; //  invalid texture handle

	LPDIRECT3DTEXTURE9 d3d_tex = d3d_textures[tex_handle];

    HRESULT hr;

	d3d_sprite->Begin(ZZ_SPRITE_ALPHABLEND | D3DXSPRITE_DONOTSAVESTATE | D3DXSPRITE_DONOTMODIFY_RENDERSTATE);
    d3d_sprite->SetTransform(&buffer_m);
       
    if (FAILED(hr = d3d_sprite->Draw(
		d3d_tex,
		(const RECT *)(src_rect),
		(const D3DXVECTOR3 *)(center),
		(const D3DXVECTOR3 *)(position),
		color)))
	{
		ZZ_LOG("r_d3d: draw_sprite(%s) failed. [%s]\n", tex->get_path(), get_hresult_string(hr));
		return false;
	}

	d3d_sprite->End();
	  
	
	/*
	zz_rect rect;
	vec3 center_;
	HRESULT hr;
    D3DXVECTOR3 texture_center;
	float length_xy[2];
	
   
    VERTEX_SPRITE *gVertPool;
	float texture_width, texture_height;

	if(sprite_vertexbuffer_origin_ex == NULL)
	{

        if (FAILED(hr = d3d_device->CreateVertexBuffer( 6 * sizeof(VERTEX_SPRITE),  
			D3DUSAGE_WRITEONLY,
			D3DFVF_SPRITE,
			D3DPOOL_MANAGED,
			&sprite_vertexbuffer_origin_ex, NULL )))
  	    zz_assertf(0, "renderer_d3d: restore_device_objects() failed. createvertexbuffer() for glow_vb failed. [%s]", get_hresult_string(hr));
	
	}
	
	
	tex->update_last_settime();

	zz_handle tex_handle = tex->get_texture_handle();

	if (!ZZ_HANDLE_IS_VALID(tex_handle)) return false; //  invalid texture handle

	LPDIRECT3DTEXTURE9 d3d_tex = d3d_textures[tex_handle];
	

	enable_fog(false);
	
	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
	if(src_rect == NULL)
	{
		rect.bottom = tex->get_height();
		rect.top = 0;
		rect.left = 0;
		rect.right = tex->get_width();
	}
	else
	{
        rect = *src_rect;
	}
	
	
	if(center == NULL)
	{
		center_.x = 0.0f;
		center_.y = 0.0f;
		center_.z = 0.0f;
	}
	else
	{
	    center_ = *center;
	}
	
	texture_center.x =  position->x+( (-rect.left + rect.right)*0.5f-center_.x);
    texture_center.y =  position->y+( (-rect.top + rect.bottom)*0.5f-center_.y);
    texture_center.z = center_.z;

	length_xy[0] = (-rect.left + rect.right)*0.5f;
	length_xy[1] = (-rect.top + rect.bottom)*0.5f;
   		
	texture_width = (float)tex->get_width();
	texture_height = (float)tex->get_height();
	
	sprite_vertexbuffer_origin_ex->Lock( 0, 0, (void**)&gVertPool, 0 );

    gVertPool[0].position.x = texture_center.x - length_xy[0]; gVertPool[0].position.y = texture_center.y - length_xy[1]; gVertPool[0].position.z = center_.z; gVertPool[0].position.w = 1.0f; gVertPool[0].color = color; gVertPool[0].uv.x = rect.left/texture_width; gVertPool[0].uv.y = rect.top/texture_height;
	gVertPool[1].position.x = texture_center.x - length_xy[0]; gVertPool[1].position.y = texture_center.y + length_xy[1]; gVertPool[1].position.z = center_.z; gVertPool[1].position.w = 1.0f; gVertPool[1].color = color; gVertPool[1].uv.x = rect.left/texture_width; gVertPool[1].uv.y = rect.bottom/texture_height; 
	gVertPool[2].position.x = texture_center.x + length_xy[0]; gVertPool[2].position.y = texture_center.y + length_xy[1];	gVertPool[2].position.z = center_.z; gVertPool[2].position.w = 1.0f; gVertPool[2].color = color; gVertPool[2].uv.x = rect.right/texture_width; gVertPool[2].uv.y =  rect.bottom/texture_height;
	    
	gVertPool[3].position.x = texture_center.x - length_xy[0]; gVertPool[3].position.y = texture_center.y - length_xy[1]; gVertPool[3].position.z = center_.z; gVertPool[3].position.w = 1.0f; gVertPool[3].color = color; gVertPool[3].uv.x = rect.left/texture_width; gVertPool[3].uv.y = rect.top/texture_height;
	gVertPool[4].position.x = texture_center.x + length_xy[0]; gVertPool[4].position.y = texture_center.y + length_xy[1]; gVertPool[4].position.z = center_.z; gVertPool[4].position.w = 1.0f; gVertPool[4].color = color; gVertPool[4].uv.x = rect.right/texture_width; gVertPool[4].uv.y = rect.bottom/texture_height; 
	gVertPool[5].position.x = texture_center.x + length_xy[0]; gVertPool[5].position.y = texture_center.y - length_xy[1];	gVertPool[5].position.z = center_.z; gVertPool[5].position.w = 1.0f; gVertPool[5].color = color; gVertPool[5].uv.x = rect.right/texture_width; gVertPool[5].uv.y = rect.top/texture_height;
	    
    sprite_vertexbuffer_origin_ex->Unlock();
		
    d3d_device->SetStreamSource( 0, sprite_vertexbuffer_origin_ex, 0, sizeof(VERTEX_SPRITE) );
	d3d_device->SetTexture(0, d3d_tex);
	d3d_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST,0,2);
			
	*/
	
	return true;

}

void zz_renderer_d3d::draw_wire_sphere(float x, float y, float z, float r)
{


	D3DXMATRIX mem_m1, mem_m2;
	D3DXMATRIX model_m;
	D3DXMATRIX t_m, s_m;
	mat4 camera_m;
	D3DXMATRIX projection_m;
	DWORD fill_mode_state;
	zz_camera * cam = znzin->get_camera();


	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
		
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->GetRenderState(D3DRS_FILLMODE, &fill_mode_state);


	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);


	if(sphere_buffer == NULL)
	{
		LPDIRECT3DVERTEXBUFFER9 pTempVertexBuffer;
		LPD3DXMESH pTempSphereMesh = NULL;
		
		struct SVertex{
		
			float x, y, z; 
			DWORD color; 
		};
		
		D3DXCreateSphere(d3d_device, 1.0f, 10, 10, &pTempSphereMesh, NULL);
		pTempSphereMesh->CloneMeshFVF( 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, d3d_device, &sphere_buffer);

		if( SUCCEEDED( sphere_buffer->GetVertexBuffer( &pTempVertexBuffer ) ) )
		{
			int nNumVerts = sphere_buffer->GetNumVertices();
			SVertex *pVertices = NULL;

			pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
			{
				for( int i = 0; i < nNumVerts; ++i )
					pVertices[i].color = D3DCOLOR_COLORVALUE( 1.0f,1.0f,0.0f,1.0f);
			}
			pTempVertexBuffer->Unlock();
			pTempVertexBuffer->Release();

		}

		pTempSphereMesh->Release();
	}

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);

	D3DXMatrixTranslation(&t_m,x, y, z);
	D3DXMatrixScaling (&s_m,r, r, r);
	model_m = s_m * t_m;
		
	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	sphere_buffer->DrawSubset(0);   


	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetRenderState(D3DRS_FILLMODE,fill_mode_state);

}

void zz_renderer_d3d::draw_wire_cylinder(float x, float y, float z, float length, float r)
{

    D3DXMATRIX mem_m1, mem_m2;
	D3DXMATRIX model_m;
	D3DXMATRIX t_m, s_m;
	mat4 camera_m;
	D3DXMATRIX projection_m;
	DWORD fill_mode_state;
	zz_camera * cam = znzin->get_camera();


	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
		
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->GetRenderState(D3DRS_FILLMODE, &fill_mode_state);


	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetRenderState(D3DRS_FILLMODE,D3DFILL_WIREFRAME);


	if(cylinder_buffer == NULL)
	{
		LPDIRECT3DVERTEXBUFFER9 pTempVertexBuffer;
		LPD3DXMESH pTempCylinderMesh = NULL;
		
		struct SVertex{
		
			float x, y, z; 
			DWORD color; 
		};
		
		D3DXCreateCylinder(d3d_device, 1.0f,1.0f,1.0f,16,5,&pTempCylinderMesh, NULL);
		pTempCylinderMesh->CloneMeshFVF( 0, D3DFVF_XYZ | D3DFVF_DIFFUSE, d3d_device, &cylinder_buffer);

		if( SUCCEEDED( cylinder_buffer->GetVertexBuffer( &pTempVertexBuffer ) ) )
		{
			int nNumVerts = cylinder_buffer->GetNumVertices();
			SVertex *pVertices = NULL;

			pTempVertexBuffer->Lock( 0, 0, (void**)&pVertices, 0 );
			{
				for( int i = 0; i < nNumVerts; ++i )
					pVertices[i].color = D3DCOLOR_COLORVALUE( 0.5f,0.5f,1.0f,1.0f);
			}
			pTempVertexBuffer->Unlock();
			pTempVertexBuffer->Release();

		}

		pTempCylinderMesh->Release();
	}

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);

	D3DXMatrixTranslation(&t_m,x, y, z);
	D3DXMatrixScaling (&s_m,r, r, length);
	model_m = s_m * t_m;
		
	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	cylinder_buffer->DrawSubset(0);   


	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->SetRenderState(D3DRS_FILLMODE,fill_mode_state);



}


void zz_renderer_d3d::draw_visible_boundingbox(const mat4& matrix, float min_vec[3], float max_vec[3],DWORD color)
{
    D3DXMATRIX mem_m1, mem_m2;
	mat4 camera_m;
	D3DXMATRIX projection_m;
	DWORD fill_mode_state;
	zz_camera * cam = znzin->get_camera();

    struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};

	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
		
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	d3d_device->GetRenderState(D3DRS_FILLMODE, &fill_mode_state);


	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);

	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);

	
	if(boundingbox_vertexbuffer == NULL || boundingbox_indexbuffer ==NULL)
	{
		d3d_device->CreateVertexBuffer(8*sizeof(MYLINEVERTEX), D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE,
                                       D3DPOOL_MANAGED, &boundingbox_vertexbuffer, NULL );

    
	      
		d3d_device->CreateIndexBuffer(24*sizeof(unsigned short), 0, D3DFMT_INDEX16, 
										   D3DPOOL_MANAGED, &boundingbox_indexbuffer, NULL);
 
		WORD indexList[24]  =  {0, 1,
								1, 2,
								2, 3,
								3, 0,
								4, 5,
								5, 6,
								6, 7,
								7, 4,
								0, 4,
								1, 5,
								2, 6,
								3, 7};
		
		VOID *index = NULL; 
		boundingbox_indexbuffer->Lock(0,0,(void**)&index,D3DLOCK_DISCARD);
		memcpy(index, indexList, 24*sizeof(WORD));
	    boundingbox_indexbuffer->Unlock();
	
	}
     
	MYLINEVERTEX *vertex_pool;
	boundingbox_vertexbuffer->Lock( 0, 0, (void**)&vertex_pool, 0 );
    
	vertex_pool[0].pos.x=min_vec[0];vertex_pool[0].pos.y=max_vec[1];vertex_pool[0].pos.z=max_vec[2];vertex_pool[0].diffuse=color;
	vertex_pool[1].pos.x=min_vec[0];vertex_pool[1].pos.y=min_vec[1];vertex_pool[1].pos.z=max_vec[2];vertex_pool[1].diffuse=color;
	vertex_pool[2].pos.x=max_vec[0];vertex_pool[2].pos.y=min_vec[1];vertex_pool[2].pos.z=max_vec[2];vertex_pool[2].diffuse=color;
	vertex_pool[3].pos.x=max_vec[0];vertex_pool[3].pos.y=max_vec[1];vertex_pool[3].pos.z=max_vec[2];vertex_pool[3].diffuse=color;
	vertex_pool[4].pos.x=min_vec[0];vertex_pool[4].pos.y=max_vec[1];vertex_pool[4].pos.z=min_vec[2];vertex_pool[4].diffuse=color;
	vertex_pool[5].pos.x=min_vec[0];vertex_pool[5].pos.y=min_vec[1];vertex_pool[5].pos.z=min_vec[2];vertex_pool[5].diffuse=color;
	vertex_pool[6].pos.x=max_vec[0];vertex_pool[6].pos.y=min_vec[1];vertex_pool[6].pos.z=min_vec[2];vertex_pool[6].diffuse=color;
	vertex_pool[7].pos.x=max_vec[0];vertex_pool[7].pos.y=max_vec[1];vertex_pool[7].pos.z=min_vec[2];vertex_pool[7].diffuse=color;
		
	boundingbox_vertexbuffer->Unlock();

	
	d3d_device->SetStreamSource(0, boundingbox_vertexbuffer, 0, sizeof(MYLINEVERTEX));
    d3d_device->SetIndices(boundingbox_indexbuffer);
    d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    d3d_device->SetTransform(D3DTS_WORLD,(const D3DXMATRIX *)&matrix);	
	d3d_device->DrawIndexedPrimitive(D3DPT_LINELIST,0,0, 8,0, 12);
	
	
	
	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2);
}

void zz_renderer_d3d::draw_axis_object(mat4& object_matrix, float size)
{


	D3DXMATRIX mem_m1, mem_m2;
	D3DXMATRIX model_m;
	D3DXMATRIX t_m, s_m;
	mat4 camera_m;
	D3DXMATRIX projection_m;
	zz_camera * cam = znzin->get_camera();


	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	set_projection_matrix(projection_matrix);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW, camera_m);
		
	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	

	if(znzin->camera_sfx.get_play_onoff())
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	else
		d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&camera_m);

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);
	
	set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
	set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
	set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
	set_texture_stage_state(1, ZZ_TSS_COLOROP, D3DTOP_DISABLE );
	enable_alpha_blend(false, ZZ_BT_NORMAL);

	enable_fog(false);

	set_vertex_shader(ZZ_HANDLE_NULL);
	set_pixel_shader(ZZ_HANDLE_NULL);
	
		
	d3d_device->SetTransform(D3DTS_WORLD,(D3DXMATRIX *)&object_matrix);
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	draw_axis(size);  


	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2);


}

zz_renderer_cached_info& zz_renderer_d3d::get_cached_info ()
{
	return _cached;
}

bool zz_renderer_d3d::_begin_scene (const char * msg)
{
	if (FAILED(d3d_device->BeginScene())) {
		ZZ_LOG("r_d3d: beginscene(%s) failed.\n", msg);
		return false;
	}
	return true;
}

bool zz_renderer_d3d::_end_scene (const char * msg)
{
	if (FAILED(d3d_device->EndScene())) {
		ZZ_LOG("r_d3d: endscene(%s) failed.\n", msg);
		return false;
	}
	return true;
}

bool zz_renderer_d3d::has_device ()
{
	return (d3d_device != NULL);
}

bool zz_renderer_d3d::support_hw_mouse ()
{
	if (!has_device()) {
		zz_assertf(0, "no d3d device");
		return false;
	}

	return (device_capability.CursorCaps & D3DCURSORCAPS_COLOR);
}	

bool zz_renderer_d3d::validate_device ()
{
	zz_assert(has_device());

	HRESULT hr;
	DWORD num_passes;
	if (FAILED(hr = d3d_device->ValidateDevice(&num_passes))) {
		zz_assertf(0, "renderer_d3d: validate_device() failed. [%s]\n", get_hresult_string(hr));
		return false;
	}
	return true;
}

void zz_renderer_d3d::invalidate_texture (int stage)
{
	_cached.invalidate_texture(stage);
}

void zz_renderer_d3d::get_viewport (zz_viewport& viewport)
{
	zz_assert(d3d_device);
	d3d_device->GetViewport( reinterpret_cast<D3DVIEWPORT9*>(&viewport) );
}

void zz_renderer_d3d::set_viewport (const zz_viewport& viewport)
{
	zz_assert(d3d_device);
	d3d_device->SetViewport( reinterpret_cast<const D3DVIEWPORT9*>(&viewport) );
}

bool zz_renderer_d3d::support_color_write ()
{
	return (device_capability.PrimitiveMiscCaps & D3DPMISCCAPS_COLORWRITEENABLE) != 0;
}

void zz_renderer_d3d::enable_color_write (dword flag)
{
	d3d_device->SetRenderState(D3DRS_COLORWRITEENABLE, flag);
}

int zz_renderer_d3d::get_num_simultaneous_render_target ()
{
	return static_cast<int>(device_capability.NumSimultaneousRTs);
}



#ifdef _DEBUG
void zz_renderer_d3d::test ()
{
	vertex_decls.dump();
	vertex_shaders.dump();
	pixel_shaders.dump();
	d3d_textures.dump();
	vertex_buffer_pool.dump();
	index_buffer_pool.dump();
}
#endif