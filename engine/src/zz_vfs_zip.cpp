/** 
 * @file zz_vfs_zip.cpp
 * @brief virtual file system (unzip).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/src/zz_vfs_zip.cpp 9     04-04-16 1:56p Zho $
 * $History: zz_vfs_zip.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-04-16   Time: 1:56p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-02-25   Time: 8:34p
 * Updated in $/engine/src
 * not to overuse static char
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-16   Time: 11:40a
 * Updated in $/engine/src
 * to use zlib121 version by static library
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/src
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
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
 * User: Zho          Date: 03-11-30   Time: 7:49p
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
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <stdio.h>
#include <string.h>
#include <cassert>
#include "zz_type.h"
#include "zz_log.h"
#include "zz_vfs_zip.h"

#define ZZ_IGNORE_ZLIB

#ifndef ZZ_IGNORE_ZLIB

#pragma comment (lib, "zlib121.lib")

zz_vfs_zip::zz_vfs_zip() : zz_vfs(), _uf(NULL)
{
	set_real_filesystem(this);
}

zz_vfs_zip::~zz_vfs_zip(void)
{
	close();
	set_real_filesystem(NULL);
}

void zz_vfs_zip::_parse_filename (const char * fullname) 
{
	int length;
	char * pdest;
	char zipname[ZZ_VFS_NAMESIZE];
	char filename[ZZ_VFS_NAMESIZE];

	pdest = strstr(fullname, ZZ_VFS_ZIP_EXTENSION);
	length = int(pdest - fullname) + int(sizeof(ZZ_VFS_ZIP_EXTENSION)) - 1; // -1(NULL TERMINATOR)
	
	strncpy(zipname, fullname, length);
	zipname[length] = '\0';
	_zipname.set(zipname);
	strcpy(filename, pdest + sizeof(ZZ_VFS_ZIP_EXTENSION));
	_filename.set(filename);
}

// filename := "C://Program Files/My Folder/mygame.zip/inner directory/readme.txt"
bool zz_vfs_zip::open (const char * filename, const zz_vfs_mode mode) 
{
	assert(filename);

	_parse_filename(filename);

	if (mode == ZZ_VFS_READ) {
		_uf = unzOpen(_zipname.get());

#define CASESENSITIVITY (0)

		if (unzLocateFile(_uf, _filename.get(), CASESENSITIVITY) != UNZ_OK) {
			printf("error!\n");
			return false;
		}

		if (unzOpenCurrentFile(_uf) != UNZ_OK) {
			printf("error!\n");
			return false;
		}

		if (_uf != NULL) {
			return true;
		}
	}

	return NULL;
}

bool zz_vfs_zip::close () 
{
	if (_uf) {
		unzCloseCurrentFile(_uf);
		unzClose(_uf);
		_uf = NULL;
	}

	_zipname.reset();
	_filename.reset();

	return true;
}

// return the size of the buffer that be read
uint32 zz_vfs_zip::read_ (char * buf, const uint32 size) 
{
	int err;
	uint32 size_buf = ZZ_VFS_BUFFER_SIZE;
	uint32 read_size = 0;

	if (size_buf > size) {
		size_buf = size;
	}

	do {
		err = unzReadCurrentFile(_uf, buf, size_buf);
		if (err > 0) {
			/*
			for (int i = 0; i < err; i++) {
				printf("%c", ((char *)buf)[i]);
			}
			*/
			//buf = (unsigned char *)buf + err;
			buf += err;
			read_size += err;
		}
	} while ((err > 0) && (read_size < size));

	//if (err == 0) {
		// end of file
	//}

	return read_size;
}

uint32 zz_vfs_zip::get_size () const
{
	unz_file_info file_info;

	unzGetCurrentFileInfo(_uf, &file_info, NULL, 0, NULL, 0, NULL, 0);
	return file_info.uncompressed_size;
}

#endif // ZZ_IGNORE_ZLIB