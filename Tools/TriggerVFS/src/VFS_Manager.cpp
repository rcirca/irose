#include "stdafx.h"
#include <stdlib.h>
#include "VFS.h"
#include "VFS_Manager.h"
#include "libFile.h"
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "libString.h"
#include "libCrc.h"
#include "BlankInfo.h"
#include "Macro.h"

/************************************************************************************
 *
 *
 * CVFS_Manager
 *
 *
 */

///bool CVFS_Manager::s_bLock = false;


CVFS_Manager::CVFS_Manager ()
{
	m_fpIDX			= NULL;
	m_sIdxFileName	= "";
	// strcpy ((char *)m_wStdVersion, VERSION_STR);	
	m_wStdVersion[ 0 ] = VERSION_DEF_WDVALUE; /// 버젼을 초기화
	m_wStdVersion[ 1 ] = VERSION_DEF_WDVALUE;
	m_wCurVersion[ 0 ] = VERSION_DEF_WDVALUE;
	m_wCurVersion[ 1 ] = VERSION_DEF_WDVALUE;

	m_vecVFS.clear ();

	///InitializeCriticalSection (&m_CS);
}

CVFS_Manager::~CVFS_Manager ()
{
	if( m_fpIDX != NULL )
		this->Close ();

	m_fpIDX			= NULL;
	m_sIdxFileName	= "";
}


/******************************************************************************************
 * 빈 index 파일를 위한 파일 헤더를 만든다
 */
bool CVFS_Manager::__WriteBlankIndexFile (void)
{
	if( m_fpIDX != NULL )
	{
		// Initialize member variable
		
		this->m_dwNumOfEntry = 0;
		m_wStdVersion[ 0 ] = VERSION_DEF_WDVALUE;
		m_wStdVersion[ 1 ] = VERSION_DEF_WDVALUE;
		m_wCurVersion[ 0 ] = VERSION_DEF_WDVALUE;
		m_wCurVersion[ 1 ] = VERSION_DEF_WDVALUE;

		fseek (m_fpIDX, 0, SEEK_SET);
		fwrite (m_wStdVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// 기준 파일 버젼을 쓴다
		fwrite (m_wCurVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// 현재 파일 버젼을 쓴다
		fwrite (&m_dwNumOfEntry	, sizeof (DWORD), 1, m_fpIDX);	/// VEntry의 갯수 = 0 을 파일에 Writing

		return ( fflush (m_fpIDX) == 0 );
	}

	return false;
}


/******************************************************************************************
 * Index File Header를 읽는다
 */
bool CVFS_Manager::__ReadVEntry (void)
{
	char * buff = NULL;
	VEntry *pVE = NULL;
	short sLength = 0;

	fseek (m_fpIDX, 0, SEEK_SET);									/// 제일 앞쪽으로 이동
	fread ((void *)m_wStdVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// 기준 버젼을 읽는다
	fread ((void *)m_wCurVersion	, sizeof (WORD)	, 2, m_fpIDX);	/// 현재 버젼을 읽는다
	fread ((void *)&m_dwNumOfEntry	, sizeof (DWORD), 1, m_fpIDX);	/// VEntry의 갯수 읽는다

	for(DWORD i = 0; i < m_dwNumOfEntry; i++)
	{
		if((pVE = new VEntry))
		{
			fread ((void *)&sLength, sizeof (short), 1, m_fpIDX);
			if((buff = new char[ sLength ]))
			{
				/// 파일명을 읽는다
				ZeroMemory (buff, sLength);
				fread ((void *)buff, sizeof (char), sLength, m_fpIDX);	/// sLength = 뒤에 NULL까지 포함한 갯수
				pVE->sVFSName = buff;
			
				delete [] buff; /// ==>  원래는 해제해 주어야 하지만 해제 하지 않는다. string은 heap일 경우 그냥 사용하는 것 같다.

				/// 인덱스의 시작 오프셋을 읽는다
				// fread ((void *)&pVE->dwNum,			sizeof (DWORD)	, 1, m_fpIDX);
				fread ((void *)&pVE->lStartOfEntry, sizeof (long)	, 1, m_fpIDX);

				/// CVFS인스턴스를 만들고 해당 엔트리와 vfs파일을 오픈한다
				long lCurPos = ftell (m_fpIDX);
				pVE->pVFS = new CVFS ();
				if(pVE->pVFS)
				{
					bool bPackOpened = pVE->pVFS->Open (
							m_fpIDX
							, pVE->lStartOfEntry
							, pVE->sVFSName.c_str ()
							, m_sBasePath.c_str()
							, m_strIdxOpenMode.c_str ()
							);

					if(bPackOpened) /// 성공하면 Vector에 집어넛고
						m_vecVFS.push_back (pVE); 
					else if( pVE->sVFSName != "ROOT.VFS" )
						return false;
				}
				
				fseek (m_fpIDX, lCurPos, SEEK_SET);
			}
			else
			{
				delete pVE;
				return false;
			}
		}
		else
		{ return false; }
	}

	return true;
}


bool CVFS_Manager::__TestMapIO (const char * szFileName)
{
	return false;

	HANDLE fpTest	= NULL;
	HANDLE fm		= NULL;
	char * src		= NULL;

	OSVERSIONINFO OsVer;
	GetVersionEx (&OsVer);

	if(OsVer.dwMajorVersion >= 5)
	{
		fpTest = CreateFile (szFileName
			, GENERIC_READ
			, FILE_SHARE_READ
			, NULL
			, OPEN_EXISTING
			, FILE_ATTRIBUTE_READONLY
			, NULL);


		if (fpTest == INVALID_HANDLE_VALUE) 
			return false;

		fm = CreateFileMapping ( fpTest, NULL, PAGE_READONLY, 0, 0, NULL );

		if (!fm)
		{
			CloseHandle( fpTest );
			return false;
		}

		src = reinterpret_cast<char *>(MapViewOfFile( fm, FILE_MAP_READ, 0, 0, 0 ));	
		if (src == NULL) 
		{
			CloseHandle (fm);
			CloseHandle (fpTest);
			return false;
		}

		UnmapViewOfFile( src );
		CloseHandle( fm );
		CloseHandle( fpTest );

		return true;
	}

	return false;
}


/******************************************************************************************
 *
 */
void CVFS_Manager::__CheckOpenMode ( const char * InputMode, char ModifiedMode[ 16 ] )
{
#ifdef __SUPPORT_MEMORY_MAPPED_IO__
	if( (Mode[ 0 ] == 'm') && __TestMapIO ("Logo.dds") == false )
		strcpy ( ModifiedMode, InputMode + 1);
	else
		strcpy ( ModifiedMode, InputMode );
#else
	if( InputMode[ 0 ] == 'm' )
	{
		strcpy ( ModifiedMode, InputMode + 1 );
	}
	else
		strcpy ( ModifiedMode, InputMode );
#endif
}


/******************************************************************************************
 * Index 파일을 오픈한다
 * @param IndexFile 열 인덱스 파일명
 * @param Mode 속성. "r" 읽기 전용, "w" 쓰기전용(사용못함), "w+" 생성 + 쓰기 + 읽기, 
 *                   "r+" 읽기 + 쓰기(생성 못함)
 */
bool CVFS_Manager::Open (const char *IndexFile, const char * __Mode)
{
	m_sIdxFileName = IndexFile;
	m_sBasePath = GetDirectory (m_sIdxFileName.c_str ()); /// "\"문자까지 포함
    /// Binary Mode로 만든다
	_fmode = _O_BINARY;
	/// 쓰기 모드로 오픈했을 경우

	char Mode[ 16 ];
	__CheckOpenMode ( __Mode, Mode );

	m_strIdxOpenMode = Mode;

	if(strcmp (Mode, "w+") == 0)
	{
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READWRITE, true) ;
		if( (m_fpIDX = fopen (IndexFile, Mode)) )	/// "w+"로 열어야 엔트리는 항상 수정됨
		{
			m_vecVFS.clear ();

			return __WriteBlankIndexFile (); // 그냥 빈 Ventry를 하나 만든다
		}
	}
	else if(strcmp (Mode, "mr") == 0 || strcmp (Mode, "mr+") == 0 )
	{
		/// 읽기 퍼미션을 검사하고 없으면 읽기로 바꾼다
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READ, true) ;

		if( CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS)  
			&& (m_fpIDX = fopen (IndexFile, Mode + 1)))
		{
			return __ReadVEntry ();
		}
	}
	else if(strcmp (Mode, "r+") == 0)
	{
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READWRITE, true) ;

		if( CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS) 
			&& (m_fpIDX = fopen (IndexFile, Mode)) )
		{
			return __ReadVEntry ();
		}
	}
	else if(strcmp (Mode, "r") == 0)
	{
		if( !CFileMode::CheckMode (IndexFile, CFileMode::MODE_EXISTS) )
			return false;

		/// 읽기 퍼미션을 검사하고 없으면 읽기로 바꾼다
		CFileMode::CheckMode (IndexFile, CFileMode::MODE_READ, true) ;

		if( (m_fpIDX = fopen (IndexFile, Mode)) )
		{
			return __ReadVEntry ();
		}
	}
	
		
	return false;
}


