/** 
 * @file zz_vfs_mem.cpp
 * @brief virtual file system (local).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/src/zz_vfs_mem.cpp 7     04-02-11 2:05p Zho $
 * $History: zz_vfs_mem.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/src
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 12:07a
 * Updated in $/engine/src
 * after reading, set status to EOF
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-01   Time: 4:11p
 * Updated in $/engine/src
 * some code cleanup
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 12:26p
 * Updated in $/engine/src
 * some updates
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:17p
 * Created in $/Engine/SRC
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include "zz_vfs_mem.h"
#include <sys/stat.h>
#include <string.h>

zz_vfs_mem::zz_vfs_mem(void) : zz_vfs()
{
	_mp = NULL;
	_mem_size = 0;
	set_real_filesystem(this);
}

zz_vfs_mem::~zz_vfs_mem(void)
{
	close();
	set_real_filesystem(NULL);
}

bool zz_vfs_mem::open (const char * mem_pointer, uint32 mem_size)
{
	_mp = mem_pointer;
	_mem_size = mem_size;

	return true;
}

bool zz_vfs_mem::close (void)
{
	_mp = NULL;
	_mem_size = 0;
	set_status(zz_vfs::ZZ_VFS_INI);
	return true;
}

uint32 zz_vfs_mem::get_size () const
{
	return _mem_size;
}

uint32 zz_vfs_mem::read_ (char * buf, const uint32 size)
{
	if (!_mp) return 0;

	uint32 copy_size = ZZ_MIN(size, _mem_size);

	memcpy(buf, _mp, copy_size);

	_mp += copy_size;
	_mem_size -= copy_size;
	
	if (copy_size == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}

	return copy_size;
}