#pragma once
#include "CCharacterVariantHandler.h"
#include "CEntityMallocObject.h"

class CEntityClient;

class CEntityVariantCallback : public CVariantCallback, public CEntityMallocObject
{
public:
	CEntityVariantCallback(CEntityClient* pEntity):m_pEntity(pEntity){}
	virtual ~CEntityVariantCallback() {m_pEntity = NULL;}
protected:
	CEntityClient* m_pEntity;
};

class CEntityBottomSizeVariantCallback : public CEntityVariantCallback
{
public:
	CEntityBottomSizeVariantCallback(CEntityClient* pEntity):CEntityVariantCallback(pEntity){}
	~CEntityBottomSizeVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};