/******************************************************************************************
 * Index파일과 VFS파일을 닫는다
 */
void CVFS_Manager::Close (void)
{
	VEntry * m_pVEntry = NULL;
	std::vector<VEntry *>::reverse_iterator ir = m_vecVFS.rbegin ();

	for(; ir != m_vecVFS.rend (); ir++)
	{
		m_pVEntry = *ir;
		if(m_pVEntry)
		{
			m_pVEntry->pVFS->Close ();
			delete m_pVEntry->pVFS;
			delete m_pVEntry;
		}
	}
	m_vecVFS.clear ();

	if(m_fpIDX)
	{
		fclose (m_fpIDX);
		m_fpIDX = NULL;
	}
}


/******************************************************************************************
 * VEntry를 쓴다
 */
void CVFS_Manager::__WriteVEntry (VEntry * pVE)
{
	short sLength = (short)pVE->sVFSName.size () + 1;
	fwrite ((void *)&sLength				, sizeof (short), 1, m_fpIDX);
	fwrite ((void *)pVE->sVFSName.c_str ()	, sizeof (char)	, pVE->sVFSName.size (), m_fpIDX);
	fwrite ((void *)"\0"					, sizeof (char) , 1, m_fpIDX);
	fwrite (&pVE->lStartOfEntry				, sizeof (long) , 1, m_fpIDX);
}


/******************************************************************************************
 * 파일에 쓰기 위한 VEntry의 크기
 */
long CVFS_Manager::__SizeOfVEntry (VEntry *VE)
{
	return (long)(VE->sVFSName.size () + 1 + SIZE_VENTRY_EXCEPTSTRING);
}


/******************************************************************************************
 * VEntry를 스킾한다
 */
void CVFS_Manager::__SkipVEntry (VEntry *VE)
{
	/// 스킾할 사이즈를 계산한다
	long lSkipSize = (long)VE->sVFSName.size () + 1 + SIZE_VENTRY_EXCEPTSTRING;
	fseek (m_fpIDX, lSkipSize, SEEK_CUR);
}


/******************************************************************************************
 * VFS파일을 추가한다
 * @param VfsName 대문자로 변환해서 사용 *** 나중에 고칠것
 */
