
#include <windows.h>
#include <tchar.h>
#include <crtdbg.h>
#include "classLOG.h"
#include "classUTIL.h"
#include "ioDataPOOL.h"

//-------------------------------------------------------------------------------------------------
char* classPACKET::GetStringPtr (short *pOffset)
{
    char *pStr;

    if ( *pOffset >= this->m_HEADER.m_nSize ) {
		// iCARUS_LogString (LOG_DEBUG, "Packet_GetStringPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->sNormalHeader.cType, pPacket->sNormalHeader.nSize, *pOffset);
        return NULL;
    }

    pStr     = (char*) &m_pDATA[ *pOffset ];
   *pOffset += ( strlen( pStr ) + 1 );

    return pStr;
}

//-------------------------------------------------------------------------------------------------
BYTE* classPACKET::GetDataPtr (short *pOffset, short nSize)
{
    BYTE *pData;

    if ( *pOffset+nSize > this->m_HEADER.m_nSize ) {
        // iCARUS_LogString (LOG_DEBUG, "Packet_GetDataPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->sNormalHeader.cType, pPacket->sNormalHeader.nSize, *pOffset);
        return NULL;
    }

    pData    = (BYTE*) &m_pDATA[ *pOffset ];
   *pOffset += nSize;

    return (BYTE*) pData;
}

//---------------------------------------------------------------------------
bool classPACKET::AppendString (char *pStr)
{
    short nLen, nSize;

    nLen  = ( pStr ) ? strlen(pStr) + 1 : 1;
    nSize = this->m_HEADER.m_nSize;

    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendString.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendString[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    if ( nLen > 1 ) {
        ::CopyMemory (m_pDATA+nSize, pStr, nLen-1);
    }

    m_pDATA[ nSize+nLen-1 ] = '\0';
    this->m_HEADER.m_nSize += nLen;

    return true;
}
bool classPACKET::AppendData (void *pData, short nLen)
{
    short nSize;

    if ( !pData || !nLen ) {
        OutputDebugString ("Error !! Packet_AppendData.. Source Data is NULL or Length is 0 \n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Source Data is NULL or Length is 0 ]");
        return false;
    }

    nSize = this->m_HEADER.m_nSize;
    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendData.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    ::CopyMemory (m_pDATA+nSize, pData, nLen);

    this->m_HEADER.m_nSize += nLen;

    return true;
}


//-------------------------------------------------------------------------------------------------
/*
LPCPACKET Packet_AllocOnly ()
{
	LPCPACKET pCPacket;

	pCPacket = new classPACKET;
	pCPacket->SetRefCnt( 0 );
	return pCPacket;
}

void Packet_ReleaseOnly (LPCPACKET pCPacket)
{
	if ( pCPacket->GetRefCnt() <= 0 )
		SAFE_DELETE( pCPacket );
}


//-------------------------------------------------------------------------------------------------
LPCPACKET Packet_AllocNLock ()
{
	LPCPACKET pCPacket;

	pCPacket = new classPACKET;
	pCPacket->SetRefCnt( 1 );
	return pCPacket;
}

void Packet_ReleaseNUnlock (LPCPACKET pCPacket)
{
	_ASSERT( pCPacket->GetRefCnt() >= 1 );
	_ASSERT( !pCPacket->m_bDel );

	if ( pCPacket->DecRefCnt() <= 0 ) {
		SAFE_DELETE( pCPacket );
	}
}

void Packet_DecRefCount (LPCPACKET pCPacket)
{
	Packet_ReleaseNUnlock( pCPacket );
}
*/
//-------------------------------------------------------------------------------------------------
