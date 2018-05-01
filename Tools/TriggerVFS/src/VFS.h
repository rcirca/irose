#ifndef __VFS_H__
#define __VFS_H__


#include <windows.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>
#include "TriggerVFS.h"

/**
 * Virstual File System. 
 * 파일 확장자 : vfs
 * 파일 엔트리 : 파일 앞부분에. struct FileEntry참조
 */


#define		VFSMODE_NOTUSEIDX	0					/// IDX파일을 사용하지 않음
#define		VFSMODE_USEIDX		1					/// IDX파일을 사용함

#define		VERSION_STR			"00000"				/// 파일버젼 스트링
#define		VERSION_DEF_DWVALUE	0x00000000
#define		VERSION_DEF_WDVALUE	0x0000
#define		VERSION_ARRARY		{'0','0','0','0'}	/// 파일버젼 배열
#define		VERSION_16ARRAY		{'0','0','0','0'}	/// 파일버젼 16진수
#define		VERSION_C1			'0','0'				/// 파일버젼 첫번째 글자
#define		VERSION_C2			'0','0'				/// 파일버젼 두번째 글자
#define		VERSION_POS_STD		0					/// 기준 버젼 위치
#define		VERSION_POS_CUR		sizeof (DWORD)		/// 현재 버젼 위치

/**
 **
 ** vfs 파일 포맷
 ** 2 BYTE			: 파일 버젼	==> 이 버젼을 Patch용 버젼으로 사용함				(2003.11.19 추가)
 ** 2 BYTE			: 파일 버젼 2 ==> 이 버젼을 Patch용 버젼으로 사용함				(2003.12.9 추가)
 ** DWORD			: 파일엔트리 테이블의 갯수
 **
 ** Pascal String	: vfs 파일명 				(1)
 ** long	lIndexStart : 엔트리 테이블 시작 위치	(2)
 ** {(1)(2)}가 갯수만큼
 **
 ** **************************  파일 엔트리 ***********************
 ** DWORD			: 파일갯수. 실제 유효한 항목의 갯수는 삭제항목의 갯수를 빼야 한다 	
 ** DWORD			: 삭제항목의 갯수. 일정갯수가 넘었을때를 알기 위해서 필요하다.	(2003.11.19 추가)
 ** long			: 파일 시작 Offset
 ** File Entry		: [ 파일갯수 ]
 **
 **/


#define VFILE_MAX_PATH			164
/// 파일이 0개일때 시작 File의 시작 Offset
/// #define	SIZEOF_VFILEHEADER		(sizeof(BYTE) * 4 + sizeof(DWORD))		/// 버젼은 여기에
#define	SIZEOF_VFILEHEADER		(sizeof(DWORD) * 2 + sizeof(DWORD))		/// 버젼은 여기에

#define	SIZEOF_VENTRYHEADER		(sizeof(DWORD) * 2 + sizeof (long))	
#define SIZEOF_FILEENTRY_HEADER (sizeof(DWORD) * 2 + sizeof (long))	 /// 파일엔트리 테이블 앞쪽에 있는 헤더의 크기


#define	SIZEOF_FILEENTRY_EXCPTSTRING (sizeof (short) + sizeof (long) * 3 + sizeof (BYTE) * 3 + 2 * sizeof (DWORD))
#define	SIZEOF_FILEENTRY_V ( s )		(SIZEOF_FILEENTRY_EXCPTSTRING + (s).size () + 1)


/// vfs파일을 오픈했을때 이 스트럭쳐로 정보유지. 
struct VFile 
{
	std::string		sVFSName;		/// vfs 파일 이름
	FILE *			fp;				/// vfs파일에 대한 파일포인터
	FILE *			fp_FAT;			/// 여기에 파일 파일엔트리 정보가 저장됨. 파일을 쓸때 사용. fp와 똑같을 수도 있다.
	DWORD			dwNumOfFile;	/// 이 vfs파일에 저장된 파일의 갯수
	long			lStartOfData;	/// 파일이 저장되어 있는 시작오프셋
	BYTE			cMode;			/// IDX파일을 사용하는 방식인가 아닌가
	std::map<std::string, FileEntry *> mapFileEntry;	/// 파일 엔트리를 저장함

	VFile () { fp = fp_FAT = NULL; dwNumOfFile = 0; lStartOfData = 0; }
};



/****************************************************************************
 * 이 클래스는 파일엔트리를 순차적으로 접근하고 수정하기 위한 코드이다
 ****************************************************************************
 */
class CVFS
{
private:
	std::string					m_sFileName;	/// 파일이름. idx, vfs확장자를 제외한
	std::map<int, std::string>	m_is;			/// 키: 번호	 value : 파일이름. 번호순서대로 저장되어 있음
	std::map<std::string, int> 	m_si;			/// 키: 파일이름 value : 번호
	std::vector<FileEntry *>	m_ve;			/// vector에 집어 넣는다

	std::map<int, FileEntry *>	m_mapDel;		/// 삭제된 파일엔트리를 보관하고 있는 map
	FILE *						m_fp;			/// File Handle
	
