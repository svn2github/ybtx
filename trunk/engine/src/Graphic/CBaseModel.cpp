#include "stdafx.h"
#include "CBaseModel.h"
#include "CGraphic.h"
#include "CDataSources.h"
#include "BaseHelper.h"
#include "CGraphicExp.h"
#include "CVertexDeclaration.h"
#include "CodeCvs.h"
#include "TSqrAllocator.inl"
#include "IModelHandler.h"
const DWORD CBaseModel::st_ModelType = 'MODL';

CBaseModel::CBaseModel(ILogicHandler* pHandler)
: CRenderNode(pHandler)
, m_pSkeFrame(NULL)
{
	m_Velocity				= 1.0f;
	//m_bFrameSynch			= FALSE;
	m_ParentFrameRatio      = 33.33f;
	m_Amalgamation			= false;
	//m_isVisible				= false;
	//m_OrgAABB.setMinimum( -8.0f,-8.0f,-8.0f);
	//m_OrgAABB.setMaximum(  8.0f, 8.0f, 8.0f);
	m_pRenderAniInst		= NULL; 
	//m_CurAABB				= m_OrgAABB;
	m_pAnimationGroup   = new CAnimationGroup();
}

CBaseModel::~CBaseModel()
{
	//SafeRelease(m_pSkeFrame);
	SafeRelease(m_pAnimationGroup);
	//ClearLink();
	ClearMeshPiece();
}

void	CBaseModel::SetSkeletalFrame(CSkeletalFrame* SkeletalFrame)
{
	//SafeRelease(m_pSkeFrame);
	if(NULL != m_pSkeFrame)
		return;

	m_pSkeFrame = SkeletalFrame;
	ClearModelRender();

	m_CenterLinkId = GetSkeletalIdByName("Bip SpinePro");
	_UpdateSkeFrm();
	/*if(-1==m_CenterLinkId)
		m_CenterLinkId = GetSkeletalIdByName("Bip01 Spine1");
	else
		m_CenterLinkId = GetSkeletalIdByName("Bip01 Spine");*/
	SKELETAL_MASK SkeletalMask;
	_UpdateMesh(SkeletalMask);
}

void CBaseModel::SetAnimateGroup( CAnimationGroup* AnimateGroup )
{
	if(AnimateGroup==NULL || AnimateGroup->GetSkeletalFrame() == NULL)
		return;

	if(m_pSkeFrame == NULL)
	{
		m_pAnimationGroup->Merge( AnimateGroup );
		//SetSkeletalFrame(AnimateGroup);
	}
	else
	{
		if( AnimateGroup->GetSkeletalFrame()->GetSkeletalNum()==m_pSkeFrame->GetSkeletalNum() )
			m_pAnimationGroup->Merge(AnimateGroup);
		else
			GenErr(GraphicErr::GetErrTypeStr(OutUse_Err)+"两个动作包骨骼数量不匹配，不能指定给同一个模型，融合失败,请查创建角色的时序问题", string(":")+m_pSkeFrame->GetName() + ":" + AnimateGroup->GetName());		
		_UpdateAniGrp();
	}
}

//在渲染前得到父连接物信息
//void CBaseModel::OnPreRender( const AniResult& ParentAniRes, float FrameRatio )
//{
//	m_IsParentAniBlend  = ParentAniRes.m_InAniBlend;
//	m_ParentFrame		= (uint16)ParentAniRes.m_CurFrame;
//	m_ParentFrameRatio	= FrameRatio;
//}

bool CBaseModel::UpdateTime(uint32 uCurTime)
{
	//EndDuke fix
	//AniResult Result;
	//bool hasEnd = false;
	//for ( GMap< GString, CRenderPair >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); )
	//{
	//	bool isAniEnd  = true;
	//	SQR_TRY
	//	{
	//		isAniEnd = (*it).second.m_pRenderObj->UpdateTime( uCurTime );
	//		++it;
	//	}
	//	SQR_CATCH(exp)
	//	{
	//		exp.AppendType(GraphicErr::GetErrTypeStr(RenderRuntime_Err));
	//		LogExp(exp);

	//		CRenderable* pRender = NULL;
	//		pRender = it->second.m_pRenderObj;
	//		pRender->OnUnlinked( this,  it->first.c_str() );
	//		it = m_CenterLink.erase( it );
	//		SafeRelease( pRender );
	//	}
	//	SQR_TRY_END;

	//	hasEnd |= isAniEnd;
	//	if(m_pRenderAniInst->m_pHandler && isAniEnd)
	//	{
	//		SQR_TRY
	//		{
	//			m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd("");
	//		}
	//		SQR_CATCH(exp)
	//		{
	//			exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err));
	//			LogExp(exp);
	//		}
	//		SQR_TRY_END;
	//	}
	//}
	return false;//hasEnd;
}

