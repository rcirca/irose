#include "stdafx.h"

#include "__d3d9.h"
#include "dxdiag.h"
#include "dispinfo.h"
IDirect3D9 *		CD3D9::s_pID3D			= NULL;


//-----------------------------------------------------------------------------
// Defines, and constants
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_BSTR_FREE(x)    if(x) { SysFreeString( x ); x = NULL; }
#define EXPAND(x)            x, sizeof(x)/sizeof(TCHAR)


/**********************************************************
 * Default Consturctor
 */
CD3D9::CD3D9 ()
{
	/*************************
	* Static Member 초기화
	*/
	m_pDxDiagProvider = NULL;
    m_pDxDiagRoot = NULL;
	m_strErr.erase ();
}

CD3D9::~CD3D9()
{
	DestroyDisplayInfo( m_vDisplayInfo );

	if( m_bCleanupCOM )
        CoUninitialize();
}

/**********************************************************
 *
 * Direct3D를 초기화 한다
 *
 */




BOOL CD3D9::Init (void)
{
	s_pID3D = Direct3DCreate9 (D3D_SDK_VERSION);

////////////////////////////////////////////////////////////////////////////////////
/// 추가코드   2005. 06. 14 - kjhpower
///////////////////////////////////////////////////////////////////////////////////

    HRESULT       hr;

    hr = CoInitialize( NULL );
    m_bCleanupCOM = SUCCEEDED(hr);

    hr = CoCreateInstance( CLSID_DxDiagProvider,
                           NULL,
                           CLSCTX_INPROC_SERVER,
                           IID_IDxDiagProvider,
                           (LPVOID*) &m_pDxDiagProvider);
    if( FAILED(hr) )
        goto LCleanup;
    if( m_pDxDiagProvider == NULL )
    {
        hr = E_POINTER;
        goto LCleanup;
    }

    // Fill out a DXDIAG_INIT_PARAMS struct and pass it to IDxDiagContainer::Initialize
    // Passing in TRUE for bAllowWHQLChecks, allows dxdiag to check if drivers are 
    // digital signed as logo'd by WHQL which may connect via internet to update 
    // WHQL certificates.    
    DXDIAG_INIT_PARAMS dxDiagInitParam;
    ZeroMemory( &dxDiagInitParam, sizeof(DXDIAG_INIT_PARAMS) );

    dxDiagInitParam.dwSize                  = sizeof(DXDIAG_INIT_PARAMS);
    dxDiagInitParam.dwDxDiagHeaderVersion   = DXDIAG_DX9_SDK_VERSION;
    dxDiagInitParam.pReserved               = NULL;

    hr = m_pDxDiagProvider->Initialize( &dxDiagInitParam );
    if( FAILED(hr) )
        goto LCleanup;

    hr = m_pDxDiagProvider->GetRootContainer( &m_pDxDiagRoot );
    if( FAILED(hr) )
        goto LCleanup;
//------------------------------------------------------------------------------
	return (s_pID3D != NULL);// <-- 원래 있던 코드.


	LCleanup:
		return hr;
}




/**********************************************************
 * ReleaseDevice
 */
void CD3D9::Release (void)
{
	if(s_pID3D)			
	{ 
		s_pID3D->Release ();			
		s_pID3D	= NULL; 
	}
}



/**********************************************************
 * 에러를 체크한다
 * @param hr 체크할 HRSEULT 값
 */
bool CD3D9::__CheckError (HRESULT hr)
{
	switch (hr)
	{
		case D3D_OK:
			// 성공
			return true;

		case D3DERR_INVALIDCALL:
			// 시스템에 있는 디스플레이 어댑터 갯수를 초과 하거나 같다. (uiAdapter가 틀린 값)
			return false;

		case D3DERR_NOTAVAILABLE:
			// surface format이 지원되지 않거나 , 주언지 포맷을 하드웨어가 지원하지 않음
			return false;

		case D3DERR_OUTOFVIDEOMEMORY: 
			//Direct3D does not have enough display memory to perform the operation. 
			return false;

		default:
			return false;
	}

	return false;
}


