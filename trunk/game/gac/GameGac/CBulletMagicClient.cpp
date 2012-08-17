#include "stdafx.h"
#include "CBulletMagicClient.h"
#include "LoadSkillCommon.h"
#include "CTxtTableFile.h"
#include "CCoreObjectFollower.h"
#include "CRenderObject.h"
#include "CCharacterFollower.h"
#include "CFighterDirector.h"
#include "CCoreSceneClient.h"
#include "CRenderSystemClient.h"
#include "CDataScene.h"
#include "FXPlayer.h"
#include "StringHelper.h"
#include "IDistortedTime.h"
#include "CAppClient.h"

#define UPDATE_TRANK_TIME	20

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CBulletMagicCfgClient::MapBulletMagicCfgClientByID CBulletMagicCfgClient::m_mapCfgByID;
CBulletMagicCfgClient::MapBulletTrackType CBulletMagicCfgClient::ms_mapBulletTrackType;

bool CBulletMagicCfgClient::LoadConfig(const TCHAR* cfgFile)
{
	CTxtTableFile TabFile;
	CBulletMagicCfgClient*	pCfgNode = NULL;

	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile)) 
		return false;

	ClearMap(m_mapCfgByID);
	InitMapBulletTrackType();

	for(int32 i = 1; i < TabFile.GetHeight(); ++i)
	{
		pCfgNode = new CBulletMagicCfgClient;
		pCfgNode->m_uID = i;
		pCfgNode->m_sFX = TabFile.GetString(i, szMagic_FxName);
		trimend(pCfgNode->m_sFX);
		pCfgNode->m_fSpeed = TabFile.GetFloat(i, szMagic_Speed, 10);
		pCfgNode->m_fGravity = float(TabFile.GetInteger(i, szMagic_Gravity, 0));
		string sType = TabFile.GetString(i, szMagic_TrackType);
		trimend(sType);
		pCfgNode->m_eBulletTrackType = ms_mapBulletTrackType[sType];
		pCfgNode->m_strFireSkeletalName = TabFile.GetString(i, szMagic_FireSkeleta);
		trimend(pCfgNode->m_strFireSkeletalName);
		pCfgNode->m_sFinalFX = TabFile.GetString(i, szMagic_FinalFxName);
		trimend(pCfgNode->m_sFinalFX);
		CBulletMagicCfgClientSharedPtr* pSharedPtr = new CBulletMagicCfgClientSharedPtr(pCfgNode);
		m_mapCfgByID.insert(make_pair(pCfgNode->m_uID, pSharedPtr));
	}
	return true;
}

void CBulletMagicCfgClient::UnloadConfig()
{
	ClearMap(m_mapCfgByID);
}

CBulletMagicCfgClientSharedPtr& CBulletMagicCfgClient::GetByID(uint32 uID)
{
	stringstream ExpStr;
	CBulletMagicCfgClient::MapBulletMagicCfgClientByID::iterator mapCfgItr;
	mapCfgItr = m_mapCfgByID.find(uID);

	if (mapCfgItr == m_mapCfgByID.end())
	{
		ExpStr << "子弹魔法" << uID << "不存在";
		GenErr(ExpStr.str());
	}
	return *(mapCfgItr->second);
}

