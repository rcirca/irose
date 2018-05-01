/** 
 * @file zz_script_simple.h
 * @brief simple script parser class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    28-jan-2003
 *
 * $Header: /engine/include/zz_script_simple.h 7     04-05-19 8:15p Zho $
 * $History: zz_script_simple.h $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-12   Time: 7:02p
 * Updated in $/engine/include
 * Explicit close() in destructor.
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-12-02   Time: 9:20p
 * Updated in $/engine/include
 * do not use zz_vfs_pkg as defaulst filesystem 
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 03-12-02   Time: 6:28p
 * Updated in $/engine/include
 * In type.h, added "zz_vfs.h"
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-01   Time: 5:19p
 * Updated in $/engine/include
 * dump() removed for code explicity
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 5:10p
 * Updated in $/engine/include
 * dump() added
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:58p
 * Updated in $/znzin11/engine/include
 * small code cleanup #include
 */

#ifndef __ZZ_SCRIPT_SIMPLE_H__
#define __ZZ_SCRIPT_SIMPLE_H__

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#include "zz_vfs.h"

#define ZZ_DONOTCHECK "_donotcheck_"

// CAUTION:
// every text mode file should start by "version" string
//

class zz_script_simple
{
private:
	zz_vfs file_system;
	bool text_mode;
	char word_buffer[ZZ_MAX_STRING];

	bool check_element (const char * element_name);
	void preload ();
	void skip_line (void);

public:
	zz_script_simple (void);
	zz_script_simple (const char * file_name,
		zz_vfs::zz_vfs_mode mode = zz_vfs::ZZ_VFS_READ,
		bool text_mode = true);
	~zz_script_simple (void);
	
	// file_system_in should already be opened
	bool open (const char * file_name,
		zz_vfs::zz_vfs_mode open_mode = zz_vfs::ZZ_VFS_READ,
		bool text_mode = true);
	void close ();
	
	// If we have any data named element_name, then return true
	// and if we do not have one, or meet the EOF, then return false.
	// These functions are for easy use. Thus, does not guarantee performance
	// (internally, auto-rewind one word that has been read).
	bool read_uint32 (const char * element_name, uint32& data_out);
	bool read_int32 (const char * element_name, int32& data_out);
	bool read_string (const char * element_name, char * string_out = NULL);
	bool read_float (const char * element_name, float& data_out);
	bool read_float2 (const char * element_name, float& data1_out, float& data2_out);
	bool read_float3 (const char * element_name, float& data1_out, float& data2_out, float& data3_out);
	bool read_float4 (const char * element_name, float& data1_out, float& data2_out, float& data3_out, float& data4_out);

	bool write_int16 (const char * element_name, int16 data_in);
	bool write_uint16 (const char * element_name, uint16 data_in);
	bool write_uint32 (const char * element_name, uint32 data_in);
	bool write_int32 (const char * element_name, int32 data_in);
	bool write_string (const char * element_name, const char * string_in = NULL);
	bool write_float (const char * element_name, float data_in);
	bool write_float2 (const char * element_name, float data1_in, float data2_in);
	bool write_float3 (const char * element_name, float data1_in, float data2_in, float data3_in);
	bool write_float4 (const char * element_name, float data1_in, float data2_in, float data3_in, float data4_in);
	bool write_endl (void);
};

#endif // __ZZ_SCRIPT_SIMPLE_H__