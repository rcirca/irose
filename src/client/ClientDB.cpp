#include "stdafx.h"
#include ".\clientdb.h"

#include <objbase.h>

#define DBINITCONSTANTS
#define INITGUID

#include <initguid.h>
#include <oledb.h>
#include "msjetoledb.h"
//#include "jetoledb.h"	// for IJetCompact interface
#include <atldbcli.h>


CClientDB::CClientDB(void)
{
	m_pIDBInitialize = NULL;
}

CClientDB::~CClientDB(void)
{
}

CClientDB& CClientDB::GetInstance()
{
	static CClientDB s_Instance;
	return s_Instance;
}
bool CClientDB::Initialize()
{
	CoInitialize( NULL );
	return true;
}

bool CClientDB::Connect()
{
	static IDBProperties*      pIDBProperties	= NULL;
	
	const ULONG         nPropSet = 1;
	const ULONG         nInitProps = 1;

	static DBPROP       InitProperties[nInitProps];
	static DBPROPSET    rgInitPropSet[nPropSet];


    HRESULT hr = CoCreateInstance(
					CLSID_JETOLEDB_4_00,
                    NULL, 
                    CLSCTX_INPROC_SERVER,
                    IID_IDBInitialize, 
                    (void **) &m_pIDBInitialize);


    if (FAILED(hr))
		return CheckError( hr );

    /*
    Initialize the property values needed
    to establish the connection.
    */
    for(int i = 0; i < nInitProps; i++)
        VariantInit(&InitProperties[i].vValue);

    //Specify server name.
    InitProperties[0].dwPropertyID = DBPROP_INIT_DATASOURCE;
    InitProperties[0].vValue.vt = VT_BSTR;
	InitProperties[0].vValue.bstrVal =  SysAllocString(L"ROSE.MDB");
	InitProperties[0].dwOptions = DBPROPOPTIONS_REQUIRED;
    InitProperties[0].colid = DB_NULLID;

   
	rgInitPropSet[0].guidPropertySet = DBPROPSET_DBINIT;
    rgInitPropSet[0].cProperties = 1;
    rgInitPropSet[0].rgProperties = InitProperties;

    //Set initialization properties.
    hr = m_pIDBInitialize->QueryInterface(
                                IID_IDBProperties, 
                                (void **)&pIDBProperties);
    if (FAILED(hr))
		return CheckError( hr );

    hr = pIDBProperties->SetProperties(
                                nPropSet, 
                                rgInitPropSet);
    if(FAILED(hr))
		return CheckError( hr );


    pIDBProperties->Release();

    if(FAILED(hr = m_pIDBInitialize->Initialize()))
		return CheckError( hr );

	return true;
}
bool CClientDB::Disconnect()
{

//    IJetCompact* pIJetCompact; 

//    HRESULT hr = m_pIDBInitialize->QueryInterface(
//                                IID_IJetCompact, 
  //                              (void **)&pIJetCompact);

//	if( hr == S_OK )
//	{
//		pIJetCompact->Compact( 0, 0 );
//		pIJetCompact->Release();
//	}

	if( m_pIDBInitialize )
	{
		if(FAILED(m_pIDBInitialize->Uninitialize()))
			assert( 0 && "Fail Disconnect ClientDB");

		m_pIDBInitialize->Release();
	}
	return true;
}

bool CClientDB::UnInitialize()
{
	CoUninitialize();
	return true;
}

bool CClientDB::CheckError( HRESULT hr )
{
	switch( hr )
	{
	case S_OK :
		return true;
	case DB_S_ASYNCHRONOUS :
		assert( 0 && "DB_S_ASYNCHRONOUS" );
		return false;
	case DB_S_ERRORSOCCURRED : 
		assert( 0 && "DB_S_ERRORSOCCURRED" );
		return false;
	case E_FAIL :
		assert( 0 && "E_FAIL" );
		return false;
	case E_OUTOFMEMORY :
		assert( 0 && "E_OUTOFMEMORY" );
		return false;
	case E_UNEXPECTED :
		assert( 0 && "E_UNEXPECTED" );
		return false;
	case DB_E_ALREADYINITIALIZED :
		assert( 0 && "DB_E_ALREADYINITIALIZED" );
		return false;
	case DB_E_CANCELED :
		assert( 0 && "DB_E_ALREADYINITIALIZED" );
		return false;
	case DB_E_ERRORSOCCURRED :
		assert( 0 && "DB_E_ERRORSOCCURRED" );
		return false;
	case DB_SEC_E_AUTH_FAILED :
		assert( 0 && "DB_SEC_E_AUTH_FAILED" );
		return false;
	default:
		assert( 0 && "DEFAULT FAILED" );
		return false;
	}
	return false;
}

