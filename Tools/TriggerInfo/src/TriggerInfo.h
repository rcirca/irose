#ifndef __TRIGGERINFO_H__
#define __TRIGGERINFO_H__

#ifdef TRIGGERINFO_EXPORTS
#define TRIGGERINFO_API __declspec(dllexport)
#else
#define TRIGGERINFO_API __declspec(dllimport)
#endif

#include <string>

// system information 을 저장하는 파일
#define SYSINFO_FILE		"sysinfo.dat"
#define SYSINFO_FILE_WEB	"sysinfow.dat"
// adapter information을 저장하는 파일
#define ADPTINFO_FILE "sysadpt.dat"

/********************************************************************
 * 
 * TI_GatherInfo - 정보를 모은다
 * hWnd			: D3D 를 이용하기 위해서
 *
 */
TRIGGERINFO_API const char * TI_GatherSysInfo (void);

/********************************************************************
 * 
 * TI_GetInfo - 알아 낸 정보를 문자열 버퍼에 복사한다
 * buff			: 여기에 카피
 * iLength		: buff의 크기
 *
 */
TRIGGERINFO_API const char * TI_GetSysInfo (void);

TRIGGERINFO_API const char * TI_GetSysInfoForWeb (void);

/********************************************************************
 * 
 * TI_SendHttpPostData - Post 방식으로 데이터를 전송한다. 데이터가 클 경우
 * szServerIP	: 웹서버 IP		예) "218.55.179.114"
 * szPage		: 페이지		예) "/testaa/receive.php"
 * pData		: 데이터		예) "v1=kkkkk&v2=rrrrrddd&v3=abcdefgh.."
 *
 */


/******************************************************************
 * Dectect되어 있는 정보를 저장하고 있는 파일에서 정보를 읽어온다
 */
TRIGGERINFO_API const char * TI_ReadSysInfoFile (void);


TRIGGERINFO_API bool TI_SendHttpPostData (const char * szServerIP, const char * szPage, const char * pData);

/********************************************************************
 * 
 * TI_SendHttpQueryData - 쿼리 방식으로 데이터를 전송한다. 데이터가 작을 경우
 * szURL	: 웹서버 IP		예) "http:://218.55.179.114/aa.php?v1=kkkkk&v2=rrrrrddd&v3=abcdefgh.."
 *
 */
TRIGGERINFO_API bool TI_SendHttpQueryData (const char * szURL);

struct SystemResolution{
      
	int pixel_width;
	int pixel_height;
	int pixel_color;
    int frequency; 
	int	adaptor_number;
};

struct SystemAdapter{
    
	char Driver[ 100 ];
    char Description[ 100 ];
    char DeviceName[ 32 ];
    int  AdapterNumber;
	int	 nResolution;
    
	SystemResolution *pResolution;
};


struct AdapterInformation
{
  	  SystemAdapter *	pAdapter;
	  int				iAdapterCNT;
};


// Resolution 정보를 Release한다
TRIGGERINFO_API void TI_ReleaseAdptInfo (void);

// 파일에 저장되어 있는 정보를 가지고 온다.
TRIGGERINFO_API AdapterInformation * TI_ReadAdptInfoFile (void);

// 현재 Dectect되어 있는 정보를 가지고 온다.
TRIGGERINFO_API AdapterInformation * TI_GetAdptInfo (void);

// Resolution 관련 정보를 Dectect한다
TRIGGERINFO_API AdapterInformation * TI_GatherAdptInfo (void);

TRIGGERINFO_API bool TI_DXInfo(void);


#endif
