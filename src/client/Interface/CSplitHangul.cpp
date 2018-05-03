#include "stdafx.h"
#include "CSplitHangul.h"
#include <algorithm>


CSplitHangul::CSplitHangul()
{
	
}

CSplitHangul::CSplitHangul( char* Str, int iLength )
{		
	DoSplit( Str, iLength );
}

CSplitHangul::~CSplitHangul()
{
	m_StringList.clear();
}

bool CSplitHangul::DoSplit( char* Str, int iLength )
{
	if( Str == NULL )
		return false;

	m_iStackDepth = 0;

	if( iLength > MAX_BUF_SIZE )
		return false;	

	SplitHangul( Str, iLength );

	return true;
}
void CSplitHangul::Clear()
{
	m_iStackDepth = 0;
	m_StringList.clear();
}
// 한글을 주어진 크기로 자른다..
bool CSplitHangul::SplitHangul( char* Str, int iLength )
{		
	if( Str == NULL )
		return false;

	m_iStackDepth++;
	if( m_iStackDepth > MAX_STACK_DEPTH )
		return false;

	if( iLength > strlen( Str ) )
	{
		m_StringList.push_back( string( Str ) );
		return false;			
	}

	memcpy( m_TempBuf, Str, iLength );
	//std::remove( &m_TempBuf[0], &m_TempBuf[ iLength ], 20 );


	// 한글의 리드바이트가 아니다..
	if( !IsDBCSTrailByte( m_TempBuf, m_TempBuf + iLength - 1 ) )
	{
		m_TempBuf[ iLength - 1 ] = NULL;
		Str += iLength - 1;	
	}else		
	{
		m_TempBuf[ iLength ] = NULL;
		Str += iLength;	
	}

	m_StringList.push_back( string( m_TempBuf ) );

	SplitHangul( Str, iLength );

	return true;
}