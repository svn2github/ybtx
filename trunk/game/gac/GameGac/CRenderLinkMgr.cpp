#include "stdafx.h"
#include "CRenderLinkMgr.h"
#include "CRenderObject.h"
#include "StringHelper.h"
#include "CCharacterFollower.h"
#include "CRenderSystemClient.h"
#include "CoreCommon.h"
#include "CRenderTime.h"
#include "TCharacterAdapterClient.h"
#include "ICharacterDirectorHandler.h"
#include "ICharacterFollowerHandler.h"
#include "IActorFollowerHandler.h"
#include "IActorDirectorHandler.h"
#include "IFighterDirectorHandler.h"
#include "CCharacterDirector.h"
#include "IRenderObjClientHandler.h"
#include "CSoundPlayer.h"

class CHorseRenderHandler : public IModelHandler
{
public:
	CHorseRenderHandler(CRenderLinkMgr* pMgr)
		:m_pMgr(pMgr)
	{

	}
private:
	void OnKeyFrame( const char * szName )
	{
		string strFrameName = szName;
		if ( strFrameName.find("f") != -1 )
		{
			string strCueNameTail = strFrameName.substr(1);
			CSoundPlayer::PlayFootstep(m_pMgr->m_pCharacter, strCueNameTail);
		}
	}
	const CRenderLinkMgr* m_pMgr;
};

class CWeaponRenderHandler : public IModelHandler
{
public:
	CWeaponRenderHandler(CRenderLinkMgr* pMgr)
		:m_pMgr(pMgr)
	{

	}
private:
	void OnResLoaded()
	{
		m_pMgr->m_pCharacter->GetRenderLinkMgr()->PlayWeaponAnimation("主手武器");
	}
	void OnAnimationEnd( const char * szName )
	{
		if (strcmp(szName, "change01") == 0)
		{
			m_pMgr->m_pCharacter->GetRenderLinkMgr()->PlayWeaponAnimation("主手武器",2);
		}
	}
	const CRenderLinkMgr* m_pMgr;
};

class COffWeaponRenderHandler : public IModelHandler
{
public:
	COffWeaponRenderHandler(CRenderLinkMgr* pMgr)
		:m_pMgr(pMgr)
	{

	}
private:
	void OnResLoaded()
	{
		m_pMgr->m_pCharacter->GetRenderLinkMgr()->PlayWeaponAnimation("副手武器");
	}
	void OnAnimationEnd( const char * szName )
	{
		if (strcmp(szName, "change01") == 0)
		{
			m_pMgr->m_pCharacter->GetRenderLinkMgr()->PlayWeaponAnimation("副手武器",2);
		}
	}
	const CRenderLinkMgr* m_pMgr;
};

CRenderLinkMgr::CRenderLinkMgr(CCharacterFollower* pCharacter)
: m_pCharacter(pCharacter)
, m_uHorseEffect(0)
, m_uMainHandEffect(0)
, m_uOffHandEffect(0)
, m_uAdditionEffect(0)
, m_eWeaponPlayType(eWPT_None)
, m_eOffWeaponPlayType(eWPT_None)
, m_bWeaponIsOpen(false)
{
	m_pWeaponChangeAniTick = new CWeaponChangeAniTick(this);
	m_pWeaponResumeAniTick = new CWeaponResumeAniTick(this);
	m_pHorseRenderHandler = new CHorseRenderHandler(this);
	m_pWeaponRenderHandler = new CWeaponRenderHandler(this);
	m_pOffWeaponRenderHandler = new COffWeaponRenderHandler(this);
	InitRenderObject();
	UpdatePos(m_pCharacter->GetPixelPos());
}

CRenderLinkMgr::~CRenderLinkMgr()
{
	RemoveRenderObject();
	SafeDelete(m_pWeaponChangeAniTick);
	SafeDelete(m_pWeaponResumeAniTick);
	SafeDelete(m_pWeaponRenderHandler);
	SafeDelete(m_pOffWeaponRenderHandler);
	SafeDelete(m_pHorseRenderHandler);
}

