#ifndef _iconcommand_
#define _iconcommand_
#include "tgamectrl/tcommand.h"
//class CTCmdItemIconInBuyList : public CTCommand
//{
//public:
//	CTCmdItemIconInBuyList(void){}
//	virtual ~CTCmdItemIconInBuyList(void){}
//	virtual bool Exec( CTObject* pObj );
//};
//
//class CTCmdItemIconInSellList : public CTCommand
//{
//public:
//	CTCmdItemIconInSellList(void){}
//	virtual ~CTCmdItemIconInSellList(void){}
//	virtual bool Exec( CTObject* pObj );
//};

class CTCmdRemoveItemInBuyList : public CTCommand
{
public:
	CTCmdRemoveItemInBuyList(void){}
	virtual ~CTCmdRemoveItemInBuyList(){}
	virtual bool Exec( CTObject* pObj );
};

class CTCmdRemoveItemInSellList : public CTCommand
{
public:
	CTCmdRemoveItemInSellList(void){}
	virtual ~CTCmdRemoveItemInSellList(void){}
	virtual bool Exec( CTObject* pObj );
};
#endif