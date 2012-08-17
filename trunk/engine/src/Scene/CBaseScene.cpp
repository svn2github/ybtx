#include "stdafx.h"
#include "CBaseScene.h"
#include "BaseMath.h"
#include "CRegion.h"
#include "BaseHelper.h"
#include "CGraphicExp.h"
#include "CTreeScene.h"
#include "TSqrAllocator.inl"
//----------------------------------------------------------------------------

CBaseScene::CBaseScene( const SString& SceneName, CTreeScene* pTreeScene )
: m_strSceneName( SceneName )
, m_ImpWidth(0)
, m_ImpHeight(0)
, m_XPos(0xffff)
, m_YPos(0xffff)
, m_uWidth(0)
, m_uHeight(0)
, m_isInitialize(false)
, m_ImpHalfWidth(0)
, m_ImpHalfHeight(0)
, m_RecycleSize(5)
{
	AddElementManager(pTreeScene);
}


CBaseScene::CBaseScene( const SString& SceneName, CTreeScene* pTreeScene , uint16 Wdith, uint16 Height, uint32 RecycleSize )
: m_strSceneName( SceneName )
, m_ImpWidth(Wdith)
, m_ImpHeight(Height)
, m_XPos(0xffff)
, m_YPos(0xffff)
, m_uWidth(0)
, m_uHeight(0)
, m_isInitialize(false)
, m_ImpHalfWidth(0)
, m_ImpHalfHeight(0)
, m_RecycleSize( RecycleSize )
{
	AddElementManager(pTreeScene);
}

CBaseScene::~CBaseScene()
{
	RegionPool::iterator it,eit = m_RegionMgr.end();
	for( it = m_RegionMgr.begin(); it!=eit; ++ it )
		delete it->second;
	m_RegionMgr.clear();
	ClearRecycle();

	for( size_t i = 0; i < m_vecEleMgrs.size(); ++i )
		m_vecEleMgrs[i]->Destroy();
}

bool CBaseScene::SetActivePos(int16 X, int16 Y)
{
	uint32 Width = m_ImpHalfWidth + 1;
	uint32 Height = m_ImpHalfHeight + 1;
	if( m_uWidth>m_ImpHalfWidth+1 )
	{
		X = limit2(X, int16(m_ImpHalfWidth) , int16(m_uWidth  - 1 - m_ImpHalfWidth));
		Width = min(Width + m_ImpHalfWidth,m_uWidth);
	}
	else
		X = m_ImpHalfWidth;
	

	if(m_uHeight>m_ImpHalfHeight+1)
	{
		Y = limit2(Y, int16(m_ImpHalfHeight)	, int16(m_uHeight - 1 - m_ImpHalfHeight));
		Height = min( Height + m_ImpHalfHeight,m_uHeight );
	}
	else
		Y = m_ImpHalfHeight;

	if( X == m_XPos && Y == m_YPos )
		return true;

	URegionID orgID(X- m_ImpHalfWidth, Y - m_ImpHalfHeight); 

	//以下程序先这样写 高效率的正在酝酿中
	bool bSync = m_RegionMgr.find(URegionID(X,Y)) == m_RegionMgr.end();
	RegionPool::iterator it,eit = m_RegionMgr.end();
	for( it = m_RegionMgr.begin(); it!=eit; ++ it )
	{
		m_RegionRecycle.AddNode(it->second);
		it->second->ToRecycle();
	}
	m_RegionMgr.clear();

	for( uint32 h = 0; h < Height; ++h )
	{
		for( uint32 w = 0; w < Width ; ++w )
		{
			CRegion* pRegion = m_RegionRecycle.PopNode(orgID);
			if( NULL != pRegion)
			{
				m_RegionMgr.insert(RegionPool::value_type(orgID,pRegion));
				pRegion->SetValid();
			}
			else
			{
				//bool bSync = (X == orgID.X && Y == orgID.Z);
				pRegion = new CRegion(this,orgID);
				m_RegionMgr.insert(RegionPool::value_type(orgID,pRegion));
				for( size_t i=0; i< m_vecEleMgrs.size(); ++i)
					pRegion->AddCtrlNode( m_vecEleMgrs[i]->CreateNode(orgID),bSync );
			}
			++(orgID.X);
		}
		++(orgID.Z);
		orgID.X = X- m_ImpHalfWidth;
	}

	m_XPos = X;
	m_YPos = Y;

	m_RegionRecycle.OptimizeRecycle(m_RecycleSize);
	return true;
}

