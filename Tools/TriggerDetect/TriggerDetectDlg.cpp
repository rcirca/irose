// TriggerDetectDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "TriggerDetect.h"
#include "TriggerDetectDlg.h"
#include "TriggerInfo.h"
#include ".\triggerdetectdlg.h"
#include "RgnBitmap.h"


extern LPCSTR g_CmdLine;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define		TESTWND_WIDHT	300
#define		TESTWND_HIEGT	300
#define		TESTWND_NAME	"Trigger Detect Window"

DefRes g_defs[] = 
{
	{1024, 768, 32}, {1024, 768, 16}, {800, 600, 32}, {800, 600, 16}
};

// CTriggerDetectDlg 대화 상자

CTriggerDetectDlg::CTriggerDetectDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTriggerDetectDlg::IDD, pParent)
	, m_rdFullScreen(0)
	, m_rdWindow(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pAdptInfo = NULL;
}

void CTriggerDetectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDCCOMBO_MONITOR, m_cbMonitor);
	DDX_Control(pDX, IDCCOMBO_RESOLUTION, m_cbResolution);
	DDX_Control(pDX, IDCCB_FREQ, m_cbFreq);
	DDX_Control(pDX, IDCSLD_PERMERMANCE, m_sldPerform);
	DDX_Control(pDX, IDC_COMBO2, m_cbWinMode);
	DDX_Control(pDX, IDOK, m_OkButton);
}

BEGIN_MESSAGE_MAP(CTriggerDetectDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_WM_CLOSE()
	ON_CBN_SELCHANGE(IDCCOMBO_MONITOR, On_Adpt_SelChange)
	ON_CBN_SELCHANGE(IDCCOMBO_RESOLUTION, On_ResnColor_SelChange)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


// CTriggerDetectDlg 메시지 처리기

BOOL CTriggerDetectDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	// 프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	LPCSTR strMyClass = AfxRegisterWndClass (CS_VREDRAW | CS_HREDRAW);

	// 시스템 정보를 모우고 파일로 저장
	TI_GatherSysInfo ();

	const char * pSysInfo = TI_GetSysInfo ();
	const char * pSysInfo2 = TI_ReadSysInfoFile ();

	// 해상도 정보를 모우고 파일로 저장
	TI_GatherAdptInfo ();

	// 파일에 저장된 해상도 정보를 다시 가지고 옴.
	m_pAdptInfo = TI_ReadAdptInfoFile ();

	ListAdapter ();

	m_sldPerform.SetRange (1, 5, true);
	m_sldPerform.SetPos (4);

	m_cbWinMode.ResetContent ();
	m_cbWinMode.AddString ("Full Screen");
	m_cbWinMode.AddString ("Window");
	m_cbWinMode.SetCurSel (0);

	//-----------------------------------------------------------------------------
	HBITMAP hBitmap = NULL;		
	hBitmap = (HBITMAP) LoadBitmap( GetModuleHandle(NULL), MAKEINTRESOURCE( IDB_BITMAP1 ) );
    if ( hBitmap == NULL)
         hBitmap = (HBITMAP) LoadBitmap( NULL, "TriggerInfoDetector.bmp" );	
	
	if( hBitmap )
	{
		HRGN hRgn = CRgnBitmap::BitmapToRegion( hBitmap );
		this->SetWindowRgn( hRgn, true );		
	}

	DeleteObject( hBitmap );

	if( strcmp( g_CmdLine, "" ) == 0 )
	m_OkButton.SetWindowText( "Apply" );
	else
    m_OkButton.SetWindowText("Play");

	return TRUE;
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면 
// 아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
// 프레임워크에서 이 작업을 자동으로 수행합니다.

void CTriggerDetectDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다. 
HCURSOR CTriggerDetectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTriggerDetectDlg::OnBnClickedRadio1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
}

void CTriggerDetectDlg::ListAdapter (void)
{
	m_cbMonitor.ResetContent ();

	if(m_pAdptInfo)
	{
		for(int i = 0; i < m_pAdptInfo->iAdapterCNT; i++)
		{
			m_cbMonitor.AddString (m_pAdptInfo->pAdapter[ i ].Description);
		}

		m_cbMonitor.SetCurSel (0);
		ListResnColor (0);
	}
}

