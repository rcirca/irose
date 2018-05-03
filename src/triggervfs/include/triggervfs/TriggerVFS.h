#ifndef __TRIGGERVFS_H__
#define __TRIGGERVFS_H__

#include <windows.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>
#include "DllMain.h"
#include <stdio.h>

#pragma warning (disable:4251)


#ifdef __DLL

	#ifdef TRIGGERVFS_EXPORTS
		#define TRIGGERVFS_API __declspec(dllexport)
	#else
		#define TRIGGERVFS_API __declspec(dllimport)
	#endif

#else
	#define TRIGGERVFS_API
#endif




#define		VHANDLE		void *
typedef		 void   (__stdcall *  VCALLBACK_CLEARBLANKALL) ( int );

/// 파일엔트리를 저장하고. 이 파일은 vfs파일 앞부분에 있음
struct FileEntry 
{
	std::string		sFileName;			/// 파일이름; 길이:문자열(NULL포함) 형태로 저장됨
	long			lFileOffset;		/// 파일오프셋
	long			lFileLength;		/// 파일이 차지하고 있는 실제 길이
	long			lBlockSize;			/// 차지하고 있는 블륵의 크기			(2003.11.19 추가)
	BYTE			cDeleted;			/// 삭제여부: 0 = 삭제 안 됨. 1 = 삭제	(2003.11.19 추가)
	BYTE			btCompressType;		/// 압축형태. 0 = 압축안함, 1= ZIP ...
	BYTE			btEncType;			/// Encryption 타입. 0 = Ecryption 안 함. 1 = Encryption 타입(1) ...
	DWORD			dwVersion;			/// 파일의 버젼
	DWORD			dwCRC;				/// CRC값
};

/// #define	SIZEOF_FILEENTRY_EXCPTSTRING	(sizeof (short) + sizeof (long) * 3 + sizeof (BYTE) * 3 + sizeof (short) + sizeof (DWORD))

/********************************************************************************
 * VFileInfo 파일에 대한 Version 정보와 CRC값을 알아냄
 */
struct VFileInfo
{
	DWORD dwVersion;				/// 버젼 값
	DWORD dwCRC;					/// CRC 값
};



/// vfs파일안에 있는 파일작업을 하기 위한 포인터
struct VFileHandle
{
	FILE	*		fp;				/// vfs파일의 파일핸들. VFile에서 그대로 카피한다
	char *			pData;			// Memory Mapped IO data
	int				iAllocOffset;	// pData + iAllocOffset = Read Data Address
	std::string		sFileName;		/// 현재 오픈한 파일이름. vfs파일내의 파일이름.
	long			lStartOff;		/// 현재파일의 시작 Offset
	long			lEndOff;		/// 현재파일의 마지막 Offset. dwEndOff + 1 이 End Of File 이다.
	long			lCurrentOffset;	/// 현재 File Indicator Offset. 자기의 Start 오프셋 기준으로
	BYTE			btFileType;		/// 0 = Packed File , 1 = Normal File outside
	BYTE			btEncrypted;	// 0 = normal data , 1 = data encryption used
	VHANDLE			hVFS;
	VHANDLE			hCVFS;			// the pointer to CVFS by which this file handle was opened.
} ;


/**********************************************************************************************
 *                                      VFS Interface
 **********************************************************************************************
 */
extern char UseEncryption;
#ifdef	__cplusplus
extern "C" {
#endif

/**
 * vfseek를 위한 Constant
 */
#define VFSEEK_CUR SEEK_CUR /// 현재 위치
#define VFSEEK_END SEEK_END /// 파일의 끝
#define VFSEEK_SET SEEK_SET	/// 파일의 앞

TRIGGERVFS_API bool __stdcall InitVFS (DWORD key);

TRIGGERVFS_API bool __stdcall __ConvertPath (const char * path , char * path2 );

/// Index File과 VFS파일을 연다. 가능한 모드 : "w+", "r+", "r"
TRIGGERVFS_API VHANDLE __stdcall OpenVFS (const char * FileName, const char * Mode = "r");

/// Index File과 VFS파일을 닫는다
TRIGGERVFS_API void __stdcall CloseVFS (VHANDLE hVFS);

/// Index파일에 Pack파일을 추가한다.
TRIGGERVFS_API bool __stdcall VAddVfs (VHANDLE hVFS, const char * VfsName);

/// 파일을 추가한다
TRIGGERVFS_API short __stdcall VAddFile (VHANDLE hVFS, const char * VfsName, const char *FileName, const char * TargetName, DWORD dwVersion, DWORD dwCrc, 
			   BYTE btEncType, BYTE btCompress, bool bUseDel);

/// 파일을 제거한다. 공백처리하는 버젼임. 파일을 정리하기 위해서는 VClearBlankAll 를 호출할것
/// Pack파일을 지정하지 않고 지움
TRIGGERVFS_API short __stdcall VRemoveFile (VHANDLE hVFS, const char *FileName);

/// index에 포함된 모든 Pack파일의 공백을 제거한다
TRIGGERVFS_API void __stdcall VClearBlankAll (VHANDLE hVFS, VCALLBACK_CLEARBLANKALL CallBackProc = NULL);

/// 파일에 대한 정보를 가져온다
TRIGGERVFS_API void __stdcall VGetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo, bool bCalCrc);

