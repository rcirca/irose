#include "stdafx.h"

#include <STDLIB.H>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <map>
#include <string>
#include <vector>
#include <fcntl.h>
#include <list>
#include <cassert>
#include "crc.h"
#include "TriggerVFS.h"
#include "VFS.h"
#include "libFile.h"
#include "ErrorEtc.h"
#include "BlankInfo.h"
#include "GlobalData.h"

#define SYSTEM_GRANULARITY (65536) // TODO: Get by GetSystemInfo()


/*****************************************************************
 *
 * Global Variable and Function
 *
 */
short g_nLastErr = 0;

void __SetVfsError (short nCode)
{
	g_nLastErr = nCode;
}

/*********************************************************************************
 * 경로를 VFS에 맞게 고침
 * 대문자로 변환해서 리턴한다
 * 1. 앞쪽과 뒷쪽에 공백을 없앤다
 * 2. / ==> \ 
 * 3. \\ ==> \
 */
bool __stdcall __ConvertPath (const char * path , char * path2 )
{
	int j = 0;
	int i = 0;
	char tmpPath[ KCHS_MAX_PATH ];
	strcpy (tmpPath, path);
	int iLength = strlen (tmpPath);

	while (tmpPath[ iLength - 1] == ' ' && iLength > 0)
		iLength--;

	tmpPath[ iLength ] = '\0';

	while (tmpPath[ i ] == ' ')
		i++;

	for(; i < iLength; i++)
	{
		switch (tmpPath[ i ])
		{
		case '/':  /* / ==> \  */
            path2[ j++ ] = '\\';
			break;

		case '\\': /* \\ ==> \ */
            path2[ j++ ] = '\\';
			if(tmpPath[ i + 1 ] == '\\') 
				i++;
			break;

		default:
			path2[ j++ ] = tmpPath[ i ];
			break;
		}
	}

	path2[ j ] = '\0';
	_strupr (path2);

	return true;
}




/*********************************************************************************
 * 파일 엔트리를 읽는다.
 * @param fe [out] 읽어들 곳
 * @param fp [in] vfs파일
 */
bool VReadFileEntry (FileEntry *fe, FILE * fp)
{	
	char *	buff = NULL;
	short	sLength = 0;

	fread ((void *)&sLength, sizeof (short), 1, fp);	/// 뒤에오는 문자열의 길이를 구한다
	if( sLength > CGlobalData::FILENAME_MAX_LENGTH )
		return false;

	if((buff = new char[ sLength ]))
	{
		ZeroMemory (buff, sLength);						/// 0으로 초기화
		fread (buff, sizeof (char), sLength, fp);		/// NULL문자까지 포함해서 읽어온다
		fe->sFileName = buff;
		delete [] buff;
	}

	fread (&fe->lFileOffset,	sizeof(long), 1, fp);
	fread (&fe->lFileLength,	sizeof(long), 1, fp);
	fread (&fe->lBlockSize,		sizeof(long), 1, fp);
	fread (&fe->cDeleted,		sizeof(BYTE), 1, fp);
	fread (&fe->btCompressType, sizeof(BYTE), 1, fp);
	fread (&fe->btEncType,		sizeof(BYTE), 1, fp);
	fread (&fe->dwVersion,		sizeof(DWORD), 1, fp);	/// 버젼
	fread (&fe->dwCRC,			sizeof(DWORD), 1, fp);

	return true;
}


long __SizeOfFileEntry (FileEntry *FE)
{
	long lSize = 0;

	lSize += (long)FE->sFileName.size () + 1;
	lSize += SIZEOF_FILEENTRY_EXCPTSTRING;

	return lSize;
}



/*********************************************************************************
 * 파일 엔트리를 쓴다
 * @param fe [in] 파일 엔트리
 * @param fp [in] vfs 파일 핸들
 */
bool VWriteFileEntry (FileEntry *fe, FILE * fp, bool bFlush)
{	
	if( fe && fp )
	{
		short sLength = (short)fe->sFileName.size () + 1;

		fwrite ((void *)&sLength	, sizeof (short), 1, fp);
		fwrite ((const void *)fe->sFileName.c_str (), sizeof (char), fe->sFileName.size (), fp);
		fwrite ((void *)"\0"		, sizeof(char), 1, fp);

		fwrite (&fe->lFileOffset,	sizeof(long), 1, fp);
		fwrite (&fe->lFileLength,	sizeof(long), 1, fp);
		fwrite (&fe->lBlockSize,	sizeof(long), 1, fp);
		fwrite (&fe->cDeleted,		sizeof(BYTE), 1, fp);
		fwrite (&fe->btCompressType,sizeof(BYTE), 1, fp);
		fwrite (&fe->btEncType,		sizeof(BYTE), 1, fp);
		fwrite (&fe->dwVersion,		sizeof(DWORD), 1, fp);
		fwrite (&fe->dwCRC,			sizeof(DWORD), 1, fp);
		if(bFlush) 
			return ( fflush (fp) == 0 );

		return true;
	}

	return false;
}


/*********************************************************************************
 * 파일 엔트리를 크기만큼 indicator를 이동시킨다
 * @param pFE skip할 File Entry
 * @param fp 파일포인터
 */
void __SkipFileEntry (FileEntry *pFE, FILE *fp)
{
	short sLength = 0;
	/// 파일이름 길이만큼 skip한다
	fread ((void *)&sLength	, sizeof (short), 1, fp);
	/// 나머지 멤버들의 길이만큼 skip한다. - sizeof(short)는 이미 지나쳤기 때문에 뺀다
	fseek (fp, SIZEOF_FILEENTRY_EXCPTSTRING + sLength - sizeof (short), SEEK_CUR);
}



