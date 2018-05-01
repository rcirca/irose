#include "stdafx.h"
#include ".\crequirematerial.h"
#include "../interface/typeresource.h"

CRequireMaterial::CRequireMaterial()
{
	Clear();
}
CRequireMaterial::~CRequireMaterial()
{

}

void CRequireMaterial::Clear()
{ 
	m_iItemClass	= 0;
	m_iItemType		= 0;
	m_iItemNo		= 0;		/// ItemNo혹은 Item종류가 들어갈수 있다.1000보다 작으면 종류, 1000 보다 크면 Type + ItemNo
	m_iRequireCount	= 0;
	m_strName.clear();
}

void CRequireMaterial::SetItemClass( int iClass )
{
	m_iItemClass = iClass;
}

void CRequireMaterial::SetItemType( int iType )
{
	m_iItemType = iType;
}

void CRequireMaterial::SetItemNo( int iNo )
{
	m_iItemNo	= iNo;
}

void CRequireMaterial::SetRequireCount( int iCount )
{
	m_iRequireCount = iCount;
}

void CRequireMaterial::SetName( const char* pszName )
{
	assert( pszName );
	if( pszName == NULL )
	{
		m_strName = STR_NONAME;
		return;
	}
	m_strName = pszName;
}

int CRequireMaterial::GetItemClass()
{
	return m_iItemClass;
}

int	CRequireMaterial::GetItemType()
{
	return m_iItemType;
}

int CRequireMaterial::GetItemNo()
{
	return m_iItemNo;
}

int CRequireMaterial::GetRequireCount()
{
	return m_iRequireCount;
}

const char* CRequireMaterial::GetName()
{
	return m_strName.c_str();
}
bool CRequireMaterial::IsEmpty()
{
	return m_strName.empty();
}