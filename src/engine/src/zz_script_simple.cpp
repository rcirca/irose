/** 
 * @file zz_script_simple.cpp
 * @brief simple script parser class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    28-jan-2003
 *
 * $Header: /engine/src/zz_script_simple.cpp 9     04-05-19 8:15p Zho $
 * $History: zz_script_simple.cpp $
 * 
 * *****************  Version 9  *****************
 * User: Zho          Date: 04-05-19   Time: 8:15p
 * Updated in $/engine/src
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-03-11   Time: 9:30p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-03-10   Time: 10:41a
 * Updated in $/engine/src
 * Not to reopen.
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-02-25   Time: 8:34p
 * Updated in $/engine/src
 * not to overuse static char
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-02-12   Time: 7:02p
 * Updated in $/engine/src
 * Explicit close() in destructor.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-11   Time: 2:05p
 * Updated in $/engine/src
 * Added zz_tier0.h
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 03-12-01   Time: 5:19p
 * Updated in $/engine/src
 * dump() removed for code explicity
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 03-12-01   Time: 5:10p
 * Updated in $/engine/src
 * dump() added
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
 * *****************  Version 5  *****************
 * User: Zho          Date: 03-11-28   Time: 2:56p
 * Updated in $/znzin11/engine/source
 * cleanup #include structure
 */

#include "zz_tier0.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "zz_type.h"
#include "zz_script_simple.h"

#define TEXT_MODE_FIRST_STRING "version"

zz_script_simple::zz_script_simple(void) : text_mode(true) 
{
}

zz_script_simple::zz_script_simple(const char * file_name, zz_vfs::zz_vfs_mode mode, bool text_mode_in)
{
	open(file_name, mode, text_mode_in);
}

zz_script_simple::~zz_script_simple()
{
	close();
}

bool zz_script_simple::open (const char * file_name,
							 zz_vfs::zz_vfs_mode mode,
							 bool text_mode_in)
{
	text_mode = text_mode_in;
	file_system.close();
	if (!file_system.open(file_name, mode)) return false;
	
	if (mode == zz_vfs::ZZ_VFS_READ) { // automatically set text_mode
		preload();
		if (strcmp(this->word_buffer, TEXT_MODE_FIRST_STRING) == 0) {
			text_mode = true;
		}
		else {
			text_mode = false; // force binary
			// TODO: implement rewind method and restart. 
			file_system.seek(0, zz_vfs::ZZ_VFS_SEEK_SET);
		}
	}
	return true;
}

void zz_script_simple::close ()
{
	file_system.close();
}

void zz_script_simple::skip_line (void)
{
	char c;
	int i = 0;

	while (file_system.read_char(c) != 0x0A &&
		file_system.get_status() != zz_vfs::ZZ_VFS_EOF)
	{
		i++;
		//assert(i < ZZ_MAX_STRING);
	}
}

// return false if cannot fill word_buffer
// this function skips comment by "//"
void zz_script_simple::preload (void)
{
	if (text_mode) {
		// pre-load single word
		if (file_system.read_string_without_whitespace(word_buffer) == NULL) {
			word_buffer[0] = '\0';
			return;
		}
		if (strcmp("//", word_buffer) == 0) { // if comment started
			skip_line();
			preload();
		}
	}
	else {
		// do nothing
	}
}

bool zz_script_simple::check_element (const char * element_name)
{
	if (!element_name) return false;

	if (strcmp(element_name, ZZ_DONOTCHECK) == 0) return true;
	if (strcmp(element_name, word_buffer) != 0) return false;
	return true;
}

