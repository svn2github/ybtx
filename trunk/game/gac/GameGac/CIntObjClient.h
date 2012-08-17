#pragma once
#include "CEntityClient.h"

namespace sqr
{
	class CCoreObjectFollower;
}

class IIntObjCallbackHandler;
class CActorIntObjClient;
class CIntObjClientVariantHandler;

class CIntObjClient : public CEntityClient
{
public:
	CIntObjClient(CCoreObjectFollower* pCoreObj, uint32 uID);
	virtual ~CIntObjClient();
	static CIntObjClient* GetIntObj(CCoreObjectFollower*);
	static CIntObjClient* GetIntObjByID(uint32 uEntityID);
	virtual EGameEntityType GetGameEntityType()const { return eGET_IntObject; }	
	CActorIntObjClient* CIntObjClientGetActor()const; 
	void SetCallbackHandler(IIntObjCallbackHandler* pCallbackHandler );
	inline bool CheckSyncDataReceived() const { return m_bSyncDataReceived; }
	inline void SetSyncDataReceived(bool bRe) { m_bSyncDataReceived = bRe; }
	IIntObjCallbackHandler* GetCallbackHandler() const { return m_pCallbackHandler; }
	void SetIntObjRenderType();
	virtual void OnDestroy();
	virtual void OnRenderObjDestroyed();
	virtual void OnPosChanged();
	virtual void OnRefreshRenderObjPos(const CFPos& PixelPos);
	virtual void OnEnteredSyncAoi();
	virtual void OnLeftedSyncAoi();
	void OnBeSelected();
	CActorIntObjClient* GetActor() const;
	void CIntObjDoActor(const TCHAR* cActorName);
private:
	IIntObjCallbackHandler* m_pCallbackHandler;
	bool m_bSyncDataReceived;
	CIntObjClientVariantHandler* m_pBottomSizeVariantHandler;
	
};

