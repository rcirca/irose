/*
	$Header: /7HeartsOnline/Server/Sho_gs/Sho_gs_lib/GS_ObjPOOL.cpp 1     04-03-25 11:20a Icarus $
*/
#include "Lib_gsMAIN.h"
#include "OBJECT.h"
//-------------------------------------------------------------------------------------------------

#define	DEF_MOB_POOL_SIZE		512
#define	INC_MOB_POOL_SIZE		256


IMPLEMENT_INSTANCE( CMobPOOL )

//-------------------------------------------------------------------------------------------------
CMobPOOL::CMobPOOL () : CDataPOOL< CObjMOB > ( "CObjMOB", DEF_MOB_POOL_SIZE, INC_MOB_POOL_SIZE )
{
}
CMobPOOL::~CMobPOOL ()
{
}

//-------------------------------------------------------------------------------------------------