int CTriggerDetectDlg::CheckDefaultRes (void)
{
	
	for(int i = m_cbResolution.GetCount () - 1; i >= 0 ; i--)
	{
		SystemResolution * pRes = (SystemResolution *)m_cbResolution.GetItemData (i);

		for(int j = 0; j < sizeof (g_defs) / sizeof (DefRes); j++)
		{
			if(pRes->pixel_width	== g_defs[ j ].iWidth && 
				pRes->pixel_height	== g_defs[ j ].iHeight&&
				pRes->pixel_color	== g_defs[ j ].iColorDetph)
			{
				return i;
			}
		}
	}

	return -1;
}

void CTriggerDetectDlg::ListResnColor (int iAdptIDX)
{
	bool bSelDef = false;
	int iSelDef = -1;
	m_cbResolution.ResetContent ();

	if(iAdptIDX < m_pAdptInfo->iAdapterCNT)
	{
		SystemResolution *pResolution = m_pAdptInfo->pAdapter[ iAdptIDX ].pResolution;
		int iResCNT = m_pAdptInfo->pAdapter[ iAdptIDX ].nResolution;

		if(iResCNT > 0)
		{
			char szResnColor[ 255 ];
			int iWidth	= pResolution[ 0 ].pixel_width;
			int iHeight = pResolution[ 0 ].pixel_height;
			int iColor	= pResolution[ 0 ].pixel_color;

			sprintf (szResnColor, "%d * %d (%dbit Color)", iWidth, iHeight, iColor);
			m_cbResolution.AddString (szResnColor);
			m_cbResolution.SetItemData (0, (DWORD_PTR)&pResolution[ 0 ]);

			for(int i = 1; i < iResCNT; i++)
			{
				if(
					iWidth	!= pResolution[ i ].pixel_width ||
					iHeight != pResolution[ i ].pixel_height ||
					iColor	!= pResolution[ i ].pixel_color
				)
				{
					iWidth	= pResolution[ i ].pixel_width;
					iHeight = pResolution[ i ].pixel_height;
					iColor	= pResolution[ i ].pixel_color;

					sprintf (szResnColor, "%d x %d (%dbit Color)", iWidth, iHeight, iColor);
					int iItemIDX = m_cbResolution.AddString (szResnColor);
					m_cbResolution.SetItemData (iItemIDX, (DWORD_PTR)&pResolution[ i ]);
				}
			}

			iSelDef = CheckDefaultRes ();
			if(iSelDef >= 0)
			{
				m_cbResolution.SetCurSel (iSelDef);
				ListFreq (iAdptIDX, iSelDef);
			}
			else
			{
				m_cbResolution.SetCurSel (0);
				ListFreq (iAdptIDX, 0);
			}
		}
	}
}

void CTriggerDetectDlg::ListFreq (int iAdptIDX, int iResComboIDX)
{
	m_cbFreq.ResetContent ();
	m_cbFreq.EnableWindow (true);
	
	if(iAdptIDX < m_pAdptInfo->iAdapterCNT)
	{
		SystemResolution *pResolution = m_pAdptInfo->pAdapter[ iAdptIDX ].pResolution;
		int iResCNT = m_pAdptInfo->pAdapter[ iAdptIDX ].nResolution;

		if(iResComboIDX < iResCNT)
		{
			SystemResolution * pItemData = (SystemResolution *)m_cbResolution.GetItemData (iResComboIDX);

			int iWidth	= pItemData->pixel_width;
			int iHeight = pItemData->pixel_height;
			int iColor = pItemData->pixel_color;

			char szFreq[ 255 ];
			int iAddCNT = 0;
			for(int i = 0; i < iResCNT; i++)
			{
				if(
					iWidth	== pResolution[ i ].pixel_width &&
					iHeight == pResolution[ i ].pixel_height &&
					iColor	== pResolution[ i ].pixel_color
				)
				{
					if(pResolution[ i ].frequency > 0)
					{
						sprintf (szFreq, "%dMHz", pResolution[ i ].frequency);
						int iItemIDX = m_cbFreq.AddString (szFreq);
						m_cbFreq.SetItemData (iItemIDX, (DWORD_PTR)pResolution[ i ].frequency);

						iAddCNT++;
					}
				}
			}

			if(iAddCNT > 0)
				m_cbFreq.SetCurSel (0);
			else
				m_cbFreq.EnableWindow (false);
		}
	}
}


void CTriggerDetectDlg::OnClose()
{
	if(m_pAdptInfo)
	{
		TI_ReleaseAdptInfo ();
		m_pAdptInfo = NULL;
	}

	CDialog::OnClose();
}

