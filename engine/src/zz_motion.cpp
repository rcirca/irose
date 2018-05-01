/** 
 * @file zz_motion.cpp
 * @brief motion class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    02-jun-2002
 *
 * $Header: /engine/src/zz_motion.cpp 17    05-05-06 2:30p Choo0219 $
 */

#include "zz_tier0.h"
#include <string.h>
#include "zz_algebra.h"
#include "zz_script_simple.h"
#include "zz_system.h"
#include "zz_channel_x.h"
#include "zz_channel_xy.h"
#include "zz_channel_position.h"
#include "zz_channel_rotation.h"
#include "zz_animatable.h"
#include "zz_profiler.h"
#include "zz_manager.h"
#include "zz_motion.h"
#include "zz_mesh.h"
#include "zz_autolock.h"
#include "zz_vfs_pkg.h"

using namespace std;

ZZ_IMPLEMENT_DYNCREATE(zz_motion, zz_node)

zz_motion::zz_motion(void) : do_loop(true), num_frames(0), fps(0), owner(NULL), 
	initial_position(vec3_null), initial_rotation(quat_id),
	interp_interval(0),
	num_channels(0),
	channels(0)
{
}

zz_motion::~zz_motion(void)
{
	if (channels)
		unload();
}

bool zz_motion::unload ()
{
	if (!channels) // already empty
		return true;

	assert(znzin);
	assert(znzin->channels);

	for (unsigned int i = 0; i < num_channels; ++i) {
		znzin->channels->kill(channels[i]);
	}

	ZZ_SAFE_DELETE_ARRAY(channels);

	return true;
}

bool zz_motion::load (const char * file_name, float scale_in_load)
{
	//ZZ_PROFILER_INSTALL(Pload_motion);
	//ZZ_LOG("motion: load(%s)\n", file_name);

	zz_vfs_pkg motion_file;
	char magic_number[8];
	uint32 frame_index, channel_index;
	uint32 frame_number;
	uint32 refer_id;
	
	assert(scale_in_load != 0.0f);
	
	if (!motion_file.open(file_name)) {
		ZZ_LOG("motion: [%s] file open failed.\n", file_name);
		return false;
	}

	filename.set(file_name);

	// header section
	motion_file.read_string(magic_number);
	
    // verify magic_number
	if (strncmp(magic_number, "ZMO0002", 7)) {
		ZZ_LOG("motion: motion file version mismatched\n");
        return false; // wrong version or file structure
    }

	// read the speed of frames
	motion_file.read_uint32(fps); // frame per second

	// read the number of frames
	motion_file.read_uint32(num_frames);

	// read num_channels
	motion_file.read_uint32(num_channels);
	assert(num_channels);
	assert(channels == 0);
	
	channels = zz_new zz_channel * [num_channels];

#ifdef _TESTCODE
	ZZ_LOG("motion: load(%s), filename%s), num_frames(%d), num_channels(%d)\n", this->get_name(),
		file_name, num_frames, num_channels);
#endif

	zz_channel * new_channel = NULL;
	uint32 channel_type;

	// read channel info
	for (channel_index = 0; channel_index < num_channels; ++channel_index) {
		motion_file.read_uint32(channel_type);
		motion_file.read_uint32(refer_id);

		// create new "noname" channel
		if (channel_type == ZZ_CTYPE_POSITION) {
            new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_position)));
			//ZZ_LOG("motion: load() channel_index(%d) = position\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_ROTATION) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_rotation)));
			//ZZ_LOG("motion: load() channel_index(%d) = rotation\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_NORMAL) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_position)));
			//ZZ_LOG("motion: load() channel_index(%d) = normal\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_ALPHA) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_x)));
			//ZZ_LOG("motion: load() channel_index(%d) = alpha\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_UV0) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_xy)));
			//ZZ_LOG("motion: load() channel_index(%d) = uv0\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_UV1) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_xy)));
			//ZZ_LOG("motion: load() channel_index(%d) = uv1\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_UV2) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_xy)));
			//ZZ_LOG("motion: load() channel_index(%d) = uv2\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_UV3) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_xy)));
			//ZZ_LOG("motion: load() channel_index(%d) = uv3\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_TEXTUREANIM) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_x)));
			//ZZ_LOG("motion: load() channel_index(%d) = textureanim\n", channel_index);
		}
		else if (channel_type == ZZ_CTYPE_SCALE) {
			new_channel = static_cast<zz_channel *>
				(znzin->channels->spawn(NULL, ZZ_RUNTIME_TYPE(zz_channel_x)));
			//ZZ_LOG("motion: load() channel_index(%d) = scale\n", channel_index);
		}
		else {
			ZZ_LOG("motion: load() failed. invalid channel type\n");
			return false; // no_channel_type error
		}
		new_channel->set_channel_type(channel_type);
		new_channel->set_refer_id(refer_id);
		new_channel->assign(num_frames);

        // add handle into this motion
		if (new_channel) {
			channels[channel_index] = new_channel;
		}
		new_channel = NULL;
	}
	
	//ZZ_LOG("motion: channel_info done\n");

	float x_data;
	vec2 xy_data;
	vec3 position_data;
	quat rotation_data;
	vec3 relative_position, last_position, new_position;
	quat relative_rotation, last_rotation, new_rotation;
	mat4 initialTM = mat4_id;
	mat4 newTM = mat4_id;
	mat4 relativeTM = mat4_id;

	// read every frame info
	for (frame_index = 0; frame_index < uint32(num_frames); ++frame_index) {
		frame_number = frame_index; // currently, do not specify frame number
		// read every channel info
		
		for (channel_index = 0; channel_index < num_channels; ++channel_index) {
			if (channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_x))) {
				motion_file.read_float(x_data);
				channels[channel_index]->set_by_frame(frame_number, (void *)&x_data);
				//ZZ_LOG("motion: load(%s), frame(%d), channel(%d), x_data(%f)\n",
				//	file_name,
				//	frame_index,
				//	channel_index,
				//	x_data);
			}
			else if (channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_xy))) {
				motion_file.read_float2(xy_data.vec_array);
				channels[channel_index]->set_by_frame(frame_number, (void *)&xy_data);
				//ZZ_LOG("motion: load(%s), frame(%d), channel(%d), x_data(%f:%f)\n",
				//		file_name,
				//		frame_index,
				//		channel_index,
				//		xy_data.x, xy_data.y);
			}
			else if (channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_position))) {
				motion_file.read_float3(position_data.vec_array);
				position_data.x *= scale_in_load;
				position_data.y *= scale_in_load;
				position_data.z *= scale_in_load;

				position_data.x = ZZ_XFORM_IN(position_data.x);
				position_data.y = ZZ_XFORM_IN(position_data.y);
				position_data.z = ZZ_XFORM_IN(position_data.z);

				channels[channel_index]->set_by_frame(frame_number, (void *)&position_data);
			}
			else if (channels[channel_index]->is_a(ZZ_RUNTIME_TYPE(zz_channel_rotation))) {
				motion_file.read_float(rotation_data.w);
				motion_file.read_float(rotation_data.x);
				motion_file.read_float(rotation_data.y);
				motion_file.read_float(rotation_data.z);
				channels[channel_index]->set_by_frame(frame_number, (void *)&rotation_data);
			}
			else {
				return false; // no_such_channel_type error
			}
		}

		// read initial rotation and position
		if (frame_number == 0) {
			channels[0]->get_by_frame(0, (void *)&initial_position);
			//ZZ_LOG("initial_position[%s] = [%f, %f, %f]\n", file_name, initial_position.x, initial_position.y, initial_position.z);
			if (num_channels > 1) {
				channels[1]->get_by_frame(0, (void *)&initial_rotation);
			}
		}

		// ignore ongoing animation
		// set initial direction
		direction_vector = vec3(0, -1, 0); // negative-y axis is front direction
	}
	return true;
}

