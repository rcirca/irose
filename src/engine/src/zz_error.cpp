/** 
 * @file zz_gamma.cpp
 * @brief gamma control class.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    14-mar-2003
 *
 * $Header: /engine/src/zz_error.cpp 8     04-10-19 12:07p Zho $
 * $History: zz_error.cpp $
 * 
 * *****************  Version 8  *****************
 * User: Zho          Date: 04-10-19   Time: 12:07p
 * Updated in $/engine/src
 * 
 * *****************  Version 7  *****************
 * User: Zho          Date: 04-10-19   Time: 11:53a
 * Updated in $/engine/src
 * 
 * *****************  Version 6  *****************
 * User: Zho          Date: 04-10-19   Time: 11:52a
 * Updated in $/engine/src
 * 
 * *****************  Version 5  *****************
 * User: Zho          Date: 04-10-19   Time: 11:50a
 * Updated in $/engine/src
 * 
 * *****************  Version 4  *****************
 * User: Zho          Date: 04-10-19   Time: 11:50a
 * Updated in $/engine/src
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-10-12   Time: 10:17p
 * Updated in $/engine/src
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-10-07   Time: 9:49a
 * Updated in $/engine/src
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-10-03   Time: 5:22p
 * Created in $/engine/src
 */

#include "zz_tier0.h"
#include "zz_error.h"

#ifdef WIN32
#include <windows.h>

static zz_error::zz_lang_code get_default_lang_code ()
{
	LANGID langid = GetUserDefaultLangID();

	zz_error::zz_lang_code lang_code = zz_error::ENGLISH;

	if (langid == LANG_SYSTEM_DEFAULT) { // use system default lang id
		langid = GetSystemDefaultLangID();
	}

	switch (PRIMARYLANGID(langid))
	{
	case LANG_KOREAN: // korean
		lang_code = zz_error::KOREAN;
		break;
	case LANG_JAPANESE: // japanese
		lang_code = zz_error::JAPANESE;
		break;
	default:
		lang_code = zz_error::ENGLISH;
	}

	return lang_code;
}

#endif // win32

zz_error::zz_lang_code zz_error::lang_code = get_default_lang_code();
std::map<zz_error::zz_error_code, std::string> zz_error::errormap[zz_error::MAXLANG];


zz_error::zz_error (zz_error_code error_code, zz_lang_code lang_code, const char * error_string)
{
	errormap[lang_code][error_code] = std::string(error_string);
}

const char * zz_error::get_text (zz_error_code error_code)
{
	if (errormap[lang_code].end() == errormap[lang_code].find(error_code)) { // not found
		if (lang_code == ENGLISH) {
			return NULL;
		}
		if (errormap[lang_code].end() == errormap[zz_lang_code::ENGLISH].find(error_code)) { // not found english
			return NULL;
		}
		// found english
		return errormap[ENGLISH][error_code].c_str();
	}
	// found errorcode
	return errormap[lang_code][error_code].c_str();
}

#define ZZ_REG_ERROR( ERCODE, ERLANG, ERMSG ) zz_error err_##ERCODE##ERLANG (zz_error::ERCODE, zz_error::ERLANG, ERMSG);

//#include "zz_error_trans.txt"


