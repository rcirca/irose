#include "stdafx.h"
#include ".\cclientstorage.h"
#include "Interface/Dlgs/ChattingDLG.h"
#include "interface/Dlgs/QuickToolBar.h"
#include "Game.h"
#include "Common/IO_STB.H"
#include "IO_Terrain.h"
#include "../Country.h"




CClientStorage	g_ClientStorage;
const   char	g_szIniFileName[] = "./Sevenhearts.ini";
const   long	g_ListBgmVolume[MAX_BGM_VOLUME] = { -10000,-3000,-1000,-500,-100,-50,-10,-5,0 };
const   long	g_ListEffectVolume[MAX_EFFECT_VOLUME] = { -10000,-3000,-1000,-500,-100,-50,-10,-5,0 };
//const   float	c_GammaValues[MAX_GAMMA_COUNT] = {0.5, 0.7, 1.0, 1.2, 1.5 };
const   int		c_iPeformances[MAX_PERFORMANCE_COUNT] = { 5,4,3,2,1 };

CClientStorage::CClientStorage(void)
{
	ZeroMemory( &m_VideoOption, sizeof( t_OptionVideo));

	m_VideoOption.iUseRoughMap = 1;

	m_bShowPartyMemberHpGuage = true;
	m_bHasSavedDialogPos	= false;
	m_iQuickBarDlgType		= CQuickBAR::TYPE_HORIZONTAL;
#ifdef _NEW_UI // 2nd SkillBar
	m_iQuickBarExtDlgType	= CQuickBAR::TYPE_HORIZONTAL;
#endif
//	m_iChatDlgType			= CChatDLG::LIST_TYPE_SMALL;
}

CClientStorage::~CClientStorage(void)
{

}

long CClientStorage::GetBgmVolumeByIndex( int i )
{
	if( i < 0 || i >= MAX_BGM_VOLUME )
		return g_ListBgmVolume[DEFAULT_BGM_VOLUME];

	return g_ListBgmVolume[ i ];
}

long CClientStorage::GetEffectVolumeByIndex( int i )
{
	if( i < 0 || i >= MAX_EFFECT_VOLUME )
		return g_ListEffectVolume[ DEFAULT_EFFECT_VOLUME ];
	return g_ListEffectVolume[ i ];
}

