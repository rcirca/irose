/** 
 * @file zz_sfx.cpp
 * @brief post render special effect class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-mar-2004
 *
 * $Header: /engine/src/zz_sfx.cpp 40    06-12-29 4:24a Choo0219 $
 * $History: zz_sfx.cpp $
 * 
 * *****************  Version 40  *****************
 * User: Choo0219     Date: 06-12-29   Time: 4:24a
 * Updated in $/engine/src
 * 
 * *****************  Version 39  *****************
 * User: Choo0219     Date: 02-01-01   Time: 1:13a
 * Updated in $/engine/src
 * 
 * *****************  Version 38  *****************
 * User: Choo0219     Date: 06-01-03   Time: 10:41a
 * Updated in $/engine/src
 * 
 * *****************  Version 37  *****************
 * User: Choo0219     Date: 05-12-24   Time: 10:23p
 * Updated in $/engine/src
 * 
 * *****************  Version 36  *****************
 * User: Choo0219     Date: 05-12-16   Time: 8:11p
 * Updated in $/engine/src
 * 
 * *****************  Version 35  *****************
 * User: Choo0219     Date: 05-06-01   Time: 10:40a
 * Updated in $/engine/src
 * 
 * *****************  Version 34  *****************
 * User: Choo0219     Date: 05-06-01   Time: 10:26a
 * Updated in $/engine/src
 * 
 * *****************  Version 33  *****************
 * User: Choo0219     Date: 05-05-30   Time: 1:54p
 * Updated in $/engine/src
 * 
 * *****************  Version 32  *****************
 * User: Choo0219     Date: 05-05-29   Time: 5:27p
 * Updated in $/engine/src
 * 
 * *****************  Version 31  *****************
 * User: Choo0219     Date: 05-05-28   Time: 4:31p
 * Updated in $/engine/src
 * 
 * *****************  Version 30  *****************
 * User: Choo0219     Date: 05-03-29   Time: 4:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 29  *****************
 * User: Choo0219     Date: 05-02-16   Time: 12:12p
 * Updated in $/engine/src
 * 
 * *****************  Version 28  *****************
 * User: Choo0219     Date: 04-11-24   Time: 5:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 27  *****************
 * User: Choo0219     Date: 04-11-23   Time: 4:04p
 * Updated in $/engine/src
 * 
 * *****************  Version 26  *****************
 * User: Choo0219     Date: 04-11-23   Time: 1:41p
 * Updated in $/engine/src
 * 
 * *****************  Version 25  *****************
 * User: Choo0219     Date: 04-11-23   Time: 10:52a
 * Updated in $/engine/src
 * 
 * *****************  Version 24  *****************
 * User: Choo0219     Date: 04-11-16   Time: 4:30p
 * Updated in $/engine/src
 * 
 * *****************  Version 23  *****************
 * User: Choo0219     Date: 04-11-11   Time: 5:22p
 * Updated in $/engine/src
 * 
 * *****************  Version 22  *****************
 * User: Jeddli       Date: 04-11-08   Time: 10:21p
 * Updated in $/engine/src
 * 
 * *****************  Version 21  *****************
 * User: Choo0219     Date: 04-11-08   Time: 8:57p
 * Updated in $/engine/src
 * 
 * *****************  Version 20  *****************
 * User: Choo0219     Date: 04-11-08   Time: 8:31p
 * Updated in $/engine/src
 * 
 * *****************  Version 19  *****************
 * User: Choo0219     Date: 04-11-08   Time: 2:50p
 * Updated in $/engine/src
 * 
 * *****************  Version 18  *****************
 * User: Choo0219     Date: 04-11-08   Time: 2:05p
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Choo0219     Date: 04-11-08   Time: 12:04a
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Choo0219     Date: 04-11-07   Time: 11:44p
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Choo0219     Date: 04-11-07   Time: 5:43p
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Choo0219     Date: 04-11-07   Time: 4:58p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Choo0219     Date: 04-11-06   Time: 8:45p
 * Updated in $/engine/src
 * 
 * *****************  Version 12  *****************
 * User: Choo0219     Date: 04-11-05   Time: 3:03p
 * Updated in $/engine/src
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-09-15   Time: 10:40a
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-09-01   Time: 2:59p
 * Updated in $/engine/src
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-08-24   Time: 1:55p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-08-16   Time: 2:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-08-02   Time: 5:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-04-24   Time: 10:09a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-03-31   Time: 10:24a
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-03-19   Time: 7:52p
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-03-19   Time: 5:28p
 * Created in $/engine/src
 * Added special effect.
 */

#include "zz_tier0.h"
#include "zz_sfx.h"
#include "zz_system.h"
#include "zz_texture.h"
#include "zz_renderer_d3d.h"
#include "zz_type.h"
#include "zz_view.h"
#include "zz_algebra.h"
#include "zz_material_ocean.h"
#include "zz_ocean_block.h"
#include "zz_manager.h"
#include "zz_vfs.h"
#include "zz_misc.h"
#include "zz_trail.h"
#include "zz_camera.h"
#include "zz_interface.h"

#include <algorithm>

// sfx_ocean uses ocean_block that currently in use.
// So, if we does not have any ocean_block available, we cannot render sfx_ocean.
class zz_sfx_ocean : public zz_sfx {
	zz_time time_interval;
	std::vector<zz_texture *> ocean_textures;
	int texture_index;

public:
	zz_sfx_ocean (); // should be called after initializing renderer
	virtual ~zz_sfx_ocean ();
	virtual bool update ();
	virtual bool render ();
};

zz_sfx_ocean * sfx_ocean = NULL;

zz_sfx_ocean::zz_sfx_ocean () :	time_interval(ZZ_MSEC_TO_TIME(100)),	texture_index(-1)
{
	if (!znzin) return;

	char filename[ZZ_MAX_STRING];
	zz_texture * tex;

	// zhotest
	char waterpath1[] = "3ddata/ground/water/ocean01_%02d.dds";
	char waterpath2[] = "3ddata/junon/water/ocean01_%02d.dds";

	zz_vfs vfs;
	char * waterpath = (vfs.exist("3ddata/junon/water/ocean01_01.dds")) ? waterpath2 : waterpath1;

	for (int i = 0; i < 25; ++i) {
		sprintf(filename, waterpath, i+1);

		tex = static_cast<zz_texture*>(znzin->textures->spawn(NULL, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */));
		tex->set_property(filename, 0, 0, false /* dynamic */, 1 /* miplevels */, true /* usefilter */, 
			zz_device_resource::ZZ_POOL_MANAGED, ZZ_FMT_UNKNOWN, true /* for image */);
		if (tex->load()) {
			tex->lock_texture();
			ocean_textures.push_back(tex);
		}
		else {
			ZZ_LOG("sfx_ocean: tex(%s) load failed.\n", filename);
		}
	}
}

zz_sfx_ocean::~zz_sfx_ocean ()
{
	for (std::vector<zz_texture*>::iterator it = ocean_textures.begin(); it != ocean_textures.end(); ++it) {
		zz_delete (*it);
	}
	ocean_textures.clear();
	texture_index = -1;
}

bool zz_sfx_ocean::update ()
{
	if (time_interval <= 0) return false;

	texture_index = znzin->get_current_time() / time_interval;
	texture_index %= ocean_textures.size();
	return true;
}

bool zz_sfx_ocean::render ()
{
	if (!znzin) return false;

	if (!update()) return false; // move to scene.update() stuff...
	
	zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
	zz_rect srcrect;

	assert(texture_index < (int)ocean_textures.size());

	zz_texture * ocean_texture = ocean_textures[texture_index];

	srcrect.left = 0;
	srcrect.right = znzin->view->get_width();
	srcrect.top = 0;
	srcrect.bottom = znzin->view->get_height();

	vec3 center(0, 0, 0), position(0, 0, 0);

	zz_color color(1.0f, 1.0f, 1.0f, 1.0f);

	mat4 sprite_tm = mat4_id;
	int texture_width = ocean_texture->get_width();
	int texture_height = ocean_texture->get_height();
	sprite_tm.a00 = static_cast<float>(srcrect.right)/texture_width;
	sprite_tm.a11 = static_cast<float>(srcrect.bottom)/texture_height;

	DWORD depth_test;
	LPDIRECT3DDEVICE9	d3d_device	= NULL;
	d3d_device						= r->get_device();
	 

	d3d_device->GetRenderState(D3DRS_ZENABLE,&depth_test);
	d3d_device->SetRenderState(D3DRS_ZENABLE,D3DZB_FALSE);
	


	r->begin_sprite( ZZ_SPRITE_ALPHABLEND, "ocean" );
	{
		r->set_sampler_state(0, ZZ_SAMP_ADDRESSU,  ZZ_TADDRESS_WRAP);
		r->set_sampler_state(0, ZZ_SAMP_ADDRESSV,  ZZ_TADDRESS_WRAP);
		r->set_sprite_transform( sprite_tm.mat_array );
		r->set_blend_type( ZZ_BLEND_ONE, ZZ_BLEND_ONE, ZZ_BLENDOP_ADD );
		//r->set_blend_type( ZZ_BLEND_SRCALPHA, ZZ_BLEND_INVSRCALPHA, ZZ_BLENDOP_ADD );
		r->draw_sprite( ocean_texture, &srcrect, &center, &position, color );
	}
	r->end_sprite();
	
	
	d3d_device->SetRenderState(D3DRS_ZENABLE,depth_test);
	
	
	return true;
}

