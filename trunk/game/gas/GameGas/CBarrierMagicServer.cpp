#include "stdafx.h"
#include "CBarrierMagicServer.h"
#include "CBarrierMagicCfgServer.h"
#include "CMagicServer.inl"
#include "CFighterDictator.h"
#include "CCoreSceneServer.h"
#include "CCharacterDictator.h"
#include "CAoiDimMgr.h"
#include "CoreCommon.h"
#include "CSkillInstServer.h"
#include "CCoreObjectDictator.h"
#include "CCfgCalc.inl"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"
#include "CAppServer.h"

CBarrierMagicServer::CBarrierMagicServer(CSkillInstServer* pSkillInst, const CBarrierMagicCfgServerSharedPtr& pCfg, CFighterDictator* pFrom, const CFPos& Pos)
:CMagicServer(pSkillInst, pFrom,0)
,m_pScene(NULL)
,m_eCamp(eCamp_None)
,m_pCfg(new CBarrierMagicCfgServerSharedPtr(pCfg))
{
	CPos pos;
	PixelPos2GridPos(pos,Pos);
	Ast(pSkillInst->GetMagicDir() < pCfg->m_vecCoord.size());
	CBarrierMagicCfgServer::VecPos_t vecBarrierPos = pCfg->m_vecCoord[pSkillInst->GetMagicDir()];
	m_vecBarrierPos.clear();

	Ast(GetFighter());
	m_eCamp = GetFighter()->CppGetCamp();
	CCharacterDictator* pChar = GetFighter()->GetCharacter();
	Ast(pChar);
	CCoreSceneServer* pScene = pChar->GetScene();
	Ast(pScene);
	m_pScene = pScene;

	CBarrierMagicCfgServer::VecPos_t::iterator it = vecBarrierPos.begin();
	for(; it != vecBarrierPos.end(); ++it)
	{
		CPos posTmp(it->x + pos.x, it->y + pos.y);
		if(m_pScene->IsGridValid(pos))
		{
			m_vecBarrierPos.push_back(posTmp);
		}
		else	//非法点不创建动态障碍
		{
			//ostringstream strm;
			//strm << "创建动态障碍时使用了非法的坐标点 " << posTmp.x << "," << posTmp.y;
			//GenErr(strm.str());
		}
	}
}

CBarrierMagicServer::~CBarrierMagicServer()
{
	if(!m_vecObj.empty())
	{
		VecObj_t::iterator it = m_vecObj.begin();
		for(; it != m_vecObj.end(); ++it)
		{
			CCoreObjectDictator* pObj = (*it);

			pObj->SetHandler(NULL);
			m_pScene->DestroyObject(pObj);
		}

		m_vecObj.clear();
	}
	CFighterDictator* pFrom = GetFighter();
	if(pFrom)
	{
		pFrom->UnRegistDistortedTick(this);
	}
	else
	{
		CAppServer::Inst()->UnRegisterDistortedTick(this);
	}
	SafeDelete(m_pCfg);
}

bool CBarrierMagicServer::Create()
{
	Ast(GetFighter());
	Ast(GetFighter()->GetCharacter());

	Ast(m_pCfg);
	CBarrierMagicCfgServerSharedPtr& pCfg = GetCfgSharedPtr();
	Ast(pCfg);
	//pChar->SetDynamicBarrier(m_vecBarrierPos, pCfg->GetBarrierType());

	uint32 uExpireTime = uint32(pCfg->m_pTime->GetDblValue(GetFighter()) * 1000);
	GetFighter()->RegistDistortedTick(this, uExpireTime);

	m_vecObj.clear();
	VecPos_t::iterator it = m_vecBarrierPos.begin();
	CFPos pixelPos;
	for(; it != m_vecBarrierPos.end(); ++it)
	{
		GridPos2PixelPos(pixelPos, *it);
		if( pCfg->GetBarrierMagicType() == CBarrierMagicCfgServer::eBMT_Reflect)
		{
			m_eEyeSightType = eAIET_RefMagic;
			m_eSizeType = eAIST_IntObj;
		}
		CCoreObjectDictator* pObj = CreateCoreObj<CBarrierMagicServer>(pixelPos);


		pObj->SetGridPos(*it);	
		SetEyeSight(2);

		VecPos_t vecPos;
		vecPos.push_back(*it);
		pObj->SetBarrier(0.5f, pCfg->GetBarrierType());

		m_vecObj.push_back(pObj);
	}

	return true;
}

void CBarrierMagicServer::OnTick()
{
	cout << "Time is up, barrier~~   Damn it." << endl;
	delete this;
	return;
}

void CBarrierMagicServer::OnCaughtViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	// 暂时注释掉 这 CRelationMgr::IsFriend 重构后，BarrierMagic 给的参数过少了

	//cout << "Barriers've seen " << uObjGlobalId << endl;
	//CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	//if (!pCoreObj || !pCoreObj->GetHandler())
	//	return;
	//size_t uType = (size_t)(pCoreObj->GetHandler()->GetTag());
	//if(uType == (uint32)(eCOHT_Magic))
	//{
	//	CMagicServer* pMagic = class_cast<CMagicServer*>(pCoreObj->GetHandler());
	//	if(pMagic->GetMagicType() == eMGT_ShockWave)
	//	{
	//		//m_setInSightCoreObjOfMagic.insert(uObjGlobalId);
	//		CShockWaveMagicHandlerServer* pShockWaveMagic = class_cast<CShockWaveMagicHandlerServer*>(pMagic);
	//		Ast(pShockWaveMagic);
	//		CFighterDictator* pFighter = pShockWaveMagic->GetFighter();
	//		if(pFighter == NULL || !CRelationMgr::IsFriend(pFighter->CppGetCamp(), m_eCamp))
	//		{
	//			pShockWaveMagic->DeleteObj(m_pScene);
	//			cout << "Barriers destroy a bullet from shockwavemagic " << endl;
	//		}
	//	}
	//	else if(pMagic->GetMagicType() == eMGT_Bullet)
	//	{
	//		CBulletMagicServer* pBulletMagic = class_cast<CBulletMagicServer*>(pMagic);
	//		Ast(pBulletMagic);
	//		CFighterDictator* pFighter = pBulletMagic->GetFighter();
	//		if(pFighter == NULL || !CRelationMgr::IsFriend(pFighter->CppGetCamp(), m_eCamp))
	//		{
	//			pBulletMagic->DestoryBullet();
	//			cout << "Barriers destroy a bullet " << endl;
	//		}

	//		//m_setInSightCoreObjOfMagic.insert(uObjGlobalId);
	//	}
	//}
}

void CBarrierMagicServer::OnLostViewSightOf(uint32 uObjGlobalId, uint32 uDimension)
{
	//CCoreObjectServer* pCoreObj = CCoreObjectServer::GetObjectServer(uObjGlobalId);
	//if (!pCoreObj || !pCoreObj->GetHandler())
	//	return;
	//uint32 uType = (uint32)(pCoreObj->GetHandler()->GetTag());
	//if(uType == (uint32)(eCOHT_Magic))
	//{
	//	//SetInSightCoreObjOfMagic::iterator it = m_setInSightCoreObjOfMagic.find(uObjGlobalId);
	//	

	//}
}

void CBarrierMagicServer::OnDestroy()
{
	delete this;
}

CBarrierMagicCfgServerSharedPtr& CBarrierMagicServer::GetCfgSharedPtr()const
{
	return *m_pCfg;
}
