#include "stdafx.h"
#include "CVertexDeclFilter.h"
#include "CRenderPipeline.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "TSqrAllocator.inl"
namespace sqr
{

	CVertexDeclFilter::CVertexDeclFilter(CVertexDeclaration* pVD, CRenderFilter* pParent)
		:m_pVD(pVD)
		,m_pParentFilter(pParent)
	{
		AddRef(); 
		AstMsg(pParent, "CVertexDeclFilter不能没有父");
	}

	CVertexDeclFilter::~CVertexDeclFilter()
	{

	}

	FilterNode* CVertexDeclFilter::Filter(FilterNode* fn)
	{
		if( m_RenderList.insert(fn) )
			return fn;
		return NULL;
	}

	void CVertexDeclFilter::Render(void)
	{
		if (m_RenderList.empty())
			return;
		CRenderList::iterator it = m_RenderList.begin();
		CRenderList::iterator eit = m_RenderList.end();
		CRenderPipeline* crp = CRenderPipeline::GetInst();
		for (;it!=eit;++it)
		{
			(*it)->param_render(**it);
			crp->Render((*it),&((*it)->m_RP));
		}
		crp->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
	}

	void CVertexDeclFilter::RenderEx(void)
	{
		if (m_RenderList.empty())
			return;
		CRenderList::iterator it = m_RenderList.begin();
		CRenderList::iterator eit = m_RenderList.end();
		CRenderPipeline* crp = CRenderPipeline::GetInst();
		FilterNode* fn = 0;

		for(;;)
		{
			SQR_TRY	
			{
				for (;it!=eit;)
				{
					fn = (FilterNode*)*(it++);
					fn->param_render(*fn);
					m_pParentFilter->UpdateRenderStyle(fn->m_RP);
					crp->RenderEx(fn);
				}
				break;
			}
			SQR_CATCH(exp)
			{
				exp.AppendType("CVertexDeclFilter::RenderEx发生结构化异常");
				GfkLogExp(exp);
				fn->Release();	
			}
			SQR_TRY_END
		}
		crp->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
	}

	void CVertexDeclFilter::Refresh(CRenderFilter* newFilter)
	{
		CRenderList::iterator it = m_RenderList.begin();
		CRenderList::iterator eit = m_RenderList.end();
		for (;it!=eit;)
		{
			newFilter->Filter(*it++);
		}
	}
}