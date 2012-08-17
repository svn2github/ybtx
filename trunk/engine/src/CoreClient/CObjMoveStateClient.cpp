#include "stdafx.h"
#include "CObjMoveStateClient.h"
#include "CTimeSystemClient.h"
#include "CCoreObjectClient.h"
#include "TObjMoveState.inl"

template class TObjMoveState<CTraitsClient>;

CObjMoveStateClient::CObjMoveStateClient(CCoreObjectClient* pObj,CPixelPath* pPath,float fSpeed,float fBeginDist)
:Parent_t(pObj,pPath,fSpeed,fBeginDist)
,m_bMovTickIsStopped(false)
,m_uMoveBeganTime(CTimeSystemClient::Inst()->GetFrameTime())
,m_fMovedDistOfLastSpeed(fBeginDist)
{
}

CObjMoveStateClient::~CObjMoveStateClient(void)
{
	DeleteTickingDerived();
}


#ifdef USE_RENDER_SLOT
void CObjMoveStateClient::OnUpdate(uint64 uLastFrameUsedTime)
{
	SetMoveCycDerived(uint32(uLastFrameUsedTime));
	//CCoreObjectClient* pCoreObj = GetObject();
	//pCoreObj->OnSpeedAdjust(uint32(uLastFrameUsedTime));
	if(!m_bMovTickIsStopped)
		Parent_t::OnTick();	//这句有可能删除this
}
#endif

bool CObjMoveStateClient::NeedSkipFrameDerived()const
{
	if( GetObject()->GetType() != eCOT_Director )
		return false;

	//director对象不允许追帧，为了防止玩家利用这个功能快速移动。
	//cout << "time err " << TimeSystem_t::Inst()->GetTimeError() << " i " << GetInterval() << endl;
#ifdef USE_RENDER_SLOT
	//帧率低于10帧/秒时director无法移动
	const static uint32 ls_uFrameTimeLimit = 1000 * 1000;
	return GetMoveCycDerived() > ls_uFrameTimeLimit;
#else
	return TimeSystem_t::Inst()->GetTimeError() < -500;
#endif
	//return false;
}

void CObjMoveStateClient::SetMoveCycDerived(uint32 uMoveCyc)
{
	m_uMoveCyc = uMoveCyc;
	//m_uMoveCyc = GetSpeed() != 0 ? min( uint32(1000/GetSpeed()), uMoveCyc ) : uMoveCyc;
}

uint32 CObjMoveStateClient::GetMoveCycDerived()const
{
	return m_uMoveCyc;
}

void CObjMoveStateClient::CreateTickingDerived(uint32 uCyc)
{
#ifdef USE_RENDER_SLOT
	//借用CoreObj对象上的虚函数来实现插入不同的渲染驱动队列
	CRenderSlotOwner::Inst().Insert(this);
	m_bMovTickIsStopped = false;
	//Insert();
#else
	//CTimeSystemClient::Inst()->Register( this, uCyc );
	GetObject()->GetDistortedTime()->RegistDistortedTick(this, uCyc);
#endif
}

void CObjMoveStateClient::DeleteTickingDerived()
{
#ifdef USE_RENDER_SLOT
	if(CRenderSlotOwner::Inst().Delete(this))
		m_bMovTickIsStopped = true;
#else
	//CTimeSystemClient::Inst()->UnRegister(this);
	GetObject()->GetDistortedTime()->UnregistDistortedTick(this);
#endif
}

uint32 CObjMoveStateClient::GetInterval()const
{
#ifdef USE_RENDER_SLOT
	return m_uMoveCyc / 1000;
#else
	return m_uMoveCyc;
#endif
}

void CObjMoveStateClient::IntSetSpeedDerived(float fSpeed)
{
	uint64 uCurTime = CTimeSystemClient::Inst()->GetFrameTime();
	Ast(uCurTime >= m_uMoveBeganTime);
	m_fMovedDistOfLastSpeed = limit2(m_fMovedDistOfLastSpeed + sqr::CalMoveStep( GetSpeed(), uint32(uCurTime - m_uMoveBeganTime)), GetMovedDist(), GetPathLength());
	m_uMoveBeganTime = uCurTime;
	//cout << "setspeed " << fSpeed << " dist " << m_fMovedDistOfLastSpeed << "(" << GetSpeed() << "," << uint32(uCurTime - m_uMoveBeganTime) << ")" << endl;
	Parent_t::IntSetSpeedDerived(fSpeed);
}

float CObjMoveStateClient::IntGetNewMoveDistDerived()const
{
	uint64 uCurTime = CTimeSystemClient::Inst()->GetFrameTime();
	Ast(uCurTime >= m_uMoveBeganTime);

	float fMovedDist = m_fMovedDistOfLastSpeed + sqr::CalMoveStep(GetSpeed(), uint32(uCurTime - m_uMoveBeganTime));
	//cout << "new dist " << fMovedDist << " (" << GetSpeed() << "," << uint32(uCurTime - m_uMoveBeganTime) << "," << m_fMovedDistOfLastSpeed << ")" << endl;
	//cout << "IntGetNewMoveDistDerived " << fMovedDist << endl;
	return fMovedDist;
	//return Parent_t::IntGetNewMoveDistDerived();
}

void CObjMoveStateClient::SetNewPathDerived(CPixelPath* pPath,float fBeginDist)
{
	uint64 uCurTime = CTimeSystemClient::Inst()->GetFrameTime();
	m_fMovedDistOfLastSpeed = 0;
	m_uMoveBeganTime = uCurTime;
	Parent_t::IntSetNewPath(pPath, fBeginDist);
}

