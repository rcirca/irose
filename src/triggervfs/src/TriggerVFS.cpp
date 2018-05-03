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
#include "__File.h"
#include "TriggerVFS.h"
#include "VFS.h"
#include "VFS_Manager.h"
#include "ErrorEtc.h"
#include "..\..\..\Client\util\classHASH.h"

//#define _ENCRYPTION
char UseEncryption;
char crypttable[KEYSIZE];
/***********************************************************
 *
 * Global Variable and Global Function
 *
 */
short __stdcall VGetError (void )
{
	short nCurError = g_nLastErr;
	g_nLastErr = VERR_NONE;
	return nCurError ;
}

//create crypttable
bool __stdcall InitVFS (DWORD key)
{
	//DWORD EAX=0;
	//DWORD EDI=0;
	//DWORD ECX = key;
	//DWORD EDX = key+1;
	//ECX = ECX*4+1;
	//for(int i = 0; i < 0x400; i++)
	//{
	//	/*DWORD EDI = ECX;
	//	EDI=EDI*EDX;*/
	//	EDI=ECX*EDX;
	//	*((DWORD*)&crypttable[EAX]) = EDI;
	//	EAX+=4;
	//	ECX+=4;
	//	EDX++;
	//}
	//return true;
	DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
	DWORD EDI=0;
	DWORD ECX = key;
	DWORD EDX = key+1;
	ECX = ECX*4+1;
	for(int i = 0; i < 0x400; i++)
	{
		EDI=ECX*EDX;
		*((DWORD*)EAX) = EDI;
		EAX++;
		ECX+=4;
		EDX++;
	}
	return true;
}

/*********************************************************************************
 *
 *
 *                                   Interface
 *
 *
 */


/*********************************************************************************
 * Index 파일과 Index파일에 의해 관리되는 Pack파일을 연다
 * @param FileName Index파일명
 * @param Mode 속성. "r" 읽기 전용, "w" 쓰기전용(사용못함), "w+" 생성 + 쓰기 + 읽기, "r+" 읽기 + 쓰기(생성 못함)
 */
VHANDLE __stdcall OpenVFS (const char * FileName, const char * Mode)
{
 	CVFS_Manager * pVFS = NULL;
	//if(FileName[0]=='_')
	//{
	//	UseEncryption = 1;
	//	return NULL;
	//}
   	if(pVFS = new CVFS_Manager)
	{ 
		if(pVFS->Open (FileName, Mode)) 
			return (VHANDLE)pVFS;
		
		delete pVFS;
		pVFS = NULL;
	}
	
	return (VHANDLE)pVFS;
}


/*********************************************************************************
 * Index File과 VFS파일을 닫는다
 * @param hVFS 오픈한 핸들
 */
void __stdcall  CloseVFS (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		pVFS->Close (); 
		delete pVFS;
		pVFS = NULL;
	}
}


/*********************************************************************************
 * 파일에 대한 정보를 가져옴
 */
void __stdcall VGetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo, bool bCalCrc)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS && FileInfo)
		pVFS->GetFileInfo (FileName, FileInfo, bCalCrc);
}


/*********************************************************************************
 * 파일에 대한 정보를 Update함
 */
bool __stdcall VSetFileInfo (VHANDLE hVFS, const char *FileName, VFileInfo * FileInfo)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		return pVFS->SetFileInfo (FileName, FileInfo);
	}

	return false;
}


/*********************************************************************************
 * Index파일에 Pack파일을 추가한다.
 * 내부적으로 이것 저것 한다.
 */
bool __stdcall VAddVfs (VHANDLE hVFS, const char * VfsName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		return pVFS->AddVFS (VfsName);
	}

	return false;
}


/*********************************************************************************
 * 파일을 추가한다
 * @param hVFS			오픈한 VFS 핸들
 * @param VfsName		Pack 파일 이름
 * @param FileName		추가할 파일명
 * @param TargetName	등록할 파일명
 * @param shVersion 	버젼
 * @param dwCRC			CRC값
 * @param btEncType 	Encript 타입. 0 = 안함. 1 = ?
 * @param btCompress	압축형태. 0 = 안 함. 1 = Zip
 * @param bUseDel		삭제된 블록 사용할 지 여부. true = 사용. false = 사용안함
 */
