/** 
 * @file zz_string.cpp
 * @brief string class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    12-jun-2002
 *
 * $Header: /engine/src/zz_string.cpp 7     04-08-11 6:20p Zho $
 * $History: zz_string.cpp $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-08-11   Time: 6:20p
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-27   Time: 5:49p
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-01-17   Time: 2:08p
 * Updated in $/engine/src
 * Added += operator
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-09   Time: 7:42p
 * Updated in $/engine/src
 * added thread-based texture loading and acceleration-based camera
 * following system(buggable)
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-02   Time: 3:57p
 * Updated in $/engine/src
 * added name(NULL), length(0) in zz_string(const zz_string& src)
 * constructor.
 * the constructor did not initialize name and length, but used these
 * values in set() method.
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
 * *****************  Version 12  *****************
 * User: Zho          Date: 03-11-29   Time: 6:28a
 * Updated in $/znzin11/engine/source
 * check double-backslash
 * 
 * *****************  Version 11  *****************
 * User: Zho          Date: 03-11-28   Time: 6:33p
 * Updated in $/znzin11/engine/source
 * fixed zz_script_lua.cpp parameter passing error. Do not compile
 * zz_script_lua.cpp with /O2 option. Why? I have no idea...
 * 
 * *****************  Version 10  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <string.h>
#include "zz_mem.h"
#include "zz_string.h"

void zz_string::set (const char * source_string)
{
	if (!source_string) { // null string
		reset();
		return;
	}

	if (length || name) {
		reset();
	}

	length = int(strlen(source_string));
	if (length == 0) return; // do nothing
	name = zz_new char[length+1];
	strcpy(name, source_string);
}

zz_string& zz_string::operator+=( const char * source_string )
{
	int right_length = strlen(source_string);
	if (right_length == 0) return *this; // do nothing
	int new_length = this->length + right_length;
	char * new_name = zz_new char[new_length + 1];
	strncpy(new_name, name, this->length);
	strncpy((new_name + this->length), source_string, right_length);
	new_name[new_length] = '\0';
	reset();
	name = new_name;
	length = new_length;
	return *this;
}

zz_string& zz_string::operator+=( const zz_string& rhs)
{
	if (&rhs != this) {
		if (rhs.length == 0) return *this; // do nothing
		int new_length = this->length + rhs.length;
		char * new_name = zz_new char[new_length + 1];
		strncpy(new_name, name, this->length);
		strncpy((new_name + this->length), rhs.name, rhs.length);
		new_name[new_length] = '\0';
		reset();
		name = new_name;
		length = new_length;
	}
	return *this;
}	
