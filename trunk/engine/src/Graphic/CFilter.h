#ifndef _CFilter_H_
#define _CFilter_H_

#include "CRenderFilter.h"
#include "FilterDefine.h"
#include "CVertexDeclaration.h"
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
		void Remove(const FilterNode* fn);
		void Render(void);
		void RenderEx(void);
		void Refresh(CRenderFilter* newFilter);
		void SetLightIndex(uint index) {};
	protected:
		SetList				m_RenderList;
		CVertexDeclaration* m_pVD;
		CRenderFilter*		m_pParentFilter;
	};

	//渲染顺序分离器
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
		typedef map<CVertexDeclaration*,CVertexDeclFilter*> FilterMap;
		FilterMap m_Filters;
		ERenderObjStyle m_eRos;
		uint			m_LightIndex;
		CRenderGroup*	m_pRenderGroup;
		CLightSystem*	m_LightSystem;
	};

	//影子分离器
	class CAlphaShadowFilter : public CDefaultFilter
	{
	public:
		CAlphaShadowFilter(ERenderObjStyle eRos,CRenderGroup* pRenderGroup)
			:CDefaultFilter(eRos,pRenderGroup) {};
		FilterNode* Filter(FilterNode* fn);
		//protected:
		//	CRenderFilter* m_pParent;
	};

	class CVertexDeclCopyFilter : public CRenderFilter
	{
	public:
		CVertexDeclCopyFilter(CVertexDeclaration* pVD);
		~CVertexDeclCopyFilter();
	public:
		FilterNode* Filter(FilterNode* fn);
		void Render(void);
		void SetLightIndex(uint index) {};
	protected:
		CopyList	 m_RenderList;
		CVertexDeclaration* m_pVD;
	};

}

#endif