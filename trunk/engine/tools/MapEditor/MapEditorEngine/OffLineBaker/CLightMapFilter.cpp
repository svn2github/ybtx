#include "StdAfx.h"
#include "CLightMapFilter.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "BaseHelper.h"
#include "CShadowMatrixBuilder.h"
#include "CTerrainMgr.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "TSqrAllocator.inl"

namespace sqr
{
const DWORD CLightMapFilter::st_LightMapVSID = MakeSysVS_ID(SSI_LIGHT_MAP);
const DWORD CLightMapFilter::st_LightMapPSID = MakeSysPS_ID(SSI_LIGHT_MAP);

//------------------------------------------------------------------------------
CLightMapFilter::CLightMapFilter(CRenderGroup* pRenderGroup)
		: CShadowFilter(pRenderGroup)
		, isTerrainShadowOn(true)
		, isObjectShadowOn(true)
		, zOffset(0)
{
	// create render target(color texture)
	this->shadowTexture = CGraphic::GetInst()->CreateRenderTarget();
	this->shadowTexture->Release();                                   //临时补丁，强制计数器减1
	this->shadowTexture->SetWidth(1024);
	this->shadowTexture->SetHeight(1024);
	this->shadowTexture->SetFormat(TFMT_R32F);
	this->shadowTexture->SetUseDepthBuffer(true);
	this->shadowTexture->Create();

	this->shadowTexture->AutoClearColor(false);
	this->shadowTexture->SetClearColor(CColor::White);

	this->matrixBuilder = new COrthoMatrixBuilder();
	this->matrixBuilder->SetCamera(CMainWindowTarget::GetInst()->GetCamera());
	this->matrixBuilder->SetShadowMapSize(1024);

	m_ShadowRs.m_ZTestFun			= CMP_LESSEQUAL;
	m_ShadowRs.m_ZWrite				= TRUE;
	m_ShadowRs.m_AlphaTestEnable	= FALSE;
	m_ShadowRs.m_AlphaBlendEnable	= FALSE;
	m_ShadowRs.m_LightEnable		= FALSE;
	m_ShadowRs.m_Cull				= CULLT_NONE;

	RenderOut=false;
	shadowTextureOut=NULL;
	matrixBuilderOut=NULL;

}

//------------------------------------------------------------------------------
CLightMapFilter::~CLightMapFilter()
{
	CRenderPipeline::GetInst()->_SetTexture(7, NULL);
	this->matrixBuilder = NULL;
	this->shadowTexture = NULL;

	shadowTextureOut=NULL;
	matrixBuilderOut=NULL;
}

//------------------------------------------------------------------------------
void
CLightMapFilter::Begin()
{
	if(RenderOut)
	{
		BeginEx();
		return ;
	}
	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	renderPipeline->_SetTexture( 7, NULL );

	// begin rendertarget
	this->shadowTexture->Begin(false);
	this->shadowTexture->Clear();

	// clear to white
	renderPipeline->_SetSamplerState(7, SAMP_BORDERCOLOR, 0xffffffffUL);
	renderPipeline->_SetSamplerState(7, SAMP_ADDRESSU, TADDRESS_BORDER);
	renderPipeline->_SetSamplerState(7, SAMP_ADDRESSV, TADDRESS_BORDER);
	renderPipeline->_SetSamplerState(7, SAMP_MINFILTER, TEXF_POINT);
	renderPipeline->_SetSamplerState(7, SAMP_MAGFILTER, TEXF_POINT);
	// set matrix
	this->matrixBuilder->SetLightDir(m_LightVector);
	this->matrixBuilder->BuildMatrix(&m_BoxClipper);
	renderPipeline->SetView(this->matrixBuilder->GetViewMatrix());
	renderPipeline->SetProject(this->matrixBuilder->GetProjectMatrix());
}

//------------------------------------------------------------------------------
void
CLightMapFilter::End()
{
	if(RenderOut)
	{
		EndEx();
		return ;
	}
	//end render target
	this->shadowTexture->End();

	//this->shadowTexture->SaveToFile("e:/DEPTH_OLD.DDS");
	// restore view/project
	
	CRenderPipeline::GetInst()->SetCamera(this->matrixBuilder->GetCamera());
}

//------------------------------------------------------------------------------
void
CLightMapFilter::ResetFilterQueue( RenderQueueFlag& fQueueFlag )
{
	fQueueFlag.Delete(ID_SHADOW_FILTER);
	fQueueFlag.Delete(ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.Delete(ID_NPC_SHADOWRECEIVET_FILTER);
	fQueueFlag>>ID_SHADOW_FILTER;
	fQueueFlag.InsertBefore(ID_TERRAIN_FILTER,ID_SCENE_SHADOWRECEIVET_FILTER);
	fQueueFlag.InsertBefore(ID_SCENE_SHADOWRECEIVET_FILTER,ID_NPC_SHADOWRECEIVET_FILTER);
}

//------------------------------------------------------------------------------
void
CLightMapFilter::SetShadowParams( STATIC_RS& RS )
{
	RS.m_ShaderType |= st_LightMapVSID;
	RS.m_ShaderType |= st_LightMapPSID;
}

//------------------------------------------------------------------------------
void
CLightMapFilter::RenderTerrain()
{
	// render
	CTerrainMgr* terrain = (CTerrainMgr*)m_pRenderGroup->GetBatch(ID_TERRAIN_FILTER);
	CRenderPipeline* rp = CRenderPipeline::GetInst();
	RenderParam state = terrain->m_Param;
	state.m_RS.m_Texture_S0 = state.m_RS.m_Texture_S1 = state.m_RS.m_Texture_S2 = state.m_RS.m_Texture_S3 = NULL;
	state.m_RS.m_AlphaBlendEnable = FALSE;
	state.m_RS.m_AlphaTestEnable = FALSE;
	state.m_RS.m_ShaderType = MakeSysVS_ID(20);
	state.m_RS.m_ShaderType |= MakeSysPS_ID(21);
	rp->SetRenderStyle(state.m_RS);
	rp->SetWorld(CMatrix::UNIT_MATRIX);
	terrain->RenderEx();
}

//------------------------------------------------------------------------------
void
CLightMapFilter::AddReceiverBox( const CAxisAlignedBox& box )
{
	static CMatrix identity;
	CAxisAlignedBox aabb = box;
	//for (index_t i = 2; i <= 5; ++i)
	//{
	//	CVector3f point = box.getAllCorners()[i] + st_LightVector * this->zOffset;
	//	aabb.mergeBox(point);
	//}
	m_BoxClipper.AddCasterBox(identity, aabb);
}

//------------------------------------------------------------------------------
void
CLightMapFilter::Render( void )
{
	
	this->Begin();
	
	if (this->isTerrainShadowOn)
	{
		this->RenderTerrain();
	}
	if (this->isObjectShadowOn && !m_RenderList.empty())
	{
		CRenderPipeline::GetInst()->SetRenderStyle(m_ShadowRs);
		CRenderList::iterator it = m_RenderList.begin();
		CRenderList::iterator eit = m_RenderList.end();
		CRenderPipeline* crp = CRenderPipeline::GetInst();
		for (;it!=eit;++it)
		{
			FilterNode* fn = (FilterNode*)*it;//st_arpFilterNode[*it];
			fn->param_render(*fn);
			UpdateRenderStyle(fn->m_RP);
			crp->RenderEx(fn);
		}
	}
	this->End();
}

//------------------------------------------------------------------------------
const CMatrix&
CLightMapFilter::GetShadowMatrix() const
{
	return this->matrixBuilder->GetShadowMatrix();
}

//------------------------------------------------------------------------------
void
CLightMapFilter::UpdateShadowParams()
{
	if(RenderOut)
	{
		UpdateShadowParamsEx();
		return;
	}

	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_GLOBAL_SHADOW_MATRIX,
		&this->matrixBuilder->GetShadowMatrix(), ACS_MATRIX);
	CRenderPipeline::GetInst()->_SetTexture(7,this->shadowTexture);
}

//------------------------------------------------------------------------------
void
CLightMapFilter::UpdateRenderStyle(const RenderParam& RP )
{
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	crp->_SetRenderStyle (RS_TEXTUREFACTOR, (DWORD(RP.m_RS.m_Material.Diffuse.a * m_ShadowRs.m_Texturefactor)) & 0xFF000000);
	// 这里弄个临时方案, 解决镂空的问题...
	crp->_SetTexture(0, RP.m_RS.m_Texture_S1 ? RP.m_RS.m_Texture_S1 : RP.m_RS.m_Texture_S0);
	SHADER_HANDLE shader;
	shader |= MakeSysVS_ID(20);
	shader |= MakeSysPS_ID(22);
	crp->SetShader(shader);
	crp->SetWorld(RP.m_Mat);
}

void CLightMapFilter::SetShadowTexOut(CRenderTexturePtr shadowTex)
{
	shadowTextureOut=shadowTex;
}

void CLightMapFilter::SetMatrixBuildOut(CShadowMatrixBuilderPtr shadowMatrixBuild)
{
	matrixBuilderOut=shadowMatrixBuild;
}

bool CLightMapFilter::EnableRenderOut()
{ 
	if(shadowTextureOut==NULL || matrixBuilderOut==NULL)
		return false;
	RenderOut=true;
	return true;
}

bool CLightMapFilter::DisableRenderOut()
{
	RenderOut=false;
	return true;
}

void CLightMapFilter::BeginEx()
{
	CRenderPipeline* renderPipeline = CRenderPipeline::GetInst();
	renderPipeline->_SetTexture( 7, NULL );

	// begin rendertarget
	shadowTextureOut->Begin(false);
	shadowTextureOut->Clear();

	// clear to white
	renderPipeline->_SetSamplerState(7, SAMP_BORDERCOLOR, 0xffffffffUL);
	renderPipeline->_SetSamplerState(7, SAMP_ADDRESSU, TADDRESS_BORDER);
	renderPipeline->_SetSamplerState(7, SAMP_ADDRESSV, TADDRESS_BORDER);
	renderPipeline->_SetSamplerState(7, SAMP_MINFILTER, TEXF_POINT);
	renderPipeline->_SetSamplerState(7, SAMP_MAGFILTER, TEXF_POINT);
	// set matrix
	matrixBuilderOut->SetLightDir(m_LightVector);
	matrixBuilderOut->BuildMatrixEx(&m_BoxClipper);
	renderPipeline->SetView(matrixBuilderOut->GetViewMatrix());
	renderPipeline->SetProject(matrixBuilderOut->GetProjectMatrix());
}

void CLightMapFilter::EndEx()
{
	//end render target
	shadowTextureOut->End();
	// restore view/project
	CRenderPipeline::GetInst()->SetCamera(matrixBuilderOut->GetCamera());
}

void CLightMapFilter::UpdateShadowParamsEx()
{
	CRenderPipeline::GetInst()->SetVertexShaderF(ACT_GLOBAL_SHADOW_MATRIX,
		&this->matrixBuilderOut->GetShadowMatrix(), ACS_MATRIX);
	CRenderPipeline::GetInst()->_SetTexture(7,this->shadowTextureOut);
}


}// namespace sqr