void CRenderLinkMgr::InitRenderObject()
{
	m_pHorseRenderObj = CClientRenderScene::GetInst()->CreateRenderObject(new CRenderTime(m_pCharacter->GetDistortedTime()));
	m_pHorseRenderObj->SetModelHandler(m_pHorseRenderHandler);
	CRenderObject* pRenderObj = m_pHorseRenderObj->CreateRenderObject(LT_SKELETAL, "zw01");
	m_pCharacter->m_pRenderObj =	pRenderObj;
	if (m_pCharacter->CastToCharacterDirector())
	{
		pRenderObj->SetModelHandler(class_cast<CCharacterDirectorAdapter*>(m_pCharacter->GetActor()->GetHandler()));
		pRenderObj->SetNodeHandler(class_cast<CCharacterDirectorAdapter*>(m_pCharacter->GetActor()->GetHandler()));
	} 
	else
	{
		pRenderObj->SetModelHandler(class_cast<CCharacterFollowerAdapter*>(m_pCharacter->GetActor()->GetHandler()));
		pRenderObj->SetNodeHandler(class_cast<CCharacterFollowerAdapter*>(m_pCharacter->GetActor()->GetHandler()));
	}
	m_pMainHandRenderObj =  pRenderObj->CreateRenderObject(LT_SKELETAL, "Bone02");
	m_pMainHandRenderObj->SetModelHandler(m_pWeaponRenderHandler);
	m_pOffHandRenderObj =  pRenderObj->CreateRenderObject(LT_SKELETAL, "Bone10");
	m_pOffHandRenderObj->SetModelHandler(m_pOffWeaponRenderHandler);
	m_pAdditionRenderObj =  pRenderObj->CreateRenderObject(LT_SKELETAL, "Bone12");


}

void CRenderLinkMgr::RemoveRenderObject()
{
	CRenderObject* pRenderObj = m_pCharacter->m_pRenderObj;
	//pRenderObj->DestroyRenderObject(m_pAdditionRenderObj);
	//pRenderObj->DestroyRenderObject(m_pOffHandRenderObj);
	//pRenderObj->DestroyRenderObject(m_pMainHandRenderObj);
	if (	CClientRenderScene::HasInst())
	{
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		if (pRenderScene->GetSelectedObj()==m_pCharacter->m_pRenderObj)
		{
			pRenderScene->SetSelectedObj(NULL);
		}
		m_pMainHandRenderObj->SetModelHandler(NULL);
		m_pOffHandRenderObj->SetModelHandler(NULL);
		m_pCharacter->m_pRenderObj->SetModelHandler(NULL);
		m_pCharacter->m_pRenderObj->SetModelHandler(NULL);
		m_pCharacter->m_pRenderObj->SetNodeHandler(NULL);
		//m_pHorseRenderObj->DestroyRenderObject(m_pCharacter->m_pRenderObj);
		m_pHorseRenderObj->SetModelHandler(NULL);
		pRenderScene->DestroyRenderObject(m_pHorseRenderObj);
	}
	m_pCharacter->m_pRenderObj =	NULL;
	m_pHorseRenderObj=NULL;
	m_pMainHandRenderObj=NULL;
	m_pOffHandRenderObj=NULL;
}

void CRenderLinkMgr::OnRenderObjDestroyed()
{
	m_pCharacter->m_pRenderObj=NULL;
}

float CRenderLinkMgr::GetPairScale()
{
	return m_pHorseRenderObj->GetScale()/**m_pCharacter->GetRenderObject()->GetSize()*/;
}

float CRenderLinkMgr::GetScale()
{
	return m_pHorseRenderObj->GetScale();
}

float CRenderLinkMgr::GetFinalScale()
	{
	return m_pHorseRenderObj->GetFinalScale();
	}

void CRenderLinkMgr::SetScale( float fScale )
{
	m_pHorseRenderObj->SetScale(fScale);
}

void CRenderLinkMgr::SetFinalScale( float fScale, uint64 uFinalTime )
{
	m_pHorseRenderObj->SetFinalScale(fScale,uFinalTime);
}

