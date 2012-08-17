//$Id: CTickMgr.cpp 54851 2007-04-11 13:08:43Z shhuang $
#include "stdafx.h"
#include "CTickMgr.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "CTickSlot.h"
#include "TimeHelper.h"
#include "CTickQueue.h"
#include "TimeHelper.h"
#include "CodeCvs.h"
#include "CTickImp.h"
#include "ErrLogHelper.h"
#include "TObjectBank.inl"
#include "CLog.h"
#include "TSqrAllocator.inl"
#include "StringHelper.h"
#include "CSqrAllocPool.h"
#include "TIDPtrMap.inl"


CTickMgr::CTickMgr(uint32 uTickTime,uint16 uSlotNum)
{
	CTickSlot* pTickSlot;
	if(uTickTime<=0 || uSlotNum <= 0)
		GenErr("uTickTime==0||uSlotNum==0");
	m_uCurPos = 0;
	m_uRing = 0;
	m_uInterval = uTickTime;
	m_pLastTickImp = NULL;
	m_pLastTickQueue = NULL;

	m_vecSlot.reserve( uSlotNum );

	for(uint16 i = 0;i < uSlotNum;i++)
	{
		pTickSlot = new CTickSlot;
		m_vecSlot.push_back(pTickSlot);
	}

	m_pTickQueueBank=new QueueBank_t;
	m_pTickImpBank=new ImpBank_t;
}

CTickMgr::~CTickMgr()
{
	PrintAndUnregisterTick();

	for(VecTickSlot_t::iterator it = m_vecSlot.begin();it != m_vecSlot.end();++it)
	{
		(*it)->ClearTick();
		delete *it;
	}
	m_vecSlot.clear();
	
	delete m_pTickQueueBank;
	delete m_pTickImpBank;
}

inline bool CTickMgr::CallTick(CTickImp* pTickImp)
{
	m_pLastTickImp = pTickImp;

	pTickImp->m_pTick->OnTick();

	if(m_pLastTickImp == NULL)//被重新注册，或者被自己析构
		return false;
	m_pLastTickImp = NULL;
	return true;
}

void CTickMgr::OnCaughtTickException(CTick* pTick)
{
	if(!m_pLastTickImp)
		return;

	UnRegister( pTick );
}


void CTickMgr::OnTick()
{
	uint32 uNewPos;

	CTickSlot* pCurSlot=m_vecSlot[m_uCurPos];

	CTickImp* pTickImp = NULL;

	const size_t stSlotSize=m_vecSlot.size();

	CTickQueue* pQueue=pCurSlot->GetTickQueue( m_uRing );

	if( pQueue )
	{
		m_pLastTickQueue = pQueue;
		size_t i = 0;
		do 
		{
			SQR_TRY
			{
				for(  ; i<pQueue->size() ; )
				{
					pTickImp = pQueue->at( i++ );

					if( !pTickImp )
						continue;

					const uint32 uTickInterval = pTickImp->m_uInterval;

					if( uTickInterval < m_uInterval )
					{
						//Tick的间隔小于TickMgr的基本间隔，因此要调用多次OnTick

						uint32 uTotalTime = pTickImp->m_uLoan;

						for( int j=0;uTotalTime < m_uInterval ; uTotalTime += uTickInterval,++j )
						{
							if( j>100 )
							{
								ostringstream strm;
								strm << pTickImp->GetTickName() << " tick loop more than 100 times,auto unregistered." <<endl
									<< " tick interval:"<<uTickInterval << " totaltime:" <<uTotalTime; 
								UnRegister(pTickImp->m_pTick);
								LogErr(strm.str());
								break;
							}
							else
							{
								if( !CallTick( pTickImp ) )
									break;
							}
						}

						if( uTotalTime < m_uInterval )
							continue;		//该Tick删除了自己

						pTickImp->m_uLoan = uTotalTime - m_uInterval;

						uNewPos = m_uCurPos + 1;	//因为Interval小于TickMgr的最小间隔，所以下一个位置只需加1即可，无需计算。
					}
					else
					{
						//Tick的间隔大于TickMgr的基本间隔

						if( !CallTick( pTickImp ) )
							continue;	//Tick删除了自己

						//pTickImp->m_uLoan为补的值，pTickImp->m_uInterval % m_uInterval为每轮多的值
						//如果补的值比多的值大的话，就说明可以提前调用。

						//计算下一段跳跃的时间距离，要扣除上次亏欠的时间
						const uint32 uTotalTime = pTickImp->m_uInterval - pTickImp->m_uLoan;

						uint32 uOffset = uTotalTime / m_uInterval;
						const uint32 uExtra = uTotalTime - m_uInterval * uOffset;

						if( uExtra > 0 )
						{
							++uOffset;
							pTickImp->m_uLoan = m_uInterval - uExtra;
						}
						else
						{
							pTickImp->m_uLoan = 0;
						}				

						uNewPos = m_uCurPos + uOffset;
					}

					const uint16 uNewSlot = uint16( uNewPos % stSlotSize );
					const uint32 uNewRing = uNewPos / stSlotSize + m_uRing;
					pTickImp->AddToSlot( uNewSlot, uNewRing );
				}

				break;
			}
			SQR_CATCH(exp)
			{
				exp.AppendMsg(pTickImp->GetTickName());
				LogExp(exp);

				OnCaughtTickException(pTickImp->m_pTick);
			}
			SQR_TRY_END;

		} while(true);

		pCurSlot->DelTickQueue( pQueue ,m_pTickQueueBank );
		m_pLastTickQueue = NULL;
	}

	if( m_uCurPos + 1 == stSlotSize )
	{
		m_uCurPos = 0;
		++m_uRing;
	}
	else
	{
		m_uCurPos++;
	}
}