void zz_motion::set_channel_interp_style (zz_node_type * channel_type, zz_interp_style style)
{
	for (unsigned int i = 0; i < num_channels; ++i) {
		if (channels[i]->is_a(channel_type)) {
			channels[i]->set_interp_style(style);
		}
	}
}

bool zz_motion::update_mesh (zz_mesh * mesh, int current_frame, int num_verts, float& alpha)
{
	assert(mesh);

	uint32 referid;

	zz_channel * channel;

	bool use_alpha_animation = false;

	for (unsigned int index = 0; index < num_channels; ++index) {
		channel = channels[index];
		referid = channel->refer_id;

		zz_assertf((int)referid < num_verts,
			"apply_motion(%s) failed. export with DO_NOT_SHARE option checked\n",
			mesh->get_path());

		switch (channel->channel_type) {
			case ZZ_CTYPE_POSITION:
				mesh->set_pos(referid, 
					static_cast<zz_channel_position*>(channel)->positions[current_frame]);
				break;
			case ZZ_CTYPE_NORMAL:
				mesh->set_normal(referid, 
					static_cast<zz_channel_position*>(channel)->positions[current_frame]);
				break;
			case ZZ_CTYPE_ALPHA:
				alpha = static_cast<zz_channel_x*>(channel)->floats[current_frame];
				mesh->alpha = alpha;
				use_alpha_animation = true;
				break;
			case ZZ_CTYPE_UV0:
				mesh->set_uv(referid, 0, static_cast<zz_channel_xy*>(channel)->xys[current_frame]);
				break;
			case ZZ_CTYPE_UV1:
				mesh->set_uv(referid, 1, static_cast<zz_channel_xy*>(channel)->xys[current_frame]);
				break;
			case ZZ_CTYPE_UV2:
				mesh->set_uv(referid, 2, static_cast<zz_channel_xy*>(channel)->xys[current_frame]);
				break;
			case ZZ_CTYPE_UV3:
				mesh->set_uv(referid, 3, static_cast<zz_channel_xy*>(channel)->xys[current_frame]);
				break;
		}
	}
	return use_alpha_animation;
}

