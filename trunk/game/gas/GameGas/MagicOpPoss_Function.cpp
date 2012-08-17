#include "stdafx.h"
#include "MagicOpPoss_Function.h"
#include "CShockWaveMagicServer.h"
#include "CShockWaveMagicCfgServer.h"
#include "CPositionMagicServer.h"
#include "CPositionMagicCfgServer.h"
#include "CBattleArrayMagicServer.h"
#include "CBattleArrayMagicCfgServer.h"
#include "CFighterDictator.h"
#include "CMagicMgrServer.h"
#include "CMoveMagicServer.h"
#include "CMoveMagicCfgServer.h"
#include "CBulletMagicServer.h"
#include "CBulletMagicCfgServer.h"
#include "CTotemMagicCfgServer.h"
#include "CCfgCalc.h"
#include "CRelationMgrServer.h"
#include "CCoreSceneServer.h"
#include "CMetaSceneServer.h"
#include "CCharacterDictator.h"
#include "CFighterMediator.h"
#include "ICharacterMediatorCallbackHandler.h"
#include "CPixelPath.h"
#include "CoreCommon.h"
#include "ErrLogHelper.h"
#include "CBarrierMagicServer.h"
#include "CBarrierMagicCfgServer.h"
#include "PatternRef.inl"
#include "Random.h"
#include "CCfgColChecker.h"
#include "CSkillInstServer.h"
#include "CCoreObjectServer.h"
#include "TSqrAllocator.inl"
#include "BaseHelper.h"
#include "CMagicOpCfgArg.h"
#include "CQuickRand.h"
#include "CDelayMagicOpTick.h"

template class TPtRefer<CMagicServer,CFighterDictator>;

uint32 CRelShockWaveMagicMOP::Do(CSkillInstServer* pSkillInst,const CCfgArg* pArg,CFighterDictator* pFrom,const CFPos& pos)
{	
	CShockWaveMagicCfgArg* pShockWaveMagicCfg = static_cast<CShockWaveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CShockWaveMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
	new CShockWaveMagicServer(pSkillInst,pCfg,pFrom,pos);
	return eHR_Success;
}

CCfgArg* CRelShockWaveMagicMOP::InitArg(const string& szArg)
{
	CShockWaveMagicCfgArg* pCfgArg = new CShockWaveMagicCfgArg(&CShockWaveMagicCfgServer::GetShockWaveMagic(szArg));
	return pCfgArg;
}

//ÑÓ³ÙÊ©·Å³å»÷²¨Tick
class CDelayRelShockWaveMagicTick
	:public CDelayMagicOpTick
	,public CConfigMallocObject
{
public:
	CDelayRelShockWaveMagicTick(CSkillInstServer* pSkillInst,const CShockWaveMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,const CFPos& pos);

	CFPos m_pos;
	CShockWaveMagicCfgServerSharedPtr* m_pCfg;
private:
	~CDelayRelShockWaveMagicTick();
	void OnTick();
};

CDelayRelShockWaveMagicTick::CDelayRelShockWaveMagicTick(CSkillInstServer* pSkillInst,const CShockWaveMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,const CFPos& pos)
:CDelayMagicOpTick(pSkillInst, pFrom)
,m_pos(pos)
,m_pCfg(new CShockWaveMagicCfgServerSharedPtr(cfg))
{
	pSkillInst->AddRef();
}

CDelayRelShockWaveMagicTick::~CDelayRelShockWaveMagicTick()
{
	m_pSkillInst->DelRef();
	SafeDelete(m_pCfg);
}

void CDelayRelShockWaveMagicTick::OnTick()
{
	DetachEvent();
	if (m_pFrom)
	{
		new CShockWaveMagicServer(m_pSkillInst, *m_pCfg, m_pFrom, m_pos);
	}
	delete this;
}

