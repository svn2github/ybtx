#pragma once
#include "CEventDelegate.h"

namespace sqr_tools
{
	class CMapDelegate : public CEventDelegate
	{
	public:
		CMapDelegate(){}
		~CMapDelegate(){};
		void	_Init(CEventOwner* pOwner);
		bool	MsgProc(const CMsg& msg);
	};
}