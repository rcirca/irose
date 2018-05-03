#ifndef	__IP_ADDR_H
#define	__IP_ADDR_H
#include "DLLIST.h"
//-------------------------------------------------------------------------------------------------

struct tagIP4ADDR {
	unsigned long	m_ulMIN;
	unsigned long	m_ulMAX;
} ;


#define	IPv4_LIST_SIZE		256
class CIPv4Addr {
private:
	classDLLIST< tagIP4ADDR >	m_LIST[ IPv4_LIST_SIZE ];

public :
	~CIPv4Addr ();
	bool Add (char *szIPFrom, char *szIPTo);
	bool Find (in_addr &inADDR);
	bool Find (char *szIPAddr);
} ;

//-------------------------------------------------------------------------------------------------
#endif