bool CD3D9::GetDisplayInfo (map<string, string> * mapDisInfo)
{
	mapDisInfo->clear ();
	if(mapDisInfo)
	{
		D3DADAPTER_IDENTIFIER9 DAI;
		if(s_pID3D)
		{
			HRESULT hRes = s_pID3D->GetAdapterIdentifier (D3DADAPTER_DEFAULT, D3DENUM_WHQL_LEVEL, &DAI);
			if(hRes	== S_OK)
			{
				mapDisInfo->insert (std::pair<string, string>("Driver"			, DAI.Driver));
				mapDisInfo->insert (std::pair<string, string>("Divice Name"		, DAI.DeviceName));
				mapDisInfo->insert (std::pair<string, string>("Description"		, DAI.Description));
				// char szDriverVersion[ 32 ];
				// sprintf (szDriverVersion, "%o %o", DAI.DriverVersion.HighPart, DAI.DriverVersion.LowPart);
				string strDrvVersion = ConvertDriverVersion (&DAI);
				mapDisInfo->insert (std::pair<string, string>("Drive Version"	, strDrvVersion));
			}
		}
	}

	return false;
}

string CD3D9::ConvertDriverVersion (D3DADAPTER_IDENTIFIER9 * pDI)
{
	char buff[ 1024 ];

	WORD Product 	= HIWORD(pDI->DriverVersion.HighPart);
	WORD Version 	= LOWORD(pDI->DriverVersion.HighPart);
	WORD SubVersion = HIWORD(pDI->DriverVersion.LowPart);
	WORD Build		= LOWORD(pDI->DriverVersion.LowPart);

	sprintf (buff, "adapter(%s) version(%d.%d.%d.%d)",
			pDI->Description, Product, Version, SubVersion, Build);

	return std::string (buff);
}


