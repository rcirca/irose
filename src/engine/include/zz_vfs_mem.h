/** 
 * @file zz_vfs_mem.h
 * @brief virtual file system (memory).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jun-2003
 *
 * $Header: /engine/include/zz_vfs_mem.h 4     03-12-02 9:20p Zho $
 * $History: zz_vfs_mem.h $
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/include
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/include
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 4:11p
 * Updated in $/engine/include
 * some code cleanup
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
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-11-28   Time: 1:47a
 * Updated in $/znzin11/engine/include
 * new zz_vfs_pkg class added and some modifications
 */

#ifndef	__ZZ_VFS_MEM_H__
#define __ZZ_VFS_MEM_H__

#ifndef __ZZ_VFS_H__
#include "zz_vfs.h"
#endif

#include <stdio.h>

// vfs_mem class does not support write operations
// and open by filename. Instead, use memory pointer.
class zz_vfs_mem : public zz_vfs {
	const char * _mp; // memory pointer
	uint32 _mem_size; // memory size in byte

	// buf : buffer to be read into
	// size : maximum size of the buffer
	// return : size of the data that was read
	uint32 read_ (char * buf, uint32 size);
		
public:
	zz_vfs_mem();
	~zz_vfs_mem();

	// mem_pointer: memory pointer of the buffer
	bool open (const char * mem_pointer, uint32 mem_size);
	bool close ();
	
	uint32 get_size () const;
};

#endif // __ZZ_VFS_MEM_H__