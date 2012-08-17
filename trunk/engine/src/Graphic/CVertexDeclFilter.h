#pragma once
#include "CRenderFilter.h"
#include "FilterDefine.h"
#include "CVertexDeclaration.h"
#include "CFilterNode.h"
#include "CRenderList.h"

namespace sqr
{
	//最后一级筛选
	class CVertexDeclFilter : public CRenderFilter
	{
	public:
		CVertexDeclFilter(CVertexDeclaration* pVD , CRenderFilter* pParent);
		~CVertexDeclFilter();
	public:
		FilterNode* Filter(FilterNode* fn);
		void Render(void);
		void RenderEx(void);
		void Refresh(CRenderFilter* newFilter);
		void SetLightIndex(uint index) {};
	protected:
		CRenderList			m_RenderList;
		CVertexDeclaration* m_pVD;
		CRenderFilter*		m_pParentFilter;
	};
}