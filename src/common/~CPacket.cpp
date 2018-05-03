#include "stdAFX.h"
#pragma hdrstop

#include "CPacket.h"

#pragma package(smart_init)

//-------------------------------------------------------------------------------------------------
char* CPacket::GetStringPtr (short *pOffset)
{
    char *pStr;

    if ( *pOffset >= m_HEADER.m_nSize ) {
		// iCARUS_LogString (LOG_DEBUG, "Packet_GetStringPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->sNormalHeader.cType, pPacket->sNormalHeader.nSize, *pOffset);
        return NULL;
    }

    pStr     = (char*) &m_pDATA[ *pOffset ];
   *pOffset += ( strlen( pStr ) + 1 );

    return pStr;
}

//-------------------------------------------------------------------------------------------------
BYTE* CPacket::GetDataPtr (short *pOffset, short nSize)
{
    BYTE *pData;

    if ( *pOffset+nSize > m_HEADER.m_nSize ) {
        // iCARUS_LogString (LOG_DEBUG, "Packet_GetDataPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->sNormalHeader.cType, pPacket->sNormalHeader.nSize, *pOffset);
        return NULL;
    }

    pData    = (BYTE*) &m_pDATA[ *pOffset ];
   *pOffset += nSize;

    return (BYTE*) pData;
}

//---------------------------------------------------------------------------
bool CPacket::AppendString (char *pStr)
{
    short nLen, nSize;

    nLen  = ( pStr ) ? strlen(pStr) + 1 : 1;
    nSize = m_HEADER.m_nSize;

    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendString.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendString[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    if ( nLen > 1 ) {
        ::CopyMemory (m_pDATA+nSize, pStr, nLen-1);
    }

    m_pDATA[ nSize+nLen-1 ] = '\0';
    m_HEADER.m_nSize += nLen;

    return true;
}
bool CPacket::AppendData (void *pData, short nLen)
{
    short nSize;

    if ( !pData || !nLen ) {
        OutputDebugString ("Error !! Packet_AppendData.. Source Data is NULL or Length is 0 \n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Source Data is NULL or Length is 0 ]");
        return false;
    }

    nSize = m_HEADER.m_nSize;
    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendData.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    ::CopyMemory (m_pDATA+nSize, pData, nLen);

    m_HEADER.m_nSize += nLen;

    return true;
}

//---------------------------------------------------------------------------
CPacket *Packet_AllocOnly ()
{
    CPacket *pCPacket;

    pCPacket = new CPacket;
    if ( NULL == pCPacket )
        return NULL;

    pCPacket->m_iRefCnt = 0;

    return pCPacket;
}

//---------------------------------------------------------------------------
void Packet_ReleaseOnly (CPacket *pCPacket)
{
    if ( pCPacket->m_iRefCnt <= 0 ) {
        SAFE_DELETE( pCPacket );
    }
}


//---------------------------------------------------------------------------
CPacket *Packet_AllocNLock ()
{
    CPacket *pCPacket;

    pCPacket = new CPacket;
    if ( NULL == pCPacket )
        return NULL;

    pCPacket->LockPacket ();
    pCPacket->m_iRefCnt = 0;

    return pCPacket;
}

//---------------------------------------------------------------------------
void Packet_ReleaseNUnlock (CPacket *pCPacket)
{
    if ( pCPacket->m_iRefCnt <= 0 ) {
        pCPacket->UnlockPacket ();
        SAFE_DELETE( pCPacket );
        return;
    }

    pCPacket->UnlockPacket ();
}

//---------------------------------------------------------------------------
void Packet_DecRefCount (CPacket *pCPacket)
{
    pCPacket->LockPacket ();

    pCPacket->m_iRefCnt --;
    if ( pCPacket->m_iRefCnt <= 0 ) {
        pCPacket->UnlockPacket ();
        SAFE_DELETE( pCPacket );
    } else
        pCPacket->UnlockPacket ();
}

//---------------------------------------------------------------------------

