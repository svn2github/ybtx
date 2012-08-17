#pragma once
#include "Cmd.h"

namespace sqr_tools
{
	class CTempCmdPool;
	class CTemporaryCmd : public CCmd
	{
		friend class CTempCmdPool;
	public:
		CTemporaryCmd(CTempCmdPool* pPool);
		bool	IsValid(void);
		void	Destroy(void);
	protected:
		void			_SetValid(bool valid);
		bool			m_isValid;		
		CTempCmdPool*	m_pPool;
	};
}