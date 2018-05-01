/** 
 * @file zz_vfs.h
 * @brief virtual file system.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-feb-2002
 *
 * $Header: /engine/include/zz_vfs.h 14    04-06-25 3:03a Zho $
 * $History: zz_vfs.h $
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/include
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/include
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 04-03-08   Time: 7:52p
 * Updated in $/engine/include
 * To use vfsgetdata()
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/include
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-06   Time: 3:05p
 * Updated in $/engine/include
 * profiler added
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-05   Time: 11:19a
 * Updated in $/engine/include
 * let some functions inline
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/include
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/include
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 12:22p
 * Updated in $/engine/include
 * changed some function order
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 12:07a
 * Updated in $/engine/include
 * after reading, set status to EOF
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
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-11-29   Time: 6:29a
 * Updated in $/znzin11/engine/include
 * fixed read(), write() bug
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/include
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef	__ZZ_VFS_H__
#define __ZZ_VFS_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_ASSERT_H__
#include "zz_assert.h"
#endif

#include <string.h>
#include <assert.h>

#define ZZ_VFS_NAMESIZE        ZZ_MAX_STRING
#define ZZ_VFS_BUFFER_SIZE     8192

#define FTP_STRING   "ftp://"
#define HTTP_STRING  "http://"
#define ZIP_STRING   "zip://"
#define LOCAL_STRING "file://"

// interface zz_vfs
class zz_vfs {
public:
	typedef enum { 
		ZZ_VFS_READ   = (1 << 0), 
		ZZ_VFS_WRITE  = (1 << 1),
	} zz_vfs_mode;

	// file name specification
	// http://192.168.0.1/test.zmd
	// ftp://192.168.0.1/test.zmd
	// http://192.168.0.1/my.zmd
	// zip://d:/temp/test.zip/my.zmd
	// c:/temp/test.zmd
	typedef enum {
		ZZ_VFS_LOCAL = 0, // ignore for now
		ZZ_VFS_HTTP,
		ZZ_VFS_FTP,
		ZZ_VFS_ZIP,		// all zip file is local
		ZZ_VFS_MEM,		// memory file system
		ZZ_VFS_PKG, // TriggerVFS package file system. default
	} zz_vfs_protocol;

	typedef enum {
		ZZ_VFS_SEEK_CUR = SEEK_CUR,
		ZZ_VFS_SEEK_END = SEEK_END,
		ZZ_VFS_SEEK_SET = SEEK_SET,
	} zz_vfs_seek;

	typedef enum {
		ZZ_VFS_INI = 0, // initialized, before reading
		ZZ_VFS_ING,     // is now reading
		ZZ_VFS_EOF,      // nothing to read. end of file
		ZZ_VFS_FILENOTFOUND
	} zz_vfs_status;

protected:
	 // derived classes should not use these members and methods.
	static zz_vfs_protocol parse_protocol_ (const char * filename, char * &real_filename);
	zz_vfs * real_filesystem_; // set by set_real_filesystem() in constructor
	char * data_;
	zz_vfs_status status_;
	void dump_ (const char * path_in, const char * data_in, uint32 size_in) const;

	// set real_filesystem. used in derived class' constructor
	void set_real_filesystem(zz_vfs * real_filesystem_in);
	// set current status
	void set_status(zz_vfs_status status_in);
	virtual uint32 read_ (char * buf, uint32 size);
	virtual uint32 write_ (const char * buf, uint32 size);
	
public:
	
	zz_vfs();
	zz_vfs(const char * filename, const zz_vfs_mode mode = ZZ_VFS_READ);
	virtual ~zz_vfs();
	
	// some virtual functions to be overrided in derived class
	virtual bool open (const char * filename, const zz_vfs_mode mode = ZZ_VFS_READ);
	virtual bool close (void);
	virtual int seek (long offset, zz_vfs_seek origin = ZZ_VFS_SEEK_CUR);	// does not support backward seek
	virtual uint32 get_size () const;
	virtual uint32 get_size (const char * file_name) const;
	virtual bool exist (const char * filename) const;
	virtual void * get_data (); // get read data pointer
	virtual zz_vfs_status get_status (); // view current status
	virtual const char * get_path () const; // get full path
	virtual bool get_mtime (const char * file_name, uint64 * t) const; // get last modified time
	
	virtual uint32 read (void);
	virtual uint32 read (const uint32 size);
	virtual uint32 read (char * buf, uint32 size);
	virtual uint32 write (const char * buf, uint32 size);
	
	char read_char (char& data_out);
	uchar read_uchar (uchar& data_out);
	float read_float (float& data_out);
	uint32 read_float2 (float data_out[2]);
	uint32 read_float3 (float data_out[3]);
	uint32 read_float4 (float data_out[4]);
	uint32 read_uint323 (uint32 data_out[3]);
	uint32 read_uint324 (uint32 data_out[4]);
	uint16 read_uint163 (uint16 data_out[3]);
	uint16 read_uint164 (uint16 data_out[4]);

	uint32 read_uint32 (uint32& data_out);
	uint32 read_uint32 (int& data_out);
	uint16 read_uint16 (uint16& data_out);
	int32 read_int16 (int16& data_out);
	int32 read_int32 (int32& data_out);
	char * read_string (char * string_out, uint32 size = 0);
	char * read_string_without_whitespace (char * string_out, bool ignore_whitespace = true);

	void write_char (char data_in);
	void write_uchar (uchar data_in);
	void write_float (float data_in);
	void write_uint32 (uint32 data_in);
	void write_int16 (int16 data_in);
	void write_uint16 (uint16 data_in);
	void write_int32 (int32 data_in);
	void write_string (const char * string_in, uint32 size = 0);

	// read data by filesize.
	// returns size read. read data can be obtained by get_data()
	uint32 readall ();
	
	// open file, read all data, and return data pointer and file size
	void * open_read_get_data (const char * path_in, uint32 * filesize_out);

	// dump file content to another file to check.
	void dump () const;

	// extract path name from full path name
	// ex) "c:\\temp\\abc.txt" -> "c:\\temp\\"
	static bool extract_path (char * extracted_path, const char * fullpath);

	// extract file name from full path name
	// ex) "c:\\temp\\abc.txt" -> "abc.txt"
	static bool extract_name_and_extension (char * extracted_name, const char * fullpath);

	// extract file name only from full path name
	// ex) "c:\\temp\\abc.txt" -> "abc.txt"
	bool extract_name_only (char * extracted_name, const char * fullpath);

	// extract file name extension from full path name
	// ex) "c:\\temp\\abc.txt" -> "abc.txt"
	static bool extract_extension_only (char * extracted_ext, const char * fullpath);

	// convert slash to backslash
	// *to* and *from* must have the same size
	static bool to_backslash (char * filename);
};


inline uint32 zz_vfs::write_ (const char * buf, uint32 size)
{
	assert(real_filesystem_);
	return real_filesystem_->write_(buf, size);
}

inline void zz_vfs::set_real_filesystem(zz_vfs * real_filesystem_in)
{
	real_filesystem_ = real_filesystem_in;
}

inline const char * zz_vfs::get_path () const
{
	return real_filesystem_->get_path();
}

inline zz_vfs::zz_vfs_status zz_vfs::get_status()
{
	return status_;
}

inline void zz_vfs::set_status (zz_vfs_status status_in)
{
	status_ = status_in;
}

inline uint32 zz_vfs::get_size () const
{
	uint32 size = real_filesystem_->get_size();
	return size;
}


inline void zz_vfs::write_char (char data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(&data_in, 1);
}

inline void zz_vfs::write_uchar (uchar data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), 1);
}

inline void zz_vfs::write_float (float data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), sizeof(float));
}

inline void zz_vfs::write_uint32 (uint32 data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), sizeof(uint32));
}

inline void zz_vfs::write_int16 (int16 data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), sizeof(int16));
}

inline void zz_vfs::write_uint16 (uint16 data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), sizeof(uint16));
}


inline void zz_vfs::write_int32 (int32 data_in)
{
	assert(real_filesystem_);
	real_filesystem_->write_(reinterpret_cast<char *>(&data_in), sizeof(int32));
}


inline void * zz_vfs::get_data ()
{
	return data_;
}

inline float zz_vfs::read_float (float& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), sizeof(float)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline uint32 zz_vfs::read_uint32 (uint32& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), sizeof(uint32)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline uint32 zz_vfs::read_uint32 (int& data_out)
{
	uint32 data_uint32;
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_uint32), sizeof(uint32)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out = static_cast<int>(data_uint32);
}

inline uint16 zz_vfs::read_uint16 (uint16& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), sizeof(uint16)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline int32 zz_vfs::read_int16 (int16& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), sizeof(int16)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline int32 zz_vfs::read_int32 (int32& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), sizeof(int32)) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline char zz_vfs::read_char (char& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(&data_out, 1) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}

inline uchar zz_vfs::read_uchar (uchar& data_out)
{
	assert(real_filesystem_);
	if (real_filesystem_->read_(reinterpret_cast<char *>(&data_out), 1) == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return data_out;
}


inline uint32 zz_vfs::read(void)
{
	uint32 size = get_size();
	uint32 read_count = read(size);
	if (read_count == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return read_count;
}


inline uint32 zz_vfs::read_ (char * buf, uint32 size)
{
	assert(real_filesystem_);
	uint32 read_count = real_filesystem_->read_(buf, size);
	if (read_count == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return read_count;
}

inline uint32 zz_vfs::read (char * buf, uint32 size)
{
	return read_(buf, size);
}

inline uint32 zz_vfs::write (const char * buf, uint32 size)
{
	return write_(buf, size);
}

inline uint32 zz_vfs::read_float2 (float data_out[2])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(float)*2);
}

inline uint32 zz_vfs::read_float3 (float data_out[3])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(float)*3);
}

inline uint32 zz_vfs::read_float4 (float data_out[4])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(float)*4);
}

inline uint32 zz_vfs::read_uint323 (uint32 data_out[3])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(uint32)*3);
}

inline uint32 zz_vfs::read_uint324 (uint32 data_out[4])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(uint32)*4);
}

inline uint16 zz_vfs::read_uint163 (uint16 data_out[3])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(uint16)*3);
}

inline uint16 zz_vfs::read_uint164 (uint16 data_out[4])
{
	return read(reinterpret_cast<char*>(data_out), sizeof(uint16)*4);
}

#endif // __ZZ_VFS_H__