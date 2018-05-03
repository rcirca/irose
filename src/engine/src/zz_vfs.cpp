/** 
 * @file zz_vfs.cpp
 * @brief virtual file system.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    05-mar-2002
 *
 * $Header: /engine/src/zz_vfs.cpp 20    04-05-19 8:15p Zho $
 * $History: zz_vfs.cpp $
 * 
 * *****************  Version 20  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 19  *****************
 * User: Zho          Date: 04-03-10   Time: 11:17a
 * Updated in $/engine/src
 * 
 * *****************  Version 18  *****************
 * User: Zho          Date: 04-03-10   Time: 10:42a
 * Updated in $/engine/src
 * 
 * *****************  Version 17  *****************
 * User: Zho          Date: 04-02-27   Time: 5:54p
 * Updated in $/engine/src
 * 
 * *****************  Version 16  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 15  *****************
 * User: Zho          Date: 04-01-12   Time: 4:51p
 * Updated in $/engine/src
 * Added macro to ignore TriggerVFS
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 13  *****************
 * User: Zho          Date: 03-12-06   Time: 3:05p
 * Updated in $/engine/src
 * profiler added
 * 
 * *****************  Version 12  *****************
 * User: Zho          Date: 03-12-05   Time: 11:19a
 * Updated in $/engine/src
 * let some functions inline
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-12-04   Time: 10:21p
 * Updated in $/engine/src
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/src
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-12-02   Time: 6:03p
 * Updated in $/engine/src
 * read_uint32(int&) bug fixed. if read_uint32(int&) read zero, then
 * status changed to EOF. that was the bug.
 * In zz_type.h, new define zz_vfs added.
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-12-02   Time: 12:25p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 03-12-02   Time: 12:22p
 * Updated in $/engine/src
 * changed some function order
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 03-12-02   Time: 12:07a
 * Updated in $/engine/src
 * after reading, set status to EOF
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-01   Time: 5:59p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-01   Time: 4:31p
 * Updated in $/engine/src
 * comments added and renamed temp_vfs_xxx to static_vfs_xxx
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
 * User: Zho          Date: 03-11-29   Time: 6:29a
 * Updated in $/znzin11/engine/source
 * fixed read(), write() bug
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/source
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <string.h>
#include "zz_mem.h"
#include "zz_vfs.h"
#include "zz_vfs_zip.h"
#include "zz_vfs_local.h"
#include "zz_vfs_pkg.h"
#include "zz_log.h"
#include "zz_profiler.h"
#include "zz_system.h"

zz_vfs_local static_vfs_local;

zz_vfs::zz_vfs () : real_filesystem_(NULL), status_(ZZ_VFS_INI), data_(NULL)
{
}

zz_vfs::zz_vfs (const char * filename, const zz_vfs_mode mode) : real_filesystem_(NULL), status_(ZZ_VFS_INI), data_(NULL)
{
	if (filename)
		open(filename, mode);
}

zz_vfs::~zz_vfs()
{
	close();
}

uint32 zz_vfs::read(uint32 size)
{
	if (data_) {
		ZZ_SAFE_DELETE(data_);
	}
    data_ = zz_new char[size];
	assert(real_filesystem_);
	uint32 read_count = real_filesystem_->read_(data_, size);
	if (read_count == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return read_count;
}

zz_vfs::zz_vfs_protocol zz_vfs::parse_protocol_ (const char * filename /* in */, char * &real_filename /* out */)
{
	const char ftp_string[] = FTP_STRING;
	const char http_string[] = HTTP_STRING;
	const char zip_string[] = ZIP_STRING;
	const char local_string[] = LOCAL_STRING;

	// ftp
	if (real_filename = const_cast<char*>( strstr(filename, ftp_string) )) {
		// reset filename
		// "ftp://zho.pe.kr/haha.txt" -> "zho.pe.kr/haha.txt"
		real_filename += strlen(ftp_string);
		// real_filename += strlen(ftp_hostname);
		return ZZ_VFS_FTP;
	}
	
	// http
	if (real_filename = const_cast<char*>( strstr(filename, http_string) ) ) {
		real_filename += strlen(http_string);
		return ZZ_VFS_HTTP;
	}

	// zip
	if (real_filename = const_cast<char*>( strstr(filename, zip_string)) ) {
		// reset filename
		// "zip://haha.zip/haha.txt" -> "haha.zip/haha.txt"
		real_filename += strlen(zip_string);
		return ZZ_VFS_ZIP;
	}

	// local
	if (real_filename = const_cast<char*>( strstr(filename, local_string)) ) {
		// reset filename
		// "file://haha.zip/haha.txt" -> "haha.zip/haha.txt"
		real_filename += strlen(local_string);
		return ZZ_VFS_LOCAL;
	}

	// in pakcage mode, just point the start position of the filename string
	real_filename = const_cast<char *>(filename);
	return ZZ_VFS_PKG;
}