void CRenderLinkMgr::UpdatePos(const CFPos& PixelPos)
{
	float fRatio = eGridSpanForObj / eGridSpan;
	Ast( CClientRenderScene::HasInst());
	CRenderScene* pRenderScene = CClientRenderScene::GetInst();
	pRenderScene->SetRenderObjectPosition(*m_pHorseRenderObj,CVector3f(PixelPos.x / fRatio, 0, PixelPos.y / fRatio),m_pCharacter->CanBeOnTheTerrain());
}

void	CRenderLinkMgr::SetWeaponInfo(const string& strMainHandLinkName, const string& strOffHandLinkName,const string& strMainHandLinkEffect,const string& strOffHandLinkEffect)
{
	m_strMainHandEffect = strMainHandLinkEffect;
	m_strOffHandEffect = strOffHandLinkEffect;
	CRenderObject* pRenderObj = m_pCharacter->m_pRenderObj;
	if (!strMainHandLinkName.empty())
	{
		pRenderObj->AddChild(m_pMainHandRenderObj,LT_SKELETAL,strMainHandLinkName.c_str());
	}
	if (!strOffHandLinkName.empty())
	{
		pRenderObj->AddChild(m_pOffHandRenderObj,LT_SKELETAL,strOffHandLinkName.c_str());
	}
}

void CRenderLinkMgr::SetHorseEffect(const string& strHorseEffect)
{
	m_strHorseEffect = strHorseEffect;
}

void CRenderLinkMgr::SetAdditionEffect(const string& strAdditionEffect)
{
	m_strAdditionEffect = strAdditionEffect;
}

void CRenderLinkMgr::PlayLinkRenderObjEffect(const string& strName,const string& AniName,bool bPlay,bool bLoop)
{
	if (bPlay)
	{
		vector<string> vecEffect;
		if (strName=="主手武器")
		{
			if (!m_strMainHandEffect.empty())
			{
				split(m_strMainHandEffect,string(","),vecEffect);
				m_uMainHandEffect = m_pMainHandRenderObj->AddEffect(vecEffect[0].c_str(),(AniName+"/create").c_str(), bLoop?-1:0,NULL);
			}
			
		}
		else if (strName=="副手武器")
		{
			if (!m_strOffHandEffect.empty())
			{
				split(m_strOffHandEffect,string(","),vecEffect);
				m_uOffHandEffect =m_pOffHandRenderObj->AddEffect(vecEffect[0].c_str(),(AniName+"/create").c_str(),bLoop?-1:0,NULL);
			}
		}
		else if (strName=="坐骑")
		{
			if (!m_strHorseEffect.empty())
			{
				split(m_strHorseEffect,string(","),vecEffect);
				m_uHorseEffect =m_pHorseRenderObj->AddEffect(vecEffect[0].c_str(),(AniName+"/create").c_str(),bLoop?-1:0,NULL);
			}
		}
		else
		{
			if (!m_strAdditionEffect.empty())
			{
				split(m_strAdditionEffect,string(","),vecEffect);
				m_uAdditionEffect =m_pAdditionRenderObj->AddEffect(vecEffect[0].c_str(),(AniName+"/create").c_str(),bLoop?-1:0,NULL);
			}
		}
	}
	else
	{
		if (strName=="主手武器")
		{
			m_pMainHandRenderObj->DelEffect(m_uMainHandEffect);
			m_uMainHandEffect = 0;
		}
		else if (strName=="副手武器")
		{
			m_pOffHandRenderObj->DelEffect(m_uOffHandEffect);
			m_uOffHandEffect = 0;
		}
		else if (strName=="坐骑")
		{
			m_pHorseRenderObj->DelEffect(m_uHorseEffect);
			m_uHorseEffect = 0;
		}
		else
		{
			m_pAdditionRenderObj->DelEffect(m_uAdditionEffect);
			m_uAdditionEffect = 0;
		}
	}
}

