/** 
 * @file zz_path.cpp
 * @brief path utility
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jan-2004
 *
 * $Header: /engine/src/zz_path.cpp 8     04-04-21 4:18p Zho $
 * $History: zz_path.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-04-21   Time: 4:18p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-02-27   Time: 5:48p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-25   Time: 8:18p
 * Updated in $/engine/src
 * to_dos() path bug fixed
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-21   Time: 12:06p
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-20   Time: 12:20p
 * Updated in $/engine/src
 * Added is_same_path, to_dos, to_unix.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-01   Time: 5:47p
 * Updated in $/engine/src
 * set/get_cwd() Added.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-17   Time: 2:07p
 * Created in $/engine/src
 * path utility
 */

#include "zz_tier0.h"
#include <stdio.h>
#include <io.h>
#include <direct.h>
#include <assert.h>

#include "zz_path.h"
#include "zz_log.h"

char _temp[2*ZZ_MAX_STRING];

char zz_path::cwd_[ZZ_MAX_STRING] = "";

zz_path::zz_path (const char * pathname)
{
	zz_slash_converter converted_path(pathname, 0, 1); // to slash, to lower
	name_.set( converted_path );
}

int zz_path::gen_list (const char * start_path_in, const char * filespec_in, zz_list<item_t>& list_out)
{
	struct _finddata_t c_file;
	long hfile;
	zz_string fullpath_spec(start_path_in);
	fullpath_spec += zz_string(filespec_in);
	zz_string fullpath;

	/* Find first .c file in current directory */
	if ( (hfile = _findfirst( fullpath_spec.get(), &c_file )) == -1L ) {
		// no file
		return 0;
	}

	int count = 0;
	do {
		if (strcmp(c_file.name, ".") == 0) continue; // skip if "."
		if (strcmp(c_file.name, "..") == 0) continue; // skip if ".."
		item_t item;
		fullpath.set(start_path_in);
		fullpath += c_file.name;
		item.size = c_file.size;
		item.type = (c_file.attrib & _A_SUBDIR) ? e_dir : e_file;
		if (item.type == e_dir) {
			zz_path::make_path(fullpath);
		}
		item.path = fullpath;
		list_out.push_back(item);
		count++;
	}
	while ( _findnext( hfile, &c_file ) == 0 );

	_findclose( hfile );

	return count;
}

int zz_path::gen_list_deep (const char * start_path_in, const char * filespec_in, zz_list<item_t>& list_out)
{
	zz_string path;
	zz_list<item_t> current_list;

	int count = 0;

	gen_list(start_path_in, filespec_in, current_list);
	zz_list<item_t>::iterator it;

	// 1. read files and dirs by filter
	item_t t;
	for (it = current_list.begin(); it != current_list.end(); it++) {
		t = (*it);
		list_out.push_back(t); // push current
		count++;
	}

	// 2. gen all list for dir
	gen_list(start_path_in, "*.*", current_list); // gen all

	// 2. recurse sub-dirs
	for (it = current_list.begin(); it != current_list.end(); it++) {
		if ((*it).is_dir()) {
            count += gen_list_deep((*it).path.get(), filespec_in, list_out);
		}
	}

	return count;
}

bool zz_path::make_path (zz_string& str)
{
	const char * cstr = str.get();
	int length = str.size();

	if (length < 1) return false; // no length

	if (cstr[length-1] == '\\') return true; // already path string

	if (cstr[length-1] == '/') return true; // already path string

	str += "/";

	return true;
}

int zz_path::get_first_slash (const zz_string& fullpath)
{
	const char * cstr = fullpath.get();
	int path_length = fullpath.size();
	
	if (path_length == 0) return -1;

	int index = 0;
	while (index < path_length) {
		if ((cstr[index] == '/') || (cstr[index] == '\\')) {
			return index;
		}
		++index;
	}
	return -1; // no slash found
}

int zz_path::get_last_slash (const zz_string& fullpath)
{
	const char * cstr = fullpath.get();
	int path_length = fullpath.size();
	
	if (path_length == 0) return -1;

	while (path_length-- > 0) {
		if ((cstr[path_length] == '/') || (cstr[path_length] == '\\')) {
			break;
		}
	}

	return path_length;
}

bool zz_path::extract_path (const zz_string& fullpath, zz_string& onlypath)
{
	const char * cstr = fullpath.get();
	int path_length = fullpath.size();
	
	int last_slash = get_last_slash(fullpath);

	if (last_slash == -1) return false;

	strncpy(_temp, cstr, last_slash + 1);
	_temp[last_slash + 1] = '\0';
	onlypath.set(_temp);

	return true;
}


bool zz_path::extract_name_ext (const zz_string& fullpath, zz_string& name_and_extension_only)
{
	const char * cstr = fullpath.get();
	int path_length = fullpath.size();
	
	if (path_length == 0) return false;

	const char * slash;
	const char * backslash;
	slash = strrchr(cstr, '/');
	backslash = strrchr(cstr, '\\');

	slash = (slash > backslash) ? slash : backslash;

	if (slash == NULL) { // whole path is name_and_extension_only
		name_and_extension_only = fullpath;
		return true;
	}

	int length = slash - cstr + 1;

	strncpy( _temp, slash + 1, length );
	_temp[length] = '\0';
	name_and_extension_only.set(_temp);

	return true;
}


