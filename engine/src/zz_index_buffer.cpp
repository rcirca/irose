/** 
 * @file zz_index_buffer.cpp
 * @brief vertex buffer class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-jan-2004
 *
 * $Header: /engine/src/zz_index_buffer.cpp 12    04-10-08 4:23p Zho $
 */

#include "zz_tier0.h"
#include "zz_index_buffer.h"
#include "zz_renderer_d3d.h"
#include "zz_system.h"

zz_index_buffer::zz_index_buffer () :
	size_total(0)
{
	this->pool = ZZ_POOL_DEFAULT;
}

zz_index_buffer::~zz_index_buffer ()
{
	assert(ZZ_HANDLE_IS_INVALID(device_handle));
}

void zz_index_buffer::create_buffer (uint16 num_indices_in)
{
	assert(znzin);
	assert(znzin->renderer);
	assert(ZZ_HANDLE_IS_INVALID(device_handle));
	assert(size_total == 0);

	size_total = num_indices_in * get_index_size();
	device_handle = znzin->renderer->create_index_buffer(*this, size_total);

	make_created();
}

void zz_index_buffer::destroy_buffer ()
{
	assert(znzin);
	assert(znzin->renderer);
	assert(ZZ_HANDLE_IS_VALID(device_handle));
	assert(size_total);

	znzin->renderer->destroy_index_buffer(device_handle);
	device_handle = ZZ_HANDLE_NULL;
	size_total = 0;

	make_uncreated();
}

void zz_index_buffer::update_buffer (const void * buf_in)
{
	assert(znzin);
	assert(znzin->renderer);
	assert(ZZ_HANDLE_IS_VALID(device_handle));
	assert(get_ready());
	assert(get_created());
	assert(size_total > 0);
	assert(buf_in);
	
	// main work
	znzin->renderer->update_index_buffer(*this, device_handle, buf_in, size_total, 0 /* offset */);	
	
	make_updated();
}