//增加模型Piece
void CBaseModel::AddMeshPiece( CPieceGroup* pPieceGroup, const TCHAR* szClassName, uint32 RSNum )
{
	if ( !pPieceGroup )
		return;
	CPieceGroup* pCPiecegroup = ( CPieceGroup* )pPieceGroup;
	GVector<CPiece*> Pieces = pCPiecegroup->GetPieceClass( szClassName );
	if ( Pieces.size() == 0 )
		return;
	ClearModelRender();
	m_Amalgamation = false;
	GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.find( szClassName );
	if ( it != m_MeshsPiece.end() )
	{
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
			SafeRelease( it->second.m_Pieces[i].m_pPiece );
	}
	m_MeshsPiece[ szClassName ]  = CRenderPiece( Pieces, (uint16)RSNum );
	SKELETAL_MASK SkeletalMask;
	_UpdateMesh(SkeletalMask);
}


uint32 CBaseModel::GetID(CPieceGroup* pPieceGroup, const TCHAR* szClassName,const TCHAR* szRsIndx)
{
	if ( !pPieceGroup )
		return 0;
	CPieceGroup* pCPiecegroup = ( CPieceGroup* )pPieceGroup;
	GVector<CPiece*> Pieces = pCPiecegroup->GetPieceClass( szClassName );
	if ( Pieces.size() == 0 )
		return 0;

	// rs用中文存的...PropRes_Common.lua配置表转成utf8后szRsIndex得到的是utf8编码字符串，
	// 而Pieces[0]->m_RSIndxToID里是mbcs字符串，此处特殊处理下
	GString str = szRsIndx;
	uint32 rs = 0;

	if (str=="")
	{
		rs = (uint32)atoi( str.c_str());
	}
	else if (Pieces[0]->m_RSIndxToID.find(str)==Pieces[0]->m_RSIndxToID.end())
	{
		GString modelname = pCPiecegroup->GetName();
		GString piecename = szClassName;
		GString message = "的渲染类型 "+ str + "没找到!!\n";

		ostringstream strm;
		strm << modelname << " 中的piece：" << piecename << " " << message;
		ArtErr(strm.str());
	}
	else
		rs = Pieces[0]->m_RSIndxToID[str];

	GVector<CPiece*>::iterator it,eit = Pieces.end();
	for ( it = Pieces.begin(); it !=eit; ++it )
		SafeRelease(*it);
	return rs;
}

//删除模型Piece
void CBaseModel::DeleteMeshPiece( const TCHAR* szClassName )
{
	ClearModelRender();
	if ( !szClassName )
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); )
		{
			for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
				SafeRelease( it->second.m_Pieces[i].m_pPiece );
			m_MeshsPiece.erase(  it++ );
		}
	}
	else
	{
		GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.find( szClassName );
		if ( it != m_MeshsPiece.end() )
		{
			for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
				SafeRelease( it->second.m_Pieces[i].m_pPiece );
			m_MeshsPiece.erase( it );
		}
	}
}

//删除模型Piece
void CBaseModel::ClearMeshPiece()
{
	ClearModelRender();
	for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
	{
		for ( int i = 0; i < (int)it->second.m_Pieces.size(); i++ )
			SafeRelease( it->second.m_Pieces[i].m_pPiece );
	}
	m_OrgAABB.setNull();
	//m_CurAABB.setNull();
	m_MeshsPiece.clear();
}

//void CBaseModel::ShowLinkObj( const TCHAR* szAttachName, bool bShow )
//{
//	if ( szAttachName == NULL )
//	{
//		for ( GMap< GString, CRenderPair >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); ++it )
//			(*it).second.m_bShow = bShow;
//
//		return;
//	}
//
//	GMap< GString, CRenderPair >::iterator it = m_CenterLink.find( szAttachName );
//	if ( it != m_CenterLink.end() )
//		it->second.m_bShow = bShow;
//
//}