bool zz_path::extract_ext (const zz_string& fullpath, zz_string& ext_only)
{
	const char * cstr = fullpath.get();
	int path_length = fullpath.size();
	
	const char * ext;
	ext = strrchr(cstr, '.');

	if (ext == NULL) {
		return false;
	}

	int length = ext - cstr + 1;

	strncpy( _temp, ext + 1, length );
	_temp[length] = '\0';
	ext_only.set(_temp);

	return true;
}

bool zz_path::extract_name (const zz_string& fullpath, zz_string& name_only)
{
	zz_string name_ext;
	if (false == extract_name_ext(fullpath, name_ext)) return  false;

	const char * cstr = name_ext.get();
	int name_ext_length = name_ext.size();
	
	const char * ext;
	ext = strrchr(cstr, '.');

	if (ext == NULL) { // use whole
		name_only = name_ext;
		return true;
	}

	int length = ext - cstr;

	strncpy( _temp, cstr, length );
	_temp[length] = '\0';
	name_only.set(_temp);

	return true;
}

const char * zz_path::get () const
{
	return name_.get();
}

bool zz_path::set_cwd (const char * fullpath)
{
	strcpy(cwd_, fullpath);
	int ret = _chdir(fullpath);
	return (ret != -1);
}

const char * zz_path::get_cwd ()
{
	if (cwd_[0] == '\0') {
		_getcwd(cwd_, ZZ_MAX_STRING);
	}
	return cwd_;
}

#define IS_UPPER(CH) ( ( (CH) >= 'A' ) && ( (CH) <= 'Z' ) )
#define IS_LOWER(CH) ( ( (CH) >= 'a' ) && ( (CH) <= 'z' ) )
#define IS_ALPHA(CH) (IS_UPPER(CH) || IS_LOWER(CH))
#define TO_UPPER(CH) ((CH) - ('a' - 'A'))
#define TO_LOWER(CH) ((CH) + ('a' - 'A'))
#define IS_SLASH(CH) ((CH == '/') || (CH == '\\'))

/*
path1 = "3ddata/haha/hoho/haha.txt"
path2 = "/3ddata/haha\\hoho\\//haha.txt"
*/
// ignores upper/lower case, and slash/backslash/doubleslash
bool zz_path::is_same_path (const char * path1, const char * path2, int len1, int len2)
{
	assert(path1 && path2);

	if (len1 == 0) len1 = strlen(path1);
	if (len2 == 0) len2 = strlen(path2);

	if (len1 < len2) { // swap if len1 is less than len2
		// swap len1 with len2
		int temp = len2;
		len2 = len1;
		len1 = temp;

		// swap path1 with path2
		const char * temp_path = path2;
		path2 = path1;
		path1 = temp_path;
	}

	char a, b;
	bool slash1(false), slash2(false);

	for (int i1 = 0; i1 < len1;) {
		assert(path1);
		
		if (!path2) { // if path2 is shorter than path1
			return IS_SLASH(*path1); // return true only if *path1 is a slash character.
		}

		a = *path1;
		b = *path2;
		
		if (IS_SLASH(a)) {
			slash1 = true; // current is slash stage
			++path1;
			++i1;
			continue;
		}
		if (IS_SLASH(b)) {
			slash2 = true; // current is slash stage
			++path2;
			continue;
		}
		if (slash1 != slash2) return false;
		slash1 = slash2 = false;

		if (a == b) { // same
			++path1;
			++path2;
			++i1;
			continue;
		}
		if (IS_ALPHA(a)) {
			a = (IS_UPPER(a)) ? TO_LOWER(a) : a;
			b = (IS_UPPER(b)) ? TO_LOWER(b) : b;
		}
		if (a != b) {
			return false;
		}
		++path1;
		++path2;
		++i1;
	}
	return true;
}

void zz_path::to_unix (char * dest, const char * src, int len_dest, int len_src)
{
	if (len_dest == 0) len_dest = strlen(dest);
	if (len_src == 0) len_src = strlen(src);

	bool last_slashed = false;
	char c;
	for (int i = 0; i < len_src;) {
		if (*src == '/' || *src == '\\') {
			++src;
			if (last_slashed) {
				continue; // ignore one more slashes
			}
			++i;
			*dest++ = '/';
			last_slashed = true;
		}
		else { // non-slash
			c = *src++;
			if (IS_UPPER(c)) {
				c = TO_LOWER(c);
			}
			*dest++ = c;
			++i;
			last_slashed = false;
		}
	}
	*dest = '\0';
}

void zz_path::to_dos (char * dest, const char * src, int len_dest, int len_src)
{
	if (len_dest == 0) len_dest = strlen(dest);
	if (len_src == 0) len_src = strlen(src);

	// for debugging
	if ((len_src >= ZZ_MAX_STRING) || (len_dest >= ZZ_MAX_STRING)) {
		ZZ_LOG("path: to_dos([%s]:%d->[%s]:%d) failed.\n", src, len_src, dest, len_dest);
		return;
	}

	bool last_slashed = false;
	char c;
	for (int i = 0; i < len_src; ++i, ++src) {
		if (*src == '/' || *src == '\\') {
			if (last_slashed) {
				continue; // ignore one more slashes
			}
			*dest++ = '\\';
			last_slashed = true;
		}
		else { // non-slash
			c = *src;
			if (IS_LOWER(c)) {
				c = TO_UPPER(c);
			}
			*dest++ = c;
			last_slashed = false;
		}
	}
	*dest = '\0';
}

