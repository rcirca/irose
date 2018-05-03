#ifndef _CDialogNpcScriptItem_
#define _CDialogNpcScriptItem_

#include "tgamectrl/winctrl.h"
#include "tgamectrl/JStringParser.h"
/**
* NPC와의 대화시 NPC SCRIPT가 출력되는 ZLISTBOX를 위한 Item Class
*
* @Author		최종진
* @Date			2005/9/15
*/
class CDialogNpcScriptItem : public CWinCtrl
{
public:
	CDialogNpcScriptItem( int iIndex, char* pszScript ,int iImageID, int iLineHeight, int iWidth , int iBorderWidth );
	virtual ~CDialogNpcScriptItem(void);
	virtual void Draw();

protected:
	int	m_iModuleID;
	int m_iImageID;
	int m_iLineCount;///이미지 그릴 라인카운트
	int m_iLineHeight;
	int m_iBorderWidth;

	CJStringParser	m_Script;
};
#endif