////清除SOCKET连接
//void CBaseModel::ClearLink( )
//{
//	while(m_CenterLink.begin()!=m_CenterLink.end())
//	{
//		CRenderable* pRender = m_CenterLink.begin()->second.m_pRenderObj;
//		pRender->OnUnlinked( this, m_CenterLink.begin()->first.c_str() );
//		m_CenterLink.erase(m_CenterLink.begin());
//		SafeRelease( pRender );
//	}
//
//	/*for ( GMap< GString, CRenderPair >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); ++it )
//	{
//		it->second.m_pRenderObj->OnUnlinked( this, it->first.c_str() );
//		SafeRelease( it->second.m_pRenderObj );
//	}
//	m_CenterLink.clear();*/
//}

int CBaseModel::GetSkeletalIdByName( const char * szName )
{
	if (!m_pSkeFrame)
		return -1;

	return m_pSkeFrame->GetRootSkeletal().GetSkeletalIDByName( szName );
}

void CBaseModel::GetSkeletaChildNum( BYTE SkeletalID, int32& Num )
{
	if ( m_pSkeFrame )
	{
		for ( int i=0; i<(int)m_pSkeFrame->GetSkeletalChildNum(SkeletalID); ++i )
		{
			++Num;
			GetSkeletaChildNum( m_pSkeFrame->GetSkeletalChildID( SkeletalID, i ), Num );
		}
	}
}


////增加SOCKE连接物体
//int CBaseModel::_AddLinkObj( CRenderable* MObj,  const TCHAR* szAttachName, bool bFrameSynch )
//{
//	if ( !MObj )
//		return -1;
//
//	GMap< GString, CRenderPair >::iterator it = m_CenterLink.find( szAttachName );
//	if ( it != m_CenterLink.end() )
//	{
//		it->second.m_pRenderObj->OnUnlinked( this, szAttachName );
//		SafeRelease( it->second.m_pRenderObj );
//		it->second.m_pRenderObj = MObj;
//	}
//	else
//		m_CenterLink.insert( pair< GString, CRenderPair >( szAttachName, CRenderPair(MObj, true) ) );
//	MObj->SetRenderVelocity(m_Velocity);
//	MObj->AddRef();
//	MObj->OnLinked( this, szAttachName, bFrameSynch );
//	return 0;
//}
//
//
//int CBaseModel::AddLinkObj( CRenderable* MObj,  const TCHAR* szAttachName, bool bFrameSynch )
//{
//	return _AddLinkObj( MObj, szAttachName, bFrameSynch );
//}
//
////删除SOCKET连接
//void CBaseModel::DeleteLinkObj( const TCHAR* szAttachName )
//{
//	GMap< GString, CRenderPair >::iterator it = m_CenterLink.find( szAttachName );
//	
//	if ( it != m_CenterLink.end() )
//	{
//		CRenderable* pRender = NULL;
//		pRender = it->second.m_pRenderObj;
//		pRender->OnUnlinked( this, szAttachName );
//		m_CenterLink.erase( it );
//		SafeRelease( pRender );
//	}
//}
//
//int32 CBaseModel::RefLinkObj( const TCHAR* szAttachName,  CRenderable** ppRenderable )
//{
//	*ppRenderable = NULL;
//
//	GMap< GString, CRenderPair >::iterator it = m_CenterLink.find( szAttachName );
//	if ( it != m_CenterLink.end() )
//	{
//		*ppRenderable = it->second.m_pRenderObj;
//	}
//
//	if ( *ppRenderable )
//		(*ppRenderable)->AddRef();
//
//	return (*ppRenderable) ? 0 : -1;
//}
//
//void CBaseModel::OnLinked( CRenderable* /*pParentObj*/, const TCHAR* /*AttachName*/, bool bFrameSynch )
//{
//	m_bFrameSynch = bFrameSynch == TRUE;
//}
//
//void CBaseModel::OnUnlinked( CRenderable* /*pParentObj*/, const TCHAR* /*AttachName*/ )
//{
//	m_bFrameSynch = FALSE;
//}

void CBaseModel::SetPieceRSIndex( const TCHAR* szClassName, uint32 nRSIndex )
{
	ClearModelRender();
	GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.find( szClassName ? szClassName : "" );
	if ( it != m_MeshsPiece.end() )
		it->second.SetPieceClassRS( nRSIndex );
	else
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
			it->second.SetPieceClassRS( nRSIndex );
	}
}

