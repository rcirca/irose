/** 
 * @file zz_error.h
 * @brief error log class
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    03-oct-2004
 *
 * $Header: /engine/include/zz_error.h 3     04-10-03 9:12p Zho $
 * $History: zz_error.h $
 * 
 * *****************  Version 3  *****************
 * User: Zho          Date: 04-10-03   Time: 9:12p
 * Updated in $/engine/include
 * 
 * *****************  Version 2  *****************
 * User: Zho          Date: 04-10-03   Time: 5:22p
 * Updated in $/engine/include
 * 
 * *****************  Version 1  *****************
 * User: Zho          Date: 04-06-22   Time: 9:53a
 * Created in $/engine/include
 * 
 */

#include <map>
#include <string>


class zz_error {
public:
	enum zz_lang_code {
		ENGLISH = 0,
		KOREAN = 1,
		JAPANESE =2,
		MAXLANG = 4,
	};

	enum zz_error_code {
		MSG_ERROR,
		INIT_FAIL,
		CREATE_D3D_FAIL,
		CREATE_FONT_FAIL,
		CREATE_TEXTURE_FAIL,
		MSG_EXIT_D3DERROR,
	};

	zz_error(zz_error_code error_code, zz_lang_code lang_code, const char * error_string);

	static const char * get_text (zz_error_code err_code);
	static zz_lang_code get_lang_code ()
	{
		return lang_code;
	}

private:
	static std::map<zz_error_code, std::string> errormap[MAXLANG];
	static zz_lang_code lang_code;
};


