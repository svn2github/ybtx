#include "stdafx.h"
#include "CDefaultFilter.h"
#include "CVertexDeclFilter.h"
#include "CRenderGroup.h"
#include "BaseHelper.h"
#include "TSqrAllocator.inl"

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
		FilterMap::iterator it = m_Filters.find(fn->vertexData->vertexDeclaration);
		if (it==m_Filters.end())
		{
			CVertexDeclFilter* pVDF = new CVertexDeclFilter(fn->vertexData->vertexDeclaration,this);
			m_Filters.insert(FilterMap::value_type(fn->vertexData->vertexDeclaration,pVDF));
			return pVDF->Filter(fn);
		}
		it->second->Filter(fn);
		return fn;
	}

	void CDefaultFilter::Render(void)
	{
		m_LightSystem->TurnOnGlobalLight(m_LightIndex);
		FilterMap::iterator it = m_Filters.begin();
		FilterMap::iterator ite = m_Filters.end();
		for ( ;it!=ite;++it)
			(it->second)->Render();
	}

	void CDefaultFilter::RenderEx(void)
	{
		m_LightSystem->TurnOnGlobalLight(m_LightIndex);
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
}