#if (0) // full version
bool zz_vfs::open (const char * filename, const zz_vfs_mode mode)
{
	char * real_filename = NULL;
	bool result = false;
	
	switch (parse_protocol(filename, real_filename)) {
		case ZZ_VFS_ZIP :
			real_filesystem_ = zz_new zz_vfs_zip;
			result = ((zz_vfs_zip *)real_filesystem_)->open(real_filename, mode);
			break;
		case ZZ_VFS_HTTP :
			break;
		case ZZ_VFS_PKG :
			if (zz_system::get_pkg_system()) {
				real_filesystem_ = zz_new zz_vfs_pkg(zz_system::get_pkg_system());
				result = ((zz_vfs_pkg *)real_filesystem_)->open(real_filename, mode);
				break;
			}
			// continue belowing
		case ZZ_VFS_LOCAL :
			real_filesystem_ = zz_new zz_vfs_local;
			result = ((zz_vfs_local *)real_filesystem_)->open(real_filename, mode);
			break;
	}
	if (!result) {
		status_ = ZZ_VFS_FILENOTFOUND;
	}
	return result;
}
#else
// pkg only version
bool zz_vfs::open (const char * filename, const zz_vfs_mode mode)
{
	//assert(filename);
	//ZZ_LOG("vfs:open(%s)\n", filename);
	//ZZ_PROFILER_INSTALL(open);

	
#ifndef ZZ_IGNORE_TRIGGERVFS
	if ((mode == ZZ_VFS_READ) && zz_system::get_pkg_system()) {
		real_filesystem_ = zz_new zz_vfs_pkg(zz_system::get_pkg_system());
		if (!((zz_vfs_pkg *)real_filesystem_)->open(filename, mode)) {
			status_ = ZZ_VFS_FILENOTFOUND;
			return false;
		}
		return true;
	}
#endif // ZZ_USE_TRIGGERVFS

	real_filesystem_ = zz_new zz_vfs_local;
	if (!((zz_vfs_local *)real_filesystem_)->open(filename, mode)) {
		status_ = ZZ_VFS_FILENOTFOUND;
		return false;
	}
	return true;
}
#endif

// return true if all right
bool zz_vfs::close(void)
{
	//if (real_filesystem_)
	//	ZZ_LOG("vfs:close(%s)\n", real_filesystem_->get_path());
	bool ret = true;
	set_status(zz_vfs::ZZ_VFS_INI);
	
	assert(real_filesystem_ != this);
		
	if (real_filesystem_) {
		ret = real_filesystem_->close();
	}
	if (real_filesystem_ != this) {
		ZZ_SAFE_DELETE(real_filesystem_); // created in open()
		ZZ_SAFE_DELETE(data_); // created in read()
	}
	return ret;
}

// read string and make null-terminated
// if size == 0, then read null-terminated string from file with '\0' char
char * zz_vfs::read_string (char * string_out, uint32 size)
{
	if (0 == size) {
		read_string_without_whitespace (string_out, false); // do not ignore whitespaces
	}
	else {
		assert(real_filesystem_);
		if (real_filesystem_->read_(string_out, size) == 0) { // not a explicit null-terminated string
 			set_status(zz_vfs::ZZ_VFS_EOF);
		}
	}
	return string_out;
}

