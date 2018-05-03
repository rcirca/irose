
#include "stdAFX.h"
#include <mmsystem.h>
#include "CConnector.h"

extern void WM_CloseSOCKET (TCustomWinSocket *pSocket, int iFrom);

//-------------------------------------------------------------------------------------------------
CConnector::CConnector()
{
    m_pRecvPket = NULL;
    ::InitializeCriticalSection( &m_csSENDq );
}
CConnector::~CConnector()
{
    ::DeleteCriticalSection( &m_csSENDq );
}

//-------------------------------------------------------------------------------------------------
bool CConnector::_Init (TCustomWinSocket *pSocket)
{
    m_nSendByte = 0;
    m_nRecvByte = 0;

    m_pSocket      = pSocket;
    m_bCloseSocket = false;
    
    // m_cStatus   = CLIENT_STATUS_CONNECT;
    m_dwLastActionTime = ::timeGetTime ();

    m_pRecvPket = (t_PACKET *)new BYTE[ MAX_PACKET_SIZE ];
    if ( m_pRecvPket == NULL ) {
        ;
        // Error Message ...
        LogString (0xffff, "ERROR[%s:%d] : New Connection Packet Memory Allocation Failed ...\n", __FILE__, __LINE__);
        ;
        return false;
    }

    return true;
}
void CConnector::_Free (void)
{
/*
	// *** 이미 클라이언트 리스트에서 제거되어 SendPacket loop에서 절때루 호출 안된다.
	this->LockSENDq ();		// classCLIENT::Client_Free
		classDLLNODE <classPACKET *> *pPketNode;
		pPketNode = m_SendPketQ.GetHeadNode ();
		while ( pPketNode ) {
			m_SendPketQ.DeleteNode (pPketNode);

			Packet_DecRefCount( pPketNode->DATA );
			delete   pPketNode;

			pPketNode = m_SendPketQ.GetHeadNode ();
		}
	this->UnlockSENDq ();
*/

    if ( m_pRecvPket ) {
        delete[] m_pRecvPket;
        m_pRecvPket = NULL;
    }
}


//-------------------------------------------------------------------------------------------------
int CConnector::SendBuf(void *pBuf, int iCount)
{
    int iResult;

    this->m_pSocket->Lock ();
    __try {
        iResult = 0;
        if ( this->m_pSocket->SocketHandle != INVALID_SOCKET )
            iResult = ::send( this->m_pSocket->SocketHandle, (const char*)pBuf, iCount, 0);
    }
    __finally {
        this->m_pSocket->Unlock ();
    }

    return iResult;
/*
function TCustomWinSocket.SendBuf(var Buf; Count: Integer): Integer;
var
  ErrorCode: Integer;
begin
  Lock;
  try
    Result := 0;
    if not FConnected then Exit;
    Result := send(FSocket, Buf, Count, 0);
    if Result = SOCKET_ERROR then
    begin
      ErrorCode := WSAGetLastError;
      if (ErrorCode <> WSAEWOULDBLOCK) then
      begin
        Error(Self, eeSend, ErrorCode);
        Disconnect(FSocket);
        if ErrorCode <> 0 then
          raise ESocketError.CreateResFmt(@sWindowsSocketError,
            [SysErrorMessage(ErrorCode), ErrorCode, 'send']);
      end;
    end;
  finally
    Unlock;
  end;
end;
*/
}

