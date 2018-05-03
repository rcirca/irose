
#include <windows.h>
#include "PacketHEADER.h"
#include "classSTR.h"

struct t_PACKET;

//---------------------------------------------------------------------------
char *Packet_GetStringPtr (t_PACKET *pPacket, short &nOffset, short &nOutStrLEN)
{
    if ( nOffset >= ((t_PACKETHEADER*)pPacket)->m_nSize ) {
		// iCARUS_LogString (LOG_DEBUG, "Packet_GetStringPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->cType, pPacket->m_nSize, *pOffset);
        return NULL;
    }

    char  *pStr = (char*) ((t_PACKETHEADER*)pPacket)->m_pDATA + nOffset;
	short nMaxStrLEN = ((t_PACKETHEADER*)pPacket)->m_nSize-nOffset;

	nOutStrLEN = CStr::StrLen( pStr, nMaxStrLEN );
	if ( nOutStrLEN > nMaxStrLEN )
		return NULL;

	nOffset += ( nOutStrLEN + 1 );
    return pStr;
}

//---------------------------------------------------------------------------
char *Packet_GetStringPtr (t_PACKET *pPacket, short &nOffset)
{
    if ( nOffset >= ((t_PACKETHEADER*)pPacket)->m_nSize ) {
		// iCARUS_LogString (LOG_DEBUG, "Packet_GetStringPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->cType, pPacket->m_nSize, *pOffset);
        return NULL;
    }

    char *pStr;
    pStr    = (char*) ((t_PACKETHEADER*)pPacket)->m_pDATA + nOffset;
    nOffset += ( strlen( pStr ) + 1 );

    return pStr;
}


//---------------------------------------------------------------------------
void *Packet_GetDataPtr (t_PACKET *pPacket, short &nOffset, short nSize)
{
    if ( nOffset+nSize > ((t_PACKETHEADER*)pPacket)->m_nSize ) {
        // iCARUS_LogString (LOG_DEBUG, "Packet_GetDataPtr return NULL !!! Type: [0x%x], Size: [%d], Offset: [%d]", pPacket->cType, pPacket->m_nSize, *pOffset);
        return NULL;
    }

    char *pData;
    pData = (char*) ((t_PACKETHEADER*)pPacket)->m_pDATA + nOffset;
    nOffset += nSize;

    return (void*) pData;
}


//---------------------------------------------------------------------------
bool Packet_AppendString (t_PACKET *pPacket, char *pStr)
{
    short nLen, nSize;

    nLen  = ( pStr ) ? strlen(pStr) + 1 : 1;
    nSize = ((t_PACKETHEADER*)pPacket)->m_nSize;

    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendString.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendString[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    if ( nLen > 1 ) {
        CopyMemory (((t_PACKETHEADER*)pPacket)->m_pDATA+nSize, pStr, nLen-1);
    }

    ((t_PACKETHEADER*)pPacket)->m_pDATA[ nSize+nLen-1 ] = '\0';

    ((t_PACKETHEADER*)pPacket)->m_nSize += nLen;

    return true;
}


//---------------------------------------------------------------------------
bool Packet_AppendData (t_PACKET *pPacket, void *pData, int nLen)
{
    short nSize;

    if ( !pData || !nLen ) {
        OutputDebugString ("Error !! Packet_AppendData.. Source Data is NULL or Length is 0 \n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Source Data is NULL or Length is 0 ]");
        return false;
    }

    nSize = ((t_PACKETHEADER*)pPacket)->m_nSize;
    if ( nSize+nLen >= MAX_PACKET_SIZE ) {
        OutputDebugString ("Error !! Packet_AppendData.. Too Big Packet\n");
        // iCARUS_LogString (LOG_NORMAL, "Error !! Packet_AppendData[ Too Big Packet, Size:%d ]", nSize+nLen);
        return false;
    }

    CopyMemory (((t_PACKETHEADER*)pPacket)->m_pDATA+nSize, pData, nLen);

    ((t_PACKETHEADER*)pPacket)->m_nSize += nLen;

    return true;
}


//---------------------------------------------------------------------------


