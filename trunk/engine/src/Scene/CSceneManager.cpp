#include "stdafx.h"
#include "CSceneManager.h"
#include "BaseMath.h"
#include "CRegion.h"
#include "CTrace.h"
#include "BaseHelper.h"
#include "TNedAllocator.inl"

CSceneCoder::CSceneCoder(void)
: m_pLockScene(NULL)
{
}

bool CSceneCoder::Lock(CSceneManager* pScene)
{
	if(m_pLockScene==NULL)
	{
		m_pLockScene = pScene;
		return true;
	}
	return false;
}

void CSceneCoder::UnLock(void)
{
	m_pLockScene = NULL;
}

//----------------------------------------------------------------------------

CSceneManager::CSceneManager( const string& SceneName )
: m_strSceneName( SceneName )
, m_ImpWidth(0)
, m_ImpHeight(0)
, m_XPos(0)
, m_YPos(0)
, m_uWidth(0)
, m_uHeight(0)
, m_isInitialize(false)
, m_ImpHalfWidth(0)
, m_ImpHalfHeight(0)
, m_RecycleSize(5)
, m_pClientCoder(NULL)
{
}


CSceneManager::CSceneManager( const string& SceneName , uint32 Wdith, uint32 Height, uint32 RecycleSize )
: m_strSceneName( SceneName )
, m_ImpWidth(Wdith)
, m_ImpHeight(Height)
, m_XPos(0xffffffff)
, m_YPos(0xffffffff)
, m_uWidth(0)
, m_uHeight(0)
, m_isInitialize(false)
, m_ImpHalfWidth(0)
, m_ImpHalfHeight(0)
, m_RecycleSize( RecycleSize )
, m_pClientCoder(NULL)
{
}

CSceneManager::~CSceneManager()
{
	RegionPool::iterator it,eit = m_RegionMgr.end();
	for( it = m_RegionMgr.begin(); it!=eit; ++ it )
		delete it->second;
	m_RegionMgr.clear();
	ClearRecycle();

	for( size_t i = 0; i < m_vecEleMgrs.size(); ++i )
		m_vecEleMgrs[i]->Destroy();

	//CSyncSystem::Clear();
}

bool CSceneManager::SetActivePos(uint32 X, uint32 Y)
{
	uint32 Width = m_ImpHalfWidth + 1;
	uint32 Height = m_ImpHalfHeight + 1;
	if( m_uWidth>m_ImpHalfWidth+1 )
	{
		X = limit2(X, m_ImpHalfWidth , m_uWidth  - 1 - m_ImpHalfWidth);
		Width += m_ImpHalfWidth;
	}
	else
		X = m_ImpHalfWidth;
	

	if(m_uHeight>m_ImpHalfHeight+1)
	{
		Y = limit2(Y, m_ImpHalfHeight	, m_uHeight - 1 - m_ImpHalfHeight);
		Height += m_ImpHalfHeight;
	}
	else
		Y = m_ImpHalfHeight;

	if( X == m_XPos && Y == m_YPos )
		return true;

	uint32 orgID = X - m_ImpHalfWidth + (Y - m_ImpHalfHeight)* m_uWidth;

	//以下程序先这样写 高效率的正在酝酿中
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
				pRegion = new CRegion(this,orgID);
				m_RegionMgr.insert(RegionPool::value_type(orgID,pRegion));
				for( size_t i=0; i< m_vecEleMgrs.size(); ++i)
					pRegion->AddCtrlNode( m_vecEleMgrs[i]->CreateNode(orgID) );
			}
			++orgID;
		}
		orgID += m_uWidth - Width;
	}

	m_XPos = X;
	m_YPos = Y;

	m_RegionRecycle.OptimizeRecycle(m_RecycleSize);
	return true;
}

void CSceneManager::ClearRecycle(void)
{
	CRegion* pRegion = NULL;
	while( pRegion = m_RegionRecycle.PopNode() )
		SafeDelete(pRegion);
}

void CSceneManager::OptimizeRecycle(void)
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

bool CSceneManager::AddElementManager( CElementManager* pEleMgr )
{
	for( size_t i = 0; i < m_vecEleMgrs.size(); ++i )
	{
		if( m_vecEleMgrs[i] == pEleMgr )
			return true;
	}
	
	m_vecEleMgrs.push_back( pEleMgr );

	RegionPool::iterator it,eit = m_RegionMgr.end();
	for( it = m_RegionMgr.begin(); it!=eit; ++ it )
		it->second->AddCtrlNode(pEleMgr->CreateNode(it->second->GetID()));

	ClearRecycle();
	return true;
}

bool CSceneManager::DelElementManager( CElementManager* pEleMgr )
{
	for( size_t i = 0; i < m_vecEleMgrs.size(); ++i )
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

void CSceneManager::Create( void )
{
	Ast(m_pClientCoder && m_FileChunk);
	OnPreLoad();
	m_isInitialize = m_pClientCoder->CSceneManagerDeCode( this, m_FileChunk );
	if(m_isInitialize)
	{
		m_ImpHalfWidth	= (m_ImpWidth>m_uWidth	||m_ImpWidth<2	) ?  m_uWidth / 2 : m_ImpWidth / 2;
		m_ImpHalfHeight = (m_ImpHeight>m_uHeight||m_ImpHeight<2	) ?  m_uHeight / 2 : m_ImpHeight / 2;
		OnLoaded();
	}
}

void CSceneManager::Destroy( void )
{
	m_isInitialize = false;
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

CRegion* CRegionRecycle::PopNode(uint32 ID)
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
	for( it = m_RecycleList.begin(); it!=eit; )
	{
		if(delNum>0)
		{
			delete (*it).pRegion;
			m_IdMap.erase((*it).IdIt);
			m_RecycleList.erase( it++ );
			--delNum;
			continue;
		}
			
		if( (tNow - (*it).time)>m_FreeTime )
		{
			delete (*it).pRegion;
			m_IdMap.erase((*it).IdIt);
			m_RecycleList.erase( it++ );
			continue;
		}
			
		break;
	}
}