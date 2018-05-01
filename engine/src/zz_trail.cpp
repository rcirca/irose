/** 
 * @file zz_trail.cpp
 * @brief trail class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    26-may-2003
 *
 * $Header: /engine/src/zz_trail.cpp 24    04-10-14 2:07p Zho $
 * $History: zz_trail.cpp $
 * 
 * *****************  Version 24  *****************
 * User: Zho          Date: 04-10-14   Time: 2:07p
 * Updated in $/engine/src
 * 
 * *****************  Version 23  *****************
 * User: Zho          Date: 04-10-14   Time: 2:05p
 * Updated in $/engine/src
 * 
 * *****************  Version 22  *****************
 * User: Zho          Date: 04-10-11   Time: 10:40a
 * Updated in $/engine/src
 * 
 * *****************  Version 21  *****************
 * User: Zho          Date: 04-10-09   Time: 2:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-10-09   Time: 12:14a
 * Updated in $/engine/src
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-08-16   Time: 4:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-07-05   Time: 9:38p
 * Updated in $/engine/src
 * vertex buffer redesigned
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-06-23   Time: 9:33a
 * Updated in $/engine/src
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-06-22   Time: 9:51a
 * Updated in $/engine/src
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-05-26   Time: 9:13p
 * Updated in $/engine/src
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-02-06   Time: 1:28p
 * Updated in $/engine/src
 * Xform coordinate system
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-12-17   Time: 10:31p
 * Updated in $/engine/src
 * - renamed MILLISECOND to MSEC.
 * - instant eye-blinking implementation.
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-12-17   Time: 8:38p
 * Updated in $/engine/src
 * restore to modulate1x
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-17   Time: 6:16p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-17   Time: 4:46p
 * Updated in $/engine/src
 * reference counting bug fixed. by check vbuffer before do reference
 * counting in invalidate_device_objects()
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-16   Time: 11:38p
 * Updated in $/engine/src
 * - In trail destructor, "not-unload" bug fixed.
 * - In trail render state, not to wrap texture coordinates.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-16   Time: 10:43p
 * Updated in $/engine/src
 * smooth trail near starting point by double zero quad spline.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-15   Time: 6:12p
 * Updated in $/engine/src
 * readded reference count
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-15   Time: 4:56p
 * Updated in $/engine/src
 * device restore added
 * 
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_node.h"
#include "zz_font.h"
#include "zz_font_d3d.h"
#include "zz_system.h"
#include "zz_trail.h"
#include "zz_texture.h"
#include "zz_manager.h"
#include "zz_renderer_d3d.h"
#include "zz_visible.h"
#include "zz_profiler.h"

ZZ_IMPLEMENT_DYNCREATE(zz_trail, zz_visible)

#define MAX_TRAIL_POINTS   (1000)   // number of total trail points are available
#define MAX_TIMES          (50)     // number of points spawned per frame can not exceed this value
#define COINCIDENT_EPSILON (1.0f*ZZ_SCALE_IN) // consider than two points(last-current) are the same point if distance < this (cm)

LPDIRECT3DDEVICE9       zz_trail::device = NULL;
LPDIRECT3DVERTEXBUFFER9 zz_trail::vbuffer = NULL;
int zz_trail::vb_ref_count = 0;

zz_trail::zz_trail() :
	trail_color(1, 1, 1, 1),
	num_segments(1),
	texture(NULL),
	life_time(0),
	total_life_time(0),
	local_sp(0, 0, 0), local_ep(0, 0, 0),
	world_length(0.0f),
	interp_curve(true),
	control_state(zz_trail::ZZ_CS_STOP),
	distance_per_point(20.0f),
	total_points(MAX_TRAIL_POINTS),
	trail_length(0.0f),
	trail_length_excess(0.0f),
	duration(0),
	last_temp_points(0)
{
	res.set_dynamic(true);
	res.set_pool(zz_device_resource::ZZ_POOL_DEFAULT);
}

zz_trail::~zz_trail()
{
	unload();
	assert(!res.get_ready());
}

void zz_trail::stop ()
{
	life_time = 0;
	this->total_life_time = 0;
	this->trail_length = 0;
	this->trail_length_excess = 0;
	control_state = zz_trail::ZZ_CS_STOP;
	sp.clear();
	ep.clear();
	point_time.clear();
}

bool zz_trail::unload ()
{
	// release vertex buffer
	if (texture) {
		znzin->textures->kill(texture);
		texture = NULL; // invalidate texture
	}

	res.make_unready();
	return unbind_device();
}

bool zz_trail::load (
		vec4 trail_color_in,
		float distance_per_point_in,
		zz_time life_time_in_millisecond,
		bool use_curve,
		const char * texture_path, 
		const vec3& start_point_in,
		const vec3& end_point_in)
{
	this->interp_curve = use_curve;
	this->trail_color = trail_color_in;
	this->distance_per_point = distance_per_point_in;
	this->duration = ZZ_MSEC_TO_TIME(life_time_in_millisecond);
	this->local_sp = start_point_in;
	this->local_ep = end_point_in;

	res.make_ready();

	// set texture
	assert(!this->texture);

	texture = (zz_texture*)znzin->textures->find(texture_path);

	if (!texture) {
		texture = (zz_texture*)znzin->textures->spawn(texture_path, ZZ_RUNTIME_TYPE(zz_texture), false /* not to autoload */);
		assert(texture);
		texture->set_path(texture_path);
	}
	
	texture->addref(); // to be released in destroy()
	znzin->textures->load(texture);

	set_bvolume_type(ZZ_BV_NONE);
	set_collision_level(ZZ_CL_NONE);
	set_cast_shadow(false);

	return bind_device();
}

