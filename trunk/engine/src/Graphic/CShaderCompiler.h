#pragma once
#include "CStageNodeInst.h"
#include "TSingleton.h"
#include "CGpuProgram.h"

namespace sqr
{
	struct FinalIndex
	{
		ParentLink	m_Diffuse;
		ParentLink	m_Ambient;
		ParentLink	m_Specular;
		ParentLink	m_SpecularPower;
		ParentLink	m_Refract;
	};

	typedef vector<CStageNodeInst>	StateList;

	class CShaderCompiler : public Singleton<CShaderCompiler>
	{
	public:
		CGpuProgram*	Compiler(FinalIndex& Final , StateList& StateList);

	protected:
		virtual	CGpuProgram*	_CreateGpuPro(void)	= 0;
	};
}