/*********************************************************************************
 * 압축 하기. 지금은 구현되지 않았음. 단순히 넘겨받은 DataBuffer값을 다시 넘겨줌
 * @param pbtData 입력데이터
 * @param lSize pbtData에 저장되어 있는 데이터의 바이트 크기
 * @param btCompress 압축형태. 0이면 압축되어 있지 않음
 * @ppbtOutData 최종 결과는 여기로 리턴
 */
bool Compress (BYTE * pbtData, long lSize, BYTE btCompress, BYTE ** ppbtOutData)
{
	*ppbtOutData = pbtData;

	return true;
}


/*********************************************************************************
 * Encryption 하기. 지금은 구현되지 않았음. 단순히 넘겨받은 DataBuffer값을 다시 넘겨줌
 * @param pbtData 입력데이터
 * @param lSize pbtData에 저장되어 있는 데이터의 바이트 크기
 * @param btEncType Encryption 형태. 0이면 압축되어 있지 않음
 * @ppbtOutData 최종 결과는 여기로 리턴
 */
bool Encrypt (BYTE * pbtData, long lSize, BYTE btEncType, BYTE ** ppbtOutData)
{
	*ppbtOutData = pbtData;

	return true;
}





/*********************************************************************************
 *
 *                                   Class CVFS
 *
 */


CVFS::CVFS ()
{
	m_dwNum			= 0;	/// 파일엔트리의 갯수
	m_lStartOffset	= 0;	/// 데이터의 시작. 사용하지는 않음
	m_fp			= NULL;
	m_fpFAT			= NULL;
	m_Buffer		= NULL;
	m_BUFSIZ		= BUFSIZ;
	m_lEntryStart	= 0;

	m_is.clear ();
	m_si.clear ();
	m_ve.clear ();
}


/*********************************************************************************
 * File Pointer를 이용하여 VFS 파일 클래스를 초기화한다
 */
CVFS::CVFS (FILE * fpFAT, long lOffset, DWORD dwNum, const char *VfsName, char * Mode)
{
	m_dwNum			= dwNum;		/// 파일엔트리의 갯수
	m_lStartOffset	= 0;			/// 데이터의 시작. 사용하지는 않음
	m_fp			= NULL;
	m_fpFAT			= fpFAT;
	m_Buffer		= NULL;
	m_BUFSIZ		= BUFSIZ;
	m_lEntryStart	= lOffset;
	m_sFileName		= VfsName;		/// vfs 파일명

	m_is.clear ();
	m_si.clear ();
	m_ve.clear ();

	if((m_fp = fopen (VfsName, Mode)))
		 __ReadFileEntry ();
}


/*********************************************************************************
 * Destructor
 */
CVFS::~CVFS ()
{
	if(m_fp) 
	{ 
		Close (); 
	}
}


/*********************************************************************************
 * Vfs파일을 오픈한다. 
 * @param Mode : r(읽기만). w+(쓰기/읽기. 파일있으면 내용 지워짐), r+(읽기/쓰기. 파일없으면 에러)
 * @param Dir 마지막에 "\"문자까지 포함
 */
bool CVFS::Open (FILE * fpFAT, long lOffset, const char *VfsName, const char *Dir, const char * Mode)
{
	m_dwNum			= 0;			/// 파일엔트리의 갯수
	m_dwDelCnt		= 0;
	m_lStartOffset	= 0;			/// 데이터의 시작. 사용하지는 않음

	m_fp			= NULL;
	m_fpFAT			= fpFAT;
	m_hFile			= INVALID_HANDLE_VALUE;
	m_hMap			= NULL;

	m_Buffer		= NULL;
	m_BUFSIZ		= BUFSIZ;
	m_lEntryStart	= lOffset;
	m_sFileName		= VfsName;		/// vfs 파일명
	std::string	sOpenPath = Dir;
	sOpenPath.append (VfsName);

	m_is.clear ();
	m_si.clear ();
	m_ve.clear ();

	if( _access( sOpenPath.c_str(), 0 ) != 0 ) // 파일이 존재하지 않으면
		return false;

	/// 읽기
	::_fmode = _O_BINARY;
	if ((strcmp (Mode, "mr") == 0) || (strcmp (Mode, "mr+") == 0))
	{
		m_hFile = CreateFile( VfsName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL);
		if (m_hFile != INVALID_HANDLE_VALUE) 
		{
			m_hMap = CreateFileMapping( m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );
			if (m_hMap) 
				return __ReadFileEntry ();
		}
	}
	/// w+ , 쓰기/읽기모드 + 생성. 기존의 파일이 있으면 내용이 지워짐
	else if(strcmp (Mode, "w+") == 0)
	{
		if((m_fp = fopen (sOpenPath.c_str (), "w+"))) /// 실제 오픈은 절대 경로로 한다
		{
			m_dwDelCnt	= 0;
			m_dwNum		= 0;
			/// 현재 초기화 되어 있는 멤버값으로 초기화한다
			__VWriteVFSHeader (m_dwNum, m_dwDelCnt, m_lStartOffset);
			return true;
		}
	}
	else if(strcmp (Mode, "r") == 0 || strcmp (Mode, "r+") == 0)
	{
		m_fp = fopen (sOpenPath.c_str (), Mode); /// 실제 오픈은 절대 경로로 한다
		if (m_fp) 
			return __ReadFileEntry ();
	}
	
	return false;
}


/*********************************************************************************
 * vfs파일을 닫는다
 */
void CVFS::Close (void)
{
	/// 오픈된 vfs파일을 닫는다
	if(m_fp)	{ fclose (m_fp);	m_fp	= NULL; }
	if (m_hFile != INVALID_HANDLE_VALUE) 
	{ 
		if(m_hMap)
		{
			CloseHandle (m_hMap);
			m_hMap = NULL;
		}

		CloseHandle( m_hFile ); 
		m_hFile = INVALID_HANDLE_VALUE; 
	}

	if(m_fpFAT)	
	{
		fclose (m_fpFAT); 
		m_fpFAT = NULL;
	}

	std::vector<FileEntry *>::iterator iv = m_ve.begin ();
	/// new된 FileEntry *를 delete한다
	for(; iv != m_ve.end (); iv++)
	{
		if(*iv) 
			delete *iv;
	}
	m_ve.clear ();

	m_is.clear ();
	m_si.clear ();

	m_dwNum			= 0;
	m_lStartOffset	= 0;

	/// 버퍼로 사용되었던 메모리를 해제한다
	if(m_Buffer) 
	{ 
		delete [] m_Buffer; 
		m_Buffer = NULL; 
	}
}


