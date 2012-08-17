#include "stdafx.h"
#include "CRenderSpace.h"
#include "CRenderSpaceNode.h"
#include "BaseHelper.h"

CRenderSpace::CRenderSpace()
{
}

CRenderSpace::~CRenderSpace()
{
	while( !m_listNode.empty() )
	{
		CRenderSpaceNode* pNode = *m_listNode.begin();
		DelChild( pNode );
		ISpaceNodeHandler* pHandler = pNode->m_pNodeHandler;

		SQR_TRY
		{
			delete pNode;
		}
		SQR_CATCH(exp)
		{
			GfkLogExp(exp);
		}
		SQR_TRY_END;	

		if(pHandler)
		{
			SQR_TRY
			{
				pHandler->OnDestroyed();
			}
			SQR_CATCH(exp)
			{
				exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err));
				LogExp(exp);
			}
			SQR_TRY_END;			
		}
	}
}

void CRenderSpace::AddChild( CRenderSpaceNode* pChild, eLinkType linkType, const char* szAttachName )
{
	if( pChild->m_pParentSpace == this )
		return;

	if( pChild->m_pParentSpace )
		pChild->m_pParentSpace->DelChild( pChild );

	AdvAddRef(pChild);
	m_listNode.insert( pChild );
	pChild->m_pParentSpace = this;
}

void CRenderSpace::DelChild( CRenderSpaceNode* pChild )
{
	if( pChild->m_pParentSpace != this )
	{
		if( pChild->m_pParentSpace )
			pChild->m_pParentSpace->DelChild( pChild );

		LogErr( "删除的不是自己的子节点!" );
		return;
	}
	
	pChild->m_pParentSpace = NULL;

	AdvDelRef(pChild);
	m_listNode.erase( pChild );
}

void CRenderSpace::GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID )
{
	matLink.Identity();
}

void CRenderSpace::OnPreRender( uint32 uCurTime,CCamera* pCamera )
{
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->OnPreRender( uCurTime,pCamera );
}

void CRenderSpace::Render(void)
{
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->Render();
}

void CRenderSpace::RenderEx( CCamera* pCamera, uint32 uCurTime )
{
	SpaceNodeList::iterator it = m_listNode.begin();
	SpaceNodeList::iterator pNode;
	while(true)
	{
		SQR_TRY	
		{
			while(it != m_listNode.end())
			{
				(*it)->OnPreRender( uCurTime,pCamera );
				(*it)->Render();
				++it;
			}
			break;
		}
		SQR_CATCH(exp)
		{
			it = m_listNode.erase(it);
			exp.AppendMsg("有不可被渲染的CRenderSpaceNode出现");
			GfkLogExp(exp);
		}
		SQR_TRY_END;
	}	
}

bool CRenderSpace::GetWillRender()
{
	return true;
}

void CRenderSpace::DestroyRenderChild(CRenderSpaceNode* pObj)
{
	delete pObj;
}


//--------------------------------------------------------------