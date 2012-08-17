#include "stdafx.h"
#include "CShadowMapFilter.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CShadowMatrixBuilder.h"
#include "CRenderGroup.h"
#include "TraceHelper.h"

const DWORD CShadowMapFilter::st_ShadowMapVSID = MakeSysVS_ID(SSI_SHADOW_MAP);
const DWORD CShadowMapFilter::st_ShadowMapPSID = MakeSysPS_ID(SSI_SHADOW_MAP);
CShadowMapFilter::CShadowMapFilter(CRenderGroup* pRenderGroup, bool b)
		: CShadowFilter(pRenderGroup)
		, isReceiveShadow(b)
{
	SetRefInfo("ShadowMapÒõÓ°");
	// create render target(color texture)
	this->shadowTexture = CGraphic::GetInst()->CreateRenderTarget();
	this->shadowTexture->SetWidth(1024);
	this->shadowTexture->SetHeight(1024);
	this->shadowTexture->SetUseDepthBuffer(false);
	this->shadowTexture->Create();
	this->shadowTexture->Release();

	// create depth texture
	this->depthTexture = CGraphic::GetInst()->CreateDepthTexture(1024, 1024);
	this->depthTexture->SetClearColor(CColor::White);

	this->matrixBuilder = new CTsmMatrixBuilder();
	this->matrixBuilder->SetCamera(pRenderGroup->GetCamera());
	this->matrixBuilder->SetShadowMapSize(1024);

	m_ShadowRs.m_ZTestFun			= CMP_LESSEQUAL;
	m_ShadowRs.m_ZWrite				= TRUE;
	m_ShadowRs.m_Cull				= CULLT_NONE;
	m_ShadowRs.m_AlphaTestEnable	= TRUE;
	m_ShadowRs.m_Alpharef			= 0x20;
	m_ShadowRs.m_AlphaBlendEnable	= FALSE;
	m_ShadowRs.m_LightEnable		= FALSE;

	m_ShadowRs.m_Texturefactor		= (DWORD(0x80))<<24;
	m_ShadowRs.m_TexOP_S0			= TOP_SELECTARG1;
	m_ShadowRs.m_TexArg1_S0			= TA_TFACTOR;
	m_ShadowRs.m_AlpOP_S0			= TOP_MODULATE;
	m_ShadowRs.m_AlpArg1_S0			= TA_TEXTURE;
	m_ShadowRs.m_AlpArg2_S0			= TA_TFACTOR;
}

CShadowMapFilter::~CShadowMapFilter()
{
	CRenderPipeline::GetInst()->_SetTexture( 7, NULL );
	CGraphic::GetInst()->DestroyDepthTexture(this->depthTexture);
	this->matrixBuilder = NULL;
	this->shadowTexture = NULL;
}

void CShadowMapFilter::ResetFilterQueue(RenderQueueFlag& fQueueFlag)
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

void CShadowMapFilter::Begin()
{
	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	renderPipeline->_SetTexture( 7, NULL );

	if( (!shadowTexture.isvalid()) || NULL == depthTexture )
		return;

	m_BoxClipper.Reset();
	CRenderList::iterator it = m_RenderList.begin();
	CRenderList::iterator eit = m_RenderList.end();
	for (;it!=eit;++it)
	{
		FilterNode* fn = *it;//st_arpFilterNode[*it];
		m_BoxClipper.AddCasterBox( fn->m_RP.m_Mat,*fn->pAABox );
	}

	// begin rendertarget
	this->shadowTexture->Begin(false);
	this->depthTexture->Begin(true);

	// disable color write, only writes z
 	renderPipeline->_SetRenderStyle(RS_COLORWRITEENABLE, 0UL);
 	renderPipeline->_SetRenderStyle(RS_SLOPESCALEDEPTHBIAS, type_cast<DWORD>(1.0f)); 
 	renderPipeline->_SetSamplerState( 7, SAMP_BORDERCOLOR, 0xffffffffUL);
	renderPipeline->_SetSamplerState( 7, SAMP_ADDRESSU, TADDRESS_BORDER );
	renderPipeline->_SetSamplerState( 7, SAMP_ADDRESSV, TADDRESS_BORDER );
	// 2x2 filter
	renderPipeline->_SetSamplerState( 7, SAMP_MINFILTER, TEXF_LINEAR);
	renderPipeline->_SetSamplerState( 7, SAMP_MAGFILTER, TEXF_LINEAR);
 	// set matrix
 	this->matrixBuilder->SetLightDir(m_LightVector);
 	this->matrixBuilder->BuildMatrix(&m_BoxClipper);
 	renderPipeline->SetView(this->matrixBuilder->GetViewMatrix());
 	renderPipeline->SetProject(this->matrixBuilder->GetProjectMatrix());
}

void CShadowMapFilter::End()
{
	if( (!shadowTexture.isvalid()) || NULL == depthTexture )
		return;

	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	this->depthTexture->End();
	this->shadowTexture->End();
	// restore color write
	renderPipeline->_SetRenderStyle(RS_COLORWRITEENABLE, COLORWRITEENABLE_RED |
	                          COLORWRITEENABLE_GREEN | COLORWRITEENABLE_BLUE | COLORWRITEENABLE_ALPHA);
	// depth bias
	renderPipeline->_SetRenderStyle(RS_SLOPESCALEDEPTHBIAS, type_cast<DWORD>(0.0f) );

	// restore view/project
	renderPipeline->SetCamera(this->matrixBuilder->GetCamera());

//#ifdef _DEBUG
//	// render shadow map
//	if (GetComputerName() == "LK-DONGKAI")
//		CGraphic::GetInst()->DrawRect(CFRect(0, 0, 256, 256), 0xffffffff,
//		                              this->depthTexture, NULL, RS_GUI, 0.0f);
//#endif // _DEBUG
}

void CShadowMapFilter::SetShadowParams(STATIC_RS& RS)
{
	if(RS.m_bPreLight)
		RS.m_ShaderType |= (st_ShadowMapVSID|SSI_PRE_RENDER_LIGHT);
	else
		RS.m_ShaderType |= st_ShadowMapVSID;
	RS.m_ShaderType |= st_ShadowMapPSID;
}

void CShadowMapFilter::UpdateShadowParams(void)
{
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_GLOBAL_SHADOW_MATRIX,
		&this->matrixBuilder->GetShadowMatrix(), ACS_MATRIX);
	CRenderPipeline::GetInst()->_SetTexture( 7,this->depthTexture );
}