void CTickMgr::Register(CTick* pTick,uint32 uTickInterval)
{
	if( !pTick )
		GenErr("pTick==NULL");
	
	if( pTick->m_pImp )
		UnRegister( pTick );
	
	CTickImp* pTickImp = m_pTickImpBank->PopOrNew();
	
	pTickImp->m_pTick = pTick;
	
	pTick->m_pImp = pTickImp;

	//清空sTickName，防止取得的是上次使用tick的名字
	pTickImp->m_sTickName.clear();
	pTickImp->m_szClassName=pTick->GetTickClassName();
	
	AdvAddRef(pTick);

	pTickImp->m_pTickMgr = this;
	
	pTickImp->m_uKeyOfTickMap = m_mapRegisteredTick.Add( pTickImp );

	pTickImp->m_uInterval = uTickInterval;

	uint32 uNewPos = uTickInterval / m_uInterval;

	
	if( uNewPos != 0 )
	{
		const uint32 uExtra = uTickInterval - m_uInterval * uNewPos;

		if( uExtra > 0 )
		{
			++uNewPos;
			pTickImp->m_uLoan = m_uInterval - uExtra;
		}
		else
		{
			pTickImp->m_uLoan = 0;
		}

		const size_t stSlotSize=m_vecSlot.size();
		const uint16 uSlot =uint16(( uNewPos + m_uCurPos) % stSlotSize);
		const uint32 uRing =( uNewPos + m_uCurPos ) / stSlotSize + m_uRing;
	
		pTickImp->AddToSlot( uSlot , uRing );
	}
	else
	{
		const uint16 uSlot = static_cast<uint16>(m_uCurPos);
		pTickImp->AddToSlot( uSlot , m_uRing );

		pTickImp->m_uLoan = 0;
	}
}

void CTickMgr::UnRegister(CTick* pTick)
{
	if( !pTick )
		return;

	CTickImp* pTickImp = pTick->m_pImp;
	if( !pTickImp )
		return;

	if( pTickImp->m_pTick != pTick )
	{
		ostringstream strm;
		strm<<"地址:"<<pTick;
		GenErr("野指针Tick",strm.str());
	}		

	if( m_pLastTickImp == pTickImp )
		m_pLastTickImp = NULL;

	m_mapRegisteredTick.Del(pTickImp->m_uKeyOfTickMap);
	AdvDelRef(pTick);

	pTickImp->DelFrSlot();
	pTickImp->m_pTick=NULL;
	pTickImp->m_uKeyOfTickMap = uint32(-1);
	m_pTickImpBank->Push( pTickImp );
	pTick->m_pImp = NULL;
}

uint32 CTickMgr::GetInterval()const
{
	return m_uInterval;
}

size_t CTickMgr::GetRegisterTickNum()
{
	return m_mapRegisteredTick.Size();
}

void CTickMgr::GetRegisterTickInfo(string& strInfo)
{
	MapTickImp_t::Iterator it1 = m_mapRegisteredTick.Begin();
	MapTickImp_t::Iterator it2 = m_mapRegisteredTick.End();
	if (it1 == it2)
		return ;

	typedef map<string ,int> MapListInfo_t;
	MapListInfo_t	mapListInfo;

	for (; it1 != it2; ++it1)
	{
		++mapListInfo[it1->GetTickName()];
	}

	size_t stSize = mapListInfo.size();

	strInfo.reserve(stSize * 35);

	MapListInfo_t::iterator iter1 = mapListInfo.begin();
	MapListInfo_t::iterator iter2 = mapListInfo.end();

	char buf[1024];

	for(size_t i = 0; iter1 != iter2; ++iter1, ++i)
	{
	
		sprintf(buf, "%-20s %d\n", (*iter1).first.c_str(), (*iter1).second);

		strInfo.append(buf);
	}

	
}

void CTickMgr::PrintAndUnregisterTick()
{
	MapTickImp_t::Iterator it1 = m_mapRegisteredTick.Begin();
	MapTickImp_t::Iterator it2 = m_mapRegisteredTick.End();
	if (it1 == it2)
		return ;

	map< string,int > mapListInfo;

	for (; it1 != it2; ++it1)
	{
		++mapListInfo[it1->GetTickName()];
		it1->UnRegister();
	}

	if( !CatchErrorEnabled() )
	{
		const char* szMessage="进程关闭时有Tick尚未被注销";

		ostringstream oss;
	
		for(map<string, int>::const_iterator it=mapListInfo.begin();
			it != mapListInfo.end(); ++it)
		{
			oss << it->first << ":" << it->second<<endl;
		}

#ifdef _WIN32
		MessageBox(NULL,oss.str().c_str(),szMessage,MB_OK|MB_ICONERROR);
#else
		cerr<<szMessage<<endl<<oss.str();
#endif
	}
	else
	{
		for(map<string, int>::const_iterator it=mapListInfo.begin();
			it != mapListInfo.end(); ++it)
		{
			ostringstream oss;
			oss<<"Tick("<<it->first << ")在进程关闭时尚未被注销";

			string sCount;
			tostring(sCount,it->second );

			LogErr( oss.str(),sCount );
		}
	}
}