bool CVFS_Manager::AddVFS (const char *VfsName)
{
	long		lSize = 0;		/// 파일 사이즈
	long		lVET_Size = 0;	/// 현재 VEntry Table의 크기
	long		lNewSize = 0;	/// 새로운 파일 사이즈
	VEntry *	pVE = NULL;		/// 추가할 VEntry
	std::vector<VEntry *>::iterator iv;
	char		uprVfsName[ 1024 ];

	if(VfsName == NULL) 
		return false;
	if(VfsExists (VfsName)) 
		return false;	/// 이미 같은 Vfs 이름이 존재하는 경우 false 리턴

	__ConvertPath (VfsName, uprVfsName);

	if((pVE = new VEntry))
	{
		lSize = __vfseek (m_fpIDX, 0, SEEK_END);
		/// VEntry를 만든다
		pVE->sVFSName		= uprVfsName;							/// vfs파일명. 대문자로 변환해서 집어 넣는다
		pVE->lStartOfEntry	= lSize + __SizeOfVEntry (pVE);			/// 새로 추가되는 엔트리테이블 시작오프셋

		/// 인덱스파일의 전체 크기
		lSize = ::__vfseek (m_fpIDX, 0, SEEK_END);
		/// VEntry도 수정해야 하기 때문에 그 앞쪽으로 이동한다
		fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
		lVET_Size = SIZEOF_VFILEHEADER;
		/// VEntry를 추가하기 위한 위치까지 거리를 계산
		iv = m_vecVFS.begin ();
		/// 앞쪽에 VEntry를 수정하고 각 CVFS인스턴스도 수정한다
		for(; iv != m_vecVFS.end (); iv++) 
		{ 
			(*iv)->lStartOfEntry += __SizeOfVEntry (pVE);
			lVET_Size += __SizeOfVEntry (*iv); 
			__WriteVEntry (*iv);
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);		/// CVFS안에도 수정해 주어야 한다
		}
		long lInsertedPos = ftell (m_fpIDX);							/// VEntry를 추가할 위치
		::__MakeFileHole (lInsertedPos, __SizeOfVEntry (pVE), m_fpIDX, true);	/// 추가하기 위한 공간을 만든다
		fseek (m_fpIDX, lInsertedPos, SEEK_SET);						/// 파일헤더 VEntry 쓸 위치로 이동
		__WriteVEntry (pVE);											/// 파일에 Write
		fflush (m_fpIDX);
		/// VFS인스턴스를 만든다
		if((pVE->pVFS = new CVFS))
		{ 
			if((pVE->pVFS->Open (m_fpIDX, pVE->lStartOfEntry, pVE->sVFSName.c_str (), m_sBasePath.c_str (), "w+")))
			{ 
				m_vecVFS.push_back (pVE); 
				m_dwNumOfEntry++;
				__WriteIndexHeader (VERSION_STR, m_dwNumOfEntry);
			}
			else
			{
				delete pVE->pVFS;
				delete pVE;
				return false;
			}
		}
		else
		{ 
			delete pVE;
			return false;
		}
	}
	else
	{ return false; }

	return true;
}


/******************************************************************************************
 * RemoveVFS : VFS를 제거한다
 * @param VfsName 제거할 vfs파일 이름
 */
bool CVFS_Manager::RemoveVFS (const char *VfsName)
{
	DWORD		iDelIndex = -1;
	VEntry *	pVE = NULL;
	long		lDelSize = 0;	/// 없어지는 영역의 크기
	std::vector<VEntry *>::iterator iv;

	fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
	if((iDelIndex = __FindEntryIndex (VfsName)) >= 0)
	{
		pVE = *(m_vecVFS.begin () + iDelIndex);
		lDelSize = __SizeOfVEntry (pVE);										/// 앞쪽에는 이만큼만 빼 주면 된다
		for(; iv != m_vecVFS.begin () + iDelIndex; iv++)
		{
			(*iv)->lStartOfEntry -= lDelSize;
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}
		/// 지워질 영역은 덮어쓰기한다
		lDelSize += pVE->pVFS->SizeOfEntryTable ();								/// 뒷쪽은 저만큼씩 땡겨야한다
		pVE->pVFS->Close ();													/// CVFS를 닫는다
		delete pVE->pVFS;														/// CVFS 인스턴스를 해제한다
		delete pVE;																/// 파일엔트리 메모리에서 삭제
		/// 지워지니까 한번 더 전진
		iv++;
		/// 뒤에 있는 VEntry도 갱신한다
		for(; iv != m_vecVFS.end (); iv++)
		{
			(*iv)->lStartOfEntry -= __SizeOfVEntry (pVE);
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}
		m_vecVFS.erase (m_vecVFS.begin () + iDelIndex);							/// 벡터에서 지운다
		m_dwNumOfEntry--;														/// 갯수를 하나 줄인다
		__WriteIndexHeader (VERSION_STR, m_dwNumOfEntry);						/// Index파일의 헤더를 다시 쓴다
		::__ftruncate (::__vfseek (m_fpIDX, 0, SEEK_END) - lDelSize, m_fpIDX);	/// Index파일의 크기를 조정한다
	}

	return false;
}


/******************************************************************************************
 * 파일 헤더를 쓴다
 */
void CVFS_Manager::__WriteIndexHeader (char * Version, DWORD dwNum)
{
	DWORD dwStdVersion;
	dwStdVersion = atoi (Version);
	fseek (m_fpIDX, 0, SEEK_SET);
	fwrite ((void *)&dwStdVersion, sizeof (WORD), 2, m_fpIDX);		/// 기준 버젼
	fwrite ((void *)m_wCurVersion, sizeof (WORD), 2, m_fpIDX);	/// 현재 버젼
	fwrite ((void *)&dwNum,  sizeof (DWORD), 1, m_fpIDX);		/// Ventry의 갯수
	fflush (m_fpIDX);
}


/******************************************************************************************
 * VFS파일에 대한 엔트리가 존재하는 검색한다
 * @param FileName 찾을 VFS파일명
 */
VEntry * CVFS_Manager::__FindEntry (const char *FileName)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		/// 파일 이름이 같으면 VEntry * 를 리턴
		if((*iv)->sVFSName == FileName) 
			return *iv;
	}

	return NULL;
}


/******************************************************************************************
 * VFS파일에 대한 엔트리가 존재하는 검색하고 인덱스를 리턴. 헤더에 있는 엔트리정보를 정보를 유용
 * @param FileName 찾을 VFS파일명
 */
