#pragma once
#include "CRenderStyle.h"
#include "CEditSubject.h"
#include "CEditInfo.h"
#include "CEditRsFormat.h"

namespace sqr_tools
{
	class CEditRenderStyle 
		: public sqr::CRenderStyle
		, public CEditSubject 
		, public CEditInfo
	{		
	public:
		static CEditRsFormat Format;

		CEditRenderStyle( const string& szName, const wstring& szAlias );
		CEditRenderStyle( const CRenderStyle& Style );
		CEditRenderStyle(void);
		int BinSave( const string& SaveName );
		int	ReLoad( const string& FileName );

		//CEditRenderStyle& operator = (const CEditRenderStyle& style);
	};
}