#include "stdafx.h"
#include "cgamedatacreateavatar.h"

#include "../CJustModelAVT.h"
#include "../Game.h"
#include "../Interface/it_mgr.h"
#include "../Interface/CToolTipMgr.h"
#include "../Interface/TypeResource.h"
#include "../System/SystemProcScript.h"

#include "../Country.h"

const float c_AvatarPositions[5][3] = {
										{ 520500, 520500, 100 },
										{ 520270, 520653, 100 },
										{ 520000, 520707, 100 },
										{ 519730, 520653, 100 },
										{ 519500, 520500, 100 }
									};

CGameDataCreateAvatar::CGameDataCreateAvatar(void)
{

}

CGameDataCreateAvatar::~CGameDataCreateAvatar(void)
{

}

CGameDataCreateAvatar& CGameDataCreateAvatar::GetInstance()
{
	static CGameDataCreateAvatar s_Instance;
	return s_Instance;
}

void CGameDataCreateAvatar::Clear()
{
	std::multimap< int , CJustModelAVT* >::iterator iter;

	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); )
	{
		iter->second->RemoveFromScene();
		iter->second->UnloadModelVisible();

		delete iter->second;
		iter = m_avatars_platinum.erase( iter );
	}

	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); )
	{
		iter->second->RemoveFromScene();
		iter->second->UnloadModelVisible();

		delete iter->second;
		iter = m_avatars_premium.erase( iter );
	}

	ClearSelectedAvatar();
}

void CGameDataCreateAvatar::AddCreateAvatar( CJustModelAVT* pAvatar )
{
	assert( pAvatar );

	std::multimap< int , CJustModelAVT* >::reverse_iterator riter;
	int count = 0;
	if( pAvatar->IsPlatinum() )
	{
		pAvatar->SetIndex( m_avatars_platinum.size() + m_avatars_premium.size() );

		m_avatars_platinum.insert( make_pair( pAvatar->GetLevel(), pAvatar ) );
		for( riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend();++riter,++count )
		{
			setPosition( riter->second->GetModelNode(), 
						c_AvatarPositions[count+3][0], 
						c_AvatarPositions[count+3][1], 
						c_AvatarPositions[count+3][2] 
					);
		}
	}
	else
	{
		pAvatar->SetIndex( m_avatars_platinum.size() + m_avatars_premium.size() );
		
		m_avatars_premium.insert( make_pair( pAvatar->GetLevel(), pAvatar ) );
		for( riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend();++riter,++count )
		{
			setPosition( riter->second->GetModelNode(), 
						c_AvatarPositions[count][0], 
						c_AvatarPositions[count][1], 
						c_AvatarPositions[count][2] 
					);
		}
	}
}

void CGameDataCreateAvatar::RemoveCreateAvatar( const char* pszName )
{
	assert( pszName );
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
		{
			iter->second->RemoveFromScene();
			iter->second->UnloadModelVisible();
			delete iter->second;
			iter = m_avatars_platinum.erase( iter );		
			return;
		}
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
		{
			iter->second->RemoveFromScene();
			iter->second->UnloadModelVisible();
			delete iter->second;
			iter = m_avatars_premium.erase( iter );		
			return;
		}
	}



}
HNODE CGameDataCreateAvatar::GetModelNode( const char* pszName )
{
	assert( pszName );
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second->GetModelNode();
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second->GetModelNode();
	}


	return 0;
}

CJustModelAVT*	CGameDataCreateAvatar::FindAvatar( const char* pszName )
{
	if( pszName == NULL ) return NULL;
	std::multimap< int, CJustModelAVT* >::iterator iter;


	for(iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second;
	}

	for(iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter)
	{
		if( iter->second->GetName().compare( pszName ) == 0 )
			return iter->second;
	}



	return NULL;
}

const char* CGameDataCreateAvatar::Pick_AVATAR( long x, long y )
{

	D3DXVECTOR3 RayOrig;
	D3DXVECTOR3 RayDir;
	D3DXVECTOR3 PickPosition;
	float		fPickDistance;

	::getRay (x, y, &RayOrig.x, &RayOrig.y,	&RayOrig.z, &RayDir.x, &RayDir.y,	&RayDir.z );

	std::multimap< int, CJustModelAVT* >::reverse_iterator riter;

	float fContactPointX;
	float fContactPointY;
	float fContactPointZ;
	float fDistance;
	HNODE hNode = pickNode ( x, y, &fContactPointX, &fContactPointY, &fContactPointZ, &fDistance );
	if( hNode )
	{
		for(riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter)
		{
			if( riter->second->GetModelNode() == hNode )
				return riter->second->GetName().c_str();
		}


		for(riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter)
		{
			if( riter->second->GetModelNode() == hNode )
				return riter->second->GetName().c_str();
		}


	}

	CJustModelAVT* pAvt = NULL;
	for(riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter)
	{

		if ( ::intersectRay( riter->second->GetModelNode(),
				RayOrig.x,	RayOrig.y,	RayOrig.z,
				RayDir.x,	RayDir.y,	RayDir.z,
				&PickPosition.x,		&PickPosition.y,		&PickPosition.z,		&fPickDistance) )
		{
			pAvt = riter->second;
			return pAvt->GetName().c_str();
		}
	}

	for(riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter)
	{

		if ( ::intersectRay( riter->second->GetModelNode(),
				RayOrig.x,	RayOrig.y,	RayOrig.z,
				RayDir.x,	RayDir.y,	RayDir.z,
				&PickPosition.x,		&PickPosition.y,		&PickPosition.z,		&fPickDistance) )
		{
			pAvt = riter->second;
			return pAvt->GetName().c_str();
		}
	}


	return NULL;
}

