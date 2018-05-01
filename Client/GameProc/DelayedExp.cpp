#include "stdafx.h"
#include ".\delayedexp.h"
#include "../interface/it_mgr.h"
#include "../CObjCHAR.h"
#include "../Object.h"
#include "../GameCommon/StringManager.h"


/// for singleton
CDelayedExp _DealyedExp;


CDelayedExp::CDelayedExp(void)
{
}

CDelayedExp::~CDelayedExp(void)
{
}

void CDelayedExp::PushEXPData( int iOwnerIndex, int iAddExp, int iEXP, int iStamina )
{
	EXP_DATA data;
	data.iAddExp        = iAddExp;
	data.iExp			= iEXP;
	data.iGetTime		= g_GameDATA.GetGameTime();
	data.iOwnerIndex	= iOwnerIndex;

	data.iStamina		= iStamina;

	m_ExpData.push_back( data );
}

void CDelayedExp::GetExp( int iOwnerIndex )
{
	std::list< EXP_DATA >::iterator		begin = m_ExpData.begin();

	for( ; begin != m_ExpData.end() ; ++begin )
	{
		EXP_DATA& expData = *begin;

		/// It's the time to apply to avatar.
		if( expData.iOwnerIndex == iOwnerIndex )
		{
			/// 갱신된 내 경험치다...
			/// 순서에 의해 계산결과가 - 가 나올수 있다.. 결국 현재 경험치가 아니라.. 추가된 경험치를 
			/// 처리하는걸로 변경..
			///long lDiff = expData.iExp - g_pAVATAR->GetCur_EXP();

			/// lDiff의 경험치를 획득했다... 메세지출력..
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, expData.iAddExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	

			g_pAVATAR->SetCur_EXP( expData.iExp );
			g_pAVATAR->SetCur_STAMINA( expData.iStamina );

			m_ExpData.erase( begin );
			break;
		}
	}
}

void CDelayedExp::Proc()
{
	std::list< EXP_DATA >::iterator		begin = m_ExpData.begin();

	for( ; begin != m_ExpData.end() ; )
	{
		EXP_DATA& expData = *begin;

		bool bProcessExp = false;

		CObjCHAR* pObjCHAR = g_pObjMGR->Get_CharOBJ( expData.iOwnerIndex, true );
		/// If the object was already dead or out of my sight, apply exp now.
		if( pObjCHAR == NULL )
		{			
			bProcessExp = true;
			begin = m_ExpData.erase( begin );		
		}else
		{
			DWORD dwCurrentTime = g_GameDATA.GetGameTime();
			if( dwCurrentTime - expData.iGetTime > 5000 )
			{
				bProcessExp = true;
				begin = m_ExpData.erase( begin );
			}else
				++begin;
		}

		/// erase exp
		if( bProcessExp )
		{			
			/// lDiff의 경험치를 획득했다... 메세지출력..
			char szMsg[256];
			sprintf( szMsg, STR_GET_EXP, expData.iAddExp );
			g_itMGR.AppendChatMsg( szMsg, IT_MGR::CHAT_TYPE_SYSTEM );	

			g_pAVATAR->SetCur_EXP( expData.iExp );	
			g_pAVATAR->SetCur_STAMINA( expData.iStamina );

		}
	}
}