zz_manager_sfx::zz_manager_sfx ()
{
}

zz_manager_sfx::~zz_manager_sfx ()
{
	cleanup();
}

// should be initialized after initializing renderer
bool zz_manager_sfx::initialize ()
{
	clear_sfx();
	
	if (!sfx_ocean) {
		sfx_ocean = zz_new zz_sfx_ocean();
	}
	return true;
}

bool zz_manager_sfx::cleanup ()
{
	clear_sfx();
	if (!sfx_ocean) return false;

	zz_delete sfx_ocean;
	sfx_ocean = NULL;

	return true;
}

void zz_manager_sfx::clear_sfx ()
{
	sequence.clear();
}

bool zz_manager_sfx::push_sfx (e_type type_in)
{
	switch (type_in)
	{
	case OCEAN:
	    if (!sfx_ocean) return false;
		sequence.push_back(sfx_ocean);
		break;
	}
	return true;
}

void zz_manager_sfx::pop_sfx ()
{
	sequence.pop_back();
}

void zz_manager_sfx::render_sfx ()
{
	std::for_each( sequence.begin(), sequence.end(), std::mem_fun<bool, zz_sfx>( &zz_sfx::render ) );
}

zz_screen_sfx::zz_screen_sfx()
{
	screen_texture=NULL;
	screen_surface=NULL;
	num_current_tile=0;
	play_onoff=FALSE;
	wide_screen_onoff = FALSE;
	clear();

}

zz_screen_sfx::~zz_screen_sfx()
{

	if(screen_texture)
	screen_texture->Release();

	if(screen_surface)
	screen_surface->Release(); 

}

void zz_screen_sfx::clear()
{
	if(screen_texture)
	{
	    screen_texture->Release();
		screen_texture=NULL;
	}
	if(screen_surface)
	{
		screen_surface->Release(); 
		screen_surface=NULL;
	}
	if(num_current_tile>0)
	{
	for(int i=0;i<num_current_tile;i+=1)
	{
		delete m_tiles[i];
		m_tiles[i]=NULL;
	}
	}

	texture_setup_onoff=FALSE;
	device_reset_onoff=FALSE;
	time=1.0f;  
	num_current_tile=0; 
	accumulate_time=0.0f;
}

void zz_screen_sfx::initialize()
{    
    clear();
}


void zz_screen_sfx::pre_make_texture()
{
   
	zz_camera * cam = znzin->get_camera();
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9 d3d_device;

	D3DFORMAT render_format;
	zz_view *view=znzin->view;
	zz_viewport view_port;
	d3d_device = r->get_device();
	               	
	if(!wide_screen_onoff)
		znzin->renderer->get_viewport(view_port);
	else
		view_port = wide_viewport;

	screen_texture_width=GetAdaptiveTextureSize(view_port.width);
	screen_texture_height=GetAdaptiveTextureSize(view_port.height);
	

	render_format = ((view->get_depth() == 16) ? D3DFMT_R5G6B5 : D3DFMT_A8R8G8B8);

	d3d_device->CreateTexture(screen_texture_width,screen_texture_height,1,D3DUSAGE_RENDERTARGET,render_format,D3DPOOL_DEFAULT,&screen_texture, NULL);

	screen_texture->GetSurfaceLevel(0,&screen_surface);

	d3d_device->GetRenderTarget(0,&screen_back_buffer);   
	    
	r->backbuffer_surface=screen_surface;
	d3d_device->SetRenderTarget(0,screen_surface);

}

void zz_screen_sfx::post_make_texture()
{
  
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9 d3d_device;

	d3d_device = r->get_device();
	r->backbuffer_surface=screen_back_buffer;
	d3d_device->SetRenderTarget(0,screen_back_buffer);

}


UINT zz_screen_sfx::GetAdaptiveTextureSize(UINT size)
{

    bool state;

	if(size<1)
	state = FALSE;
	else
	state = ((size&(size-1))==0);
	
	if(state)
    return size;
   
	return 1 << log2ge(size);

}

void zz_screen_sfx::make_tiles1()
{

    int i;
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    LPDIRECT3DDEVICE9 d3d_device;
	d3d_device = r->get_device();
    zz_camera *cam = znzin->get_camera();
    zz_view *view = znzin->view;

	float z_depth;
    float region[2];
    float texture_ratio_x,texture_ratio_y;


	z_depth=cam->get_near_plane();
    z_depth*=-1.1f;
	get_viewing_region(z_depth,region);
	
	if(!wide_screen_onoff)
	{
		texture_ratio_x = (float(view->get_width()))/screen_texture_width;
		texture_ratio_y = (float(view->get_height()))/screen_texture_height;
	}
	else
	{

        texture_ratio_x = (float(wide_viewport.width))/screen_texture_width;
		texture_ratio_y = (float(wide_viewport.height))/screen_texture_height;

	}

	int grid_x,grid_y;
	float center_x,center_y;
    float length_x,length_y;
    float position_x,position_y;

	D3DXMATRIX t_m,s_m;

	length_x=texture_ratio_x/10.0f;
	length_y=texture_ratio_y/10.0f;

    num_current_tile=25;
	
	for(i=0;i<num_current_tile;i+=1)
	{
		m_tiles[i] = new TileBehavior;
        
		d3d_device->CreateVertexBuffer(6*sizeof(VERTEX_TRAIL),D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1,
                                      D3DPOOL_MANAGED, &m_tiles[i]->m_pVBTile, NULL );  
                           
		grid_x=i%5;
		grid_y=i/5;
		
        center_x=length_x+grid_x*length_x*2.0f;
		center_y=length_y+grid_y*length_y*2.0f;
   
   				
		VERTEX_TRAIL vec[6]; 
        char *vertes=NULL;
		m_tiles[i]->m_pVBTile->Lock(0,0,(void**)&vertes,D3DLOCK_DISCARD);

	    vec[0].position.x=-1.0f;vec[0].position.y=1.0f;vec[0].position.z=z_depth;vec[0].uv.x=center_x-length_x;vec[0].uv.y=center_y-length_y;	   
        vec[1].position.x=-1.0f;vec[1].position.y=-1.0f;vec[1].position.z=z_depth;vec[1].uv.x=center_x-length_x;vec[1].uv.y=center_y+length_y;	   
   	    vec[2].position.x=1.0f;vec[2].position.y=-1.0f;vec[2].position.z=z_depth;vec[2].uv.x=center_x+length_x;vec[2].uv.y=center_y+length_y;	   
          
	    vec[3].position.x=-1.0f;vec[3].position.y=1.0f;vec[3].position.z=z_depth;vec[3].uv.x=center_x-length_x;vec[3].uv.y=center_y-length_y;	   
        vec[4].position.x=1.0f;vec[4].position.y=-1.0f;vec[4].position.z=z_depth;vec[4].uv.x=center_x+length_x;vec[4].uv.y=center_y+length_y;	   
   	    vec[5].position.x=1.0f;vec[5].position.y=1.0f;vec[5].position.z=z_depth;vec[5].uv.x=center_x+length_x;vec[5].uv.y=center_y-length_y;	   
        
		
		
		
		memcpy(vertes, vec, 6*sizeof(VERTEX_TRAIL));
		m_tiles[i]->m_pVBTile->Unlock();
	    
	    position_x=region[0]*(-0.8f+grid_x*0.4f);
		position_y=region[1]*(0.8f-grid_y*0.4f);
		
		D3DXMatrixTranslation(&t_m,position_x,position_y,0.0f);
		D3DXMatrixScaling(&s_m,0.2f*region[0],0.2f*region[1],1.0f);
	 
	    m_tiles[i]->modelbase=s_m*t_m;
	}
    
	
}



