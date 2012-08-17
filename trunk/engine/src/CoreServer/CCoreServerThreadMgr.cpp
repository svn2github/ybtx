#include "stdafx.h"
#include "CCoreServerThreadMgr.h"
#include "CCoreServerThread.h"
#include "CCoreServerJob.inl"
#include "CCoreServerResult.inl"
#include "CCoreConnServerJob.h"
#include "CCoreServerObjJob.h"
#include "CCoreConnServerResult.h"
#include "CCoreServerObjResult.h"
#include "ThreadHelper.h"
#include "ErrLogHelper.h"
#include "CCoreServerSceneJob.h"
#include "CCoreServerSceneResult.h"
#include "CCoreServerSceneJob.h"
#include "CCoreServerSceneResult.h"
#include "CCoreServerAppJob.h"
#include "CCoreServerAppResult.h"
#include "CAppServer.h"
#include "CSyncAppServer.h"
#include "ExpHelper.h"

#include "CCoreTrMirrorBufferMgr.h"
#include "TTrThreadMsgBuffer.inl"
#include "CAsynServerTrMsgBufferSwapper.h"

#include "CSyncTimeSystemServer.h"
#include "CTick.h"
#include "TSqrAllocator.inl"

template class TTrThreadMsgBuffer<CCoreServerThreadTraits>;

#ifdef _WIN32
#pragma warning(disable:4661)
#endif

namespace sqr
{
	class CTrWIBufferSwapedResult : public CCoreServerResult
	{
	public:
		~CTrWIBufferSwapedResult(){} //什么都不干
	};

	class CAddWIBufferSwapedResultTick : public CTick
	{
	public:
		void OnTick()
		{
			if(CCoreServerThreadMgr::Inst()->IntRightBufferEmpty())
				(new CTrWIBufferSwapedResult())->Add();
		}
	};
}

CCoreServerThreadMgr::CCoreServerThreadMgr()
: Parent_t(10240, 1024 * 1024, CAsynServerTrMsgBufferSwapper::Inst())
{
	Inst() = this;

	//为了保证wi buffer持续更新，所以我们每150毫秒加入一个result5	
	m_pWISwapedTick = new CAddWIBufferSwapedResultTick;
	CSyncTimeSystemServer::Inst()->Register(m_pWISwapedTick, 150);

	m_pThread = new CCoreServerThread(this);
}

CCoreServerThreadMgr::~CCoreServerThreadMgr()
{
	delete m_pThread;

	CSyncTimeSystemServer::Inst()->UnRegister(m_pWISwapedTick);
	delete m_pWISwapedTick;
}

void CCoreServerThreadMgr::Init()
{
	if(ms_pInst)
		GenErr("Can't Init because the mgr is already initial");

	new CCoreServerThreadMgr();
}

CCoreServerThreadMgr*& CCoreServerThreadMgr::Inst()
{
	return ms_pInst;
}

void CCoreServerThreadMgr::Unit()
{
	if(!ms_pInst)
		GenErr("Can't Unit because the mgr is already uninitial ");

	delete Inst();
	Inst() = NULL;
}

CCoreServerThreadMgr* CCoreServerThreadMgr::ms_pInst=NULL;


void CCoreServerThreadMgr::OnWakeRight()
{
	CSyncAppServer::Inst()->ReactorCancelBlock();
}

bool CCoreServerThreadMgr::FlushRightMsg()
{
	if( !RightBallOwnedByRight() )
		return false;

	if( IntRightBufferEmpty() )
		return false;

	CCoreTrMirrorBufferMgr::Inst()->OnWBUpdate();

	Parent_t::SwapRightBuffer();

	return true;
}


void CCoreServerThreadMgr::IntOnRightBufferSwaped()
{
	//在处理result之前，我们必须交换ri，得到最新的数据
	CCoreTrMirrorBufferMgr::Inst()->OnRBUpdate();

	Parent_t::IntOnRightBufferSwaped();
}

