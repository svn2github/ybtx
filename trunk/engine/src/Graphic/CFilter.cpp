#include "stdafx.h"
#include "CFilter.h"
#include "CLightSystem.h"
#include "CRenderGroup.h"
#include "CRenderPipeline.h"
#include "CSkeletalMesh.h"
#include "CVertexAniMesh.h"
#include "CShadowFilter.h"
#include "BaseHelper.h"

//-----------CDefaultFilter------------------------------------------------
namespace sqr
{
CDefaultFilter::CDefaultFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
		:m_eRos(eRos),m_pRenderGroup(pRenderGroup)//,m_LightSystem(pRenderGroup)
{
	AddRef(); 
	m_LightIndex = 1 - m_eRos&ROS_LIGHTMAT1;
	m_LightSystem = pRenderGroup->GetLightSystem();
}

CDefaultFilter::~CDefaultFilter()
{
	FilterMap::iterator it = m_Filters.begin();
	FilterMap::iterator ite = m_Filters.end();
	for ( ;it!=ite;++it)
		SafeRelease(it->second);
	m_Filters.clear();
}

FilterNode* CDefaultFilter::Filter(FilterNode*  fn)
{
	FilterMap::iterator it = m_Filters.find(((FilterNode*)fn)->m_Op.vertexData->vertexDeclaration);
	if (it==m_Filters.end())
	{
		CVertexDeclFilter* pVDF = new CVertexDeclFilter(((FilterNode*)fn)->m_Op.vertexData->vertexDeclaration,this);
		m_Filters.insert(FilterMap::value_type(((FilterNode*)fn)->m_Op.vertexData->vertexDeclaration,pVDF));
		return pVDF->Filter(fn);
	}
	it->second->Filter(fn);
	return fn;
}

void CDefaultFilter::Render(void)
{
	((CLightSystem*)m_LightSystem)->TurnOnGlobalLight(m_LightIndex);
	FilterMap::iterator it = m_Filters.begin();
	FilterMap::iterator ite = m_Filters.end();
	for ( ;it!=ite;++it)
		(it->second)->Render();
}

void CDefaultFilter::RenderEx(void)
{
	((CLightSystem*)m_LightSystem)->TurnOnGlobalLight(m_LightIndex);
	FilterMap::iterator it = m_Filters.begin();
	FilterMap::iterator ite = m_Filters.end();
	for ( ;it!=ite;++it)
		(it->second)->RenderEx();
}

void CDefaultFilter::Refresh(CRenderFilter* newFilter)
{
	FilterMap::iterator it = m_Filters.begin();
	FilterMap::iterator ite = m_Filters.end();
	for ( ;it!=ite;++it)
		(it->second)->Refresh(newFilter);
}

FilterNode* CAlphaShadowFilter::Filter(FilterNode* fn)
{
	FilterNode FN = *(FilterNode*)fn;
	FN.m_eType = ROS_SHADOW;
	m_pRenderGroup->Filter(&FN);
	return CDefaultFilter::Filter(fn);
}


CVertexDeclFilter::CVertexDeclFilter(CVertexDeclaration* pVD, CRenderFilter* pParent)
		:m_pVD(pVD)
		,m_pParentFilter(pParent)
{
	AddRef(); 
	AstMsg(pParent, "CVertexDeclFilter不能没有父");
}

CVertexDeclFilter::~CVertexDeclFilter()
{}

void CVertexDeclFilter::Remove(const FilterNode* fn)
{
	m_RenderList.erase(((FilterNode*)fn)->m_node);
}

FilterNode* CVertexDeclFilter::Filter(FilterNode* fn)
{
	pair< SetList::iterator, bool >	p = m_RenderList.insert(fn);
	((FilterNode*)fn)->m_node = p.first;
	((FilterNode*)fn)->m_ParentFilter = this;
	return fn;
}

void CVertexDeclFilter::Render(void)
{
	if (m_RenderList.empty())
		return;
	SetList::iterator it = m_RenderList.begin();
	SetList::iterator eit = m_RenderList.end();
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	for (;it!=eit;++it)
	{
		FilterNode* fn = (FilterNode*)*it;//st_arpFilterNode[*it];
		switch (fn->param_type)
		{
			case NONE_PARAM:
				crp->SetTextureMatrixImmediate(0,NULL);
				break;
			case BONE_LIST:
				crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CSkeletalMesh::SkeletalParams*)(fn->param_ptr),((CSkeletalMesh::SkeletalParams*)(fn->param_ptr))->BoneSize);
				crp->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
				break;
			case TEXTURE_MATRIX:
				crp->SetTextureMatrixImmediate(0,(CMatrix*)fn->param_ptr);
				break;
			case VERTEX_ANI:
				crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CVertexAniMesh::VertexAniParams*)(fn->param_ptr),1);
				break;
		}
		crp->Render(&(fn->m_Op),&(fn->m_RP));
	}
}