bool CVFS::__ReadFileEntry (void)
{
	FileEntry *fe = NULL;

	if (!m_fp && (this->m_hFile == INVALID_HANDLE_VALUE)) 
		return false;

	/// 파일 앞으로 이동
	long lMaxOffset = 0;
	if(m_hFile != INVALID_HANDLE_VALUE)		// Memory Mapped IO 가 아니면 파일의 크기를 조사한다
		lMaxOffset = __fseek (m_fp, 0, SEEK_END);

	fseek (m_fpFAT, m_lEntryStart, SEEK_SET);
	fread ((void *)&m_dwNum			, sizeof (DWORD), 1, m_fpFAT); /// 갯수를 읽는다
	fread ((void *)&m_dwDelCnt		, sizeof (DWORD), 1, m_fpFAT); /// 삭제항목의 갯수를 읽는다
	fread ((void *)&m_lStartOffset	, sizeof (long)	, 1, m_fpFAT); /// vfs파일에서 데이터의 시작

	// 값 체크
	if( m_dwNum > CGlobalData::FILEENTRY_MAX_COUNT || m_dwNum < 0 
		|| m_dwDelCnt > CGlobalData::FILEENTRY_MAX_COUNT || m_dwDelCnt < 0 )
		return false;

	// Memory Mapped IO 가 아니면 범위를 체크한다
	if(m_hFile != INVALID_HANDLE_VALUE && m_lStartOffset > lMaxOffset )
		return false;

	for(unsigned int i = 0; i < m_dwNum; i++)
	{
		if((fe = new FileEntry))
		{
			if( !VReadFileEntry (fe, m_fpFAT) )
				return false;

			/// 삭제항목이 아니면. 
			if(fe->cDeleted == 0)
			{
				m_ve.push_back (fe); /// 이 리스트에는 
				m_is[ i ]				= fe->sFileName;
				m_si[ fe->sFileName ]	= i;
			}
			/// 삭제항목이면 삭제항목리스트에 추가
			else
			{ 
				/// m_is, m_si맵에 추가하지 않으면 파일엔트리에 접근할수 없다.
				/// 하지만 vector에는 추가해 놓는다. 나중에 재사용되는 경우를 위해서
				m_ve.push_back (fe); /// 이 리스트에는 
				m_mapDel[ i ] = fe; 
			}

		}
		else
			return false;
	}

	return true;
}


//int CVFS::index (std::string sKey)
//{
//	std::map<std::string, int>::iterator im = m_si.find (sKey);
//	if(im != m_si.end ()) { return im->second; }
//
//	return -1;
//}


int CVFS::index (const char * sKey)
{
	char buff[1024];
	
	__ConvertPath (sKey, buff);

	int iRet = -1;
	std::map<std::string, int>::iterator im = m_si.find (buff);
	if(im != m_si.end ()) 
	{ 
		iRet = im->second; 
		return iRet;
	}

	return -1;
}



std::string CVFS::index (int iKey)
{
	std::string strRet = "";

	std::map<int, std::string>::iterator im = m_is.find (iKey);
	if(im != m_is.end ()) 
	{
		strRet = im->second;
	}

	return strRet;;
}




bool CVFS::__SortFiles (const char **Files, int iNum)
{
	int i = 0, j = 0;
	const char * temp = NULL;
	std::list<const char *> list;

	for(i = 0; i < iNum - 1; i++)
	{
		for(j = i + 1; j < iNum; j++)
		{
			if(index(Files[ j ]) < index (Files[ i ]))
			{
				temp = Files[ i ];
				Files [ i ] = Files [ j ];
				Files [ j ] = temp;
			}
		}
	}

	return true;
}






/**
 * vfs파일을 처음 만들었을때 초기화 시켜 준다
 * @param dwNum			vfs파일의 파일 갯수
 * @param lStartOffset	vfs파일의 데이터 시작오프셋
 */
void CVFS::__VWriteVFSHeader (DWORD dwNum, DWORD dwDelCnt, long lStartOffset)
{
	/// 파일의 앞부분으로 파일 indicator를 이동시킨다
	fseek (m_fpFAT, m_lEntryStart, SEEK_SET);

	fwrite (&dwNum			, sizeof(DWORD)	, 1, m_fpFAT);
	fwrite (&dwDelCnt		, sizeof(DWORD)	, 1, m_fpFAT);
	fwrite (&lStartOffset	, sizeof(long)	, 1, m_fpFAT);
	fflush (m_fpFAT);
}


/********************************************************************************************
 * map에서 파일이름에 해당하는 엔트리를 제거한다. 
 * @param FileName	지울 파일명
 * @param iIndex	지울 인덱스
 */
void CVFS::__EraseMemEntry (const char *FileName, int iIndex)
{
	std::vector<FileEntry *>::iterator iv;
	std::map<std::string, int>::iterator iSI;
	std::map<int, std::string>::iterator iIS;

	/// iSI->second == iIndex 여야 함
	iSI = m_si.find (FileName);
	iIS = m_is.find (iIndex);
	/// map에서 지운다
	m_si.erase (iSI);
	m_is.erase (iIS);
	/// 벡터에서 지운다
	iv = m_ve.begin () + iIndex;
	m_ve.erase (iv);
}


/********************************************************************************************
 * 파일을 뒤쪽에 추가할때 사용
 * @param FileName		추가될 파일
 * @param TargetName	이 이름으로 추가된다. 모든 타겟파일명은 대문자로 변환된다
 * @param pVFile		vfs파일 핸들
 * @param btEncType		Encryption 타입
 * @param btCompress	압축형태
 */
