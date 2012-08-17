#pragma once
#include "CBaseUnit.h"
#include "CEditModel.h"
#include "CPkgDirectory.h"
namespace sqr_tools
{
	class CChgAniGrpUnit : public CBaseUnit
	{
		DEF_TEST_UNIT(CChgAniGrpUnit);
	public:
		CChgAniGrpUnit(CEditContext* pContext);
		~CChgAniGrpUnit();

		void	_OnTick(void);
		void	UpdateWorkDir(void);
	protected:
		string	m_strWorkPath;
		CPkgDir	m_WorkDir;
		typedef set<string> ANI_POOL;
		ANI_POOL	m_FileList;

	protected:
		uint32 m_Index;
	};
}