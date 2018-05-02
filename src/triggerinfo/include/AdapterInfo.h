#ifndef __ADAPTERINFO_H__
#define __ADAPTERINFO_H__

#include "TriggerInfo.h"
#include "__d3d9.h"

struct sAdapterInfo
{
	int iAdapterNO;
	int i565CNT;
	int i8888CNT;
};

class CAdapterInfo
{
private:
	IDirect3D9 *			m_pID3D;
	AdapterInformation *	m_pAdapter;
	int						m_iAdapterCNT;
	sAdapterInfo *			m_AdapterInfos;

public:
	CAdapterInfo (AdapterInformation * pInfo);

	CAdapterInfo (IDirect3D9 * pID3D);

	~CAdapterInfo(void);

	void Release (void);

	int GetAdapterCNT (void) ;

	int GetAllModeCNT (void);

	AdapterInformation * GetAllInformation (void);

	AdapterInformation * GetData (void);

	void GetSystemInformation (int iAdapterNO);

	bool CheckDesiredRes (int iWidht, int iHeight);
};

#endif