short CVFS::AddFile (const char * FileName
					, const char * uprTargetName
					, DWORD dwVersion
					, DWORD dwCRC
					, BYTE btEncType
					, BYTE btCompress
					, bool bUseDel)
 {
	FILE * fp						= NULL;	/// 추가할 파일의 파일포인터
	BYTE * pbtBuff					= NULL;	/// 추가할 파일에서 읽은 데이터를 여기에 저장
	BYTE * pbtChangedData			= NULL;	/// Encryption이나 Compress한 데이터는 여기로 리턴
	FileEntry * pFileEntry			= NULL; /// 새롭게 new한 FileEntry는 여기에 저장
	/// DWORD dwCRC					= 0;	/// CRC값
	bool bError						= false;/// 처리과정중 에러가 발생하면 여기에 true
	bUseDel							= false; /// 강제로 false로 함. 이 부분 잘 못됐음. 고쳐야 함

	/// 파일이 존재하는지 확인. "rb" binary mode로 열어야 함
	::_fmode = _O_BINARY;
	
	short nErrorCode = VADDFILE_SUCCESS;

	if( _access (FileName, 00) != 0 )
		return VADDFILE_DONTEXIST;

	if( (fp = fopen (FileName, "rb")) == NULL ) 
		return VADDFILE_CANTOPENFILE;
	
	if(FileExists (uprTargetName)) 
		return VADDFILE_EXISTSALREADY;

	/// 파일의 크기를 알아낸다
	long lSize = __vfseek (fp, 0, SEEK_END);

	if(lSize <= 0)
	{
		fclose ( fp );
		fp = NULL;
		return VADDFILE_ZEROFILESIZE;
	}

	/// 파일 indicator를 다시 처음으로 돌려 놓는다
	fseek (fp, 0, SEEK_SET);

	if((pbtBuff = new BYTE[ lSize ]))
	{
		fread (pbtBuff, sizeof (BYTE), (size_t)lSize, fp);	/// 파일의 내용을 읽어 들인다
		fclose (fp);	// 파일을 Close 한다

		int iIndex = 0;
		/// 지워진 블럭을 사용하고, 적당한 파일크기의 블럭을 찾으면
		if(bUseDel && (pFileEntry = __FindBlock (FileName, lSize, &iIndex)))
		{
			pFileEntry->sFileName		= uprTargetName;
			pFileEntry->btEncType		= btEncType;
			pFileEntry->btCompressType	= btCompress;
			pFileEntry->lFileLength		= lSize;
			pFileEntry->cDeleted		= 0;
			pFileEntry->dwVersion		= dwVersion;
			pFileEntry->dwCRC			= dwCRC;

			std::vector<FileEntry *>::iterator iv = m_ve.begin (); /// 지워진 파일 엔트리 앞까지 이동한다

			fseek (m_fpFAT, m_lEntryStart + SIZEOF_VENTRYHEADER, SEEK_SET); /// 파일엔트리 앞까지 이동하고
			for(int i = 0; i < iIndex; i++) 
			{ 
				__SkipFileEntry (*iv, m_fpFAT); 
				iv++; 
			}

			::VWriteFileEntry (pFileEntry, m_fpFAT); /// 이동한 다음 파일엔트리를 갱신한다
			fflush (m_fpFAT);

			fseek (m_fp, pFileEntry->lFileOffset, SEEK_SET);
			fwrite (pbtBuff, sizeof(BYTE), (size_t)lSize, m_fp);

			// 2004 10 28 수정
			// VFS 파일을 flush 하고 인덱스를 갱신하고 인덱스파일을 flush 한다
			fflush (m_fp);
			m_dwNum++;
			__VWriteVFSHeader (m_dwNum, m_dwDelCnt, m_lStartOffset);
			fflush (m_fpFAT);

			delete [] pbtBuff; // 메모리를 해제한다.
			pbtBuff = NULL;
			delete pFileEntry; 
			pFileEntry = NULL;

			return VADDFILE_SUCCESS;
		}
		/// 지워진 블록을 사용 안 하거나 , 위가 실패했을 경우
		else
		{
			long lOldSize = __vfseek (m_fp, 0, SEEK_END); /// vfs파일의 크기
			size_t stWrtSize = 0;
			int iRetFlush = 0;
			if( ( stWrtSize = fwrite (pbtBuff, sizeof(BYTE), (size_t)lSize, m_fp) ) == lSize 
				&& (iRetFlush = fflush (m_fp)) != EOF)
			{
				pFileEntry = new FileEntry; /// 파일 엔트리를 만든다
				pFileEntry->sFileName		= uprTargetName;
				pFileEntry->btEncType		= btEncType;
				pFileEntry->btCompressType	= btCompress;
				pFileEntry->lFileLength		= lSize;
				pFileEntry->lBlockSize		= lSize;
				pFileEntry->cDeleted		= 0;
				pFileEntry->dwVersion		= dwVersion;
				pFileEntry->dwCRC			= dwCRC;
				/// File Offset : 이 버젼은 무조건 뒤에 붙이니까 FileOffset 은 기존의 파일 크기가 됨
				pFileEntry->lFileOffset		= lOldSize ;

				/// 파일엔트리 테이블의 크기
				long lSizeOfIdxFile = __vfseek (m_fpFAT, 0, SEEK_END);
				long lTableSize		= SizeOfEntryTable ();
				::__MoveFileBlock (m_lEntryStart + lTableSize
					, lSizeOfIdxFile - (m_lEntryStart + lTableSize)
					, m_lEntryStart + lTableSize + SIZEOF_FILEENTRY_EXCPTSTRING + pFileEntry->sFileName.size () + 1
					, 1000000
					, m_fpFAT
					, true); 
				/* 파일엔트리 하나의 크기 : SIZEOF_FILEENTRY_EXCPTSTRING + pFileEntry->sFileName.size () + 1 */
				fseek (m_fpFAT, m_lEntryStart + lTableSize, SEEK_SET);

				VWriteFileEntry (pFileEntry, m_fpFAT, true); /// File Entry를 쓴다

				__AddMemEntry (pFileEntry);

				m_dwNum++;
				__VWriteVFSHeader (m_dwNum, m_dwDelCnt, m_lStartOffset);
				fflush (m_fpFAT);

				delete [] pbtBuff; // 메모리를 해제한다.
				pbtBuff = NULL;

				return VADDFILE_SUCCESS;
			}

			delete [] pbtBuff;
			pbtBuff = NULL;

			CErrorInfo::Refresh ();

			if(stWrtSize != lSize )
				return VADDFILE_CANTWRITE;
			if(iRetFlush == EOF)
				return VADDFILE_CANTFLUSH;

			return VADDFILE_CVFS_AddFile_NONOE;

		}

	}

	return VADDFILE_MEMALLOCFAIL;

}


