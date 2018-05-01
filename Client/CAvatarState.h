#ifndef _CAvatarState_
#define _CAvatarState_

class CJustModelAVT;
class CAvatarState
{
public:
	CAvatarState(void);
	virtual ~CAvatarState(void);
	virtual void Enter( CJustModelAVT* pAvt ) = 0;
	virtual void Update( CJustModelAVT* pAvt ) = 0;
	virtual void Leave( CJustModelAVT* pAvt ) = 0;
};

class CAvatarStateNormal : public CAvatarState
{
public:
	CAvatarStateNormal(void){}
	virtual ~CAvatarStateNormal(void){}
	virtual void Enter( CJustModelAVT* pAvt );
	virtual void Update( CJustModelAVT* pAvt ){}
	virtual void Leave( CJustModelAVT* pAvt ){}
};

class CAvatarStateSitdown : public CAvatarState
{
public:
	CAvatarStateSitdown(void){}
	virtual ~CAvatarStateSitdown(void){}
	virtual void Enter( CJustModelAVT* pAvt );
	virtual void Update( CJustModelAVT* pAvt );
	virtual void Leave( CJustModelAVT* pAvt ){}
};

class CAvatarStateSit : public CAvatarState
{
public:
	CAvatarStateSit(void){}
	virtual ~CAvatarStateSit(void){}
	virtual void Enter( CJustModelAVT* pAvt );
	virtual void Update(CJustModelAVT* pAvt){}
	virtual void Leave( CJustModelAVT* pAvt ){}
};

class CAvatarStateStandup : public CAvatarState
{
public:
	CAvatarStateStandup(void){}
	virtual ~CAvatarStateStandup(void){}
	virtual void Enter( CJustModelAVT* pAvt );
	virtual void Update(CJustModelAVT* pAvt);
	virtual void Leave( CJustModelAVT* pAvt ){}
};
#endif