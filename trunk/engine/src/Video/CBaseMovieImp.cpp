#include "stdafx.h"
#include "CGraphic.h"
#include "CBaseMovieImp.h"
#include "BaseHelper.h"

CBaseMovieImp::CBaseMovieImp()
:m_pTexture(NULL)
{

}

CBaseMovieImp::~CBaseMovieImp()
{
	// 2)  Õ∑≈Ã˘Õº
	SafeRelease(m_pTexture);
}

ITexture * CBaseMovieImp::GetMovieTexture()
{
	return m_pTexture;
}