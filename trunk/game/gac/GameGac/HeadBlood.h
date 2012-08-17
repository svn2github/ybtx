#pragma once

#include "CRenderDialog.h"
#include "SQRProgress.h"

enum EBloodDialogType
{
	eBDT_Normal,
	eBDT_GAttack,
	eBDT_SuperNpc,
	eBDT_RingLeader,
	eBDT_Leader,
	eBDT_Boss,
};


class HeadBlood: public sqr::CRenderDialog
{
public:
	HeadBlood();
	~HeadBlood();
	void CreateBloodRendlerDialog(SQRWnd* ParentWnd, bool bShow, EBloodDialogType eBDType);
	void UpdateInfo(uint32 HealthPoint, uint32 HealthPointAgile, bool bIsFeignDeath);
	void SetHeadBloodTex(EBloodDialogType eBDType,bool bIsEnemy);
private:
	SQRProgress* m_BloodProgress;
};