ICommandText* CClientDB::CreateCommand( IDBInitialize* pIDBInitialize ,LPCWSTR wCmdString )
{
 	IDBCreateSession*   pIDBCreateSession       = NULL;
	IDBCreateCommand*   pIDBCreateCommand       = NULL;
	ICommandText*		pICommandText			= NULL;

	if( pIDBInitialize == NULL || wCmdString == NULL )
	{
		return NULL;
	}

	//Create a new activity from the data source object.
    if(FAILED(pIDBInitialize->QueryInterface( IID_IDBCreateSession, (void**) &pIDBCreateSession)))
    {
		return NULL;
    }

    if(FAILED(pIDBCreateSession->CreateSession( NULL, IID_IDBCreateCommand, (IUnknown**) &pIDBCreateCommand)))
    {
		return NULL;
    }
    pIDBCreateSession->Release();    

    //Create a Command object.
    if(FAILED(pIDBCreateCommand->CreateCommand( NULL, IID_ICommandText, (IUnknown**) &pICommandText)))
    {
        pIDBCreateCommand->Release();
		return NULL;
    }
    pIDBCreateCommand->Release();
  
    //Set the command text.
    if(FAILED(pICommandText->SetCommandText(DBGUID_DBSQL, wCmdString )))
    {
		pICommandText->Release();
		return NULL;
    }
	return pICommandText;
}

//*----------------------------------------------------------------------------
/// 매번 새로 만들려고 시도한다.. 이미 만들어져 있다면 Fail
//*----------------------------------------------------------------------------
bool CClientDB::CreateInventoryTable()
{
	ICommandText*       pICommandText           = NULL;
	HRESULT             hr;
	LONG                cNumRows = 0;

	WCHAR wCmdString[] = L"CREATE TABLE INVENTORY ( name TEXT(30), realindex long , virtualindex long )";
	
	pICommandText = CreateCommand ( m_pIDBInitialize ,wCmdString 	);

	if( pICommandText == NULL )
		return false;

    if(FAILED(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_NULL, 
                                    NULL, 
                                    &cNumRows, 
                                    NULL)))
    {
		LogString( LOG_NORMAL,"Fail CreateInventoryTable\n" );
	}
	pICommandText->Release();
	return true;
}

