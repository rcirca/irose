/** 
 * @file zz_sfx.h
 * @brief post render special effect class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    19-mar-2004
 *
 * $Header: /engine/include/zz_sfx.h 18    02-01-01 1:13a Choo0219 $
 * $History: zz_sfx.h $
 * 
 * *****************  Version 18  *****************
 * User: Choo0219     Date: 02-01-01   Time: 1:13a
 * Updated in $/engine/include
 * 
 * *****************  Version 17  *****************
 * User: Choo0219     Date: 05-12-16   Time: 8:11p
 * Updated in $/engine/include
 * 
 * *****************  Version 16  *****************
 * User: Choo0219     Date: 05-06-01   Time: 10:40a
 * Updated in $/engine/include
 * 
 * *****************  Version 15  *****************
 * User: Choo0219     Date: 05-06-01   Time: 10:26a
 * Updated in $/engine/include
 * 
 * *****************  Version 14  *****************
 * User: Choo0219     Date: 05-05-29   Time: 5:27p
 * Updated in $/engine/include
 * 
 * *****************  Version 13  *****************
 * User: Choo0219     Date: 05-05-28   Time: 4:31p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Choo0219     Date: 04-11-23   Time: 4:04p
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Choo0219     Date: 04-11-23   Time: 1:41p
 * Updated in $/engine/include
 * 
 * *****************  Version 10  *****************
 * User: Choo0219     Date: 04-11-23   Time: 10:52a
 * Updated in $/engine/include
 * 
 * *****************  Version 9  *****************
 * User: Choo0219     Date: 04-11-16   Time: 4:30p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Choo0219     Date: 04-11-11   Time: 5:22p
 * Updated in $/engine/include
 * 
 * *****************  Version 7  *****************
 * User: Choo0219     Date: 04-11-08   Time: 2:05p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Choo0219     Date: 04-11-07   Time: 11:44p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Choo0219     Date: 04-11-07   Time: 5:43p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Choo0219     Date: 04-11-07   Time: 4:59p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Choo0219     Date: 04-11-06   Time: 8:45p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Choo0219     Date: 04-11-05   Time: 3:03p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-03-19   Time: 5:28p
 * Created in $/engine/include
 * Added special effect.
 */

#ifndef __ZZ_SFX_H__
#define __ZZ_SFX_H__

#include <vector>
#include "d3dx9.h"
#include "zz_algebra.h"
#include "zz_camera.h"
#include "zz_renderer_d3d.h"

class zz_sfx {
public:
	virtual ~zz_sfx() {}
	virtual bool update () = 0;
	virtual bool render () = 0;
};

class zz_manager_sfx {
public:
	enum e_type {
		OCEAN = 0
	};

protected:
	std::vector<zz_sfx*> sequence;

public:
	zz_manager_sfx ();
	virtual ~zz_manager_sfx ();

	bool initialize ();
	bool cleanup ();

	void clear_sfx ();

	// returns true if it was succeeded.
	bool push_sfx (e_type type_in);

	void pop_sfx ();

	// render sfx sequence
	void render_sfx ();
};

class TileBehavior{

public:
 
	LPDIRECT3DVERTEXBUFFER9 m_pVBTile;	
    D3DXMATRIX modelview;	
    D3DXMATRIX modelbase; 

	TileBehavior()
	{
	   m_pVBTile = NULL;  
		
	};
	~TileBehavior()
	{
	  if(m_pVBTile)
      m_pVBTile->Release(); 
		 
	};

};

#define MAX_TILE_NUMBER 100

class zz_screen_sfx{

public:

	zz_screen_sfx ();
    ~zz_screen_sfx ();
    void initialize ();
    void clear();
    void pre_make_texture();
	void post_make_texture();
	UINT GetAdaptiveTextureSize(UINT size);
	void make_tiles1();
	void make_tiles2();
	void make_tiles3();
	void make_tiles4();
	void get_viewing_region(float z_depth,float region[2]);
    void update_screen();
    void update_screen2();
	void update_screen3();
	void update_screen4();
	void render();
	void pre_render();
	void post_render();
	void start_screen_sfx(int state,float fade_t);
	void start_fade_inout(float fade_t1,float fade_t2,float fade_t3,int color_r,int color_g,int color_b);
	void stop();
    void update_time();
	void pre_clear_wide();
	void post_clear_wide();
    void play_widescreen_mode(float ratio);
	void play_widescreen_mode(int x,int y, int width,int height);
	void stop_widescreen_mode();
	bool get_widescreen_mode();
	int get_widescreen_startx();
	int get_widescreen_starty();
	int get_widescreen_width();
	int get_widescreen_height();
    void get_widescreen_viewport(zz_viewport& screen_viewport);

	LPDIRECT3DTEXTURE9 screen_texture;
	LPDIRECT3DSURFACE9 screen_surface;
    LPDIRECT3DSURFACE9 screen_back_buffer;
	
	int screen_texture_width,screen_texture_height;
		