/********************************************************************************************
 * vector와 map에 엔트리를 추가한다
 * @param FE 추가할 FileEntry
 */
void CVFS::__AddMemEntry (FileEntry *FE)
{
	std::map<int, std::string>::iterator im = m_is.end ();
	im--;
	int iAdd = im->first + 1;
	m_is[     iAdd		] = FE->sFileName;
	m_si[ FE->sFileName ] = iAdd;
	m_ve.push_back (FE);
}


/********************************************************************************************
 * 파일엔트리테이블이 차지하는 크기를 구한다
 */
long CVFS::SizeOfEntryTable (void)
{
	long lEndOfThisEntry = SIZEOF_FILEENTRY_HEADER;

	std::vector<FileEntry *>::iterator im = m_ve.begin ();
	int i = 0;
	for(; im != m_ve.end (); im++) 
	{ 
		FileEntry * pFE = *im;
		lEndOfThisEntry += (long)pFE->sFileName.size () + 1 + SIZEOF_FILEENTRY_EXCPTSTRING; 
		i++;
		if(pFE->sFileName.size () > 600)
			int zzz = 0;
		if(lEndOfThisEntry < 0)
		{
			size_t aaaa = pFE->sFileName.size ();
			int cc = 0;
		}
	}

	return lEndOfThisEntry;
}


/********************************************************************************************
 * vfs파일에서 특정파일을 오픈해 준다
 * @param FileName 파일명
 * @param Mode 모드. "w", "r"
 * @param pVFile vfs파일 핸들
 */
VFileHandle * CVFS::OpenFile (const char * FileName)
{
	VFileHandle * pVFH	= NULL;
	FileEntry * pFE		= NULL; /// 여기 찾은 파일의 파일 엔트리를 저장한다

	int iFind = index (FileName);
	/// 파일이름의 키가 있으면 계속한다
	if(iFind >= 0)
	{
		pFE = *(m_ve.begin () + iFind);
		if((pVFH = new VFileHandle))
		{
			pVFH->hCVFS = this;
			pVFH->lCurrentOffset	= 0;
			pVFH->fp				= NULL;
			pVFH->lStartOff			= pFE->lFileOffset;
			pVFH->lEndOff			= pFE->lFileOffset + pFE->lFileLength;
			pVFH->sFileName			= FileName;
			pVFH->btFileType		= 0; /// Packed 파일일 경우 0
			pVFH->btEncrypted		= pFE->btEncType;
			
			//pVFH->hMap = CreateFileMapping( this->m_hFile, NULL, PAGE_READONLY, 0, 0, NULL );
			// Memory Mapped 일 경우
			if (m_hMap) {
				// MapViewOfFile() 의 파일오프셋은 시스템의 Granularity 단위로 끊어져야 함.
				// 따라서, iAllocOffset을 통해 실제 오프셋을 기억토록 함.
				pVFH->iAllocOffset = pFE->lFileOffset % SYSTEM_GRANULARITY;
				pVFH->pData = reinterpret_cast<char *>(MapViewOfFile( m_hMap, FILE_MAP_READ, 0, 
					pFE->lFileOffset - pVFH->iAllocOffset, pFE->lFileLength + pVFH->iAllocOffset));	
			}
			/// 그냥 파일방식
			else {

				pVFH->fp	= m_fp;
				pVFH->pData = NULL;
			}
			/// 파일 indicator를 이동시킨다
			//fseek (m_fp, pFE->lFileOffset, SEEK_SET);
		}
	}

	return pVFH;
}


/******************************************************************************************
 * 오픈한 파일을 닫는다. new VFileHand을 delete한다
 * @param FH 오픈한 파일핸들
 */
void CVFS::CloseFile (VFileHandle * FH)
{
	if(FH->btFileType) 
		fclose (FH->fp);		/// Packed 파일이 아닐 경우 일반파일을 오픈했으므로 닫는다
	else 
	{
		if (FH->pData) 
		{
			UnmapViewOfFile (FH->pData);
			FH->pData = NULL;
		}
	}

	delete FH;
}


/******************************************************************************************
 * 파일의 크기를 알아낸다
 * @param FileName 파일명
 * @param pVFile vfs파일 핸들
 * @return 있으면 크기를, 없으면 -1을 리턴한다. 0크기의 파일이 있을 수 있다. 파일이 vfs내에 없으면 -1을 리턴
 */
long CVFS::GetFileLength (const char * FileName)
{
	int iIndex		= -1;
	FileEntry * pFE	= NULL; /// 여기 찾은 파일의 파일 엔트리를 저장한다

	iIndex = index (FileName);
	/// 파일이름의 키가 있으면 계속한다
	if(iIndex >= 0)
	{
		std::vector<FileEntry *>::iterator iv = m_ve.begin () + iIndex;
		if(iv != m_ve.end ())
		{
			pFE = *iv;
			return pFE->lFileLength;
		}
	}

	return -1;
}