void zz_screen_sfx::make_tiles2()
{

    int i;
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    LPDIRECT3DDEVICE9 d3d_device;
	d3d_device = r->get_device();
    zz_camera *cam = znzin->get_camera();
    zz_view *view = znzin->view;

	float z_depth;
    float region[2];
    float texture_ratio_x,texture_ratio_y;


	z_depth=cam->get_near_plane();
    z_depth*=-1.1f;
	get_viewing_region(z_depth,region);
	
	if(!wide_screen_onoff)
	{
		texture_ratio_x = (float(view->get_width()))/screen_texture_width;
		texture_ratio_y = (float(view->get_height()))/screen_texture_height;
	}
	else
	{

        texture_ratio_x = (float(wide_viewport.width))/screen_texture_width;
		texture_ratio_y = (float(wide_viewport.height))/screen_texture_height;

	}
		
	float center_x,center_y;
    float length_x,length_y;
    float position_x,position_y;

	D3DXMATRIX t_m,s_m;

	length_x=texture_ratio_x/4.0f;
	length_y=texture_ratio_y/2.0f;

    num_current_tile=2;
	
	for(i=0;i<num_current_tile;i+=1)
	{
		m_tiles[i] = new TileBehavior;
        
		d3d_device->CreateVertexBuffer(6*sizeof(VERTEX_TRAIL),D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1,
                                      D3DPOOL_MANAGED, &m_tiles[i]->m_pVBTile, NULL );  
                           
		center_x=length_x+i*length_x*2.0f;
		center_y=length_y;
   
   				
		VERTEX_TRAIL vec[6]; 
        char *vertes=NULL;
		m_tiles[i]->m_pVBTile->Lock(0,0,(void**)&vertes,D3DLOCK_DISCARD);

	    vec[0].position.x=-1.0f;vec[0].position.y=1.0f;vec[0].position.z=z_depth;vec[0].uv.x=center_x-length_x;vec[0].uv.y=center_y-length_y;	   
        vec[1].position.x=-1.0f;vec[1].position.y=-1.0f;vec[1].position.z=z_depth;vec[1].uv.x=center_x-length_x;vec[1].uv.y=center_y+length_y;	   
   	    vec[2].position.x=1.0f;vec[2].position.y=-1.0f;vec[2].position.z=z_depth;vec[2].uv.x=center_x+length_x;vec[2].uv.y=center_y+length_y;	   
        vec[3].position.x=-1.0f;vec[3].position.y=1.0f;vec[3].position.z=z_depth;vec[3].uv.x=center_x-length_x;vec[3].uv.y=center_y-length_y;	   
        vec[4].position.x=1.0f;vec[4].position.y=-1.0f;vec[4].position.z=z_depth;vec[4].uv.x=center_x+length_x;vec[4].uv.y=center_y+length_y;	   
   	    vec[5].position.x=1.0f;vec[5].position.y=1.0f;vec[5].position.z=z_depth;vec[5].uv.x=center_x+length_x;vec[5].uv.y=center_y-length_y;	   
        
		
		memcpy(vertes, vec, 6*sizeof(VERTEX_TRAIL));
		m_tiles[i]->m_pVBTile->Unlock();
	    
	    position_x=region[0]*(-0.5f+i);
		position_y=0.0f;
		
		D3DXMatrixTranslation(&t_m,position_x,position_y,0.0f);
		D3DXMatrixScaling(&s_m,region[0]*0.5f,region[1],1.0f);
	 
	    m_tiles[i]->modelbase=s_m*t_m;
	}
  
   
}


void zz_screen_sfx::make_tiles3()
{

	int i;
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9 d3d_device;
	d3d_device = r->get_device();
	zz_camera *cam = znzin->get_camera();
	zz_view *view = znzin->view;

	float z_depth;
	float region[2];
	float texture_ratio_x,texture_ratio_y;


	z_depth=cam->get_near_plane();
	z_depth*=-1.1f;
	get_viewing_region(z_depth,region);

	if(!wide_screen_onoff)
	{
		texture_ratio_x = (float(view->get_width()))/screen_texture_width;
		texture_ratio_y = (float(view->get_height()))/screen_texture_height;
	}
	else
	{

        texture_ratio_x = (float(wide_viewport.width))/screen_texture_width;
		texture_ratio_y = (float(wide_viewport.height))/screen_texture_height;

	}

		
	int grid_x,grid_y;	
	float center_x,center_y;
	float length_x,length_y;
	float position_x,position_y;

	D3DXMATRIX t_m,s_m;

	length_x=texture_ratio_x/4.0f;
	length_y=texture_ratio_y/4.0f;

	num_current_tile=4;

	for(i=0;i<num_current_tile;i+=1)
	{
		m_tiles[i] = new TileBehavior;
	    
		grid_x=i%2;
		grid_y=i/2;
		
		d3d_device->CreateVertexBuffer(6*sizeof(VERTEX_TRAIL),D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1,
										D3DPOOL_MANAGED, &m_tiles[i]->m_pVBTile, NULL );  
	                        
		center_x=length_x+grid_x*length_x*2.0f;
		center_y=length_y+grid_y*length_y*2.0f;

	   			
		VERTEX_TRAIL vec[6]; 
		char *vertes=NULL;
		m_tiles[i]->m_pVBTile->Lock(0,0,(void**)&vertes,D3DLOCK_DISCARD);

		vec[0].position.x=-1.0f;vec[0].position.y=1.0f;vec[0].position.z=z_depth;vec[0].uv.x=center_x-length_x;vec[0].uv.y=center_y-length_y;	   
		vec[1].position.x=-1.0f;vec[1].position.y=-1.0f;vec[1].position.z=z_depth;vec[1].uv.x=center_x-length_x;vec[1].uv.y=center_y+length_y;	   
   		vec[2].position.x=1.0f;vec[2].position.y=-1.0f;vec[2].position.z=z_depth;vec[2].uv.x=center_x+length_x;vec[2].uv.y=center_y+length_y;	   
	  	vec[3].position.x=-1.0f;vec[3].position.y=1.0f;vec[3].position.z=z_depth;vec[3].uv.x=center_x-length_x;vec[3].uv.y=center_y-length_y;	   
		vec[4].position.x=1.0f;vec[4].position.y=-1.0f;vec[4].position.z=z_depth;vec[4].uv.x=center_x+length_x;vec[4].uv.y=center_y+length_y;	   
   		vec[5].position.x=1.0f;vec[5].position.y=1.0f;vec[5].position.z=z_depth;vec[5].uv.x=center_x+length_x;vec[5].uv.y=center_y-length_y;	   
	    
		
		memcpy(vertes, vec, 6*sizeof(VERTEX_TRAIL));
		m_tiles[i]->m_pVBTile->Unlock();
		
		position_x=region[0]*(-0.5f+grid_x);
		position_y=region[1]*(0.5f-grid_y);
		
		D3DXMatrixTranslation(&t_m,position_x,position_y,0.0f);
		D3DXMatrixScaling(&s_m,region[0]*0.5f,region[1]*0.5f,1.0f);
		
		m_tiles[i]->modelbase=s_m*t_m;
	}



}

void zz_screen_sfx::make_tiles4()
{
	HRESULT hr;
	
	struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};

	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9 d3d_device;
	d3d_device = r->get_device();
	zz_camera *cam = znzin->get_camera();
	float region[2];
	float z_depth;

	z_depth=cam->get_near_plane();
	z_depth*=-1.1f;
	get_viewing_region(z_depth,region);

	num_current_tile=1;

	m_tiles[0] = new TileBehavior;
	
	if(FAILED(hr = d3d_device->CreateVertexBuffer(6*sizeof(MYLINEVERTEX),D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE,
		D3DPOOL_MANAGED, &m_tiles[0]->m_pVBTile, NULL ))) 
	{
			return;
	}
		                  
	char *vertes=NULL;
	MYLINEVERTEX vec[6];
	
	if(FAILED(m_tiles[0]->m_pVBTile->Lock(0,0,(void**)&vertes,D3DLOCK_DISCARD) ))
	{
		assert(!"renderer_d3d: vertex_buffer->Lock() failed\n");
		ZZ_LOG("r_d3d: vertex_buffer->Lock() failed\n");
	    return;
	}

	vec[0].pos.x=-region[0];vec[0].pos.y=region[1];vec[0].pos.z=z_depth;vec[0].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	vec[1].pos.x=-region[0];vec[1].pos.y=-region[1];vec[1].pos.z=z_depth;vec[1].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	vec[2].pos.x=region[0];vec[2].pos.y=-region[1];vec[2].pos.z=z_depth;vec[2].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	vec[3].pos.x=-region[0];vec[3].pos.y=region[1];vec[3].pos.z=z_depth;vec[3].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	vec[4].pos.x=region[0];vec[4].pos.y=-region[1];vec[4].pos.z=z_depth;vec[4].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	vec[5].pos.x=region[0];vec[5].pos.y=region[1];vec[5].pos.z=z_depth;vec[5].diffuse=D3DCOLOR_COLORVALUE(1.0f,1.0f,1.0f,1.0f);	   
	    
		
	memcpy(vertes, vec, 6*sizeof(MYLINEVERTEX));
	
	if(FAILED(m_tiles[0]->m_pVBTile->Unlock() ))
	{
		assert(!"renderer_d3d: vertex_buffer->UnLock() failed\n");
		ZZ_LOG("r_d3d: vertex_buffer->UnLock() failed\n");
	    return;
	}
	
		
 
}




