/** 
 * @file zz_mesh_ishared.CPP
 * @brief index shared mesh class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    06-july-2004
 *
 * $Header: /engine/src/zz_mesh_ishared.cpp 8     04-07-21 12:11p Zho $
 */

#include "zz_tier0.h"
#include "zz_algebra.h"
#include "zz_mesh_ishared.h"
#include "zz_mesh_tool.h"
#include "zz_log.h"
#include "zz_profiler.h"

zz_mesh_ishared::zz_index zz_mesh_ishared::s_indices[NUM_INDEX_ORDER];

ZZ_IMPLEMENT_DYNCREATE(zz_mesh_ishared, zz_mesh)

// ishared index order
// see $docs/terrain/PolygonStrip.txt
// - first array item is the number of indicies
uint16 terrain0 [] = { 46 /* num_indices */, 5, 0, 6, 1, 7, 2, 8, 3, 9, 4, 4,10,10, 5,11, 6,12, 7,13, 8,14, 9, 9,15,15,10,16,11,17,12,18,13,19,14,14,20,20,15,21,16,22,17,23,18,24,19 };
uint16 terrain1 [] = { 24, 10, 0,12, 2, 8, 4, 9, 9, 8,14,12,18,22,24,24,18,19,14,14,20,20,10,22,12 };
uint16 terrain2 [] = { 26, 20,10,22,12,24,14,14, 8, 4, 3, 3, 8, 2,12, 6,10, 0, 0, 6, 1, 2, 2, 8, 8,12,14 };
uint16 terrain3 [] = { 28, 4,14, 2,12, 6,10, 5, 5, 6, 0, 2, 2,14,14,12,24,22,22,16,20,15,15,16,10,12,12,16,22 };
uint16 terrain4 [] = { 28, 20,10,16,12,22,18,23,24,24,18,14,12, 2, 0, 0,12,10,10,20,20,21,16,22,22,14,14, 4, 2 };
uint16 terrain5 [] = { 14, 10, 0,12, 2,14, 4, 4,20,20,10,22,12,24,14 };
uint16 terrain6 [] = { 9, 20, 0,12, 4,14,14,12,24,20 };
uint16 terrain7 [] = { 9, 20, 0,12, 2, 4, 4,12,24,20 };
uint16 terrain8 [] = { 9, 10, 0,12, 4,24,24,12,20,10 };
uint16 terrain9 [] = { 9, 20, 0,12, 4,24,24,12,22,20 };
uint16 terrain10 [] = { 4, 20, 0,24, 4 };

uint16 ocean11 [] = {
	4,
	2, 0, 3, 1
};