CBulletMagicClient::CBulletMagicClient(CCoreObjectFollower* pCoreObjFol, uint32 uSrcEntityID, uint32 uTargetEntityID, const CBulletMagicCfgClientSharedPtr& pCfg, CFPos pos, EBurstSoulType eBurstSoulType)
	: CMagicClient(pCoreObjFol)
	, m_uOwnerEntityID(uSrcEntityID)
	, m_uTargetEntityID(uTargetEntityID)
	, m_pCfg(pCfg)
	, m_uPassTime(UPDATE_TRANK_TIME)
	, m_vSrcPosition(0.0f, 0.0f, 0.0f)
	, m_vCurPosition(0.0f, 0.0f, 0.0f)
	, m_vDstPosition(0.0f, 0.0f, 0.0f)
	, m_fHeightSrcSpeed(0.0f)
	, m_bJump(false)
	, m_fCurPlaneSpeed(pCfg->GetPixelSpeed())
	, m_posJumpPoint(pos)
	, m_eBurstSoulType(eBurstSoulType)
{
	CCharacterFollower* pCharacterSrc = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);
	if (pCharacterSrc)
	{
		CRenderObject* pRenderObjectSrc = pCharacterSrc->GetRenderObject();
		Ast(pRenderObjectSrc);
		BYTE ID = pRenderObjectSrc->GetSkeletalIdByName(pCfg->GetFireSkeletalName());
		if (-1 != ID&& pRenderObjectSrc->IsVaildSkeId(ID))
		{
			m_vSrcPosition = pRenderObjectSrc->GetSkeletalPositionByName(pCfg->GetFireSkeletalName());
		}
		else
		{
			m_vSrcPosition = pRenderObjectSrc->GetCenterPosition();
		}
	}
	else
	{
		CRenderObject* pRenderObj = GetRenderObject();
		CFPos posPixel = m_pCoreObj->GetPixelPos();
		m_vSrcPosition.x = posPixel.x;
		m_vSrcPosition.y = pRenderObj->GetCenterPosition().y + 50.0f;
		m_vSrcPosition.z = posPixel.y;
	}

	m_vCurPosition = m_vSrcPosition;

	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);

	switch (m_pCfg->GetTrackType())
	{
	case eBTT_Line:
		{
			CRenderObject* pRenderObj = GetRenderObject();
			if (pCharTarget)
			{
				CRenderObject* pRenderObjTarget = pCharTarget->GetRenderObject();
				float fTargetRealHeight = pRenderObjTarget->GetCenterPosition().y;
		
				float fTan = 0.0f;
				if (GetCurDistance() != 0.0f)
					fTan = (fTargetRealHeight - m_vCurPosition.y)/GetCurDistance();
				pRenderObj->Pitch(-atan(fTan));
				
				m_pCoreObj->GetDistortedTime()->RegistDistortedTick(this, UPDATE_TRANK_TIME);
			}
			pRenderObj->SetPosition(m_vCurPosition);
			pRenderObj->SetDirection(GetCurDir());
		}
		break;
	case eBTT_Parabola:
	case eBTT_MarblesSoul:
		{
			if (m_pCfg->GetTrackType() != eBTT_MarblesSoul)
			{
				CFPos posDetPixel;
				if (pCharTarget)
					pCharTarget->GetPixelPos(posDetPixel);
				else
					m_pCoreObj->GetDestOfCurPath(posDetPixel);

				m_vDstPosition.x = posDetPixel.x;
				m_vDstPosition.z = posDetPixel.y;

				m_fRealPixelSpeed = m_pCfg->GetPixelSpeed();
				m_fRealGravity = m_pCfg->GetGravity();
			}
			else
			{
				m_vDstPosition.x = m_posJumpPoint.x;
				m_vDstPosition.z = m_posJumpPoint.y;

				m_fRealPixelSpeed = CQuickRand::Rand1(m_pCfg->GetPixelSpeed() - 16.0f, m_pCfg->GetPixelSpeed() + 16.0f);
				m_fRealGravity = CQuickRand::Rand1(m_pCfg->GetGravity() - 50.0f, m_pCfg->GetGravity() + 50.0f);
				m_fPlaneAcc = CQuickRand::Rand1(250.0f, 350.0f);
			}

			CCoreSceneClient* pCoreScene = CCoreSceneClient::MainScene();
			m_vDstPosition.y = pCoreScene->GetRenderScene()->GetDataScene()->GetOffsetHeightInPixel(m_vDstPosition.x, m_vDstPosition.z);

			if (GetCurDistance() != 0.0f)
			{
				float fFlyTime = GetCurDistance()/m_fRealPixelSpeed;
				m_fHeightSrcSpeed = (m_vDstPosition.y - m_vCurPosition.y - m_fRealGravity * fFlyTime * fFlyTime/2) / fFlyTime;
			}
			else
				m_fHeightSrcSpeed = 0.0f;
			m_fJumpPointRadian = float(TWO_PI * GetCurDir().uDir / 256);

			CFPos posPixel(m_vSrcPosition.x, m_vSrcPosition.z);
			GetRenderObject()->SetDirection(GetCurDir());
			GetRenderObject()->Pitch(-atan(m_fHeightSrcSpeed/m_fRealPixelSpeed));
			pCoreScene->GetRenderScene()->AttachRenderObjectAbs(*GetRenderObject(), CVector3f(m_vCurPosition.x, m_vCurPosition.y, m_vCurPosition.z));
			CCharacterFollower* pCharater = pCharacterSrc?pCharacterSrc:(pCharTarget?pCharTarget:NULL);
			if (pCharater&&pCharater->CoreObjisExist())
			{
				pCharater->GetDistortedTime()->RegistDistortedTick(this,UPDATE_TRANK_TIME);
			}
			else
			{
				CAppClient::Inst()->RegisterDistortedTick(this,UPDATE_TRANK_TIME);
			}
		}
		break;
	}
}

