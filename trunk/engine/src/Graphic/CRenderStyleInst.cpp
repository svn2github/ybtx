#include "stdafx.h"
#include "CRenderStyleInst.h"

CRenderStyleExInst::CRenderStyleExInst(CRenderStyleEx* pRenderStyle,TexturesPool* pTexturePool)
{

}

CRenderStyleExInst::~CRenderStyleExInst()
{

}

void CRenderStyleExInst::Execute(void)
{

}

void CRenderStyleExInst::UnExecute( void )
{

}

ITexture* 
CRenderStyleExInst::GetTexture( uint32 idx )
{
	if ( idx >= m_TexturePool.size() )
		return NULL;

	return m_TexturePool[idx];		
}

float CRenderStyleExInst::GetTimeRange() const
{
	return m_pRenderStyle->GetTimeRange();
}

bool CRenderStyleExInst::GetIsSync() const
{
	return m_pRenderStyle->GetIsSync();
}

void CRenderStyleExInst::Update( uint32 uFrameTime, uint32 uCurTime )
{
	float pos;
	if ( !m_pRenderStyle->GetIsSync() )
	{
		pos = ( uCurTime + uint32( this ) ) / m_pRenderStyle->GetTimeRange();
		pos -= (int)pos;
	}
	else
		pos = uFrameTime / m_pRenderStyle->GetTimeRange();
}

bool CRenderStyleExInst::PushModify(const CModifyRS* pMrs)
{
	if(pMrs==NULL)
		return false;
	return true;
}

bool CRenderStyleExInst::PopModify(const CModifyRS* pMrs)
{
	return true;
}