#pragma once
#include "CCharacterVariantHandler.h"
#include "CFighterMallocObject.h"

class CFighterFollower;

class CFighterVariantCallback : public CVariantCallback, public CFighterMallocObject
{
public:
	CFighterVariantCallback(CFighterFollower* pFollower):m_pFighter(pFollower){}
	virtual ~CFighterVariantCallback() {m_pFighter = NULL;}
protected:
	CFighterFollower* m_pFighter;
};


class CFighterHealthPointVariantCallback 
	:public CFighterVariantCallback
{
public:
	CFighterHealthPointVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterHealthPointVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterConsumePointVariantCallback 
	:public CFighterVariantCallback
{
public:
	CFighterConsumePointVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterConsumePointVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterCastingBeginVariantCallback 
	:public CFighterVariantCallback
{
public:
	CFighterCastingBeginVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterCastingBeginVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterCastingEndVariantCallback 
	:public CFighterVariantCallback
{
public:
	CFighterCastingEndVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterCastingEndVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterSetStateVariantCallback
	:public CFighterVariantCallback
{
public:
	CFighterSetStateVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterSetStateVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterDelStateVariantCallback
	:public CFighterVariantCallback
{
public:
	CFighterDelStateVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterDelStateVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterClearAllStateVariantCallback
	:public CFighterVariantCallback
{
public:
	CFighterClearAllStateVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterClearAllStateVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};

class CFighterCtrlValueVariantCallback
	:public CFighterVariantCallback
{
public:
	CFighterCtrlValueVariantCallback(CFighterFollower* pFighter):CFighterVariantCallback(pFighter){}
	~CFighterCtrlValueVariantCallback(){};
	virtual int exec(CCypherVariant *pVariant);
};