char * zz_vfs::read_string_without_whitespace (char * string_out,
	bool ignore_whitespace)
{
	char c;
	int i = 0;
	bool read_started = false;
	bool in_double_quote = false;
	assert(real_filesystem_);
	while (real_filesystem_->read_char(c) != '\0' &&
		real_filesystem_->get_status() != zz_vfs::ZZ_VFS_EOF)
	{
		if (c == '\"') { // if double quote
			in_double_quote = !in_double_quote;
			continue; // ignore double quote
		}
		
		if (ignore_whitespace && !in_double_quote && 
			(c == ' ' || c == '\t' || c == 0x0D || c == 0x0A))
		{
			if (read_started) break;
			continue; // skip
		}
		string_out[i++] = c;
		read_started = true;
	}
	string_out[i] = '\0';
	if (i == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
		return NULL;
	}
	return string_out;
}

void zz_vfs::write_string (const char * string_in, uint32 size)
{
	if (!string_in) return;
	if (size == 0) { // calculate string length manually assuming null-terminated string
		size = static_cast<uint32>(strlen(string_in));
	}
	assert(real_filesystem_);
	real_filesystem_->write_(string_in, size);
}

int zz_vfs::seek(long offset, zz_vfs_seek origin)
{
	if (real_filesystem_) {
		return real_filesystem_->seek(offset, origin);
	}

	int count = 0;
	char character_to_get;

	if (offset == 0) return 0;

	switch (origin) {
		case ZZ_VFS_SEEK_CUR :
			// does not support backward seek
			for (count = 0; count < offset; count++) {
				assert(real_filesystem_);
				real_filesystem_->read_char(character_to_get);
				if (get_status() == ZZ_VFS_EOF) {
					return count;
				}
			}
			return count;
			break;
		case ZZ_VFS_SEEK_END : // not yet implemented
			break;
		case ZZ_VFS_SEEK_SET : // not yet implemented
			break;
	}

	return count;
}

bool zz_vfs::extract_path (char * extracted_path, const char * fullpath)
{
	int size = (int)strlen(fullpath);
	int i;
	int path_end_index = 0;
	assert(fullpath);
	assert(extracted_path);

	if (!fullpath) return false;
	if (!extracted_path) return false;

	for (i = size-1; i >= 0; i--) {
		if (fullpath[i] == '\\') {
			path_end_index = i;
			break;
		}
		if (fullpath[i] == '/') {
			path_end_index = i;
			break;
		}
	}

	for (i = 0; i <= path_end_index; i++) {
		extracted_path[i] = fullpath[i];
	}
	extracted_path[i] = '\0';

	return true;
}

bool zz_vfs::extract_name_and_extension (char * extracted_name, const char * fullpath)
{
	int size = (int)strlen(fullpath);
	int i;
	int path_end_index = 0;
	assert(fullpath);
	assert(extracted_name);

	if (!fullpath) return false;
	if (!extracted_name) return false;

	for (i = size-1; i >= 0; i--) {
		if (fullpath[i] == '\\') {
			path_end_index = i;
			break;
		}
		if (fullpath[i] == '/') {
			path_end_index = i;
			break;
		}
	}

	int name_index = 0;
	for (i = path_end_index + 1; i < size; i++, name_index++) {
		extracted_name[name_index] = fullpath[i];
	}
	extracted_name[name_index] = '\0';

	return true;
}

bool zz_vfs::extract_name_only (char * extracted, const char * fullpath)
{
	int size = (int)strlen(fullpath);
	int i;
	int path_end_index = 0;

	assert(fullpath);
	assert(extracted);

	if (!fullpath) return false;
	if (!extracted) return false;

	for (i = size-1; i >= 0; i--) {
		if (fullpath[i] == '\\') {
			path_end_index = i;
			break;
		}
		if (fullpath[i] == '/') {
			path_end_index = i;
			break;
		}
	}

	int name_index = 0;
	for (i = path_end_index + 1; i < size; i++, name_index++) {
		extracted[name_index] = fullpath[i];
		if (extracted[name_index] == '.') break;
	}
	extracted[name_index] = '\0';

	return true;
}

