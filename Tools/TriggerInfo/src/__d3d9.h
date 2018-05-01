#ifndef __D3D9_H__
#define __D3D9_H__

#include "stdafx.h"
#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#include <map>
#include <string>
#include <vector>
#include <dxdiag.h>
#include "dispinfo.h"
#include "systeminfo.h"
using namespace std;

class CD3D9
{
private:
	/// Direct3D Object
	static IDirect3D9 *			s_pID3D;
	
	/// 에러메시지를 저장할 스트링변수
	std::string					m_strErr;
	
private:
	/// 에러를 조사한다
	bool __CheckError (HRESULT hr);
	string ConvertDriverVersion (D3DADAPTER_IDENTIFIER9 *);

public :
	/// 디폴트 생성자
	CD3D9 ();
	~CD3D9();
	/// 다이렉트X를 초기화하고 Device를 생성함
	BOOL Init (void);


	/// 생성한 오브젝트를 모두 해제함
	void Release (void);	
	void DestroyDisplayInfo( vector<DisplayInfo*>& vDisplayInfo );


	bool GetDisplayInfo (map<string, string> * DisInfo);
	IDirect3D9 * GetDevice (void) { return s_pID3D; }


    vector<DisplayInfo*>        m_vDisplayInfo;


	BOOL              m_bCleanupCOM;
	IDxDiagProvider*  m_pDxDiagProvider;
    IDxDiagContainer* m_pDxDiagRoot;

	/// 시스템 정보.
	HRESULT CD3D9::GetSystemInfo( SystemInformation** ppSysInfo );

	/// 디스플레이 정보.
	HRESULT GetDisplayInfo( vector<DisplayInfo*>& vDisplayInfo );

	/// 정보 변환
	HRESULT GetIntValue( IDxDiagContainer* pObject, WCHAR* wstrName, LONG* pnValue );
	HRESULT GetBoolValue( IDxDiagContainer* pObject, WCHAR* wstrName, BOOL* pbValue );
	HRESULT GetInt64Value( IDxDiagContainer* pObject, WCHAR* wstrName, ULONGLONG* pullValue );
	HRESULT GetStringValue( IDxDiagContainer* pObject, WCHAR* wstrName, TCHAR* strValue, int nStrLen );
	HRESULT GetUIntValue( IDxDiagContainer* pObject, WCHAR* wstrName, DWORD* pdwValue );
	HRESULT GatherDXVA_DeinterlaceCaps( IDxDiagContainer* pParent, vector<DxDiag_DXVA_DeinterlaceCaps*>& vDXVACaps );
	
};

extern CD3D9 g_D3D;


#endif