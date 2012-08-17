#pragma once
#include "CDynamicObject.h"

namespace sqr
{
	class ISend :public virtual CDynamicObject
	{
	public:
		template<typename CmdType>
		bool SendCmd(const CmdType *pCmdType)
		{
			return Send(pCmdType,sizeof(CmdType)) == sizeof(CmdType);
		}

		virtual uint32 Send(const void* pData,uint32 uSize)=0;
	};
}