DWORD CVFS_Manager::__FindEntryIndexWithFile (const char *FileName)
{
	DWORD dwRet = 0;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		/// 파일 이름이 같으면 VEntry * 를 리턴
		if((*iv)->pVFS->FileExists (FileName))
			return dwRet;

		dwRet++;
	}

	return -1;
}


/******************************************************************************************
 * VFS파일에 대한 엔트리가 존재하는 검색하고 인덱스를 리턴. 헤더에 있는 엔트리정보를 정보를 유용
 * @param FileName 찾을 VFS파일명
 */
long CVFS_Manager::__FindEntryIndex (const char *FileName)
{
	DWORD dwRet = 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		/// 파일 이름이 같으면 VEntry * 를 리턴
		if((*iv)->sVFSName == FileName) { return dwRet; }
		dwRet++;
	}

	return -1;
}


/******************************************************************************************
 * vfs에 파일을 추가한다
 * @param dwNum			추가될 파일의 갯수
 * @param TargetName	이 이름으로 등록된다. 
 */
short CVFS_Manager::AddFile (const char *VfsName
							, const char *FileName
							, const char * TargetName
							, DWORD dwVersion
							, DWORD dwCrc
							, BYTE btEncType
							, BYTE btCompres
							, bool bUseDel)
{
	long		lOldSize = 0, lNewSize = 0;
	VEntry *	pVE = NULL;
	std::vector<VEntry *>::iterator iv;
	char uprVfsName[ 1024 ];
	char uprTargetName[ 1024 ];
	
	/// 경로를 대문자 , 앞뒤 공백제거
	__ConvertPath (VfsName, uprVfsName);
	__ConvertPath (TargetName, uprTargetName);

	
	/// VfsName에 해당하는 엔트리가 존재하면 추가한다
	if((pVE = __FindEntry (uprVfsName))) 
	{ 
		lOldSize = pVE->pVFS->SizeOfEntryTable ();						/// 이전 파일엔트리 Table의 크기
		short nAddResult = pVE->pVFS->AddFile (FileName
								, uprTargetName
								, dwVersion
								, dwCrc
								, btEncType
								, btCompres
								, bUseDel);		/// 파일을 추가한다

		if(nAddResult != VADDFILE_SUCCESS)						
			return nAddResult;

		lNewSize = pVE->pVFS->SizeOfEntryTable ();						/// 추가하고 나서 파일엔트리 테이블의 크기
		/// VEntry 테이블 갱신은 여기서 해야 한다
		int iIndex = __FindEntryIndex (uprVfsName);
		/// VEntry Table을 수정하기 File Indicator를 Table앞쪽으로 이동시킨다. 
		fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
		/// 앞쪽에는 변경할 꺼 없음. 
		for(iv = m_vecVFS.begin (); iv <= m_vecVFS.begin () + iIndex; iv++)
		{
			__SkipVEntry (*iv);
		}
		/// 뒤쪽은 VEntry에 해당하는 엔트리테이블의 오프셋은 밀려난다
		for(; iv != m_vecVFS.end (); iv++)
		{
			(*iv)->lStartOfEntry += lNewSize - lOldSize;
			(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
			__WriteVEntry (*iv);
		}

		__FlushIdxFile ();

		return VADDFILE_SUCCESS;
	}

	return VADDFILE_INVALIDVFS;
}


/******************************************************************************************
 * pack안에 있는 파일이름으로 VEntry를 찾는다
 */
VEntry * CVFS_Manager::__FindVEntryWithFile (const char *FileName)
{
	VEntry * pVE = NULL;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		/// 파일 이름이 같으면 VEntry * 를 리턴
		if((*iv)->pVFS->FileExists (FileName))
		{  
			return *iv;
		}
	}

	return NULL;
}


/******************************************************************************************
 * Pack파일에 찾아서, 파일을 한개 제거한다.
 * @param File	pack 파일안의 파일 이름
 * @return VRMVFILE_XXXXX , 성공하면 VRMVFILE_SUCCESS를 리턴
 */
short CVFS_Manager::RemoveFile (const char *FileName)
{
	const char *szNEW = NULL;
	char uprTargetName[1024];

	VEntry *	pVE = NULL;
	long		lOldSize = 0, lNewSize = 0;
	DWORD		iIndex = -1;
	short		i = 0;
	
	/// 타겟이름을 올바른 경로로 바꾼다
	__ConvertPath (FileName, uprTargetName);

	szNEW = uprTargetName;

	/// 엔트리를 찾아서 있으면 지운다
	pVE = __FindVEntryWithFile (uprTargetName);
	if(pVE)
	{
		iIndex = __FindEntryIndexWithFile (uprTargetName);
		if(iIndex >= 0)
		{
			CVFS * pVFS = pVE->pVFS;
			lOldSize = pVFS->SizeOfEntryTable ();

			short bRet;
			if( (bRet = pVFS->RemoveFilesB (&szNEW, 1)) == VRMVFILE_SUCCESS )
			{
				lNewSize = pVFS->SizeOfEntryTable ();
				fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);

				for(i = 0; i <= (signed)iIndex; i++)
					__SkipVEntry (*(m_vecVFS.begin () + i)); 

				for(; i < (signed)m_dwNumOfEntry; i++)
				{
					std::vector<VEntry *>::iterator iv = m_vecVFS.begin () + i;
					if(iv != m_vecVFS.end ())	/// vfs파일이 없는 경우. vfs파일의 갯수와 파일에 기록된 갯수가 다를 경우
					{
						VEntry * pVEtoModify = *iv;
						/// VEntry의 엔트리테이블의 Start Offset을 수정하고 다시쓴다
						pVEtoModify->lStartOfEntry -= (lOldSize - lNewSize);
						__WriteVEntry (pVEtoModify);
					}
				}

				fflush ( m_fpIDX );

				return VRMVFILE_SUCCESS;
			}
			
			return bRet; // 있는데 삭제 못 하면 false 리턴
		}
	}

	return VRMVFILE_INVALIDVFS; // 없는 파일을 삭제 시도했을 경우 true를 리턴
}


