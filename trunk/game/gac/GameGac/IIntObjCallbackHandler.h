#pragma once
#include "CallBackX.h"
#include "CEntityMallocObject.h"
#include "CStaticObject.h"

class CIntObjClient;

// CallbackHandler上面不能写纯虚的函数，并且不能注册成Abstract
class IIntObjCallbackHandler 
	: public virtual CStaticObject
	, public CEntityMallocObject
{
public:
	virtual void OnCreated(CIntObjClient *pObject) { CALL_CLASS_CALLBACK_1(pObject) };
	virtual void OnDestroy(CIntObjClient *pObject) { CALL_CLASS_CALLBACK_1(pObject) };
	virtual void OnIntoSyncAoi(CIntObjClient *pObject) { CALL_CLASS_CALLBACK_1(pObject) };
	virtual void OnLeaveSyncAoi(CIntObjClient *pObject) { CALL_CLASS_CALLBACK_1(pObject) };
	virtual void OnObjDoActionEnd(CIntObjClient *pObject) { CALL_CLASS_CALLBACK_1(pObject) };
	virtual void OnAnimationEnd(CIntObjClient *pObject,const TCHAR* szName) { CALL_CLASS_CALLBACK_2(pObject,szName) };
};