//uint16 rough_terrain12 [] = {
//	574, 
//	260, 0, 264, 4, 268, 8, 272, 12, 276, 16, 280, 20, 284, 24, 288, 28, 292, 32, 296, 36, 300, 40, 304, 44, 308, 48, 312, 52, 316, 56, 320, 60, 324, 64, 64, 520, 520, 260, 524, 264, 528, 268, 532, 272, 536, 276, 540, 280, 544, 284, 548, 288, 552, 292, 556, 296, 560, 300, 564, 304, 568, 308, 572, 312, 576, 316, 580, 320, 584, 324, 324, 780, 780, 520, 784, 524, 788, 528, 792, 532, 796, 536, 800, 540, 804, 544, 808, 548, 812, 552, 816, 556, 820, 560, 824, 564, 828, 568, 832, 572, 836, 576, 840, 580, 844, 584, 584, 1040, 1040, 780, 1044, 784, 1048, 788, 1052, 792, 1056, 796, 1060, 800, 1064, 804, 1068, 808, 1072, 812, 1076, 816, 1080, 820, 1084, 824, 1088, 828, 1092, 832, 1096, 836, 1100, 840, 1104, 844, 844, 1300, 1300, 1040, 1304, 1044, 1308, 1048, 1312, 1052, 1316, 1056, 1320, 1060, 1324, 1064, 1328, 1068, 1332, 1072, 1336, 1076, 1340, 1080, 1344, 1084, 1348, 1088, 1352, 1092, 1356, 1096, 1360, 1100, 1364, 1104, 1104, 1560, 1560, 1300, 1564, 1304, 1568, 1308, 1572, 1312, 1576, 1316, 1580, 1320, 1584, 1324, 1588, 1328, 1592, 1332, 1596, 1336, 1600, 1340, 1604, 1344, 1608, 1348, 1612, 1352, 1616, 1356, 1620, 1360, 1624, 1364, 1364, 1820, 1820, 1560, 1824, 1564, 1828, 1568, 1832, 1572, 1836, 1576, 1840, 1580, 1844, 1584, 1848, 1588, 1852, 1592, 1856, 1596, 1860, 1600, 1864, 1604, 1868, 1608, 1872, 1612, 1876, 1616, 1880, 1620, 1884, 1624, 1624, 2080, 2080, 1820, 2084, 1824, 2088, 1828, 2092, 1832, 2096, 1836, 2100, 1840, 2104, 1844, 2108, 1848, 2112, 1852, 2116, 1856, 2120, 1860, 2124, 1864, 2128, 1868, 2132, 1872, 2136, 1876, 2140, 1880, 2144, 1884, 1884, 2340, 2340, 2080, 2344, 2084, 2348, 2088, 2352, 2092, 2356, 2096, 2360, 2100, 2364, 2104, 2368, 2108, 2372, 2112, 2376, 2116, 2380, 2120, 2384, 2124, 2388, 2128, 2392, 2132, 2396, 2136, 2400, 2140, 2404, 2144, 2144, 2600, 2600, 2340, 2604, 2344, 2608, 2348, 2612, 2352, 2616, 2356, 2620, 2360, 2624, 2364, 2628, 2368, 2632, 2372, 2636, 2376, 2640, 2380, 2644, 2384, 2648, 2388, 2652, 2392, 2656, 2396, 2660, 2400, 2664, 2404, 2404, 2860, 2860, 2600, 2864, 2604, 2868, 2608, 2872, 2612, 2876, 2616, 2880, 2620, 2884, 2624, 2888, 2628, 2892, 2632, 2896, 2636, 2900, 2640, 2904, 2644, 2908, 2648, 2912, 2652, 2916, 2656, 2920, 2660, 2924, 2664, 2664, 3120, 3120, 2860, 3124, 2864, 3128, 2868, 3132, 2872, 3136, 2876, 3140, 2880, 3144, 2884, 3148, 2888, 3152, 2892, 3156, 2896, 3160, 2900, 3164, 2904, 3168, 2908, 3172, 2912, 3176, 2916, 3180, 2920, 3184, 2924, 2924, 3380, 3380, 3120, 3384, 3124, 3388, 3128, 3392, 3132, 3396, 3136, 3400, 3140, 3404, 3144, 3408, 3148, 3412, 3152, 3416, 3156, 3420, 3160, 3424, 3164, 3428, 3168, 3432, 3172, 3436, 3176, 3440, 3180, 3444, 3184, 3184, 3640, 3640, 3380, 3644, 3384, 3648, 3388, 3652, 3392, 3656, 3396, 3660, 3400, 3664, 3404, 3668, 3408, 3672, 3412, 3676, 3416, 3680, 3420, 3684, 3424, 3688, 3428, 3692, 3432, 3696, 3436, 3700, 3440, 3704, 3444, 3444, 3900, 3900, 3640, 3904, 3644, 3908, 3648, 3912, 3652, 3916, 3656, 3920, 3660, 3924, 3664, 3928, 3668, 3932, 3672, 3936, 3676, 3940, 3680, 3944, 3684, 3948, 3688, 3952, 3692, 3956, 3696, 3960, 3700, 3964, 3704, 3704, 4160, 4160, 3900, 4164, 3904, 4168, 3908, 4172, 3912, 4176, 3916, 4180, 3920, 4184, 3924, 4188, 3928, 4192, 3932, 4196, 3936, 4200, 3940, 4204, 3944, 4208, 3948, 4212, 3952, 4216, 3956, 4220, 3960, 4224, 3964
//};

