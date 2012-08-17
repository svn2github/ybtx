#include "stdafx.h"
#include "CEditNullTexture.h"

CEditNullTexture::CEditNullTexture(const TCHAR* szTexName)
	: CEditTexture(szTexName, 0)
{
	m_TexName = szTexName;
}

CEditNullTexture::~CEditNullTexture()
{
}