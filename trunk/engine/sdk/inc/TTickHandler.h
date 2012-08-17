//$Id: TTickHandler.h 55929 2007-04-29 08:11:21Z shhuang $
#pragma once

#include "CTick.h"

namespace sqr
{
	template<typename HolderType>
	class TTickHandler
		:public CTick
	{
	private:
		HolderType*	m_pHolder;
	public:
		void*		m_pArg;

		TTickHandler(HolderType* pHolder,void* pArg = NULL)
			:m_pHolder(pHolder),m_pArg(pArg)
		{
		}

		virtual void OnTick()
		{
			return m_pHolder->OnTick(this);
		}
	};



	template<typename HolderType, void(HolderType::*pFun)() >
	class TTickFunctionHandler
		:public CTick
	{
	private:
		HolderType*	m_pHolder;
	public:

		TTickFunctionHandler(HolderType* pHolder)
			:m_pHolder(pHolder)
		{
		}

		virtual void OnTick()
		{
			return (m_pHolder->*pFun)();
		}
	};
}