int CConnector::ReceiveBuf(void *pBuf, int iCount)
{
    int iResult;

    this->m_pSocket->Lock ();
    __try {
        iResult = 0;
        if ( this->m_pSocket->SocketHandle != INVALID_SOCKET ) {
            if ( iCount == -1 ) {
                // unsigned long ulLen;
                // get receive buff length
                ioctlsocket(this->m_pSocket->SocketHandle, FIONREAD, &(unsigned long)iResult);
                // return (int)ulLen;
            } else
                iResult = ::recv( this->m_pSocket->SocketHandle, (char*)pBuf, iCount, 0);
        }
    }
    __finally {
        this->m_pSocket->Unlock ();
    }
    return iResult;
/*
function TCustomWinSocket.ReceiveBuf(var Buf; Count: Integer): Integer;
var
  ErrorCode: Integer;
begin
  Lock;
  try
    Result := 0;
    if (Count = -1) and FConnected then
      ioctlsocket(FSocket, FIONREAD, Longint(Result))
    else begin
      if not FConnected then Exit;
      Result := recv(FSocket, Buf, Count, 0);
      if Result = SOCKET_ERROR then
      begin
        ErrorCode := WSAGetLastError;
        if ErrorCode <> WSAEWOULDBLOCK then
        begin
          Error(Self, eeReceive, ErrorCode);
          Disconnect(FSocket);
          if ErrorCode <> 0 then
            raise ESocketError.CreateResFmt(@sWindowsSocketError,
              [SysErrorMessage(ErrorCode), ErrorCode, 'recv']);
        end;
      end;
    end;
  finally
    Unlock;
  end;
end;
*/
}


//-------------------------------------------------------------------------------------------------
bool CConnector::AppendSendPacket (classPACKET *pCPacket)
{
    if ( pCPacket->m_HEADER.m_nSize >= MAX_PACKET_SIZE ) {
        LogString (0xffff, "ERROR[%s:%d] : Too big send packet ... Type: 0x%x, Size: %d", __FILE__, __LINE__, pCPacket->m_HEADER.m_wType, pCPacket->m_HEADER.m_nSize);

        m_bCloseSocket = true;
        return true;
    }

    classDLLNODE <classPACKET*> *pNewNode;
    pNewNode = new classDLLNODE<classPACKET*> (NULL);
    if ( pNewNode == NULL ) {
        // ERROR out of Memory ...
        return false;
    }

    pCPacket->m_iRefCnt ++;
    pNewNode->DATA = pCPacket;

    this->LockSENDq ();	// classCLIENT::AppendSendPacket
        m_SendPketQ.AppendNode (pNewNode);
        if ( m_SendPketQ.GetNodeCount() == 1 ) {
            // 비어 있던 큐다
        }
    this->UnlockSENDq ();

    return true;
}

