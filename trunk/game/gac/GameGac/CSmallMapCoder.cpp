#include "stdafx.h"
#include "CSmallMapCoder.h"
#include "CRenderSystemClient.h"
#include "CSmallMap.h"
#include "StringHelper.h"
#include "CDataScene.h"
#include "CGraphic.h"

bool CSmallMapCoder::_CElementDeCode( const DeCodeParams& Params )
{
	CSmallMapTex *pSmallTex = (CSmallMapTex*)(Params.m_pNode);
	string FileName;
	format(FileName,"%s/MinMap/M_%u_%u.dds",CClientRenderScene::GetInst()->GetDataScene()->GetSceneName(),
		pSmallTex->GetXRegion(),pSmallTex->GetYRegion());
	CGraphic::GetInst()->CreateTexture( PATH_ALIAS_RES.c_str(), FileName.c_str(), &(pSmallTex->m_MapText) );		
	pSmallTex->SetValid(true);
	return true;
}