//-----------------------------------------------------------------------------
// Name: GetSystemInfo()
// Desc: Get the system info from the dll
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetSystemInfo( SystemInformation** ppSysInfo )
{
    HRESULT           hr;
    IDxDiagContainer* pObject         = NULL;
    DWORD             nCurCount         = 0;

    if( NULL == m_pDxDiagProvider )
        return E_INVALIDARG;

 //   int i;
    SystemInformation* pSysInfo = new SystemInformation;
    ZeroMemory( pSysInfo, sizeof(SystemInformation) );
    *ppSysInfo = pSysInfo;

    // Get the IDxDiagContainer object called "DxDiag_SystemInfo".
    // This call may take some time while dxdiag gathers the info.
    hr = m_pDxDiagRoot->GetChildContainer( L"DxDiag_SystemInfo", &pObject );
    if( FAILED( hr ) || pObject == NULL )
    {
        hr = E_FAIL;
        goto LCleanup;
    }
//=======================================================================================================	
// 2005. 06. 15 kjhpower    필요 없는 정보는 주석처리.
//=======================================================================================================

    //if( FAILED( hr = GetUIntValue( pObject, L"dwOSMajorVersion", &pSysInfo->m_dwOSMajorVersion ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetUIntValue( pObject, L"dwOSMinorVersion", &pSysInfo->m_dwOSMinorVersion ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetUIntValue( pObject, L"dwOSBuildNumber", &pSysInfo->m_dwOSBuildNumber ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetUIntValue( pObject, L"dwOSPlatformID", &pSysInfo->m_dwOSPlatformID ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetUIntValue( pObject, L"dwDirectXVersionMajor", &pSysInfo->m_dwDirectXVersionMajor ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetUIntValue( pObject, L"dwDirectXVersionMinor", &pSysInfo->m_dwDirectXVersionMinor ) ) )
    //    goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetStringValue( pObject, L"szDirectXVersionLetter", EXPAND(pSysInfo->m_szDirectXVersionLetter) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bDebug", &pSysInfo->m_bDebug ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bNECPC98", &pSysInfo->m_bNECPC98 ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetInt64Value( pObject, L"ullPhysicalMemory", &pSysInfo->m_ullPhysicalMemory ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetInt64Value( pObject, L"ullUsedPageFile", &pSysInfo->m_ullUsedPageFile ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetInt64Value( pObject, L"ullAvailPageFile", &pSysInfo->m_ullAvailPageFile ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bNetMeetingRunning", &pSysInfo->m_bNetMeetingRunning ) ) )
    //    goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetBoolValue( pObject, L"bIsD3D8DebugRuntimeAvailable", &pSysInfo->m_bIsD3D8DebugRuntimeAvailable ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsD3DDebugRuntime", &pSysInfo->m_bIsD3DDebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDInput8DebugRuntimeAvailable", &pSysInfo->m_bIsDInput8DebugRuntimeAvailable ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDInput8DebugRuntime", &pSysInfo->m_bIsDInput8DebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDMusicDebugRuntimeAvailable", &pSysInfo->m_bIsDMusicDebugRuntimeAvailable ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDMusicDebugRuntime", &pSysInfo->m_bIsDMusicDebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDDrawDebugRuntime", &pSysInfo->m_bIsDDrawDebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDPlayDebugRuntime", &pSysInfo->m_bIsDPlayDebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetBoolValue( pObject, L"bIsDSoundDebugRuntime", &pSysInfo->m_bIsDSoundDebugRuntime ) ) )
    //    goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetIntValue( pObject, L"nD3DDebugLevel", &pSysInfo->m_nD3DDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDDrawDebugLevel", &pSysInfo->m_nDDrawDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDIDebugLevel", &pSysInfo->m_nDIDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDMusicDebugLevel", &pSysInfo->m_nDMusicDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDPlayDebugLevel", &pSysInfo->m_nDPlayDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDSoundDebugLevel", &pSysInfo->m_nDSoundDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetIntValue( pObject, L"nDShowDebugLevel", &pSysInfo->m_nDShowDebugLevel ) ) )
    //    goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetStringValue( pObject, L"szWindowsDir", EXPAND(pSysInfo->m_szWindowsDir) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szBuildLab", EXPAND(pSysInfo->m_szBuildLab) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szDxDiagVersion", EXPAND(pSysInfo->m_szDxDiagVersion) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szSetupParamEnglish", EXPAND(pSysInfo->m_szSetupParamEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    if( FAILED( hr = GetStringValue( pObject, L"szProcessorEnglish", EXPAND(pSysInfo->m_szProcessorEnglish) ) ) )
        goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szSystemManufacturerEnglish", EXPAND(pSysInfo->m_szSystemManufacturerEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szSystemModelEnglish", EXPAND(pSysInfo->m_szSystemModelEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szBIOSEnglish", EXPAND(pSysInfo->m_szBIOSEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szPhysicalMemoryEnglish", EXPAND(pSysInfo->m_szPhysicalMemoryEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szCSDVersion", EXPAND(pSysInfo->m_szCSDVersion) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szDirectXVersionEnglish", EXPAND(pSysInfo->m_szDirectXVersionEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    if( FAILED( hr = GetStringValue( pObject, L"szDirectXVersionLongEnglish", EXPAND(pSysInfo->m_szDirectXVersionLongEnglish) ) ) )
        goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetStringValue( pObject, L"szMachineNameLocalized", EXPAND(pSysInfo->m_szMachineNameLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSLocalized", EXPAND(pSysInfo->m_szOSLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSExLocalized", EXPAND(pSysInfo->m_szOSExLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSExLongLocalized", EXPAND(pSysInfo->m_szOSExLongLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szLanguagesLocalized", EXPAND(pSysInfo->m_szLanguagesLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szPageFileLocalized", EXPAND(pSysInfo->m_szPageFileLocalized) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szTimeLocalized", EXPAND(pSysInfo->m_szTimeLocalized) ) ) )
    //    goto LCleanup; nCurCount++;

    //if( FAILED( hr = GetStringValue( pObject, L"szMachineNameEnglish", EXPAND(pSysInfo->m_szMachineNameEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSEnglish", EXPAND(pSysInfo->m_szOSEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSExEnglish", EXPAND(pSysInfo->m_szOSExEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szOSExLongEnglish", EXPAND(pSysInfo->m_szOSExLongEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    if( FAILED( hr = GetStringValue( pObject, L"szLanguagesEnglish", EXPAND(pSysInfo->m_szLanguagesEnglish) ) ) )
        goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szPageFileEnglish", EXPAND(pSysInfo->m_szPageFileEnglish) ) ) )
    //    goto LCleanup; nCurCount++;
    //if( FAILED( hr = GetStringValue( pObject, L"szTimeEnglish", EXPAND(pSysInfo->m_szTimeEnglish) ) ) )
    //    goto LCleanup; nCurCount++;

    //// Get the extended cpuid for args 0x80000008 through 0x80000018.  
    //// pSysInfo->m_ExtFuncBitmasks[0]  will contain extended cpuid info from arg 0x80000009 
    //// pSysInfo->m_ExtFuncBitmasks[15] will contain extended cpuid info from arg 0x80000018
    ////for( i=0; i<16; i++ )
    ////{
    ////    WCHAR strName[256];
    ////    WCHAR strName2[256];
    ////    StringCchPrintfW( strName, 256, L"ExtendedCPUFunctionBitmasks_0x800000%0.2x_bits", i+0x09 );

    ////    StringCchCopyW( strName2, 256, strName ); StringCchCatW( strName2, 256, L"0_31" );
    ////    if( FAILED( hr = GetUIntValue( pObject, strName2, &pSysInfo->m_ExtFuncBitmasks[i].dwBits0_31 ) ) )
    ////        goto LCleanup; nCurCount++;
    ////    StringCchCopyW( strName2, 256, strName ); StringCchCatW( strName2, 256, L"32_63" );
    ////    if( FAILED( hr = GetUIntValue( pObject, strName2, &pSysInfo->m_ExtFuncBitmasks[i].dwBits32_63 ) ) )
    ////        goto LCleanup; nCurCount++;
    ////    StringCchCopyW( strName2, 256, strName ); StringCchCatW( strName2, 256, L"64_95" );
    ////    if( FAILED( hr = GetUIntValue( pObject, strName2, &pSysInfo->m_ExtFuncBitmasks[i].dwBits64_95 ) ) )
    ////        goto LCleanup; nCurCount++;
    ////    StringCchCopyW( strName2, 256, strName ); StringCchCatW( strName2, 256, L"96_127" );
    ////    if( FAILED( hr = GetUIntValue( pObject, strName2, &pSysInfo->m_ExtFuncBitmasks[i].dwBits96_127 ) ) )
    ////        goto LCleanup; nCurCount++;
    ////}

#ifdef _DEBUG
    // debug check to make sure we got all the info from the object
    // normal clients should't do this
    if( FAILED( hr = pObject->GetNumberOfProps( &pSysInfo->m_nElementCount ) ) )
        return hr;
    if( pSysInfo->m_nElementCount != nCurCount )
        OutputDebugString( TEXT("Not all elements in pSysInfo recorded") );
#endif

LCleanup:
    SAFE_RELEASE( pObject );
    return hr;
}

//-----------------------------------------------------------------------------
// Name: GetDisplayInfo()
// Desc: Get the display info from the dll
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetDisplayInfo( vector<DisplayInfo*>& vDisplayInfo )
{
    HRESULT           hr;
    WCHAR             wszContainer[256];
    IDxDiagContainer* pContainer      = NULL;
    IDxDiagContainer* pObject         = NULL;
    DWORD             nInstanceCount    = 0;
    DWORD             nItem             = 0;
    DWORD             nCurCount         = 0;

    // Get the IDxDiagContainer object called "DxDiag_DisplayDevices".
    // This call may take some time while dxdiag gathers the info.
    if( FAILED( hr = m_pDxDiagRoot->GetChildContainer( L"DxDiag_DisplayDevices", &pContainer ) ) )
        goto LCleanup;
    if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
        goto LCleanup;

    for( nItem = 0; nItem < nInstanceCount; nItem++ )
    {
        nCurCount = 0;

        DisplayInfo* pDisplayInfo = new DisplayInfo;
        if (pDisplayInfo == NULL)
            return E_OUTOFMEMORY;
        ZeroMemory(pDisplayInfo, sizeof(DisplayInfo));

        // Add pDisplayInfo to vDisplayInfo
        vDisplayInfo.push_back( pDisplayInfo );

        hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
        if( FAILED( hr ) )
            goto LCleanup;
        hr = pContainer->GetChildContainer( wszContainer, &pObject );
        if( FAILED( hr ) || pObject == NULL )
        {
            if( pObject == NULL )
                hr = E_FAIL;
            goto LCleanup;
        }

//=======================================================================================================	
// 2005. 06. 15 kjhpower    필요 없는 정보는 주석처리.
//=======================================================================================================
        //if( FAILED( hr = GetStringValue( pObject, L"szDeviceName", EXPAND(pDisplayInfo->m_szDeviceName) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szDescription", EXPAND(pDisplayInfo->m_szDescription) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szKeyDeviceID", EXPAND(pDisplayInfo->m_szKeyDeviceID) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szKeyDeviceKey", EXPAND(pDisplayInfo->m_szKeyDeviceKey) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szManufacturer", EXPAND(pDisplayInfo->m_szManufacturer) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szChipType", EXPAND(pDisplayInfo->m_szChipType) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szDACType", EXPAND(pDisplayInfo->m_szDACType) ) ) )
        //    goto LCleanup; nCurCount++;
        //if( FAILED( hr = GetStringValue( pObject, L"szRevision", EXPAND(pDisplayInfo->m_szRevision) ) ) )
        //    goto LCleanup; nCurCount++;
		if( FAILED( hr = GetStringValue( pObject, L"szDisplayMemoryLocalized", EXPAND(pDisplayInfo->m_szDisplayMemoryLocalized) ) ) )
		   goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDisplayMemoryEnglish", EXPAND(pDisplayInfo->m_szDisplayMemoryEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
        if( FAILED( hr = GetStringValue( pObject, L"szDisplayModeLocalized", EXPAND(pDisplayInfo->m_szDisplayModeLocalized) ) ) )
           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDisplayModeEnglish", EXPAND(pDisplayInfo->m_szDisplayModeEnglish) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetUIntValue( pObject, L"dwWidth", &pDisplayInfo->m_dwWidth ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetUIntValue( pObject, L"dwHeight", &pDisplayInfo->m_dwHeight ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetUIntValue( pObject, L"dwBpp", &pDisplayInfo->m_dwBpp ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetUIntValue( pObject, L"dwRefreshRate", &pDisplayInfo->m_dwRefreshRate ) ) )
 //           goto LCleanup; nCurCount++;
 //       
 //       if( FAILED( hr = GetStringValue( pObject, L"szMonitorName", EXPAND(pDisplayInfo->m_szMonitorName) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szMonitorMaxRes", EXPAND(pDisplayInfo->m_szMonitorMaxRes) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverName", EXPAND(pDisplayInfo->m_szDriverName) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverVersion", EXPAND(pDisplayInfo->m_szDriverVersion) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverAttributes", EXPAND(pDisplayInfo->m_szDriverAttributes) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverLanguageEnglish", EXPAND(pDisplayInfo->m_szDriverLanguageEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverLanguageLocalized", EXPAND(pDisplayInfo->m_szDriverLanguageLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverDateEnglish", EXPAND(pDisplayInfo->m_szDriverDateEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverDateLocalized", EXPAND(pDisplayInfo->m_szDriverDateLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetIntValue( pObject, L"lDriverSize", &pDisplayInfo->m_lDriverSize ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szMiniVdd", EXPAND(pDisplayInfo->m_szMiniVdd) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szMiniVddDateLocalized", EXPAND(pDisplayInfo->m_szMiniVddDateLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szMiniVddDateEnglish", EXPAND(pDisplayInfo->m_szMiniVddDateEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetIntValue( pObject, L"lMiniVddSize", &pDisplayInfo->m_lMiniVddSize ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szVdd", EXPAND(pDisplayInfo->m_szVdd) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetBoolValue( pObject, L"bCanRenderWindow", &pDisplayInfo->m_bCanRenderWindow ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bDriverBeta", &pDisplayInfo->m_bDriverBeta ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bDriverDebug", &pDisplayInfo->m_bDriverDebug ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bDriverSigned", &pDisplayInfo->m_bDriverSigned ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bDriverSignedValid", &pDisplayInfo->m_bDriverSignedValid ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDeviceIdentifier", EXPAND(pDisplayInfo->m_szDeviceIdentifier) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDriverSignDate", EXPAND(pDisplayInfo->m_szDriverSignDate) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetUIntValue( pObject, L"dwDDIVersion", &pDisplayInfo->m_dwDDIVersion ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDDIVersionEnglish", EXPAND(pDisplayInfo->m_szDDIVersionEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDDIVersionLocalized", EXPAND(pDisplayInfo->m_szDDIVersionLocalized) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetUIntValue( pObject, L"iAdapter", &pDisplayInfo->m_iAdapter ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szVendorId", EXPAND(pDisplayInfo->m_szVendorId) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDeviceId", EXPAND(pDisplayInfo->m_szDeviceId) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szSubSysId", EXPAND(pDisplayInfo->m_szSubSysId) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szRevisionId", EXPAND(pDisplayInfo->m_szRevisionId) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetUIntValue( pObject, L"dwWHQLLevel", &pDisplayInfo->m_dwWHQLLevel ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetBoolValue( pObject, L"bNoHardware", &pDisplayInfo->m_bNoHardware ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bDDAccelerationEnabled", &pDisplayInfo->m_bDDAccelerationEnabled ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"b3DAccelerationExists", &pDisplayInfo->m_b3DAccelerationExists ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"b3DAccelerationEnabled", &pDisplayInfo->m_b3DAccelerationEnabled ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bAGPEnabled", &pDisplayInfo->m_bAGPEnabled ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bAGPExists", &pDisplayInfo->m_bAGPExists ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetBoolValue( pObject, L"bAGPExistenceValid", &pDisplayInfo->m_bAGPExistenceValid ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDXVAModes", EXPAND(pDisplayInfo->m_szDXVAModes) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetStringValue( pObject, L"szDDStatusLocalized", EXPAND(pDisplayInfo->m_szDDStatusLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szDDStatusEnglish", EXPAND(pDisplayInfo->m_szDDStatusEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szD3DStatusLocalized", EXPAND(pDisplayInfo->m_szD3DStatusLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szD3DStatusEnglish", EXPAND(pDisplayInfo->m_szD3DStatusEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szAGPStatusLocalized", EXPAND(pDisplayInfo->m_szAGPStatusLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szAGPStatusEnglish", EXPAND(pDisplayInfo->m_szAGPStatusEnglish) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetStringValue( pObject, L"szNotesLocalized", EXPAND(pDisplayInfo->m_szNotesLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szNotesEnglish", EXPAND(pDisplayInfo->m_szNotesEnglish) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetStringValue( pObject, L"szRegHelpText", EXPAND(pDisplayInfo->m_szRegHelpText) ) ) )
 //           goto LCleanup; nCurCount++;

 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultDDLocalized", EXPAND(pDisplayInfo->m_szTestResultDDLocalized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultDDEnglish", EXPAND(pDisplayInfo->m_szTestResultDDEnglish) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D7Localized", EXPAND(pDisplayInfo->m_szTestResultD3D7Localized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D7English", EXPAND(pDisplayInfo->m_szTestResultD3D7English) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D8Localized", EXPAND(pDisplayInfo->m_szTestResultD3D8Localized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D8English", EXPAND(pDisplayInfo->m_szTestResultD3D8English) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D9Localized", EXPAND(pDisplayInfo->m_szTestResultD3D9Localized) ) ) )
 //           goto LCleanup; nCurCount++;
 //       if( FAILED( hr = GetStringValue( pObject, L"szTestResultD3D9English", EXPAND(pDisplayInfo->m_szTestResultD3D9English) ) ) )
 //           goto LCleanup; nCurCount++;

#ifdef _DEBUG
        // debug check to make sure we got all the info from the object
        if( FAILED( hr = pObject->GetNumberOfProps( &pDisplayInfo->m_nElementCount ) ) )
            return hr;
        if( pDisplayInfo->m_nElementCount != nCurCount )
            OutputDebugString( TEXT("Not all elements in pDisplayInfo recorded") );
#endif

        GatherDXVA_DeinterlaceCaps( pObject, pDisplayInfo->m_vDXVACaps );

        SAFE_RELEASE( pObject );
    }

LCleanup:
    SAFE_RELEASE( pObject );
    SAFE_RELEASE( pContainer );

	return hr;
}


HRESULT CD3D9::GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, TCHAR* strValue, int nStrLen )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_BSTR )
        return E_INVALIDARG;
    
#ifdef _UNICODE
    wcsncpy( strValue, var.bstrVal, nStrLen-1 );
#else
    wcstombs( strValue, var.bstrVal, nStrLen );   
#endif
    strValue[nStrLen-1] = TEXT('\0');
    VariantClear( &var );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetUIntValue()
// Desc: Get a UINT value from a IDxDiagContainer object
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetUIntValue( IDxDiagContainer* pObject, WCHAR* wstrName, DWORD* pdwValue )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_UI4 )
        return E_INVALIDARG;

    *pdwValue = var.ulVal;
    VariantClear( &var );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetIntValue()
// Desc: Get a INT value from a IDxDiagContainer object
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetIntValue( IDxDiagContainer* pObject, WCHAR* wstrName, LONG* pnValue )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_I4 )
        return E_INVALIDARG;

    *pnValue = var.lVal;
    VariantClear( &var );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetBoolValue()