uint16 rough_terrain12 [] = {
	574, 
	17, 0, 18, 1, 19, 2, 20, 3, 21, 4, 22, 5, 23, 6, 24, 7, 25, 8, 26, 9, 27, 10, 28, 11, 29, 12, 30, 13, 31, 14, 32, 15, 33, 16, 16, 34, 34, 17, 35, 18, 36, 19, 37, 20, 38, 21, 39, 22, 40, 23, 41, 24, 42, 25, 43, 26, 44, 27, 45, 28, 46, 29, 47, 30, 48, 31, 49, 32, 50, 33, 33, 51, 51, 34, 52, 35, 53, 36, 54, 37, 55, 38, 56, 39, 57, 40, 58, 41, 59, 42, 60, 43, 61, 44, 62, 45, 63, 46, 64, 47, 65, 48, 66, 49, 67, 50, 50, 68, 68, 51, 69, 52, 70, 53, 71, 54, 72, 55, 73, 56, 74, 57, 75, 58, 76, 59, 77, 60, 78, 61, 79, 62, 80, 63, 81, 64, 82, 65, 83, 66, 84, 67, 67, 85, 85, 68, 86, 69, 87, 70, 88, 71, 89, 72, 90, 73, 91, 74, 92, 75, 93, 76, 94, 77, 95, 78, 96, 79, 97, 80, 98, 81, 99, 82, 100, 83, 101, 84, 84, 102, 102, 85, 103, 86, 104, 87, 105, 88, 106, 89, 107, 90, 108, 91, 109, 92, 110, 93, 111, 94, 112, 95, 113, 96, 114, 97, 115, 98, 116, 99, 117, 100, 118, 101, 101, 119, 119, 102, 120, 103, 121, 104, 122, 105, 123, 106, 124, 107, 125, 108, 126, 109, 127, 110, 128, 111, 129, 112, 130, 113, 131, 114, 132, 115, 133, 116, 134, 117, 135, 118, 118, 136, 136, 119, 137, 120, 138, 121, 139, 122, 140, 123, 141, 124, 142, 125, 143, 126, 144, 127, 145, 128, 146, 129, 147, 130, 148, 131, 149, 132, 150, 133, 151, 134, 152, 135, 135, 153, 153, 136, 154, 137, 155, 138, 156, 139, 157, 140, 158, 141, 159, 142, 160, 143, 161, 144, 162, 145, 163, 146, 164, 147, 165, 148, 166, 149, 167, 150, 168, 151, 169, 152, 152, 170, 170, 153, 171, 154, 172, 155, 173, 156, 174, 157, 175, 158, 176, 159, 177, 160, 178, 161, 179, 162, 180, 163, 181, 164, 182, 165, 183, 166, 184, 167, 185, 168, 186, 169, 169, 187, 187, 170, 188, 171, 189, 172, 190, 173, 191, 174, 192, 175, 193, 176, 194, 177, 195, 178, 196, 179, 197, 180, 198, 181, 199, 182, 200, 183, 201, 184, 202, 185, 203, 186, 186, 204, 204, 187, 205, 188, 206, 189, 207, 190, 208, 191, 209, 192, 210, 193, 211, 194, 212, 195, 213, 196, 214, 197, 215, 198, 216, 199, 217, 200, 218, 201, 219, 202, 220, 203, 203, 221, 221, 204, 222, 205, 223, 206, 224, 207, 225, 208, 226, 209, 227, 210, 228, 211, 229, 212, 230, 213, 231, 214, 232, 215, 233, 216, 234, 217, 235, 218, 236, 219, 237, 220, 220, 238, 238, 221, 239, 222, 240, 223, 241, 224, 242, 225, 243, 226, 244, 227, 245, 228, 246, 229, 247, 230, 248, 231, 249, 232, 250, 233, 251, 234, 252, 235, 253, 236, 254, 237, 237, 255, 255, 238, 256, 239, 257, 240, 258, 241, 259, 242, 260, 243, 261, 244, 262, 245, 263, 246, 264, 247, 265, 248, 266, 249, 267, 250, 268, 251, 269, 252, 270, 253, 271, 254, 254, 272, 272, 255, 273, 256, 274, 257, 275, 258, 276, 259, 277, 260, 278, 261, 279, 262, 280, 263, 281, 264, 282, 265, 283, 266, 284, 267, 285, 268, 286, 269, 287, 270, 288, 271
};

uint16 * orders [] = {
	terrain0, terrain1, terrain2, terrain3, terrain4, terrain5,
	terrain6, terrain7, terrain8, terrain9, terrain10,
	ocean11,
	rough_terrain12
};

zz_mesh_ishared::zz_mesh_ishared (int key_in) :
	key(key_in),
	zz_mesh(false)
{
	index_type = zz_mesh::TYPE_STRIP;
	vbuf_res = zz_new zz_vertex_buffer;

	assert(s_indices[key].num_indices > 0);
	assert(s_indices[key].ibuf);
	assert(s_indices[key].ibuf_res);

	ibuf_res = s_indices[key_in].ibuf_res;
}

zz_mesh_ishared::~zz_mesh_ishared ()
{
	assert(ibuf_strip == 0);
	ibuf_res = NULL;
}

bool zz_mesh_ishared::load ()
{
	if (vbuf_res->get_updated()) { // already updated
		return true;
	}

	if (!vbuf_res->get_ready()) { // data not ready
		load_mesh();
		vbuf_res->make_ready();
	}

	return bind_device();
}

bool zz_mesh_ishared::unload ()
{
	if (!vbuf_res) { // already unloaded
		assert(get_num_verts() == 0);
		assert(get_num_faces() == 0);
		assert(get_num_matids() == 0);
		return true;
	}

	bool ret = unbind_device();

	if (ret) {
		vbuf_res->make_init();
		set_num_verts(0);
		set_num_faces(0);
		set_num_matids(0);
	}
	assert(!vbuf_res->get_ready());
	assert(get_num_verts() == 0);
	assert(get_num_faces() == 0);
	assert(get_num_matids() == 0);

	return true;
}