void CTriggerDetectDlg::On_Adpt_SelChange()
{
	int iSel = m_cbMonitor.GetCurSel ();

	if(iSel >= 0)
		ListResnColor (iSel);
}

void CTriggerDetectDlg::On_ResnColor_SelChange()
{
	int iSelAdpt		= m_cbMonitor.GetCurSel ();
	int iSelResnColor	= m_cbResolution.GetCurSel ();

	if(iSelAdpt >= 0 && iSelResnColor >= 0)
	{
		ListFreq (iSelAdpt, iSelResnColor);
	}
}


/********************************************************************
 *
 * "확인" 버튼 눌렀을 때
 *
 */
void CTriggerDetectDlg::OnBnClickedOk()
{
	int iResWidth	= 0;
	int iHeight		= 0;
	int iColorBits	= 0;
	int	iFreq		= 0;

	int iAdapterIDX = m_cbMonitor.GetCurSel ();
	int iResnColorIDX = m_cbResolution.GetCurSel ();
	int iFreqIDX = m_cbFreq.GetCurSel ();
	SystemResolution *pResolution = (SystemResolution *)m_cbResolution.GetItemData (iResnColorIDX);
	if(pResolution)
	{
		// 해상도 - 폭
		iResWidth	= pResolution->pixel_width;
		// 해상도 - 높이
		iHeight		= pResolution->pixel_height;
		// 해상도 - 컬러비트수
		iColorBits	= pResolution->pixel_color;
	}

	if(iFreqIDX >= 0)
	{
		// 주파수
		iFreq = (int)m_cbFreq.GetItemData (iFreqIDX);
	}
	
	if(iFreq <= 0)
		iFreqIDX = 60;

	// 윈도우 모드 : 전체 : 0, 창모드 : 1
	int iWinModeIDX = m_cbWinMode.GetCurSel ();
	iWinModeIDX = iWinModeIDX >= 0 ? iWinModeIDX : 0;
		
	// 퍼포먼스 범위 : 1 ~ 5
	int iPerformance = m_sldPerform.GetPos ();

	// 런처나 클라이언트에서 넘어오는 커맨드 라인이 아래에 저장되어 있음.
	// g_CmdLine 

	// 여기서 클라이언트 실행 시켜 주세요. 난 모름.
/// 이하 nAvy 추가 	2004/8/2
	char	szTemp[128];
	const   char	g_szIniFileName[] = "./SevenHearts.ini";

	///성능
	itoa( iPerformance - 1, szTemp, 10 );
	WritePrivateProfileString("VIDEO","PERFORMANCE", szTemp,g_szIniFileName );

	///풀스크린
	int iFullScreen = (iWinModeIDX == 0)? 1: 0;
	itoa( iFullScreen, szTemp, 10 );
	WritePrivateProfileString("VIDEO","FULLSCREEN", szTemp,g_szIniFileName );

	///해상도
	itoa( iResWidth, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION", "WIDTH", szTemp, g_szIniFileName );

	itoa( iHeight, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION", "HEIGHT", szTemp, g_szIniFileName );

	itoa( iColorBits, szTemp, 10 );
	WritePrivateProfileString("RESOLUTION", "DEPTH", szTemp, g_szIniFileName );

	///주파수
	itoa( iFreq, szTemp, 10 );
	WritePrivateProfileString( "RESOLUTION","FREQUENCY", szTemp, g_szIniFileName );

	///ADAPTER
	itoa( iAdapterIDX, szTemp, 10 );
	WritePrivateProfileString( "VIDEO", "ADAPTER", szTemp, g_szIniFileName );


	///
	itoa( 1, szTemp, 10 );
	WritePrivateProfileString( "TRIGGER","DETECTED", szTemp, g_szIniFileName );
	OnOK ();

	if(g_CmdLine && strlen (g_CmdLine) > 1 )
		ShellExecute( NULL, NULL, "TRose.exe", g_CmdLine, NULL, SW_SHOWNORMAL );
	else
	{
		ShellExecute( NULL, NULL, "ROSEonline.exe",NULL,NULL, SW_SHOWNORMAL );
		//MessageBox("Execute ROSEonline.exe.",NULL,MB_OK);
	
	}

}
LRESULT CTriggerDetectDlg::OnNcHitTest(CPoint point)
{
	

	return HTCAPTION;//CDialog::OnNcHitTest(point);
}