void zz_screen_sfx::get_viewing_region(float z_depth,float region[2])
{
    zz_camera * cam = znzin->get_camera();

    if(z_depth>=0)
	*(region)=*(region+1)=1000.0f;
    else
	{
     *(region+1)=-1.0f*z_depth*tanf(cam->get_fov()*0.5f*3.141592f/180.0f);
	 *(region)=*(region+1)*cam->get_aspect_ratio();
	}
  	   
}


void zz_screen_sfx::update_screen()
{


	D3DXMATRIX s_m;

	update_time();

	D3DXMatrixScaling(&s_m,time,time,1.0f);

	for(int i=0;i<num_current_tile;i+=1)
	m_tiles[i]->modelview=s_m*m_tiles[i]->modelbase;


}

void zz_screen_sfx::update_screen2()
{

	float region[2];
    D3DXMATRIX t_m;

    zz_camera *cam = znzin->get_camera();
    get_viewing_region(-1.1f*cam->get_near_plane(),region);
  
    update_time();

   
	for(int i=0;i<num_current_tile;i+=1)
	{
		if(i==0)
			D3DXMatrixTranslation (&t_m,-region[0]*(1.0f-time),0.0f,0.0f);
		else
			D3DXMatrixTranslation (&t_m,region[0]*(1.0f-time),0.0f,0.0f);

		 m_tiles[i]->modelview=m_tiles[i]->modelbase*t_m;


	}
}

void zz_screen_sfx::update_screen3()
{
   
	float region[2];
	D3DXMATRIX t_m;

	zz_camera *cam = znzin->get_camera();
	get_viewing_region(-1.1f*cam->get_near_plane(),region);

	update_time();

	for(int i=0;i<num_current_tile;i+=1)
	{
		
		switch(i)
		{

		case 0:
			D3DXMatrixTranslation (&t_m,-region[0]*(1.0f-time),region[1]*(1.0f-time),0.0f);
			break;

		case 1:
			D3DXMatrixTranslation (&t_m,region[0]*(1.0f-time),region[1]*(1.0f-time),0.0f);
			break;

		case 2:
			D3DXMatrixTranslation (&t_m,-region[0]*(1.0f-time),-region[1]*(1.0f-time),0.0f);
			break;

		case 3:
			D3DXMatrixTranslation (&t_m,region[0]*(1.0f-time),-region[1]*(1.0f-time),0.0f);
			break;
		}
		
		m_tiles[i]->modelview=m_tiles[i]->modelbase*t_m;
	}
}

void zz_screen_sfx::update_screen4()
{
	struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};

	float region[2];
	float z_depth;
	float color_a;
	int color; 
	D3DXMATRIX t_m;

	zz_camera *cam = znzin->get_camera();
	z_depth = -1.1f*cam->get_near_plane();
	get_viewing_region(z_depth,region);

	update_time();


	if(accumulate_time<fade_t1)
		color_a=accumulate_time/fade_t1;
	else if(accumulate_time<fade_t2)
		color_a=1.0f;
	else if(accumulate_time<max_time)
		color_a=1.0f-(accumulate_time-fade_t2)/(max_time-fade_t2); 
	else
		color_a=0.0f;

	color = D3DCOLOR_COLORVALUE(screen_color[0]/255.0f,screen_color[1]/255.0f,screen_color[2]/255.0f,color_a);


	char *vertes=NULL;
	MYLINEVERTEX vec[6];
	m_tiles[0]->m_pVBTile->Lock(0,0,(void**)&vertes,D3DLOCK_DISCARD);

	vec[0].pos.x=-region[0];vec[0].pos.y=region[1];vec[0].pos.z=z_depth;vec[0].diffuse=color;	   
	vec[1].pos.x=-region[0];vec[1].pos.y=-region[1];vec[1].pos.z=z_depth;vec[1].diffuse=color;	   
	vec[2].pos.x=region[0];vec[2].pos.y=-region[1];vec[2].pos.z=z_depth;vec[2].diffuse=color;	   
	        
	vec[3].pos.x=-region[0];vec[3].pos.y=region[1];vec[3].pos.z=z_depth;vec[3].diffuse=color;	   
	vec[4].pos.x=region[0];vec[4].pos.y=-region[1];vec[4].pos.z=z_depth;vec[4].diffuse=color;	   
	vec[5].pos.x=region[0];vec[5].pos.y=region[1];vec[5].pos.z=z_depth;vec[5].diffuse=color;	   
	    
	memcpy(vertes, vec, 6*sizeof(MYLINEVERTEX));
	m_tiles[0]->m_pVBTile->Unlock();

	D3DXMatrixIdentity(&m_tiles[0]->modelview);
}



void zz_screen_sfx::render()
{
    
  struct MYLINEVERTEX {
		D3DXVECTOR3 pos;
		D3DCOLOR diffuse;
	};
	
	D3DXMATRIX id_m;	
	D3DXMATRIX camera_m;
	D3DXMATRIX model_m;
	D3DXMATRIX projection_m;
	  
	zz_camera*			cam			= znzin->get_camera();
	zz_renderer_d3d*	r			= (zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9	d3d_device	= NULL;
	d3d_device						= r->get_device();
	D3DVIEWPORT9 buffer_port;
    	    
	D3DXMatrixIdentity(&id_m);
	  
	d3d_device->GetTransform(D3DTS_PROJECTION,&projection_m);
	d3d_device->GetTransform(D3DTS_VIEW,&camera_m);
	d3d_device->GetTransform(D3DTS_WORLD,&model_m);
	d3d_device->SetTransform(D3DTS_VIEW,&id_m);
	    
	if(wide_screen_onoff)
	r->set_viewport(wide_viewport); 	
	
	
	r->set_projection_matrix(r->get_projection_matrix());
	r->enable_fog(false);
	r->set_vertex_shader(ZZ_HANDLE_NULL);
	r->set_pixel_shader(ZZ_HANDLE_NULL);
	r->enable_alpha_blend(false, ZZ_BT_NORMAL);
   
    
	if(state_number<4)
	{
		r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE ); 
	}
	else
	{

		//r->set_texture_stage_state(0, ZZ_TSS_ALPHAOP, D3DTOP_DISABLE);
		//r->set_texture_stage_state(0, ZZ_TSS_COLORARG1, D3DTA_DIFFUSE);
		//r->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_SELECTARG1 );
		r->set_texture_stage_state(0, ZZ_TSS_COLOROP, D3DTOP_DISABLE );

		d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		d3d_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
  
	}

	d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	if(state_number<4)
	{
		d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
		d3d_device->SetTexture(0,screen_texture);
	}  
	else
		d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
  
 	for(int i=0;i<num_current_tile;i+=1)
	{
		d3d_device->SetTransform(D3DTS_WORLD,&m_tiles[i]->modelview); 
		if(state_number<4)
			d3d_device->SetStreamSource(0,m_tiles[i]->m_pVBTile,0,sizeof(VERTEX_TRAIL));
		else
			d3d_device->SetStreamSource(0,m_tiles[i]->m_pVBTile,0,sizeof(MYLINEVERTEX));

		d3d_device->DrawPrimitive( D3DPT_TRIANGLELIST, 0, 2);
	}
  
	d3d_device->SetTransform(D3DTS_VIEW,&camera_m);
	d3d_device->SetTransform(D3DTS_WORLD,&model_m);
	d3d_device->SetTransform(D3DTS_PROJECTION,&projection_m);
	d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

	if(state_number>=4)
	{
		d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	}

	d3d_device->SetTexture(0,NULL);
}

void zz_screen_sfx::pre_render()
{
  
	if(play_onoff)
	{
		if(!texture_setup_onoff)
		{
			clear();
			
			if(state_number<4)
			pre_make_texture();
			
			switch(state_number)
			{
				case 1:
				make_tiles1();
				break; 
				
				case 2:
				make_tiles2();
				break; 
			    
				case 3:
				make_tiles3();
				break; 

				case 4:
//				make_tiles4();
				break;
				
				default:
				make_tiles1();
				break;

			}
			
			device_reset_onoff=FALSE;
			texture_setup_onoff=TRUE;
		}
			
	}
 
}

void zz_screen_sfx::post_render()
{

	if(play_onoff)
	{
		if(texture_setup_onoff)
		{
			if(!device_reset_onoff)
			{
				if(state_number<4)
				post_make_texture();
				device_reset_onoff=TRUE;
			} 
		}
			
		switch(state_number)
		{
			case 1:
				update_screen(); 
				break;
				
			case 2:
				update_screen2(); 
				break;
			
			case 3:
				update_screen3(); 
				break;
			
			case 4:
				make_tiles4();
				update_screen4(); 
				break;

			default:
				update_screen(); 
				break;
		}

		render();

		if(state_number == 4)
		{
			delete m_tiles[0];
			m_tiles[0] = NULL;
		}

		if(!play_onoff)
			clear();

	}
  
 
}