void CVertexDeclFilter::RenderEx(void)
{
	if (m_RenderList.empty())
		return;
	SetList::iterator it = m_RenderList.begin();
	SetList::iterator eit = m_RenderList.end();
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	for (;it!=eit;++it)
	{
		FilterNode* fn = (FilterNode*)*it;//st_arpFilterNode[*it];
		switch (fn->param_type)
		{
		case NONE_PARAM:
			crp->SetTextureMatrixImmediate(0,NULL);
			break;
		case BONE_LIST:
			crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CSkeletalMesh::SkeletalParams*)(fn->param_ptr),((CSkeletalMesh::SkeletalParams*)(fn->param_ptr))->BoneSize);
			crp->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
			break;
		case TEXTURE_MATRIX:
			crp->SetTextureMatrixImmediate(0,(CMatrix*)fn->param_ptr);
			break;
		case VERTEX_ANI:
			crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CVertexAniMesh::VertexAniParams*)(fn->param_ptr),1);
			break;
		}
		m_pParentFilter->UpdateRenderStyle(fn->m_RP);
		crp->RenderEx(&(fn->m_Op));
	}
}

void CVertexDeclFilter::Refresh(CRenderFilter* newFilter)
{
	SetList::iterator it = m_RenderList.begin();
	SetList::iterator eit = m_RenderList.end();
	for (;it!=eit;++it)
	{
		newFilter->Filter(*it);
	}
	m_RenderList.clear();
}


CVertexDeclCopyFilter::CVertexDeclCopyFilter(CVertexDeclaration* pVD)
		:m_pVD(pVD)
{
	AddRef(); 
}

CVertexDeclCopyFilter::~CVertexDeclCopyFilter()
{}

FilterNode*  CVertexDeclCopyFilter::Filter(FilterNode* fn)
{
	m_RenderList.insert(*(FilterNode*)fn);
	return fn;
}

void CVertexDeclCopyFilter::Render(void)
{
	if (m_RenderList.empty())
		return;
	CopyList::iterator it = m_RenderList.begin();
	CopyList::iterator eit = m_RenderList.end();
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	for (;it!=eit;++it)
	{
		FilterNode& fn = *it;//st_arpFilterNode[*it];

		switch (fn.param_type)
		{
			case NONE_PARAM:
				crp->SetTextureMatrixImmediate(0,NULL);
				break;
			case BONE_LIST:
				//D3DXMatrixIdentity(&fn.m_RP.m_Mat);
				crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CSkeletalMesh::SkeletalParams*)(fn.param_ptr),((CSkeletalMesh::SkeletalParams*)(fn.param_ptr))->BoneSize);
				break;
			case TEXTURE_MATRIX:
				crp->SetTextureMatrixImmediate(0,(CMatrix*)fn.param_ptr);
				break;
			case VERTEX_ANI:
				crp->SetVertexShaderF(ACT_CUSTOM_PARAMS_LIST,(CVertexAniMesh::VertexAniParams*)(fn.param_ptr),1);
				break;
		}
		crp->Render(&(fn.m_Op),&(fn.m_RP));
	}
	m_RenderList.clear();
}
}