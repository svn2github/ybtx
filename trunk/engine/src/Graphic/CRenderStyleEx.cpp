#include "stdafx.h"
#include "CRenderStyleEx.h"
#include "CStageNodeInst.h"
#include "CShaderCompiler.h"

namespace sqr
{
	CRenderStyleEx::CRenderStyleEx()
		: CResUnit("",L"",eRT_eRs)
		, m_fTimeRange(0.0f)
		, m_bSync(false)
	{

	}

	CRenderStyleEx::~CRenderStyleEx()
	{

	}

	//void CRenderStyleEx::BinLoad(void)
	//{
	//	FinalIndex		finalIndex;
	//	StateList		sList;

	//	m_pPixelShader = CShaderCompiler::GetInst()->Compiler(finalIndex, sList);
	//}

	//void CRenderStyleEx::BinSave(void)
	//{

	//}

	float CRenderStyleEx::GetTimeRange() const
	{
		return m_fTimeRange;
	}

}
