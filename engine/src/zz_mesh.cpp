/** 
 * @file zz_mesh.CPP
 * @brief mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/src/zz_mesh.cpp 48    05-05-06 2:27p Choo0219 $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_profiler.h"
#include "zz_shader.h"
#include "zz_system.h"
#include "zz_vfs.h"
#include "zz_renderer.h"
#include "zz_mesh.h"
#include "nvtristrip.h"

#ifndef _DEBUG
	#pragma comment (lib, "nvtristrip.lib")
#else
	#pragma comment (lib, "NvTriStrip_d.lib")
#endif

ZZ_IMPLEMENT_DYNCREATE(zz_mesh, zz_node)

zz_mesh::zz_mesh(bool create_buffer_in) :
	vertex_format(ZZ_VF_NONE),
	vbuf(NULL),
	ibuf_list(NULL),
	ibuf_strip(NULL),
	index_type(TYPE_NONE),
	matid_numfaces(NULL),
	pmin(vec3_null),
	pmax(vec3_null),
	num_verts(0),
	num_normals(0),
	num_faces(0),
	num_bones(0),
	num_clip_faces(0),
	num_matids(0),
	clip_face(ZZ_CLIP_FACE_NONE),
	revision(0),
	max_bone_index(-1),
	delayed_rebind(false),
	num_indices(0),
	timestamp(0),
	ibuf_res(NULL),
	vbuf_res(NULL)
{
	load_weight = 1;
	num_uvs[0] = num_uvs[1] = num_uvs[2] = num_uvs[3] = 0;
	alpha = 1.0f;
	
    if (create_buffer_in) {
		if (!vbuf_res)
			vbuf_res = zz_new zz_vertex_buffer;
		if (!ibuf_res)
			ibuf_res = zz_new zz_index_buffer;
	}
	else {
		vbuf_res = NULL;
		ibuf_res = NULL;
	}
}

zz_mesh::~zz_mesh(void)
{
	unload();
	ZZ_SAFE_DELETE(vbuf_res);
	ZZ_SAFE_DELETE(ibuf_res);
}

void zz_mesh::set_num_matids ( uint16 num_matids_in )
{
	if ((num_matids != num_matids_in) || !matid_numfaces) {
		ZZ_SAFE_DELETE(matid_numfaces);
		num_matids = num_matids_in;
		
		if (num_matids_in > 0) {
			matid_numfaces = zz_new uint16[num_matids_in]; // will be deallocated in unload()::set_num_matids(0)
		}
		else {
			matid_numfaces = NULL;
		}
	}
}

void zz_mesh::set_num_verts (uint16 num_verts_in)
{
	if ((num_verts != num_verts_in) || !vbuf) {
		assert((num_verts_in == 0) || (num_verts == 0));
		ZZ_SAFE_DELETE(vbuf);
		num_verts = num_verts_in;

		// device vertex buffer will be destroyed in zz_renderer_d3d::create_vertex_buffer()
		if (num_verts_in > 0) {
			int size = num_verts*vertex_format.size;
			vbuf = zz_new char[size];
		}
		else {
			vbuf = NULL;
		}
		//ZZ_LOG("mesh: set_num_verts(%s, %d)\n", this->get_path(), num_verts*vertex_size);
	}
}

void zz_mesh::set_num_faces (uint16 num_faces_in)
{
	if (num_faces_in == num_faces) return; // do nothing

	if (num_faces_in == 0) { // removing case
		// delete existing first
		assert(ibuf_list);
		if (ibuf_list) {
			zz_delete [] ibuf_list;
			ibuf_list = NULL;
		}
		if (ibuf_strip) {
			zz_delete [] ibuf_strip;
			ibuf_strip = NULL;
		}
	}
	else { // creating case
		assert(!ibuf_list);
		assert(!ibuf_strip);
		assert(num_faces == 0);
		assert(num_faces_in > 0);
		ibuf_list = zz_new unsigned short[num_faces_in*3]; // deleted in set_num_faces(0) in unload()
	}
	num_faces = num_faces_in;
	num_indices = num_faces*3; // precalculated before generate_strip()
}

// create managed/systemmem/scratch pool objects
bool zz_mesh::init_device_objects ()
{
	if (!get_bindable()) return true; // if scratch, use internal pointer

	if (vbuf_res && vbuf_res->is_init_device_objects()) { // managed, systemmem, scratch
		if (vbuf_res->to_create()) {
			create_vertex_buffer();
		}
		if (vbuf_res->to_update()) {
			update_vertex_buffer();
		}
	}
	if (ibuf_res && ibuf_res->is_init_device_objects()) { // managed, systemmem, scratch
		if (ibuf_res->to_create()) {
			create_index_buffer();
		}
		if (ibuf_res->to_update()) {
			update_index_buffer();
		}
	}
	return true;
}

// destroy managed/systemmem/scratch pool objects
bool zz_mesh::delete_device_objects ()
{
	if (!get_bindable()) return true;

	if (vbuf_res && vbuf_res->is_delete_device_objects()) {
		if (vbuf_res->to_destroy()) {
			destroy_vertex_buffer(); // all index buffer is dynamic
		}
	}
	if (ibuf_res && ibuf_res->is_delete_device_objects()) {
		if (ibuf_res->to_destroy()) {
			destroy_index_buffer();
		}
	}
	return true;
}

// create default pool objects
bool zz_mesh::restore_device_objects ()
{
	if (!get_bindable()) return true; // if scratch, use internal pointer

	if (vbuf_res && vbuf_res->is_restore_device_objects()) { // managed, systemmem, scratch
		if (vbuf_res->to_create()) {
			create_vertex_buffer();
		}
		if (vbuf_res->to_update()) {
			update_vertex_buffer();
		}
	}
	if (ibuf_res && ibuf_res->is_restore_device_objects()) { // managed, systemmem, scratch
		if (ibuf_res->to_create()) {
			create_index_buffer();
		}
		if (ibuf_res->to_update()) {
			update_index_buffer();
		}
	}
	return true;
}

// destroy default_pool objects
bool zz_mesh::invalidate_device_objects ()
{
	if (!get_bindable()) return true;

	if (vbuf_res && vbuf_res->is_invalidate_device_objects()) {
		if (vbuf_res->to_destroy()) {
			destroy_vertex_buffer(); // all index buffer is dynamic
		}
	}
	if (ibuf_res && ibuf_res->is_invalidate_device_objects()) {
		if (ibuf_res->to_destroy()) {
			destroy_index_buffer();
		}
	}
	return true;
}

bool zz_mesh::set_path (const char * path_in)
{
	path.set(path_in);

	// set load_weight
	const int load_byte_per_msec = 1000;
	int filesize = znzin->file_system.get_size(path_in);

#if (1)
	load_weight = 1 + filesize/load_byte_per_msec; // 1(minimum size) + 1ms/(load_byte_per_msec )byte
#endif

	return true;
}

const char * zz_mesh::get_path () const
{
	return path.get();
}

void zz_mesh::init_reuse ()
{
	if (vbuf_res->get_ready()) {
		vbuf_res->make_unready();
	}
	if (ibuf_res->get_ready()) {
		ibuf_res->make_unready();
	}
	if (vbuf_res->get_updated()) {
		vbuf_res->make_unupdated();
		assert(vbuf_res->get_created());
	}
	if (ibuf_res->get_updated()) {
		ibuf_res->make_unupdated();
		assert(ibuf_res->get_created());
	}
}

// load() called by manager->spawn() or explicitly
bool zz_mesh::load ()
{
	assert(!delayed_rebind);

	if (vbuf_res->get_updated()) {
		assert(vbuf_res->get_created());
		return true;
	}

	if (!vbuf_res->get_ready()) { // data not ready
		if (!load_mesh()) { // read into mesh
			zz_assertf(0, "mesh: load_mesh(%s) failed", get_path());
			return false;
		}
		vbuf_res->make_ready();
		if (!ibuf_res->get_ready())
			ibuf_res->make_ready();
		revision++;
	}
	return bind_device();
}

// unload() called by manager->kill
bool zz_mesh::unload ()
{
	if (!vbuf_res) {
		return true;
	}

	bool ret = unbind_device();
	zz_assert(ret);

	if (ret) {
		if (vbuf_res)
			vbuf_res->make_init();
		if (ibuf_res)
			ibuf_res->make_init();
		set_num_verts(0);
		set_num_faces(0);
		set_num_matids(0);
	}
	assert(!vbuf_res || !vbuf_res->get_ready());
	assert(!ibuf_res || !ibuf_res->get_ready());
	assert(get_num_verts() == 0);
	assert(get_num_faces() == 0);
	assert(get_num_matids() == 0);
	return ret;
}

bool zz_mesh::load_mesh ()
{
	bool ret = zz_mesh_tool::load_mesh(get_path(), this);
	if (!ret) {
		ZZ_LOG("mesh: load_mesh(%s) failed.\n", get_path());
	}
	return ret;
}

void zz_mesh::set_dynamic (bool true_or_false)
{
	// if has already assigned vertex buffer
	vbuf_res->set_dynamic(true_or_false);
	
	assert(!vbuf_res->get_created());
}

void zz_mesh::set_pool (bool bindable)
{
	if (bindable) {
		if (vbuf_res->get_dynamic())
			vbuf_res->set_pool( zz_device_resource::ZZ_POOL_DEFAULT );
		else
			vbuf_res->set_pool( zz_device_resource::ZZ_POOL_MANAGED );
	}
	else {
		vbuf_res->set_pool( zz_device_resource::ZZ_POOL_SCRATCH );
	}
}

void zz_mesh::set_bindable (bool true_or_false)
{
	assert(true_or_false || (!true_or_false && get_bindable())); // cannot revert to unbind state!
	set_pool(true_or_false);
}

bool zz_mesh::get_dynamic () const
{
	return vbuf_res->get_dynamic(); // only affected by vertex buffer
}

bool zz_mesh::get_bindable () const
{
	return (vbuf_res->get_pool() != zz_device_resource::ZZ_POOL_SCRATCH); // check only vres
}

bool zz_mesh::flush_device (bool immediate)
{
	if (delayed_rebind) { // set from set_dynamic()
		unbind_device(); // unbind first
		bind_device(); // make bindable
		delayed_rebind = false; // initialize state
	}

	if (vbuf_res->get_updated()) { // if already was buffered, skip
		return true;
	}
	
	// set the mesh's priority level to top
	// CAUTION: manager can be meshes, terrain_meshes, or ocean_meshes
	zz_manager * man = (zz_manager *)get_root();
	assert(man->is_a(ZZ_RUNTIME_TYPE(zz_manager)));

	if (immediate) {
		man->flush_entrance(this);
	}
	else {
		man->promote_entrance(this);
	}

	return true;
}

// if the mesh has already had its own vertex buffer, this does not re-create it.
void zz_mesh::create_vertex_buffer ()
{
	assert(vbuf_res);
	assert(vbuf_res->get_ready());
	assert(vertex_format.size);
	assert(num_verts);

	vbuf_res->create_buffer(vertex_format.size * num_verts);
}

void zz_mesh::create_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_ready());
	assert(num_indices);

	ibuf_res->create_buffer(num_indices);
}

void zz_mesh::destroy_vertex_buffer ()
{
	assert(vbuf_res);
	assert(vbuf_res->get_created());

	vbuf_res->destroy_buffer();
}

void zz_mesh::destroy_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_created());

	ibuf_res->destroy_buffer();
}

void zz_mesh::update_vertex_buffer ()
{
	assert(vbuf_res);
	assert(vbuf_res->get_created());
	// can be updated() state

	vbuf_res->update_buffer(vbuf);
}

void zz_mesh::update_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_created());

	ibuf_res->update_buffer((index_type == TYPE_STRIP) ? ibuf_strip : ibuf_list);
}

// use this at off-line(tool interface)
bool zz_mesh::generate_strip ()
{
	if (num_indices != num_faces*3) { // already striped?
		ZZ_LOG("mesh:generate_strip() failed. already striped\n");
		return false;
	}

	if (num_indices == num_verts) { // if it is not welded mesh(for morph), skip
		index_type = TYPE_LIST;
		ZZ_LOG("mesh:generate_strip() failed. not welded\n");
		return false;
	}
	
	if (num_matids > 0) { // if use material id, do not generate strip
        index_type = TYPE_LIST;
		ZZ_LOG("mesh:generate_strip() failed. mesh has material ids\n");
		return false;
	}

	PrimitiveGroup * prim_group(NULL);
	unsigned short num_groups(0);
	
	// generate strip by list index buffer
	GenerateStrips(reinterpret_cast<const unsigned short*>(ibuf_list), num_indices, &prim_group, &num_groups);

	bool ret(false);
	bool not_effective(false);

	if (prim_group->numIndices > (unsigned int)(num_indices - 100*3)) { // if not effective more than 100 polygon
		not_effective = true;
	}

	if (not_effective) {
		ZZ_LOG("mesh:generate_strip() failed. not effective(%d->%d)\n", num_indices, prim_group->numIndices);
		index_type = TYPE_LIST;
		ret = false;
	}
	else {
		index_type = TYPE_STRIP;
		num_indices = prim_group->numIndices;
		assert(!ibuf_strip);
		ibuf_strip = zz_new unsigned short[prim_group->numIndices];
		memcpy(ibuf_strip, prim_group->indices, sizeof(unsigned short)*prim_group->numIndices);
		ret = true;
	}
	
	delete [] prim_group;

	return ret;
}

void zz_mesh::dump_indices ()
{
	ZZ_LOG("mesh:dump_indices(%d)...\n", num_indices);
	for (uint16 i = 0; i < num_indices; ++i) {
		ZZ_LOG("%d, ", ibuf_list[i]);
	}
	ZZ_LOG("\n");
}

int transform_index (int index, int src_size, int dest_size)
{
	// src_size = 3, dest_size = 5;
	int x, y;
	int scale = (dest_size - 1) / (src_size - 1);

	x = index % src_size;
	y = index / src_size;

	x *= scale;
	y *= scale;

	return x  + y * dest_size;
}

// create sqare strip index buffer
// returns s_num_indices
void zz_mesh::print_strip_indices (uint16 input_num_verts, uint16 output_num_verts)
{
	uint16 num_faces_one_axis = output_num_verts - 1;
	uint16 num_indices = (num_faces_one_axis)*(num_faces_one_axis*2 + 4) - 2;

	int strip_index = 0;
	unsigned short last_index = 0;
	int first, second, third;
	unsigned short i;
	int width = num_faces_one_axis * 2;
	int num_tri_faces = num_faces_one_axis*num_faces_one_axis*2;

	ZZ_LOG("#%d, \n", num_indices);

	int src_vert_size = output_num_verts, dest_vert_size = input_num_verts;

	for (i = 0; i < num_tri_faces; ++i) {
		if (i % 2) {
			first = (num_faces_one_axis + 1) * (i / width) + (i % width) / 2;
			second = first + num_faces_one_axis + 2;
			third = first + 1;
		}
		else {
			second = (num_faces_one_axis + 1) * (i / width) + (i % width) / 2;
			first = second + num_faces_one_axis + 1;
			third = first + 1;
		}
		if ((i % width) == 0) { // for each horizontal starting vertex
			if (i != 0) {
				ZZ_LOG("%d, ", transform_index(last_index, src_vert_size, dest_vert_size)); // degenerate vertex
				ZZ_LOG("%d, ", transform_index(first, src_vert_size, dest_vert_size));
			}
			ZZ_LOG("%d, ", transform_index(first, src_vert_size, dest_vert_size));
			ZZ_LOG("%d, ", transform_index(second, src_vert_size, dest_vert_size));
		}
		ZZ_LOG("%d, ", transform_index(third, src_vert_size, dest_vert_size));
		last_index = third;
	}
	ZZ_LOG("\n");
}

// create sqare strip index buffer
// returns s_num_indices
void zz_mesh::create_strip_indices (
	uint16 num_faces_one_axis,
	uint16& num_indices_out, 
	uint16 *& buffer_strip_out
	)
{
	num_indices_out = (num_faces_one_axis)*(num_faces_one_axis*2 + 4) - 2;

	assert(buffer_strip_out == 0);

	buffer_strip_out = zz_new uint16 [num_indices_out];

	int strip_index = 0;
	unsigned short last_index = 0;
	int first, second, third;
	unsigned short i;
	int width = num_faces_one_axis * 2;
	int num_tri_faces = num_faces_one_axis*num_faces_one_axis*2;

	for (i = 0; i < num_tri_faces; ++i) {
		if (i % 2) {
			first = (num_faces_one_axis + 1) * (i / width) + (i % width) / 2;
			second = first + num_faces_one_axis + 2;
			third = first + 1;
		}
		else {
			second = (num_faces_one_axis + 1) * (i / width) + (i % width) / 2;
			first = second + num_faces_one_axis + 1;
			third = first + 1;
		}
		if ((i % width) == 0) { // for each horizontal starting vertex
			if (i != 0) {
				buffer_strip_out[strip_index++] = last_index; // degenerate vertex
				buffer_strip_out[strip_index++] = first;
			}
			buffer_strip_out[strip_index++] = first;
			buffer_strip_out[strip_index++] = second;
		}
		buffer_strip_out[strip_index++] = third;
		last_index = third;
	}
}
