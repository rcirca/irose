/** 
 * @file zz_path.h
 * @brief path utility
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-jan-2004
 *
 * $Header: /engine/include/zz_path.h 5     04-03-05 6:06p Zho $
 * $History: zz_path.h $
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-03-05   Time: 6:06p
 * Updated in $/engine/include
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-27   Time: 5:48p
 * Updated in $/engine/include
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-20   Time: 12:20p
 * Updated in $/engine/include
 * Added is_same_path, to_dos, to_unix.
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-01   Time: 5:47p
 * Updated in $/engine/include
 * set/get_cwd() Added.
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-01-17   Time: 2:07p
 * Created in $/engine/include
 * path utility
 */

#ifndef __ZZ_PATH_H__
#define __ZZ_PATH_H__

#ifndef __ZZ_STRING_H__
#include "zz_string.h"
#endif

#ifndef __ZZ_LIST_H__
#include "zz_list.h"
#endif

class zz_path {
public:
	enum e_type { e_dir, e_file };

	struct item_t {
		zz_string path;
		e_type type;
		unsigned int size;

		item_t() : size(0), type(e_file) {}
		bool is_dir () { return (type == e_dir); }
		bool is_file () { return (type == e_file); }
	};

	zz_path (const char * pathname);

	/// get the list of current path which includes files and dirs.
	/// @filespec_in file filter
	/// @start_path_in start path
	/// @list_out has the list required.
	static int gen_list (const char * start_path_in, const char * filespec_in, zz_list<item_t>& list_out);

	/// get the list of current path which includes files, dirs and subdirs.
	/// like get_list()
	/// @start_path_in start path
	/// @filespec_in file filter
	static int gen_list_deep (const char * start_path_in, const char * filespec_in, zz_list<item_t>& list_out);

	/// make this string into path string
	static bool make_path (zz_string& str);

	static bool extract_path (const zz_string& fullpath, zz_string& onlypath);

	static bool extract_name_ext (const zz_string& fullpath, zz_string& name_and_extension_only);

	static bool extract_ext (const zz_string& fullpath, zz_string& ext_only);

	static bool extract_name (const zz_string& fullpath, zz_string& name_only);

	const char * get () const;

	/// set current working directory
	static bool set_cwd (const char * fullpath);

	/// get current working directory
	static const char * get_cwd ();

	/// checks if the two path string is same
	static bool is_same_path (const char * path1, const char * path2, int len1 = 0, int len2 = 0);

	/// converts unix path into clean lower case unix style path string
	static void to_unix (char * dest, const char * src, int len_dest = 0, int len_src = 0);

	/// converts unix path into clean upper case dos style path string
	static void to_dos (char * dest, const char * src, int len_dest = 0, int len_src = 0);

protected:
	zz_string name_; // path name
	static char cwd_[ZZ_MAX_STRING]; // current working directory

	/// @return index of last slash and -1 if no slash.
	static int get_last_slash (const zz_string& fullpath);

	/// @return index of last slash and -1 if no slash.
	static int get_first_slash (const zz_string& fullpath);
};

#endif // __ZZ_PATH_H__