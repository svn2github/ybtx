#include "stdafx.h"
#include "CSceneNodeCameraBase.h"


namespace sqr
{
	CCameraSceneNode::CCameraSceneNode( ISceneNode *pParent, CSceneNodeCameraBase *pCamBase )
		: ISceneNode(pParent)
		, m_pCameraBase(pCamBase)
	{
	}

	void CCameraSceneNode::OnEvent( InputEvent &event )
	{
		ASSERT(m_pCameraBase != NULL);
		m_pCameraBase->OnEvent(event);
	}

	void CCameraSceneNode::Render()
	{
		ASSERT(m_pCameraBase != NULL);
		m_pCameraBase->Render();
	}


	CSceneNodeCameraBase::CSceneNodeCameraBase()
		: m_pSceneNode(NULL)
	{
	}

	void CSceneNodeCameraBase::AddToScene( ISceneNode *pParent )
	{
		ASSERT(pParent);
		m_pSceneNode = new CCameraSceneNode(pParent, this);
	}

	void CSceneNodeCameraBase::RemoveFromScene()
	{
		ASSERT(m_pSceneNode);
		m_pSceneNode->Remove();
	}

}