void zz_mesh_ishared::init_reuse ()
{
	if (vbuf_res->get_ready()) {
		vbuf_res->make_unready();
	}
	if (vbuf_res->get_updated()) {
		vbuf_res->make_unupdated();
		assert(vbuf_res->get_created());
	}
}

void zz_mesh_ishared::set_num_faces (uint16 num_faces_in)
{
	if (num_faces_in == num_faces) return; // do nothing

	if (num_faces_in == 0) { // removing case
		num_faces = 0;
		num_indices = 0;
		assert(ibuf_list);
		if (ibuf_list) {
			zz_delete [] ibuf_list;
			ibuf_list = NULL;
		}
		ibuf_strip = NULL;
	}
	else { // creating case
		assert(num_faces == 0);
		assert(num_faces_in > 0);
		ibuf_list = zz_new unsigned short[num_faces_in*3]; // deleted in set_num_faces(0) in unload()
		num_faces = num_faces_in;
		num_indices = s_indices[key].num_indices;
	}
}

void zz_mesh_ishared::create_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_created());
}

void zz_mesh_ishared::destroy_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_created());
}

void zz_mesh_ishared::update_index_buffer ()
{
	assert(ibuf_res);
	assert(ibuf_res->get_created());
}

void zz_mesh_ishared::init_static_all ()
{
	assert((sizeof(orders)/sizeof(void *)) == NUM_INDEX_ORDER);
	memset(s_indices, 0, NUM_INDEX_ORDER*sizeof(zz_index));
	for (int i = 0; i < NUM_INDEX_ORDER; ++i) {
		zz_mesh_ishared::init_static(i);
	}
}


void zz_mesh_ishared::dest_static_all ()
{
	assert((sizeof(orders)/sizeof(void *)) == NUM_INDEX_ORDER);
	for (int i = 0; i < NUM_INDEX_ORDER; ++i) {
		zz_mesh_ishared::dest_static(i);
	}
}

void zz_mesh_ishared::init_static (int key_in)
{
	assert(key_in >= 0);
	assert(key_in < NUM_INDEX_ORDER);

	assert(s_indices[key_in].num_indices == 0);

	s_indices[key_in].num_indices = orders[key_in][0];
	s_indices[key_in].ibuf_res = zz_new zz_index_buffer;
	s_indices[key_in].ibuf = &orders[key_in][1];
	s_indices[key_in].ibuf_res->make_ready();

	assert(s_indices[key_in].num_indices > 0);
}

void zz_mesh_ishared::dest_static (int key_in)
{
	assert(key_in >= 0);
	assert(key_in < NUM_INDEX_ORDER);

	//ZZ_SAFE_DELETE(s_indices[key_in].ibuf);
	s_indices[key_in].ibuf = 0;
	ZZ_SAFE_DELETE(s_indices[key_in].ibuf_res);
	s_indices[key_in].ibuf_res = NULL;
	s_indices[key_in].num_indices = 0;
}

// create managed/systemmem/scratch pool objects
bool zz_mesh_ishared::init_device_objects ()
{
	if (!get_bindable()) return true; // if scratch, use internal pointer

	if (vbuf_res->is_init_device_objects()) { // managed, systemmem, scratch
		if (vbuf_res->to_create()) {
			create_vertex_buffer();
		}
		if (vbuf_res->to_update()) {
			update_vertex_buffer();
		}
	}
	return true;
}

// destroy managed/systemmem/scratch pool objects
bool zz_mesh_ishared::delete_device_objects ()
{
	if (!get_bindable()) return true;

	if (vbuf_res->is_delete_device_objects()) {
		if (vbuf_res->to_destroy()) {
			destroy_vertex_buffer(); // all index buffer is dynamic
		}
	}
	return true;
}

// create default pool objects
bool zz_mesh_ishared::restore_device_objects ()
{
	if (!get_bindable()) return true; // if scratch, use internal pointer

	if (vbuf_res->is_restore_device_objects()) { // managed, systemmem, scratch
		if (vbuf_res->to_create()) {
			create_vertex_buffer();
		}
		if (vbuf_res->to_update()) {
			update_vertex_buffer();
		}
	}
	return true;
}

// destroy default_pool objects
bool zz_mesh_ishared::invalidate_device_objects ()
{
	if (!get_bindable()) return true;

	if (vbuf_res->is_invalidate_device_objects()) {
		if (vbuf_res->to_destroy()) {
			destroy_vertex_buffer(); // all index buffer is dynamic
		}
	}
	return true;
}

void zz_mesh_ishared::set_index_order (int index_order_in)
{
	key = index_order_in;
	zz_assert(key >= 0);
	zz_assert(key < NUM_INDEX_ORDER);

	ibuf_res = s_indices[key].ibuf_res;
	num_indices = s_indices[key].num_indices;

	assert(ibuf_res);
	assert(num_indices);
}
