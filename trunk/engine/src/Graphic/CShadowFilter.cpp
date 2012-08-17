#include "stdafx.h"
#include "CShadowFilter.h"
#include "CRenderPipeline.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "TSqrAllocator.inl"
#include "CColor.inl"

//CVector3f CShadowFilter::st_LightVector;
CShadowFilter::CShadowFilter(CRenderGroup* pRenderGroup)
: m_pRenderGroup(pRenderGroup)
{
	SetRefInfo("ÒõÓ°»ùÀà");
	AddRef(); 
}

CShadowFilter::~CShadowFilter() 
{
};

void CShadowFilter::Render(void)
{
	this->Begin();

	if (!m_RenderList.empty())
	{
		CRenderPipeline::GetInst()->SetRenderStyle(m_ShadowRs);
		CRenderList::iterator it = m_RenderList.begin();
		CRenderList::iterator eit = m_RenderList.end();
		CRenderPipeline* crp = CRenderPipeline::GetInst();

		for(;;)
		{
			FilterNode* fn = 0;
			SQR_TRY
			{
				for(;it!=eit;)
				{
					fn = (FilterNode*)*(it++);//st_arpFilterNode[*it];
					if( gIsZero( fn->m_RP.m_RS.m_Material.Diffuse.a , 0.004f  ) )
						continue;

					fn->param_render(*fn);
					UpdateRenderStyle(fn->m_RP);
					crp->RenderEx(fn);
				}
				break;
			}
			SQR_CATCH(exp)
			{
				exp.AppendType(GetRefInfo().c_str());
				GfkLogExp(exp);
				fn->Release();
			}
			SQR_TRY_END;
		}
	}
	
	this->End();
}

FilterNode* CShadowFilter::Filter(FilterNode* fn)
{
	if( m_RenderList.insert(fn) )
	{
		fn->m_eType = ROS_SHADOW;	
		fn->param_release = NullRelease;
		fn->m_RP.m_RS.m_ZTestEnable = TRUE;
		fn->m_RP.m_RS.m_ZTestFun = CMP_NEVER;
	}
	return fn;
}

void CShadowFilter::UpdateRenderStyle(const RenderParam& RP )
{
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	crp->_SetRenderStyle (RS_TEXTUREFACTOR, (DWORD(RP.m_RS.m_Material.Diffuse.a * m_ShadowRs.m_Texturefactor)) & 0xFF000000);
	crp->SetTexture(RP.m_RS);
	crp->SetShader(RP.m_RS.m_ShaderType);
	crp->SetWorld(RP.m_Mat);
}

void CShadowFilter::Refresh(CRenderFilter* newFilter)
{
	CRenderList::iterator it = m_RenderList.begin();
	CRenderList::iterator eit = m_RenderList.end();
	for (;it!=eit;)
	{
		newFilter->Filter(*it++);
	}
}

void	CShadowFilter::SetLightVector( const CVector3f& lv )
{
	m_LightVector = lv;
}

const CVector3f& CShadowFilter::GetLightVector(void)
{
	return m_LightVector;
}

