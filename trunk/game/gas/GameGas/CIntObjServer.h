#pragma once
#include "CEntityServer.h"
#include "ICoreObjectDictatorHandler.h"

namespace sqr
{
	class ICoreObjectServerHandler;
	class CCoreObjectDictator;
	class CCoreSceneServer;
	class CCoreObjectServer;
	class CSyncVariantServer;
}

class CIntObjServer;
class IIntObjCallbackHandlerServer;
class CActorIntObj;
class CFighterDictator;
class CEntityServer;
class CCharacterDictator;

class IIntObjServerHandler : public ICoreObjectDictatorHandler
{
public:
	void OnAoiMsgFromSelf(const void* pContext);
	virtual void* GetTag() const { return (void*)eCOHT_Obj; }
};

class CIntObjServer : public CEntityServer
{
	friend class CEntityServerCreator;
	typedef set<string> SetNpcName;
	typedef SetNpcName::iterator SetNameIter;
public:
	CIntObjServer(CCoreObjectDictator *pCoreObj);
	virtual ~CIntObjServer();
	static CIntObjServer* GetIntObjServer(CCoreObjectServer*);
	static CIntObjServer* GetIntObjServerByID(uint32 uEntityID);

	virtual EGameEntityType GetGameEntityType()const { return eGET_IntObject; }
	virtual CIntObjServer* CastToCharacterMediator() { return NULL; }

	IIntObjCallbackHandlerServer* GetCallbackHandler() const;
	void SetCallbackHandler( IIntObjCallbackHandlerServer* pCallbackHandler);

	void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);

	virtual void Release();

	void SetOwner(CCharacterDictator* pMaster);
	CCharacterDictator*	GetOwner();

	CActorIntObj* GetActor() const;
	void SetAndSyncActionDir(uint8 uDir);
	uint8 GetActionDir() const;

	void SetObjName(const TCHAR* sName);
	const string& GetObjName() { return m_sObjName;}

	double GetStateZoomRate();
	void OnAoiMsgFromSelf();
	void DoPosIntObjSkill(const TCHAR* sName, uint32 uLevel,const CFPos& Pos);
	void DoIntObjSkill(const TCHAR* sName, uint32 uLevel,CEntityServer* pTarget);
	void SetIntObjSeeType(ETrapSeeType eSeeType);
	void InsertNpcSet(const char* sNpcName);
	void OnSetNpcArgEnd();
private:
	void CreateFighter();
private:
	string					m_sObjName;
	ETrapSeeType			m_eTrapSeeType;
	bool					m_bSeeAllNpc;
	TPtRefer<CIntObjServer, IIntObjCallbackHandlerServer>	m_CallbackHandler;
	TPtRefer<CIntObjServer, CCharacterDictator>				m_Owner;
	SetNpcName												m_setNpcName;
	CSyncVariantServer*										m_pViewVariantAttribute_Type;
	CSyncVariantServer*										m_pViewVariantAttribute_EntityID;
};