uint32 CDelayRelShockWaveMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CShockWaveMagicCfgArg* pShockWaveMagicCfg = static_cast<CShockWaveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CShockWaveMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
	pFrom->RegistDistortedTick(new CDelayRelShockWaveMagicTick(pSkillInst, pCfg, pFrom, pos),uint32(330));

	return eHR_Success;
}

CCfgArg* CDelayRelShockWaveMagicMOP::InitArg(const string& szArg)
{
	CShockWaveMagicCfgArg* pCfgArg = new CShockWaveMagicCfgArg(&CShockWaveMagicCfgServer::GetShockWaveMagic(szArg));
	return pCfgArg;
}

class CDelayRelPositionMagicTick
	:public CDelayMagicOpTick
	,public CConfigMallocObject
{
public:
	CDelayRelPositionMagicTick(CSkillInstServer* pSkillInst,const CPositionMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,const CFPos& pos);

	CFPos m_pos;
	CPositionMagicCfgServerSharedPtr* m_pCfg;
private:
	~CDelayRelPositionMagicTick();
	void OnTick();
};

CDelayRelPositionMagicTick::CDelayRelPositionMagicTick(CSkillInstServer* pSkillInst,const CPositionMagicCfgServerSharedPtr& cfg,CFighterDictator* pFrom,const CFPos& pos)
:CDelayMagicOpTick(pSkillInst, pFrom)
,m_pos(pos)
,m_pCfg(new CPositionMagicCfgServerSharedPtr(cfg))
{
	pSkillInst->AddRef();
}

CDelayRelPositionMagicTick::~CDelayRelPositionMagicTick()
{
	m_pSkillInst->DelRef();
	SafeDelete(m_pCfg);
}

void CDelayRelPositionMagicTick::OnTick()
{
	DetachEvent();
	if (m_pFrom)
	{
		new CPositionMagicServer(m_pSkillInst, *m_pCfg, m_pFrom, m_pos);
	}
	delete this;
}

uint32 CDelayRelPositionMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CDelayPositionMagicCfgArg* pPositionMagicCfg = static_cast<CDelayPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CPositionMagicCfgServerSharedPtr& pCfg = *pPositionMagicCfg->m_pCfg;
	pFrom->RegistDistortedTick(new CDelayRelPositionMagicTick(pSkillInst, pCfg, pFrom, pos),pPositionMagicCfg->m_uDelayTime*1000);

	return eHR_Success;
}

CCfgArg* CDelayRelPositionMagicMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CDelayPositionMagicCfgArg(atoi(pCfgCalc.GetString(0).c_str()), &CPositionMagicCfgServer::GetPositionMagic(pCfgCalc.GetString(1)));
}

uint32 CRelPositionMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{
	CPositionMagicCfgArg* pShockWaveMagicCfg = static_cast<CPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CPositionMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
	new CPositionMagicServer(pSkillInst,pCfg,pFrom,pos);
	return eHR_Success;
}

CCfgArg* CRelPositionMagicMOP::InitArg(const string& szArg)
{
	CPositionMagicCfgArg* pCfgArg = new CPositionMagicCfgArg(&CPositionMagicCfgServer::GetPositionMagic(szArg), szArg);
	return pCfgArg;
}

uint32 CRelPositionMagicByDirAndDistanceMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{	
	CPositionMagicCfgArg* pShockWaveMagicCfg = static_cast<CPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	int32 iDis;
	CfgChk::SetValue(iDis, pShockWaveMagicCfg->m_pArg->GetString(1));

	CDir dir(pSkillInst->GetDir());
	CVector2f aVector2f;
	dir.Get(aVector2f);
	CFPos fposDis(aVector2f.x * float(iDis * eGridSpan), aVector2f.y * float(iDis * eGridSpan));
	CFPos fposTarget = pSkillInst->GetSkillPos() + fposDis;
	CFPos posTarget(fposTarget.x+eGridSpan*0.5f, fposTarget.y+eGridSpan*0.5f);

	if(!pFrom->GetScene()->IsPixelValid(posTarget))
	{
		return eHR_Fail;
	}

	CPositionMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
	new CPositionMagicServer(pSkillInst, pCfg, pFrom, posTarget);
	return eHR_Success;
}