/// 파일 정보를 갱신한다
TRIGGERVFS_API bool __stdcall VSetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo);

/// 인덱스파일내에 있는 Pack파일의 갯수를 알아낸다
TRIGGERVFS_API int __stdcall VGetVfsCount (VHANDLE hVFS);

/// 인덱스파일에서 Pack파일 리스트를 가져온다
TRIGGERVFS_API int __stdcall VGetVfsNames (VHANDLE hVFS, char **ppFiles, DWORD dwNum, short dwMaxPath);

/// Vfs파일내에 있는 파일의 갯수를 알아낸다
TRIGGERVFS_API int __stdcall VGetFileCount (VHANDLE hVFS, const char *VfsName);
/// 인덱스파일에 있는 총 파일갯수를 알아낸다.
TRIGGERVFS_API int __stdcall VGetTotFileCount (VHANDLE hVFS);

/// Pack파일에서 파일이름을 가져온다
TRIGGERVFS_API int __stdcall VGetFileNames (VHANDLE hVFS, const char *VfsName, char **FileName, int nNum, int nMax);

/// 기준버젼을 가져옴
TRIGGERVFS_API DWORD __stdcall VGetStdVersion (VHANDLE hVFS);

/// 인덱스파일의 기준버젼을 설정함
TRIGGERVFS_API void __stdcall VSetStdVersion (VHANDLE hVFS, DWORD dwVersion);

/// 현재버젼을 가져옴
TRIGGERVFS_API DWORD __stdcall VGetCurVersion (VHANDLE hVFS);

/// 인덱스파일의 현재버젼을 설정함
TRIGGERVFS_API void __stdcall VSetCurVersion (VHANDLE hVFS, DWORD dwVersion);




/// 파일을 오픈한다
TRIGGERVFS_API VFileHandle * __stdcall VOpenFile (const char * FileName, VHANDLE hVFS);

/// 파일을 닫는다
TRIGGERVFS_API void __stdcall VCloseFile (VFileHandle *hVFH);

/// 파일 읽을때
TRIGGERVFS_API size_t __stdcall vfread (void *buffer, size_t size, size_t count, VFileHandle *pVFH);

/// 파일 전체 내용의 포인터 얻어오기
TRIGGERVFS_API void * __stdcall vfgetdata (size_t * psize, VFileHandle *pVFH);

/// 파일 indicator의 위치를 이동
TRIGGERVFS_API int __stdcall vfseek (VFileHandle *pVFH, long offset, int origin );

/// 오픈한 파일의 indicator 위치값을 리턴. 실제로 File Offset은 vftell이 리턴하는 값이 아닐수 있다.
TRIGGERVFS_API long __stdcall vftell (VFileHandle *pVFH);

/// 파일의 끝에 왔는지 확인
TRIGGERVFS_API int __stdcall vfeof (VFileHandle *pVFH);

/// 파일의 크기 구하기
TRIGGERVFS_API size_t __stdcall vfgetsize (VFileHandle *pVFH);
TRIGGERVFS_API size_t __stdcall VGetFileLength (VHANDLE hVFS, const char *FileName);


/// 파일이 존재하는지 조사한다
TRIGGERVFS_API bool __stdcall VFileExists (VHANDLE hVFS, const char * FileName);

/// Pack파일안에서만 파일이 존재유무를 검사한다
TRIGGERVFS_API bool __stdcall VFileExistsInVfs (VHANDLE hVFS, const char * FileName);

