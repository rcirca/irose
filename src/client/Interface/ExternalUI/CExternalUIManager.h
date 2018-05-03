#ifndef _EXTERNALUIMANAGER_
#define _EXTERNALUIMANAGER_

#include "CExternalUI.h"
#include "CLogin.h"
#include "WarningDlg.h"
#include "CSelectAvata.h"
#include "CSelectServer.h"
#include "CCreateAvata.h"

///
/// Managing all external UI class
/// Include LOGIN, SELECT SERVER, SELECT AVATA
/// !! This class is observer class for all EUIS.
///
class CTDialog;
class CExternalUIManager
{
public:

private:	

	CTDialog*		m_ExternalUI[ EUI_MAX ];	

public:
	CExternalUIManager();
	~CExternalUIManager();

	///
	/// add External UI in EUI manager
	///
	bool	AddEUI( unsigned int iType, CTDialog* pEUI );

	///
	/// get External UI from EUI manager
	///
	CTDialog*	GetEUI( int iType );


	///
	/// InitAll EUIs
	///
	bool	InitExternalUIs();
};


#endif //_EXTERNALUIMANAGER_