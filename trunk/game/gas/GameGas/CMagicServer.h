#pragma once
#include "PatternCOR.h"
#include "ICoreObjectDictatorHandler.h"
#include "GameDef.h"
#include "FightDef.h"
#include "PatternRef.h"
#include "CAoiDimFilterType.h"
#include "TMagicAllocator.h"

class CSkillInstServer;
class CFighterDictator;
struct FilterLimit;
class CAoiDimMgr;

namespace sqr
{
class CCoreObjectDictator;
}

//所有技能魔法的公共基类
class CMagicServer
	: public ICoreObjectDictatorHandler
	, public CPtCORHandler
	, public CMagicMallocObject
{
public:
	CMagicServer(CSkillInstServer* pInst,CFighterDictator* pFighter, uint32 uTargetEntityID);
	virtual ~CMagicServer();

	CSkillInstServer*		GetSkillInst() const	{return m_pSkillInst;}
	CFighterDictator*		GetFighter() const		{return m_From.Get();}
	void SetFighter(CFighterDictator* pFighter);

	CCoreObjectDictator*	GetCoreObj()const;
	uint32					GetGlobalID()const;
	template<typename ImpClass>
	CCoreObjectDictator*	CreateCoreObj(const CFPos& pos);
	void*					GetTag() const			{return (void*)eCOHT_Magic;}
	virtual EMagicGlobalType		GetMagicType() const = 0;
	//CMagicCfgServer*				GetCfg() const	{return m_pCfg;}
	uint32	GetCheckID()const {return m_uCheckID;}

	virtual void DeleteObj(CCoreSceneServer* pScene);
	virtual void OnCOREvent(CPtCORSubject* pSubject,uint32 uEvent,void* pArg){}
	virtual void OnDestroy();
	virtual void OnTransfered(CCoreSceneServer* pOldScene, const CFPos& OldPos){}

	uint32 GetTargetEntityID()const {return m_uTargetEntityID;}
	void SetTargetEntityID(uint32 uEntityID);
	virtual void OnEvent(const void* pData);

protected:
	void SetEyeSight(float fEyeSight);
	void SetBarrierSize(float fBarrierSize);
	template<typename ImpClass>
	void MakeFilterLimit(FilterLimit& filterLimit);

protected:
	CSkillInstServer*		m_pSkillInst;
	EAoiItemSizeType		m_eSizeType;
	EAoiItemEyeSightType	m_eEyeSightType;

private:
	TPtRefer< CMagicServer, CFighterDictator >	m_From;
	uint32		m_uCoreObjID;
	CAoiDimMgr*	m_pAoiDimMgr;
	uint32		m_uTargetEntityID;
	uint32		m_uCheckID;
};
