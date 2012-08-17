#include "stdafx.h"
#include "CClientFighterMsgDispatcher.h"
#include "CCoreObjectFollower.h"
#include "CBulletMagicClient.h"
#include "CPositionMagicClient.h"
#include "CShockWaveMagicClient.h"
#include "CBarrierMagicClient.h"
#include "CTransferableMagicClient.h"
#include "FxPlayer.h"
#include "CBattleArrayMagicClient.h"
#include "CodeCvs.h"
#include "CCharacterFollower.h"
#include "CRenderObject.h"
#include "CQuickRand.h"

void CClientFighterMsgDispatcher::OnMagicCommand(const CGas2GacCPP_OnBurstSoul* pCmd, void *pParam)
{
	CCharacterFollower* pCharFrom = CCharacterFollower::GetCharacterByID(pCmd->uFromEntityID);
	if (!pCharFrom)
		return;

	CFPos posFrom, posDst;
	posFrom = pCharFrom->GetPixelPos();

	float fDistance(0.0f), fRadian(0.0f);
	uint8 uCurDir(0), uRandDir(0);
	uint32 uSoulNum = pCmd->uBurstSoulCount;
	for (uint32 i = 0; i < uSoulNum; ++i)
	{
		fDistance = CQuickRand::Rand1(100.0f, 200.0f);
		uRandDir = CQuickRand::Rand1(uint8(0), uint8(128/uSoulNum));
		fRadian = float(TWO_PI * (uCurDir + uRandDir) / 256);
		posDst.x = posFrom.x + fDistance * sin(fRadian);
		posDst.y = posFrom.y + fDistance * cos(fRadian);
		uCurDir += 256/uSoulNum;

		const CBulletMagicCfgClientSharedPtr& pCfg = CBulletMagicCfgClient::GetByID(pCmd->uCategoryID);
		CBulletMagicClient* pMagicClient = new CBulletMagicClient(NULL, pCmd->uFromEntityID, pCmd->uTargetEntityID, pCfg, posDst, (EBurstSoulType)pCmd->uBurstSoulType);
		pMagicClient->PlayMagicFx(pCfg->GetFX());
	}
}