void CBaseScene::ClearRecycle(void)
{
	CRegion* pRegion = NULL;
	while( pRegion = m_RegionRecycle.PopNode() )
		GfkSafeDelete(pRegion);
}

void CBaseScene::OptimizeRecycle(void)
{
	m_RegionRecycle.OptimizeRecycle(m_RecycleSize);
	//if( m_RegionRecycle.size() < 1 )
	//	return;

	//RegionPool::iterator it,eit = m_RegionRecycle.end();
	//RegionPool::iterator maxIt;

	//int32 MaxDis = 0;
	//int32 MaxY = 0;
	//int32 tempX,tempY;
	//for( it = m_RegionRecycle.begin(); it != eit; ++it )
	//{
	//	tempX = it->first % m_uWidth;
	//	tempY = it->first / m_uWidth;

	//	tempX = tempX - m_XPos;
	//	tempY = tempY - m_YPos;

	//	tempX *= tempX;
	//	tempY *= tempY;

	//	tempX += tempY;
	//	if( MaxDis < tempX )
	//	{
	//		maxIt = it;
	//		MaxDis = tempX;
	//	}
	//}

	//delete maxIt->second;
	//m_RegionRecycle.erase(maxIt);
}

bool CBaseScene::AddElementManager( CElementManager* pEleMgr )
{
	for( size_t i = 0; i < m_vecEleMgrs.size(); ++i )
	{
		if( m_vecEleMgrs[i] == pEleMgr )
			return true;
	}
	
	m_vecEleMgrs.push_back( pEleMgr );

	RegionPool::iterator it,eit = m_RegionMgr.end();
	for( it = m_RegionMgr.begin(); it!=eit; ++ it )
		it->second->AddCtrlNode(pEleMgr->CreateNode(it->second->GetID()),true);

	ClearRecycle();
	return true;
}

bool CBaseScene::DelElementManager( CElementManager* pEleMgr )
{
	for( size_t i = 1; i < m_vecEleMgrs.size(); ++i )
	{
		if( m_vecEleMgrs[i] == pEleMgr )
		{
			RegionPool::iterator it,eit = m_RegionMgr.end();
			for( it = m_RegionMgr.begin(); it!=eit; ++ it )
				it->second->DelCtrlNode(pEleMgr);
			ClearRecycle();
			m_vecEleMgrs[i]->Destroy();
			m_vecEleMgrs.erase( m_vecEleMgrs.begin() + i );
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------------------------------
CRegionRecycle::CRegionRecycle()
{
	m_FreeTime = 60 * 1000 * 1000; //暂时设定成1分钟，以后可以放入配置文件
}

void CRegionRecycle::AddNode(CRegion* fp)
{
	m_RecycleList.push_back(RecycleNode(fp));
	NodeList::iterator it = --m_RecycleList.end();
	pair<NodeID_Map::iterator,bool> p = m_IdMap.insert(NodeID_Map::value_type(fp->GetID(),it));
	(*it).IdIt = p.first;
}

CRegion* CRegionRecycle::PopNode(URegionID ID)
{
	NodeID_Map::iterator it = m_IdMap.find(ID);
	if(it!=m_IdMap.end())
	{
		CRegion* pRegion = (*(it->second)).pRegion;
		m_RecycleList.erase(it->second);
		m_IdMap.erase(it);
		return pRegion;
	}
	return NULL;
}

CRegion* CRegionRecycle::PopNode(void)
{
	if(m_RecycleList.size() == 0)
		return NULL;

	NodeList::iterator it = m_RecycleList.begin();
	CRegion* pRegion = (*it).pRegion;
	m_IdMap.erase((*it).IdIt);
	m_RecycleList.erase(it);
	return pRegion;
}

void CRegionRecycle::OptimizeRecycle(uint32 size)
{
	int32 delNum = m_RecycleList.size() - size;
	NodeList::iterator it,eit = m_RecycleList.end();
	uint64 tNow = GetHDProcessTime();

	CRegion* pRegion = NULL;
	SQR_TRY
	{
		for( it = m_RecycleList.begin(); it!=eit; )
		{
			if(delNum>0)
			{
				pRegion = (*it).pRegion;
				m_IdMap.erase((*it).IdIt);
				m_RecycleList.erase( it++ );
				--delNum;
				delete pRegion;
				continue;
			}

			if( (tNow - (*it).time)>m_FreeTime )
			{
				pRegion = (*it).pRegion;
				m_IdMap.erase((*it).IdIt);
				m_RecycleList.erase( it++ );
				delete pRegion;
				continue;
			}
			break;
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendType("CRegionRecycle::OptimizeRecycle");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
}