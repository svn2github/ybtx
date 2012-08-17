#pragma once
#include "CFighterCalInfo.h"
#include "TPropertyHolder.h"
#include "CFighterBaseInfo.h"
#include "CFighterMallocObject.h"

class CFighterViewInfo
	:public CFighterBaseInfo
	,public CFighterCalInfo
	,public TPropertyHolder<CFighterViewInfo>
	,public CFighterMallocObject
{
public:
	CFighterViewInfo(void);
	~CFighterViewInfo(void);

	void CppInit(uint32 uGlobalID);
};
