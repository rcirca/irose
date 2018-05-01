#ifndef _CREATEAVATA_
#define _CREATEAVATA_

#include "CExternalUI.h"

#include "Net_Prototype.h"
#include "../../../TGameCtrl/TDialog.h"
#include <string>
#include <deque>


///
/// create specific avata
///
struct AvatarSelectItem{
	std::string		m_strValue;
	std::string		m_strIdentify;
};


/**
* 캐릭터 생성시 사용되는 인터페이스
*
*
* @Warning		스크립트와 같이 동작한다.( SystemFunc.lua )
* @Author		최종진
* @Date			2005/9/6
*
*/
class CCreateAvata : public CTDialog
{
public:
	CExternalUIManager*		m_pEUIManager;

	std::string				m_strAvataName;							/// 입력받은 캐릭터 이름
	cli_CREATE_CHAR			m_AvataInfo;							/// 서버에 생성 요청시 보낼 데이타

	/// 스크립트상에서 추가되어 진다.
	std::deque< AvatarSelectItem >	m_mapSex;						/// 캐릭터생성시 선택할수 있는 성별
	std::deque< AvatarSelectItem >	m_mapFace;						/// 캐릭터생성시 선택할수 있는 얼굴형
	std::deque< AvatarSelectItem >	m_mapHair;						/// 캐릭터생성시 선택할수 있는 머리모양
	std::deque< AvatarSelectItem >	m_mapStartPos;					/// 시작위치
	std::deque< AvatarSelectItem >	m_mapBirthStone;				/// 캐릭터생성시 선택할수 있는 탄생석
	
	int	m_iSelectedSex;												/// 선택한 성별
	int m_iSelectedFace;											/// 선택한 얼굴
	int m_iSelectedHair;											/// 선택한 머리모양
	int m_iSelectedStartPos;										/// 시작위치
	int m_iSelectedBirthStone;										/// 선택한 탄생석
public:
	CCreateAvata();
	~CCreateAvata();

	///
	/// set observer class
	///
	void SetEUIObserver( CExternalUIManager* pObserver );
	///
	/// overrided function
	///
	virtual UINT	Process( UINT uiMsg, WPARAM wParam, LPARAM lParam );
	///
	/// overrided function
	///

	virtual void	Show();
	virtual void	Hide();
	virtual void	Draw();
	///
	/// Set Avata Info
	///
	void SetAvataInfo( char* strName, cli_CREATE_CHAR& info );
	///
	/// Create Avata
	///
	void SendCreateAvata( );

	void FailCreateAvata();
	///
	/// Packet receive proc
	///
	bool RecvCreateAvata( t_PACKET* recvPacket );

	void MsgBoxProc(  unsigned int iMsgType, short int iButton );

	void ClearSelectItem( int iType );
	void AddSelectItem( int iType, const char* pszIdentify, const char* pszValue );
	void SelectItem( int iType, unsigned iSubscript );

	enum{
		IID_EDITBOX				= 7,
		IID_BTN_OK				= 10,
		IID_BTN_CANCEL			= 11,
		IID_BTN_LEFT_SEX		= 20,
		IID_BTN_LEFT_FACE		= 21,
		IID_BTN_LEFT_HAIR		= 22,
		IID_BTN_LEFT_STARTPOS	= 23,
		IID_BTN_LEFT_BIRTHSTONE = 24,

		IID_BTN_RIGHT_SEX		 = 30,
		IID_BTN_RIGHT_FACE		 = 31,
		IID_BTN_RIGHT_HAIR		 = 32,
		IID_BTN_RIGHT_STARTPOS	 = 33,
		IID_BTN_RIGHT_BIRTHSTONE = 34,
	};

	enum{
		MSG_TYPE_NORMAL
	};
protected:
	void		OnLButtonUp( unsigned iProcID );
	void		CreateAvata( char* strName, int iRace, int iSex, int iBoneStone, int iHair, int iFace, int iStartPos );	
};


#endif //_CREATEAVATA_