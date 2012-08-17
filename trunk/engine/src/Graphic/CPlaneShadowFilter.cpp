#include "stdafx.h"
#include "CRenderPipeline.h"
#include "CPlaneShadowFilter.h"
#include "ILogicalHandler.h"
#include "MathAlgorithm.h"

//----------------------平面阴影区-------------------------------------
CPlaneShadowFilter::CPlaneShadowFilter(CRenderGroup* pRenderGroup)
		:CShadowFilter(pRenderGroup)
{
	SetRefInfo("平面阴影"); 
	m_ShadowStencil.m_nPassFlag = STENCILOP_INCR;//STENCILOP_REPLACE;
	m_ShadowStencil.m_nCompFun	= CMP_NOTEQUAL;
	m_ShadowStencil.m_nRefrence = 1;

	m_ShadowRs.m_ZTestFun			= CMP_ALWAYS;
	m_ShadowRs.m_ZWrite				= FALSE;
	m_ShadowRs.m_Texturefactor		= (DWORD(0x70))<<24;
	m_ShadowRs.m_AlphaTestEnable	= TRUE;	
	m_ShadowRs.m_AlphaTestFun		= CMP_GREATEREQUAL;
	m_ShadowRs.m_Alpharef			= 0x55;
	m_ShadowRs.m_LightEnable		= FALSE;
	m_ShadowRs.m_TexOP_S0			= TOP_SELECTARG1;
	m_ShadowRs.m_TexArg1_S0			= TA_TFACTOR;
	//m_ShadowRs.m_AlpOP_S0			= TOP_SELECTARG2;//TOP_MODULATE;
	m_ShadowRs.m_AlpOP_S0			= TOP_MODULATE;
	m_ShadowRs.m_AlpArg1_S0			= TA_TEXTURE;
	m_ShadowRs.m_AlpArg2_S0			= TA_TFACTOR;
	m_ShadowRs.m_AlphaBlendEnable	= TRUE;
	m_ShadowRs.m_Material.Diffuse.r	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.g	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.b	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.a	= 0.0f;
	m_ShadowRs.m_SrcBlend 			= BLEND_SRCALPHA;
	m_ShadowRs.m_DestBlend 			= BLEND_INVSRCALPHA;	
}

void CPlaneShadowFilter::Begin(void)
{
	m_ShadowStencil.m_bEnable = true;
	CRenderPipeline::GetInst()->SetStencil(m_ShadowStencil);
}

void CPlaneShadowFilter::End(void)
{
	m_ShadowStencil.m_bEnable = false;
	CRenderPipeline::GetInst()->SetStencil(m_ShadowStencil);
}

FilterNode* CPlaneShadowFilter::Filter(FilterNode* fn)
{
	//FilterNode FNode = *fn;
	Update(fn);
	return CShadowFilter::Filter(fn);
}

void CPlaneShadowFilter::Update(FilterNode* fn)
{
	CMatrix tmpMat = ((FilterNode*)fn)->m_RP.m_Mat;
	CreateShadowMatrixOnPlane(tmpMat,-m_LightVector,CVector3f(0.0f,1.0f,0.0f),
		CVector3f(0.0f,tmpMat._42,0.0f));
	((FilterNode*)fn)->m_RP.m_Mat = ((FilterNode*)fn)->m_RP.m_Mat * tmpMat;
}

void CPlaneShadowFilter::ResetFilterQueue(RenderQueueFlag& fQueueFlag)
{
	fQueueFlag.Delete(ID_SHADOW_FILTER);
	fQueueFlag.Delete(ID_SCENE_FILTER);
	fQueueFlag.Delete(ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.Delete(ID_NPC_SHADOWRECEIVET_FILTER);
	fQueueFlag.Delete(ID_GEOMETRY_INST_FILTER);
	fQueueFlag.Delete(ID_GEOMETRY_INST_SR_FILTER);

	fQueueFlag.InsertBefore(ID_TERRAIN_FILTER,ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.InsertBefore(ID_SCENE_SHADOWRECEIVET_FILTER,ID_NPC_SHADOWRECEIVET_FILTER);
	fQueueFlag.InsertBefore(ID_NPC_SHADOWRECEIVET_FILTER,ID_GEOMETRY_INST_SR_FILTER);
	fQueueFlag.InsertBack(ID_TERRAIN_FILTER,ID_SHADOW_FILTER);
	fQueueFlag.InsertBack(ID_SHADOW_FILTER,ID_GEOMETRY_INST_FILTER);
	fQueueFlag.InsertBack(ID_GEOMETRY_INST_FILTER,ID_SCENE_FILTER);
}