/******************************************************************************************
 * 파일의 갯수 : 총 갯수 - 지워진 갯수
 */
DWORD CVFS::GetEntryCount (void)
{
	return m_dwNum;
}

/******************************************************************************************
 * 파일의 갯수 : 총 갯수 - 지워진 갯수
 */
DWORD CVFS::GetFileCount (void)
{
	return (m_dwNum - m_dwDelCnt);
}
	

/******************************************************************************************
 * 파일 이름 가져오. 메모리 할당은 호출하는 쪽에서
 */
DWORD CVFS::GetFileNames (char **pFiles, DWORD nFiles, int nMaxPath)
{
	DWORD iGet = 0;

	if(nFiles > m_dwNum) { nFiles = m_dwNum; }

	std::vector<FileEntry *>::iterator iv = m_ve.begin ();

	for(; iv != m_ve.end () && iGet < nFiles; iv++)
	{
		/// 지워진 항목이 아니면 파일명을 카피한다
		if(!(*iv)->cDeleted)
		{
			strcpy (pFiles [ iGet ], (*iv)->sFileName.c_str ());
			iGet++;
		}
	}

	return iGet;
}


/******************************************************************************************
 * 지워진 파일엔트리중 이 크기가 들어 갈수 있는 블륵을 찾는다. bRestore == true 이면 복구한다
 * @param lSize		찾을 블록의 크기
 * @param bRestore	복원할지 말지 결정
 * @param iIndex	[out] 여기에 찾은 인덱스를 넣는다
 * @return 찾으면 적당한 파일엔트리 포인터, 못 찾으면 NULL을 리턴
 */
FileEntry * CVFS::__FindBlock (std::string sFileName, long lSize, int *iIndex, bool bRestore)
{
	std::map<int, FileEntry *>::iterator im = m_mapDel.begin ();
	FileEntry * FE = NULL;

	for(; im != m_mapDel.end (); im++)
	{
		FE = im->second;
		if(FE->lBlockSize > lSize) 
		{
			if(bRestore)
			{
				m_is[ im->first ] = sFileName;
				m_si[ sFileName ] = im->first;
				*iIndex = im->first;
				return FE;
			}
			else
			{ 
				*iIndex = im->first;
				return FE; 
			}
		}
	}

	return NULL;
}


/******************************************************************************************
 * 파일을 지우는데 파일을 정리하지 않고 그냥 빈공간으로 놔 두는 버젼
 * @param Files 지울 파일명이 저장되어 있음
 * @param iNum	지울 파일의 갯수
 * @return VRMVFILE_XXXX, 성공하면 VRMVFILE_SUCCESS을 리턴
 */
short CVFS::RemoveFilesB (const char **Files, int iNum)
{
	/// 인덱스와 파일명을 하나로 묶어서 관리
	struct sFind
	{
		int			iIndex;
		const char *sFileName; /// 여기에 std::string을 사용하면 delete pFInd에서 에러 발생
	};

	/// 파일이름을 파일엔트리순으로 정렬한다
	/// __SortFiles (Files, iNum);
	/// 먼저 인덱스를 찾아서 여기에 저장
	sFind *	pFind = new sFind[ iNum ];
	int		iTemp;
	int		iFind = 0, i = 0;

	if(pFind)
	{
		/// 인덱스를 찾아서 저장한다
		for(i = 0; i < iNum; i++)
		{
			/// 인덱스를 못 찾을 경우 0보다 작은 값이다. 이 경우 아예 제외시켜 버림
			if((iTemp = index (Files[ i ])) >= 0) 
			{ 
				pFind[ i ].iIndex		= iTemp; 
				pFind[ i ].sFileName	= Files[ i ];
				iFind++; 
			}
		}

		if(iFind > 0)
		{
			std::vector<FileEntry *>::iterator iv = m_ve.begin ();
			/// 파일엔트리를 다시 쓰기 위해 파일엔트리 앞쪽으로 이동한다
			fseek (m_fpFAT, m_lEntryStart + SIZEOF_VENTRYHEADER, SEEK_SET);
			/// fseek (m_fpFAT, m_lStartOffset + SIZEOF_VENTRYHEADER, SEEK_SET);
			/// 제일 앞쪽부터 파일엔트리를 스킾한다
			i = 0;
			for(int j = 0; j <= pFind[ iFind - 1 ].iIndex; j++)
			{
				/// 지울려는 파일이름하고 같으면 다시 쓰고
				if((*iv)->sFileName == pFind[ i ].sFileName && (*iv)->cDeleted != 1)
				{
					(*iv)->cDeleted = 1;
					/// 1로 마킹한 것을 다시 파일에 쓴다
					::VWriteFileEntry (*iv, m_fpFAT, false);
					/// 맵에서 제거한다
					m_si.erase (m_si.find (pFind[ i ].sFileName));
					m_is.erase (m_is.find (pFind[ i ].iIndex));
					/// 삭제리스트에는 추가한다
					m_mapDel[ pFind[ i ].iIndex ] = *iv;
					/// 삭제항목의 갯수를 1 증가시킨다
					m_dwDelCnt++;
					/// 다음 파일이름으로 이동하기 위해 i++
					i++;
				}
				/// 아니면 그냥 스킾한다
				else
				{ 
					::__SkipFileEntry (*iv, m_fpFAT); 
				}
				/// 반복자를 하나 증가
				iv++;
			}
			/// flush는 한번에
			fflush (m_fpFAT);
			/// 파일헤더를 쓴다
			__VWriteVFSHeader (m_dwNum, m_dwDelCnt, m_lStartOffset);

			delete[] pFind;
			pFind = NULL;

			return VRMVFILE_SUCCESS;
		}	

		delete[] pFind;
		pFind = NULL;

		return VRMVFILE_DONTEXIST;
	}
	
	return VRMVFILE_MEMALLOCFAIL;
}


