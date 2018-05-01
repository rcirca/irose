/** 
 * @file zz_vfs_local.cpp
 * @brief virtual file system (local).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/src/zz_vfs_local.cpp 10    04-02-11 2:05p Zho $
 * $History: zz_vfs_local.cpp $
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-06   Time: 3:05p
 * Updated in $/engine/src
 * profiler added
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-05   Time: 11:19a
 * Updated in $/engine/src
 * let some functions inline
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/src
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 2:48a
 * Updated in $/engine/src
 * profiling
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
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/source
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 1:47a
 * Updated in $/znzin11/engine/source
 * new zz_vfs_pkg class added and some modifications
 */

#include "zz_tier0.h"
#include "zz_vfs_local.h"
#include "zz_profiler.h"
#include <string.h>

#ifdef WIN32
#include <windows.h>
#endif

zz_vfs_local::zz_vfs_local(void) : _fp(NULL)
{
	set_real_filesystem(this);
}

zz_vfs_local::~zz_vfs_local(void)
{
	close();
	set_real_filesystem(NULL);
}

bool zz_vfs_local::open (const char * filename, const zz_vfs_mode mode)
{
	//ZZ_LOG("vfs_pkg:open(%s)\n", filename);
	//ZZ_PROFILER_INSTALL(vfs_local_open);

	if (_fp) {
		close(); // close first
	}

	switch (mode) {
		case zz_vfs::ZZ_VFS_READ:
			_fp = fopen(filename, "rb");
			if (_fp) 	_filename.set(filename);
			break;
		case zz_vfs::ZZ_VFS_WRITE:
			_fp = fopen(filename, "wb");
			if (_fp) _filename.set(filename);
			break;
	}

	return (_fp) ? true : false;
}

bool zz_vfs_local::close (void)
{
	//ZZ_LOG("vfs_pkg:close(%s)\n", _filename.get());
	//ZZ_PROFILER_INSTALL(vfs_local_close);

	set_status(zz_vfs::ZZ_VFS_INI);

	if (_fp) {
		if (fclose(_fp) == 0) {
            _fp = NULL;
			return true;
		}
		else return false;
	}
	return true;
}

bool zz_vfs_local::get_mtime (const char * file_name, uint64 * t) const
{
	struct __stat64 st;

	if (_stat64(file_name, &st) != 0) {
		return false;
	}
	*t = st.st_mtime;
	return true;
}

uint32 zz_vfs_local::read_ (char * buf, uint32 size)
{
	//static zz_profiler p("Pvfs_local_read");
	//if (size > 1000) {
	//	ZZ_LOG("vfs_local: read_(%s, %d)\n", this->_filename.get(), size);
	//	p.begin();
	//}
	//ZZ_PROFILER_INSTALL(Pvfs_local_read); // 0.000131ms
	int read_count = 0;
	if (_fp) {
		read_count = uint32(fread(buf, 1, size, _fp));
	}
	if (read_count == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	//if (size > 1000) {
	//	p.end();
	//}
	return read_count;
}
