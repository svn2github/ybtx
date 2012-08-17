#include "stdafx.h"
#include "CTexture2DImp.h"
#include "GraphicRenderStyle.h"

namespace sqr
{
	

	// ---- CTexture2DImp ------------------------------------------------------------------------

	CTexture2DImp::CTexture2DImp()
		: m_bIsLoaded(false)
	{

	}

	CTexture2DImp::~CTexture2DImp()
	{

	}

	bool CTexture2DImp::IsLoaded() const
	{
		return m_bIsLoaded;
	}

}
