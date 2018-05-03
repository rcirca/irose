#ifndef _COMMAND_FILTER_
#define _COMMAND_FILTER_

#include "Util/JSingleton.h"
#include "ObjectCommand/ObjCommand.h"


//----------------------------------------------------------------------------------------------------
///
/// class CCommandFilter
/// 서버로 전송할 명령을 필터링 합니다.
///
//----------------------------------------------------------------------------------------------------

class CCommandFilter : public CJSingleton< CCommandFilter >
{
private:
	/// Object command pool
	CObjCommand*		m_ObjCommandPool[ OBJECT_COMMAND_MAX ];
	CObjCommand*		m_pPrevCommand;
	


public:
	CCommandFilter(void);
	~CCommandFilter(void);

	CObjCommand*		GetCommandObject( int iObjectType );
	void				SetPrevCommand( CObjCommand* pCommand ){ m_pPrevCommand = pCommand; }

	bool				CanSendAttackCommand();

	//----------------------------------------------------------------------------------------------------
	/// 스킬관련 명령은 캐스팅이 시작되면 이전스킬명령을 리셋
	//----------------------------------------------------------------------------------------------------
	bool				CanSendSelfSkillCommand( int iSkillSlot );	
	bool				CanSendTargetSkillCommand( int iClientTarget, int iSkillSlot );	
	bool				CanSendPositionSkillCommand( D3DVECTOR &PosTO, int iSkillSlot );

};


#define g_CommandFilter (CCommandFilter::GetSingleton())

#endif //_COMMAND_FILTER_
