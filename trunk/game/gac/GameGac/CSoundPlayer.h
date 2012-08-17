#pragma once
#include "CSoundConfig.h"
#include "ActorDef.h"
#include "FightDef.h"

class CCharacterFollower;
class CFighterFollower;

class CSoundPlayer
{
public:
	
	static string GetYelpCueName(CCharacterFollower* pCharacter, ESoundTypeName eSoundTypeName);
	static string GetWhiskCueName(CCharacterFollower* pAttacker, EActionState eActionState);
	static string GetHitCueName(CCharacterFollower* pAttacker, EActionState eActionState);
	static string GetOtherHitNmae(EHurtResult eHurtResult);

	static void PlayYelpByType(CCharacterFollower* pCharacter, ESoundTypeName eSoundTypeName);
	static void Play3DSoundByName(CCharacterFollower* pAttacker, string strSoundName);
	static void PlayFootstep(CCharacterFollower* pCharacter, string strCueNameTail);
private:
	static string GetNameByCharacter(CCharacterFollower* pCharacter);
};