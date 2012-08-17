#include "stdafx.h"
#include "CLapseDeleteQueue.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------
CLapseDeleteNode::CLapseDeleteNode(CRenderSpaceNode* pChild,uint32 LapseTime)
 : m_LapseTime( LapseTime )
 , m_LapseAlpha(255.0f)
 , m_pRenderChild(pChild)
{

}

CLapseDeleteNode::~CLapseDeleteNode()
{
	delete m_pRenderChild;
}

////是否半透删除的接口
//uint32	CLapseDeleteNode::GetLapseTime(void)
//{
//	return m_LapseTime; 
//}
//
//void	CLapseDeleteNode::SetLapseTime( uint32 tLapse )
//{
//	m_LapseTime = tLapse;
//}

CLapseDeleteQueue::CLapseDeleteQueue()
{

}

CLapseDeleteQueue::~CLapseDeleteQueue()
{
	Clear();
}

void CLapseDeleteQueue::InsertNode( CLapseDeleteNode* pNode )
{
	if(pNode == NULL)
		return;

	if(pNode->m_LapseTime >0)
	{
		pNode->m_LapseAlpha = float(pNode->m_pRenderChild->GetAlphaValue()) / float( pNode->m_LapseTime - 500 );
		m_lDeleteQueue.push_back(pNode);
	}
	else
		delete pNode;
}

void CLapseDeleteQueue::Update( uint32 uCurTime )
{
	uint32	dTime = uCurTime - m_CurTime;
	m_CurTime = uCurTime;
	DeleteQueue::iterator it,eit = m_lDeleteQueue.end();
	for( it = m_lDeleteQueue.begin(); it != eit; )
	{
		uint8 Alpha = uint8(dTime * (*it)->m_LapseAlpha);
		uint8 CurAlpha = (*it)->m_pRenderChild->GetAlphaValue();
		if( CurAlpha > 0 )
		{
			(*it)->m_pRenderChild->SetAlphaValue( CurAlpha > Alpha?CurAlpha - Alpha : 0 );
			++it;
		}
		else
		{
			CLapseDeleteNode *pTempLapseDeleteNode = *it;
			
			it = m_lDeleteQueue.erase(it);
			delete pTempLapseDeleteNode;
		}
	}
}

void CLapseDeleteQueue::Clear(void)
{
	DeleteQueue::iterator it,eit = m_lDeleteQueue.end();
	for( it = m_lDeleteQueue.begin(); it != eit; ++it )
		delete (*it);
	m_lDeleteQueue.clear();
}