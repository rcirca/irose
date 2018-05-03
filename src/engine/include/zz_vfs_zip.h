/** 
 * @file zz_vfs_zip.h
 * @brief virtual file system (unzip).
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    15-feb-2002
 *
 * $Header: /engine/include/zz_vfs_zip.h 5     03-12-16 11:40a Zho $
 * $History: zz_vfs_zip.h $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-16   Time: 11:40a
 * Updated in $/engine/include
 * to use zlib121 version by static library
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
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_VFS_ZIP_H__
#define __ZZ_VFS_ZIP_H__

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

// preprocessor needed for unzip.h
#ifdef WIN32
#ifndef _WINDOWS
#define _WINDOWS
#endif // _WINDOWS
#endif // WIN32

//#define ZLIB_DLL // for zlib dll use

// zlib.dll download
// http://sourceforge.net/project/showfiles.php?group_id=23617&release_id=79177
// or the unzip library in http://www.gzip.org/zlib/

#include "unzip.h" // in /zlip121/contrib/minizip/

#include "zz_vfs.h" // virtual file system base class

// read & write zip(WinZip, PKWare .ZIP) compressed file
class zz_vfs_zip : public zz_vfs {
	// unzip file class instance from [unzip.h]
	unzFile _uf;

	// zip file name. include path name. ex) D:/cvs/znzin/haha.zip
	// It can include relative path and absolute full path.
	zz_string _zipname; 
	
	// filename. inside the zip file. ex) /haha/zho.txt
	zz_string _filename;
	
	// parse _zipname & _filename from fullname
	// ex) fullname := "D:/cvs/znzin/haha.zip/haha/zho.txt"
	//     _zipname := "D:/cvs/znzin/haha.zip"
	//     _filename := "/haha/zho.txt"
	// used only in zz_vfs_zip::open() method
	void _parse_filename (const char * fullname); 

	// buf : buffer to be read into
	// size : maximum size of the buffer
	// return : size of the data that was read
	uint32 read_ (char * buf, uint32 size);
	
public:
	zz_vfs_zip();
	~zz_vfs_zip();

#define ZZ_VFS_ZIP_EXTENSION ".zip"

	// filename : ex) d:/cvs/znzin/haha.zip/haha/zho.txt
	bool open (const char * filename, const zz_vfs_mode mode = ZZ_VFS_READ);
	bool close (void);
	
	uint32 get_size () const;
};

// usage :
/*
void main(int argc, char ** argv)
{
	zz_vfs_zip myzip;
	char buf[100000];
	int i;
	int size;

	if (argc != 2) {
		return;
	}

	if (!myzip.open(argv[1], zz_vfs::ZZ_VFS_READ)) {
		printf("cannot open\n");
	}

	size = myzip.read(buf, sizeof(buf));

	printf("-------------------------\n");
	for (i = 0; i < size; i++) {
		printf("%c", buf[i]);
	}
	printf("------------------------\n");
	printf("size : %d\n", size);

	myzip.close();
}
*/

#endif // __ZZ_VFS_ZIP_H__