void CClientDB::GetInventoryData( const char* pszName, std::list<S_InventoryData>& Inventory)
{
	ICommandText*       pICommandText           = NULL;
	IRowset*            pIRowset                = NULL;
	HRESULT             hr;

	ULONG               cbColOffset = 0;
	LONG                cNumRows = 0;
	LONG				cRowsAffected = 0;
	int					iCount = 0;

	unsigned long		cRowsObtained;
	unsigned long		cCount;
	HACCESSOR			hAccessor;
	IAccessor *			pIAccessor = NULL;

	WCHAR wCmdString[1024];
	wsprintfW(wCmdString, L"SELECT realindex,virtualindex FROM INVENTORY WHERE name = '%S'", pszName );

	pICommandText = CreateCommand( m_pIDBInitialize ,wCmdString );
	if( pICommandText == NULL )
	{
		assert( 0 && "Fail GetInventoryData");
		return;
	}

   
    if(FAILED(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_IRowset, 
                                    NULL, 
                                    &cNumRows, 
                                    (IUnknown **) &pIRowset)))
    {
		assert( 0 && "Fail GetInventoryData : ICommandText->Execute");
	    pICommandText->Release();
		return;
    }

	if( FAILED( pIRowset->QueryInterface( IID_IAccessor, (void **) &pIAccessor)))
    {
		assert( 0 && "Fail IRowset->QueryInterface"	);
		pIRowset->Release();
		pICommandText->Release();
		return;
    }
	
	const int       nParams = 2;
	S_InventoryData Data;
	memset( &Data, 0, sizeof( S_InventoryData ));
	
	static DBBINDING ExactBindings [nParams] = {
      {
         1,                          // iOrdinal
         offsetof (S_InventoryData, lRealIndex), // obValue
         0,                          // No length binding
         0,                          // No Status binding
         NULL,                       // No TypeInfo
         NULL,                       // No Object
         NULL,                       // No Extensions
         DBPART_VALUE,
         DBMEMOWNER_CLIENTOWNED,   // Ignored
         DBPARAMIO_NOTPARAM,
         sizeof (long),
         0,
         DBTYPE_I4,
         0,                          // No Precision
         0                           // No Scale
      },
      {
         2,                          // iOrdinal
         offsetof (S_InventoryData, lVirtualIndex ),   // obValue
         0,                          // No length binding
         0,                          // No Status binding
         NULL,                       // No TypeInfo
         NULL,                       // No Object
         NULL,                       // No Extensions
         DBPART_VALUE,
         DBMEMOWNER_CLIENTOWNED,    // Ignored
         DBPARAMIO_NOTPARAM,
		 sizeof(long),
         0,
         DBTYPE_I4,
         0,                          // No Precision
         0                           // No Scale
      },
	};

    if( FAILED( hr = pIAccessor->CreateAccessor( 
				DBACCESSOR_ROWDATA, 
                                nParams, 
                                ExactBindings, 
                                0, 
                                &hAccessor, 
                                NULL)))
    {
		pIAccessor->Release();
		pIRowset->Release();
		pICommandText->Release();
		assert( 0 && "Fail CreateAccessor");
		return;
        // Handle errors here.
    }

    HROW*	pRows = new HROW[256];
    while( TRUE )
    {
        if( FAILED( hr = pIRowset->GetNextRows( NULL, 0, 256, &cRowsObtained, &pRows ) ) )
        {
			CheckErrorGetNextRows( hr );
            break;
			// Handle errors here.
        }

        // Make sure some rows were obtained.
        if( cRowsObtained == 0 )
        {
			LogString(LOG_NORMAL,"ROW IS 0");
            break;
        }

        // Get the data for the each of the rows.
        for( cCount = 0; cCount < cRowsObtained; cCount++ )
        {
            // Get the row data needed.
			ZeroMemory( &Data, sizeof( Data ));
            if( FAILED( hr = pIRowset->GetData( pRows[cCount], hAccessor,  (void*)&Data )))
                CheckErrorGetData( hr );
			else
				Inventory.push_back( Data );

	    }
        // Release the rows.
        if( FAILED( hr = pIRowset->ReleaseRows( 
                                    cRowsObtained, 
                                    pRows, 
                                    NULL, 
                                    NULL, 
                                    NULL )))
        {
            // Handle errors here.
        }
		memset( &Data, 0 , sizeof( S_InventoryData ));
    }

    if( FAILED( hr = pIAccessor->ReleaseAccessor( hAccessor, NULL )))
    {
        // Handle errors here.
    }

    // Release the IAccessor object.
	if( pRows )
		delete []pRows;

    pIAccessor->Release();
	pIRowset->Release();
	pICommandText->Release();
}

//*----------------------------------------------------------------------------
/// Update를 하고 실패하면 Insert한다.
//*----------------------------------------------------------------------------
void CClientDB::SetInventoryData( const char* pszName, std::list<S_InventoryData>& Inventory )
{
	std::list<S_InventoryData>::iterator iter;
	for( iter = Inventory.begin(); iter != Inventory.end(); ++iter )
	{
		if( !UpdateInventoryData( pszName, *iter ) )
		{
			if( !InsertInventoryData( pszName, *iter ))
			{
				assert( 0 && "Fail SetInventoryData");
			}
		}
	}
}

bool CClientDB::UpdateInventoryData( const char* pszName, S_InventoryData Data )
{
	ICommandText*       pICommandText           = NULL;
	HRESULT             hr;
	LONG                cNumRows = 0;

	WCHAR wCmdString[1024];

	wsprintfW(wCmdString, L"UPDATE INVENTORY SET virtualindex = %d WHERE name = '%S' AND realindex = %d",Data.lVirtualIndex , pszName ,Data.lRealIndex );

	pICommandText = CreateCommand ( m_pIDBInitialize ,wCmdString 	);

	if( pICommandText == NULL )
		return false;

    if(FAILED(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_NULL, 
                                    NULL, 
                                    &cNumRows, 
                                    NULL)))
    {
		LogString( LOG_NORMAL,"Fail CreateInventoryTable\n" );
		pICommandText->Release();
		return false;
	}

	pICommandText->Release();
	
	if( cNumRows == 0 )
		return false;

	return true;
}