short __stdcall VAddFile (VHANDLE hVFS
						 , const char * VfsName
						 , const char *FileName
						 , const char * TargetName
						 , DWORD dwVersion
						 , DWORD dwCrc
						 , BYTE btEncType
						 , BYTE btCompress
						 , bool bUseDel)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
	{ 
		// ODS ("파일추가 전 : %s\n", FileName );
		short nRes = pVFS->AddFile (VfsName
								, FileName
								, TargetName
								, dwVersion
								, dwCrc
								, btEncType
								, btCompress
								, bUseDel);
		// ODS ("파일추가 후 : %s\n", FileName);
		
		if(nRes == VADDFILE_CANTWRITE || nRes == VADDFILE_CANTFLUSH )
		{
			DWORD dwErrCode = 0;
			switch ( nRes )
			{
			case VADDFILE_CANTWRITE:
				dwErrCode = CErrorInfo::GetError () + VADDFILE_FIXEVALUE_CANTWRITE ;
				break;

			case VADDFILE_CANTFLUSH:
				dwErrCode = CErrorInfo::GetError () + VADDFILE_FIXEVALUE_CANTFLUSH ;
				break;

			}
			
			if(dwErrCode <= 32767 && dwErrCode > 0)
				nRes = (short)dwErrCode ;
			else
				nRes = VADDFILE_FIOERROR_ETC;
			
		}
		return nRes;
	}

	return VADDFILE_INVALIDHANDLE ;
}


/**********************************************************************************************
 * 한개의 파일을 제거한다. 공백처리하는 버젼임. 파일을 정리하기 위해서는 VClearBlank 를 호출할것
 * Pack파일을 지정하지 않고 지움
 * @param FileName 지울 파일명
 */
short __stdcall VRemoveFile (VHANDLE hVFS, const char *FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS) 
	{
		// ODS ( "파일삭제 전 : %s\n", FileName);
		short bRet = pVFS->RemoveFile (FileName);
		// ODS ( "파일삭제 후 : %s\n", FileName);

		return bRet ;
	}

	
	return VRMVFILE_INVALIDHANDLE;
}


/**********************************************************************************************
 * index에 포함된 모든 Pack파일의 공백을 제거한다
 */
void __stdcall  VClearBlankAll (VHANDLE hVFS, VCALLBACK_CLEARBLANKALL CallBackProc)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS) 
		pVFS->ClearBlankAll ( CallBackProc );
}


/**********************************************************************************
 * 기준버젼을 가져옴
 */
DWORD __stdcall VGetStdVersion (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetStdVersion ();
	}

	return 0;
}


/**********************************************************************************
 * 기준버젼을 설정함
 */
void __stdcall VSetStdVersion (VHANDLE hVFS, DWORD dwVersion)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->SetStdVersion (dwVersion);
	}
}


/**********************************************************************************
 * 현재버젼을 가져옴
 */
DWORD __stdcall VGetCurVersion (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetCurVersion ();
	}

	return 0;
}


/**********************************************************************************
 * 현재버젼을 설정함
 */
void __stdcall VSetCurVersion (VHANDLE hVFS, DWORD dwVersion)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->SetCurVersion (dwVersion);
	}
}


/*********************************************************************************
 * 인덱스파일내에 있는 Pack파일의 갯수를 알아낸다
 */
int __stdcall VGetVfsCount (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetVFSCount ();
	}

	return 0;
}


/*********************************************************************************
 * 인덱스파일에서 Pack파일 리스트를 가져온다
 * @param ppFiles
 * @param dwNum			가져올 이름의 갯수
 * @param dwMaxPath		저장할 수 있는 최대 길이 (NULL포함)
 * @return				실제 알아낸 파일이름의 갯수
 */
int __stdcall VGetVfsNames (VHANDLE hVFS, char **ppFiles, DWORD dwNum, short dwMaxPath)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetVfsNames (ppFiles, dwNum, dwMaxPath);
	}

	return 0;
}


/*********************************************************************************
 * Vfs파일내에 있는 파일의 갯수를 알아낸다
 */
int __stdcall VGetFileCount (VHANDLE hVFS, const char *VfsName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileCount (VfsName);
	}

	return 0;
}


/*********************************************************************************
 * Vfs파일내에 있는 파일의 갯수를 알아낸다
 */
int __stdcall VGetTotFileCount (VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileTotCount ();
	}

	return 0;
}


/************************************************************************************
 * Pack파일에서 파일이름을 가져온다
 */
int __stdcall VGetFileNames (VHANDLE hVFS, const char *VfsName, char **FileName, int nNum, int nMax)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileNames (VfsName, FileName, nNum, nMax);
	}

	return 0;
}


/// ----------------------- Pack 파일내 파일 작업 관련 ---------------------------


/*********************************************************************************
 * 파일을 오픈한다
 */
VFileHandle * __stdcall VOpenFile (const char * FileName, VHANDLE hVFS)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;

	if(pVFS) 
	{ 
		return pVFS->OpenFile (FileName); 
	}

	return NULL;
}

/*********************************************************************************
 * 파일을 닫는다
 */
