#pragma once
#include "CMagicServer.h"
#include "CDistortedTick.h"
#include "TMagicAllocator.h"

class CBarrierMagicCfgServerSharedPtr;

class CBarrierMagicServer 
	: public CMagicServer
	, public CDistortedTick
{
public:
	CBarrierMagicServer(CSkillInstServer* pSkillInst, const CBarrierMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& pos);
	virtual ~CBarrierMagicServer();

	virtual EMagicGlobalType GetMagicType() const{ return eMGT_Barrier; }
	virtual void OnTick();
	virtual void OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension);
	virtual void OnDestroy();
	bool Create();

	CBarrierMagicCfgServerSharedPtr&	GetCfgSharedPtr()const;
private:
	typedef vector<CPos, TMagicAllocator<CPos> > VecPos_t;
	typedef vector<CCoreObjectDictator*, TMagicAllocator<CCoreObjectDictator*> > VecObj_t;
	//typedef set<uint32> SetInSightCoreObjOfMagic;

	VecPos_t	m_vecBarrierPos;	// 每个Pos上都会创建一个CoreObject
	VecObj_t	m_vecObj;
	//SetInSightCoreObjOfMagic	m_setInSightCoreObjOfMagic;

	CCoreSceneServer* m_pScene;
	ECamp	m_eCamp;			//因为施法者有可能已经不在了，所以需要记录阵营
	CBarrierMagicCfgServerSharedPtr*	m_pCfg;
};
