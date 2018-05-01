/** 
 * @file zz_log.cpp
 * @brief log class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /engine/src/zz_log.cpp 41    04-10-19 12:12p Zho $
 */


#ifdef ZZ_MEM_ON
#define ZZ_MEM_ONED // save original mem_on state here.
#endif

#undef ZZ_MEM_ON // logging system does not use memory managing
#include "zz_tier0.h"
#include "zz_log.h"
#include "zz_autolock.h"
#include "zz_path.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <io.h>
#include <process.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <assert.h>

#include "zz_version_info.inc" // To get the ZZ_BUILD_COUNT, ZZ_VERSION_STRING

#include "zz_vfs_local.h"

#include "zz_system.h"
#include "zz_error.h"

#define LOG_FILENAME "error.txt"

//#define ZZ_LOG_EVERY_FLUSH // DO LOG BUFFER FLUSH EVERYTIME

// if you want to log into visual studio debug window, uncomment the following
#ifdef _DEBUG
#define ZZ_LOG_DEBUG_CONSOLE
#else
//#define ZZ_LOG_DEBUG_CONSOLE
#endif

#define MAX_LOG_HISTORY 100 // log only MAX_LOG_HISTORY log lines
#include <queue>

std::queue<std::string> log_history;

//#define ZZ_LOG_LOCK

//--------------------------------------------------------------------------------
unsigned long zz_log::_error_index = 0;

char zz_log::_file_name[ZZ_MAX_STRING] = LOG_FILENAME;

FILE * zz_log::_fp = NULL;
bool zz_log::_active = false;
zz_critical_section * zz_log::_cs = NULL;
bool zz_log::_started = false;

#ifndef ZZ_MEM_ONED
zz_log g_log; // if memory manager is on, g_log will be in zz_mem.
#endif
//--------------------------------------------------------------------------------

zz_log::zz_log ()
{
#ifndef ZZ_LOG_DISABLE
	begin();
#endif
}

zz_log::~zz_log () 
{
#ifndef ZZ_LOG_DISABLE
	end();
#endif
}

void zz_log::begin ()
{
	if (_started) return;

#ifdef ZZ_LOG_LOCK
	assert(!_cs);

	if (!_cs) {
		_cs = new zz_critical_section;
	}
#endif

	activate(true);

#ifndef ZZ_LOG_FILE_OFF
	//if (_access(filename, 0) == 0) { // only if exists
	if (zz_vfs_local::s_get_size(_file_name) > 1000000) { // size check
		_fp = fopen(_file_name, "w");
	}
	else {
        _fp = fopen(_file_name, "a+");
	}
#endif

	_started = true;

#ifdef WIN32
	char time_string[ZZ_MAX_STRING] = "";
	char date_string[ZZ_MAX_STRING] = "";
	char path_string[ZZ_MAX_STRING] = "";
	zz_path::to_unix(path_string, zz_path::get_cwd());
	::GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, "HH':'mm':'ss", time_string, sizeof(time_string));
	::GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, "yyyy/MM/dd", date_string, sizeof(date_string));
	
	ZZ_LOG("\n----------------------------------------------------------------------\n");
	ZZ_LOG("log: start. (%s, %s) from(%s) version(%s). lang(%d)\n", date_string, time_string, path_string, ZZ_VERSION_STRING, zz_error::get_lang_code());
#endif
}


void zz_log::end ()
{
	if (!_started) return;

#ifdef WIN32
	char time_string[ZZ_MAX_STRING] = "";
	char date_string[ZZ_MAX_STRING] = "";
	::GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, "HH':'mm':'ss", time_string, sizeof(time_string));
	::GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, "yyyy/MM/dd", date_string, sizeof(date_string));
	ZZ_LOG("log: end. (%s, %s).\n", date_string, time_string);
#endif

#ifdef ZZ_LOG_LOCK
	assert(_cs);
	if (_cs) {
		delete _cs;
		_cs = NULL;
	}
#endif

#ifndef ZZ_LOG_FILE_OFF
	if (_fp) {
		fclose(_fp);
		_fp = NULL;
	}

#ifdef _DEBUG
	flush();
	//intptr_t ret = _execlp("notepad.exe", "notepad.exe", _file_name, NULL);
#endif
#endif

	_file_name[0] = '\0';
	_active = false;
	_started = false;
}

bool zz_log::activate (bool true_or_false)
{
	static bool old_value = _active;
	_active = true_or_false;
	return old_value;
}

void zz_log::flush ()
{
#ifndef ZZ_LOG_FILE_OFF
	if (_fp) {
		fflush(_fp);
	}
#endif
}

void zz_log::log (const char * format, ...)
{
	//if (!_started) {
	//	begin();
	//}

	if (!_active) return;

#ifdef ZZ_LOG_LOCK
	zz_autolock<zz_critical_section> lockitem(_cs);
#endif

	if (_file_name[0] == '\0') {
		return;
	}

	va_list va;
	va_start(va, format);	

	static char buffer[1024];
	vsprintf(buffer, format, va);
	
#ifdef ZZ_LOG_DEBUG_CONSOLE
	::OutputDebugString(buffer);
	puts(buffer);
#endif

#ifndef ZZ_LOG_FILE_OFF
	if (_fp) {
#ifdef ZZ_LOG_USE_ID
		fprintf(_fp, "%05Ld: ", _error_index++);
#endif
		fputs(buffer, _fp);
		
#ifdef ZZ_LOG_EVERY_FLUSH
		int ret = fflush(_fp);
		assert(0 == ret);
#endif
	}
#endif // ZZ_LOG_FILE_OFF

#ifndef ZZ_MEM_ONED
	if (log_history.size() < MAX_LOG_HISTORY) {
		log_history.push(std::string(buffer));
	}
	else {
		int topop = log_history.size() - MAX_LOG_HISTORY + 1;
		for (int i = 0; i < topop; ++i) {
			log_history.pop();
		}
		assert(log_history.size() < MAX_LOG_HISTORY);
		log_history.push(std::string(buffer));
	}
#endif // ZZ_MEM_ONED

	va_end(va);
}

const char * zz_log::read_latest_log (int num_latest, const char * linebreaking)
{
	static std::string latest_log;

	int total_size = log_history.size();
	int latest_size = total_size;

	if (num_latest != 0) {
		latest_size = ZZ_MIN(total_size, num_latest);
	}

	latest_log.clear();

	int i = 0;
	std::string temp;

	// skip
	for (i = 0; i < (total_size - latest_size); ++i) {
		log_history.pop();
	}

	char system_info_string[255];

	sprintf(system_info_string, "engine: %s, lang: %d%sadapter: %s%s",
		znzin->sysinfo.engine_string, zz_error::get_lang_code(), linebreaking, 
		znzin->sysinfo.video_string, linebreaking);

	latest_log += system_info_string;

	for ( ; i < total_size; ++i) {
		latest_log += log_history.front();

		// log <BR> for HTML
		if (linebreaking && (latest_log.size() > 0)) {
			if (*(latest_log.c_str() + latest_log.size() - 1) == '\n') {
				latest_log += linebreaking;
			}
		}
		log_history.pop();
	}
	return latest_log.c_str();
}