/*******************************************************************************************
 * 파일에 대한 정보를 알아냄
 */
void CVFS::GetFileInfo (const char * FileName, VFileInfo * pFileInfo)
{
	int iIndex = 0;
	FileEntry * pFE = NULL;

	if((iIndex = index (FileName)) >= 0)
	{
		pFE = *(m_ve.begin () + iIndex);
		pFileInfo->dwCRC		= pFE->dwCRC;
		pFileInfo->dwVersion	= pFE->dwVersion;
	}
}


/*******************************************************************************************
 * 파일에 대한 정보를 Update함
 */
bool CVFS::SetFileInfo (const char * FileName, VFileInfo * pFileInfo)
{
	int i = 0;
	int iIndex = index (FileName);

	if(iIndex >= 0)
	{
		std::vector<FileEntry *>::iterator iv = m_ve.begin ();
		/// 파일엔트리를 다시 쓰기 위해 파일엔트리 앞쪽으로 이동한다
		fseek (m_fpFAT, m_lEntryStart + SIZEOF_VENTRYHEADER, SEEK_SET);
		/// 제일 앞쪽부터 파일엔트리를 스킾한다
		for(; i < iIndex; i++)
		{
			::__SkipFileEntry (*iv, m_fpFAT); 
			/// 반복자를 하나 증가
			iv++;
		}
		(*iv)->dwCRC		= pFileInfo->dwCRC;			/// 수정함
		(*iv)->dwVersion	= pFileInfo->dwVersion;
		VWriteFileEntry (*iv, m_fpFAT);				/// 파일에 기록함
		/// flush는 한번에
		fflush (m_fpFAT);

		return true;
	}
	
    return false;
}


/******************************************************************************************
 * vfs파일내에 있는 공백을 정리한다. RemoveFilesB ()를 호출했을때 생긴 공백들
 * 삭제하는 부분 : 파일 뒤의 여백 <=== 이 부분은 추가된 파일이 지워진 파일엔트리에 덮어쓰기 된 경우
 *                 , 지워진 엔트리
 */
struct sDeleteBlockB
{
	long lTarget;		/// 옮겨지는 위치
	long lStart;		/// 옮길 부분의 시작. 뒤에 없으면(마지막) -1
	long lLength;		/// 옮길 부분의 길이. 0이 될수 있다
	long lDelLength;	/// 지울 부분의 길이
};
bool CVFS::ClearBlank (void)
{
	int i = 0;

	sDeleteBlockB * pBlock = NULL;

	/// 삭제할 위치 저장
	std::list< std::vector< FileEntry * >::iterator > listDel;
	std::list< sDeleteBlockB * > listDelBlok;
	/// vfs파일의 크기
	long lFileSize		= __vfseek (m_fp, 0, SEEK_END);					/// Vfs 파일의 크기
	
	std::vector<FileEntry *>::iterator iv = m_ve.begin ();
	/// 파일엔트리도 동시에 수정
	fseek (m_fpFAT, m_lEntryStart + SIZEOF_VENTRYHEADER, SEEK_SET);
	/// 삭제되는 길이는 전부 여기에 더해진다
	long lDeletedLength = 0;
	/// 삭제되는 엔트리 길리
	long lDeletedEntryLength = 0;
	/// for loop안에서 vector element를 삭제한다. <== 주의할 것
	for(; iv != m_ve.end (); iv++)
	{
		/// 엔트리 전체 삭제일 경우
		if((*iv)->cDeleted)
		{
			pBlock = new sDeleteBlockB; assert (pBlock);
			pBlock->lTarget		= (*iv)->lFileOffset - lDeletedLength;		/// 옮길 위치
			pBlock->lStart		= (*iv)->lFileOffset + (*iv)->lBlockSize;	/// 옮겨지는 위치	
			pBlock->lLength		= 0;										/// 옮길 길이. 삭제인 경우 0
			pBlock->lDelLength	= (*iv)->lBlockSize;						/// 삭제되는 길이
			listDelBlok.push_back (pBlock);									/// vector에 저장
			listDel.push_back (iv);
			lDeletedLength		+= (*iv)->lBlockSize;						/// 총지워진 길이 += 이 블럭 크기
			lDeletedEntryLength += __SizeOfFileEntry (*iv);
			/// 이 블럭은 지워지니까 덮어쓰기 되어야 한다
		}
		/// 부분적인 이동인 경우
		else if((*iv)->lBlockSize > (*iv)->lFileLength)
		{
			pBlock = new sDeleteBlockB; assert (pBlock);
			/// 옮길 위치 = 이 파일이 뒤쪽 - 앞쪽에서 삭제된 길이
			pBlock->lTarget		= (*iv)->lFileOffset + (*iv)->lFileLength - lDeletedLength;	
			pBlock->lStart		= (*iv)->lFileOffset + (*iv)->lBlockSize;	/// 옮겨지는 위치
			/// 옮길 길이. (총 블럭의 크기 - 파일이 차지하는 크기)만큼이 옮겨져야 한다
			pBlock->lLength		= (*iv)->lBlockSize - (*iv)->lFileLength;
			pBlock->lDelLength	= (*iv)->lBlockSize;/* 이상함 */			/// 지워지는 길이	
			listDelBlok.push_back (pBlock);									/// vector에 저장
			(*iv)->lFileOffset	-= lDeletedLength;							/// 살아 남은 놈은 앞으로 전진해야 함
			(*iv)->lBlockSize	= (*iv)->lFileLength;						/// 뒷부분을 삭제하니까 이제 Block Size와 File Lenght는 같다
			VWriteFileEntry (*iv, m_fpFAT, false);							/// 수정된 부분이 있으니까 다시 엔트리를 쓴다.
			lDeletedLength		+= (*iv)->lBlockSize - (*iv)->lFileLength;	/// 뒷부분 지워지는 만큼 총 지워지는 길이에 더한다
		}
		/// 앞쪽에 지워지는 블럭이 있으면 이블록은 단순히 크기만 추가해서 같이 이동되도록한다
		else if (pBlock)
		{
			pBlock->lLength		+= (*iv)->lBlockSize;
			(*iv)->lFileOffset	-= lDeletedLength;							/// 앞쪽에 삭제된 길이만큼 앞으로 당겨져야 하니까 여기도 수정
			::VWriteFileEntry (*iv, m_fpFAT, false);						/// 앞쪽에 수정된 게 있으니까 여기도 수정 다씨 write file
		}
		/// 그냥 엔트리 이동
		else
		{
			(*iv)->lFileOffset	-= lDeletedLength;
			VWriteFileEntry (*iv, m_fpFAT, false);
		}

		CBlankInfo::DoStep ();
	}
	fflush (m_fpFAT);																/// 인덱스파일에 fflush한다
	/// ::__ftruncate (__vfseek (m_fpFAT, 0, SEEK_END) - lDeletedEntryLength, m_fpFAT);	/// 인덱스파일의 크기를 조정한
	/// 인덱스 파일하나에 여러개의 Vfs가 들어 가 있을때는 Manager에서 뒷부분을 정리해 준다

	std::list< sDeleteBlockB * >::iterator il = listDelBlok.begin ();

	for(; il != listDelBlok.end (); il++)
	{
		/// 블럭을 이동시킨다
		::__MoveFileBlock ((*il)->lStart, (*il)->lLength, (*il)->lTarget, (int)m_BUFSIZ, m_fp, false);

		CBlankInfo::DoStep ();
	}
	fflush (m_fp);
	/// 현재 위치로 크기를 줄인다
	::__ftruncate (lFileSize - lDeletedLength, m_fp);
	
	std::list< std::vector< FileEntry * >::iterator >::reverse_iterator ir = listDel.rbegin ();
	for(; ir != listDel.rend (); ir++)
	{
		delete *(*ir);														/// 파일엔트리를 메모리공간을 해제한다
		m_ve.erase (*ir);													/// vector에서 제거
	}
	m_dwNum		-= (DWORD)listDel.size ();									/// 엔트리의 갯수를 줄인다
	m_dwDelCnt	= 0;														/// 삭제항목의 갯수를 0으로
	__VWriteVFSHeader (m_dwNum, m_dwDelCnt, m_lStartOffset);				/// 파일헤더를 다시 작성한다
	m_mapDel.clear ();														/// 삭제항목을 가지고 맵을 비운다
	CBlankInfo::DoStep ();													/// 파일 헤더작성까지....

	il = listDelBlok.begin();
	for(; il != listDelBlok.end(); il++)
		delete *il;
	listDelBlok.clear();

	return true;
}