void CClientStorage::Load()
{
	///JAPAN ROUTE
	m_iJapanRoute = GetPrivateProfileInt("JAPAN","ROUTE", 1, g_szIniFileName );

	///KeyBoard
	///일본일경우 디폴트를 기존 방식( 자동엔터 )으로 한다.
	if( CCountry::GetSingleton().IsJapan() )
		m_KeyboardOption.iChattingMode = GetPrivateProfileInt("KEYBOARD","CHATMODE", 1, g_szIniFileName );
	else
		m_KeyboardOption.iChattingMode = GetPrivateProfileInt("KEYBOARD","CHATMODE", 0, g_szIniFileName );

	///LastConnected ID
	if( GetPrivateProfileInt("LASTCONNECT","SAVE",0, g_szIniFileName ) )
		m_bSaveLastConnectID = true;
	else
		m_bSaveLastConnectID = false;

	char szBuffer[64];ZeroMemory( szBuffer, sizeof( szBuffer) );
	if( GetPrivateProfileString("LASTCONNECT","ID","",szBuffer,sizeof(szBuffer), g_szIniFileName ) )
	{
		if( szBuffer == NULL )	
			m_strLastConnectID.c_str();
		else
			m_strLastConnectID = szBuffer;
	}



	if( GetPrivateProfileInt("PARTY","SHOWHPGUAGE",1,g_szIniFileName ) )
		m_bShowPartyMemberHpGuage = true;
	else
		m_bShowPartyMemberHpGuage = false;


	m_VideoOption.tResolution.iWidth    = GetPrivateProfileInt( "RESOLUTION","WIDTH", c_iDefaultScreenWidth, g_szIniFileName );
	m_VideoOption.tResolution.iHeight    = GetPrivateProfileInt( "RESOLUTION","HEIGHT", c_iDefaultScreenHeight, g_szIniFileName );
	m_VideoOption.tResolution.iDepth     = GetPrivateProfileInt( "RESOLUTION","DEPTH", c_iDefaultColorDepth, g_szIniFileName );
	m_VideoOption.tResolution.iFrequency = GetPrivateProfileInt( "RESOLUTION","FREQUENCY", c_iDefaultFrequency, g_szIniFileName );


	m_VideoOption.iCamera		= GetPrivateProfileInt("VIDEO","Camera",g_iDefaultCamera,g_szIniFileName );
	m_VideoOption.iPerformance	= GetPrivateProfileInt("VIDEO","PERFORMANCE",g_iDefaultPerfromance,g_szIniFileName );
	m_VideoOption.iFullScreen	= GetPrivateProfileInt("VIDEO","FULLSCREEN",g_iDefaultFullScreen,g_szIniFileName );
	m_VideoOption.iAntiAlising	= GetPrivateProfileInt("VIDEO","ANTIALISING",g_iDefaultAntiAlising,g_szIniFileName );


	m_SoundOption.iBgmVolume = GetPrivateProfileInt("SOUND","BGMVOLUME",DEFAULT_BGM_VOLUME,g_szIniFileName );
	m_SoundOption.iEffectVolume = GetPrivateProfileInt("SOUND","EFFECTVOLUME",DEFAULT_EFFECT_VOLUME,g_szIniFileName );


	m_PlayOption.uiControlType = GetPrivateProfileInt("PLAY","CONTROL",c_iDefaultControlType,g_szIniFileName );
	m_PlayOption.iShowPcName   = GetPrivateProfileInt("PLAY","SHOWPCNAME",c_iDefaultShowName,g_szIniFileName );
	m_PlayOption.iShowNpcName  = GetPrivateProfileInt("PLAY","SHOWNPCNAME",c_iDefaultShowName,g_szIniFileName );
	m_PlayOption.iShowMobName  = GetPrivateProfileInt("PLAY","SHOWMOBNAME",c_iDefaultShowName,g_szIniFileName );

	m_CommunityOption.iWhisper   = GetPrivateProfileInt("COMMUNITY","WHISPER",1,g_szIniFileName );
	m_CommunityOption.iAddFriend = GetPrivateProfileInt("COMMUNITY","ADDFRIEND",1,g_szIniFileName );
	m_CommunityOption.iExchange  = GetPrivateProfileInt("COMMUNITY","EXCHANGE",1,g_szIniFileName );
	m_CommunityOption.iParty     = GetPrivateProfileInt("COMMUNITY","PARTY",1,g_szIniFileName );
	m_CommunityOption.iMessanger = GetPrivateProfileInt("COMMUNITY","MESSANGER",1,g_szIniFileName );

	
	/// 각 Dialog들의 위치
	ZeroMemory( m_DialogPos, sizeof( m_DialogPos ));
	if( GetPrivateProfileStruct( "DIALOG", "POSITION", m_DialogPos, sizeof( m_DialogPos ), g_szIniFileName ) )
		m_bHasSavedDialogPos = true;
	else
		m_bHasSavedDialogPos = false;

	if( m_DialogPos[DLG_TYPE_MAX].nPosX != -999 || m_DialogPos[ DLG_TYPE_MAX].nPosY != -999 )
		m_bHasSavedDialogPos = false;
	

	m_iQuickBarDlgType = GetPrivateProfileInt("DIALOG","QUICKBARDLGTYPE", CQuickBAR::TYPE_HORIZONTAL, g_szIniFileName );
#ifdef _NEW_UI // 2nd Skill Bar
	m_iQuickBarExtDlgType = GetPrivateProfileInt("DIALOG","QUICKBAREXTDLGTYPE", CQuickBAR::TYPE_HORIZONTAL, g_szIniFileName );
#endif
}

