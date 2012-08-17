#include "stdafx.h"
#include "CModelInst.h"
#include "CGraphic.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------

inline BATCH_HANDLE GET_BATCH_HANDLE(void* pHandle) 
{
	return BATCH_HANDLE(pHandle);
}


CModelInst::CModelInst(ILogicHandler* pHandler)
: CBaseModel(pHandler)
, m_bNeedUpdate(true)
{

}

CModelInst::~CModelInst()
{
	ClearModelRender();
}

void CModelInst::_UpdateSkeFrm(void)
{
	m_InstAniParams.SKE_HANDLE	= GET_BATCH_HANDLE(m_pSkeFrame);
	m_InstAniParams.ANI_HANDLE	= 0;
	m_InstAniParams.ANI_INDEX	= 0xffffffff;
}

void CModelInst::_UpdateAniGrp(void)
{
	m_InstAniParams.ANI_HANDLE	= 0;
	m_InstAniParams.ANI_INDEX	= 0xffffffff;
}

void CModelInst::_UpdateMesh(SKELETAL_MASK& SkeletalMask)
{
	CBaseModel::_UpdateMesh(SkeletalMask);
	m_InstAniParams.SKE_MASK  = SkeletalMask;
}

void CModelInst::ClearModelRender(void)
{
	m_mapInst.clear();
	m_bNeedUpdate  = true;
}

bool CModelInst::SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop, int32 DelayTime , float AniSpeed )
{
	uint32 AniNo = m_pAnimationGroup->GetAniIDbyName( AniName );

	if ( FAILED( AniNo ) )
		return false;

	if( AniNo == m_InstAniParams.ANI_INDEX ) 
		return true;

	m_InstAniParams.ANI_INDEX  = AniNo;
	m_InstAniParams.ANI_HANDLE = GET_BATCH_HANDLE(m_pAnimationGroup->GetAniById(AniNo));
	ClearModelRender();
	return true;
}

void CModelInst::SetFixMatrix(const CMatrix& parent, const CMatrix& fix)
{
	m_RenderMat = fix * GetWorldMatrix() * parent;
}

void CModelInst::UpdateBox(void)
{
	m_CenterMatrix = m_RenderMat;
	//m_CurAABB = m_OrgAABB;
	//m_CurAABB.transformBox(m_RenderMat);
}

const AniResult CModelInst::Render( uint32 uCurTime, RenderObjStyle* pRORS )
{
	if(pRORS == NULL )
		return  AniResult();

	if(m_bNeedUpdate)
	{
		CGeometryInst tInst;
		tInst.ANI_PARAMS	= &m_InstAniParams;
		tInst.m_pMatrix = &m_RenderMat;
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
		{
			CRenderPiece& rPiece = (*it).second;
			if( rPiece.m_bShow )
			{
				for ( int iPieces = 0; iPieces < (int)rPiece.m_Pieces.size(); iPieces++ )
				{
					if (rPiece.m_Pieces[iPieces].m_bShow)
					{
						tInst.MESH_HANDLE = GET_BATCH_HANDLE(rPiece.m_Pieces[iPieces].m_pPiece->GetMesh());
						tInst.RS_HANDLE	  = GET_BATCH_HANDLE(rPiece.m_Pieces[iPieces].m_pPiece->GetRS(rPiece.m_RenderStyle));
						tInst.m_pPieceModify = &rPiece.m_Pieces[iPieces].m_Modify;

						if( m_InstAniParams.ANI_HANDLE == NULL ||
							rPiece.m_Pieces[iPieces].m_pPiece->GetMesh()->GetType() == VT_SKELETALANI )
						{
							m_mapInst.push_front(tInst);
							CGraphic::GetInst()->InsertGeometry(&(*m_mapInst.begin()),pRORS->m_eType);
						}
						else
						{	
							BATCH_HANDLE TempHandle = m_InstAniParams.ANI_HANDLE;
							m_InstAniParams.ANI_HANDLE = NULL;
							m_mapInst.push_front(tInst);
							CGraphic::GetInst()->InsertGeometry(&(*m_mapInst.begin()),pRORS->m_eType);
							m_InstAniParams.ANI_HANDLE = TempHandle;
						}
					};
				}
			}
		}
		m_pRenderAniInst = m_InstAniParams.pAniInst;
		m_bNeedUpdate = false;
	}

	/*if ( !pRORS || pRORS->m_bRenderLink )
		RenderLink( uCurTime, (pRORS && pRORS->m_eType == ROS_UNKNOWN) ? NULL : pRORS );*/
	return m_pRenderAniInst?m_pRenderAniInst->m_Result:AniResult();
}
