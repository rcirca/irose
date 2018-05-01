/*
	$Header: /7HeartsOnline/Server/Sho_gs/Sho_gs_lib/GS_ObjPOOL.h 1     04-03-25 11:20a Icarus $
*/
#ifndef	__GS_OBJPOOL_H
#define	__GS_OBJPOOL_H
#include "CDataPOOL.h"
//-------------------------------------------------------------------------------------------------

class CMobPOOL : private CDataPOOL< CObjMOB >
{
	DECLARE_INSTANCE( CMobPOOL )

private:
	CMobPOOL ();
	~CMobPOOL ();

public :
} ;

//-------------------------------------------------------------------------------------------------
#endif