vec3 recalc_ep (const vec3& sp, const vec3& ep, float length)
{
	vec3 sp_ep = ep - sp;
	sp_ep.normalize();
	sp_ep = sp + length*sp_ep;
	return sp_ep;
}

void zz_trail::shift_points (const vec3& new_world_sp, const vec3& new_world_ep)
{
	if (interp_curve) {
		// shift values
		sp_quad[3] = sp_quad[2];
		ep_quad[3] = ep_quad[2];
		sp_quad[2] = sp_quad[1];
		ep_quad[2] = ep_quad[1];
		sp_quad[1] = sp_quad[0];
		ep_quad[1] = ep_quad[0];
		sp_quad[0] = new_world_sp;
		ep_quad[0] = new_world_ep;
		time_quad[3] = time_quad[2];
		time_quad[2] = time_quad[1];
		time_quad[1] = time_quad[0];
		time_quad[0] = life_time;
	}
	else {
		// shift values
		sp_quad[1] = sp_quad[0];
		ep_quad[1] = ep_quad[0];
		sp_quad[0] = new_world_sp;
		ep_quad[0] = new_world_ep;
		time_quad[1] = time_quad[0];
		time_quad[0] = life_time;
	}
}

// Pushes num_to_add points into the point list.
void zz_trail::push_points (int num_to_add)
{
	std::list<vec3>::iterator it;
	vec3 new_sp, new_ep;
	zz_time new_time;
	int i;
	float dist;

	// insert sp, ep
	float t;

	if (interp_curve) {
		// clear last frame's points in range[0, 1]
		// : pop last frame's temporary positions.
		for (i = 0; i < last_temp_points; i++) {
			if (!sp.empty())
				sp.pop_front();
			if (!ep.empty())
				ep.pop_front();
			if (!point_time.empty())
				point_time.pop_front();
		}

		// recalc num_to_add for range[1, 2]
		dist = sp_quad[1].distance(sp_quad[2]) + ep_quad[1].distance(ep_quad[2]);
		num_to_add = (int)(dist / distance_per_point);

		// do catmull-rom curve interpolation. for range[1, 2]
		for (i = 1; i <= num_to_add; i++) {
			t = float(i)/num_to_add;
			new_sp = catmull_rom(sp_quad[3], sp_quad[2], sp_quad[1], sp_quad[0], t);
			new_ep = catmull_rom(ep_quad[3], ep_quad[2], ep_quad[1], ep_quad[0], t);
			new_time = (zz_time)catmull_rom(float(time_quad[3]), float(time_quad[2]), 
				float(time_quad[1]), float(time_quad[0]), t);

			new_ep = recalc_ep(new_sp, new_ep, this->world_length);

			sp.push_front(new_sp);
			ep.push_front(new_ep);
			point_time.push_front(new_time);
			
			if (sp.size() > MAX_TRAIL_POINTS) break; // exceeded maximum
		}

		// recalc num_to_add for range[0, 1]
		dist = sp_quad[0].distance(sp_quad[1]) + ep_quad[0].distance(ep_quad[1]);
		num_to_add = (int)(dist / distance_per_point);

		// do catmull-rom curve interpolation. for range[1, 2]
		for (i = 1; i <= num_to_add; i++) {
			t = float(i)/num_to_add;
			new_sp = catmull_rom(sp_quad[2], sp_quad[1], sp_quad[0], sp_quad[0], t);
			new_ep = catmull_rom(ep_quad[2], ep_quad[1], ep_quad[0], ep_quad[0], t);
			new_time = (zz_time)catmull_rom(float(time_quad[2]), float(time_quad[1]), 
				float(time_quad[0]), float(time_quad[0]), t);

			new_ep = recalc_ep(new_sp, new_ep, this->world_length);

			sp.push_front(new_sp);
			ep.push_front(new_ep);
			point_time.push_front(new_time);
			
			if (sp.size() > MAX_TRAIL_POINTS) break; // exceeded maximum
		}

		// add current point
		sp.push_front(sp_quad[0]);
		ep.push_front(ep_quad[0]);
		point_time.push_front(time_quad[0]);

		// store num_to_add in range[0,1] to be popped in next time
		last_temp_points = num_to_add + 1; // "1" means point in current position.
	}
	else {
		// do linear interpolation
		for (i = 1; i <= num_to_add; i++) {
			t = float(i)/num_to_add;
			lerp(new_sp, t, sp_quad[1], sp_quad[0]);
			lerp(new_ep, t, ep_quad[1], ep_quad[0]);
			new_time = (zz_time)lerp(t, float(time_quad[1]), float(time_quad[0]));
			new_ep = recalc_ep(new_sp, new_ep, this->world_length);
			sp.push_front(new_sp);
			ep.push_front(new_ep);
			point_time.push_front(new_time);
			if (sp.size() > MAX_TRAIL_POINTS) break; // exceeded maximum
		}
	}
}