CCfgArg* CRelPositionMagicByDirAndDistanceMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	CPositionMagicCfgArg* pCfgArg = new CPositionMagicCfgArg(&CPositionMagicCfgServer::GetPositionMagic(pCfgCalc.GetString(0)),szArg);
	return pCfgArg;
}

uint32 CRelPositionMagicRandomInAreaMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, const CFPos& pos)
{	
	CPositionMagicCfgArg* pShockWaveMagicCfg = static_cast<CPositionMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	
	int32 iDis, iNum;
	CfgChk::SetValue(iDis, pShockWaveMagicCfg->m_pArg->GetString(1));
	CfgChk::SetValue(iNum, pShockWaveMagicCfg->m_pArg->GetString(2));
	int32 iMinDis = iDis/6;
	int32 iMaxDis = iDis*5/6;
	float fOffSetDir = 1.0f/float(iNum*2.0f);
	CDir dir(uint8(256*fOffSetDir));
	CVector2f aVector2f;

	for (int32 i = 0; i < iNum; ++i)
	{
		dir.uDir = uint8(CQuickRand::Rand1(dir.uDir * (1 - fOffSetDir), dir.uDir * (1 + fOffSetDir)));
		dir.Get(aVector2f);
		int32 iCurDis = CQuickRand::Rand1(iMinDis, iMaxDis);
		CFPos fposDis(aVector2f.x * float(iCurDis * eGridSpan), aVector2f.y * float(iCurDis * eGridSpan));
		CFPos posTarget = pos + fposDis;
		if(pFrom->GetScene()->IsPixelValid(posTarget))
		{
			CPositionMagicCfgServerSharedPtr& pCfg = *pShockWaveMagicCfg->m_pCfg;
			new CPositionMagicServer(pSkillInst, pCfg, pFrom, posTarget);
		}	
		dir.uDir += uint8(512 * fOffSetDir);
	}
	return eHR_Success;
}

CCfgArg* CRelPositionMagicRandomInAreaMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	CPositionMagicCfgArg* pCfgArg = new CPositionMagicCfgArg(&CPositionMagicCfgServer::GetPositionMagic(pCfgCalc.GetString(0)),szArg);
	return pCfgArg;
}

uint32 CRelPosBulletMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{	
	CBulletMagicCfgArg* pBulletMagicCfg = static_cast<CBulletMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CBulletMagicCfgServerSharedPtr& pCfg = *pBulletMagicCfg->m_pCfg;
	new CBulletMagicServer(pSkillInst,pCfg,pFrom,pos);
	return eHR_Success;
}

CCfgArg* CRelPosBulletMagicMOP::InitArg(const string& szArg)
{
	CBulletMagicCfgArg* pCfgArg = new CBulletMagicCfgArg(&CBulletMagicCfgServer::GetBulletMagic(szArg));
	return pCfgArg;
}

uint32 CRelPosMoveMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgArg* pArg, CFighterDictator *pFrom, const CFPos& pos)
{	
	CMoveMagicCfgArg* pBulletMagicCfg = static_cast<CMoveMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
	CMoveMagicCfgServerSharedPtr& pCfg = *pBulletMagicCfg->m_pCfg;
	if (!pFrom)
	{
		return eHR_Fail;
	}
	CCharacterDictator* pCharacter = pFrom->GetCharacter();
	pCharacter->SetAndSyncActionDirByPos(pos);
	new CMoveMagicServer(pSkillInst,pCfg,pFrom,pos);
	return eHR_Success;
}

CCfgArg* CRelPosMoveMagicMOP::InitArg(const string& szArg)
{
	CMoveMagicCfgArg* pCfgArg = new CMoveMagicCfgArg(&CMoveMagicCfgServer::GetMoveMagic(szArg));
	return pCfgArg;
}

uint32 CRelBattleArrayMagicMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, const CFPos& pos)
{	
	CFighterMediator* pFighter=pFrom?pFrom->CastToFighterMediator():NULL;
	Ast(pFighter)
	CMagicMgrServer* pMagicMgr = pFighter->GetMagicMgr();
	CBattleArrayMagicCfgServerSharedPtr& pCfg = CBattleArrayMagicCfgServer::GetBattleArrayMagic(pMagicMgr->GetArrayName());
	CBattleArrayMagicServer::VecBattleArray_t vecArray=pMagicMgr->GetBattleArrayVector();
	CBattleArrayMagicServer::PositionTransfer(vecArray,pos);
	pMagicMgr->SetBattleArrayVector(vecArray);
	if (pMagicMgr->GetArrayStation()==NULL&&pCfg)
	{
		new CBattleArrayMagicServer(pSkillInst, pCfg, pFrom, vecArray);
		return eHR_Success;
	}
	else
	{
		return eHR_Fail;
	}
}

uint32 CCreateTotemMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, const CFPos& Pos)
{
	const string& strServantName = Arg.GetString();

	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
		pHandler->OnCreateServant(pFrom->GetEntityID(),strServantName.c_str(),pSkillInst->GetSkillLevel(),Pos.x,Pos.y,true);
	return eHR_Success;
}

uint32 CCreateTotemByDirAndDistanceMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, const CFPos& Pos)
{
	const string& magicName = Arg.GetString(0);
	int32 iDis;
	CfgChk::SetValue(iDis, Arg.GetString(1));

	CDir dir(pSkillInst->GetDir());
	CVector2f aVector2f;
	dir.Get(aVector2f);
	CFPos fposDis(aVector2f.x * float(iDis * eGridSpan), aVector2f.y * float(iDis * eGridSpan));
	CFPos fposTarget = pSkillInst->GetSkillPos() + fposDis;
	CFPos posTarget(fposTarget.x+eGridSpan*0.5f, fposTarget.y+eGridSpan*0.5f);

	if(!pFrom->GetScene()->IsPixelValid(posTarget))
	{
		return eHR_Fail;
	}

	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
		pHandler->OnCreateServant(pFrom->GetEntityID(),magicName.c_str(),pSkillInst->GetSkillLevel(),posTarget.x,posTarget.y,true);

	return eHR_Success;
}

uint32 CCreateShadowMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, const CFPos& Pos)
{	
	const string& strServantName = Arg.GetTestString();
	CPos pos;
	PixelPos2GridPos(pos,Pos);

	CCoreSceneServer* pScene = pFrom->GetScene();
	CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
	int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
	int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
	if(pos.x < 0 || pos.y < 0 || pos.x > uMaxGridInX || pos.y > uMaxGridInY)
		return eHR_Fail;

	int32 iX = Random::Rand(-2, 2);
	int32 iY = Random::Rand(-2, 2);
	CPos posRandom;
	posRandom.x = pos.x + iX;
	posRandom.y = pos.y + iY;
	if(posRandom.x < 0 || posRandom.x > uMaxGridInX)
		posRandom.x = pos.x;
	if(posRandom.y < 0 || posRandom.y > uMaxGridInY)
		posRandom.y = pos.y;
	CFPos fPos;
	GridPos2PixelPos(fPos, posRandom);
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
		pHandler->OnCreateShadow(pFrom->GetEntityID(),strServantName.c_str(),pSkillInst->GetSkillLevel(),fPos.x,fPos.y,true);

	return eHR_Success;
}