// 变形武器 BEGIN
void CRenderLinkMgr::PlayWeaponAnimation(string strWeapon, uint8 uType)
{
	EWeaponPlayType eWeaponPlayType = eWPT_None;
	CRenderObject* pRenderWeapon =NULL;
	if (strWeapon == "主手武器")
	{
		eWeaponPlayType = m_eWeaponPlayType;
		pRenderWeapon = GetMainHandRenderObj();
	}
	else if (strWeapon == "副手武器")
	{
		eWeaponPlayType = m_eOffWeaponPlayType;
		pRenderWeapon = GetOffHandRenderObj();
	}

	if (eWeaponPlayType == eWPT_None)
	{
		PlayLinkRenderObjEffect(strWeapon,"",false,false);
		return;
	}

	if (!pRenderWeapon)
		return;

	if (eWeaponPlayType == eWPT_Always)
	{
		pRenderWeapon->DoAni("change01", true);
	}
	else if (eWeaponPlayType == eWPT_BattleChange)
	{
		switch (uType)
		{
		case 0:
			{
				pRenderWeapon->DoAni("stand01", true);
				PlayLinkRenderObjEffect(strWeapon,"stand01",false,true);
				m_bWeaponIsOpen = false;
			}
			break;
		case 1:
			{
				m_pWeaponChangeAniTick->UnRegisterTick();

				pRenderWeapon->DoAni("change01", false);
				PlayLinkRenderObjEffect(strWeapon, "change01", true, false);
			}
			break;
		case 2:
			{
				pRenderWeapon->DoAni("stand02", true);
				PlayLinkRenderObjEffect(strWeapon,"stand02",true,true);
			}
			break;
		default:
			break;
		}
	}
}

void CRenderLinkMgr::SetWeaponPlayType(EWeaponPlayType eWeaponPlayType)
{
	m_eWeaponPlayType = eWeaponPlayType;
	PlayWeaponAnimation("主手武器");
}

void CRenderLinkMgr::SetOffWeaponPlayType(EWeaponPlayType eWeaponPlayType)
{
	m_eOffWeaponPlayType = eWeaponPlayType;
	PlayWeaponAnimation("副手武器");
}

void CRenderLinkMgr::SetWeaponVisible(bool bIsMainHand, bool bVisible)
{
	if (bIsMainHand)
	{
		CRenderObject* pRenderWeapon = GetMainHandRenderObj();
		if (bVisible)
		{
			if (m_eWeaponPlayType == eWPT_None)
			m_pCharacter->GetRenderObject()->ShowPiece("weapon", true);
			else
				pRenderWeapon->SetVisible(true);
		}
		else
		{
			if (m_eWeaponPlayType == eWPT_None)
			m_pCharacter->GetRenderObject()->ShowPiece("weapon", false);
			else
				pRenderWeapon->SetVisible(false);
		}
	}
	else
	{
		CRenderObject* pRenderOffWeapon = GetOffHandRenderObj();
		if (bVisible)
		{
	 		if (m_eOffWeaponPlayType == eWPT_None)
	 			m_pCharacter->GetRenderObject()->ShowPiece("offweapon", true);
	 		else
	 			pRenderOffWeapon->SetVisible(true);
		}
		else
		{
	 		if (m_eOffWeaponPlayType == eWPT_None)
	 			m_pCharacter->GetRenderObject()->ShowPiece("offweapon", false);
	 		else
	 			pRenderOffWeapon->SetVisible(false);
		}
	}
}

bool CRenderLinkMgr::WeaponPlayTypeIsBattleChange()
{
	return (m_eWeaponPlayType == eWPT_BattleChange || m_eOffWeaponPlayType == eWPT_BattleChange);
}

void CRenderLinkMgr::DoWeaponChangeAni(bool bChange)
{
	if (bChange)
	{
		if (!m_bWeaponIsOpen)
		{
			m_pWeaponChangeAniTick->RegisterTick();
			m_bWeaponIsOpen = true;
		}
		else if (m_pCharacter->CppGetCtrlState(eFCS_InBattle))
			m_pWeaponResumeAniTick->UnRegisterTick();
	}
	else
	{
		m_pWeaponResumeAniTick->RegisterTick();
	}

	m_pCharacter->SetIsCtrlAll(!bChange);
}

void CRenderLinkMgr::DoWeaponAnimation(uint8 uType)
{
	if (!WeaponPlayTypeIsBattleChange())
		return;

	if (m_eWeaponPlayType == eWPT_BattleChange)
		PlayWeaponAnimation("主手武器", uType);

	if (m_eOffWeaponPlayType == eWPT_BattleChange)
		PlayWeaponAnimation("副手武器", uType);
}


// 变形武器 END