void zz_trail::pop_points ()
{
	std::list<zz_time>::iterator it;
	
	if (life_time < duration) return; // skip
	while (!point_time.empty() && (point_time.back() < life_time - duration)) {
		if (!sp.empty())
			sp.pop_front();
		if (!ep.empty())
			ep.pop_front();
		if (!point_time.empty())
			point_time.pop_front();
	}
}

void zz_trail::update_animation (bool recursive, zz_time diff_time)
{
//	ZZ_PROFILER_INSTALL(Ptrail_update);
	if (control_state == ZZ_CS_STOP) return; // do nothing

	// + calculate num_to_add
	life_time += diff_time;
	int num_to_add = 1;

	vec3 new_world_sp, new_world_ep;
	const mat4& wtm = this->get_worldTM();

	// * get world_sp, world_ep
	mult(new_world_sp, wtm, local_sp);
	mult(new_world_ep, wtm, local_ep);

	// + set world length
	this->world_length = new_world_sp.distance(new_world_ep);
	
	assert(sp.size() == ep.size());

	// + initialize if first
	if (sp.size() == 0 && (control_state != zz_control_state::ZZ_CS_PAUSE)) {
		// if it is pause-mode and has no points, then do not insert starting points.
		// no points to interpolate
		sp.push_back(new_world_sp);
		ep.push_back(new_world_ep);
		point_time.push_back(life_time);
		sp_quad[0] = sp_quad[1] = sp_quad[2] = sp_quad[3] = new_world_sp;
		ep_quad[0] = ep_quad[1] = ep_quad[2] = ep_quad[3] = new_world_ep;
		time_quad[0] = time_quad[1] = time_quad[2] = time_quad[3] = life_time;
		return;
	}
	
	// + calculate distance and num_to_add
	float dist = new_world_sp.distance(sp_quad[0]) + new_world_ep.distance(ep_quad[0]); // both sp and ep affects dist
	trail_length = trail_length_excess + dist;
	num_to_add = (int)(trail_length / distance_per_point);
	trail_length_excess = trail_length - (int)(num_to_add*distance_per_point);

	// + push points
	if (control_state == zz_control_state::ZZ_CS_START) {
		if (num_to_add > 0) { // if it's time to update
			// shift points(update last point) and push points
			shift_points(new_world_sp, new_world_ep);
			push_points(num_to_add);
		}
	}

	// + pop points
	if (control_state != zz_control_state::ZZ_CS_PAUSE) {
		pop_points();
	}
}

