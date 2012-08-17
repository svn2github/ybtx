#include "stdafx.h"
#include "CMirrorRBPatchMgr.h"
#include "CMirrorRBPatch.h"

CMirrorRBPatchMgr::CMirrorRBPatchMgr()
{
	m_pPatch = new CMirrorRBPatch();
}

CMirrorRBPatchMgr::~CMirrorRBPatchMgr()
{
	delete m_pPatch;
}


CMirrorRBPatch* CMirrorRBPatchMgr::GetCurPatch()
{
	return m_pPatch;
}

void CMirrorRBPatchMgr::Patch(CMirrorBuffer* pBuffer)
{
	m_pPatch->Patch(pBuffer);
}