CBulletMagicClient::~CBulletMagicClient()
{
	Relase();
}

void CBulletMagicClient::Relase()
{
	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);
	if (m_pCoreObj)
	{
		m_pCoreObj->GetDistortedTime()->UnregistDistortedTick(this);
		m_pCoreObj->SetHandler(NULL);
	}
	if (m_pCfg->GetTrackType() == eBTT_MarblesSoul && m_eBurstSoulType != eBST_Normal)
	{
		CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);
		CFighterFollower* pFighterTarget = pCharTarget?pCharTarget->GetFighter():NULL;
		if (pFighterTarget && pFighterTarget->CastToFighterDirector())
			pFighterTarget->CastToFighterDirector()->NotifyBurstSoulArrived(m_eBurstSoulType);
	}
}

void CBulletMagicClient::OnTick()
{
	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);
	if (!pCharTarget && !m_pCoreObj || GetCurDistance() == 0.0f|| !GetRenderObject())
	{
		delete this;
		return;
	}

	switch (m_pCfg->GetTrackType())
	{
	case eBTT_Line:
		UpdateTrackOfLine();
		break;
	case eBTT_Parabola:
		UpdateTrackOfParabola();
		break;
	case eBTT_MarblesSoul:
		UpdateTrackOfMarblesSoul();
		break;
	}
}

CFPos CBulletMagicClient::GetDstPixelPos() const
{
	CFPos posDst;
	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);
	if(pCharTarget)
	{
		if (m_pCfg->GetTrackType() == eBTT_MarblesSoul && !m_bJump)
		{
			posDst.x = m_posJumpPoint.x;
			posDst.y = m_posJumpPoint.y;
		}
		else
		{
			pCharTarget->GetPixelPos(posDst);
		}
	}
	else
	{
		if (!m_pCoreObj)
		{	
			posDst.x = m_vCurPosition.x;
			posDst.y = m_vCurPosition.z;
		}
		else
			m_pCoreObj->GetDestOfCurPath(posDst);
	}

	return posDst;
}

float CBulletMagicClient::GetCurDistance() const
{
	CFPos posFrom;
	if (m_pCfg->GetTrackType() >= eBTT_Parabola)
	{
		posFrom.x = m_vCurPosition.x;
		posFrom.y = m_vCurPosition.z;
	}
	else
	{
		posFrom = m_pCoreObj->GetPixelPos();
	}
	
	float fDistX = GetDstPixelPos().x - posFrom.x;
	float fDistY = GetDstPixelPos().y - posFrom.y;
	float fDistance = sqrt(fDistX * fDistX + fDistY * fDistY);
	return fDistance;
}

