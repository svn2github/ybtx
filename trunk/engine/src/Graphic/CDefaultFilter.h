#pragma once
#include "CRenderFilter.h"
#include "FilterDefine.h"
namespace sqr
{
	class CVertexDeclFilter;
	//‰÷»æÀ≥–Ú∑÷¿Î∆˜
	class CDefaultFilter : public CRenderFilter
	{
	public:
		CDefaultFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup);
		virtual ~CDefaultFilter();
	public:
		virtual FilterNode* Filter(FilterNode* fn);
		virtual void Render(void);
		virtual void RenderEx(void);
		virtual	void Refresh(CRenderFilter* newFilter);
		void	SetLightIndex(uint index)
		{
			m_LightIndex = index;
		}
	protected:
		typedef GMap<CVertexDeclaration*,CVertexDeclFilter*> FilterMap;
		FilterMap m_Filters;
		ERenderObjStyle m_eRos;
		uint			m_LightIndex;
		CRenderGroup*	m_pRenderGroup;
		CLightSystem*	m_LightSystem;
	};
}