bool zz_trail::update_vertex_buffer ()
{
//	ZZ_PROFILER_INSTALL(Ptrail_update_vertex_buffer);
	int i;

	VERTEX_TRAIL * pv;

	// lock vertex buffer
	if (FAILED(vbuffer->Lock(0, num_points * 2 * sizeof(VERTEX_TRAIL), // 2 for sp and ep
		(void **) &pv, D3DLOCK_DISCARD)))
	{
		return false; // lock failed
	}

	std::list<vec3>::iterator it_sp, it_ep;
	it_sp = sp.begin();
	it_ep = ep.begin();

	// test
	vec3 last_pos(0, 0, 0);
	//ZZ_LOG("trail: update_vertex_buffer()\n");
	for (i = 0; i < num_points; i++) {
		pv->position = *it_sp;
		pv->uv.set(float(i)/(num_points-1), 1); // bottom
		pv++;
		it_sp++;

		pv->position = *it_ep;
		pv->uv.set(float(i)/(num_points-1), 0); // top
		last_pos = pv->position;
		pv++;
		it_ep++;
	}

	vbuffer->Unlock();
	return true;
}

void zz_trail::setup_render_state ()
{
	assert(vbuffer);
	assert(device);

	if (!vbuffer || !device) {
		ZZ_LOG("trail: setup_render_state(%s, %x) failed. vbuffer or device was not set.\n", get_name(), this);
		return; // no vbuffer assigned
	}

	// prepare stream source and fvf
	device->SetStreamSource(0, vbuffer, 0, sizeof(VERTEX_TRAIL));
	device->SetFVF(D3DFVF_TRAIL);

	zz_renderer * r = znzin->renderer;
	if (!r) {
		ZZ_LOG("trail: setup_render_state(%s, %x) failed. renderer was not set.\n", get_name(), this);
		return;
	}

	r->set_vertex_shader(ZZ_HANDLE_NULL);
	r->set_pixel_shader(ZZ_HANDLE_NULL);

	// set cull mode to none (2-side)
	r->set_cullmode(zz_render_state::ZZ_CULLMODE_NONE);
	
	r->enable_fog(false);
	r->enable_alpha_blend(true, ZZ_BT_LIGHTEN); // ZZ_BT_LIGHTEN); // ZZ_BT_NORMAL);
	r->enable_alpha_test(false);
	r->enable_zwrite(false);

	// set up texture stages for simple texture stage copy
	r->set_render_state(ZZ_RS_TEXTUREFACTOR, ZZ_TO_D3DRGBA(trail_color.r, trail_color.g, trail_color.b, trail_color.a));

	r->set_texture_stage_state(0, ZZ_TSS_COLORARG1, ZZ_TA_TEXTURE);
	r->set_texture_stage_state(0, ZZ_TSS_COLORARG2, ZZ_TA_TFACTOR);
	r->set_texture_stage_state(0, ZZ_TSS_COLOROP, ZZ_TOP_MODULATE);
	r->set_texture_stage_state(1, ZZ_TSS_COLOROP, ZZ_TOP_DISABLE);

	r->set_texture_stage_state(0, ZZ_TSS_ALPHAOP, ZZ_TOP_DISABLE);

	r->set_sampler_state(0, ZZ_SAMP_ADDRESSU, ZZ_TADDRESS_CLAMP);
	r->set_sampler_state(0, ZZ_SAMP_ADDRESSV, ZZ_TADDRESS_CLAMP);

	// set to stage 0
	assert(texture);

	if (texture) {
		zz_assert(texture->get_device_updated());
		texture->set(0);
	}
}

