#include "stdAFX.h"
#include "CQuest.h"
#include "IO_Quest.h"

#ifndef	__SERVER
#include "..\GameProc\CDayNNightProc.h"
#include "../Game.h"
#include "../interface/it_mgr.h"
#else
extern DWORD Get_WorldPassTIME();
#endif

#define	MAX_ASSOCIATION		20	// 최대 조합 갯수

	// 메인 퀘스트 진행 수치
	// 친분 수치[ MAX_ASSOCIATION ] .	처음 시작 시 : 0
	// 퀘스트 진행 수치[ MAX_ASSOCIATION ]
	/*
	// 진행중인 퀘스트
	{
		퀘스트 수행 변수 10개
		퀘스트 스위치 8개
		퀘스트 타이머 
		퀘스트 제공자 
	}
	*/

//-------------------------------------------------------------------------------------------------
void CQUEST::Init ()
{
	m_wID = 0;
	ClearAllSwitch ();
	::ZeroMemory (m_pVAR,	sizeof(m_pVAR)  );
	::ZeroMemory (m_ITEMs,	sizeof(m_ITEMs) );
}

void CQUEST::CheckExpiredTIME ()
{
	// if ( this->m_wID ) 
	{
		if ( QUEST_TIME_LIMIT( this->m_wID ) ) {
			DWORD dwNewExpired = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT( this->m_wID );
			if ( this->m_dwExpirationTIME > dwNewExpired ) {
				// 서버 통합되면서 케릭이전될때... 서버타임이 틀려지니까...
				this->m_dwExpirationTIME = dwNewExpired;
			}
		}
	}
}

void CQUEST::SetID (WORD wID, bool bResetTimer)
{	
	m_wID = wID;

	if ( bResetTimer ) {
		if ( QUEST_TIME_LIMIT( wID ) ) {
			m_dwExpirationTIME = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT( wID );
		} else
			m_dwExpirationTIME = 0;		// 무제한..
	} else 
	if ( QUEST_TIME_LIMIT( wID ) ) {
		// 리셋이 필요없는 경우인데...바뀌는것이 시간 제한이 필요하면...
		m_dwExpirationTIME = ::Get_WorldPassTIME() + QUEST_TIME_LIMIT( wID );
	}
}

//-------------------------------------------------------------------------------------------------
DWORD CQUEST::GetRemainTIME()
{
	DWORD dwCurTime = ::Get_WorldPassTIME();

	if ( dwCurTime >= m_dwExpirationTIME ) {
		// 남은 시간 없다.
		return 0;
	}

	return ( m_dwExpirationTIME - dwCurTime );
}

//-------------------------------------------------------------------------------------------------
void CQUEST::Set_VAR (int iVarNO, int iValue)
{
    if ( iVarNO < 0 || iVarNO >= QUEST_VAR_PER_QUEST )
        return;

    m_pVAR[ iVarNO ] = iValue;
}
int  CQUEST::Get_VAR (int iVarNO)
{
    if ( iVarNO < 0 || iVarNO >= QUEST_VAR_PER_QUEST )
        return 0;
    return m_pVAR[ iVarNO ];
}


//-------------------------------------------------------------------------------------------------
void CQUEST::ClearAllSwitch ()
{
	::ZeroMemory (m_btSWITCHES, sizeof(m_btSWITCHES));
}
BYTE CQUEST::GetBit (int iIndex)
{
	return ( m_btSWITCHES[ iIndex >> BIT_SHIFT ] & g_btSwitchBitMask[ iIndex & WHICH_BIT ] ) ? 1 : 0;
}
void CQUEST::SetBit (int iIndex)
{
	m_btSWITCHES[ iIndex >> BIT_SHIFT ] |= g_btSwitchBitMask[ iIndex & WHICH_BIT ];
}
void CQUEST::ClearBit (int iIndex)
{
	m_btSWITCHES[ iIndex >> BIT_SHIFT ] &= ~(g_btSwitchBitMask[ iIndex & WHICH_BIT ]);
}
BYTE CQUEST::FlipBit (int iIndex)
{
	if ( this->GetBit( iIndex ) )
		this->ClearBit( iIndex );
	else
		this->SetBit ( iIndex );

	return this->GetBit( iIndex );
}

//-------------------------------------------------------------------------------------------------
void CQUEST::Set_SWITCH (int iSwitchNO, int iValue)
{
    if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST )
        return;

    if ( iValue )
        this->SetBit( iSwitchNO );
    else
        this->ClearBit( iSwitchNO );
}
int  CQUEST::Get_SWITCH (int iSwitchNO)
{
    if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST )
        return -1;

    return this->GetBit( iSwitchNO );
}
int  CQUEST::Flip_SWITCH(int iSwitchNO)
{
    if ( iSwitchNO < 0 || iSwitchNO >= QUEST_SWITCH_PER_QUEST )
        return -1;

    return this->FlipBit( iSwitchNO );
}

//-------------------------------------------------------------------------------------------------
tagBaseITEM* CQUEST::GetSlotITEM( int iSlotNo )
{
	if( iSlotNo < 0 || iSlotNo >= QUEST_ITEM_PER_QUEST )
		return NULL;

	return &(m_ITEMs[ iSlotNo ]);
}
tagBaseITEM*CQUEST::GetSameITEM( WORD wItemHEADER )
{
	for (short nI=0; nI<QUEST_ITEM_PER_QUEST; nI++) {
		if ( this->m_ITEMs[ nI ].GetHEADER() == wItemHEADER )
			return &this->m_ITEMs[ nI ];
	}
	return NULL;
}
bool CQUEST::SubITEM( tagITEM &sITEM )
{
	for (short nI=0; nI<QUEST_ITEM_PER_QUEST; nI++) {
		if ( this->m_ITEMs[ nI ].GetHEADER() == sITEM.GetHEADER() ) 
		{
			tagITEM sOriITEM;
			sOriITEM = this->m_ITEMs[ nI ];
			sOriITEM.SubtractOnly( sITEM );
			this->m_ITEMs[ nI ] = sOriITEM;

#ifndef __SERVER
			g_itMGR.AppendChatMsg( sITEM.SubtractQuestMESSAGE(), IT_MGR::CHAT_TYPE_QUEST );
#endif
			return true;
		}
	}
	return false;
}

//-------------------------------------------------------------------------------------------------
bool CQUEST::AddITEM( tagITEM &sITEM )
{
	short nI;

	if ( sITEM.IsEnableDupCNT() ) {
		// 중복 가능, 같은 아이템이 있는가 ??
		for (nI=0; nI<QUEST_ITEM_PER_QUEST; nI++) {
			if ( this->m_ITEMs[ nI ].GetHEADER() == sITEM.GetHEADER() ) {
				this->m_ITEMs[ nI ].m_uiQuantity += sITEM.GetQuantity();
				return true;
			}
		}
	}
	
	// 빈곳에 추가..
	for (nI=0; nI<QUEST_ITEM_PER_QUEST; nI++) {
		if ( 0 == this->m_ITEMs[ nI ].GetHEADER() ) {
			this->m_ITEMs[ nI ] = sITEM;
			return true;
		}
	}

	return false;
}

//-------------------------------------------------------------------------------------------------
