/*
	$History: CBITArray.cpp $
 * 
 * *****************  Version 2  *****************
 * User: Icarus       Date: 04-10-03   Time: 2:44a
 * Updated in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:20a
 * Created in $/7HeartsOnline/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 11:09a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:51a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:47a
 * Created in $/SevenHearts/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:35a
 * Created in $/SHO/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 04-03-25   Time: 10:26a
 * Created in $/SevenHearts/LIB_Util/LIB_Util
 * 
 * *****************  Version 1  *****************
 * User: Icarus       Date: 03-07-03   Time: 11:37a
 * Created in $/LIB_Util
*/
#include "CBITArray.h"
#include <memory.h>


//-------------------------------------------------------------------------------------------------
void CBITARRAY::SetArray (BYTE *pDATA, short nDataByteCnt)
{
	int iMinBytes;

	iMinBytes = ( nDataByteCnt < m_iByteLength ) ? nDataByteCnt : m_iByteLength;
	memcpy (m_pDATA, pDATA, iMinBytes);
}
void CBITARRAY::ClearAll (void)
{
	memset (m_pDATA, 0, m_iByteLength);
}
void CBITARRAY::FillAll (void)
{
	memset (m_pDATA, 0xff, m_iByteLength);
}

//-------------------------------------------------------------------------------------------------
C1BITARRAY::C1BITARRAY (int iLength)
{
	for (char cI=0; cI<8; cI++)
		m_btBitMask[ cI ] = 1 << cI;

	if ( iLength <= 0 )
		m_iMaxDataCount = DEFAULT_LENGTH;
	else
		m_iMaxDataCount = iLength;

	m_iByteLength = (m_iMaxDataCount >> BIT_SHIFT)+1;
	m_pDATA = new BYTE [ m_iByteLength ];
	ClearAll ();
}

C1BITARRAY::~C1BITARRAY ()
{
	delete[] m_pDATA;
}

BYTE C1BITARRAY::GetBit (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount )
		return ( m_pDATA[ iIndex >> BIT_SHIFT ] & m_btBitMask[ iIndex & WHICH_BIT ] );

	return 0;
}

void C1BITARRAY::SetBit (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) 
		m_pDATA[ iIndex >> BIT_SHIFT ] |= m_btBitMask[ iIndex & WHICH_BIT ];
}

void C1BITARRAY::ClearBit (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) 
		m_pDATA[ iIndex >> BIT_SHIFT ] &= ~(m_btBitMask[ iIndex & WHICH_BIT ]);
}

BYTE C1BITARRAY::FlipBit (int iIndex)
{
	if ( GetBit( iIndex ) )
		ClearBit( iIndex );
	else
		SetBit ( iIndex );

	return GetBit( iIndex );
}



//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
C2BITARRAY::C2BITARRAY (int iLength)
{
	for (char cI=0; cI<8; cI+=2)
		m_btBitMask[ cI/2 ] = MAX_VALUE << cI;

	if ( iLength <= 0 )
		m_iMaxDataCount = DEFAULT_LENGTH;
	else
		m_iMaxDataCount = iLength;

	m_iByteLength = (m_iMaxDataCount >> BIT_SHIFT)+1;
	m_pDATA = new BYTE [ m_iByteLength ];
	ClearAll ();
}

C2BITARRAY::~C2BITARRAY ()
{
	if ( m_pDATA )
		delete[] m_pDATA;
	m_pDATA = NULL;
}

BYTE C2BITARRAY::GetValue (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) {
		BYTE btValue;

		btValue = m_pDATA[ iIndex >> BIT_SHIFT ] & m_btBitMask[ iIndex & WHICH_BIT ];
		return ( btValue >> ( (iIndex & WHICH_BIT)<<1 ) );
	}

	return 0;
}

void C2BITARRAY::SetValue (int iIndex, BYTE btValue)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) {
		btValue &= MAX_VALUE;
		btValue <<= ( (iIndex & WHICH_BIT)<<1 );
		m_pDATA[ iIndex >> BIT_SHIFT ] &= ~(m_btBitMask[ iIndex & WHICH_BIT ]);
		m_pDATA[ iIndex >> BIT_SHIFT ] |= btValue;
	}
}

void C2BITARRAY::ClearValue (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) 
		m_pDATA[ iIndex >> BIT_SHIFT ] &= ~(m_btBitMask[ iIndex & WHICH_BIT ]);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
C4BITARRAY::C4BITARRAY (int iLength)
{
	for (char cI=0; cI<8; cI+=4)
		m_btBitMask[ cI/4 ] = MAX_VALUE << cI;

	if ( iLength <= 0 )
		m_iMaxDataCount = DEFAULT_LENGTH;
	else
		m_iMaxDataCount = iLength;

	m_iByteLength = (m_iMaxDataCount >> BIT_SHIFT)+1;
	m_pDATA = new BYTE [ m_iByteLength ];
	ClearAll ();
}

C4BITARRAY::~C4BITARRAY ()
{
	delete[] m_pDATA;
}

BYTE C4BITARRAY::GetValue (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) {
		BYTE btValue;

		btValue = m_pDATA[ iIndex >> BIT_SHIFT ] & m_btBitMask[ iIndex & WHICH_BIT ];
		return ( btValue >> ( (iIndex & WHICH_BIT)<<2 ) );
	}

	return 0;
}

void C4BITARRAY::SetValue (int iIndex, BYTE btValue)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) {
		btValue &= MAX_VALUE;
		btValue <<= ( (iIndex & WHICH_BIT)<<2 );
		m_pDATA[ iIndex >> BIT_SHIFT ] &= ~(m_btBitMask[ iIndex & WHICH_BIT ]);
		m_pDATA[ iIndex >> BIT_SHIFT ] |= btValue;
	}
}

void C4BITARRAY::ClearValue (int iIndex)
{
	if ( iIndex >= 0 && iIndex < m_iMaxDataCount ) 
		m_pDATA[ iIndex >> BIT_SHIFT ] &= ~(m_btBitMask[ iIndex & WHICH_BIT ]);
}

//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------
