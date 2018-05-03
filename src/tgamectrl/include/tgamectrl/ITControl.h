#ifndef _ITCONTROL_
#define _ITCONTROL_
#pragma warning( disable : 4251 )
#pragma warning( disable : 4786 )
#include "TGameCtrl.h"
#include "TObject.h"
#include <list>
#include <map>
#include <crtdbg.h>
using namespace std;

class ITControl;

typedef list< ITControl* >				CONTROL_LIST;	
typedef CONTROL_LIST::iterator			CONTROL_LIST_ITOR;

typedef map< int,ITControl* >			CONTROL_MAP;	
typedef CONTROL_MAP::iterator			CONTROL_MAP_ITOR;

//--------------------------------------------------------------------------------------------------------
///	Game control interface class - super parent class
///
/// @Author √÷¡æ¡¯
/// @Date	2005/08/23
//--------------------------------------------------------------------------------------------------------

class TGAMECTRL_API ITControl
{
public:
	ITControl(void);
	virtual ~ITControl(void);

public:
	virtual unsigned int Process( UINT uiMsg,WPARAM wParam,LPARAM lParam ) = 0;
	virtual	void Update( POINT ptMouse ) = 0;
	virtual void Draw() = 0;
};


#endif // _ITCONTROL_	