uint32 CPursueKillMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos)
{
	double dPercent=Arg.GetDblValue(pFrom,0);
	double dArea=Arg.GetDblValue(pFrom,1);
	CFighterDictator* pTarget = NULL;

	VecFighter vecCoreObj;
	QueryObj(vecCoreObj, pFrom, float(dArea), eSR_PlayerEnemy);
	VecFighter::iterator it_coreObj = vecCoreObj.begin();
	for(; it_coreObj != vecCoreObj.end(); ++it_coreObj)
	{
		if (!(*it_coreObj)->CppIsAlive())
			continue;

		uint32 uNowHP = (*it_coreObj)->m_HealthPoint.LimitGet();
		uint32 uMaxHP = (*it_coreObj)->m_HealthPoint.Get((*it_coreObj));
		CCoreSceneServer* pScene = pFrom->GetScene();
		CPixelPath* pPath =pScene->CreatePath(pFrom->GetPixelPos(), (*it_coreObj)->GetPixelPos(), eFPT_Line, eBT_LowBarrier);
		if(uNowHP < uMaxHP*dPercent && pPath != NULL)
		{
			pTarget=(*it_coreObj);
			pPath->Release();
			break;  
		}
		pPath->Release();
	}
	if (NULL == pTarget)
		return eHR_Fail;

	pFrom->SetTarget(pTarget);
	pFrom->SetLockingTarget(pTarget);
	CFighterMediator* pFighter=pFrom?pFrom->CastToFighterMediator():NULL;
	ICharacterMediatorCallbackHandler* pHandler = pFighter?pFighter->GetCallBackHandler():NULL;
	if(pHandler)
		pHandler->OnSetLockObj(pFrom->GetEntityID(),pTarget->GetEntityID());
	if(pFighter)
		pFighter->DoNormalAttack(pTarget);
	return eHR_Success;
}

void CPursueKillMOP::QueryObj(VecFighter& vecAreaObj, CFighterDictator* pFrom, float fArea, ESimpleRelation eRelation)
{
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();

	vector<CCoreObjectServer*> vecCoreObj;
	CCoreSceneServer* pScene = pFrom->GetScene();
	CFPos pixelPos = pFrom->GetPixelPos();
	pScene->QueryObject(vecCoreObj, pixelPos, fArea);

	vector<CCoreObjectServer*>::iterator it_CoreObj = vecCoreObj.begin();
	for(; it_CoreObj != vecCoreObj.end(); ++it_CoreObj)
	{
		CCharacterDictator *pCharacter=CCharacterDictator::GetCharacterByCoreObj(*it_CoreObj);
		if(pCharacter == NULL)
			continue;
		CFighterDictator* pTarget = pCharacter->GetFighter();

		if (!pTarget->CppIsAlive())
			continue;

		if(relationMgr.IsFriend(pFrom, pTarget))
		{
			if (eSR_PlayerFriend == eRelation)
				vecAreaObj.push_back(pTarget);			
		}

		if(relationMgr.IsEnemy(pFrom, pTarget))
		{
			if (eSR_PlayerEnemy == eRelation)
				vecAreaObj.push_back(pTarget);
		}
	}
}

uint32 CSetupDynamicBarrierMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, const CFPos& pos)
{
	//CCoreSceneServer* pScene = pFrom->GetScene();
	//CCoreObjectDictator* pCoreObj = pScene->CreateObjectDictator(pos, NULL, NULL);
	////pCoreObj->SetGridPos(pos);
	//pCoreObj->SetSize(1);
	//vector<CPos> dynamicBarrier;
	//dynamicBarrier.push_back(pos);
	//dynamicBarrier.push_back(CPos(pos.x, pos.y+1));
	//dynamicBarrier.push_back(CPos(pos.x+1, pos.y));
	//dynamicBarrier.push_back(CPos(pos.x+1, pos.y+1));
	//dynamicBarrier.push_back(CPos(pos.x, pos.y-1));
	//dynamicBarrier.push_back(CPos(pos.x-1, pos.y));
	//dynamicBarrier.push_back(CPos(pos.x-1, pos.y-1));
	//dynamicBarrier.push_back(CPos(pos.x-1, pos.y+1));
	//dynamicBarrier.push_back(CPos(pos.x+1, pos.y-1));

	//pCoreObj->SetDynamicBarrier(dynamicBarrier, eBT_HighBarrier);
	SQR_TRY
	{
		CBarrierMagicCfgArg* pBarrierMagicCfg = static_cast<CBarrierMagicCfgArg*>(const_cast<CCfgArg*>(pArg));
		CBarrierMagicCfgServerSharedPtr& pCfg = *pBarrierMagicCfg->m_pCfg;
		Ast(pCfg);

		CBarrierMagicServer* pBarrierMagic = new CBarrierMagicServer(pSkillInst, pCfg, pFrom, pos);
		pBarrierMagic->Create();
	}
	SQR_CATCH(exp)
	{
		LogExp(exp);		
		return eHR_Fail;
	}
	SQR_TRY_END;

	return eHR_Success;
}
CCfgArg* CSetupDynamicBarrierMOP::InitArg(const string& szArg)
{
	CBarrierMagicCfgArg* pCfgArg = new CBarrierMagicCfgArg(&CBarrierMagicCfgServer::GetBarrierMagic(szArg));
	return pCfgArg;
}

uint32 CUseDeadBobyMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos)
{
	//return eHR_Fail;
	return eHR_Success;
}

uint32 CEraseEnemyPositionMagicMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos)
{
	const string& sPositonMagicName = Arg.GetString(1);
	float dArea ;
	CfgChk::SetValue(dArea, Arg.GetString(0));
	VecPosMagic vecPositionMagic;
	QueryObj(vecPositionMagic, pFrom, pos, dArea, eSR_PlayerEnemy);

	VecPosMagic::iterator it = vecPositionMagic.begin();
	for(; it != vecPositionMagic.end(); ++it)
	{
		CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(*it);
		if(pCoreObj && pCoreObj->GetHandler() && (size_t)(pCoreObj->GetHandler()->GetTag()) == eCOHT_Magic)
		{
			CMagicServer* pMagic = class_cast<CMagicServer*>(pCoreObj->GetHandler());
			if (pMagic && pMagic->GetMagicType() == eMGT_Position)
			{
				CPositionMagicServer* pPositionMagic = class_cast<CPositionMagicServer*>(pMagic);
				if (pPositionMagic && pPositionMagic->GetPostionMagicName() == sPositonMagicName)
					pPositionMagic->Cancel();
			}
		}
	}
	return eHR_Success;
}

void CEraseEnemyPositionMagicMOP::QueryObj(VecPosMagic& vecAreaObj, CFighterDictator* pFrom, const CFPos& pos, float fArea, ESimpleRelation eRelation)
{
	CRelationMgrServer& relationMgr = CRelationMgrServer::Inst();
	vector<CCoreObjectServer*> vecCoreObj;
	CCoreSceneServer* pScene = pFrom->GetScene();
	pScene->QueryObject(vecCoreObj, pos, fArea);

	vector<CCoreObjectServer*>::iterator it_CoreObj = vecCoreObj.begin();
	for(; it_CoreObj != vecCoreObj.end(); ++it_CoreObj)
	{
		if((*it_CoreObj)->GetHandler() && (size_t)((*it_CoreObj)->GetHandler()->GetTag()) == eCOHT_Magic)
		{
			CMagicServer* pMagic = class_cast<CMagicServer*>((*it_CoreObj)->GetHandler());
			if (pMagic && pMagic->GetMagicType() == eMGT_Position)
			{
				CPositionMagicServer* pPositionMagic = class_cast<CPositionMagicServer*>(pMagic);
				if (!pPositionMagic)
					continue;
				CFighterDictator* pMagicOwer = pPositionMagic->GetFighter();
				if (pMagicOwer && !pMagicOwer->CppIsAlive())
					continue;

				if(relationMgr.IsFriend(pFrom, pMagicOwer))
				{
					if (eSR_PlayerFriend == eRelation)
						vecAreaObj.push_back(pPositionMagic->GetGlobalID());
				}
				
				if(relationMgr.IsEnemy(pFrom, pMagicOwer))
				{
					if (eSR_PlayerEnemy == eRelation)
						vecAreaObj.push_back(pPositionMagic->GetGlobalID());
				}
			}
		}
	}
}

