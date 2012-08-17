#pragma once
#include "CSkyMgr.h"

namespace sqr_tools
{
	class CEditSkyMgr : public CSkyMgr
	{
	public:
		CEditSkyMgr(CRenderTarget* pTarget);
		~CEditSkyMgr();

		void SetEditSkyTextureName( const string& name );
		void SetEditSkyUVOffset( const float u, const float v );
	};
}