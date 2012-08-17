#pragma once
#include "CBaseUnit.h"
#include "CPkgDirectory.h"

namespace sqr_tools
{
	class CChangeSceneUnit
		: public CBaseUnit
	{
		DEF_TEST_UNIT(CChangeSceneUnit);
	public:
		CChangeSceneUnit( CEditContext* pContext );
		~CChangeSceneUnit();

		void	UpdateWorkDir(void);
	
	protected:
		void	_OnTick(void);

	private:
		string		m_strWorkPath;
		CPkgDir		m_WorkDir;
		uint32		m_uPreIndex;
		uint32		m_uCurIndex;
	};
}