void zz_screen_sfx::start_screen_sfx(int state,float fade_t)
{

	if(!play_onoff)
	{
		play_onoff=TRUE;
		time = 1.0f;
		state_number=state;
		accumulate_time=0.0f;
		max_time=fade_t;
	}
}

void zz_screen_sfx::start_fade_inout(float fade_in_t,float fade_m_t,float fade_out_t,int color_r,int color_g,int color_b)
{

	if(!play_onoff)
	{
		play_onoff=TRUE;
		time = 1.0f;
		accumulate_time=0.0f;
		state_number=4;

		fade_t1=fade_in_t;
		fade_t2=fade_t1+fade_m_t;
		max_time=fade_t2+fade_out_t;
		screen_color[0]=color_r;
		screen_color[1]=color_g;
		screen_color[2]=color_b;

	}
}

void zz_screen_sfx::stop()
{

	if(play_onoff)
	{
      accumulate_time = max_time+1.0f;
	}
	

}

void zz_screen_sfx::update_time()
{
	accumulate_time+=0.001f*ZZ_TIME_TO_MSEC(znzin->get_diff_time());
	
	if(max_time>accumulate_time)
		time=1.0f-accumulate_time/max_time;
	else
	{
		time = 0.0f;
		play_onoff = FALSE;
	}
}


void zz_screen_sfx::pre_clear_wide()
{
	if(wide_screen_onoff)
	{
        zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
		r->set_viewport(viewport);
    }
}

void zz_screen_sfx::post_clear_wide()
{
    if(wide_screen_onoff)
	{
        zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
		r->set_viewport(wide_viewport);
    }

}

void zz_screen_sfx::play_widescreen_mode(float ratio)
{
	wide_screen_ratio = ratio;
    wide_screen_onoff = true;

    zz_camera * cam = znzin->get_camera();
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	r->get_viewport(viewport);
    wide_viewport.height = (dword)(viewport.width*wide_screen_ratio);
	wide_viewport.width = viewport.width;
	wide_viewport.x = viewport.x;
	wide_viewport.y = (viewport.height-wide_viewport.height)/2;
	wide_viewport.maxz = viewport.maxz;
	wide_viewport.minz = viewport.minz;
    r->set_viewport(wide_viewport);
    screen_ratio=cam->get_aspect_ratio();
	cam->set_aspect_ratio(1.0f/wide_screen_ratio);
	cam->set_perspective(cam->get_fov(),cam->get_aspect_ratio(),cam->get_near_plane(),cam->get_far_plane());
   
}

void zz_screen_sfx::play_widescreen_mode(int x,int y, int width,int height)
{
	wide_screen_ratio = ((float)height)/width;
    wide_screen_onoff = true;

    zz_camera * cam = znzin->get_camera();
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	r->get_viewport(viewport);
    wide_viewport.height = height;
	wide_viewport.width = width;
	wide_viewport.x = x;
	wide_viewport.y = y;
	wide_viewport.maxz = viewport.maxz;
	wide_viewport.minz = viewport.minz;
    r->set_viewport(wide_viewport);
    screen_ratio=cam->get_aspect_ratio();
	cam->set_aspect_ratio(1.0f/wide_screen_ratio);
	cam->set_perspective(cam->get_fov(),cam->get_aspect_ratio(),cam->get_near_plane(),cam->get_far_plane());
   
}

void zz_screen_sfx::stop_widescreen_mode()
{
	zz_camera * cam = znzin->get_camera();
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	wide_screen_onoff = false;	
	r->set_viewport(viewport);
    cam->set_aspect_ratio(screen_ratio);
	cam->set_perspective(cam->get_fov(),cam->get_aspect_ratio(),cam->get_near_plane(),cam->get_far_plane());
 
}

bool zz_screen_sfx::get_widescreen_mode()
{
	return wide_screen_onoff;
}

int zz_screen_sfx::get_widescreen_startx()
{
	return wide_viewport.x;
}

int zz_screen_sfx::get_widescreen_starty()
{
	return wide_viewport.y;
}

int zz_screen_sfx::get_widescreen_width()
{
	return wide_viewport.width;
}

int zz_screen_sfx::get_widescreen_height()
{
	return wide_viewport.height;
}

void zz_screen_sfx::get_widescreen_viewport(zz_viewport& screen_viewport)
{
	screen_viewport = wide_viewport; 
}


#define angle_ratio 3.141592f/180.0f


zz_camera_sfx::zz_camera_sfx()
{
	init();
}

zz_camera_sfx::~zz_camera_sfx()
{
}

void zz_camera_sfx::init()
{

	camera = NULL;
	seta = 0.0f;
	phi = 270.0f;
	length = 100.0f;
	steal_onoff = FALSE;
    base_angle_speed = 0.0f;
	speed = 0.0f;
    side_speed = 0.0f;
    height_speed = 0.0f;

}

void zz_camera_sfx::Init_observer_Camera(float xPos, float yPos, float zPos)
{
	mat4 inv_m;
	vec3 base_dir;
	float flength;
	
	camera = znzin->get_camera();
	camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW,camera_m);
	
	inv_m = camera_m.inverse();

	base_dir.x = -inv_m._13; base_dir.y = -inv_m._23; base_dir.z = 0.0f;
	flength = sqrtf(base_dir.x*base_dir.x + base_dir.y*base_dir.y);
	base_dir.x /= flength;
	base_dir.y /= flength;

	camera_base_m._11 = base_dir.y;
	camera_base_m._21 = -base_dir.x;
	camera_base_m._31 = 0.0f;
	camera_base_m._41 = 0.0f;

	camera_base_m._12 = base_dir.x;
	camera_base_m._22 = base_dir.y;
	camera_base_m._32 = 0.0f;
	camera_base_m._42 = 0.0f;

	camera_base_m._13 = 0.0f;
	camera_base_m._23 = 0.0f;
	camera_base_m._33 = 1.0f;
	camera_base_m._43 = 0.0f;

	camera_base_m._14 = xPos;
	camera_base_m._24 = yPos;
    camera_base_m._34 = zPos;
	camera_base_m._44 = 1.0f;

	length = inv_m._34 - zPos;
    seta = 0.0f;
	phi = 0.0f;

	speed = 0.0f;
    speed_step = 0.0f;

	side_speed = 0.0f;
    side_speed_step = 0.0f;

	height_speed = 0.0f;
	height_speed_step = 0.0f;
	
	base_angle_speed = 0.0f;
	base_angle_speed_step = 0.0f;

    view_angle_speed = 0;
    view_angle_speed_step = 0; 
}

void zz_camera_sfx::steal_camera()
{
   if(steal_onoff)
   {
	   if(steal_success)	 	   	
	   {
		   zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
		   mat4 projection_matrix;

		   camera->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,camera_sfx_m);
	       far_plane = camera->get_far_plane();	       
		   camera->set_perspective(camera->get_fov(), camera->get_aspect_ratio(), camera->get_near_plane(), 50000.0f);
	       camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_matrix);
		   r->set_projection_matrix(projection_matrix);
	   }  
   }
}


void zz_camera_sfx::calculate_steal_camera()
{	
	steal_success = false;
	if(steal_onoff)
	{
		vec3 t;

		camera = znzin->get_camera();
		frustum = camera->get_frustum();

		camera->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW,camera_m);
		t=camera->get_eye();

//		get_camera_sfx_matrix(t);
		get_camera_observer_sfx_matrix();
	    
		steal_success = true;
	}

}

const mat4& zz_camera_sfx::get_steal_camera()
{
  
	vec3 t;

	camera = znzin->get_camera();
	frustum = camera->get_frustum();
	t=camera->get_eye();

//	get_camera_sfx_matrix(t);
    calculate_steal_camera();

	return camera_sfx_m;
}


void zz_camera_sfx::return_camera()
{

	if(steal_onoff)
	{
		zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
		camera = znzin->get_camera();
	 	camera->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,camera_m);
	    
		mat4 projection_matrix;
		camera->set_perspective(camera->get_fov(), camera->get_aspect_ratio(), camera->get_near_plane(), far_plane);
	    camera->get_transform(zz_camera::ZZ_MATRIX_PROJECTION, projection_matrix);
		r->set_projection_matrix(projection_matrix);

	}

}

void zz_camera_sfx::get_camera_sfx_matrix(vec3 &t)
{
	mat4 m1,m2;
	vec3 v; 

	m1._11=-sinf(phi*angle_ratio);m1._21=-cosf(seta*angle_ratio)*cosf(phi*angle_ratio);m1._31= sinf(seta*angle_ratio)*cosf(phi*angle_ratio);m1._41=0;
	m1._12= cosf(phi*angle_ratio);m1._22=-cosf(seta*angle_ratio)*sinf(phi*angle_ratio);m1._32= sinf(seta*angle_ratio)*sinf(phi*angle_ratio);m1._42=0;
	m1._13= 0;m1._23=sinf(seta*angle_ratio);m1._33=cosf(seta*angle_ratio);m1._43=0;
	m1._14=0;m1._24=0;m1._34=-length;m1._44=1;

	v=-1.0f*t;  

	m2.set_identity();
	m2.set_translation(v);

	mult(camera_sfx_m,m1,m2);
}