void   CBaseModel::SetVisible( bool isVisible )
{
	if ( m_isVisible!= isVisible )
	{
		m_isVisible = isVisible;
		ClearModelRender();
		
		if( m_pRenderAniInst && m_pRenderAniInst->m_pHandler )
		{
			SQR_TRY
			{
				m_pRenderAniInst->m_pHandler->OnSetVisable(m_isVisible);
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

//------------------------------------------------------------------------------
void CBaseModel::SetRenderVelocity( float Velocity , int32 id )
{
	if( id == -1 )
		m_Velocity = Velocity;//limit2(Velocity,0.5f,2.0f);
}

//#ifdef _DEBUG



void CBaseModel::SetModelHandler( IModelHandler * pHandler )
{
	if(m_pRenderAniInst==NULL||m_pRenderAniInst->m_pHandler == pHandler)
		return; 

	if(m_pRenderAniInst->m_pHandler)AdvDelRef(m_pRenderAniInst->m_pHandler);
	m_pRenderAniInst->m_pHandler = pHandler;
	if(m_pRenderAniInst->m_pHandler)AdvAddRef(m_pRenderAniInst->m_pHandler);
}

//int CBaseModel::RenderLink( uint32 uCurTime, RenderObjStyle* pRORS )
//{
//	if(m_pRenderAniInst == NULL)
//		return -1;
//	AniResult Result;
//	for ( GMap< GString, CRenderPair >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); )
//	{
//		SQR_TRY
//		{
//			if ( !(*it).second.m_bShow )
//				continue;
//			//这里考虑到的动作分离时候的情况。分离的时候动作帧不在是下半身的，而应该是上半身的
//			if (m_pSkeFrame)
//			{			
//				CAnimateSeparation* pAniSep = NULL;
//				if( m_CenterLinkId >= 0 && m_CenterLinkId < (int)m_pRenderAniInst->m_pSkeletalSepList.size() 
//					&& NULL != (pAniSep = m_pRenderAniInst->m_pSkeletalSepList[m_CenterLinkId]))
//				{
//					if( NULL!=pAniSep && APS_NORMAL  == pAniSep->m_State )
//					{	
//						AniResult res;
//						res.m_CurFrame = pAniSep->m_CurFrame;
//						res.m_IsAniEnd = false; 
//						res.m_IsReset  = false;
//
//						if( APS_FADEOUT == pAniSep->m_State )
//							pAniSep->m_State = APS_FADEIN;
//
//						(*it).second.m_pRenderObj->OnPreRender( res, m_ParentFrameRatio );
//					}
//				}
//				else
//					(*it).second.m_pRenderObj->OnPreRender( m_pRenderAniInst->m_Result, m_ParentFrameRatio );
//				Result = (*it).second.m_pRenderObj->Render(  m_pRenderAniInst->m_MatWorld,uCurTime, pRORS );
//			}		
//			else
//			{
//				(*it).second.m_pRenderObj->OnPreRender( m_pRenderAniInst->m_Result, m_ParentFrameRatio );
//				Result = (*it).second.m_pRenderObj->Render( m_pRenderAniInst->m_MatWorld,uCurTime, pRORS );
//			}
//
//			++it;
//		}
//		SQR_CATCH(exp)
//		{
//			exp.AppendType(GraphicErr::GetErrTypeStr(RenderRuntime_Err));
//			LogExp(exp);
//
//			CRenderable* pRender = NULL;
//			pRender = it->second.m_pRenderObj;
//			pRender->OnUnlinked( this,  it->first.c_str() );
//			it = m_CenterLink.erase( it );
//			SafeRelease( pRender );
//		}
//		SQR_TRY_END;
//
//
//		if(m_pRenderAniInst->m_pHandler && Result.m_IsAniEnd)
//		{
//			SQR_TRY
//			{
//				m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd((const char*)Result.m_PreAnimationName);
//			}
//			SQR_CATCH(exp)
//			{
//				exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err));
//				LogExp(exp);
//			}
//			SQR_TRY_END;
//		}
//	}
//	return 0;
//}

void CBaseModel::_UpdateMesh(SKELETAL_MASK& SkeletalMask)
{
	ClearModelRender();
	SkeletalMask.Empty();
	SkeletalMask.AddSkeletal(0);
	
	m_OrgAABB.setNull();
	for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
	{
		for ( size_t np = 0; np < it->second.m_Pieces.size(); ++np )
		{
			SkeletalMask += it->second.m_Pieces[np].m_pPiece->GetSkeletalMask();

			if ( it->second.m_Pieces[np].m_pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI 
				&& string("weapon") != it->second.m_Pieces[np].m_pPiece->GetPieceClassName() )
			{
				Ast(!it->second.m_Pieces[np].m_pPiece->GetAABox().isNull());
				m_OrgAABB.mergeBox(it->second.m_Pieces[np].m_pPiece->GetAABox());
				_AddAABB(it->second.m_Pieces[np].m_pPiece);
			}
		}
	}

	m_Scale = m_OrgAABB.getSize();
	m_Off = m_OrgAABB.getCenter();
	//float half = (m_Scale.x + m_Scale.z)*0.5f;

	//m_Scale.x = m_Scale.x * 0.5f + half * 0.5f;
	//m_Scale.z = m_Scale.z * 0.5f + half * 0.5f;

	m_OrgAABB.setExtents( m_Off - m_Scale * 0.5f, m_Off + m_Scale *0.5f );

	if ( m_pSkeFrame )
	{
		if ( m_pSkeFrame->GetSkeletalNum() > 1 )
			SkeletalMask.AddSkeletal( 1 );
		if( !m_pSkeFrame->SetSkeletalMask( SkeletalMask ) )
		{
			string ErrStr = "CModel::AddMeshPiece加入的或现存模型中:";
			for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
			{
				if(it->second.m_Pieces.size()>0)
				{
					ErrStr += it->second.m_Pieces[0].m_pPiece->GetPieceGroup()->GetName();
					ErrStr += "\n";
				}
			}
			ErrStr += "与模型骨架:";
			ErrStr += m_pSkeFrame->GetName();
			ErrStr += "不符!";

			ClearMeshPiece();
			GenErr(GraphicErr::GetErrTypeStr(OutUse_Err), ErrStr);
		}
	}
	if ( m_pSkeFrame )
		m_pSkeFrame->SetSkeletalMask( SkeletalMask );
}

bool CBaseModel::IsMeshsPieceEmpty()
{
	return m_MeshsPiece.empty();
}

bool CBaseModel::ModifyMesh(const TCHAR* szClassName, CVertexHardwareBuffer* pBuffer, size_t Offset,ModifyOperator fModifyOp , uint pieceIndex,size_t VertexNum ,CVertexElementSemantic Ves , CVertexElementType Vet, uint Index )
{
	RenderPieceMap::iterator it = m_MeshsPiece.find(szClassName ? szClassName : "");
	if (it == m_MeshsPiece.end())
		return false;
	uint size = it->second.m_Pieces.size();
	if( pieceIndex >= size )
		return false;

	PieceModify& Modify = it->second.m_Pieces[pieceIndex].m_Modify;
	CPiece*	pPiece = it->second.m_Pieces[pieceIndex].m_pPiece;

	if( VertexNum!=0 && VertexNum != pPiece->GetMesh()->GetVerNum() ) //加了一个断言。。。。
		return false;
	ClearModelRender();
	switch (fModifyOp)
	{
	case MO_ADD:
		{
			if ( Modify.Sorce != 0 )
				return false;
			Modify.pAddVB	= pBuffer;
			Modify.Offset	= Offset;
			Modify.Lenght	= pPiece->GetMesh()->GetVerNum()*CVertexElement::GetTypeSize(Vet);
			Modify.Sorce	= pPiece->GetMesh()->GetVertexData()->vertexBufferBinding->GetNextIndex();
			Modify.AddFVF	= CVertexElement::GetBaseFormatID(Modify.Sorce,Ves,Index);

			CVertexDeclaration* pVD = pPiece->GetMesh()->GetVertexData()->vertexDeclaration;
			if ( NULL == CVertexDeclaration::GET_VERDECL( Modify.AddFVF | pVD->GetVertexFormatID() ) )
			{
				CVertexDeclaration* pVDdest = CDataSources::GetInst()->NewVertexDeclaration( Modify.AddFVF | 	pVD->GetVertexFormatID());

				const CVertexDeclaration::VertexElementList elems = pVD->GetElements();
				CVertexDeclaration::VertexElementList::const_iterator ei, eiend;
				eiend = elems.end();
				for (ei = elems.begin(); ei != eiend; ++ei)
				{
					pVDdest->AddElement(
						ei->GetSource(),
						ei->GetOffset(),
						ei->GetType(),
						ei->GetSemantic(),
						ei->GetIndex() );
				}
				pVDdest->AddElement( Modify.Sorce, 0, Vet, Ves, Index );
			}
			return true;
		}
		break;
	case MO_DELETE:
		{
			Modify.Sorce = 0;
			Modify.pAddVB = NULL;
			return true;
		}
		break;
	case MO_EDIT:
		{
			if ( Modify.Sorce == 0 || Modify.pAddVB==NULL)
				return false;

			Modify.pAddVB	= pBuffer;
			Modify.Offset	= Offset;
			Modify.Lenght	= pPiece->GetMesh()->GetVerNum()*CVertexElement::GetTypeSize(Vet);
			Modify.AddFVF	= CVertexElement::GetBaseFormatID(Modify.Sorce,Ves,Index);
			CVertexDeclaration* pVD = pPiece->GetMesh()->GetVertexData()->vertexDeclaration;
			if ( NULL == CVertexDeclaration::GET_VERDECL( Modify.AddFVF | pVD->GetVertexFormatID() ) )
			{
				CVertexDeclaration* pVDdest =CDataSources::GetInst()->NewVertexDeclaration( Modify.AddFVF | 	pVD->GetVertexFormatID());

				const CVertexDeclaration::VertexElementList elems = pVD->GetElements();
				CVertexDeclaration::VertexElementList::const_iterator ei, eiend;
				eiend = elems.end();
				for (ei = elems.begin(); ei != eiend; ++ei)
				{
					pVDdest->AddElement(
						ei->GetSource(),
						ei->GetOffset(),
						ei->GetType(),
						ei->GetSemantic(),
						ei->GetIndex() );
				}
				pVDdest->AddElement( Modify.Sorce, 0, Vet, Ves, Index );
			}
			return true;
		}
		break;
	}
	return false;
}


bool CBaseModel::GetMatrix( CMatrix& Matrix, uint32 SkeletalID, uint32 SocketID )
{
	if(SkeletalID == 0 || m_pRenderAniInst == NULL || m_pRenderAniInst->m_CurrentTime == 0 )
	{
		Matrix = GetWorldMatrix();
		return true;
	}

	if ( IsVaildSkeId(SkeletalID) )
	{
		CMatrixUnit& Unit = *m_pRenderAniInst->m_pMatrixUnit[SkeletalID];
		CSkeletalUnit*	pSkeUnit = (*m_pSkeFrame)[SkeletalID];
		if ( pSkeUnit && pSkeUnit->m_SocketList.size() )
			Matrix = pSkeUnit->m_SocketList[SocketID].m_Socket*Unit.GetMATRIX()*GetWorldMatrix();
		else
			Matrix =Unit.GetMATRIX()*GetWorldMatrix();
		return true;
	}
	return false;
}

bool CBaseModel::GetLocalMatrix( CMatrix& Matrix, uint32 SkeletalID, uint32 SocketID )
{
	if(SkeletalID == 0)
	{
		Matrix = CMatrix::UNIT_MATRIX;
		return true;
	}

	if ( m_pRenderAniInst->m_CurrentTime && IsVaildSkeId(SkeletalID) )
	{
		CMatrixUnit& Unit = *m_pRenderAniInst->m_pMatrixUnit[SkeletalID];
		CSkeletalUnit*	pSkeUnit = (*m_pSkeFrame)[ SkeletalID ];
		if ( pSkeUnit && SocketID < pSkeUnit->m_SocketList.size() )
			Matrix = pSkeUnit->m_SocketList[SocketID].m_Socket*Unit.GetMATRIX();
		else
			Matrix = Unit.GetMATRIX();
		return true;
	}
	return false;
}

bool CBaseModel::IsVaildSkeId(BYTE SkeletalID)
{
	return m_pSkeFrame && SkeletalID <= m_pRenderAniInst->m_MaxUseSkeletalID && NULL != m_pRenderAniInst->m_pMatrixUnit[SkeletalID] ;
}