void CClientStorage::LoadAvatarData()
{
//	GetPrivateProfileStruct( m_strAvatarName.c_str(),"INVENTORY", m_VirtualInventory, sizeof( m_VirtualInventory ), g_szIniFileName );
}
void CClientStorage::SaveJapanRoute()
{
	WritePrivateProfileString("JAPAN","ROUTE", CStr::Printf("%d", m_iJapanRoute), g_szIniFileName );
}
void CClientStorage::Save()
{
	char szTemp[512] = {0};

	SaveOptionLastConnectID();
	///Japan Route
	SaveJapanRoute();
	///Keyboard
	if( m_KeyboardOption.iChattingMode )
		WritePrivateProfileString("KEYBOARD","CHATMODE", "1", g_szIniFileName );
	else
		WritePrivateProfileString("KEYBOARD","CHATMODE", "0", g_szIniFileName );
	///party
	if( m_bShowPartyMemberHpGuage )
		WritePrivateProfileString("PARTY","SHOWHPGUAGE", "1", g_szIniFileName );
	else
		WritePrivateProfileString("PARTY","SHOWHPGUAGE", "0", g_szIniFileName );

	///RESOLUTION
	itoa( m_VideoOption.tResolution.iWidth, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION","WIDTH", szTemp,g_szIniFileName );

	itoa( m_VideoOption.tResolution.iHeight, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION","HEIGHT", szTemp,g_szIniFileName );

	itoa( m_VideoOption.tResolution.iDepth, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION","DEPTH", szTemp,g_szIniFileName );

	itoa( m_VideoOption.tResolution.iFrequency, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION","FREQUENCY", szTemp,g_szIniFileName );


	itoa( m_VideoOption.iCamera, szTemp, 10 );
	WritePrivateProfileString("VIDEO","Camera", szTemp,g_szIniFileName );

	itoa( m_VideoOption.iPerformance, szTemp, 10 );
	WritePrivateProfileString("VIDEO","PERFORMANCE", szTemp,g_szIniFileName );

	itoa( m_VideoOption.iFullScreen, szTemp, 10 );
	WritePrivateProfileString("VIDEO","FULLSCREEN", szTemp,g_szIniFileName );

	// <AntiAlising>
	itoa( m_VideoOption.iAntiAlising, szTemp, 10 );
	WritePrivateProfileString("VIDEO","ANTIALISING", szTemp,g_szIniFileName );
	// </AntiAlising>

	itoa( m_SoundOption.iBgmVolume, szTemp, 10 );
	WritePrivateProfileString("SOUND","BGMVOLUME", szTemp,g_szIniFileName );

	itoa( m_SoundOption.iEffectVolume, szTemp, 10 );
	WritePrivateProfileString("SOUND","EFFECTVOLUME", szTemp,g_szIniFileName );


	/// Play
	itoa( m_PlayOption.uiControlType,szTemp, 10 );
	WritePrivateProfileString("PLAY","CONTROL", szTemp, g_szIniFileName );

	itoa( m_PlayOption.iShowPcName, szTemp, 10 );
	WritePrivateProfileString("PLAY","SHOWPCNAME", szTemp, g_szIniFileName );

	itoa( m_PlayOption.iShowNpcName, szTemp, 10 );
	WritePrivateProfileString("PLAY","SHOWNPCNAME", szTemp, g_szIniFileName );

	itoa( m_PlayOption.iShowMobName, szTemp, 10 );
	WritePrivateProfileString("PLAY","SHOWMOBNAME", szTemp, g_szIniFileName );


	///<-community option
	itoa( m_CommunityOption.iWhisper, szTemp, 10 );
	WritePrivateProfileString("COMMUNITY","WHISPER", szTemp,g_szIniFileName );


	itoa( m_CommunityOption.iAddFriend,szTemp, 10 );
	WritePrivateProfileString("COMMUNITY","ADDFRIEND", szTemp,g_szIniFileName );

	itoa( m_CommunityOption.iExchange, szTemp,10 );
	WritePrivateProfileString("COMMUNITY","EXCHANGE", szTemp,g_szIniFileName );

	itoa( m_CommunityOption.iParty, szTemp, 10 );
	WritePrivateProfileString("COMMUNITY","PARTY", szTemp,g_szIniFileName );

	itoa( m_CommunityOption.iMessanger, szTemp, 10 );
	WritePrivateProfileString("COMMUNITY","MESSANGER", szTemp, g_szIniFileName );


	///->

//	WritePrivateProfileStruct(m_strAvatarName.c_str(),"INVENTORY", m_VirtualInventory, sizeof( m_VirtualInventory ),g_szIniFileName );

	m_DialogPos[DLG_TYPE_MAX].nPosX = -999;///끝까지 모든 Dialog들의 위치가 저장되어 있는가를 판단하기 위한 값
	m_DialogPos[DLG_TYPE_MAX].nPosY = -999;
	WritePrivateProfileStruct("DIALOG","POSITION", m_DialogPos, sizeof( m_DialogPos ),g_szIniFileName );
	m_bHasSavedDialogPos = true;


	itoa( m_iQuickBarDlgType, szTemp,10 );
	WritePrivateProfileString("DIALOG","QUICKBARDLGTYPE", szTemp, g_szIniFileName );

	itoa( m_iChatDlgType, szTemp, 10 );
	WritePrivateProfileString("DIALOG","CHATDLGTYPE", szTemp, g_szIniFileName );

#ifdef _NEW_UI // 2nd skill bar
	_itoa( m_iQuickBarDlgType, szTemp,10 );
	WritePrivateProfileString("DIALOG","QUICKBAREXTDLGTYPE", szTemp, g_szIniFileName );
#endif
}

void CClientStorage::SetVideoOption( t_OptionVideo& option )
{
	m_VideoOption = option;
}

void CClientStorage::GetVideoOption( t_OptionVideo& option )
{
	option = m_VideoOption;
}

void CClientStorage::SetSoundOption( t_OptionSound& option )
{
	m_SoundOption = option;
}

void CClientStorage::GetSoundOption( t_OptionSound& option )
{
	option = m_SoundOption;
}


short CClientStorage::GetSavedDialogPosX( int iDialogID )
{
	if( iDialogID < 1 || iDialogID >= DLG_TYPE_MAX )
		return 0;

	return m_DialogPos[ iDialogID ].nPosX;
}

short CClientStorage::GetSavedDialogPosY( int iDialogID )
{
	if( iDialogID < 1 || iDialogID >= DLG_TYPE_MAX )
		return 0;

	return m_DialogPos[ iDialogID ].nPosY;
}

void  CClientStorage::SetSavedDialogPos( int iDialogID ,POINT pt )
{

	if( iDialogID < 1 || iDialogID >= DLG_TYPE_MAX )
		return ;

	m_DialogPos[ iDialogID ].nPosX = (short)pt.x;
	m_DialogPos[ iDialogID ].nPosY = (short)pt.y;
}

void CClientStorage::ApplyCameraOption( short i )
{
	/// 현재 존의 카메라셋으로 변환
	if( g_pTerrain )
	{
		int iZoneType = g_pTerrain->GetZoneNO();
		i += ZONE_CAMERA_TYPE( iZoneType );
	}

	if( i >= 0 && i < g_TblCamera.m_nDataCnt )
	{		
		float fFogNear = CAMERA_NEAR_ALPHA_FOG(i) * 100;
		float fFogFar = CAMERA_FAR_ALPHA_FOG(i) * 100;
		::setAlphaFogRange( fFogNear, fFogFar );

		fFogNear = CAMERA_NEAR_FOG(i) * 100;
		fFogFar = CAMERA_FAR_FOG(i) * 100;
		::setFogRange( fFogNear, fFogFar );


		HNODE camera = findNode( "avatar_camera" );
		setCameraAspectRatio( camera, atof(CAMERA_ASPECT_RATIO(i)) );
		setCameraPerspective( camera, CAMERA_FOV(i), atof(CAMERA_ASPECT_RATIO(i)), CAMERA_NEAR_PLANE(i) * 100, CAMERA_FAR_PLANE(i) * 100);
		g_GameDATA.m_nSightRange = CAMERA_MAX_RANGE(i) / 10;
	}
}

void CClientStorage::SetPlayOption( t_OptionPlay& option )
{
	m_PlayOption = option;
}

void CClientStorage::GetPlayOption( t_OptionPlay& option )
{
	option = m_PlayOption;
}

UINT CClientStorage::GetControlType()
{ 
	return m_PlayOption.uiControlType; 
}

void CClientStorage::SetCommunityOption( t_OptionCommunity& option )
{
	m_CommunityOption = option;
}

void CClientStorage::GetCommunityOption( t_OptionCommunity& option )
{
	option = m_CommunityOption;
}

bool CClientStorage::IsApproveAddFriend()
{
	return (m_CommunityOption.iAddFriend)?true:false;
}

bool CClientStorage::IsApproveWhisper()
{
	return (m_CommunityOption.iWhisper)?true:false;
}

bool CClientStorage::IsApproveMessanger()
{
	return (m_CommunityOption.iMessanger)?true:false;
}

bool CClientStorage::IsApproveParty()
{
	return (m_CommunityOption.iParty)?true:false;
}

bool CClientStorage::IsApproveExchange()
{
	return (m_CommunityOption.iExchange)?true:false;
}
bool CClientStorage::IsShowNpcName()
{
	return (m_PlayOption.iShowNpcName )? true:false;
}
bool CClientStorage::IsShowPcName()
{
	return (m_PlayOption.iShowPcName )? true:false;
}
bool CClientStorage::IsShowMobName()
{
	return (m_PlayOption.iShowMobName )? true:false;
}

void CClientStorage::GetInventoryData( const char* pszName, std::list<S_InventoryData>& list )
{
	assert( pszName );
	if( pszName )
	{
		short Inventory[ MAX_INV_TYPE ][ INVENTORY_PAGE_SIZE ];
		ZeroMemory( Inventory, sizeof( Inventory ) );
		if( GetPrivateProfileStruct( pszName,"INVENTORY", Inventory, sizeof( Inventory ), g_szIniFileName ) )
		{
			list.clear();
			S_InventoryData Data;
			for( int iType = 0; iType < MAX_INV_TYPE; ++iType )
			{
				for( int iSlot = 0; iSlot < INVENTORY_PAGE_SIZE ; ++iSlot )
				{
					if( Inventory[iType][iSlot] )
					{
						Data.lVirtualIndex	= iType * INVENTORY_PAGE_SIZE + iSlot;
						Data.lRealIndex		= Inventory[iType][iSlot];
						list.push_back( Data );
					}
				}
			}
		}
	}
}

void CClientStorage::SetInventoryData( const char* pszName, std::list<S_InventoryData>& list )
{
	assert( pszName );
	if( pszName )
	{
		short	Inventory[ MAX_INV_TYPE ][ INVENTORY_PAGE_SIZE ];
		ZeroMemory( Inventory, sizeof( Inventory ) );
		std::list<S_InventoryData>::iterator iter;
		int iType = 0;
		int iSlot = 0;
		for( iter = list.begin(); iter != list.end(); ++iter )
		{
			iType = iter->lVirtualIndex / INVENTORY_PAGE_SIZE;
			iSlot = iter->lVirtualIndex % INVENTORY_PAGE_SIZE;
			assert( iType >= 0 && iType < MAX_INV_TYPE );
			assert( iSlot >= 0 && iSlot < INVENTORY_PAGE_SIZE );

			if( iType < 0 || iType >= MAX_INV_TYPE )
				continue;

			if( iSlot < 0 || iSlot >= INVENTORY_PAGE_SIZE )
				continue;

			Inventory[ iType ][ iSlot ] = iter->lRealIndex;
		}
		WritePrivateProfileStruct( pszName,"INVENTORY", Inventory, sizeof( Inventory ),g_szIniFileName );
	}
}

bool CClientStorage::IsShowPartyMemberHpGuage()
{
	return m_bShowPartyMemberHpGuage;
}

void CClientStorage::SetShowPartyMemberHpGuage( bool b )
{
	m_bShowPartyMemberHpGuage = b;
}

bool CClientStorage::IsSaveLastConnectID()
{
	return m_bSaveLastConnectID;
}

void CClientStorage::SaveLastConnectID( const char* pszID )
{
	if( pszID == NULL )
		m_strLastConnectID.clear();
	else
		m_strLastConnectID = pszID;
}

const char* CClientStorage::GetLastConnectedID()
{
	return m_strLastConnectID.c_str();
}

void CClientStorage::SetSaveLastConnectID( bool b )
{
	m_bSaveLastConnectID = b;
}

void CClientStorage::SaveOptionLastConnectID()
{
	if( m_bSaveLastConnectID )
		WritePrivateProfileString("LASTCONNECT","SAVE", "1", g_szIniFileName );
	else
		WritePrivateProfileString("LASTCONNECT","SAVE", "0", g_szIniFileName );


	if( !m_bSaveLastConnectID )
		m_strLastConnectID.clear();
	WritePrivateProfileString("LASTCONNECT","ID", m_strLastConnectID.c_str(), g_szIniFileName );
}

void CClientStorage::SetKeyboardOption( t_OptionKeyboard& option )
{
	m_KeyboardOption = option;
}

void CClientStorage::GetKeyboardOption( t_OptionKeyboard& option )
{
	option = m_KeyboardOption;
}

int	 CClientStorage::GetJapanRoute()
{
	return m_iJapanRoute;
}

void CClientStorage::SetJapanRoute( int route )
{
	m_iJapanRoute = route;
}