#pragma once
/**
	@class CVertexShadowJudge

	顶点阴影鉴定.
	判断一个顶点是否处于阴影之内, ShadowMap的CPU实现

	(C) 2009 ThreeOGCMan
*/
#include "CRefObject.h"
#include "GraphicBase.h"
#include "MathAlgorithm.h"

namespace sqr
{
	class CRenderTexture;
	SmartPointer(CVertexShadowJudge);

	class CVertexShadowJudge : public CRefObject
	{
	public:
		CVertexShadowJudge();
		virtual ~CVertexShadowJudge();

		/// 开始比较
		void Begin(CRenderTexture* target, const CMatrix& shadowMatrix);
		/// 判断一个点是否处于阴影之中
		bool IsInShadow(const CVector3f& point);
		/// 结束比较
		void End();

	private:
		/// the same to HLSL
		float tex2Dproj(const CVector4f& texCoord);
	private:
		ITexture*	m_pDepthTarget;
		CMatrix		m_ShadowMatrix;
		bool		m_isLocking;
		float*		m_pDepthData;
	};
}
