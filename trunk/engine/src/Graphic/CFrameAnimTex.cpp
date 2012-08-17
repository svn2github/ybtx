#include "stdafx.h"
#include "CFrameAnimTex.h"


namespace sqr
{
	CFrameAnimTex::CFrameAnimTex( const string& szName, const wstring& szAlias )
		: CTexture2D(szName, szAlias)
	{

	}

	CFrameAnimTex::~CFrameAnimTex()
	{

	}

	const char* CFrameAnimTex::GetType() const
	{
		return CFrameAnimTex::Type();
	}

	const char* CFrameAnimTex::Type()
	{
		return "txfa";
	}

	void CFrameAnimTex::UpdateAnim( float fRadio )
	{

	}
}
