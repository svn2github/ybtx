#include "StdAfx.h"
#include "CRefractFilter.h"
#include "CFilterNode.h"
#include "CRenderPipeline.h"
#include "CPostProcessManager.h"
#include "CRenderTexture.h"
#include "CDynamicVB.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

namespace  sqr
{
//------------------------------------------------------------------------------
CRefractFilter::CRefractFilter(ERenderObjStyle ros)
: effectVB(new CDynamicVB())
{
	AddRef();
	HRESULT hr = this->effectVB->Create(0x1000, 0x800);
	if(FAILED(hr))
		GfkLogErr(GraphicErr::GetErrTypeStr(RenderRuntime_Err),"RefractFilter effectVB Ê§°Ü");

	//m_RefractRs.m_ShaderType		|= MakeSysVS_ID(1);
	m_RefractRs.m_TexOP_S0			= TOP_MODULATE;
	m_RefractRs.m_TexArg1_S0		= TA_TEXTURE;
	m_RefractRs.m_TexArg2_S0		= TA_DIFFUSE;
	m_RefractRs.m_Texture_S1		= NULL;
	m_RefractRs.m_Texture_S2		= NULL;
	m_RefractRs.m_Texture_S3		= NULL;
	m_RefractRs.m_AlpOP_S0			= TOP_DISABLE;
	m_RefractRs.m_TexOP_S1			= TOP_DISABLE;
	m_RefractRs.m_TexOP_S2			= TOP_DISABLE;
	m_RefractRs.m_TexOP_S3			= TOP_DISABLE;
	m_RefractRs.m_UAddr_S0			= m_RefractRs.m_VAddr_S0 = TADDRESS_CLAMP;
	m_RefractRs.m_AlphaBlendEnable  = FALSE;
	m_RefractRs.m_Cull				= CULLT_CCW;
	m_RefractRs.m_ZWrite			= TRUE;
	m_RefractRs.m_ZTestEnable		= TRUE;
}

//------------------------------------------------------------------------------
CRefractFilter::~CRefractFilter()
{
	GfkSafeDelete(this->effectVB);
}

//------------------------------------------------------------------------------
FilterNode* CRefractFilter::Filter( FilterNode* node )
{
	Ast(NULL != node && ROS_REFRACT == node->m_eType);
	if( m_RenderList.insert(node) )
	{
		node->m_RP.m_RS.m_ShaderType	|= MakeSysVS_ID(1);
	}
	return node;
}

//------------------------------------------------------------------------------
inline bool
CRefractFilter::IsValidQueue() const
{
	return (!(this->m_RenderList.empty() && this->effectVB->IsEmpty()) &&
		CPostProcessManager::GetInst()->IsEffectEnabled("Refract"));
}

//------------------------------------------------------------------------------
void CRefractFilter::Render()
{
	if (!IsValidQueue())
		return;

	m_RefractRs.m_Texture_S0 =  CPostProcessManager::GetInst()->GetDefaultRenderTarget();
	if( m_RefractRs.m_Texture_S0 == NULL )
		return;

	CRenderPipeline::GetInst()->SetRenderStyle(m_RefractRs);

	CRenderPipeline* pipeline = CRenderPipeline::GetInst();
	for (CRenderList::iterator it = m_RenderList.begin();
	        it != m_RenderList.end(); ++it)
	{
		FilterNode* node = static_cast<FilterNode*>(*it);
		node->param_render(*node);
		pipeline->_SetMaterial( node->m_RP.m_RS.m_Material );
		pipeline->SetVertexShaderF(ACT_REFRACT_SCALAR	, node->m_RP.m_RS.m_fRefractIndex);
		pipeline->SetVertexShaderF(ACT_MAT_DIFFUSE_COLOR, (float*)&node->m_RP.m_RS.m_Material.Diffuse, 1);
		pipeline->SetWorld(node->m_RP.m_Mat);
		pipeline->SetShader(node->m_RP.m_RS.m_ShaderType);
		pipeline->RenderEx(node);
		// fix a render state error, this state was changed in CModel::Render()!!! [7/8/2009 xoyojank]
	/*	node->m_RP.m_RS.m_Texture_S0 = CPostProcessManager::GetInst()->GetDefaultRenderTarget();
		node->m_RP.m_RS.m_TexOP_S0   = TOP_MODULATE;
		pipeline->Render(&node->m_Op, &node->m_RP);*/
	}
	this->effectVB->Render();
}

//------------------------------------------------------------------------------
void CRefractFilter::SetLightIndex( uint index )
{
	// empty
}

//------------------------------------------------------------------------------
CMatrix*
CRefractFilter::GetRenderStack( ERenderStyle RenderStyle,
							   ERenderObjStyle RenderObjStyle,
							   ITexture* Text0,					// ignore
							   ITexture* Text1,					// ignore
							   EPrimitiveType PrimitiveType,
							   uint32 VertexNum,
							   uint32 PrimitiveCount,
							   uint32 VerFormat,
							   uint32 VerStride,
							   void** ppVB, void** ppIB,
							   void** ppRS /*= NULL*/,	
							   DWORD ShaderID /*= 0*/ )			// ignore
{
	Ast(ROS_REFRACT == RenderObjStyle);
	ITexture* defaultRTT = CPostProcessManager::GetInst()->GetDefaultRenderTarget();
	if (NULL == defaultRTT)
		return NULL;

	RenderParam& RP = this->effectVB->GetRenderStack( PrimitiveType, VertexNum,
		PrimitiveCount, VerFormat, VerStride, ppVB, ppIB );
	RP.m_RS.m_Texture_S0 = defaultRTT;
	RP.m_RS.m_Texture_S1 = NULL;
	RP.m_RS.m_Texture_S2 = NULL;
	RP.m_RS.m_Texture_S3 = NULL;
	RP.m_RS.m_ShaderType		|= MakeSysVS_ID(1);
	RP.m_RS.m_TexOP_S0			= TOP_MODULATE;
	RP.m_RS.m_TexArg1_S0		= TA_TEXTURE;
	RP.m_RS.m_TexArg2_S0		= TA_DIFFUSE;
	RP.m_RS.m_TexOP_S1			= TOP_DISABLE;
	RP.m_RS.m_TexOP_S2			= TOP_DISABLE;
	RP.m_RS.m_TexOP_S3			= TOP_DISABLE;
	RP.m_RS.m_UAddr_S0			= RP.m_RS.m_VAddr_S0 = TADDRESS_CLAMP;
	RP.m_RS.m_AlphaBlendEnable  = FALSE;
	RP.m_RS.m_Cull				= CULLT_NONE;
	RP.m_RS.m_ZWrite			= TRUE;
	RP.m_RS.m_ZTestEnable		= TRUE;
	RP.m_RS.m_Texture_S0->AddRef();
	*ppRS = &RP.m_RS;
	return ( CMatrix* )&RP.m_Mat;
}

}// namespace sqr
