#pragma once
#include "CRenderOperation.h"
#include "CRenderMatrix.h"

namespace sqr
{
	class CRenderCommand : public CRenderOperation
	{
	public:
		CRenderCommand();
		CRenderCommand(const CRenderCommand& Com);
		virtual ~CRenderCommand();

		TCHAR*				param_ptr;
		size_t              param_size;

		//‰÷»æ≤Œ ˝
		void				(*param_fun)(CRenderCommand& Com,CRenderMatrix& AniInst);
		void				(*param_render)(CRenderCommand& Com);
		void				(*param_release)(CRenderCommand& Com);

		CRenderOperation&	operator = (const CRenderOperation& op);
		CRenderCommand&		operator=(const CRenderCommand& Com);
	};

	void NullRender(CRenderCommand& Com);
	void NullRelease(CRenderCommand& Com);
	void NullUpdate(CRenderCommand& Com,CRenderMatrix& AniInst);
}