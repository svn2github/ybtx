#pragma once
#include "CEditContext.h"

namespace sqr_tools
{
	class CCharacterBrowserContext : public CEditContext
	{
	public:
		CCharacterBrowserContext(){}
		~CCharacterBrowserContext(){};
		bool _ProcMsg( const CMsg& msg );
		void _Update( uint32 uTime );
		void _Render( uint32 uTime );
	};
}