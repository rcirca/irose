/** 
 * @file zz_log.h
 * @brief error log class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    16-feb-2002
 *
 * $Header: /engine/include/zz_log.h 7     04-06-25 3:03a Zho $
 * $History: zz_log.h $
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-06-25   Time: 3:03a
 * Updated in $/engine/include
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-04-25   Time: 1:18p
 * Updated in $/engine/include
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-04-16   Time: 11:31a
 * Updated in $/engine/include
 * skeleton dummy includes rotation in dummy.
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-02-28   Time: 5:44p
 * Updated in $/engine/include
 * riva tnt2 enabled.
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-02-27   Time: 5:48p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-02-21   Time: 12:06p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 8:05p
 * Created in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:57p
 * Created in $/engine_1/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:53p
 * Created in $/engine/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 7:47p
 * Created in $/engine/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:19p
 * Created in $/Engine/BUILD/msvc71/msvc71.root/msvc71/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 03-11-30   Time: 6:08p
 * Created in $/Engine/INCLUDE
 * 
 * *****************  Version 14  *****************
 * User: Zho          Date: 03-11-28   Time: 2:55p
 * Updated in $/znzin11/engine/include
 * cleanup #include structure
 */

#ifndef	__ZZ_LOG_H__
#define __ZZ_LOG_H__

//#ifdef _DEBUG
//#else
//#define __ZZ_LOG_ERROR_DISABLE__
//#define __ZZ_LOG_DISABLE__
//#endif

//#define ZZ_LOG_USE_ID

#include <stdio.h> // for FILE *

#ifndef __ZZ_TYPE_H__
#include "zz_type.h"
#endif

#ifndef __ZZ_CRITICAL_SECTION_H__
#include "zz_critical_section.h"
#endif

#ifndef __ZZ_ASSERT_H__
#include "zz_assert.h"
#endif

// Message Logging Class
// message file name : "error.txt" defined in "zz_log.cpp"
// no need to initialize
// one instance allowed (_zzlog), but no need to access this directly.
//--------------------------------------------------------------------------------
class zz_log {
protected:
	static char _file_name[ZZ_MAX_STRING];
	static FILE * _fp;
	static unsigned long _error_index; // for debugging
	static bool _active; 
	static zz_critical_section * _cs; // critical section for logging
	static bool _started;

	static void begin ();
	static void end ();

public:
	zz_log ();
	~zz_log ();

	// if false, do not log on file, default to true
	// returns old value
	static bool activate (bool true_or_false);

	/*
	enum {
		ZZ_ERR_NOERROR, // no error
		ZZ_ERR_NOTENOUGHMEMORY, // not enough memory
		ZZ_ERR_UNKNOWN // unknown error
	} zz_error_id;
	*/

	// do message log
	// exam) log("error : file not found (%s).\n", filename);
	static void log (const char * format, ...);
	static void flush ();

	static const char * read_latest_log (int num_latest, const char * linebreaking);
};

#ifndef ZZ_LOG_DISABLE
#define ZZ_LOG zz_log::log
#else
#define ZZ_LOG
#endif // __ZZ_LOG_DISABLE__

#ifndef ZZ_LOG_ERROR_DISABLE
#define ZZ_LOG_ERROR(message) ZZ_LOG("error : " #message " from [%s#%d]\n", __FILE__, __LINE__)
#else
#define ZZ_LOG_ERROR(message)
#endif // __ZZ_LOG_ERROR_DISABLE

// usage :
// ...
// ZZ_LOG("error at line(%d) of file(%s)\n", __LINE__, __FILE__);
// ...

#endif // __ZZ_ERROR_H__