/************************************************************
 * 재사용된 블럭의 갯수를 조사한다
 */
DWORD CVFS::GetReUsedCnt (void)
{
	DWORD dwReUsedCnt = 0;
	std::vector<FileEntry *>::iterator iv = m_ve.begin ();
	for(; iv != m_ve.end () ; iv++)
	{
		if(*iv && (*iv)->lBlockSize > (*iv)->lFileLength)
			dwReUsedCnt++;
	}

	return dwReUsedCnt;
}

/******************************************************************************************
 * 삭제되었지만 블록은 남아있는 엔트리의 갯수를 리턴
 */
DWORD CVFS::GetDelCnt (void)
{
	return m_dwDelCnt;
}


/******************************************************************************************
 * 파일은 있지만 블록크기와 파일크기가 일치하지 않는 엔트리의 갯수를 조사한다. (땜빵당한 블록)
 */
DWORD CVFS::GetFileCntWithHole (void)
{
	DWORD dwCnt = 0;

	std::vector<FileEntry *>::iterator iv = m_ve.begin ();
	for(; iv != m_ve.end (); iv++)
	{
		/// 파일길이와 블럭길이가 다르면 이거는 hole있는 엔트리이다.
		if((*iv)->lBlockSize != (*iv)->lFileLength) 
			dwCnt++;
	}

	return dwCnt;
}


/******************************************************************************************
 * 파일엔트리 테이블의 시작오프셋을 업데이트한다
 */
void CVFS::SetStartOfEntry (long lStartOfEntry)
{
	m_lEntryStart = lStartOfEntry;
}


/******************************************************************************************
 * 파일이 존재하는지 검사한다. 이 FileExists 버젼은 Pack내에 있을때만 true. 
 * 바깥의 파일은 false
 * 주의 사항 :  FileName은 무조건 대문자로 넘어와야 한다
 */
bool CVFS::FileExists (const char *FileName)
{
	std::map<std::string, int>::iterator im ;
	char UpperName[ 1024 ];

	__ConvertPath (FileName, UpperName);
	
	/// 검색해서 end 가 아니면 있는 것이다
	im = m_si.find (UpperName);
	if(im != m_si.end ()) 
		return true;

	return false;
}


/*********************************************************
 * VFS에 대한 FILE * 를 조사한다
 */
FILE * CVFS::GetFP ( void )
{
	return this->m_fp;
}

/********************************************************
 * VFS 파일내에서 Data의 시작 Offset을 조사한다
 */
long CVFS::GetStartOff (void )
{
	return m_lStartOffset;
}


/********************************************************
 * 총 Blank Area 를 계산한다
 */
int CVFS::GetSizeofBlankArea ()
{
	std::vector<FileEntry *>::iterator iv = this->m_ve.begin ();

	long iSum = 0;
	for(; iv != m_ve.end (); iv++)
	{
		if(*iv)
		{
			FileEntry * pFE = *iv;
			if(pFE->cDeleted == 1 ) 
				iSum += pFE->lBlockSize;
			else if (pFE->lBlockSize > pFE->lFileLength)
				iSum += ( pFE->lBlockSize - pFE->lFileLength );
		}
	}

	return iSum;
}