void CGameDataCreateAvatar::SelectAvatar( const char* pszName )
{
	assert( pszName );
	if( FindAvatar( pszName ))
		m_strSelectedAvatarName = pszName;
	else
		_RPT1(_CRT_WARN,"Not Found Avatar(%s) @CGameDataCreateAvatar::SelectAvatar\n", pszName );
}

void CGameDataCreateAvatar::SelectAvatar( int iIndex )
{
	std::multimap< int, CJustModelAVT* >::reverse_iterator riter;
	int iCount = 0 ;

	for( riter = m_avatars_premium.rbegin(); riter != m_avatars_premium.rend(); ++riter,++iCount )
	{
		if( iCount == iIndex )
		{
			SelectAvatar( riter->second->GetName().c_str() );
			break;
		}
	}

	iCount = 3;
	for( riter = m_avatars_platinum.rbegin(); riter != m_avatars_platinum.rend(); ++riter,++iCount )
	{
		if( iCount == iIndex )
		{
			SelectAvatar( riter->second->GetName().c_str() );
			break;
		}
	}

}

void CGameDataCreateAvatar::ClearSelectedAvatar()
{
	m_strSelectedAvatarName.clear();
}

const char*	CGameDataCreateAvatar::GetSelectedAvatarName()
{
	return m_strSelectedAvatarName.c_str();
}

int CGameDataCreateAvatar::GetSelectedAvatarIndex()
{
	if( m_strSelectedAvatarName.empty() )
		return -1;

	std::multimap< int, CJustModelAVT* >::iterator iter;


	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second->GetIndex();
	}


	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second->GetIndex();
	}

	return -1;
}

CJustModelAVT* CGameDataCreateAvatar::GetSelectedAvatar()
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for( iter = m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second;
	}

	for( iter = m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
	{
		if( iter->second->GetName().compare( m_strSelectedAvatarName.c_str() ) == 0 )
			return iter->second;
	}

	return NULL;
}

void CGameDataCreateAvatar::Update()
{
	std::multimap< int, CJustModelAVT* >::iterator iter;

	for( iter =	m_avatars_premium.begin(); iter != m_avatars_premium.end(); ++iter )
		iter->second->Update();

	for( iter =	m_avatars_platinum.begin(); iter != m_avatars_platinum.end(); ++iter )
		iter->second->Update();

	if( m_strSelectedAvatarName.empty() )
		return;
	
	if( CJustModelAVT* pAVT = FindAvatar( m_strSelectedAvatarName.c_str() ) )
	{
		float fPositionXYZ[3];
		float fScreenXYZ[3];
		if( getPosition ( pAVT->GetModelNode(),  fPositionXYZ ) )
		{
			::worldToScreen( fPositionXYZ[0], fPositionXYZ[1], fPositionXYZ[2] , &fScreenXYZ[0], &fScreenXYZ[1], &fScreenXYZ[2] );
			
			
			CInfo Info;
			Info.Clear();
			POINT pt = { fScreenXYZ[0] - 40, fScreenXYZ[1] - 250};
			
			Info.AddString( pAVT->GetName().c_str() ,g_dwYELLOW, g_GameDATA.m_hFONT[ FONT_NORMAL ], DT_CENTER );
			Info.AddString( CStr::Printf( "%s: %d",CStringManager::GetSingleton().GetAbility( AT_LEVEL ), pAVT->GetLevel() ));
			Info.AddString( CStr::Printf( "%s: %s",CStringManager::GetSingleton().GetAbility( AT_CLASS ), CStringManager::GetSingleton().GetJobName( pAVT->GetJob() )));
			
		
			if( pAVT->GetDeleteRemainSEC() )
				Info.AddString( CStr::Printf( "%s: %3.1f%s",
				STR_REMAIN_REMOVE_WAITTIME,
				pAVT->GetDeleteRemainSEC() / 3600.f, STR_HOUR ), g_dwWHITE );

			if( pAVT->IsPlatinum() )
			{
				if(CCountry::GetSingleton().IsJapan() || CCountry::GetSingleton().IsPhilipplines() )
				{
					
				}
				else
				{
					Info.AddString( STR_PLATINUM, g_dwYELLOW );
				}				
			}

			Info.SetPosition( pt );
			CToolTipMgr::GetInstance().RegistInfo( Info );
			CToolTipMgr::GetInstance().Draw();
		}
		
		if( getAnimatableState( pAVT->GetModelNode() ) == 0 )
		{
			CSystemProcScript::GetSingleton().CallLuaFunction( "OnSelectAvatarMotionEnd", 
				ZZ_PARAM_STRING, m_strSelectedAvatarName.c_str(),
				ZZ_PARAM_END );
		}
		
		
	}
}
int	CGameDataCreateAvatar::GetAvatarCount()
{
	return (int)( m_avatars_premium.size() + m_avatars_platinum.size() );
}