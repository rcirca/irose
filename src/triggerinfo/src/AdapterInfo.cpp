#include "StdAfx.h"
#include ".\adapterinfo.h"

struct DesiredRes 
{
	int iWidth;
	int iHeight;
};

DesiredRes DesrRes[] = 
{
	{640, 480},	{800, 600},	{1024, 768},	{1152, 864},
	{1280, 768}, {1280, 960}, {1280, 1024},	{1600, 1200}
};

bool CAdapterInfo::CheckDesiredRes (int iWidth, int iHeight)
{
	for(int i = 0; i < sizeof (DesrRes) / sizeof (DesiredRes); i++)
	{
		if(iWidth == DesrRes[ i ].iWidth && iHeight == DesrRes[ i ].iHeight)
			return true;
	}

	return false;
}


void CAdapterInfo::GetSystemInformation (int iAdapterNO)
{
	D3DDISPLAYMODE checked_display_mode;
	SystemResolution *buffer_resolution;

	m_pAdapter->pAdapter[iAdapterNO].nResolution=0;

	for (int mode = 0; mode < m_AdapterInfos[ iAdapterNO ].i565CNT; mode++) 
	{
		if (!FAILED(m_pID3D->EnumAdapterModes( iAdapterNO, D3DFMT_R5G6B5, mode, &checked_display_mode ))) 
		{
			int bpp=0;
			if (checked_display_mode.Format == D3DFMT_R5G6B5) 
			{
				if(CheckDesiredRes (checked_display_mode.Width, checked_display_mode.Height))
				{
					if(checked_display_mode.RefreshRate % 5 == 0)
					{      		  
						bpp=16; 
						buffer_resolution=&(m_pAdapter->pAdapter[iAdapterNO].pResolution[m_pAdapter->pAdapter[iAdapterNO].nResolution]);
						buffer_resolution->frequency	= checked_display_mode.RefreshRate;
						buffer_resolution->pixel_color	= bpp;
						buffer_resolution->pixel_height = checked_display_mode.Height;
						buffer_resolution->pixel_width	= checked_display_mode.Width;
						buffer_resolution->adaptor_number = iAdapterNO;

						m_pAdapter->pAdapter[iAdapterNO].nResolution++;
					}
				}
			}
		}
	}

	for (int mode = 0; mode < m_AdapterInfos[ iAdapterNO ].i8888CNT; mode++) 
	{
		if (!FAILED(m_pID3D->EnumAdapterModes( iAdapterNO, D3DFMT_X8R8G8B8, mode, &checked_display_mode ))) 
		{
			int bpp=0;
			if (checked_display_mode.Format == D3DFMT_X8R8G8B8) 
			{
				if(CheckDesiredRes (checked_display_mode.Width, checked_display_mode.Height))
				{
					if(checked_display_mode.RefreshRate % 5 == 0)
					{      		  
						bpp=32; 
						buffer_resolution=&(m_pAdapter->pAdapter[iAdapterNO].pResolution[m_pAdapter->pAdapter[iAdapterNO].nResolution]);
						buffer_resolution->frequency	= checked_display_mode.RefreshRate;
						buffer_resolution->pixel_color	= bpp;
						buffer_resolution->pixel_height = checked_display_mode.Height;
						buffer_resolution->pixel_width	= checked_display_mode.Width;
						buffer_resolution->adaptor_number = iAdapterNO;

						m_pAdapter->pAdapter[iAdapterNO].nResolution++;
					}
				}
			}
		}
	}
}

CAdapterInfo::CAdapterInfo (AdapterInformation * pInfo)
{
	m_pID3D = NULL;
	m_iAdapterCNT = 0;
	m_AdapterInfos = NULL;

	m_pAdapter = pInfo;
	if(m_iAdapterCNT)
		m_iAdapterCNT = m_pAdapter->iAdapterCNT;
}

CAdapterInfo::CAdapterInfo (IDirect3D9 * pID3D)
{
	m_pID3D = pID3D;

	m_iAdapterCNT =0;
	m_AdapterInfos = NULL;

	m_pAdapter = new AdapterInformation;
	m_pAdapter->iAdapterCNT = 0;
	m_pAdapter->pAdapter = NULL;
}

CAdapterInfo::~CAdapterInfo(void)
{
	Release ();
}

void CAdapterInfo::Release (void)
{
	int i = 0;	

	if(m_pAdapter)
	{
		if( m_pAdapter->iAdapterCNT>0)
		{
			for(i = 0;i < m_pAdapter->iAdapterCNT; i++)
			{		
				if( m_pAdapter->pAdapter[i].nResolution > 0)
					delete [] m_pAdapter->pAdapter[i].pResolution;   
			}

			delete []  m_pAdapter->pAdapter; 
		}

		if(m_AdapterInfos)
		{
			delete [] m_AdapterInfos;
			m_AdapterInfos = NULL;
			m_iAdapterCNT = 0;
		}

		delete m_pAdapter;
		m_pAdapter = NULL;
	}
}

int CAdapterInfo::GetAdapterCNT (void) 
{
	if(m_pID3D)
		return (int)m_pID3D->GetAdapterCount ();

	return 0;
}

int CAdapterInfo::GetAllModeCNT (void)
{
	int iAllCNT = 0;
	m_iAdapterCNT = GetAdapterCNT ();

	if(m_iAdapterCNT > 0)
	{
		m_pAdapter->iAdapterCNT = m_iAdapterCNT;
		m_pAdapter->pAdapter = new SystemAdapter[ m_iAdapterCNT ];

		if(m_iAdapterCNT > 0)
		{
			m_AdapterInfos = new sAdapterInfo[ m_iAdapterCNT ];

			for(int i = 0; i < m_iAdapterCNT; i++)
			{
				m_AdapterInfos[ i ].iAdapterNO	= i;
				m_AdapterInfos[ i ].i565CNT		= m_pID3D->GetAdapterModeCount (i, D3DFMT_R5G6B5 );
				m_AdapterInfos[ i ].i8888CNT	= m_pID3D->GetAdapterModeCount (i, D3DFMT_X8R8G8B8 );

				m_pAdapter->pAdapter[i].nResolution=m_AdapterInfos[ i ].i565CNT+m_AdapterInfos[ i ].i8888CNT;
				m_pAdapter->pAdapter[i].pResolution= new SystemResolution[ m_pAdapter->pAdapter[i].nResolution ];


				iAllCNT += (m_AdapterInfos[ i ].i565CNT + m_AdapterInfos[ i ].i8888CNT);
			}
		}
	}

	return iAllCNT;
}

AdapterInformation * CAdapterInfo::GetAllInformation (void)
{
	int iTotModeCNT = GetAllModeCNT ();


	D3DADAPTER_IDENTIFIER9 pIdentifier;

	for(int i = 0; i < m_iAdapterCNT; i++)
	{
		m_pID3D->GetAdapterIdentifier(i,D3DENUM_WHQL_LEVEL,&pIdentifier);
		strcpy (m_pAdapter->pAdapter[ i ].Description,pIdentifier.Description);
		strcpy (m_pAdapter->pAdapter[ i ].DeviceName,pIdentifier.DeviceName);
		strcpy (m_pAdapter->pAdapter[ i ].Driver,pIdentifier.Driver);
		GetSystemInformation (i);
	}

	return m_pAdapter;
}

AdapterInformation * CAdapterInfo::GetData (void)
{
	return m_pAdapter;
}