void zz_camera_sfx::get_camera_observer_sfx_matrix()
{
	float delta_time;
	float height_step;

	delta_time = 0.001f*ZZ_TIME_TO_MSEC(znzin->get_diff_time());
    
	height_step = height_speed * delta_time;
    
	if((length + height_step) < 0.0f)
	{
		length = 0.0f;
	}
	else
	{
		length += height_step;
	}

	mat4 m1, m2, m3, m4;
	
	m1._11 = camera_base_m._11;
	m1._21 = camera_base_m._21;
	m1._31 = camera_base_m._31;
	m1._41 = 0.0f;

	m1._12 = camera_base_m._13;
	m1._22 = camera_base_m._23;
	m1._32 = camera_base_m._33;
	m1._42 = 0.0f;

	m1._13 = -camera_base_m._12;
	m1._23 = -camera_base_m._22;
	m1._33 = -camera_base_m._32;
	m1._43 = 0.0f;

	m1._14 = camera_base_m._14;
	m1._24 = camera_base_m._24;
	m1._34 = camera_base_m._34 + length;
	m1._44 = 1.0f;

    
	float fangle_step;
	fangle_step = view_angle_speed*delta_time;
	
	if((phi + fangle_step) > 3.141592f*0.9f)
	{
		phi = 3.141592f*0.9f;
	}
	else if((phi + fangle_step) < -3.141592f*0.9f)
	{
		phi = -3.141592f*0.9f;
	}
	else
	{
		phi += fangle_step;
	}

	
	m2._11 = 1.0f; m2._21 = 0.0f; m2._31 = 0.0f; m2._41 = 0.0f;
	m2._12 = 0.0f; m2._22 = cosf(phi); m2._32 = sinf(phi); m2._42 = 0.0f;
	m2._13 = 0.0f; m2._23 = -sinf(phi); m2._33 = cosf(phi); m2._43 = 0.0f;
	m2._14 = 0.0f; m2._24 = 0.0f; m2._34 = 0.0f; m2._44 = 1.0f;

	m4 = m1 * m2;
	camera_sfx_m = m4.inverse();

}

void zz_camera_sfx::update_time(float *xPos, float *yPos)
{
//------------------------------------------------------------------
	if(speed > 0)
	{
		if((speed - fabsf(speed_step*0.7f)) > 0.0f)
		{
			speed -= fabsf(speed_step*0.7f);
		}
		else
		{
			speed = 0.0f;

		}

	}
	else if(speed < 0)
	{
		if((speed + fabsf(speed_step*0.7f)) < 0.0f)
		{
			speed += fabsf(speed_step*0.7f);
		}
		else
		{
			speed = 0.0f;

		}
	}

	if(speed > fabsf(speed_step)*60.0f)
		speed = fabsf(speed_step)*60.0f;

	if(speed < -fabsf(speed_step)*60.0f)
		speed = -fabsf(speed_step)*60.0f;


	if(side_speed > 0)
	{
		if((side_speed - fabsf(side_speed_step*0.7f)) > 0.0f)
		{
			side_speed -= fabsf(side_speed_step*0.7f);
		}
		else
		{
			side_speed = 0.0f;

		}

	}
	else if(side_speed < 0)
	{
		if((side_speed + fabsf(side_speed_step*0.7f)) < 0.0f)
		{
			side_speed += fabsf(side_speed_step*0.7f);
		}
		else
		{
			side_speed = 0.0f;

		}
	}

	
	if(height_speed > 0)
	{
		if((height_speed - fabsf(height_speed_step*0.7f)) > 0.0f)
		{
			height_speed -= fabsf(height_speed_step*0.7f);
		}
		else
		{
			height_speed = 0.0f;

		}

	}
	else if(height_speed < 0)
	{
		if((height_speed + fabsf(height_speed_step*0.7f)) < 0.0f)
		{
			height_speed += fabsf(height_speed_step*0.7f);
		}
		else
		{
			height_speed = 0.0f;

		}
	}

	if(height_speed > fabsf(height_speed_step)*60.0f)
		height_speed = fabsf(height_speed_step)*60.0f;

	if(height_speed < -fabsf(height_speed_step)*60.0f)
		height_speed = -fabsf(height_speed_step)*60.0f;
	
//-----------------------------------------------------------------
	

	if(base_angle_speed > 0)
	{
		if((base_angle_speed - fabsf(base_angle_speed)*0.2f) > 0.0f)
		{
			base_angle_speed -= fabsf(base_angle_speed)*0.2f;
		}
		else
		{
			base_angle_speed = 0.0f;

		}

	}
	else if(base_angle_speed < 0)
	{
		if((base_angle_speed + fabsf(base_angle_speed)*0.2f) < 0.0f)
		{
			base_angle_speed += fabsf(base_angle_speed)*0.2f;
		}
		else
		{
			base_angle_speed = 0.0f;

		}
	}

	if(base_angle_speed > fabsf(base_angle_speed)*60.0f)
		base_angle_speed = fabsf(base_angle_speed)*60.0f;

	if(base_angle_speed < -fabsf(base_angle_speed)*60.0f)
		base_angle_speed = -fabsf(base_angle_speed)*60.0f;

//----------------------------------------------------------------------	
	
	if(view_angle_speed > 0)
	{
		if((view_angle_speed - fabsf(view_angle_speed)*0.1f) > 0.0f)
		{
			view_angle_speed -= fabsf(view_angle_speed)*0.1f;
		}
		else
		{
			view_angle_speed = 0.0f;

		}

	}
	else if(view_angle_speed < 0)
	{
		if((view_angle_speed + fabsf(view_angle_speed)*0.1f) < 0.0f)
		{
			view_angle_speed += fabsf(view_angle_speed)*0.1f;
		}
		else
		{
			view_angle_speed = 0.0f;

		}
	}

	if(view_angle_speed > fabsf(view_angle_speed)*60.0f)
		view_angle_speed = fabsf(view_angle_speed)*60.0f;

	if(view_angle_speed < -fabsf(view_angle_speed)*60.0f)
		view_angle_speed = -fabsf(view_angle_speed)*60.0f;
//---------------------------------------------------------------------
    
	float delta_time;
	delta_time = 0.001f*ZZ_TIME_TO_MSEC(znzin->get_diff_time());
	
	mat4 m1, m2;
    float base_angle = base_angle_speed*delta_time;

	m1 = camera_base_m;
	
	m2._11 = cosf(base_angle); m2._21 = sinf(base_angle); m2._31 = 0.0f; m2._41 = 0.0f;
	m2._12 = -sinf(base_angle); m2._22 = cosf(base_angle); m2._32 = 0.0f; m2._42 = 0.0f;
	m2._13 = 0.0f; m2._23 = 0.0f; m2._33 = 1.0f; m2._43 = 0.0f;
	m2._14 = 0.0f; m2._24 = 0.0f; m2._34 = 0.0f; m2._44 = 1.0f;
    
	camera_base_m = m1 *  m2;

	
	camera_base_m._14 += speed*delta_time* camera_base_m._12;
	camera_base_m._24 += speed*delta_time* camera_base_m._22;

	camera_base_m._14 += side_speed*delta_time* camera_base_m._11;
	camera_base_m._24 += side_speed*delta_time* camera_base_m._21;


	*xPos = camera_base_m._14;
	*yPos = camera_base_m._24;
}

void zz_camera_sfx::set_observer_zpos(float z)
{
	camera_base_m._34 = z;
}

void zz_camera_sfx::update_angle(int mouse_xx,int mouse_yy)
{
    if(steal_onoff)
	{
		if(mouse_xx*mouse_xx>mouse_yy*mouse_yy)
		phi-=0.5f*mouse_xx;
		else
		seta-=0.5f*mouse_yy;
	}
}

void zz_camera_sfx::update_speed(float fspeed_step)
{
	speed += fspeed_step;
	speed_step = fspeed_step;
}

void zz_camera_sfx::update_side_speed(float fspeed_step)
{
	side_speed += fspeed_step;
	side_speed_step = fspeed_step;
}

void zz_camera_sfx::update_height_speed(float fspeed_step)
{
	height_speed += fspeed_step;
	height_speed_step = fspeed_step;
}



void zz_camera_sfx::update_base_angle(float angle_step)
{
	base_angle_speed += angle_step;
	base_angle_speed_step = angle_step;
}

void zz_camera_sfx::update_view_angle(float angle_step)
{
	view_angle_speed += angle_step;
	view_angle_speed_step = angle_step;
}

