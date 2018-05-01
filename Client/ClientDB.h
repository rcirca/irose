#ifndef _CClientDB_
#define _CClientDB_

interface IDBInitialize;
interface ICommandText;

struct S_InventoryData{
	long		lRealIndex;
	long		lVirtualIndex;
};

class CClientDB
{
	CClientDB(void);
	~CClientDB(void);
public:
	static CClientDB& GetInstance();
	bool Initialize();
	bool Connect();
	bool Disconnect();
	bool UnInitialize();
	
	bool CreateInventoryTable();
	void GetInventoryData( const char* pszName, std::list<S_InventoryData>& Inventory );
	void SetInventoryData( const char* pszName, std::list<S_InventoryData>& Inventory );

private:
	bool			CheckError( HRESULT hr );
	bool			CheckErrorGetNextRows( HRESULT hr );
	bool			CheckErrorGetData( HRESULT hr );
	ICommandText*	CreateCommand( IDBInitialize* pIDBInitialize ,LPCWSTR wCmdString );



	bool			UpdateInventoryData( const char* pszName, S_InventoryData Data );
	bool			InsertInventoryData( const char* pszName, S_InventoryData Data ); 
private:
	IDBInitialize*	m_pIDBInitialize;
};
#endif