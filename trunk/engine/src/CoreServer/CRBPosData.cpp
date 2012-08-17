#include "stdafx.h"
#include "CRBPosData.h"
#include "CTrMirrorPosBuffer.h"
#include "CMirrorPatchCmd.h"
#include "CMirrorPatch.h"
#include "CMirrorBuffer.h"

namespace sqr
{
	
	class CRBPosDataCreatedCmd : public CMirrorRBPatchCmd
	{
	public:
		CRBPosDataCreatedCmd(uint32 uSlot, const CFPos& pos);
		void Release();
		void HandleCmd(CMirrorBuffer* pBuffer);

	private:
		CFPos	m_Pos;
	};
}

CRBPosDataCreatedCmd::CRBPosDataCreatedCmd(uint32 uSlot, const CFPos &pos)
: CMirrorRBPatchCmd(uSlot)
, m_Pos(pos)
{

}

void CRBPosDataCreatedCmd::Release()
{
	delete this;
}

void CRBPosDataCreatedCmd::HandleCmd(CMirrorBuffer* pBuffer)
{
	pBuffer->RemoveIdleSlot(m_uSlot);

	new (pBuffer, m_uSlot) CRBPosData(m_Pos);
}


////////////////////////////////////////////////////////////////////////
CRBPosData::CRBPosData(const CFPos& pos)
: CMirrorPosData(pos)
{

}

CRBPosData::~CRBPosData()
{

}

CRBPosData* CRBPosData::Create(const CFPos &pos, uint32& uSlot, CTrMirrorBuffer* pMgr)
{
	CMirrorBuffer* pBuffer = pMgr->GetReadBuffer();

	uSlot = pBuffer->WithdrawIdleSlot();
	
	CRBPosData* pData = new(pBuffer, uSlot)CRBPosData(pos);

	CMirrorPatch* pPatch = pMgr->GetCurRBPatch();
	pPatch->AddCmd(new CRBPosDataCreatedCmd(uSlot, pos));

	return pData;
}

void CRBPosData::Release(uint32 uSlot, CTrMirrorBuffer* pMgr)
{

}