void zz_camera_sfx::update_length(int delta)
{
	if(steal_onoff)
		length-=delta*0.01f;
}

void zz_camera_sfx::play_onoff()
{
    steal_onoff=!steal_onoff;  
}

bool zz_camera_sfx::get_play_onoff()
{
	return steal_onoff;
}

bool zz_camera_sfx::get_success()
{
	return steal_success;
}
void zz_camera_sfx::draw_camera()
{

	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	LPDIRECT3DDEVICE9 d3d_device;
	d3d_device = r->get_device();
	D3DXMATRIX mem_m1,mem_m2;
	mat4 model_m;

	model_m=znzin->camera_sfx.camera_m.inverse();

	d3d_device->GetTransform(D3DTS_WORLD,&mem_m1);
	d3d_device->GetTransform(D3DTS_VIEW,&mem_m2);

	d3d_device->SetTransform(D3DTS_VIEW, (const D3DXMATRIX *)&znzin->camera_sfx.camera_sfx_m);
	d3d_device->SetTransform(D3DTS_WORLD,(const D3DXMATRIX *)&model_m);
    
	r->draw_camera();
	
	d3d_device->SetTransform(D3DTS_WORLD,&mem_m1); 
	d3d_device->SetTransform(D3DTS_VIEW,&mem_m2); 


}

void zz_camera_sfx::get_viewing_region(float z_depth,float region[2])
{
    zz_camera * cam = znzin->get_camera();

    if(z_depth>=0)
		*(region)=*(region+1)=1000.0f;
    else
	{
    *(region+1)=-1.0f*z_depth*tanf(cam->get_fov()*0.5f*3.141592f/180.0f);
	*(region)=*(region+1)*cam->get_aspect_ratio();
	}
  	   
}

zz_sprite_sfx::zz_sprite_sfx()
{
	play_onoff = false;
    pause_onoff = false;
} 
zz_sprite_sfx::~zz_sprite_sfx()
{


}

void zz_sprite_sfx::input_sprite_sfx_element(zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color, float ft1, float ft2, float max_t)
{
    
	if( play_onoff == false)
	{
		sfx_tex = tex;
		sfx_rect = *src_rect;
		sfx_center = *center;
		sfx_color = color;
		sfx_position = *position;
		play_onoff = true;
		accumulate_time = 0.0f;
	    fade_t1 = ft1;
	    fade_t2 = ft2;
		max_time = max_t;
	    pause_onoff = false;
	}

}

void zz_sprite_sfx::update_time()
{
	if(play_onoff && !pause_onoff)
	{
		accumulate_time+=0.001f*ZZ_TIME_TO_MSEC(znzin->get_diff_time());
		
		if(max_time<accumulate_time)
		{
			accumulate_time = 0.0f;
			play_onoff = FALSE;
		}
	}
}


void zz_sprite_sfx::draw_sprite()
{
	if (!znzin) return ;

	if(play_onoff && !pause_onoff)
	{
		zz_color color;
		
		color = sfx_color;
				
		if(accumulate_time < fade_t1)
		{
			color.a = color.a * (accumulate_time /fade_t1);
		}
		else if( accumulate_time < fade_t2)
		{

		}
		else
		{
			color.a = color.a * (1.0f - (accumulate_time - fade_t2)/(max_time - fade_t2));
		}
				
		zz_renderer_d3d * r = static_cast<zz_renderer_d3d*>(znzin->renderer);
		r->draw_sprite_ex(sfx_tex, &sfx_rect, &sfx_center, &sfx_position,color);
	}
}

void zz_sprite_sfx::pre_render()
{
	update_time();
}

void zz_sprite_sfx::post_render()
{
   if(play_onoff)
   {
	   zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;   
	   LPDIRECT3DDEVICE9 d3d_device;
	   d3d_device = r->get_device();

/*     r->set_vertex_shader(ZZ_HANDLE_NULL);
	   r->set_pixel_shader(ZZ_HANDLE_NULL);
	   r->enable_alpha_blend(TRUE, ZZ_BT_NORMAL);

	   r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE ); 
	   r->set_texture_stage_state( 0, 
	   d3d_device->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	   d3d_device->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	   d3d_device->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
*/
       r->init_sprite_state();
       
	   draw_sprite();
   }

}

void zz_sprite_sfx::stop_sprite_sfx()
{
  if(play_onoff)
  {
	  play_onoff = false;
	  accumulate_time = 0.0f;
  }

}

bool zz_sprite_sfx::get_play_onoff()
{
	return play_onoff;
}

bool zz_sprite_sfx::get_pause_onoff()
{
	return pause_onoff;
}

void zz_sprite_sfx::pause_on()
{
	if(play_onoff)
		pause_onoff = true;
}

void zz_sprite_sfx::pause_off()
{
	if(play_onoff)
		pause_onoff = false;
}

zz_avatar_selection_sfx::zz_avatar_selection_sfx()
{
	view_length = 2.15f;
	view_seta = 3.141592f * 1.5f;
	view_height = -0.10f;
    m_texBackGround = NULL;
}

zz_avatar_selection_sfx::~zz_avatar_selection_sfx()
{
/*	if (m_texBackGround) {
		znzin->textures->kill(m_texBackGround);
	}
	m_texBackGround = NULL;
*/
}

void zz_avatar_selection_sfx::change_avatar_viewport()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    r->get_viewport(viewport);
	r->set_viewport(avatar_selection_viewport);
}

void zz_avatar_selection_sfx::set_avatar_selection_viewport(float x, float y, float width, float height)
{
	avatar_selection_viewport.maxz = 1.0f;
	avatar_selection_viewport.minz = 0.0f;
	
	avatar_selection_viewport.x = x;
	avatar_selection_viewport.y = y;
	avatar_selection_viewport.width = width;
	avatar_selection_viewport.height = height;
}

void zz_avatar_selection_sfx::clear_scene()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;	
	
	r->clear_zbuffer();
}

void zz_avatar_selection_sfx::change_default_viewport()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    r->set_viewport(viewport);
}

void zz_avatar_selection_sfx::change_avatar_graphicpipeline()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	zz_camera * cam = znzin->get_camera();
	
	LPDIRECT3DDEVICE9 d3d_device;
    d3d_device = r->get_device();

	d3d_device->GetTransform(D3DTS_PROJECTION, &projection_m);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW,view_m);
	
	buffer_m = r->get_modelview_matrix();
    buffer_m2 = r->get_projection_matrix();

//	d3d_device->SetTransform(D3DTS_PROJECTION, &avatar_projection_m);
//	d3d_device->SetTransform(D3DTS_VIEW, &avatar_view_m);

	r->set_projection_matrix((mat4)avatar_projection_m);
	r->set_modelview_matrix((mat4)avatar_view_m);
//	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,(mat4)avatar_view_m);

	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,(mat4)avatar_view_m);
		

	D3DXMATRIX mat;
	D3DXMatrixIdentity(&mat);
	r->set_world_matrix((mat4)mat);
	
}

void zz_avatar_selection_sfx::change_default_graphicpipeline()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	zz_camera * cam = znzin->get_camera();
	LPDIRECT3DDEVICE9 d3d_device;
    d3d_device = r->get_device();
	
	
	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,view_m);
	r->set_projection_matrix((mat4)buffer_m2);
	r->set_modelview_matrix(buffer_m);

//    d3d_device->SetTransform(D3DTS_PROJECTION, &projection_m);

}

void zz_avatar_selection_sfx::calculate_avatar_view_matrix()
{
/*	inv_avatar_view_m._11 = -sinf(view_phi);inv_avatar_view_m._12 = cosf(view_phi);inv_avatar_view_m._13 = 0;inv_avatar_view_m._14 = 0;
	inv_avatar_view_m._21 = -cosf(view_seta)*cosf(view_phi);inv_avatar_view_m._22 = -cosf(view_seta)*sinf(view_phi);inv_avatar_view_m._23 = sinf(view_seta);inv_avatar_view_m._24 = 0;
	inv_avatar_view_m._31 = sinf(view_seta)*cosf(view_phi);inv_avatar_view_m._32 = sinf(view_seta)*sinf(view_phi);inv_avatar_view_m._33 = cosf(view_seta);inv_avatar_view_m._34 = 0;
	inv_avatar_view_m._41 = view_length*sinf(view_seta)*cosf(view_phi);inv_avatar_view_m._42 = view_length*sinf(view_seta)*sinf(view_phi);inv_avatar_view_m._43 = view_length*cosf(view_seta);inv_avatar_view_m._44 = 1;
*/
	inv_avatar_view_m._11 = -sinf(view_seta);inv_avatar_view_m._12 = cosf(view_seta);inv_avatar_view_m._13 = 0;inv_avatar_view_m._14 = 0;
	inv_avatar_view_m._21 = 0.0f;inv_avatar_view_m._22 = 0.0f;inv_avatar_view_m._23 = 1.0f;inv_avatar_view_m._24 = 0;
	inv_avatar_view_m._31 = cosf(view_seta);inv_avatar_view_m._32 = sinf(view_seta);inv_avatar_view_m._33 = 0.0f;inv_avatar_view_m._34 = 0;
	inv_avatar_view_m._41 = view_length*cosf(view_seta);inv_avatar_view_m._42 = view_length*sinf(view_seta);inv_avatar_view_m._43 = view_height;inv_avatar_view_m._44 = 1;
	D3DXMatrixInverse(&avatar_view_m, NULL, &inv_avatar_view_m);
}