/******************************************************************************************
 * 파일 여러개 제거하고. 그냥 빈공간으로 남겨두기
 */
bool CVFS_Manager::RemoveFiles (const char *VfsName, const char **Files, int iNum)
{
	VEntry *	pVE = NULL;
	long		lOldSize = 0, lNewSize = 0;
	DWORD		iIndex = -1;
	short		i = 0;

	/// 엔트리를 찾아서 있으면 지운다
	if((pVE = __FindEntry (VfsName)) && (iIndex = __FindEntryIndex (VfsName)) >= 0)
	{
		lOldSize = pVE->pVFS->SizeOfEntryTable ();
		if(pVE->pVFS->RemoveFilesB (Files, iNum))
		{
			lNewSize = pVE->pVFS->SizeOfEntryTable ();
			fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
			for(i = 0; i <= (signed)iIndex; i++)
			{ __SkipVEntry (*(m_vecVFS.begin () + i)); }

			for(; i < (signed)m_dwNumOfEntry; i++)
			{
				/// VEntry의 엔트리테이블의 Start Offset을 수정하고 다시쓴다
				(*(m_vecVFS.begin () + i))->lStartOfEntry -= (lOldSize - lNewSize);
				__WriteVEntry (*(m_vecVFS.begin () + i));
			}

			return true;
		}
	}
	
	return false;
}


/// 인덱스 파일을 fflush한다.
void CVFS_Manager::__FlushIdxFile (void)
{
	if(m_fpIDX)
		fflush (m_fpIDX);
}


/******************************************************************************************
 * vfs파일에서 파일을 오픈한다
 * 다른 디렉토리에서 인덱스를 열어 버리면 문제 발생할 수 있다. ==> 수정
 */
VFileHandle * CVFS_Manager::OpenFile (const char *FileName)
{
	VFileHandle *pVF = NULL;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	if(FileName == NULL) { return NULL; }

	
	if( FileExistsInVfs( FileName ) ) // vfs 안에 있으면 그걸 먼저 open 한다. 그리고 외부 파일로 존재 여부 검색
	{
		char rightName[1024];
		::__ConvertPath (FileName, rightName);
		for(; iv != m_vecVFS.end (); iv++)
		{
			if((pVF = (*iv)->pVFS->OpenFile (rightName)))
			{
				pVF->hVFS = (VHANDLE)this;
				return pVF;
			}
		}
	}
	else if(_access (FileName, 0) == 0 && _access (FileName, 4) == 0)
	{
		if((pVF = new VFileHandle))
		{
			_fmode = _O_BINARY;
			if((pVF->fp = fopen (FileName, "r")))
			{
				pVF->lCurrentOffset	= 0;
				pVF->lStartOff		= 0;
				pVF->lEndOff		= __vfseek (pVF->fp, 0, SEEK_END);
				pVF->sFileName		= FileName;
				pVF->btFileType		= 1;									/// 밖에 있는 파일일 경우 1
				pVF->hVFS			= NULL;
				pVF->pData			= NULL;

				fseek (pVF->fp, 0, SEEK_SET);

				return pVF;
			}
		}
	}

	return NULL; /// 발견하지 못하면 pVF에NULL이 리턴
}


/******************************************************************************************
 * OpenFile로 오픈한 파일을 닫는다
 * 현재는 단순히 메모리를 해제하는 기능만...
 */
void CVFS_Manager::CloseFile (VFileHandle *pVFH)
{
	/// VCloseFile을 호출할 경우 VCloseFile안에 일반 파일인지 확인하고 닫는다
	if(pVFH->btFileType) { fclose (pVFH->fp); }
	/// 지금은 단순히 메모리를 해제하는 기능만 넣는다
	delete pVFH;
}


/******************************************************************************************
 * VFS 파일에서 파일이름을 검색한다
 */
DWORD CVFS_Manager::GetFileNames (const char *VfsName, char **pFiles, DWORD nFiles, int nMaxPath)
{
	VEntry * pVE = NULL;
	if(!(pVE = __FindEntry (VfsName))) { return 0; }

	return pVE->pVFS->GetFileNames (pFiles, nFiles, nMaxPath);
}


/******************************************************************************************
 * 파일 크기를 알아낸다
 * @return 파일을 발견하지 못하면 0을 리턴한다. (실제 파일의 크기가 0인 경우도)
 */
long CVFS_Manager::GetFileLength (const char *FileName)
{
	VFileHandle * pVF	= NULL;
	long lFileSize		= 0;

	/// 밖에 존재하는 파일일 경우에도 파일 크기를 리턴한다.
	struct _stat file_stat;	
	if (_stat(FileName, &file_stat) == 0) 
		lFileSize = (long)file_stat.st_size;
	else
	{
		std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
		/// 인덱스 파일안에 많지 않은 vfs파일이 있으므로 그냥 for loop로 해도 무방하리라 생각됨
		for(; iv != m_vecVFS.end (); iv++)
		{
			lFileSize = (*iv)->pVFS->GetFileLength (FileName);	
			if(lFileSize >= 0) /// 발견하면 크기를 리턴. 맵을 이용해서 찾아서 크기 알아냄
				break;
		}
	}

	return lFileSize; /// 발견하지 못하면 -1을 리턴
}


/******************************************************************************************
 * 파일갯수를 알아낸다
 */
DWORD CVFS_Manager::GetFileCount (const char *VfsName)
{
	VEntry * pVE = NULL;
	if(pVE = __FindEntry (VfsName))			/// 파일을 검색해서 있으면
	{
		return pVE->pVFS->GetFileCount ();	/// 갯수를 리턴
	}

	return 0;
}


/******************************************************************************************
 * 인덱스파일안에 있는 파일의 총갯수를 알아낸다.
 */
DWORD CVFS_Manager::GetFileTotCount (void)
{
	if(m_vecVFS.size () <= 0) 
		return 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	int iCnt = 0;
	for(; iv != m_vecVFS.end (); iv++)
	{
		if(*iv) /// iterator에 적합한 값이 들어가 있다고 생각지 말것.
		{
			iCnt += (*iv)->pVFS->GetFileCount ();
		}
	}

	return iCnt;
}