bool zz_script_simple::read_int32 (const char * element_name, int32& data_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data_out = (int32)atoi(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_int32(data_out);
	return true;
}

bool zz_script_simple::read_uint32 (const char * element_name, uint32& data_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data_out = (uint32)atoi(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_uint32(data_out);
	return true;
}

bool zz_script_simple::read_string (const char * element_name, char * string_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		if (!string_out) return true;
		strcpy(string_out, word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_string(string_out);
	return true;
}

bool zz_script_simple::read_float (const char * element_name, float& data_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data_out = (float)atof(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_float(data_out);
	return true;
}

bool zz_script_simple::read_float2 (const char * element_name,
									float& data1_out,
									float& data2_out
									)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data1_out = (float)atof(word_buffer);
		preload(); // read next element
		data2_out = (float)atof(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_float(data1_out);
	file_system.read_float(data2_out);
	return true;
}

bool zz_script_simple::read_float3 (const char * element_name,
				  float& data1_out, float& data2_out, float& data3_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data1_out = (float)atof(word_buffer);
		preload(); // read value
		data2_out = (float)atof(word_buffer);
		preload(); // read value
		data3_out = (float)atof(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_float(data1_out);
	file_system.read_float(data2_out);
	file_system.read_float(data3_out);
	return true;
}

bool zz_script_simple::read_float4 (const char * element_name,
				  float& data1_out, float& data2_out,
				  float& data3_out, float& data4_out)
{
	if (file_system.get_status() == zz_vfs::ZZ_VFS_EOF) return false;
	if (text_mode) {
		if (element_name) { // check element_name
			if (!check_element(element_name)) return false;
			preload(); // read value
		}
		data1_out = (float)atof(word_buffer);
		preload(); // read value
		data2_out = (float)atof(word_buffer);
		preload(); // read value
		data3_out = (float)atof(word_buffer);
		preload(); // read value
		data4_out = (float)atof(word_buffer);
		preload(); // read next element
		return true;
	}
	// binary mode
	file_system.read_float(data1_out);
	file_system.read_float(data2_out);
	file_system.read_float(data3_out);
	file_system.read_float(data4_out);
	return true;
}


bool zz_script_simple::write_endl (void)
{
	if (text_mode) {
		file_system.write_string("\r\n");
	}
	return true;
}

bool zz_script_simple::write_int16 (const char * element_name, int16 data_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%d ", data_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_int16(data_in);
	}
	return true;
}

bool zz_script_simple::write_uint16 (const char * element_name, uint16 data_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%d ", data_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_uint16(data_in);
	}
	return true;
}

bool zz_script_simple::write_uint32 (const char * element_name, uint32 data_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%d ", data_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_uint32(data_in);
	}
	return true;
}

bool zz_script_simple::write_int32 (const char * element_name, int32 data_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%d ", data_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_int32(data_in);
	}
	return true;
}

bool zz_script_simple::write_string (const char * element_name,
									 const char * string_in)
{
	if (text_mode) {
		if (element_name) {
			file_system.write_string(element_name);
			file_system.write_string(" "); // add space
		}
		if (string_in) {
			file_system.write_string(string_in);
			file_system.write_string(" "); // add space
		}
	}
	else {
		// write null-terminated string
		file_system.write_string(string_in, (uint32)strlen(string_in)+1);
	}
	return true;
}

bool zz_script_simple::write_float (const char * element_name, float data_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%f ", data_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_float(data_in);
	}
	return true;
}

bool zz_script_simple::write_float2 (const char * element_name, 
									 float data1_in,
									 float data2_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%f %f ", data1_in, data2_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_float(data1_in);
		file_system.write_float(data2_in);
	}
	return true;
}

bool zz_script_simple::write_float3 (const char * element_name, 
									 float data1_in,
									 float data2_in,
									 float data3_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%f %f %f ", data1_in, data2_in, data3_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_float(data1_in);
		file_system.write_float(data2_in);
		file_system.write_float(data3_in);
	}
	return true;
}

bool zz_script_simple::write_float4 (const char * element_name,
									 float data1_in,
									 float data2_in,
									 float data3_in,
									 float data4_in)
{
	if (text_mode) {
		char buf[ZZ_MAX_STRING];
		if (element_name) {
			sprintf(buf, "%s ", element_name);
			file_system.write_string(buf);
		}
		sprintf(buf, "%f %f %f %f ",
			data1_in, data2_in, data3_in, data4_in);
		file_system.write_string(buf);
	}
	else {
		file_system.write_float(data1_in);
		file_system.write_float(data2_in);
		file_system.write_float(data3_in);
		file_system.write_float(data4_in);
	}
	return true;
}