void __stdcall VCloseFile (VFileHandle *hVFH)
{
	if(hVFH) 
	{ 
		if(hVFH->btFileType) { fclose (hVFH->fp); }
		
		if (hVFH->hCVFS) {
			CVFS * pVFS = reinterpret_cast<CVFS *>(hVFH->hCVFS);
			pVFS->CloseFile(hVFH); // includes delete pVFH
		}
		else {
	       delete hVFH;
		}
		hVFH = NULL;
	}	
}


/*********************************************************************************
 * 파일의 크기를 리턴 (1)
 */
size_t __stdcall vfgetsize (VFileHandle *pVFH)
{
	if(pVFH) { return (size_t)(pVFH->lEndOff - pVFH->lStartOff); }

	return 0;
}


/*********************************************************************************
 * 파일의 크기를 리턴 (2)
 */
size_t __stdcall VGetFileLength (VHANDLE hVFS, const char *FileName)
{
	/// long lSize = 0;

	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->GetFileLength (FileName);
		/// if(lSize > 0) { return lSize; }
	}

	return 0;
}


/*********************************************************************************
 * 파일이 존재유무를 검사한다
 */
bool __stdcall VFileExists (VHANDLE hVFS, const char *FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->FileExists (FileName);
	}
	return false;
}


/*********************************************************************************
 * Pack파일안에서만 파일이 존재유무를 검사한다
 */
bool __stdcall VFileExistsInVfs (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
	{
		return pVFS->FileExistsInVfs (FileName);
	}

	return false;
}


/*********************************************************************************
 * vfs파일에서 특정파일을 읽는다
 * @param buffer 읽은 데이터를 저장할 버퍼
 * @param size 한 항목의 크기
 * @param count 읽을 항목의 갯수
 */
size_t __stdcall vfread (void *buffer, size_t size, size_t count, VFileHandle *pVFH)
{
	size_t stReadSize	= size * count; /// 읽을 바이트 수
	size_t stReaded		= 0; 			/// 한 바이트씩 읽을었때 실제로 읽힌 바이트수
	size_t stItems		= 0; 			/// 읽혀진 항목수

	long lOffset = pVFH->lStartOff + pVFH->lCurrentOffset;

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	/// 이 파일의 끝은 초과하는지 확인한다
	long lToWhere = pVFH->lStartOff + pVFH->lCurrentOffset + stReadSize;
	if(lToWhere >= (signed)(pVFH->lEndOff))
		stReadSize = pVFH->lEndOff - (pVFH->lStartOff + pVFH->lCurrentOffset);
	if(pVFH->pData) 
	{
		stReaded = sizeof(BYTE)*stReadSize;
		memcpy(buffer, pVFH->pData + pVFH->iAllocOffset + pVFH->lCurrentOffset, stReaded);
	}
	else
	{
		fseek (pVFH->fp, lOffset, SEEK_SET);
		stReaded = fread (buffer, sizeof (BYTE), stReadSize, pVFH->fp);
	}
#ifdef _ENCRYPTION
#define KEYSIZE 0x1000
	long Mpointer;
	// 데이터를 읽는다
	if(pVFH->pData) 
	{
		stReaded = sizeof(BYTE)*stReadSize;
		memcpy(buffer, pVFH->pData + pVFH->iAllocOffset + pVFH->lCurrentOffset, stReaded);
		
		if(pVFH->btEncrypted && UseEncryption==1)
		{
		//	MessageBox( NULL, "decrypting", "TRose", MB_OK | MB_ICONERROR);
// 			const char *hashstring = pVFH->sFileName.c_str();
// 			unsigned long key = StrToHashKey(hashstring);
// 			char crypttable[KEYSIZE];
// 			DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
// 			DWORD EDI=0;
// 			DWORD ECX = key;
// 			DWORD EDX = key+1;
// 			ECX = ECX*4+1;
// 			for(int i = 0; i < 0x400; i++)
// 			{
// 				EDI=ECX*EDX;
// 				*((DWORD*)EAX) = EDI;
// 				EAX++;
// 				ECX+=4;
// 				EDX++;
// 			}

			Mpointer = (pVFH->lCurrentOffset) % KEYSIZE;
			for(size_t k=0;k<stReaded;k++)
			{
				*((char*)buffer+k)^= crypttable[Mpointer];
				Mpointer++; Mpointer %= KEYSIZE;
			}
		}
	}
	else
	{
		fseek (pVFH->fp, lOffset, SEEK_SET);
		stReaded = fread (buffer, sizeof (BYTE), stReadSize, pVFH->fp);
		if (!pVFH->btFileType)
		{
			if(pVFH->btEncrypted && UseEncryption==1)
			{
		//		MessageBox( NULL, "decrypting", "TRose", MB_OK | MB_ICONERROR);
// 				const char *hashstring = pVFH->sFileName.c_str();
// 				unsigned long key = StrToHashKey(hashstring);
// 
// 				char crypttable[KEYSIZE];
// 				DWORD *EAX=reinterpret_cast<DWORD*>(crypttable);
// 				DWORD EDI=0;
// 				DWORD ECX = key;
// 				DWORD EDX = key+1;
// 				ECX = ECX*4+1;
// 				for(int i = 0; i < 0x400; i++)
// 				{
// 					EDI=ECX*EDX;
// 					*((DWORD*)EAX) = EDI;
// 					EAX++;
// 					ECX+=4;
// 					EDX++;
// 				}

				Mpointer = (pVFH->lCurrentOffset) % KEYSIZE;
				for(size_t k=0;k<stReaded;k++)
				{
					*((char*)buffer+k)^= crypttable[Mpointer];
					Mpointer++; Mpointer %= KEYSIZE;
				}
			}
		}
	}
#endif
	/// 읽은 만큼 현재 파일 오프셋을 이동시켜 놓는다
	pVFH->lCurrentOffset += (long)stReaded;
	/// 읽혀진 항목의 갯수 = 읽혀진 바이트수 / 한 항목의 크기
	stItems = stReaded / size;

	return stItems;
}

