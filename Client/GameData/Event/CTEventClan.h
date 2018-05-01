#ifndef _CTEventClan_
#define _CTEventClan_
#include "ctevent.h"

class CTEventClan :	public CTEvent
{
public:
	CTEventClan(void);
	virtual ~CTEventClan(void);
	
	enum{
		EID_NONE,
		EID_ADD_MEMBER,				
		EID_REMOVE_MEMBER,				
		EID_CHANGE_CLASS,				
		EID_CHANGE_CONNECT_STATUS,
		EID_SET_JOB,
		EID_SET_LEVEL,
	};

	void SetName( const char* pszName );
	void SetClass( int iClass );
	void SetPoint( int iPoint );
	void SetChannelNo( int iChannelNo );
	void SetJob( short nJob );
	void SetLevel( short nJob );

	int			GetClass();
	int			GetConnectStatus();
	int			GetPoint();
	int			GetChannelNo();
	short		GetJob();
	short		GetLevel();
	const char* GetName();

private:
	std::string m_strName;
	int			m_iPoint;
	int			m_iChannelNo;
	int			m_iClass;
	short		m_nLevel;
	short		m_nJob;
};
#endif