/******************************************************************************************
 * 이 인덱스파일에 있는 묶음 파일의 갯수를 조사한다
 */
DWORD CVFS_Manager::GetVFSCount (void)
{
	return m_dwNumOfEntry;
}


/******************************************************************************************
 * GetVfsNames : Index File안에 있는 묶음 파일의 이름을 조사함
 * @param ppFiles 파일이름을 저장할 버퍼
 * @param dwNum ppFiles에 저장할 스트링갯수
 * @param dwMaxPath 스트링의 최고 길이
 */
DWORD CVFS_Manager::GetVfsNames (char **ppFiles, DWORD dwNum, short dwMaxPath)
{
	int i = 0;
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(i = 0; i < dwMaxPath && iv != m_vecVFS.end (); i++)
	{
		strncpy (ppFiles[ i ], (*iv)->sVFSName.c_str (), dwMaxPath - 1);
		ppFiles[ i ][ dwMaxPath - 1 ] = 0;
		iv++;
	}

	return i;
}


/******************************************************************************************
 * Pack 파일에서 지워졌으나 아직 정리되지 않은 파일의 갯수를 조사한다
 */
DWORD CVFS_Manager::GetDelCnt (const char *VfsName)
{
	VEntry * pVE = NULL;

	if(pVE = __FindEntry (VfsName))			/// 파일을 검색해서 있으면
	{
		return pVE->pVFS->GetDelCnt ();		/// 갯수를 리턴
	}

	return 0;
}


/******************************************************************************************
 * 
 */
DWORD CVFS_Manager::GetFileCntWithHole (const char *VfsName)
{
	VEntry * pVE = NULL;

	if(pVE = __FindEntry (VfsName))					/// 파일을 검색해서 있으면
	{
		return pVE->pVFS->GetFileCntWithHole ();	/// 갯수를 리턴
	}

	return 0;
}


/// 공백을 지운다
bool CVFS_Manager::ClearBlank (const char * VfsName )
{
	long		lOldSize = 0 , lNewSize = 0;
	VEntry *	pVE = NULL;
	int			iIndex = 0;
	std::vector<VEntry *>::iterator iv;
	long		lFileSize = 0;

	if(pVE = __FindEntry (VfsName))						/// 파일을 검색해서 있으면
	{
		lFileSize = __vfseek (m_fpIDX, 0, SEEK_END);	/// 파일 크기
		lOldSize = pVE->pVFS->SizeOfEntryTable ();		/// 테이블 크기 변화로 크기변화를 알아낸다
		if(pVE->pVFS->ClearBlank ())
		{
			lNewSize = pVE->pVFS->SizeOfEntryTable ();
			/// 뒷부분을 당긴다
			::__MoveFileBlock (pVE->lStartOfEntry + lOldSize
				, lFileSize - (pVE->lStartOfEntry + lOldSize)
				, pVE->lStartOfEntry + lNewSize, 1000000, m_fpIDX, false);

			fflush (m_fpIDX);
			/// 파일크기를 조정한다
			::__ftruncate (lFileSize - (lOldSize - lNewSize), m_fpIDX);

			iIndex = __FindEntryIndex (VfsName);
			iv = m_vecVFS.begin ();
			/// VEntry Table을 수정하기 위해 앞쪽으로 이동한다
			fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
			/// 이전까지는 그냥 스킾
			for(; iv <= m_vecVFS.begin () + iIndex; iv++)
			{
				__SkipVEntry (*iv);
			}
			/// 여기는 건너뛰고
			/// iv++;   /// <== 삭제가 아니기 때문에 건너 뛰면 안 됨
			/// 뒷쪽부터 다시 삭제된 만큼 뺀다
			for(; iv != m_vecVFS.end (); iv++)
			{
				(*iv)->lStartOfEntry -= lOldSize - lNewSize;
				(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
				__WriteVEntry (*iv);
			}

			return true;
		}
	}

	return false;
}


/// 모든 Pack파일의 공백을 지운다
bool CVFS_Manager::ClearBlankAll (VCALLBACK_CLEARBLANKALL CallBackProc)
{

	long			lOldSize = 0 , lNewSize = 0;
	VEntry *		pVE = NULL;
	int				iIndex = 0;
	std::vector<VEntry *>::iterator iv;
	std::vector<VEntry *>::iterator il;
	long			lFileSize = 0;
	const char *	VfsName;

	std::vector<int> vecStepPos;

	CBlankInfo::procCallBack = CallBackProc;

	il = m_vecVFS.begin ();
	for(; il != m_vecVFS.end (); il++)
	{
		if( *il )
		{
			VEntry * pVE = *il;
			CBlankInfo::iMaxCount += ( pVE->pVFS->GetEntryCount () + pVE->pVFS->GetDelCnt () 
									+ pVE->pVFS->GetReUsedCnt () + 1 ) ;
			CBlankInfo::iMaxCount += 2;

			vecStepPos.push_back ( CBlankInfo::iMaxCount );
		}
	}

	std::vector<int>::iterator itStepPos = vecStepPos.begin ();

	for( il = m_vecVFS.begin (); il != m_vecVFS.end (); il++ )
	{
		VfsName = (*il)->sVFSName.c_str ();

		if(pVE = __FindEntry (VfsName))						/// 파일을 검색해서 있으면
		{
			lFileSize = __vfseek (m_fpIDX, 0, SEEK_END);	/// 파일 크기
			lOldSize = pVE->pVFS->SizeOfEntryTable ();		/// 테이블 크기 변화로 크기변화를 알아낸다
			if(pVE->pVFS->ClearBlank ())
			{
				lNewSize = pVE->pVFS->SizeOfEntryTable ();
				/// 뒷부분을 당긴다
				::__MoveFileBlock (pVE->lStartOfEntry + lOldSize
					, lFileSize - (pVE->lStartOfEntry + lOldSize)
					, pVE->lStartOfEntry + lNewSize, 1000000, m_fpIDX, false);
				CBlankInfo::DoStep ();
				fflush (m_fpIDX);
				/// 파일크기를 조정한다
				::__ftruncate (lFileSize - (lOldSize - lNewSize), m_fpIDX);

				iIndex = __FindEntryIndex (VfsName);
				iv = m_vecVFS.begin ();
				/// VEntry Table을 수정하기 위해 앞쪽으로 이동한다
				fseek (m_fpIDX, SIZEOF_VFILEHEADER, SEEK_SET);
				/// 이전까지는 그냥 스킾
				for(; iv <= m_vecVFS.begin () + iIndex; iv++)
				{
					__SkipVEntry (*iv);
				}
				/// 여기는 건너뛰고
				/// iv++;   /// <== 삭제가 아니기 때문에 건너 뛰면 안 됨
				/// 뒷쪽부터 다시 삭제된 만큼 뺀다
				for(; iv != m_vecVFS.end (); iv++)
				{
					(*iv)->lStartOfEntry -= lOldSize - lNewSize;
					(*iv)->pVFS->SetStartOfEntry ((*iv)->lStartOfEntry);
					__WriteVEntry (*iv);
				}
				CBlankInfo::DoStep ();
				if( itStepPos != vecStepPos.end () )
				{
					CBlankInfo::SetStep ( *itStepPos );
					itStepPos++;
				}
			}
		}
	}

	CBlankInfo::iMaxCount		= 0;
	CBlankInfo::iDealedCount	= 0;
	CBlankInfo::DoStep ();
	CBlankInfo::procCallBack	= NULL;
	
	return true;

}


/****************************************************************************************
 * Vfs Name이 존재하는지 조사한다
 */
bool CVFS_Manager::VfsExists (const char *VfsName)
{
	char uprVfsName[ 1024 ];
	__ConvertPath (VfsName, uprVfsName); /// 대문자로 변환해서 검색

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	for(;iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->sVFSName == uprVfsName) 
			return true;
	}

	return false;
}