	TileBehavior* m_tiles[MAX_TILE_NUMBER];
	int num_current_tile;
    int type;
    float time;   
     
private:

    bool texture_setup_onoff;
	bool device_reset_onoff;
    bool play_onoff;
    int state_number;
	float max_time;
	float accumulate_time;
    int screen_color[3];
    float fade_t1,fade_t2;    
    bool wide_screen_onoff;
    float wide_screen_ratio;   
    float screen_ratio;
	zz_viewport wide_viewport;
	zz_viewport viewport;
    zz_viewport avatar_selection_viewport;

};


class zz_camera_sfx{

public:
    
	zz_camera_sfx();
	~zz_camera_sfx();
 
   	mat4 camera_m;
	zz_camera *camera;
	zz_viewfrustum frustum;
	mat4 camera_sfx_m;
	float far_plane;

	mat4 camera_base_m;
    float speed;      
    float speed_step;

	float side_speed;      
    float side_speed_step;
	
	float height_speed;
	float height_speed_step;


	float base_angle_speed;
    float base_angle_speed_step; 

	float view_angle_speed;
    float view_angle_speed_step; 


	void steal_camera();
	void return_camera();
    void init();
    void get_camera_sfx_matrix(vec3 &t);
	void calculate_steal_camera();
	void update_angle(int mouse_xx,int mouse_yy);
	void update_speed(float fspeed_step);
	void update_side_speed(float fspeed_step);
	void update_base_angle(float angle_step);
    void update_height_speed(float fspeed_step);
	void update_view_angle(float angle_step);
	void update_length(int delta);
	void update_time(float *xPos, float *yPos);
	void set_observer_zpos(float zPos);
    void play_onoff();
    void draw_camera();
	void get_viewing_region(float z_depth,float region[2]);
	bool get_play_onoff();
    const mat4& get_steal_camera();
	bool get_success();
    void Init_observer_Camera(float xPos, float yPos, float zPos);
	
    
private:	
	
	float seta,phi,length;
    float base_angle;
	bool steal_onoff;
    bool steal_success;
	void Init_observer_Camera();
	void get_camera_observer_sfx_matrix();

};


class zz_sprite_sfx{

public:
    
	zz_sprite_sfx();
	~zz_sprite_sfx();
 
	void draw_sprite_sfx();
    void input_sprite_sfx_element(zz_texture * tex, const zz_rect * src_rect, const vec3 * center, const vec3 * position, color32 color, float ft1, float ft2, float max_t);
    void draw_sprite();
	void update_time();
    void pre_render();
	void post_render();
	void stop_sprite_sfx();
    bool get_play_onoff();
    bool get_pause_onoff();  
    void pause_on();
	void pause_off();

	zz_texture *sfx_tex;
	zz_rect sfx_rect;
	vec3 sfx_center;
	vec3 sfx_position;
	color32 sfx_color;
   
private:
	
	float max_time;
	float accumulate_time;
    float fade_t1, fade_t2;    
    bool play_onoff;
	bool pause_onoff;
};

class zz_avatar_selection_sfx{

public:
	zz_avatar_selection_sfx();
    ~zz_avatar_selection_sfx();

	zz_viewport viewport;
    zz_viewport avatar_selection_viewport;
 
	D3DXMATRIX projection_m;
	D3DXMATRIX avatar_projection_m, avatar_view_m, inv_avatar_view_m;
    mat4 view_m;
	mat4 buffer_m;
    mat4 buffer_m2;    

	float view_length, view_seta;
    float view_height;
    float view_fov, view_ratio, view_near, view_far;

    zz_texture *m_texBackGround;
	
	void change_avatar_viewport();
	void change_avatar_graphicpipeline();
	void change_default_graphicpipeline();
	void change_default_viewport();
    void clear_scene();
    void set_avatar_selection_viewport(float x, float y, float width, float height);
    void calculate_avatar_view_matrix();
	void calculate_avatar_projection_matrix();
    void set_texture(const char *filename);
	void draw_background();
};


class zz_moving_camera_screen_sfx{

public:

	zz_moving_camera_screen_sfx();
    ~zz_moving_camera_screen_sfx();

	zz_viewport viewport;
    zz_viewport moving_camera_screen_viewport;
 
	D3DXMATRIX projection_m;
	D3DXMATRIX camera_projection_m, camera_view_m, inv_camera_view_m;
    mat4 view_m;
	mat4 buffer_m;
    mat4 buffer_m2;    
    
	float view_fov, view_ratio, view_near, view_far;
	zz_viewfrustum view_frustum;
	vec3 view_position;
    


	void change_camera_viewport();
	void change_camera_graphicpipeline();
	void change_default_graphicpipeline();
	void change_default_viewport();
    void set_camera_screen_viewport(int x, int y, int width, int height);
    void calculate_camera_projection_matrix();
    void Input_camera_matrix(const vec3 &position, const quat &rotation);
	void clear_scene();
};


#endif // __ZZ_SFX_H__