// Desc: Get a BOOL value from a IDxDiagContainer object
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetBoolValue( IDxDiagContainer* pObject, WCHAR* wstrName, BOOL* pbValue )
{
    HRESULT hr;
    VARIANT var;
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    if( var.vt != VT_BOOL )
        return E_INVALIDARG;

    *pbValue = ( var.boolVal != 0 );
    VariantClear( &var );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetInt64Value()
// Desc: Get a ULONGLONG value from a IDxDiagContainer object
//-----------------------------------------------------------------------------
HRESULT CD3D9::GetInt64Value( IDxDiagContainer* pObject, WCHAR* wstrName, ULONGLONG* pullValue )
{
    HRESULT hr;
    VARIANT var;	
    VariantInit( &var );

    if( FAILED( hr = pObject->GetProp( wstrName, &var ) ) )
        return hr;

    // 64-bit values are stored as strings in BSTRs
    if( var.vt != VT_BSTR )
        return E_INVALIDARG;

    *pullValue = _wtoi64( var.bstrVal );
    VariantClear( &var );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: GatherDXVA_DeinterlaceCaps
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CD3D9::GatherDXVA_DeinterlaceCaps( IDxDiagContainer* pParent, 
                                                 vector<DxDiag_DXVA_DeinterlaceCaps*>& vDXVACaps )
{
    HRESULT            hr;
    WCHAR              wszContainer[256];
    IDxDiagContainer*  pContainer      = NULL;
    IDxDiagContainer*  pObject         = NULL;
    DWORD              nInstanceCount    = 0;
    DWORD              nItem             = 0;
    DWORD              nCurCount         = 0;

    if( FAILED( hr = pParent->GetChildContainer( L"DXVADeinterlaceCaps", &pContainer ) ) )
        goto LCleanup;
    if( FAILED( hr = pContainer->GetNumberOfChildContainers( &nInstanceCount ) ) )
        goto LCleanup;

    for( nItem = 0; nItem < nInstanceCount; nItem++ )
    {
        nCurCount = 0;

        DxDiag_DXVA_DeinterlaceCaps* pDXVANode = new DxDiag_DXVA_DeinterlaceCaps;
        if (pDXVANode == NULL)
            return E_OUTOFMEMORY;

        // Add pDXVANode to vDXVACaps
        vDXVACaps.push_back( pDXVANode );

        hr = pContainer->EnumChildContainerNames( nItem, wszContainer, 256 );
        if( FAILED( hr ) )
            goto LCleanup;
        hr = pContainer->GetChildContainer( wszContainer, &pObject );
        if( FAILED( hr ) || pObject == NULL )
        {
            if( pObject == NULL )
                hr = E_FAIL;
            goto LCleanup;
        }

        if( FAILED( hr = GetStringValue( pObject, L"szD3DInputFormat", EXPAND(pDXVANode->szD3DInputFormat) ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetStringValue( pObject, L"szD3DOutputFormat", EXPAND(pDXVANode->szD3DOutputFormat) ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetStringValue( pObject, L"szGuid", EXPAND(pDXVANode->szGuid) ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetStringValue( pObject, L"szCaps", EXPAND(pDXVANode->szCaps) ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetUIntValue( pObject, L"dwNumPreviousOutputFrames", &pDXVANode->dwNumPreviousOutputFrames ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetUIntValue( pObject, L"dwNumForwardRefSamples", &pDXVANode->dwNumForwardRefSamples ) ) )
            goto LCleanup; nCurCount++;
        if( FAILED( hr = GetUIntValue( pObject, L"dwNumBackwardRefSamples", &pDXVANode->dwNumBackwardRefSamples ) ) )
            goto LCleanup; nCurCount++;

#ifdef _DEBUG
        // debug check to make sure we got all the info from the object
        if( FAILED( hr = pObject->GetNumberOfProps( &pDXVANode->m_nElementCount ) ) )
            goto LCleanup;
        if( pDXVANode->m_nElementCount != nCurCount )
            OutputDebugString( TEXT("Not all elements in pDXVANode recorded") );
#endif

        SAFE_RELEASE( pObject );
    }

LCleanup:
    SAFE_RELEASE( pObject );
    SAFE_RELEASE( pContainer );
    return hr;
}


void CD3D9::DestroyDisplayInfo( vector<DisplayInfo*>& vDisplayInfo )
{
    DisplayInfo* pDisplayInfo;
    vector<DisplayInfo*>::iterator iter; 
    for( iter = vDisplayInfo.begin(); iter != vDisplayInfo.end(); iter++ )
    {
        pDisplayInfo = *iter;

        DxDiag_DXVA_DeinterlaceCaps* pDXVANode;
        vector<DxDiag_DXVA_DeinterlaceCaps*>::iterator iterDXVA; 
        for( iterDXVA = pDisplayInfo->m_vDXVACaps.begin(); iterDXVA != pDisplayInfo->m_vDXVACaps.end(); iterDXVA++ )
        {
            pDXVANode = *iterDXVA;
            SAFE_DELETE( pDXVANode );
        }
        pDisplayInfo->m_vDXVACaps.clear();

        SAFE_DELETE( pDisplayInfo );
    }
    vDisplayInfo.clear();
}

