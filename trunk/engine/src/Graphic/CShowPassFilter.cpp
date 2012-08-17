#include "stdafx.h"
#include "CShowPassFilter.h"
#include "CRenderPipeline.h"
#include "CGpuProgramMgr.h"
#include "CRenderGroup.h"
#include "CLightSystem.h"

CShowPassFilter::CShowPassFilter(CRenderGroup* pGroup)
: m_pParentGroup(pGroup)
{
	AddRef();

	m_ShadowStencil.m_nPassFlag = STENCILOP_REPLACE;
	m_ShadowStencil.m_nCompFun	= CMP_NOTEQUAL;
	m_ShadowStencil.m_nRefrence = 255;

	m_ShowRs.m_TexOP_S0			= TOP_ADD;
	m_ShowRs.m_TexArg1_S0		= TA_SPECULAR;
	m_ShowRs.m_TexArg2_S0		= TA_TFACTOR;

	m_ShowRs.m_AlpOP_S0			= CGpuProgramMgr::GetInst()->GetShaderVer() == Shader_V_0 ? TOP_SELECTARG1 : TOP_ADD;
	m_ShowRs.m_AlpArg1_S0		= TA_TFACTOR;
	m_ShowRs.m_AlpArg2_S0		= TA_SPECULAR;
	m_ShowRs.m_TexOP_S1			= TOP_DISABLE;
	m_ShowRs.m_TexOP_S2			= TOP_DISABLE;
	m_ShowRs.m_TexOP_S3			= TOP_DISABLE;

	m_ShowRs.m_AlphaBlendEnable  = TRUE;
	m_ShowRs.m_SrcBlend 		= BLEND_SRCALPHA;
	m_ShowRs.m_DestBlend 		= BLEND_ONE;
	m_ShowRs.m_Cull				= CULLT_CCW;
	m_ShowRs.m_ZWrite			= FALSE;
	m_ShowRs.m_ZTestEnable		= TRUE;
	m_ShowRs.m_ZTestFun			= CMP_GREATER;
	m_ShowRs.m_Texturefactor	= 0x403D5FFF;
	m_ShowRs.m_SpecularEnable	= TRUE;
	m_ShowRs.m_LightEnable		= TRUE;
	m_ShowRs.m_Material.Specular = CColor4(0xFFFFF382);
	m_ShowRs.m_Material.Power	= 4.0f;

}

CShowPassFilter::~CShowPassFilter()
{
}

FilterNode* CShowPassFilter::Filter(FilterNode* node)
{
	if( m_RenderList.insert(node) )
	{	
		node->param_release = NullRelease;
	}

	return node;
}

void CShowPassFilter::Render()
{
	//m_ShadowStencil.m_bEnable = true;
	//CRenderPipeline::GetInst()->SetStencil(m_ShadowStencil);

	CRenderPipeline::GetInst()->SetRenderStyle(m_ShowRs);
	CRenderPipeline* pipeline = CRenderPipeline::GetInst();
	
	//LightStruct* pGlobeLight = m_pParentGroup->GetLightSystem()->GetGlobalLight()->GetLightStruct();
	//LightStruct tmpLight = *pGlobeLight;
	//tmpLight.Diffuse = CColor4(255,255,255,255);
	//pipeline->SetLight(0,&tmpLight);
	//pipeline->SetVertexShaderF(ACT_GLOBAL_LIGHT_COLOR, &(tmpLight.Diffuse), 1);

	for (CRenderList::iterator it = m_RenderList.begin();
		it != m_RenderList.end(); ++it)
	{
		FilterNode* node = static_cast<FilterNode*>(*it);
		node->param_render(*node);
		pipeline->SetWorld(node->m_RP.m_Mat);
		//pipeline->SetTexture(node->m_RP.m_RS);
		pipeline->SetShader(node->m_RP.m_RS.m_ShaderType);
		pipeline->RenderEx(node);
	}
	//pipeline->SetLight(0,pGlobeLight);
	//pipeline->SetVertexShaderF(ACT_GLOBAL_LIGHT_COLOR, &(pGlobeLight->Diffuse), 1);

	//m_ShadowStencil.m_bEnable = false;
	//CRenderPipeline::GetInst()->SetStencil(m_ShadowStencil);
}