bool zz_vfs::extract_extension_only (char * extracted, const char * fullpath)
{
	int size = (int)strlen(fullpath);
	int i;
	int path_end_index = 0;

	assert(fullpath);
	assert(extracted);

	if (!fullpath) return false;
	if (!extracted) return false;

	for (i = size-1; i >= 0; i--) {
		if (fullpath[i] == '.') {
			path_end_index = i;
			break;
		}
	}

	int name_index = 0;
	for (i = 0; i < (size - path_end_index); i++) {
		extracted[i] = fullpath[path_end_index + i + 1];
	}
	extracted[i] = '\0';

	return true;
}

uint32 zz_vfs::readall ()
{
	int filesize = get_size();
	uint32 readsize = read_(data_, filesize);
	if (readsize == 0) {
		set_status(zz_vfs::ZZ_VFS_EOF);
	}
	return readsize;
}

// open file, read all data, and return data pointer and file size
void * zz_vfs::open_read_get_data (const char * path_in, uint32 * filesize_out)
{
	//ZZ_LOG("vfs:open_read_get_data(%s)\n", path_in);
	//ZZ_PROFILER_INSTALL(Popen_read_get_data);
	if (!open(path_in)) {
		filesize_out = 0;
		return NULL;
	}
	*filesize_out = read();
	return data_;
}

uint32 zz_vfs::get_size (const char * file_name) const
{
	if (real_filesystem_) {
		return real_filesystem_->get_size(file_name);
	}

#ifndef ZZ_IGNORE_TRIGGERVFS
	static zz_vfs_pkg static_vfs_pkg(zz_system::get_pkg_system());

	// if does not have real filesystem. use vfs_pkg's or vfs_local's
	if (zz_system::get_pkg_system()) {
		return static_vfs_pkg.get_size(file_name);
	}
#endif
	return static_vfs_local.get_size(file_name);

	return 0;
}

bool zz_vfs::to_backslash (char * str)
{
	if (!str) return false;

	uint32 size = (uint32)strlen(str);

	if (size == 0) return false;

	for (uint32 i = 0; i < size; i++) {
		str[i] = (str[i] == '/') ? '\\' : str[i];
	}
	return true;
}

bool zz_vfs::get_mtime (const char * path_in, uint64 * t) const
{
	//ZZ_LOG("vfs:get_mtime(%s)\n", path_in);
	//ZZ_PROFILER_INSTALL(get_mtime);

	if (real_filesystem_) {
		return real_filesystem_->get_mtime(path_in, t);
	}
#ifndef ZZ_IGNORE_TRIGGERVFS
	static zz_vfs_pkg static_vfs_pkg(zz_system::get_pkg_system());

	// if does not have real filesystem. use vfs_pkg's or vfs_local's
	if (zz_system::get_pkg_system()) {
		return static_vfs_pkg.get_mtime(path_in, t);
	}
#endif
	return static_vfs_local.get_mtime(path_in, t);
}

// data pointer will be invalidated
void zz_vfs::dump () const
{
	assert(real_filesystem_);
	if (data_) {
		return dump_(real_filesystem_->get_path(), data_, real_filesystem_->get_size());
	}
}

void zz_vfs::dump_ (const char * path_in, const char * data_in, uint32 size_in) const
{
	FILE * stream;
	char filename[256];

	sprintf(filename, "%s.dump", path_in);

	stream = fopen(filename, "wb+");

	fwrite((const void *)data_in, sizeof(char), size_in, stream);

	fclose(stream);
}

bool zz_vfs::exist (const char * filename) const
{
	//ZZ_LOG("vfs:exist(%s)\n", filename);
	//ZZ_PROFILER_INSTALL(Popen_read_get_data);
	if (real_filesystem_) {
		return real_filesystem_->exist(filename);
	}
#ifndef ZZ_IGNORE_TRIGGERVFS
	static zz_vfs_pkg static_vfs_pkg(zz_system::get_pkg_system());

	// if does not have real filesystem. use vfs_pkg's or vfs_local's
	if (zz_system::get_pkg_system()) {
		return static_vfs_pkg.exist(filename);
	}
#endif
	return static_vfs_local.exist(filename);
}
