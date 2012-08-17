#include "stdafx.h"
#include "CElementManager.h"
#include "ExpHelper.h"
#include "BaseHelper.h"
#include "ErrLogHelper.h"
#include "CGraphicExp.h"
#include "TSqrAllocator.inl"

CElementNode::CElementNode(CElementManager* pParent , URegionID id)
: m_pParentManager(pParent)
, m_ID(id)
, m_isValid(false)
, m_pCoder(NULL)
{
	Ast(m_pParentManager);
}

CElementNode::~CElementNode()
{
}

void CElementNode::StopDeCode(void)
{
	if(m_pCoder)
	{
		m_pCoder->StopDeCode(this);
		m_pCoder=NULL;
	}
}

void CElementNode::Destroy(void)
{
	Ast(m_pParentManager);
	m_pParentManager->DestroyNode(m_ID);
}

bool CElementNode::DeCode(bool bSync)
{
	return m_pParentManager->GetCoder()?m_pParentManager->GetCoder()->CElementDeCode( this, bSync ) : true;
}

void  CElementNode::SetCoder(CElementCoder* pCoder)
{
	if(m_pCoder!=pCoder)
	{
		SafeRelease(m_pCoder);
		m_pCoder = pCoder;
		if(m_pCoder)
			m_pCoder->AddRef();
	}
}

CElementManager::CElementManager()
: m_pCoder(NULL)
{
	
}

CElementCoder* CElementManager::SetCoder( CElementCoder* pCoder )
{
	if( pCoder != m_pCoder )
	{
		SafeRelease(m_pCoder);
		m_pCoder = pCoder;
		m_pCoder->AddRef();
	}
	return m_pCoder;
}

CElementManager::~CElementManager()
{
	Destroy();
	SafeRelease(m_pCoder);
}

void CElementManager::Destroy(void)
{
	ElementPool::iterator it,eit = m_ElePool.end();
	for( it = m_ElePool.begin(); it!=eit; ++it )
	{
		it->second->StopDeCode();
		delete it->second;
	}
	m_ElePool.clear();
}

CElementNode*	CElementManager::GetNode(URegionID ID)
{
	ElementPool::iterator it = m_ElePool.find( ID );
	if(it!=m_ElePool.end())
		return it->second;
	return NULL;
}

CElementNode*	CElementManager::CreateNode( URegionID ID )
{
	CElementNode* pNode = GetNode(ID);
	
	if(pNode)
		return pNode;
	
	pNode = _CreateNode(ID);
	if(NULL!=pNode)
		m_ElePool[ID] = pNode;
	return pNode;
}

bool	CElementManager::DestroyNode( URegionID ID )
{
	ElementPool::iterator it = m_ElePool.find( ID );
	if(it!=m_ElePool.end())
	{
		it->second->StopDeCode();
		CElementNode* pCurNode = it->second;
		m_ElePool.erase(it);
		delete pCurNode;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
bool	CElementCoder::Lock( CElementNode* pNode )
{
	if(m_pLockNode==NULL)
	{
		m_pLockNode = pNode;
		pNode->SetCoder(this);
		return true;
	}
	return false;
}
//------------------------------------------------------------------------------

void	CElementCoder::UnLock( void )
{
	if(m_pLockNode)
	{
		CElementNode* pNode = m_pLockNode;
		m_pLockNode = NULL;
		if(pNode->GetCoder() == this)
			pNode->SetCoder(NULL);
	}
}

void CElementCoder::StopDeCode(CElementNode* pNode)
{
	if(pNode==m_pLockNode)	
	{
		CSyncSystem::SafeDelCoder(this);
		UnLock();
	}
	else
		pNode->SetCoder(NULL);
}

void CElementCoderEx::Clear(void)
{
	CElementNode* pNode = m_pLockNode;
	DeCodeList::iterator it,eit = m_listDeCodeNode.end();
	for( it = m_listDeCodeNode.begin(); it!=eit; ++it)
	{
		CSyncSystem::SafeDelCoder(this);	
	}
	m_listDeCodeNode.clear();

	if(pNode)
	{
		CSyncSystem::SafeDelCoder(this);
		UnLock();
	}
}

bool CElementCoderEx::CElementDeCode( CElementNode* pNode, bool bSync )
{
	pNode->SetCoder(this);
	m_listDeCodeNode.push_back(DeCodeParams(pNode));
	CSyncSystem::PushCoder(this);
	return true;
}

void CElementCoderEx::StopDeCode(CElementNode* pNode)
{	
	if(pNode == m_pLockNode)
	{
		CSyncSystem::SafeDelCoder(this);
		UnLock();
	}
	else
	{
		DeCodeList::iterator it,eit = m_listDeCodeNode.end();
		for( it = m_listDeCodeNode.begin(); it!=eit; ++it)
		{
			if( (*it).m_pNode == pNode )
			{		
				if(m_pLockNode)
				{
					SyncListIterator It;
					PopIterator(It);
					CSyncSystem::DelCoder(this);
					PushIterator(It);
				}
				else
					CSyncSystem::DelCoder(this);
				m_listDeCodeNode.erase(it);			
				break;
			}
		}	
		pNode->SetCoder(NULL);
	}
}

void  CElementCoderEx::SyncCode(CElementNode* pNode)
{
	SQR_TRY	
	{
		if(pNode == NULL)
		{
			while(m_pLockNode)
			{CSyncSystem::BeginSyncFun(1);}

			DeCodeList::iterator it,eit = m_listDeCodeNode.end();
			for( it = m_listDeCodeNode.begin(); it!=eit; ++it)
			{
				DeCodeParams Param = *it;
				m_listDeCodeNode.erase(it);

				while(!_CElementDeCode(Param));
				Param.m_pNode->SetCoder(NULL);
			}	
		}
		else
		{
			if(pNode == m_pLockNode)
			{
				while(m_pLockNode!=pNode)
				{CSyncSystem::BeginSyncFun(1);}
			}
			else
			{
				DeCodeList::iterator it,eit = m_listDeCodeNode.end();
				for( it = m_listDeCodeNode.begin(); it!=eit; ++it)
				{
					if( (*it).m_pNode == pNode )
					{		
						DeCodeParams Param = *it;
						m_listDeCodeNode.erase(it);
						while(!_CElementDeCode(Param));
						break;
					}
				}	
			}
		}
	}
	SQR_CATCH(exp)
	{
		exp.AppendMsg("CElementCoderEx::SyncCode");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	
}

bool CElementCoderEx::AutoDeCode(void)
{
	if(m_listDeCodeNode.empty())
		return true;

	DeCodeParams Param;
	DeCodeList::iterator it = m_listDeCodeNode.begin();
	Param = *it;
	if( !Lock(Param.m_pNode) )
		return false;

	SQR_TRY	
	{
		m_listDeCodeNode.pop_front();
		if( !_CElementDeCode(Param) )
		{
			UnLock();
			m_listDeCodeNode.push_back(Param);
			return false;
		}
		UnLock();
	}
	SQR_CATCH(exp)
	{
		UnLock();
		exp.AppendMsg("CElementCoderEx::资源文件加载失败");
		GfkLogExp(exp);
	}
	SQR_TRY_END;
	return true;
}