//-------------------------------------------------------------------------------------------------
bool CConnector::SendPacket ()
{
    static DWORD dwCurrentTime;

    dwCurrentTime = timeGetTime ();

    if ( !m_bIsWritable ) {
        // 쓰기 상태가 아니다...
        /*
        if ( dwCurrentTime - this->m_dwLastActionTime >= TIMEOUT_DISCONNECT ) {
            // 바로 pClientNode->DATA.m_pSocket->Close () 를 호출하면 데드락 걸린다.
            WM_CloseSOCKET( pClientNode->DATA.m_pSocket, 3 );
        }
        */
        return false;
    }

    if ( this->m_SendPketQ.GetNodeCount () ) {
        classDLLNODE <classPACKET*> *pPketNode;
        int  iSendByte, iPacketSize;
        bool bSendFile;

        this->m_dwLastActionTime = dwCurrentTime;

        this->LockSENDq ();	// classCLIENTMANAGER::Proc
            pPketNode   = this->m_SendPketQ.GetHeadNode ();
        this->UnlockSENDq ();
        while ( pPketNode != NULL ) {
            iPacketSize = pPketNode->DATA->m_HEADER.m_nSize;

            // iSendByte   = this->m_pSocket->SendBuf ( (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte, iPacketSize - this->m_nSendByte);
            iSendByte   = this->SendBuf ( (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte, iPacketSize - this->m_nSendByte);
            /*
            if ( this->m_pSocket->SocketHandle == INVALID_SOCKET )
                return false;
            iSendByte = ::send( this->m_pSocket->SocketHandle,
                                (BYTE*) pPketNode->DATA->m_pDATA + this->m_nSendByte,
                                iPacketSize - this->m_nSendByte, 0);
            */
            if ( iSendByte == SOCKET_ERROR ) {
                int iErrorCode;
                iErrorCode = WSAGetLastError ();
                if ( iErrorCode == WSAEWOULDBLOCK ) {
                    this->m_bIsWritable = false;
                    break;
                }

                /* TODO : 접속 끊어야 한다. */
                /*
                this->m_pSocket->Disconnect( m_pSocket->SocketHandle ); // Disconnect(FSocket);
                */

                return false;
            } else {
                LogString (0xffff, "SendPacket :: %d ", iSendByte);

                this->m_nSendByte += iSendByte;
                if ( this->m_nSendByte == iPacketSize ) {
                    classDLLNODE <classPACKET*> *pDelNode;

                    pDelNode  = pPketNode;
                    this->LockSENDq ();	// classCLIENTMANAGER::Proc
                        pPketNode = this->m_SendPketQ.GetNextNode (pDelNode);
                        this->m_SendPketQ.DeleteNode ( pDelNode );
                    this->UnlockSENDq ();

                    Packet_DecRefCount( pDelNode->DATA );
                    delete   pDelNode;

                    this->m_nSendByte = 0;
                }
            }
        }
    } else {
        /*
        if ( dwCurrentTime - this->m_dwLastActionTime > TIMEOUT_INTERVAL ) {
            pCPacket = ::Packet_AllocOnly();
            pCPacket->m_HEADER.wType = TN_SERVER_CHECK_ALIVE;
            pCPacket->m_HEADER.m_nSize = sizeof( t_normal_header );
            this->AppendSendPacket( pCPacket );
        }
        */
    }

    return true;
}

//-------------------------------------------------------------------------------------------------
void CConnector::ON_Read (void)
{
    int   iRecvByte, iQueueByte;
    SHORT nPacketLen;

    iQueueByte = m_pSocket->ReceiveLength ();

    m_dwLastActionTime = timeGetTime ();

    do {
        if ( m_nRecvByte < sizeof(t_PACKETHEADER) ) {
            // iRecvByte = m_pSocket->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, sizeof(t_PACKETHEADER) - m_nRecvByte);
            iRecvByte = this->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, sizeof(t_PACKETHEADER) - m_nRecvByte);
        } else {
            nPacketLen = m_pRecvPket->m_HEADER.m_nSize;
            if ( nPacketLen >= MAX_PACKET_SIZE ) {
                // Error Too Big Packet or Invalid Packet !!!
                ;
                // LogString (LOG_NORMAL, "ERROR[%s:%d] : Too Big packet or Invalid packet ... Type: 0x%x, Size: %d", __FILE__, __LINE__, m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
                ;
                m_nRecvByte    = 0;
                m_bIsWritable  = false;
                // m_bCloseSocket = true;
                LogString (0xffff, "Too big packet size :: IP[ %s ] Close() ...\n", __FILE__, __LINE__, m_pSocket->RemoteAddress.c_str() );

                WM_CloseSOCKET( m_pSocket, 2 );

                break;
            }

            iRecvByte = this->ReceiveBuf ( m_pRecvPket->m_pDATA + m_nRecvByte, nPacketLen - m_nRecvByte);
        }
        if ( iRecvByte == SOCKET_ERROR ) {
            // Error ...
            ;
            ;
            return;
        }

        // FormMain->m_iRecvBytesPerSec += iRecvByte;

        m_nRecvByte += iRecvByte;
        if ( m_nRecvByte >= sizeof( t_PACKETHEADER ) &&
             m_nRecvByte == m_pRecvPket->m_HEADER.m_nSize ) {

            m_pRecvPket->m_pDATA[ m_nRecvByte ] = 0;

            // if ( !m_bCloseSocket )
                PacketHandle ();

            m_nRecvByte = 0;
        }

        iQueueByte -= iRecvByte;
    } while ( iQueueByte > 0 ) ;
}


//-------------------------------------------------------------------------------------------------
void CConnector::PacketHandle (void)
{
    LogString (0xffff, "virtual CConnector::PacketHandle() type: 0x%x, Size: %d ", m_pRecvPket->m_HEADER.m_wType, m_pRecvPket->m_HEADER.m_nSize);
}

//-------------------------------------------------------------------------------------------------

