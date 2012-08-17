#include "stdafx.h"
#include "CCoreTrMirrorBufferMgr.h"
#include "CTrMirrorBuffer.h"
#include "CTrMirrorPosBuffer.h"
#include "CTrMirrorQuadBuffer.h"
#include "ExpHelper.h"
#include "CAppConfigServer.h"

void CCoreTrMirrorBufferMgr::Init()
{
	if(Inst())
		GenErr("Can't Init because the instance is ready");

	Inst() = new CCoreTrMirrorBufferMgr();
}

void CCoreTrMirrorBufferMgr::Unit()
{
	delete Inst();
	Inst() = NULL;
}

CCoreTrMirrorBufferMgr*& CCoreTrMirrorBufferMgr::Inst()
{
	static CCoreTrMirrorBufferMgr*	ls_pInst = NULL;
	return ls_pInst;
}

CCoreTrMirrorBufferMgr::CCoreTrMirrorBufferMgr()
{
	const uint32 uDataNum = CAppConfigServer::Inst()->GetTripleBufferInitDataNum();

	m_pPosBufferMgr = CTrMirrorPosBuffer::CreateBufferMgr(uDataNum);
	m_pQuadBufferMgr = CTrMirrorQuadBuffer::CreateBufferMgr(uDataNum);
}

CCoreTrMirrorBufferMgr::~CCoreTrMirrorBufferMgr()
{
	m_pPosBufferMgr->Release();
	m_pQuadBufferMgr->Release();
}

void CCoreTrMirrorBufferMgr::OnRBUpdate()
{
	m_pPosBufferMgr->OnRBUpdate();
	m_pQuadBufferMgr->OnRBUpdate();
}

void CCoreTrMirrorBufferMgr::OnWBUpdate()
{
	m_pPosBufferMgr->OnWBUpdate();
	m_pQuadBufferMgr->OnWBUpdate();
}
