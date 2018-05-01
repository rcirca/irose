#ifndef _CTEventStoreUnion_
#define _CTEventStoreUnion_
#include "ctevent.h"

class CTEventStoreUnion : public CTEvent
{
public:
	CTEventStoreUnion(void);
	virtual ~CTEventStoreUnion(void);
};
#endif