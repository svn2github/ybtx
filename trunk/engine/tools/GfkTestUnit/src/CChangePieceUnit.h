#pragma once
#include "CBaseUnit.h"
#include "CEditModel.h"
#include "CPkgDirectory.h"

namespace sqr_tools
{
	class CChangePieceUnit : public CBaseUnit
	{
		DEF_TEST_UNIT(CChangePieceUnit);
	public:
		CChangePieceUnit(CEditContext* pContext);
		~CChangePieceUnit();

		void	UpdateWorkDir(void);
	protected:
		void	_OnTick(void);

		string	m_strWorkPath;
		CPkgDir	m_WorkDir;
		bool	m_bIsRandom;
		bool	m_bIgnoreHasPiece;
		

	protected:
		uint32	m_uLastIndex;
		uint32	m_uModelIndex;
	};
}
