#pragma once
#include "CallBackX.h"
#include "CEntityMallocObject.h"

class CIntObjServer;
class CCharacterDictator;
class IIntObjCallbackHandlerServer 
	: public virtual CStaticObject
	, public CEntityMallocObject
{
public:
	virtual ~IIntObjCallbackHandlerServer(){}
	IIntObjCallbackHandlerServer(){m_RefByIntObj.SetHolder(this);}
	typedef TPtRefee<IIntObjCallbackHandlerServer,CIntObjServer> RefCallbackHandlerByIntObj;
	RefCallbackHandlerByIntObj& GetRefByIntObj(){return m_RefByIntObj;}
	virtual void OnSetLockObj(uint32 uObjID,uint32 uTargetObjID){
		CALL_CLASS_CALLBACK_2(uObjID, uTargetObjID)
	}
	virtual void OnDestroyObj(uint32 uObjID){
		CALL_CLASS_CALLBACK_1(uObjID)
	}
	virtual void OnIntoTrapViewAoi(uint32 uIntObjID, CIntObjServer* pIntObj, CCharacterDictator *pCharacter){
		CALL_CLASS_CALLBACK_3(uIntObjID, pIntObj, pCharacter)
	}
	virtual void OnLeaveTrapViewAoi(uint32 uIntObjID, CIntObjServer* pIntObj, CCharacterDictator *pCharacter){
		CALL_CLASS_CALLBACK_3(uIntObjID, pIntObj, pCharacter)
	}
	virtual void OnObjAoiMsgFromSelf( uint32 uiObjID, CIntObjServer *pIntObj){
		CALL_CLASS_CALLBACK_2(uiObjID, pIntObj)
	}
	//注册给lua用的回调
private:
	RefCallbackHandlerByIntObj	m_RefByIntObj;
};
