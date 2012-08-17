#pragma once

#include "CmdSetVertexHeight.h"
namespace sqr_tools
{

	class CCmdSetTerrainHeight :public CCmdSetVertexHeight
	{
	public:
		CCmdSetTerrainHeight(CEditContext* pContext);
		~CCmdSetTerrainHeight(void);

		virtual void _Execute();
		virtual void _UnExecute();
	};

//!namespace
}
