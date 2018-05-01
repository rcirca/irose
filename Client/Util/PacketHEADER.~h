#ifndef	__PACKETHEADER_H
#define	__PACKETHEADER_H
#pragma warning (disable:4201)
//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

#define	MAX_PACKET_SIZE			( (size_t)1024 )
#define SOCKET_NETWORK_STATUS	0x07ff

#pragma pack (push, 1)
#ifndef	__T_PACKETHEADER
#define	__T_PACKETHEADER
// Total 6 bytes..
struct t_PACKETHEADER 
{
	union {
		struct {
			short	m_nSize;
			WORD	m_wType;
			WORD	m_wReserved;
		} ;
		BYTE		m_pDATA[ 1 ];
	} ;
} ;
#endif

struct t_NETWORK_STATUS : public t_PACKETHEADER
{
	BYTE	m_btStatus;
	DWORD	m_dwSocketIDs[7];
} ;
#define NETWORK_STATUS_CONNECT			0x001
#define	NETWORK_STATUS_ACCEPTED			0x002
#define NETWORK_STATUS_DISCONNECT		0x003	// 서버와의 접속이 끊겼다.. But TGNet_DisconnectFromServer호출해야되~~
#define NETWORK_STATUS_DERVERDEAD		0x004
#pragma pack (pop)
//-------------------------------------------------------------------------------------------------

struct t_PACKET;

char *Packet_GetStringPtr   (t_PACKET *pPacket, short &nOffset, short &nOutStrLEN);
char *Packet_GetStringPtr   (t_PACKET *pPacket, short &nOffset);
void *Packet_GetDataPtr     (t_PACKET *pPacket, short &nOffset, short nSize);
bool  Packet_AppendString   (t_PACKET *pPacket, char *pStr);
bool  Packet_AppendData     (t_PACKET *pPacket, void *pData, int nLen);

//-------------------------------------------------------------------------------------------------
#pragma warning (default:4201)
#endif