bool CClientDB::InsertInventoryData( const char* pszName, S_InventoryData Data )
{
	ICommandText*       pICommandText           = NULL;
	HRESULT             hr;
	LONG                cNumRows = 0;

	WCHAR wCmdString[1024];

	wsprintfW(wCmdString, L"INSERT INTO INVENTORY ( name, realindex, virtualindex ) VALUES ( '%S', %d, %d )", pszName, Data.lRealIndex, Data.lVirtualIndex );

	pICommandText = CreateCommand ( m_pIDBInitialize ,wCmdString 	);

	if( pICommandText == NULL )
		return false;

    if(FAILED(hr = pICommandText->Execute(
                                    NULL, 
                                    IID_NULL, 
                                    NULL, 
                                    &cNumRows, 
                                    NULL)))
    {
		LogString( LOG_NORMAL,"Fail CreateInventoryTable\n" );
	}
	pICommandText->Release();
	return true;
}

bool CClientDB::CheckErrorGetNextRows( HRESULT hr )
{
	switch( hr )
	{
	case S_OK:
		return true;
	case DB_S_ENDOFROWSET:
		assert( 0 && "DB_S_ENDOFROWSET" );
		break;
	case DB_S_ROWLIMITEXCEEDED:
		assert( 0 && "DB_S_ROWLIMITEXCEEDED" );
		break;
	case DB_S_STOPLIMITREACHED:
		assert( 0 && "DB_S_STOPLIMITREACHED" );
		break;
	case E_FAIL:
		assert( 0 && "E_FAIL" );
		break;
	case E_INVALIDARG:
		assert( 0 && "E_INVALIDARG" );
		break;
	case E_OUTOFMEMORY:
		assert( 0 && "E_OUTOFMEMORY");
		break;
	case E_UNEXPECTED:
		assert( 0 && "E_UNEXPECTED" );
		break;
	case DB_E_BADCHAPTER:
		assert( 0 && "DB_E_BADCHAPTER" );
		break;
	case DB_E_BADSTARTPOSITION:
		assert( 0 && "DB_E_BADSTARTPOSITION" );
		break;
	case DB_E_CANCELED:
		assert( 0 && "DB_E_CANCELED" );
		break;
	case DB_E_CANTFETCHBACKWARDS:
		assert( 0 && "DB_E_CANTFETCHBACKWARDS" );
		break;
	case DB_E_CANTSCROLLBACKWARDS:
		assert( 0 && "DB_E_CANTSCROLLBACKWARDS" );
		break;
	case DB_E_NOTREENTRANT:
		assert( 0 && "DB_E_NOTREENTRANT" );
		break;
	case DB_E_ROWSNOTRELEASED:
		assert( 0 && "DB_E_ROWSNOTRELEASED" );
		break; 
	case DB_SEC_E_PERMISSIONDENIED:
		assert( 0 && "DB_SEC_E_PERMISSIONDENIED" );
		break;
	default:
		assert( 0 && "Default Fail" );
		return false;
	}
	return false;
}
bool CClientDB::CheckErrorGetData( HRESULT hr )
{
	switch( hr )
	{
	case S_OK:
		return true;
		break;
	case DB_S_ERRORSOCCURRED:
		assert( 0 && "DB_S_ERRORSOCCURRED" );
		break;
	case E_FAIL:
		assert( 0 && "E_FAIL" );
		break;
	case E_INVALIDARG:
		assert( 0 && "E_INVALIDARG" );
		break;
	case E_UNEXPECTED:
		assert( 0 && "E_UNEXPECTED" );
		break;
	case DB_E_BADACCESSORHANDLE:
		assert( 0 && "DB_E_BADACCESSORHANDLE" );
		break;
	case DB_E_BADACCESSORTYPE:
		assert( 0 && "DB_E_BADACCESSORTYPE" );
		break;
	case DB_E_BADROWHANDLE:
		assert( 0 && "DB_E_BADROWHANDLE" );
		break;
	case DB_E_DELETEDROW:
		assert( 0 && "DB_E_DELETEDROW" );
		break;
	case DB_E_ERRORSOCCURRED:
		assert( 0 && "DB_E_ERRORSOCCURRED" );
		break;
	default:
		assert( 0 && "Fail Default");
		break;
	}
	return false;
}