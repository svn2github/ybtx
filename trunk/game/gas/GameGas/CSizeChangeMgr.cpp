#include "stdafx.h"
#include "CSizeChangeMgr.h"
#include "CBaseStateServer.h"
#include "CCharacterDictator.h"
#include "CFighterDictator.h"

#define MIN_SIZE_CHANGE_INTERVAL	500
#define MAX_SIZE_CHANGE_COUNT		10

CSizeChangeMgr::CSizeChangeMgr(CFighterDictator* pOwner)
: m_dFinalScale(1.0),
  m_dCurScale(1.0)
{
	Ast(pOwner);
	m_pOwner = pOwner;
}

CSizeChangeMgr::~CSizeChangeMgr()
{

}

void CSizeChangeMgr::ChangeSizeRate(CBaseStateCfgServer* pCfg, int32 iChangeRateCount)
{
	if(pCfg->GetScale() == 1.0f)
		return;

	m_pOwner->UnRegistDistortedTick(this);

#ifdef COUT_SIZE_CHANGE_INFO
	cout << "注销CSizeChangeMgr当前Tick\n";
#endif


	const uint32 uScaleTime = uint32(pCfg->GetScaleTime());
	const double dScale = double(pCfg->GetScale());
	const double dScaleChange = iChangeRateCount == 1 ? dScale : pow(dScale, iChangeRateCount);
	
	m_uScaleMaxCount = uScaleTime / MIN_SIZE_CHANGE_INTERVAL;
	m_uScaleCurCount = 0;

	m_dFinalScale *= dScaleChange;

	if(m_uScaleMaxCount == 0)
	{
		//直接设置Size *= dScaleChange，不用RegisterTick
		m_dCurScale = m_dFinalScale;
		Ast(m_pOwner);
		CCharacterDictator* pChar = m_pOwner->GetCharacter();
		Ast(pChar);
		pChar->StateSetSize();
		pChar->StateSetBottomSize();
		//cout << "在CSizeChangeMgr直接设置Size/BarrierSize = " << (float)m_dCurScale << endl;
	}
	else
	{
		if(m_uScaleMaxCount > MAX_SIZE_CHANGE_COUNT)
			m_uScaleMaxCount = MAX_SIZE_CHANGE_COUNT;
		uint32 uTimeInterval = uScaleTime / m_uScaleMaxCount;
		Ast(uTimeInterval >= MIN_SIZE_CHANGE_INTERVAL);
		m_pOwner->RegistDistortedTick(this, uTimeInterval);

#ifdef COUT_SIZE_CHANGE_INFO
		cout << "[" << uTimeInterval << "]在CSizeChangeMgr注册间隔为" << uTimeInterval << "毫秒的Tick用于渐变设置Size/BarrierSize\n";
#endif

	}
}

void CSizeChangeMgr::OnTick()
{
	Ast(m_uScaleCurCount < m_uScaleMaxCount);
	++m_uScaleCurCount;
	if(m_uScaleCurCount == m_uScaleMaxCount)
	{
		m_dCurScale = m_dFinalScale;
		m_pOwner->UnRegistDistortedTick(this);
	}
	else
	{
		m_dCurScale *= pow(m_dFinalScale / m_dCurScale, 1.0 / (double)(m_uScaleMaxCount - m_uScaleCurCount + 1));
	}
	Ast(m_pOwner);
	CCharacterDictator* pChar = m_pOwner->GetCharacter();
	Ast(pChar);
	pChar->StateSetSize();
	pChar->StateSetBottomSize();
	//cout << "在CSizeChangeMgr第" << m_uScaleCurCount << "次渐变设置Size/BarrierSize = " << (float)m_dCurScale << endl;
}

double CSizeChangeMgr::GetStateZoomRate()
{
	return m_dCurScale;
}

