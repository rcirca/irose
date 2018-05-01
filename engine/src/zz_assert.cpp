/** 
 * @file zz_assert.cpp
 * @brief assertion.
 * @author Jiho Choi (zho@korea.com)
 * @version 1.0
 * @date    24-jun-2004
 *
 * $Id: zz_visible.h,v 1.1.1.1 2002/09/06 00:44:05 Jiho Choi Exp $
 */

#include "zz_tier0.h"
#include "zz_log.h"
#include "zz_assert.h"
#include "zz_path.h"
#include "zz_string.h"
#include "zz_version_info.inc"

#include <string>
#include <map>
#include <vector>
#include <algorithm>

int zz_assert_shared_struct::s_current_line = 0;
const char * zz_assert_shared_struct::s_current_file = 0;

#ifdef _WIN32
#include <windows.h>
#include "resource.h"

extern HINSTANCE g_hinstDLL;
extern HWND g_hWnd;

const char * popup_dlg_msg = 0;

BOOL CALLBACK PopupDlgProc (HWND hDlg, UINT iMessage, WPARAM wParam, LPARAM lParam)
{
	switch (iMessage)
	{
		case WM_INITDIALOG:
			{
				SetDlgItemText( hDlg, IDC_EDIT_ASSERT, popup_dlg_msg );
			}
			return TRUE;
		case WM_COMMAND:
			switch (wParam)
			{
			case IDIGNORE:
				EndDialog(hDlg, 1);
				return TRUE;
			case IDCANCEL:
				EndDialog(hDlg, 0);
				return TRUE;
			}
			break;
	}
	return FALSE;
}

int zz_popup_message (const char * msg)
{
	//MessageBox( NULL, msg, "engine zz_assert failed", MB_OK | MB_TOPMOST );
	popup_dlg_msg = msg;

	if (0 == g_hinstDLL) {
		MessageBox( NULL, msg, "no window instance", MB_OK | MB_TOPMOST );
		return 0;
	}

	return DialogBox(g_hinstDLL, MAKEINTRESOURCE(IDD_DIALOG_ASSERT), g_hWnd, PopupDlgProc);
}

#endif // _WIN32


// @return 0 for stop application, 1 for ignoring assert
int _zz_assert ( const char * msg, const char * filename, int linenum )
{
	static char buffer[1024];

	static std::map< std::string, std::vector<int> > name_num;


	zz_string onlyname;
	zz_path::extract_name_ext(zz_string(filename), onlyname);

	// search history, and if found, skip this assert
	if (name_num.find(std::string(onlyname.get())) != name_num.end()) { // found
		std::vector<int>& numlist = name_num[onlyname.get()];
		if (std::find(numlist.begin(), numlist.end(), linenum) != numlist.end()) { // found
			return 1; // ignore this assert as this is already in history.
		}
	}

	sprintf(buffer, "%s\r\n\r\n[%s:#%d]\r\nver. %s", msg, onlyname.get(), linenum, ZZ_VERSION_STRING );
	ZZ_LOG("assert: failed. %s\n", buffer);
	zz_log::flush();

	// popup message and get response from user
	if (zz_popup_message( buffer )) { // user selected "ignore" button, log into history
		// insert into history
		name_num[std::string(onlyname.get())].push_back( linenum );
		return 1; // ignore this assert
	}
	return 0; // stop application!
}

void zz_msgbox ( const char * caption, const char * msg )
{
	MessageBox( g_hWnd, msg, caption, MB_OK | MB_TOPMOST );
}

void zz_msgboxf ( const char * caption, const char * msg_format, ... )
{
	static char buffer[1024];
	
	va_list va;
	va_start(va, msg_format);	
	vsprintf(buffer, msg_format, va);
	va_end(va);	

	zz_msgbox(caption, buffer);
}
