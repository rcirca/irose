#include "stdafx.h"
#include "CChatBox.h"
#include "..\\Object.h"
#include "..\\Game.h"

#include "CNameBox.h"
#include "../util/Localizing.h"
#include "tgamectrl/SplitHangul.h"
#include <algorithm>


#define CHATBOX_WIDTH			179
#define CHATBOX_HEIGHT_BIG		50
#define CHATBOX_HEIGHT_SMALL	30

#define CHAT_TEXT_WIDTH			28



void SweepChatBox( CChatBox* pBox )
{
	if( pBox != NULL )
	{
		delete pBox;
		pBox = NULL;
	}
}



CChatBoxManager::CChatBoxManager()
{
	for( int i = 0; i < CHATBOX_MAX ; i++ )
		m_BackGroundTex[ i ] = NULL;
}

CChatBoxManager::~CChatBoxManager()
{
	Clear();
}

bool CChatBoxManager::Init( )
{	
	m_BackGroundTex[ CHATBOX_SMALL ] = loadTexture ( "3DData\\Control\\Res\\Chatbox01.tga", 
													"3DData\\Control\\Res\\Chatbox01.tga", 
													1, 
													0 );
	if( m_BackGroundTex[ CHATBOX_SMALL ] == NULL )
	{
#ifdef _DEBUG
			::MessageBox (NULL, "Chatbox load failed !!", "File open error", MB_OK);
#endif
			//실패한 이유를 적어준다..
			return false;
	}

	m_BackGroundTex[ CHATBOX_BIG ] = loadTexture ( "3DData\\Control\\Res\\Chatbox02.tga", 
													"3DData\\Control\\Res\\Chatbox02.tga", 
													1, 
													0 );
	if( m_BackGroundTex[ CHATBOX_BIG ] == NULL )
	{
#ifdef _DEBUG
			g_pCApp->ErrorBOX( "Chatbox load failed !!", "File open error" );
#endif
			//실패한 이유를 적어준다..
			return false;
	}	

	m_ChatBoxPool.clear();
	m_ActiveChatBoxList.clear();

	for( int i = 0; i < MAX_CHAT_BOX ; i++ )
	{
		CChatBox* pBox = new CChatBox();
		m_ChatBoxPool.push_back( pBox );
	}

	return true;
}

void CChatBoxManager::FreeResource()
{
	if( m_BackGroundTex[ CHATBOX_BIG ] != NULL )
	{
		::unloadTexture( m_BackGroundTex[ CHATBOX_BIG ] );
		m_BackGroundTex[ CHATBOX_BIG ] = NULL;
	}

	if( m_BackGroundTex[ CHATBOX_SMALL ] != NULL )
	{
		::unloadTexture( m_BackGroundTex[ CHATBOX_SMALL ] );
		m_BackGroundTex[ CHATBOX_SMALL ] = NULL;
	}
}

void CChatBoxManager::Clear()
{
	std::for_each( m_ChatBoxPool.begin(), m_ChatBoxPool.end(), SweepChatBox );
	std::for_each( m_ActiveChatBoxList.begin(), m_ActiveChatBoxList.end(), SweepChatBox );
}

void CChatBoxManager::AddChat(int iCharIndex,const char* Msg, D3DCOLOR Color )
{
	if( m_ChatBoxPool.empty() )
		return;

	CChatBox* pBox = NULL;

	std::list< CChatBox* >::iterator begin = m_ActiveChatBoxList.begin();

	for(; begin != m_ActiveChatBoxList.end() ; ++begin )
	{
		pBox = *begin;

		/// 이미 다른 말중이라면.. 갱신..
		if( iCharIndex == pBox->GetChatIndex() )
		{
			pBox->SetMember( iCharIndex, Msg, Color );			
			return;
		}
	}

	pBox = m_ChatBoxPool.back();
	m_ChatBoxPool.pop_back();

	pBox->SetMember( iCharIndex, Msg, Color );
	m_ActiveChatBoxList.push_back( pBox );
}


void CChatBoxManager::Draw()
{

	//reinterpret_cast<LPD3DDEVICE>( getDevice () )->SetRenderState( D3DRS_ZWRITEENABLE, false );

	CChatBox* pBox = NULL;

	std::list< CChatBox* >::iterator begin = m_ActiveChatBoxList.begin();

	for(; begin != m_ActiveChatBoxList.end() ; )
	{
		pBox = *begin;

		if( pBox->Draw( m_BackGroundTex ) == false )
		{
			m_ChatBoxPool.push_back( pBox );
			begin = m_ActiveChatBoxList.erase( begin );
		}else
			++begin;
	}

	//reinterpret_cast<LPD3DDEVICE>( getDevice () )->SetRenderState( D3DRS_ZWRITEENABLE, true );
}








CChatBox::CChatBox()
{
	m_iCharIndex = 0;	
	m_Color = D3DCOLOR_ARGB( 255, 0, 0, 0);
}


CChatBox::~CChatBox()
{
	// Destruct
}

void CChatBox::SetMember(int iCharIndex,const char* Msg, DWORD Color )
{
	m_iCharIndex = iCharIndex;
	SetText( Msg );

	m_Color = Color;
}


bool CChatBox::Draw( HNODE* backTexture )
{
	if( m_iDisplayTime <= 0 )
	{
		return false;
	}

	m_iDisplayTime--;

	D3DVECTOR   PosSCR;

	CObjCHAR *pCHAR = g_pObjMGR->Get_ClientCharOBJ( m_iCharIndex, false );	

	if( pCHAR == NULL )
		return false;

	pCHAR->GetScreenPOS (PosSCR);

	
	
	CSplitHangul	splitHAN( (char*)m_strText.c_str(), CHAT_TEXT_WIDTH ,CLocalizing::GetSingleton().GetCurrentCodePageNO());
	short	nCNT   = splitHAN.GetLineCount();
	int iAdjustY = 0;
	if( nCNT >= 2 )
	{
		nCNT = 1;
		iAdjustY = 70;
	}
	else
	{
		nCNT = 0;
		iAdjustY = 50;
	}

	float fPosY = 0;
	switch( pCHAR->Get_TYPE() )
	{
	case OBJ_USER:
	case OBJ_AVATAR:
	case OBJ_NPC:
		fPosY = PosSCR.y - iAdjustY - NAMEBOX_HEIGHT;
		break;

	default:
		fPosY = PosSCR.y - iAdjustY;
		break;
	}

	// Transform	
	D3DXMATRIX mat;	
	D3DXMatrixTranslation( &mat, PosSCR.x - CHATBOX_WIDTH/2, fPosY, PosSCR.z );
	
	::setTransformSprite( mat );
	::drawSprite( backTexture[ nCNT ], 
					NULL,
					NULL,
					&D3DXVECTOR3( 0, 0, 0 ),								
					D3DCOLOR_RGBA( 255, 255, 255, 255 ) );
	

	RECT rt = { 4, 4, CHATBOX_WIDTH, CHATBOX_HEIGHT_BIG };
	int iCharHeight = ::getFontHeight( g_GameDATA.m_hFONT[ FONT_NORMAL ] );
	for( int i = 0; i <= nCNT; i++ )
	{
		SetRect( &rt, 4, 4 + ( iCharHeight + 2 ) * i, CHATBOX_WIDTH, 4 + ( iCharHeight + 2 ) * i +  iCharHeight );
		if( strlen( splitHAN.GetString(i) ) > 0 )
			::drawFont( g_GameDATA.m_hFONT[ FONT_NORMAL ], true, &rt, m_Color, DT_LEFT, (char*)splitHAN.GetString(i) );		
	}

	return true;
}