	HANDLE m_hFile; // File Handle
	HANDLE m_hMap;	// Memory Mapped IO mapping handle

	FILE *						m_fpFAT;		/// File Handle of Index 파일

	DWORD						m_dwNum;		/// 파일엔트리의 갯수
	DWORD						m_dwDelCnt; 	/// 삭제된 항목의 갯수
	long						m_lStartOffset; /// 데이터의 시작. 사용하지는 않음
	BYTE						m_cMode;		/// VFSMODE_USEIDX:인덱스 사용, VFSMODE_NOTUSEIDX:인덱스 사용하지 않음
	size_t						m_BUFSIZ;		/// Buffer Size
	char *						m_Buffer;		/// File Buffer

	long						m_lEntryStart;	/// FileEntry의 Start Address. 한 index파일에 여러개의 vfs가 있을 경우 각각의 파일엔트리테이블은 시작 위치를 가진다

private:
	/// 파일헤더와 엔트리를 읽는다
	bool		__ReadFileEntry (void);

	/// map과 vector에서 엔트리를 지운다
	void		__EraseMemEntry (const char *FileName, int iIndex);
	/// map과 vector에 파일을 추가한다
	void		__AddMemEntry (FileEntry *FE);
	/// 지울 파일명을 순서대로 정렬해 준다
	bool		__SortFiles (const char **Files, int iNum);
	/// 지워진 파일엔트리중 이 크기가 들어 갈수 있는 블륵을 찾는다. bRestore == true 이면 복구한다
	FileEntry * __FindBlock (std::string sFileName, long lSize, int *iIndex, bool bRestore = true);
	/// index파일 앞쪽에 엔트리갯수와 시작오프셋을 쓴다
	void		__VWriteVFSHeader (DWORD dwNum, DWORD dwDelCnt, long lStartOffset);
	

	int index (const char * sKey);
	std::string index (int iKey);
	int operator [] (std::string sKey);
	std::string operator [] (int iKey);

public :
	CVFS ();
	CVFS (FILE * fpFAT, long lOffset, DWORD dwNum, const char *VfsName, char * Mode);
	~CVFS ();

	long SizeOfEntryTable (void);		/// 파일엔트리테이블이 차지하는 크기를 구한다
	
	/// CVFS_Manager에서 호출
	bool Open (FILE * fpFAT, long lOffset, const char *VfsName, const char *Dir, const char * Mode);

	/// vfs파일을 닫는다
	void Close (void);

	
	short RemoveFilesB (const char **Files, int iNum);
	/// 파일을 추가한다. CRC값은 자체적으로 계산한다

	short AddFile (const char * FileName, const char * TargetName, DWORD dwVersion, DWORD dwCRC
		, BYTE btEncType, BYTE btCompress, bool bUseDel = false);
	/// 파일을 오픈한다
	VFileHandle * OpenFile (const char * FileName);
	/// 오픈한 파일을 닫는다
	void CloseFile (VFileHandle * FH);

	/// 파일 크기를 알아낸다
	long GetFileLength (const char * FileName);
	
	/// 총 파일 갯수  가져오기
	DWORD GetFileCount (void);
	/// 파일 이름 가져오. 메모리 할당은 호출하는 쪽에서
	DWORD GetFileNames (char **pFiles, DWORD nFiles, int nMaxPath);
	/// 공백을 지운다
	bool ClearBlank (void);
	/// 삭제되었지만 블록은 남아있는 엔트리의 갯수를 리턴
	DWORD GetDelCnt (void);
	/// 재사용된 블럭의 갯수
	DWORD GetReUsedCnt (void);
	/// 
	int GetSizeofBlankArea ();
	/// 파일은 있지만 블록크기와 파일크기가 일치하지 않는 엔트리의 갯수를 조사한다. (땜빵당한 블록)
	DWORD GetFileCntWithHole (void);
	
	/// 파일엔트리 테이블의 시작오프셋을 업데이트한다
	void SetStartOfEntry (long lStartOfEntry);
	
	/// 파일의 갯수 : 총 갯수 - 지워진 갯수
	DWORD GetEntryCount (void);
	/// 파일이 존재하는지 검사한다
	bool FileExists (const char *FileName);

	/// 이 인덱스파일에 있는 묶음 파일의 갯수를 조사한다
	DWORD GetVFSCount (void);
	/// Index File안에 있는 묶음 파일의 이름을 조사함
	DWORD GetVfsNames (char **ppFiles, DWORD dwNum, short dwMaxPath);

	/// 파일에 대한 정보를 알아낸다
	void GetFileInfo (const char * FileName, VFileInfo * pFileInfo);
	///
	bool SetFileInfo (const char * FileName, VFileInfo * pFileInfo);

	FILE * GetFP ( void );

	long GetStartOff (void );
};


/// 파일 엔트리를 쓴다
bool VWriteFileEntry (FileEntry *fe, FILE * fp, bool bFlush = true);


#endif