void zz_avatar_selection_sfx::calculate_avatar_projection_matrix()
{
	zz_camera * cam = znzin->get_camera();
	view_ratio = avatar_selection_viewport.width / ((float)avatar_selection_viewport.height);
	view_fov = 1.0f;//cam->get_fov();
	view_near = 0.2f;
	view_far = 100.0f;
	
//	D3DXMatrixPerspectiveFovRH( &avatar_projection_m, view_fov, view_ratio, view_near, view_far);
    D3DXMatrixOrthoRH( &avatar_projection_m, view_length, (view_length/view_ratio), view_near, view_far);

}

void zz_avatar_selection_sfx::set_texture(const char *filename)
{
	//ZZ_SAFE_RELEASE(m_texParticle);
	if (m_texBackGround){
		znzin->textures->kill(m_texBackGround);
	}
		
	m_texBackGround = (zz_texture*)znzin->textures->find(filename);

	if (!m_texBackGround) {
		m_texBackGround = (zz_texture*)znzin->textures->spawn(filename, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
		assert(m_texBackGround);
		m_texBackGround->set_path(filename);
	}
	
	assert(m_texBackGround);
	// add reference count
	m_texBackGround->addref();
	//znzin->textures->load(m_texParticle);
	znzin->textures->flush_entrance(m_texBackGround);
}

void zz_avatar_selection_sfx::draw_background()
{
	VERTEX_TRAIL vec[6];
    D3DXMATRIX buffer_mat;
    D3DXMATRIX I_mat;
    float max_length[2];
    const float z_length = 1.1f * view_near; 
	 
	LPDIRECT3DDEVICE9 d3d_device;
    zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	d3d_device = r->get_device();
    zz_camera * cam = znzin->get_camera();

    d3d_device->GetTransform(D3DTS_VIEW, &buffer_mat);
    
	D3DXMatrixIdentity(&I_mat);
    d3d_device->SetTransform(D3DTS_VIEW, &I_mat);

	max_length[1] = z_length * tanf(view_fov * 0.5f);
	max_length[0] = max_length[1] * view_ratio;

	vec[0].position.x = -max_length[0]; vec[0].position.y = max_length[1]; vec[0].position.z = -z_length; vec[0].uv.x = 0.0f; vec[0].uv.y = 0.0f;
    vec[1].position.x = -max_length[0]; vec[1].position.y = -max_length[1]; vec[1].position.z = -z_length; vec[1].uv.x = 0.0f; vec[1].uv.y = 1.0f;
	vec[2].position.x = max_length[0]; vec[2].position.y = -max_length[1]; vec[2].position.z = -z_length; vec[2].uv.x = 1.0f; vec[2].uv.y = 1.0f;
	
	vec[3].position.x = -max_length[0]; vec[3].position.y = max_length[1]; vec[3].position.z = -z_length; vec[3].uv.x = 0.0f; vec[3].uv.y = 0.0f;
	vec[4].position.x = max_length[0]; vec[4].position.y = -max_length[1]; vec[4].position.z = -z_length; vec[4].uv.x = 1.0f; vec[4].uv.y = 1.0f;
	vec[5].position.x = max_length[0]; vec[5].position.y = max_length[1]; vec[5].position.z = -z_length; vec[5].uv.x = 1.0f; vec[5].uv.y = 0.0f;

    r->enable_fog(false);
	r->set_vertex_shader(ZZ_HANDLE_NULL);
	r->set_pixel_shader(ZZ_HANDLE_NULL);
	r->enable_alpha_blend(false, ZZ_BT_NORMAL);
	r->set_texture_stage_state( 0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE ); 
	d3d_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	
	m_texBackGround->flush_device(false);
	m_texBackGround->set(0);
	
	d3d_device->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
    d3d_device->DrawPrimitiveUP( D3DPT_TRIANGLELIST, 2, vec, sizeof(VERTEX_TRAIL)); 
    d3d_device->SetTransform(D3DTS_VIEW, &buffer_mat);

}



zz_moving_camera_screen_sfx::zz_moving_camera_screen_sfx()
{
	
}

zz_moving_camera_screen_sfx::~zz_moving_camera_screen_sfx()
{


}

void zz_moving_camera_screen_sfx::change_camera_viewport()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    r->get_viewport(viewport);
	r->set_viewport(moving_camera_screen_viewport);
}

void zz_moving_camera_screen_sfx::set_camera_screen_viewport(int x, int y, int width, int height)
{
	moving_camera_screen_viewport.maxz = 1.0f;
	moving_camera_screen_viewport.minz = 0.0f;
	
	moving_camera_screen_viewport.x = x;
	moving_camera_screen_viewport.y = y;
	moving_camera_screen_viewport.width = width;
	moving_camera_screen_viewport.height = height;
}


void zz_moving_camera_screen_sfx::change_default_viewport()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
    r->set_viewport(viewport);
}

void zz_moving_camera_screen_sfx::change_camera_graphicpipeline()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	zz_camera * cam = znzin->get_camera();
	
	LPDIRECT3DDEVICE9 d3d_device;
    d3d_device = r->get_device();

	d3d_device->GetTransform(D3DTS_PROJECTION, &projection_m);
	cam->get_transform(zz_camera::ZZ_MATRIX_MODELVIEW,view_m);
	
	buffer_m = r->get_modelview_matrix();
    buffer_m2 = r->get_projection_matrix();


//	d3d_device->SetTransform(D3DTS_PROJECTION, &avatar_projection_m);
//	d3d_device->SetTransform(D3DTS_VIEW, &avatar_view_m);

	r->set_projection_matrix((mat4)camera_projection_m);
	r->set_modelview_matrix((mat4)camera_view_m);
//	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,(mat4)avatar_view_m);

	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,(mat4)camera_view_m);
}

void zz_moving_camera_screen_sfx::change_default_graphicpipeline()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;
	zz_camera * cam = znzin->get_camera();
	LPDIRECT3DDEVICE9 d3d_device;
    d3d_device = r->get_device();
		
	cam->set_transform(zz_camera::ZZ_MATRIX_MODELVIEW,view_m);
	r->set_projection_matrix((mat4)buffer_m2);
	r->set_modelview_matrix(buffer_m);

   d3d_device->SetTransform(D3DTS_PROJECTION, &projection_m);
}

void zz_moving_camera_screen_sfx::calculate_camera_projection_matrix()
{
	zz_camera * cam = znzin->get_camera();
	view_ratio = moving_camera_screen_viewport.width / ((float)moving_camera_screen_viewport.height);
	view_fov = 1.0f;//cam->get_fov();
	view_near = 0.2f;
	view_far = 100.0f;
	
	D3DXMatrixPerspectiveFovRH( &camera_projection_m, view_fov, view_ratio, view_near, view_far);
}

void zz_moving_camera_screen_sfx::Input_camera_matrix(const vec3 &position, const quat &rotation)
{
	mat4 buffer_m, inv_buffer_m;

	buffer_m.set(position, rotation);
	inv_buffer_m = buffer_m.inverse();
    
	camera_view_m._11 = inv_buffer_m._11;	camera_view_m._12 = inv_buffer_m._21; camera_view_m._13 = inv_buffer_m._31; camera_view_m._14 = inv_buffer_m._41;    
	camera_view_m._21 = inv_buffer_m._12;	camera_view_m._22 = inv_buffer_m._22; camera_view_m._23 = inv_buffer_m._32; camera_view_m._24 = inv_buffer_m._42;    
	camera_view_m._31 = inv_buffer_m._13;	camera_view_m._32 = inv_buffer_m._23; camera_view_m._33 = inv_buffer_m._33; camera_view_m._34 = inv_buffer_m._43;    
	camera_view_m._41 = inv_buffer_m._14;	camera_view_m._42 = inv_buffer_m._24; camera_view_m._43 = inv_buffer_m._34; camera_view_m._44 = inv_buffer_m._44;    

    view_position = position;
	
/*	D3DXMatrixInverse(&inv_camera_view_m, NULL, &camera_view_m);
   
	D3DXVECTOR4 buffer_position;
    float max_x, max_y;

	max_y = 
     


	buffer_position. 

*/



}

void zz_moving_camera_screen_sfx::clear_scene()
{
	zz_renderer_d3d *r =(zz_renderer_d3d*)(znzin->renderer) ;	
	r->clear_screen();
}