/*********************************************************************************
 * 파일존재하는지 검사
 * 주의 : 바깥에 있는 파일 먼저 검사. 그리고, Pack파일 검사
 * Pack파일의 갯수는 그리 많지 않기 때문에 무식하게 for loop를 돌린다
 */
bool CVFS_Manager::FileExists (const char * FileName)
{
	std::vector<VEntry *>::iterator iv;
	char uprFileName[ 1024 ];

	/// FileName이 NULL이면 바로 false를 리턴
	if(FileName == NULL) 
		return false;

	/// 밖에 존재하는 파일일 경우에도 true를 리턴한다
	if(_access (FileName, 0) == 0) 
		return true;

	__ConvertPath (FileName, uprFileName); /// Pack파일내를 검사할때는 대문자 파일 이름을 사용해야 한다

	iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pVE = *iv;

		if(pVE)
		{
			CVFS * pVFS = pVE->pVFS;

			if(pVFS->FileExists (uprFileName))
				return true;
		}
	}

	return false;
}

/// Pack파일안에서만 파일존재하는지 검사
bool CVFS_Manager::FileExistsInVfs (const char * FileName)
{
	std::vector<VEntry *>::iterator iv;
	char uprFileName[ 1024 ];
	
	__ConvertPath (FileName, uprFileName); /// Pack파일내를 검사할때는 대문자 파일 이름을 사용해야 한다
	
	iv = m_vecVFS.begin ();
	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (uprFileName)) 
			return true;
	}

	return false;
}

/***********************************************************************************
 * 파일에 대한 정보를 알아낸다
 */
void CVFS_Manager::GetFileInfo (const char * FileName, VFileInfo * pFileInfo, bool bCalCrc)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (FileName)) 
		{
			(*iv)->pVFS->GetFileInfo (FileName, pFileInfo );

			if( bCalCrc )
				pFileInfo->dwCRC = ComputeCrc ( FileName );

			return ;
		}
	}

	pFileInfo->dwCRC		= 0;
	pFileInfo->dwVersion	= 0;
}


/***********************************************************************************
 * 파일에 대한 정보를 알아낸다
 */
bool CVFS_Manager::SetFileInfo (const char * FileName, VFileInfo * pFileInfo)
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		if((*iv)->pVFS->FileExists (FileName)) 
		{
			return (*iv)->pVFS->SetFileInfo (FileName, pFileInfo);
		}
	}

	return false;
}


/***********************************************************************************
 * 인덱스 파일에 대한 기준버젼을 알아낸다
 */
DWORD CVFS_Manager::GetStdVersion (void)
{
	DWORD dwRet = MAKEDWORD( m_wStdVersion[ 1 ], m_wStdVersion[ 0 ] );
	
	// ((WORD *)&dwRet)[ 0 ] = m_wStdVersion[ 0 ];
	// ((WORD *)&dwRet)[ 1 ] = m_wStdVersion[ 1 ];

	return dwRet;
}


/***********************************************************************************
 * 인덱스 파일에 대한 기준버젼을 설정한다
 */
void CVFS_Manager::SetStdVersion (DWORD dwVersion)
{
	SetStdVersion (HIWORD(dwVersion), LOWORD(dwVersion));
}


void CVFS_Manager::SetStdVersion (WORD wHiVer, WORD wLoVer)
{
	fseek (m_fpIDX, VERSION_POS_STD, SEEK_SET);
	fwrite ((void *)&wLoVer, sizeof (WORD), 1, m_fpIDX);
	fwrite ((void *)&wHiVer, sizeof (WORD), 1, m_fpIDX);
	fflush (m_fpIDX);
}


/***********************************************************************************
 * 적용된 버젼 알아냄
 */
DWORD CVFS_Manager::GetCurVersion (void)
{
	DWORD dwRet = MAKEDWORD( m_wCurVersion[ 1 ] , m_wCurVersion[ 0 ] );
	
	// ((WORD *)&dwRet)[ 0 ] = m_wCurVersion[ 0 ];
	// ((WORD *)&dwRet)[ 1 ] = m_wCurVersion[ 1 ];

	return dwRet;	
}


