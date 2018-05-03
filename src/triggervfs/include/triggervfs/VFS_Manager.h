#ifndef __VFS_MANAGER_H__
#define __VFS_MANAGER_H__

#include <windows.h>
#include <string>
#include <vector>
#include <map>
#include "VFS.h"


/// 파일헤더에 있는 정보를 여기에 읽어 들인다
struct VEntry 
{	
	std::string		sVFSName;			/// vfs파일명
	long			lStartOfEntry;		/// 인덱스 시작
	CVFS *			pVFS;				/// CVFS 인스턴스
};

/// 파일 헤더에 있는 VEntry 하나의 크기
#define		SIZE_VENTRY_EXCEPTSTRING (sizeof (long) + sizeof (short))


/**********************************************************************************************
 * 여러개의 vfs파일을 관리할 수 있는 Container Class
 */
class CVFS_Manager 
{
private:
	std::string					m_sIdxFileName;		/// 인덱스 파일명
	std::string					m_sBasePath;		/// 인덱스 파일이 있는 폴더 경로
	std::vector<VEntry *>		m_vecVFS;			/// 여기에 각각의 vfs instance 를 저장
	FILE *						m_fpIDX;			/// Index File 핸들
	DWORD						m_dwNumOfEntry;		/// VEntry의 갯수 = 묶음 파일의 갯수
	WORD						m_wStdVersion[ 2 ];	/// 기준 버젼을 저장
	WORD						m_wCurVersion[ 2 ];	/// 현재 버젼
	std::string					m_strIdxOpenMode;	/// Index파일의 오픈모드


private:
	/// 빈 인덱스 파일을 하나 만든다
	bool __WriteBlankIndexFile (void);

	/// VEntry Table 을 읽는다
	bool __ReadVEntry (void);
	/// 현재 File Offset에 VEntry를 기록한다
	void __WriteVEntry (VEntry * pVE);

	/// pack안에 있는 파일 이름 VEntry를 찾는다
	VEntry *	__FindVEntryWithFile (const char *FileName);
	
	/// VFS파일에 대한 엔트리가 존재하는 검색한다. Vfs파일의 존재 유무를 검사할때 사용해도 됨
	VEntry *	__FindEntry (const char *FileName);
	long		__FindEntryIndex (const char *FileName);
	/// 파일에 쓰기 위한 VEntry의 크기
	long		__SizeOfVEntry (VEntry *VE);
	/// Entry를 스킾한다
	void		__SkipVEntry (VEntry *VE);
	/// 파일 헤더를 쓴다
	void		__WriteIndexHeader (char * Version, DWORD dwNum);
	/// Pack파일안의 파일이름으로 VEntry의 인덱스를 찾는다
	DWORD		__FindEntryIndexWithFile (const char *FileName);
	// Memory Mapped IO를 테스트 한다
	bool		__TestMapIO (const char * szFileName);
	/// 인덱스 파일을 fflush한다.
	void		__FlushIdxFile (void);
	/// 오픈 모드를 체크한다
	void		__CheckOpenMode ( const char * InputMode, char ModifiedMode[ 16 ] );

public:
	CVFS_Manager ();
	~CVFS_Manager ();

	/// 인덱스 파일을 오픈한다
	bool Open (const char *IndexFile, const char * Mode);

	/// vfs 파일을 추가한다
	bool AddVFS (const char *VfsName);
	/// VFS를 제거한다
	bool RemoveVFS (const char *VfsName);

	/// 인덱스 파일을 닫는다
	void Close (void);

	/// vfs에 파일을 추가한다
	short AddFile (const char *VfsName, const char *File, const char * TargetName, 
		DWORD dwVersion, DWORD dwCrc, BYTE btEncType = 0, BYTE btCompres = 0, 
		bool bUseDel = true);

	/// Pack파일에 찾아서, 파일을 한개 제거한다. 
	short RemoveFile (const char *File);

	/// 파일 여러개 제거하고. 그냥 빈공간으로 남겨두기
	bool RemoveFiles (const char *VfsName, const char **Files, int iNum);

	/// vfs파일에서 파일을 오픈한다
	VFileHandle * OpenFile (const char *FileName);
	/// OpenFile로 오픈한 파일을 닫는다
	void CloseFile (VFileHandle *pVFH);

	/// 파일이름을 가져온다
	DWORD GetFileNames (const char *VfsName, char **pFiles, DWORD nFiles, int nMaxPath);
	/// 파일갯수를 알아낸다
	DWORD GetFileCount (const char *VfsName);
	/// 인덱스 파일안에 있는 파일의 총갯수를 알아낸다.
	DWORD GetFileTotCount (void);
	/// 빈공간의 크기를 조사한다.
	DWORD GetSizeOfBlankArea (void);

	/// 이 index파일에서 관리되는 묶음 파일의 갯수를 알아낸다
	DWORD GetVFSCount (void);
	/// VFS파일이름을 가져온다
	DWORD GetVfsNames (char **ppFiles, DWORD dwMax, short dwMaxPath);
	/// 삭제되었지만 블록은 남아있는 엔트리의 갯수를 리턴
	DWORD GetDelCnt (const char *VfsName);
	/// 파일엔트리 테이블의 시작오프셋을 업데이트한다
	DWORD GetFileCntWithHole (const char *VfsName);
	
	/// 공백을 지운다
	bool ClearBlank (const char * VfsName);
	/// 모든 Pack파일의 공백을 정리한다
	bool ClearBlankAll (VCALLBACK_CLEARBLANKALL CallBackProc);
	
	/// 파일 크기를 알아낸다
	long GetFileLength (const char *FileName);
	
	/// 묶음 파일이 인덱스파일안에 있는지 확인한다
	bool VfsExists (const char *VfsName);
	
	/// 파일존재하는지 검사
	bool FileExists (const char * FileName);
	/// Pack파일안에서만 파일존재하는지 검사
	bool FileExistsInVfs (const char * FileName);
	
	// 파일에 대한 정보를 알아냄, bCalCrc = true 이면 CVFS_Manager::GetFileInfo 에서
	// 실제 데이터로 계산을 한다. 
	void GetFileInfo (const char * FileName, VFileInfo * pFileInfo, bool bCalCrc);
	/// 파일에 대한 정보를 갱신함
	bool SetFileInfo (const char * FileName, VFileInfo * pFileInfo);

	
	/// 기준 버젼을 알아냄
	DWORD GetStdVersion (void);
	/// 버젼 정보를 기록한다
	void SetStdVersion (DWORD dwVersion);
	void SetStdVersion (WORD wHiVer, WORD wLoVer);

	/// 적용된 버젼 알아냄
	DWORD GetCurVersion (void);
	/// 적용된 버젼 기록함
	void SetCurVersion (DWORD dwVersion);
	void SetCurVersion (WORD wHiVer, WORD wLoVer);

	// FileName 이 인덱스데이터와 실제 데이터가 일치하는지 체크한다
	short __stdcall TestFile ( const char * FileName);

	// VFS 이름으로 VFS Entry를 조사한다
	VEntry * CVFS_Manager::GetVEntry ( const char * VfsName );
	// 파일 이름으로 VFS 엔트리를 조사한다
	VEntry * CVFS_Manager::GetVEntryWF ( const char * FileName );
	bool GetVfsInfo (const char * VfsName, VfsInfo * VI );
	
	// VFS 데이터를 읽어서 Crc를 계산한다
	DWORD ComputeCrc ( const char * FileName);
};


#endif
