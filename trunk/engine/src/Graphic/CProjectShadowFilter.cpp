#include "stdafx.h"
#include "CProjectShadowFilter.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CShadowMatrixBuilder.h"
#include "TraceHelper.h"
#include "TSqrAllocator.inl"
#include "CColor.inl"

const DWORD CProjectShadowFilter::st_ProjectShadowVSID = MakeSysVS_ID(SSI_SHADOW_MAP);
const DWORD CProjectShadowFilter::st_ProjectShadowPSID = MakeSysPS_ID(SSI_SHADOW_MAP);
CProjectShadowFilter::CProjectShadowFilter(CRenderGroup* pRenderGroup)
		: CShadowFilter(pRenderGroup)
{
	SetRefInfo("投影阴影");
	this->shadowTexture = CGraphic::GetInst()->CreateRenderTarget();
	this->shadowTexture->SetWidth(1024);
	this->shadowTexture->SetHeight(1024);
	this->shadowTexture->SetUseDepthBuffer(true);
	this->shadowTexture->Create();
	this->shadowTexture->Release();

	this->shadowTexture->AutoClearColor(false);
	this->shadowTexture->SetClearColor(CColor::White);

	this->matrixBuilder = new CTsmMatrixBuilder();
	this->matrixBuilder->SetCamera(CMainWindowTarget::GetInst()->GetCamera());
	this->matrixBuilder->SetShadowMapSize(1024);


	//进入队列影子的渲染状态
	m_ShadowRs.m_ZTestFun			= CMP_ALWAYS;
	m_ShadowRs.m_ZWrite				= FALSE;
	m_ShadowRs.m_Texturefactor		= (DWORD(0x80))<<24;
	m_ShadowRs.m_AlphaTestEnable	= TRUE;
	m_ShadowRs.m_Alpharef			= 0x20;
	m_ShadowRs.m_TexOP_S0			= TOP_SELECTARG1;
	m_ShadowRs.m_TexArg1_S0			= TA_TFACTOR;
	m_ShadowRs.m_AlpOP_S0			= TOP_MODULATE;
	m_ShadowRs.m_AlpArg1_S0			= TA_TEXTURE;
	m_ShadowRs.m_AlpArg2_S0			= TA_TFACTOR;
	m_ShadowRs.m_AlphaBlendEnable	= FALSE;
	m_ShadowRs.m_Material.Diffuse.r	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.g	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.b	= 0.0f;
	m_ShadowRs.m_Material.Diffuse.a	= 0.0f;
}

CProjectShadowFilter::~CProjectShadowFilter()
{
	CRenderPipeline::GetInst()->_SetTexture( 7, NULL );
	this->shadowTexture = NULL;
	this->matrixBuilder = NULL;
}

void CProjectShadowFilter::ResetFilterQueue(RenderQueueFlag& fQueueFlag)
{
	fQueueFlag.Delete(ID_SHADOW_FILTER);
	fQueueFlag.Delete(ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.Delete(ID_NPC_SHADOWRECEIVET_FILTER);
	fQueueFlag.Delete(ID_GEOMETRY_INST_FILTER);
	fQueueFlag.Delete(ID_GEOMETRY_INST_SR_FILTER);
	fQueueFlag>>ID_SHADOW_FILTER;
	fQueueFlag.InsertBefore(ID_TERRAIN_FILTER,ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.InsertBefore(ID_SCENE_SHADOWRECEIVET_FILTER,ID_NPC_SHADOWRECEIVET_FILTER);
	fQueueFlag.InsertBefore(ID_NPC_SHADOWRECEIVET_FILTER,ID_GEOMETRY_INST_SR_FILTER);
	fQueueFlag.InsertBefore(ID_GEOMETRY_INST_SR_FILTER,ID_GEOMETRY_INST_FILTER);
}

void CProjectShadowFilter::Begin(void)
{
	CRenderPipeline::GetInst()->_SetTexture( 7, NULL );
	m_BoxClipper.Reset();
	CRenderList::iterator it = m_RenderList.begin();
	CRenderList::iterator eit = m_RenderList.end();
	for (;it!=eit;++it)
	{
		FilterNode* fn = *it;//st_arpFilterNode[*it];
		m_BoxClipper.AddCasterBox( fn->m_RP.m_Mat,*fn->pAABox );
	}


	this->shadowTexture->Begin(false);
	// clear to white
	this->shadowTexture->Clear();

	CRenderPipeline::GetInst()->_SetSamplerState(7, SAMP_BORDERCOLOR, 0xffffffffUL);
	CRenderPipeline::GetInst()->_SetSamplerState( 7, SAMP_ADDRESSU, TADDRESS_BORDER );
	CRenderPipeline::GetInst()->_SetSamplerState( 7, SAMP_ADDRESSV, TADDRESS_BORDER );
	// 2x2 filter
	CRenderPipeline::GetInst()->_SetSamplerState( 7, SAMP_MINFILTER, TEXF_LINEAR);
	CRenderPipeline::GetInst()->_SetSamplerState( 7, SAMP_MAGFILTER, TEXF_LINEAR);

	// set matrix
	this->matrixBuilder->SetLightDir(m_LightVector);
	this->matrixBuilder->BuildMatrix(&m_BoxClipper);
	CRenderPipeline::GetInst()->SetView(this->matrixBuilder->GetViewMatrix());
	CRenderPipeline::GetInst()->SetProject(this->matrixBuilder->GetProjectMatrix());
}

void CProjectShadowFilter::End(void)
{
	this->shadowTexture->End();
	// restore view/project
	CRenderPipeline::GetInst()->SetCamera(this->matrixBuilder->GetCamera());
#ifdef _DEBUG
	// render shadow map
	if (GetComputerName() == "LK-WXQ")
		CGraphic::GetInst()->DrawRect(CFRect(0, 0, 256, 256), 0xffffffff,
		this->shadowTexture, NULL, RS_GUI, 0.0f);
#endif // _DEBUG
}

void CProjectShadowFilter::SetShadowParams(STATIC_RS& RS)
{
	if(RS.m_bPreLight)
		RS.m_ShaderType |= (st_ProjectShadowVSID|SSI_PRE_RENDER_LIGHT);
	else
		RS.m_ShaderType |= st_ProjectShadowVSID;
	RS.m_ShaderType |= st_ProjectShadowPSID;
}

void CProjectShadowFilter::UpdateShadowParams(void)
{
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_GLOBAL_SHADOW_MATRIX,
		&this->matrixBuilder->GetShadowMatrix(), ACS_MATRIX);
	CRenderPipeline::GetInst()->_SetTexture( 7, this->shadowTexture );
}