/***********************************************************************************
 * 적용된 버젼 기록함
 */
void CVFS_Manager::SetCurVersion (DWORD dwVersion)
{
	SetCurVersion ( HIWORD(dwVersion), LOWORD(dwVersion) );
}


void CVFS_Manager::SetCurVersion (WORD wHiVer, WORD wLoVer)
{
	fseek (m_fpIDX, VERSION_POS_CUR, SEEK_SET);
	fwrite ((void *)&wLoVer, sizeof (WORD), 1, m_fpIDX);
	fwrite ((void *)&wHiVer, sizeof (WORD), 1, m_fpIDX);
	fflush (m_fpIDX);
}


/***********************************************************************************
 * 빈공간의 크기를 조사한다.
 * 리턴 : 성공적으로 조사가 끝나면 공백의 크기. Pack파일이 하나도 없으도 0을 리턴
 */
DWORD CVFS_Manager::GetSizeOfBlankArea (void)
{
	if(m_vecVFS.size () <= 0) 
		return 0;

	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();
	int iSum = 0;
	for(; iv != m_vecVFS.end (); iv++)
	{
		if(*iv) /// iterator에 적합한 값이 들어가 있다고 생각지 말것.
		{
			///
			VEntry * pVEntry = *iv;
			iSum += pVEntry->pVFS->GetSizeofBlankArea ();
		}
	}

	return iSum;
}


// FileName 이 인덱스데이터와 실제 데이터가 일치하는지 체크한다
short CVFS_Manager::TestFile (const char * FileName)
{

	if(FileExistsInVfs ( FileName ) )
	{

		VEntry * pVEntry = GetVEntryWF ( FileName );
		VFileHandle * pVF = OpenFile (FileName );
		if(pVF && pVEntry)
		{
			VfsInfo VfsRange ;
			if(GetVfsInfo ( pVEntry->sVFSName.c_str (), &VfsRange) ) // GetVEntryWF 성공했다면 GetVfsInfo 도 성공해야 한다
			{
				if (pVF->lStartOff < VfsRange.lStartOff || pVF->lEndOff > VfsRange.lEndOff )
				{
					::VCloseFile ( pVF );
					pVF = NULL;
					return VTEST_INVALIDRANGE;
				}
			}
			else
			{
				::VCloseFile ( pVF );
				pVF = NULL;
				return VTEST_CANTKNOWVFSINFO;
			}

			long lSize = GetFileLength ( FileName );
			if( lSize == 0 )
			{
				::VCloseFile ( pVF );
				pVF = NULL;
				return VTEST_ZEROLENGTH;
			}

			BYTE * pbtData = new BYTE[ lSize ];
			if( pbtData )
			{
				size_t stReadCnt = ::vfread (pbtData, sizeof (BYTE), lSize, pVF);
				if( stReadCnt != (size_t)lSize )
				{
					::VCloseFile ( pVF );
					pVF = NULL;

					delete [] pbtData;
					pbtData = NULL;

					return  VTEST_LENGTHNOTMATCH;
				}

				::VCloseFile ( pVF );
				pVF = NULL;

				DWORD dwCRC = CLibCrc::GetIcarusCrc ( pbtData , lSize );
				VFileInfo FileInfo;
				this->GetFileInfo ( FileName, &FileInfo , false);
				if( FileInfo.dwCRC != dwCRC )
				{
					delete [] pbtData;
					pbtData = NULL;

					return VTEST_CRCNOTMATCH;
				}

				delete [] pbtData;
				pbtData = NULL;

				return VTEST_SUCCESS;
			}
			return VTEST_NOTENOUGHMEM;
		}

		return VTEST_CANTOPEN;
	}

	if( FileExists ( FileName ) )
		return VTEST_OUTFILE;

	return VTEST_FILENOTEXISTS;
}


/***************************************************************
 *
 * VFS 에 대해서 조사한다
 *
 */
// Vfs 파일 이름으로 VFS Entry를 조사한다
VEntry * CVFS_Manager::GetVEntry ( const char * VfsName )
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pV = *iv;
		if(pV && CLibString::CompareNI ( pV->sVFSName.c_str () , VfsName) )
			return pV;
	}

	return NULL;
}

// VFS 안에 존재하는 VFS Entry를 검색한다
VEntry * CVFS_Manager::GetVEntryWF ( const char * FileName )
{
	std::vector<VEntry *>::iterator iv = m_vecVFS.begin ();

	for(; iv != m_vecVFS.end (); iv++)
	{
		VEntry * pV = *iv;
		if(pV && pV->pVFS->FileExists ( FileName ) )
			return pV;
	}

	return NULL;
}


bool CVFS_Manager::GetVfsInfo (const char * VfsName, VfsInfo * VI )
{
	VEntry * pVE = GetVEntry ( VfsName );
	if(pVE )
	{
		FILE * FP = pVE->pVFS->GetFP ();
		if(FP)
		{
			VI->lEndOff		= __vfseek (FP, 0 , SEEK_END);
			VI->lStartOff	= pVE->pVFS->GetStartOff ();

			return true;
		}
	}

	return false;
}

// 외부 파일도 포함된다
DWORD CVFS_Manager::ComputeCrc ( const char * FileName)
{
	DWORD dwCrc = 0;
	VFileHandle * pVH = OpenFile ( FileName );
	if(pVH)
	{
		long lSize = GetFileLength ( FileName );
		BYTE * pbtData = new BYTE[ lSize ] ;
		if(pbtData)
		{
			if(::vfread ( pbtData, sizeof (BYTE), lSize, pVH ) == lSize)
				dwCrc = CLibCrc::GetIcarusCrc ( pbtData, lSize );

			delete [] pbtData;
			pbtData = NULL;
		}

		VCloseFile ( pVH );
		pVH = NULL;
	}

	return dwCrc;
}
