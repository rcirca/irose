/** 
* @file zz_string.h
* @brief string class.
* @author Jiho Choi (zho@korea.com)
* @version 1.0
* @date    12-jun-2002
*
* $Header: /engine/include/zz_string.h 9     04-08-31 4:46p Zho $
* $History: zz_string.h $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-08-31   Time: 4:46p
 * Updated in $/engine/include
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/include
* 
* *****************  Version 7  *****************
* User: Zho          Date: 04-04-15   Time: 11:22a
* Updated in $/engine/include
* 
* *****************  Version 6  *****************
* User: Zho          Date: 04-02-24   Time: 3:39p
* Updated in $/engine/include
* 
* *****************  Version 5  *****************
* User: Zho          Date: 04-02-03   Time: 8:37p
* Updated in $/engine/include
* 
* *****************  Version 4  *****************
* User: Zho          Date: 04-02-03   Time: 8:21p
* Updated in $/engine/include
* Added zz_string::operator const char * ()
* 
* *****************  Version 3  *****************
* User: Zho          Date: 04-01-17   Time: 2:08p
* Updated in $/engine/include
* Added += operator
* 
* *****************  Version 2  *****************
* User: Zho          Date: 03-12-02   Time: 10:46a
* Updated in $/engine/include
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
* *****************  Version 7  *****************
* User: Zho          Date: 03-11-29   Time: 6:28a
* Updated in $/znzin11/engine/include
* check double-backslash
* 
* *****************  Version 6  *****************
* User: Zho          Date: 03-11-28   Time: 6:33p
* Updated in $/znzin11/engine/include
* fixed zz_script_lua.cpp parameter passing error. Do not compile
* zz_script_lua.cpp with /O2 option. Why? I have no idea...
* 
* *****************  Version 5  *****************
* User: Zho          Date: 03-11-28   Time: 2:58p
* Updated in $/znzin11/engine/include
* small code cleanup #include
*/

#ifndef __ZZ_STRING_H__
#define __ZZ_STRING_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#include <string.h>

struct zz_string {
private:
	char * name;
	int length;

public:
	zz_string(void) : name(NULL), length(0)
	{
	}

	explicit zz_string (const char * source_string) : name(NULL), length(0)
	{
		set(source_string);
	}

	zz_string (const zz_string& src_string) : name(NULL), length(0)
	{
		if (src_string.size() == 0) {
			name = NULL;
			length = 0;
			return;
		}
		set(src_string.get());
	}

	zz_string::~zz_string(void)
	{
		reset();
	}

	void reset (void)
	{
		if (name) {
			zz_delete[] name; // do not ZZ_SAFE_DELETE
			name = NULL;
			length = 0;
		}
	}

	int size (void) const
	{
		return length;
	}

	void set (const char * source_string);

	void set ( zz_string& source_string )
	{
		set(source_string.get());
	}

	const char * get (void) const
	{
		return name;
	}

	zz_string& operator=( const char * source_string)
	{
		set(source_string);
		return *this;
	}

	zz_string& operator=( const zz_string& rhs)
	{
		if (&rhs != this) {
			set(rhs.get());
		}
		else {
			int i = 0;
		}
		return *this;
	}

	zz_string& operator+=( const char * source_string );
	zz_string& operator+=( const zz_string& rhs );

	bool operator== (const zz_string& rhs) const
	{
		if (!name) return (rhs.get() == NULL);
		return (strcmp(name, rhs.get()) == 0);
	}

	bool operator!= (const zz_string& rhs) const
	{
		if (!name) return (rhs.get() != NULL);
		return (strcmp(name, rhs.get()) != 0);
	}

	bool operator== (const char * rhs) const
	{
		if (!name) return (rhs == NULL);
		return (strcmp(name, rhs) == 0);
	}

	bool operator!= (const char * rhs) const
	{
		if (!name) return (rhs != NULL);
		return (strcmp(name, rhs) != 0);
	}
};

// Following is not recommended for its performance.
//inline const zz_string operator+(const zz_string& s1, const zz_string& s2)
//{
//	zz_string s3(s1);
//	s3 += s2;
//	return s3;
//}

// Miscellaneous utility class
// This utility class converts slash(or backslash)-ed path string into backslash(or slash)-ed path string.
// This filters double-backslash, also.
class zz_slash_converter {
protected:
	char _str [ZZ_MAX_STRING];
	enum {
		SLASH = '/',
		BACKSLASH = '\\'
	};

public:
	zz_slash_converter(const char * str, bool to_backslash = true, int lower1_upper2 = 0) {
		uint32 size = (uint32)strlen(str);
		uint32 current;
		uint32 i;
		bool last_slashed = false;

		for (i = 0, current = 0; i < size; i++) {
			if (str[i] == SLASH || str[i] == BACKSLASH) {
				if (last_slashed) {
					continue;
				}
				_str[current++] = to_backslash ? BACKSLASH : SLASH;
				last_slashed = true;
			}
			else { // non-slash
				_str[current++] = str[i];
				last_slashed = false;
			}
		}
		_str[current] = '\0';
	}

	~zz_slash_converter()
	{
		_str[0] = '\0'; // verification not to use this invalid string
	}

	operator const char * () const
	{
		return _str;
	}
	operator char * () const
	{
		return (char *)_str;
	}
	char * get () const
	{
		return (char *)_str;
	}
};


#endif // __ZZ_STRING_H__