/***************************************************************
 * 
 * Error Dealing
 *
 */
// 마지막 에러상태를 가져온다
TRIGGERVFS_API short __stdcall VGetError (void );

/// FileName에 대한 인덱스 정보와 VFS내의 실제 데이터가 일치하는지 체크한다
TRIGGERVFS_API short __stdcall VTestFile (VHANDLE hVFS, const char * FileName);

/// VFS내의 파일을 읽어서 Crc값을 계산한다
TRIGGERVFS_API DWORD __stdcall ComputeCrc (VHANDLE hVFS, const char * FileName);

#ifdef	__cplusplus
}
#endif


extern short g_nLastErr;

#define VERR_NONE					0
#define VERR_DELETE_CANTFIND		1
#define VERR_MEM_ALLOCFAIL			2


/***************************************************************
 * VTestFile 에 대한 리턴값
 */
#define	VTEST_SUCCESS				0
#define	VTEST_INVALIDHANDLE			1	// VFS Handle이 유효하지 않다.
#define	VTEST_CANTOPEN				2	// 파일을 오픈할수 없다
#define	VTEST_NOTENOUGHMEM			3	// 테스트를 위한 메모리를 확보 실패
#define	VTEST_LENGTHNOTMATCH		4	// 파일 크기가 인덱스 정보와 실제 읽은 크기와 일치하지 않는다
#define	VTEST_CRCNOTMATCH			5	// CRC 값이 일치하지 않는다.
#define	VTEST_FILENOTEXISTS			6	// 인덱스내에 혹은 외부파일로  존재하지 않는다
#define	VTEST_INVALIDRANGE			7	// 파일의 위치가 VFS파일 크기보다 크다
#define VTEST_OUTFILE				8	// 외부 파일로 존재한다
#define VTEST_CANTKNOWVFSINFO		9	// VFS 파일 정보를 알아 낼수 없다
#define VTEST_ZEROLENGTH			10	// 인덱스파일내에서 파일의 길이가 0이다

struct VfsInfo 
{
	long lStartOff;	// 0부터 시작하지 않을 수도 있다.
	long lEndOff;	
};


/******************************************************************
 * VAddFile  Error Code
 */
#define	VADDFILE_SUCCESS				0	// 성공
#define	VADDFILE_INVALIDHANDLE			1	// 핸들이 유효하지 않다 OpenVFS의 리턴값이 유효하지 않았음.
#define	VADDFILE_INVALIDVFS				2	// VFS 파일을 찾을 수 없다
#define	VADDFILE_CANTOPENFILE			3	// 소스 파일을 오픈할수 없다
#define	VADDFILE_EXISTSALREADY			4	// 타겟 파일이 VFS안에 이미 존재한다 VRemoveFile 이 실패한 경우
#define	VADDFILE_DONTEXIST				5	// 소스 파일이 없다
#define	VADDFILE_MEMALLOCFAIL			6	// 메모리 할당 실패
#define	VADDFILE_CANTWRITE				7	// VFS에 fwrite 할수 없거나 fflush 할수 없다
#define	VADDFILE_CANTFLUSH				8	// VFS에 fwrite 할수 없거나 fflush 할수 없다
#define	VADDFILE_CVFS_AddFile_FAILURE	9	// VFS에 fwrite 할수 없거나 fflush 할수 없다
#define	VADDFILE_ZEROFILESIZE			10	// 파일 크기가 0
#define	VADDFILE_FIOERROR_ETC			11	// fwrite나 fflush에서 발생한 에러인데 CErrorInfo::GetLastError 값이 short 값을 넘어 갈때
#define VADDFILE_CVFS_AddFile_NONOE		12

#define VADDFILE_FIXEVALUE_CANTWRITE		15000	// fwrite 가 실패한 경우 
#define VADDFILE_FIXEVALUE_CANTFLUSH		20000	// fflush 가 실패한 경우


/******************************************************************
 * VRemoveFile  Error Code
 */
#define VRMVFILE_SUCCESS			0	// 성공
#define VRMVFILE_INVALIDVFS			1	// VFS 파일을 찾을 수 없다
#define VRMVFILE_MEMALLOCFAIL		2	// Memory 할당 실패
#define VRMVFILE_DONTEXIST			3	// 삭제할려는 파일을 찾을 수가 없다
#define VRMVFILE_INVALIDHANDLE		4	// VHANDLE이 유효하지 않다


#endif