uint32 CPosCreateNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& Pos)
{	
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}
	CPos pos;
	PixelPos2GridPos(pos,Pos);
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
		int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
		if(pos.x < 0 || pos.y < 0 || pos.x > uMaxGridInX || pos.y > uMaxGridInY)
			return eHR_Fail;

		int32 iX = Random::Rand(-2, 2);
		int32 iY = Random::Rand(-2, 2);
		CPos posRandom;
		posRandom.x = pos.x + iX;
		posRandom.y = pos.y + iY;
		if(posRandom.x < 0 || posRandom.x > uMaxGridInX)
			posRandom.x = pos.x;
		if(posRandom.y < 0 || posRandom.y > uMaxGridInY)
			posRandom.y = pos.y;

		CFPos fPos;
		GridPos2PixelPos(fPos, posRandom);
		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), 0, pFrom->GetEntityID(),
			strNPCName.c_str(), 0, fPos.x, fPos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CPosCreateNPCLevelSameMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& Pos)
{	
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}
	CPos pos;
	PixelPos2GridPos(pos,Pos);
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
		int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
		if(pos.x < 0 || pos.y < 0 || pos.x > uMaxGridInX || pos.y > uMaxGridInY)
			return eHR_Fail;

		int32 iX = Random::Rand(-2, 2);
		int32 iY = Random::Rand(-2, 2);
		CPos posRandom;
		posRandom.x = pos.x + iX;
		posRandom.y = pos.y + iY;
		if(posRandom.x < 0 || posRandom.x > uMaxGridInX)
			posRandom.x = pos.x;
		if(posRandom.y < 0 || posRandom.y > uMaxGridInY)
			posRandom.y = pos.y;
		CFPos fPos;
		GridPos2PixelPos(fPos, posRandom);
		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), 0, pFrom->GetEntityID(),
			strNPCName.c_str(), pFrom->GetCharacter()->CppGetLevel(), fPos.x, fPos.y);

		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CTargetPosCreateNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos)
{
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}
	CPos gridPos;
	PixelPos2GridPos(gridPos,pos);
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
		int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
		if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x > uMaxGridInX || gridPos.y > uMaxGridInY)
			return eHR_Fail;

		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), 0, pFrom->GetEntityID(),
			strNPCName.c_str(), 0, pos.x, pos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

class CPosDelayCreateNpcTick
	:public CDelayMagicOpTick
	,public CConfigMallocObject
{
public:
	CPosDelayCreateNpcTick(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,const CFPos& pos,const string& strNPCName);

	CFPos	m_pos;
	string	m_strNPCName;
private:
	~CPosDelayCreateNpcTick();
	void OnTick();
};

CPosDelayCreateNpcTick::CPosDelayCreateNpcTick(CSkillInstServer* pSkillInst,CFighterDictator* pFrom,const CFPos& pos, const string& strNPCName)
:CDelayMagicOpTick(pSkillInst, pFrom)
,m_pos(pos)
,m_strNPCName(strNPCName)
{
	pSkillInst->AddRef();
}

CPosDelayCreateNpcTick::~CPosDelayCreateNpcTick()
{
	m_pSkillInst->DelRef();
}

void CPosDelayCreateNpcTick::OnTick()
{
	DetachEvent();
	ICharacterDictatorCallbackHandler* pHandler = m_pFrom->GetCallBackHandler();
	if (pHandler)
	{
		pHandler->OnCreateNPCWithTarget(m_pFrom->GetEntityID(), 0, 0, m_strNPCName.c_str(), 0, m_pos.x, m_pos.y);
	}
	delete this;
}

