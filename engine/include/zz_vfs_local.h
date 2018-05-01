/** 
 * @file zz_vfs_local.h
 * @brief virtual file system (local).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/include/zz_vfs_local.h 9     04-03-10 10:42a Zho $
 * $History: zz_vfs_local.h $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-27   Time: 9:45p
 * Updated in $/engine/include
 * gamma ramp bug fixed.
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-06   Time: 3:05p
 * Updated in $/engine/include
 * profiler added
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-05   Time: 11:19a
 * Updated in $/engine/include
 * let some functions inline
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/include
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/include
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-01   Time: 4:11p
 * Updated in $/engine/include
 * some code cleanup
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 12:26p
 * Updated in $/engine/include
 * some updates
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:06p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:58p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:54p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:48p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:20p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/include
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-11-28   Time: 1:47a
 * Updated in $/znzin11/engine/include
 * new zz_vfs_pkg class added and some modifications
 */

#ifndef	__ZZ_VFS_LOCAL_H__
#define __ZZ_VFS_LOCAL_H__

#ifndef __ZZ_VFS_H__
#include "zz_vfs.h"
#endif

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <io.h> // for _access

class zz_vfs_local : public zz_vfs {
	FILE * _fp;
	zz_string _filename;

	// buf : buffer to be read into
	// size : maximum size of the buffer
	// return : size of the data that was read
	uint32 read_ (char * buf, uint32 size);
	
	// buf : buffer to be written
	// size : maximum size of the buffer
	// return : size of the data that was written
	uint32 write_ (const char * buf, uint32 size);

public:
	zz_vfs_local(void);
	~zz_vfs_local(void);

	// filename : ex) d:/cvs/znzin/haha/zho.txt or zho.txt
	bool open (const char * filename, const zz_vfs_mode mode = ZZ_VFS_READ);
	bool close (void);
	
	bool exist (const char * filename) const;

	const char * get_path () const;

	// get file size in byte
	uint32 get_size () const;
	uint32 get_size (const char * file_name) const;

	static uint32 s_get_size (const char * file_name);

	// get last modified time
	bool get_mtime (const char * file_name, uint64 * t) const;

	virtual int seek (long offset, zz_vfs_seek origin);
};

inline uint32 zz_vfs_local::write_ (const char * buf, const uint32 size)
{
	if (_fp) {
		return uint32(fwrite(buf, 1, size, _fp));
	}
	return 0;
}

inline uint32 zz_vfs_local::get_size () const
{
	return get_size(_filename.get());
}

inline uint32 zz_vfs_local::s_get_size (const char * file_name)
{
	//ZZ_PROFILER_INSTALL(Pget_size); // about 0.1ms!!! so big
	struct _stat file_stat;
	
	if (!file_name) return 0;

	if (_stat(file_name, &file_stat) == 0) {
		return file_stat.st_size;
	}
	return 0;
}

inline uint32 zz_vfs_local::get_size (const char * file_name) const
{
	return s_get_size(file_name);
}

inline const char * zz_vfs_local::get_path () const
{
	return _filename.get();
}

inline bool zz_vfs_local::exist (const char * filename) const
{
	assert(filename);
	return (_access(filename, 0) != -1);
}

inline int zz_vfs_local::seek (long offset, zz_vfs_seek origin)
{
	return fseek(_fp, offset, origin);
}

#endif // __ZZ_VFS_LOCAL_H__