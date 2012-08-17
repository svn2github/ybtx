#include "stdafx.h"
#include "CEditSkyMgr.h"

CEditSkyMgr::CEditSkyMgr(CRenderTarget* pTarget)
: CSkyMgr(pTarget)
{

}

CEditSkyMgr::~CEditSkyMgr()
{

}

void CEditSkyMgr::SetEditSkyTextureName( const string& name )
{
	CSkyMgr::SetSkyBoxTextureName(name);
}

void CEditSkyMgr::SetEditSkyUVOffset( const float u, const float v )
{
	CSkyMgr::SetUVOffset(u, v);
}