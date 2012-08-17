#pragma once
#include "CTexture2D.h"


namespace sqr
{
	class CFrameAnimTex
		: public CTexture2D
	{
	public:
		CFrameAnimTex( const string& szName, const wstring& szAlias );
		~CFrameAnimTex();

		const char*
				GetType() const;

		static const char*
				Type();


		void	UpdateAnim( float fRadio );
	};
}