CDir CBulletMagicClient::GetCurDir() const
{
	CDir dir;
	CVector2f vecDir(GetDstPixelPos().x - m_vCurPosition.x, GetDstPixelPos().y - m_vCurPosition.z);
	dir.Set(vecDir);
	return dir;
}

void CBulletMagicClient::PlayMagicFx(const string& strMagicFx)
{
	if (strMagicFx.empty()||!GetMagicFX())
		return;

	CCharacterFollower* pCharacter = CCharacterFollower::GetCharacterByID(m_uOwnerEntityID);


	string sFXFile;
	string sFXName;
	uint32 uDelayTime = 0;
	if (pCharacter)
	{
		pCharacter->GetActor()->SplitSkillFxFile(strMagicFx,sFXFile,sFXName,uDelayTime,false);
	}
	else
	{
		vector<string> sFXTable = CBaseFX::Split(strMagicFx, ",");

		sFXFile	= sFXTable[0]+sFXTable[1]+".efx";
		sFXName	= sFXTable[1];

		if (sFXTable.size() > 2)
		{
			uDelayTime =atoi(sFXTable[2].c_str());
		}
	}
	if (!sFXName.empty())
	{
		GetMagicFX()->PlayLoopFX(sFXFile, sFXName, uDelayTime);
	}
}

void CBulletMagicClient::UpdateTrackOfLine()
{
	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);
	if (pCharTarget)
	{
		if (GetCurDistance() <= 1.0f)
		{
			delete this;
			return;
		}
		CRenderObject* pRenderObj = GetRenderObject();
		Ast(pRenderObj);
		pRenderObj->SetDirection(GetCurDir());

		float fSpeed = m_pCfg->GetPixelSpeed();
		float fFlyingTime = GetCurDistance() * 1000 / fSpeed;
		if (fFlyingTime < 10.0f||!GetRenderObject())
		{
			delete this;
			return;
		}

		CRenderObject* pRenderObjTarget = pCharTarget->GetRenderObject();
		float fTargetRealHeight = pRenderObjTarget->GetCenterPosition().y;
		float fHeightChangeSpeed = (m_vCurPosition.y - fTargetRealHeight) / (fFlyingTime / UPDATE_TRANK_TIME);
		
		float fTan = (fTargetRealHeight - m_vCurPosition.y)/GetCurDistance();
		pRenderObj->Pitch(-atan(fTan));

		CFPos posPixel;
		m_pCoreObj->GetPixelPos(posPixel);
		m_vCurPosition.x = posPixel.x;
		m_vCurPosition.z = posPixel.y;
		m_vCurPosition.y -= fHeightChangeSpeed;
		GetRenderObject()->SetPosition(m_vCurPosition);
	}
}

void CBulletMagicClient::UpdateTrackOfParabola()
{
	CCoreSceneClient* pCoreScene = CCoreSceneClient::MainScene();
	if (!pCoreScene||!GetRenderObject())
	{
		delete this;
		return;
	}

	CFPos posPixel;
	m_pCoreObj->GetPixelPos(posPixel);
	m_vCurPosition.x = posPixel.x;
	m_vCurPosition.z = posPixel.y;

	m_vCurPosition.y = m_vSrcPosition.y + m_fHeightSrcSpeed * (float(m_uPassTime)/1000) + m_fRealGravity * (float(m_uPassTime)/1000) * (float(m_uPassTime)/1000)/2;
	float fCurSpeed = m_fHeightSrcSpeed + m_fRealGravity * m_uPassTime / 1000;

	GetRenderObject()->SetRotate( CVector3f::UNIT_X,-atan(fCurSpeed/m_fRealPixelSpeed) );
	GetRenderObject()->RotateY( float(GetRenderObject()->GetDirection().uDir) * CMath::pi *2.0f/256.0f );

	pCoreScene->GetRenderScene()->AttachRenderObjectAbs(*GetRenderObject(), CVector3f(m_vCurPosition.x, m_vCurPosition.y, m_vCurPosition.z));

	m_uPassTime += UPDATE_TRANK_TIME;
}

