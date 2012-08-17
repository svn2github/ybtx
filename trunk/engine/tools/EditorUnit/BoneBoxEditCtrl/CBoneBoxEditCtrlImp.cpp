#include "stdafx.h"
#include "CCtrlBase.h"
#include "CEditObserver.h"
#include "CEditContext.h"
#include "BoneBoxEditCtrl/CBoneBoxEditCtrlImp.h"
#include "BoneBoxEditCtrl/CBoneBoxEditDelegate.h"
#include "ContextMsg.h"
#include "CRenderDebugFun.h"

DEF_UI_IMP(CBoneBoxEditCtrlImp);

#define MODEL ((CEditModel*)m_pOwnContext->GetActiveObj()) 

CBoneBoxEditCtrlImp::CBoneBoxEditCtrlImp( void* param )
:m_pDelegate(NULL)
,m_pAABB(NULL)
,m_bUpdateList(true)
,m_bShowBox(false)
{

}

CBoneBoxEditCtrlImp::~CBoneBoxEditCtrlImp( void )
{
	if (m_pDelegate)
	{
		delete m_pDelegate;
		m_pDelegate = NULL;
	}
}

void sqr_tools::CBoneBoxEditCtrlImp::SetContext( CEditContext* pContext )
{
	if( m_pOwnContext == pContext )
		return;
	m_pOwnContext = pContext;
	m_pDelegate = new CBoneBoxEditDelegate(this);
	m_pOwnContext->RegisterDelegate(m_pDelegate);
	m_pOwnContext->ActiveDelegate(m_pDelegate);
}

void sqr_tools::CBoneBoxEditCtrlImp::UpdateData( std::string str )
{
	if (MODEL)
	{
		SetUpdateList(true);
		m_strSelBoneName = str;
		m_vtNames.clear();

		if (!str.empty())
		{
			int ID = MODEL->GetSkeletalIdByName(str.c_str());
			for( CModel::RenderPieceMap::iterator rit = MODEL->GetRenderPieces().begin(); rit != MODEL->GetRenderPieces().end(); rit++ )			
			{
				GVector<CRenderPiece::PieceData>::iterator it = rit->second.m_Pieces.begin();
				for( ; it!=rit->second.m_Pieces.end(); ++it )
				{
					if( (*it).m_pPiece && (*it).m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI )
					{
						CMesh* pMesh = (*it).m_pPiece->GetMesh();
						if(pMesh->m_BoxMap.end()!=pMesh->m_BoxMap.find(ID))
							m_vtNames.push_back((*it).m_pPiece->GetPieceName());
					}
				}
			}
		}	

		Update();
	}
}

int sqr_tools::CBoneBoxEditCtrlImp::GetBoxNum()
{
	return m_vtNames.size();
}

std::string sqr_tools::CBoneBoxEditCtrlImp::GetBoxNameByIndex( int index )
{
	return m_vtNames[index];
}

void sqr_tools::CBoneBoxEditCtrlImp::GetBoxInfo( float& max_x, float& max_y, float& max_z, float& min_x, float& min_y, float& min_z )
{
	if (m_pAABB)
	{
		max_x = m_pAABB->m_vMaximum.x;
		max_y = m_pAABB->m_vMaximum.y;
		max_z = m_pAABB->m_vMaximum.z;
		min_x = m_pAABB->m_vMinimum.x;
		min_y = m_pAABB->m_vMinimum.y;
		min_z = m_pAABB->m_vMinimum.z;
	}
}

void sqr_tools::CBoneBoxEditCtrlImp::SetBoxInfo( const float max_x, const float max_y, const float max_z, const float min_x, const float min_y, const float min_z )
{
	if (m_pAABB)
	{
		m_pAABB->m_vMaximum.x = max_x;
		m_pAABB->m_vMaximum.y = max_y;
		m_pAABB->m_vMaximum.z = max_z;
		m_pAABB->m_vMinimum.x = min_x;
		m_pAABB->m_vMinimum.y = min_y;
		m_pAABB->m_vMinimum.z = min_z;
	}
}

void sqr_tools::CBoneBoxEditCtrlImp::DeleteBox( std::string name )
{
	if (MODEL)
	{
		SetUpdateList(false);

		std::vector<std::string>::iterator itr = m_vtNames.begin();
		while (itr != m_vtNames.end())
		{
			if (name.compare(*itr) == 0)
			{
				m_vtNames.erase(itr);
				break;
			}
			itr++;
		}

		int ID = MODEL->GetSkeletalIdByName(m_strSelBoneName.c_str());
		CModel::RenderPieceMap::iterator it = MODEL->GetRenderPieces().find(name.c_str());
		if(it!=MODEL->GetRenderPieces().end())
		{
			for( int i = 0; i< (int)it->second.m_Pieces.size(); ++ i)
			{
				CMesh* pMesh = it->second.m_Pieces[i].m_pPiece->GetMesh();
				pMesh->m_BoxMap.erase(ID);
				MODEL->DelBox(m_pAABB);
				//g_Model.RefreshBox();
			}
			m_pAABB = NULL;
		}

		if(m_vtNames.size()==0)
		{
			MSG_SYS_SELCHANGE msg(NULL);
			m_pOwnContext->EventCollect(msg);		
		}
	}
}

void sqr_tools::CBoneBoxEditCtrlImp::NodeClick( std::string name )
{
	if (MODEL)
	{
		SetUpdateList(false);
		m_pAABB = NULL;
		int ID = MODEL->GetSkeletalIdByName(m_strSelBoneName.c_str());
		if(ID!=-1)
		{
			CModel::RenderPieceMap::iterator it = MODEL->GetRenderPieces().find(name.c_str());
			if(it!=MODEL->GetRenderPieces().end())
			{			
				CMesh* pMesh = it->second.m_Pieces[0].m_pPiece->GetMesh();
				CMesh::BoxMap::iterator itb = pMesh->m_BoxMap.find(ID);
				if(itb!=pMesh->m_BoxMap.end())
				{
					m_pAABB	 = &itb->second;
					Update();
				}
			}
		}
	}
}

void sqr_tools::CBoneBoxEditCtrlImp::SetUpdateList( bool value )
{
	m_bUpdateList = value;
}

bool sqr_tools::CBoneBoxEditCtrlImp::GetUpdateList( void )
{
	return m_bUpdateList;
}

void sqr_tools::CBoneBoxEditCtrlImp::Reset( void )
{
	m_pAABB = NULL;
}

void sqr_tools::CBoneBoxEditCtrlImp::SetShowBox( bool value )
{
	m_bShowBox = value;
}

void sqr_tools::CBoneBoxEditCtrlImp::Render( void )
{
	if (MODEL)
	{
		if (MODEL->GetSketetalFrame() && m_bShowBox)
		{
			RenderSmallBoxDebug(MODEL, m_pAABB?m_pAABB:NULL);
		}
	}
}