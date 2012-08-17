#pragma once

namespace sqr
{
	class CEventOwner;
}

namespace sqr_tools
{
	class CCtrlBase;

	class ICtrlImpFactory
	{
	public:
		virtual CCtrlBase*  CreateImp( char* class_name, void* pParam ) = 0;
	};
}