#pragma once
#include "CBaseUnit.h"
#include "CPkgDirectory.h"
#include <stack>
#include "CEfxInfoReader.h"

namespace sqr_tools
{
	class CEfxTestUnit : public CBaseUnit
	{
		DEF_TEST_UNIT(CEfxTestUnit);
	public:
		CEfxTestUnit(CEditContext* pContext);
		~CEfxTestUnit();

		void	_OnTick(void);
		void	UpdateWorkDir(void);
	protected:
		bool	m_isErgDir;
		string	m_EfxFile;
		uint32	m_uCount;

	protected:
		struct CDirNode
		{
			CDirNode(const string& dir);
			CPkgDir m_pDir;
			uint32	m_uIndex;
		};

		typedef		stack<CDirNode> DirStack;
		DirStack	m_ErgDirPool;
		uint32		m_ModelIdx;
		CEfxInfoReader	m_EfxInfo;
		uint32			m_EfxIdx;
	};
}