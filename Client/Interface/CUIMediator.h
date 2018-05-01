#ifndef _UIMEDIATOR_
#define _UIMEDIATOR_

#include "CChatBox.h"
#include "CNameBox.h"
#include "CDigitEffect.h"
#include "PersonalStoreManager.h"

#include "..\\CObjCHAR.h"

#include <vector>
#include <string>


/**
* 아바타를 기준(Z값)으로 그려지는 인터페이스들을 표시하는 Class
* 
* @Author		이동현,최종진
*
* @Date			2005/9/5
*/
class CUIMediator
{
private:
	CChatBoxManager					m_ChatBoxMgr;				/// 아바타 머리위 채팅글 표시 박스
	
	CNameBox						m_NameBox;					/// 아바타 이름및 길드네임, HP게이지등..

	CDigitEffect					m_DigitEffect;				/// 데미지, 크리티컬, 미스등 이펙트
	
	CPersonalStoreManager			m_PersonalStoreManager;		/// 개인상점 간판 관리자

	HNODE							m_LogoTex;					/// 게임로고
	//std::string					m_strZoneName;

public:
	CUIMediator();
	~CUIMediator();


	bool	InitMediator();	
	void	FreeResource();

	void	Update();
	void    Draw();
	void	DrawAvataName( CObjCHAR* pCharOBJ );


	void	AddChatMsg( int iCharIndex, const char* pMsg, DWORD Color = D3DCOLOR_ARGB( 255, 0, 0, 0) );
	void	DrawChatBox(){ m_ChatBoxMgr.Draw(); };	
	void	CreateDamageDigit( int iPoint, float x, float y, float z, bool bIsUSER );


	//void	SetZoneName( const char* pszName );
	//void	DrawZoneName();


	//----------------------------------------------------------------------------------------------------	
	/// 개인상점 간판
	//----------------------------------------------------------------------------------------------------
	void	AddPersonalStoreIndex( int iObjIDX );
	void	SubPersonalStoreIndex( int iObjIDX );
	void	ResetPersonalStore();


protected:

	int		m_iLogoWidth;
	int		m_iLogoHeight;
};

extern CUIMediator	g_UIMed;

#endif //_UIMEDIATOR_