/*********************************************************************************
 * 파일에 전체 내용을 가리키는 포인터를 얻는다.
 * 얻은 포인터는 직접 해제하면 안됨.
 * @psize 사이즈를 얻기 위해 사용됨.
 * @pVFH 파일 핸들
 * @return 포인터
 */
void * __stdcall vfgetdata (size_t * psize, VFileHandle *pVFH)
{
	long lOffset = pVFH->lStartOff + pVFH->lCurrentOffset;

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	/// 데이터를 읽는다
	if (!pVFS) {
		return NULL;
	}

	if (psize) {
		*psize = pVFH->lEndOff - pVFH->lStartOff;
	}
	return pVFH->pData + pVFH->iAllocOffset;
}

/*********************************************************************************
 * 파일 indicator를 이동시킨다. fseek와 동일하지만 이 파일이 차지하는 범위내에서..
 * @return fseek와 동일
 */
int __stdcall vfseek (VFileHandle *pVFH, long offset, int origin )
{
	int iRet = 0;
	long lOffset = 0;

	/// 이 파일의 범위를 절대로 넘지 못하게 해야 한다.
	switch (origin)
	{
	case VFSEEK_SET:
		/// 새롭게 정한 값이 상한 하한을 넘지 못하도록
		lOffset = pVFH->lStartOff + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;

	case VFSEEK_CUR:
		lOffset = pVFH->lStartOff + pVFH->lCurrentOffset + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;

	case VFSEEK_END:
		lOffset = pVFH->lEndOff + offset;
		if(lOffset < pVFH->lStartOff)	{ lOffset = pVFH->lStartOff; }
		if(lOffset > pVFH->lEndOff)		{ lOffset = pVFH->lEndOff; }
		pVFH->lCurrentOffset = lOffset - pVFH->lStartOff;
		break;
	}

	CVFS_Manager * pVFS = (CVFS_Manager *)pVFH->hVFS;

	if (!pVFS) { // 일반 파일일 경우
		iRet = fseek (pVFH->fp, lOffset, SEEK_SET); /// 실제로 fseek 수행
	}

	return iRet;
}


/*********************************************************************************
 * 파일 indicator값을 리턴한다 ftell과 동일하지만 이 파일이 차지하는 범위내에서..
 */
long __stdcall vftell (VFileHandle *pVFH)
{
	/// 오픈된 파일의 Indicator를 현재위치로 옮긴다
	vfseek (pVFH, 0, VFSEEK_CUR);
	/// 그리고 리턴. 현재 indicator 위치값은 상대위치임
	return pVFH->lCurrentOffset;
}


/*********************************************************************************
 * feof와 동일. 파일의 끝이면 1, 아니면 0을 리턴
 */
int __stdcall vfeof (VFileHandle *pVFH)
{
	return (pVFH->lStartOff + pVFH->lCurrentOffset == pVFH->lEndOff ? 1 : 0);
}


/********************************************************************************
 * FileName에 대한 인덱스 정보와 VFS내의 실제 데이터가 일치하는지 체크한다
 */
short __stdcall VTestFile (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
		return pVFS->TestFile (FileName);

	return VTEST_INVALIDHANDLE;
}

/********************************************************************************
 * FileName에 대한 인덱스 정보와 VFS내의 실제 데이터가 일치하는지 체크한다
 */
DWORD __stdcall ComputeCrc (VHANDLE hVFS, const char * FileName)
{
	CVFS_Manager * pVFS = (CVFS_Manager *)hVFS;
	if(pVFS)
		return pVFS->ComputeCrc (FileName);

	return 0;
}