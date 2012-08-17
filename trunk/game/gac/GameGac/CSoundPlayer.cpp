#include "stdafx.h"
#include "CSoundPlayer.h"
#include "CCharacterDirector.h"
#include "CFighterDirector.h"
#include "CNormalAttackClient.h"
#include "CSyncVariantClient.h"
#include "IAudioModule.h"
#include "ICharacterFollowerCallbackHandler.h"
#include "CRenderObject.h"

void CSoundPlayer::PlayYelpByType(CCharacterFollower* pCharacter, ESoundTypeName eSoundTypeName)
{
	string strCueName = GetYelpCueName(pCharacter, eSoundTypeName);
	if (strCueName.empty())
		return;

	Play3DSoundByName(pCharacter, strCueName);
}

void CSoundPlayer::Play3DSoundByName(CCharacterFollower* pAttacker, string strSoundName)
{
	if (strSoundName.empty())
		return;

	CRenderObject* pRenderObject = pAttacker->GetRenderObject();
	if (NULL == pRenderObject)
		return;
	CVector3f pos = pRenderObject->GetCenterPosition();
	const static CVector3f nullVel;

	Play3DCue(strSoundName.c_str(), pos, nullVel);
}


string CSoundPlayer::GetYelpCueName(CCharacterFollower* pCharacter, ESoundTypeName eSoundTypeName)
{
	string strName = GetNameByCharacter(pCharacter);
	if (strName.empty())
		return "";

	CSoundConfig* pSoundCfg = CSoundConfig::GetSoundCfg(strName);
	if (!pSoundCfg)
		return "";

	string strCueName = "";
	switch (eSoundTypeName)
	{
	case eSTN_EnterBattle:
		strCueName = pSoundCfg->GetEnterBattleSound();
		break;
	case eSTN_Suffer:
		strCueName = pSoundCfg->GetSufferSound();
		break;
	case eSTN_Die:
		strCueName = pSoundCfg->GetDieSound();
		break;
	default:
		break;
	}

	return strCueName;
}

string CSoundPlayer::GetWhiskCueName(CCharacterFollower* pAttacker, EActionState eActionState)
{
	bool bIsMainHandWeapon = (eAS_Attack_Assist == eActionState) ? false : true;	
	string strWhiskCueName = "";
	if (pAttacker->GetIsPlayer())
	{
		strWhiskCueName = pAttacker->GetCallbackHandler()->GetWeaponTypeString(pAttacker->GetEntityID(), bIsMainHandWeapon);
	}
	else
	{
		string strNpcName = pAttacker->GetViewVariant()->GetMember("Attribute")->GetMember("CharName")->GetString();
		string strName = pAttacker->GetCallbackHandler()->GetNpcAniFileName(strNpcName.c_str());
		CSoundConfig* pSoundCfg = CSoundConfig::GetSoundCfg(strName);
		if (!pSoundCfg)
			return "";
		strWhiskCueName = pSoundCfg->GetWeaponType();
	}

	return strWhiskCueName;
}

string CSoundPlayer::GetOtherHitNmae(EHurtResult eHurtResult)
{
	string strHitCueName;
	switch (eHurtResult)
	{
	case eHR_PhyDodge:
	case eHR_MagDodge:
		strHitCueName = "escap";
		break;
	case eHR_Immune:
		strHitCueName = "insulation";
		break;
	case eHR_ComResist:
		strHitCueName = "magic_resist";
		break;
	case eHR_Parry:
		strHitCueName = "physical_wardoff";
		break;
	case eHR_Block:
		strHitCueName = "sheild_block";
		break;
	default:
		strHitCueName = "";
		break;
	}
	return strHitCueName;
}

string CSoundPlayer::GetHitCueName(CCharacterFollower* pAttacker, EActionState eActionState)
{
	EHurtResult eHurtResult = pAttacker->GetActor()->GetHurtResult();
	if (eHurtResult != eHR_Hit && eHurtResult != eHR_Strike && eHurtResult != eHR_Resist)
		return GetOtherHitNmae(eHurtResult);

	string strHitCueName = GetWhiskCueName(pAttacker, eActionState);
	if (strHitCueName == "")
		return "";
	
	//strHitCueName += (eActionState == eAS_Strike) ? "_bang_" : "_hit_";
	strHitCueName += "_hit_";	// 暂时让暴击命中声同普通命中
	
	string strArmor = pAttacker->GetActor()->GetTargetArmorString();
	if (strArmor == "")
		return "";
	else
		strHitCueName += strArmor;

	return strHitCueName;
}

string CSoundPlayer::GetNameByCharacter(CCharacterFollower* pCharacter)
{
	string strName;
	if (pCharacter->GetIsPlayer())
	{
		switch (pCharacter->CppGetClass())
		{
		case eCL_Warrior:
		case eCL_MagicWarrior:
		case eCL_Paladin:
		case eCL_NatureElf:
		case eCL_EvilElf:
		case eCL_Priest:
			strName = "人类";
			break;
		case eCL_DwarfPaladin:
			strName = "人类";
			break;
		case eCL_ElfHunter:
			strName = "人类";
			break;
		case eCL_OrcWarrior:
			strName = "兽人";
			break;
		}
		uint32 uSex = pCharacter->GetViewVariant()->GetMember("Attribute")->GetMember("Sex")->GetNumber<uint8>();
		if (uSex == 1)
			strName += "男";
		else
			strName += "女";
		return strName;
	}
	else
	{
		string strNpcName = pCharacter->GetViewVariant()->GetMember("Attribute")->GetMember("CharName")->GetString();
		strName = pCharacter->GetCallbackHandler()->GetNpcAniFileName(strNpcName.c_str());
		return strName;
	}
}

//------------------------------------------------------------------------------
void
CSoundPlayer::PlayFootstep(CCharacterFollower* pCharacter, string strCueNameTail)
{
	if (NULL == pCharacter)
		return;

	CRenderObject* pRenderObject = pCharacter->GetRenderObject();
	if (NULL == pRenderObject)
		return;

	string strCueName(""), strName("");

	if (pCharacter->CppGetCtrlState(eFCS_InNormalHorse) || pCharacter->CppGetCtrlState(eFCS_InBattleHorse))
	{
		strName = pCharacter->GetHorseName();
	}
	else
	{
		strName = GetNameByCharacter(pCharacter);
		if (strName.empty())
			return;
		
		if (!strCueNameTail.empty())
			strCueNameTail = "";
	}
	CSoundConfig* pSoundCfg = CSoundConfig::GetSoundCfg(strName);
	if (!pSoundCfg)
		return;
	strCueName = pSoundCfg->GetFootstepSound();
	if (strCueName.empty())
		return;

	int32 uMeterialID = pCharacter->GetHorseRenderObj()->GetTerrainMaterialIndex();
	string strMeterial = CSoundConfig::GetMeterialString(uMeterialID);

	if (strMeterial.empty())
		return;
	strCueName = strMeterial + "_" + strCueName;

	if (strCueNameTail != "")
		strCueName = strCueName + strCueNameTail;

	CVector3f pos = pRenderObject->GetCenterPosition();
	const static CVector3f nullVel;

	Play3DCue(strCueName.c_str(), pos, nullVel);
}
