#ifndef _CSelectChannel_
#define _CSelectChannel_
#include "tgamectrl/tdialog.h"
class CExternalUIManager;
struct tagCHANNEL_SRV;
class CSelectChannel :	public CTDialog
{
public:
	CSelectChannel(void);
	virtual ~CSelectChannel(void);

	virtual unsigned Process( UINT uiMsg,WPARAM wParam,LPARAM lParam );
	virtual void Show();

	void SetEUIObserver( CExternalUIManager* pObserver );


private:
};
#endif