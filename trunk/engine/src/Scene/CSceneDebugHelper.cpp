#include "stdafx.h"
#include "CSceneDebugHelper.h"

//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

CSceneDebugMgr::CSceneDebugMgr()
{
}

CSceneDebugMgr::~CSceneDebugMgr()
{
	SceneDebugClear();
}

void CSceneDebugMgr::RegistDebugNode(CSceneDebugNode* pDebugNode)
{
	pDebugNode->AddRef();
	m_SceneDebugList.push_back(pDebugNode);
}

void CSceneDebugMgr::SceneDebugClear()
{
	for(size_t i = 0 ;i<m_SceneDebugList.size();i++)
		m_SceneDebugList[i]->Release();
	m_SceneDebugList.clear();
}

void CSceneDebugMgr::SceneDebugRun()
{
	SceneDebugPool::iterator iter = m_SceneDebugList.begin();
	while(iter != m_SceneDebugList.end())
	{
		(*iter)->RenderDebug();
		iter++;
	}
}

void CSceneDebugMgr::DelelteDebugNode(CSceneDebugNode* pDebugNode)
{
	SceneDebugPool::iterator iter = m_SceneDebugList.begin();
	for(;iter!=m_SceneDebugList.end();++iter)
	{
		if(*iter==pDebugNode)
		{
			pDebugNode->Release();
			m_SceneDebugList.erase(iter);
			break;
		}
	}
}

