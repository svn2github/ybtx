#include "stdafx.h"
#include "CMagicClient.h"
#include "CoreCommon.h"
#include "FXPlayer.h"
#include "CCoreObjectFollower.h"
#include "CRenderSystemClient.h"
#include "PatternRef.inl"

CMagicClient::CMagicClient(CCoreObjectFollower* pCoreObj)
:m_pCoreObj(pCoreObj)
{
	m_MagicFX.SetHolder(this);
	CMagicFX* pMagicFX = new CMagicFX();
	m_MagicFX.Attach(pMagicFX->GetRefByMagic());
	if (pCoreObj)
	{
		pCoreObj->SetViewVariantHandler(this,eCTI_Static);
		OnRefreshRenderObjPos(m_pCoreObj->GetPixelPos());
	}
}

CMagicClient::~CMagicClient()
{
	if (m_pCoreObj)
	{
		m_pCoreObj->SetViewVariantHandler(NULL,eCTI_Static);
	}
	if (GetMagicFX())
	{
		GetMagicFX()->CancelFx();
	}
}

CRenderObject* CMagicClient::GetRenderObject()const
{
	if (GetMagicFX())
	{
		return GetMagicFX()->GetRenderObj();
	}
	return NULL;
}

CMagicFX* CMagicClient::GetMagicFX()const
{
	return m_MagicFX.Get();
}

void CMagicClient::PlayMagicFx(const string& strMagicFx)
{
	vector<string> sFXTable = CBaseFX::Split(strMagicFx, ",");
	string sFXFile	= sFXTable[0]+sFXTable[1]+".efx";
	string sFXName	= sFXTable[1];
	uint32 uDelayTime = 0;
	if (sFXTable.size()>2)
	{
		uDelayTime=atoi(sFXTable[2].c_str());
	}
	
	if (!sFXFile.empty())
	{
		GetMagicFX()->PlayLoopFX(sFXFile, sFXName,uDelayTime);
	}
}

void CMagicClient::OnDestroy()
{
	m_pCoreObj->SetHandler(NULL);
	delete this;
}

void CMagicClient::OnRefreshRenderObjPos(const CFPos& PixelPos)
{
	if(GetRenderObject())
	{
		float fRatio = eGridSpanForObj / eGridSpan;
		Ast( CClientRenderScene::HasInst());
		CRenderScene* pRenderScene = CClientRenderScene::GetInst();
		pRenderScene->SetRenderObjectPosition(*GetRenderObject(),CVector3f(PixelPos.x / fRatio, 0, PixelPos.y / fRatio),true);
	}
}