#pragma once
#include "GameDef.h"
#include "HeadBlood.h"
#include "CStaticObject.h"
#include "TBloodRenderAllocator.h"

class CBloodTypeCfg
	:public virtual CStaticObject
{
public:
	static bool LoadCfg();
	static void UnLoadCfg();
	CBloodTypeCfg(){};
	~CBloodTypeCfg();

	static EBloodDialogType GetBloodType(EClass eClass);
private:
	typedef map<EClass, EBloodDialogType, less<EClass>, TBloodRenderAllocator<pair<EClass, EBloodDialogType> > >	MapClassBloodType;
	static MapClassBloodType		ms_mapClassBloodType;
};