uint32 CPosDelayCreateNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgArg* pArg, CFighterDictator* pFrom, const CFPos& pos)
{
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}

	CPosDelayCreateNPCArg* pTowStringArg = static_cast<CPosDelayCreateNPCArg*>(const_cast<CCfgArg*>(pArg));
	pFrom->RegistDistortedTick(new CPosDelayCreateNpcTick(pSkillInst, pFrom, pos, pTowStringArg->m_strArg2), pTowStringArg->m_uArg1*1000);

	return eHR_Success;
}

CCfgArg* CPosDelayCreateNPCMOP::InitArg(const string& szArg)
{
	CCfgCalc pCfgCalc(szArg);
	return new CPosDelayCreateNPCArg(atoi(pCfgCalc.GetString(0).c_str()), pCfgCalc.GetString(1));
}

uint32 CTargetPosCreateNPCLevelSameMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& pos)
{
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}
	CPos gridPos;
	PixelPos2GridPos(gridPos,pos);
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
		int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
		if(gridPos.x < 0 || gridPos.y < 0 || gridPos.x > uMaxGridInX || gridPos.y > uMaxGridInY)
			return eHR_Fail;

		pHandler->OnCreateNPCWithTarget(pFrom->GetEntityID(), 0, pFrom->GetEntityID(),
			strNPCName.c_str(), pFrom->GetCharacter()->CppGetLevel(), pos.x, pos.y);

		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CPosCreateDeadNPCMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& Pos)
{	
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}

	//string strNPCName = Arg.GetString();
	CPos pos;
	PixelPos2GridPos(pos,Pos);
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		int32 uMaxGridInX = int32(pMetaScene->GetWidthInGrid());
		int32 uMaxGridInY = int32(pMetaScene->GetHeightInGrid());
		if(pos.x < 0 || pos.y < 0 || pos.x > uMaxGridInX || pos.y > uMaxGridInY)
			return eHR_Fail;

		int32 iX = Random::Rand(-2, 2);
		int32 iY = Random::Rand(-2, 2);
		CPos posRandom;
		posRandom.x = pos.x + iX;
		posRandom.y = pos.y + iY;
		if(posRandom.x < 0 || posRandom.x > uMaxGridInX)
			posRandom.x = pos.x;
		if(posRandom.y < 0 || posRandom.y > uMaxGridInY)
			posRandom.y = pos.y;
		CFPos fPos;
		GridPos2PixelPos(fPos, posRandom);
		pHandler->OnCreateDeadNPC(pFrom->GetEntityID(), pSkillInst->GetSkillLevel(), fPos.x, fPos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

uint32 CPostionPlayFXMOP::Do(CSkillInstServer *pSkillInst, const CCfgCalc& Arg, CFighterDictator *pFrom, const sqr::CFPos &pos)
{
	return eHR_Success;
}

uint32 CPosCreateNpcWithMasterMOP::Do(CSkillInstServer* pSkillInst, const CCfgCalc& Arg, CFighterDictator* pFrom, const CFPos& Pos)
{	
	if(pFrom == NULL)
	{
		return eHR_Fail;
	}
	const string& strNPCName = Arg.GetString();
	ICharacterDictatorCallbackHandler* pHandler = pFrom->GetCallBackHandler();
	if(pHandler) 
	{
		CCoreSceneServer* pScene = pFrom->GetScene();
		CMetaSceneServer* pMetaScene = pScene->GetMetaScene();
		float fMaxGridInX = float(pMetaScene->GetWidthInPixel());
		float fMaxGridInY = float(pMetaScene->GetHeightInPixel());
		if(Pos.x < 0 || Pos.y < 0 || Pos.x > fMaxGridInX || Pos.y > fMaxGridInY)
			return eHR_Fail;

		pHandler->PosCreateNpcWithMaster(pFrom->GetEntityID(), strNPCName.c_str(), Pos.x, Pos.y);
		return eHR_Success;
	}
	return eHR_Fail;
}