void CBulletMagicClient::UpdateTrackOfMarblesSoul()
{
	CCoreSceneClient* pCoreScene = CCoreSceneClient::MainScene();
	if (!pCoreScene||!GetRenderObject())
	{
		delete this;
		return;
	}

	CCharacterFollower* pCharTarget = CCharacterFollower::GetCharacterByID(m_uTargetEntityID);
	float fCurHeightSpeed(0.0f);
	if (!m_bJump)
	{	
		m_vCurPosition.x = m_vSrcPosition.x + float(m_uPassTime)/1000 * m_fCurPlaneSpeed * sin(m_fJumpPointRadian);
		m_vCurPosition.z = m_vSrcPosition.z + float(m_uPassTime)/1000 * m_fCurPlaneSpeed * cos(m_fJumpPointRadian);
		m_vCurPosition.y = m_vSrcPosition.y + m_fHeightSrcSpeed * (float(m_uPassTime)/1000) + m_fRealGravity * (float(m_uPassTime)/1000) * (float(m_uPassTime)/1000)/2;
		fCurHeightSpeed = m_fHeightSrcSpeed + m_fRealGravity * m_uPassTime / 1000;

		if (m_vCurPosition.y <= m_vDstPosition.y && fCurHeightSpeed < 0.0f)
		{
			m_bJump = true;
			CRenderObject* pRenderObject = pCharTarget->GetRenderObject();
			m_vDstPosition.y = pRenderObject->GetCenterPosition().y;
		}
	}
	else
	{
		float fCufDistance = GetCurDistance();
		if (fCufDistance < 10 || fCufDistance > 1000)
		{
			if (fCufDistance < 10)
			{
				vector<string> sFXTable = CBaseFX::Split(m_pCfg->GetFinalFX(), ",");
				string sFXFile	= sFXTable[0]+sFXTable[1]+".efx";
				string sFXName	= sFXTable[1];
				CPlayerFX::PlayOnceFX(pCharTarget, sFXFile, sFXName);
			}

			delete this;
			return;
		}

		float fTickTime = float(UPDATE_TRANK_TIME)/1000;
		float fCurRadian = float(TWO_PI * GetCurDir().uDir / 256);
		m_fCurPlaneSpeed += fTickTime * m_fPlaneAcc;
		m_vCurPosition.x += fTickTime * m_fCurPlaneSpeed * sin(fCurRadian);
		m_vCurPosition.z += fTickTime * m_fCurPlaneSpeed * cos(fCurRadian);

		CRenderObject* pRenderObjectDst = pCharTarget->GetRenderObject();
		m_vDstPosition = pRenderObjectDst->GetSkeletalPositionByName("Bip01 Spine");

		float fFlyTime = sqrt(2*fCufDistance/m_fPlaneAcc+(m_fCurPlaneSpeed*m_fCurPlaneSpeed)/(m_fPlaneAcc*m_fPlaneAcc))-m_fCurPlaneSpeed/m_fPlaneAcc;
		fCurHeightSpeed = (m_vDstPosition.y - m_vCurPosition.y - m_fRealGravity * fFlyTime * fFlyTime/2) / fFlyTime;
		m_vCurPosition.y += fTickTime * fCurHeightSpeed;

		GetRenderObject()->SetDirection(GetCurDir());
	}
	GetRenderObject()->SetRotate( CVector3f::UNIT_X, -atan(fCurHeightSpeed/m_fRealPixelSpeed));
	GetRenderObject()->RotateY( (GetRenderObject()->GetDirection().uDir)* CMath::pi *2/256 );
	pCoreScene->GetRenderScene()->AttachRenderObjectAbs(*GetRenderObject(), CVector3f(m_vCurPosition.x, m_vCurPosition.y, m_vCurPosition.z));
	m_uPassTime += UPDATE_TRANK_TIME;
}
