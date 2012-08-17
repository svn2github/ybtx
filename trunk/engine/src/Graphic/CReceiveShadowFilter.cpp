#include "StdAfx.h"
#include "CReceiveShadowFilter.h"
#include "CFilterNode.h"
#include "CShadowFilter.h"
#include "CRenderPipeline.h"
#include "CRenderTexture.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "CRenderGroup.h"

//------------------------------------------------------------------------------
CReceiveShadowFilter::CReceiveShadowFilter(ERenderObjStyle ros, CRenderGroup* pRenderGroup , CShadowFilter* pShadowFilter)
		: CDefaultFilter(ros,pRenderGroup)
		, m_pShadowFilter(pShadowFilter)
{
}

//------------------------------------------------------------------------------
CReceiveShadowFilter::~CReceiveShadowFilter()
{}

//------------------------------------------------------------------------------
FilterNode* CReceiveShadowFilter::Filter( FilterNode* fn )
{	
	m_pShadowFilter->SetShadowParams(fn->m_RP.m_RS);
	return CDefaultFilter::Filter(fn);
}

//------------------------------------------------------------------------------
void CReceiveShadowFilter::Render()
{
	m_pShadowFilter->UpdateShadowParams();
	CDefaultFilter::Render();
}


CUnReceiveShadowFilter::CUnReceiveShadowFilter( ERenderObjStyle eRos,CRenderGroup* pRenderGroup )
: CDefaultFilter(eRos, pRenderGroup)
{
}

FilterNode* CUnReceiveShadowFilter::Filter( FilterNode* fn )
{
	SHADER_HANDLE& sh = fn->m_RP.m_RS.m_ShaderType;

	DWORD VS = sh.GetVSShaderID();
	DWORD PS = sh.GetPSShaderID();
	DWORD AVS =  fn->m_RP.m_RS.m_bPreLight?SSI_PRE_RENDER_LIGHT:0;
	VS = VS & ~MakeSysVS_ID(SSI_SHADOW_MAP|AVS);
	PS = PS & ~MakeSysPS_ID(SSI_SHADOW_MAP);
	sh.Clear();

	if ( VS!=0 ) 
		sh |= MakeSysVS_ID(VS|AVS);
	if( PS!=0 )
		sh |= MakeSysPS_ID(PS);

	return CDefaultFilter::Filter(fn);
}