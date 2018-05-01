#ifndef _CInfo_
#define _CInfo_

#include "CTString.h"
#include "../game.h"
#include "Interface/SlotContainer/ClanSkillSlot.h"

//#include "JStringParser.h"

const D3DCOLOR	g_dwBlueToolTip = D3DCOLOR_ARGB( 255, 100, 200,255);

#define INFO_STATUS_SIMPLE			0x00010000
#define INFO_STATUS_DETAIL			0x00100000
#define INFO_STATUS_FEW				0x01000000 ///SIMPLE보다 더 적은 정보 ( 단축키바,활성화 슬롯에서의 스킬 정보 표시시 사용 )

#define INFO_ADD_PRICE						0x00000001
#define INFO_ADD_PRICE_SELL					0x00000002
#define INFO_ADD_PRICE_STORAGE_FEE			0x00000004
#define INFO_ADD_PRICE_PRIVATESTORE			0x00000008
#define INFO_ADD_PRICE_AVATARSTORE_BUYTAB	0x00000010
#define INFO_ADD_PRICE_AVATARSTORE_SELLTAB	0x00000020

#define INFO_ADD_PRICE_REPAIR			0x10000000
#define INFO_ADD_PRICE_APPRAISAL		0x20000000

/**
* 툴팁시스템및 드랍되어 있는 아이템의 이름표시( CTRL키가 눌렸을때 )에 사용되는 스트링 표시 Class
*	- 화면밖으로 표시가 나가지 않도록 위치 조정 기능
*	- 글자표시에 따라 이미지의 너비를 조정하는 기능
*
* @Author		최종진
* @Date			2005/9/5
*/
class CInfo
{
public:
	CInfo(void);
	~CInfo(void);
	void Draw();
	void DrawLineImage( int x, int y, int iWidth );
	void Clear();
	void SetPosition( POINT pt );
	int	 GetWidth();
	int  GetHeight();
	int	 GetMaxSizeString();
	void AddString( const char* pszTxt, DWORD color = D3DCOLOR_ARGB( 255,255,255,255 ), HNODE hFont = g_GameDATA.m_hFONT[FONT_NORMAL], UINT uFormat = DT_LEFT );
	void AddString( CTString& TString );
	bool IsEmpty();
//	void SetTitle( char* pszTitle );

protected:
	void AdjustPosition();

protected:
//	CJStringParser	m_Title;
	int			m_iLineImageNo;
	POINT		m_ptPosition;
	int			m_iWidth;
	int			m_iHeight;
	UINT		m_uMaxSizeString;///최대 글자수 
	std::vector< CTString >  m_listString;
	std::string				 m_strTitle;

};
#endif