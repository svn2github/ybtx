#pragma once
//------------------------------------------------------------------------------
/**
	@class CRefractFilter

	折射过滤器.
	扔进去的模型会被换掉shader和texture.
	因为这样的对象不多, 就不排序了...
 */
#include "CRenderFilter.h"
#include "CFilterNode.h"
#include "CRenderList.h"
//------------------------------------------------------------------------------
namespace sqr
{

class CRefractFilter : public CRenderFilter
{
public:
	CRefractFilter(ERenderObjStyle ros);
	virtual ~CRefractFilter();

	/// 往里扔
	FilterNode* Filter(FilterNode* node);
	/// 画
	void Render();
	/// 这个没用
	void SetLightIndex(uint index);
	/// 是否需要处理
	bool IsValidQueue() const;

	/// 取得特效专用buffer
	virtual CMatrix* GetRenderStack(ERenderStyle RenderStyle,
									ERenderObjStyle RenderObjStyle,
									ITexture* Text0,
									ITexture* Text1,
									EPrimitiveType PrimitiveType,
									uint32 VertexNum,
									uint32 PrimitiveCount,
									uint32 VerFormat,
									uint32 VerStride,
									void** ppVB,
									void** ppIB,
									void** ppRS = NULL,
									DWORD ShaderID = 0);

private:
	STATIC_RS	m_RefractRs;
	CRenderList	m_RenderList;
	CDynamicVB*	effectVB;
};

}// namespace sqr
