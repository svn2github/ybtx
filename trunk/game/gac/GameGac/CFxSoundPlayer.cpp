//------------------------------------------------------------------------------
//  CFxSoundPlayer.cpp
//  (C) 2010 ThreeOGCMan
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "CFxSoundPlayer.h"
#include "IAudioModule.h"
#include "Algorithm.h"
#include "CSoundPlayer.h"
#include "CFighterFollower.h"
#include "CCharacterFollower.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CSyncVariantClient.h"
#include "CEntityClientManager.h"

//------------------------------------------------------------------------------
CFxSoundPlayer::CFxSoundPlayer()
{
	this->Reset();
}

//------------------------------------------------------------------------------
bool
CFxSoundPlayer::RenderCallback( IEffectUnit* pUnit, CMatrix& matWorld, uint32 uCurTime, RenderObjStyle* pRORS )
{
	if (!m_bHasPlayed)
	{
		if (!pUnit->IsInRenderTime())
			return false;

		const CVector3f pos = matWorld.GetLoc();
		const static CVector3f nullVel;

		string strCueName = GetCueName();
		if (strCueName != "")
		{
			Play3DCue(strCueName.c_str(), pos, nullVel);
			m_bHasPlayed = true;
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------
CWhiskSoundPlayer::CWhiskSoundPlayer(uint32 uEntityID)
:m_uEntityID(uEntityID)
{
	m_strUnitName = "whisk";
}

//------------------------------------------------------------------------------
string
CWhiskSoundPlayer::GetCueName() const
{
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(m_uEntityID);
	if (!pCharacter)
		return "";

	CFighterFollower* pTarget = pCharacter->GetFighter()->GetTarget();
	if (!pTarget)
		return "";

	string strArmor = "";
	if (pTarget->GetCharacter()->GetIsPlayer())
	{
		strArmor = pTarget->GetCharacter()->GetCallbackHandler()->GetArmorTypeString(pTarget->GetCharacter()->GetEntityID());
	}
	else
	{
		string strNpcName = pTarget->GetCharacter()->GetViewVariant()->GetMember("Attribute")->GetMember("CharName")->GetString();
		string strName = pTarget->GetCharacter()->GetCallbackHandler()->GetNpcAniFileName(strNpcName.c_str());
		CSoundConfig* pSoundCfg = CSoundConfig::GetSoundCfg(strName);
		if (pSoundCfg)
			strArmor = pSoundCfg->GetArmorType();
	}
	pCharacter->GetActor()->SetTargetArmorString(strArmor);

	return CSoundPlayer::GetWhiskCueName(pCharacter, m_eActionState);
}

//------------------------------------------------------------------------------
CHitSoundPlayer::CHitSoundPlayer(uint32 uEntityID)
:m_uEntityID(uEntityID)
{
	m_strUnitName = "hit";
}

//------------------------------------------------------------------------------
string
CHitSoundPlayer::GetCueName() const
{
	CCharacterFollower* pCharacter = CEntityClientManager::GetInst()->GetCharacter(m_uEntityID);
	if (!pCharacter)
		return "";

	return CSoundPlayer::GetHitCueName(pCharacter, m_eActionState);
}