void zz_trail::render (bool recursive)
{
//	ZZ_PROFILER_INSTALL(Ptrail_render);

	if (znzin->renderer->get_render_where() == ZZ_RW_SHADOWMAP) {
		return; // if shadow rendering pass, just skip
	}

	// setup render state
	setup_render_state();

	assert(sp.size() == ep.size());

	// get number of points
	num_points = (int)sp.size();
	assert(sp.size() == ep.size());
	if (num_points > MAX_TRAIL_POINTS) num_points = MAX_TRAIL_POINTS;
	if (num_points < 2) return; // skip if has none to render

	// build vertex buffer
	if (!update_vertex_buffer()) return;

	// set world tm
	mat4 modelviewTM;
	get_modelviewTM(modelviewTM);
	znzin->renderer->set_modelview_matrix(modelviewTM);

	// draw primitive
	try {
		if (FAILED(device->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, (num_points - 1)*2)))
			return;
	}
	catch (...) {
		ZZ_LOG("trail: render(). dp exception\n");
		// do nothing. maybe device lost state
	}

	//ZZ_LOG("trail: render(%d)\n", num_points);
	// not proceed recursively
}

void zz_trail::set_control_state (zz_control_state state)
{
	control_state = state;
	if (state == ZZ_CS_STOP) {
		stop();
	}	
}


bool zz_trail::restore_device_objects ()
{  
	//ZZ_LOG("trail: restore_device_objects(%s, %d)\n", get_name(), vb_ref_count);

	if (res.get_created()) { // if already buffered,
		return true; // do nothing
	}

	if (!device) {
		// get device
		device = ((zz_renderer_d3d *)(znzin->renderer))->get_device();
	}
	
	zz_render_state * state = znzin->get_rs();

	if (vb_ref_count == 0) {
		assert(!vbuffer);
		// create vertex buffer
		DWORD usage = D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY;
		if (!state->use_hw_vertex_processing_support) {
			usage |= D3DUSAGE_SOFTWAREPROCESSING;
		}
		if(FAILED(device->CreateVertexBuffer(
			2 * MAX_TRAIL_POINTS * sizeof(VERTEX_TRAIL),
			usage,	
			D3DFVF_TRAIL, D3DPOOL_DEFAULT,
			&this->vbuffer,
			NULL))) 
		{
			return false;
		}
	}
	
	vb_ref_count++;
	res.make_created();

	return true;
}

bool zz_trail::invalidate_device_objects ()
{
	if (!res.get_created()) return true; // already unbuffered

	assert(vbuffer);
	assert(device);

	assert(vb_ref_count > 0);

	//ZZ_LOG("trail: invalidate_device_objects(%s, %d)\n", get_name(), vb_ref_count);

	vb_ref_count--;

	if (vb_ref_count == 0) {
		SAFE_RELEASE(vbuffer);
		device = NULL; // invalidate device handle
	}
	res.make_uncreated();
	return true;
}

void zz_trail::before_render ()
{
	if (texture) {
		texture->load(); // preload
	}
}

void zz_trail::after_render ()
{
}
