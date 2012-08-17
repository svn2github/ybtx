#include "stdafx.h"
#include "CModel.h"
#include "BaseHelper.h"
#include "CShadowFilter.h"
#include "CRenderGroup.h"
#include "CRenderPipeline.h"
#include "CGraphic.h"
#include "CVertexAniMesh.h"
#include "CStaticMesh.h"
#include "CSkeletalMesh.h"
#include "CRenderMatrix.h"
#include "CAnimationController.h"
#include "CPieceGroup.h"
//#include "TraceHelper.h"
#include "CAnimation.h"
#include "CVertexDeclaration.h"
#include "TSqrAllocator.inl"
#include "IModelHandler.h"
#include "CColor4.inl"

#define DEF_CENTER_ID 1
CModel::CModel(ILogicHandler* pHandler)
:CBaseModel(pHandler)
, m_eRenderStyle(ROS_UNKNOWN)
{
	m_bNeedPushRender       = false;
	m_bLoop					= TRUE;
	m_pRenderAniInst		= &m_RenderMatrix;
	m_Material.Diffuse		= CColor4::Gray;
	m_Material.Ambient		= CColor4::Gray;
	m_Material.Specular		= CColor4::Black;
	m_Material.Emissive		= CColor4::White;

	//m_InvLoop				= false;
}

CModel::~CModel(void)
{
	ClearModelRender();
}

void	CModel::_UpdateSkeFrm(void)
{
	m_RenderMatrix.Initialization( (CSkeletalFrame*)m_pSkeFrame );

	if( -1 != m_CenterLinkId )
	{
		m_RenderMatrix.m_SkeletalMask.AddSkeletal(uint32(m_CenterLinkId));
		if(m_CenterLinkId > m_RenderMatrix.m_MaxUseSkeletalID&&m_RenderMatrix.m_MaxUseSkeletalID!=0)
		{
			if ( m_pSkeFrame )
				m_pSkeFrame->SetSkeletalMask( m_RenderMatrix.m_SkeletalMask );
			m_RenderMatrix.SetSkeletalMask(m_RenderMatrix.m_SkeletalMask);
		}
	}
	int m_CenterBoneID = GetSkeletalIdByName("Bip01");
	if(m_CenterBoneID!=1)
		m_CenterBoneID = GetSkeletalIdByName("Bone01");
	m_RenderMatrix.m_AniEndFrame  = WORD( m_pAnimationGroup->GetAnimateNum() ? m_pAnimationGroup->GetAniById(0)->GetFrameNum() : 0 );
}

void	CModel::_UpdateAniGrp(void)
{
	CAnimate* pAni = NULL;
	pAni = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);
	
	if(pAni)
		m_RenderMatrix.m_AniEndFrame = min( m_RenderMatrix.m_AniEndFrame, (uint16)pAni->GetFrameNum());
}

void CModel::GetScaleDimension( float* pHeightBuf, int& Width, int&Depth, float GridSize )
{
	Width = ( (int)( ( m_Scale.x/GridSize )/2 + 0.5f ) )*2 + 1;
	Depth = ( (int)( ( m_Scale.z/GridSize )/2 + 0.5f ) )*2 + 1;

	if ( pHeightBuf )
	{
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
		{
			for ( int iPieces = 0; iPieces < (int)it->second.m_Pieces.size(); iPieces++ )
			{
				CPiece* pPiece = it->second.m_Pieces[iPieces].m_pPiece;
				if ( pPiece && pPiece->GetMesh()->GetVertexType() != VT_SOFTBODYANI )
				{
					CMesh* pMesh = pPiece->GetMesh();//->GetLevel()[0];
					for ( int i = 0; i < (int)pMesh->GetVerNum(); i++ )
					{
						CVector3f Ver = pMesh->GetVertex(i);
						int x = (int)( ( ( Ver.x - m_Off.x )/GridSize )/2 + 0.5f );
						int y = (int)( ( ( Ver.z - m_Off.z )/GridSize )/2 + 0.5f );
						x = x + Width/2;
						y = y + Depth/2;
						pHeightBuf[ y*Width + x ] = max( Ver.y, pHeightBuf[ y*Width + x ] );
					}
				}
			}
		}
	}
}

//得到当前动画名
IDNAME CModel::GetCurAnimateName()
{
	IDNAME Name;
	CAnimate* Ani = NULL;

	Ani = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);
	if ( Ani )
		Name = Ani->GetName();

	return Name;
}

//得到当前动画最大帧数
int CModel::GetAnimateMaxFrameNum()
{
	if ( m_RenderMatrix.m_IsAniBlend )
		return (int)m_RenderMatrix.m_BlendFrame;
	else
	{
		if(!m_pAnimationGroup)
			return 0;
		CAnimate* Ani = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);
		if ( Ani )
			return Ani->GetFrameNum();
	}
	return 0;
}


void CModel::ClearAnimateGroup()
{
	m_RenderMatrix.ClearSkeletaInfo();
}

bool CModel::SetDefaultAniSep( int32 id, int32 DelayTime , float AniSpeed )
{
	if( id<0 || m_pAnimationGroup->GetAnimateNum() == 0 )
		return false;

	CAnimateSeparation* pAniSep = m_RenderMatrix.m_pSkeletalSepList[id];

	if ( pAniSep != NULL )
	{
		pAniSep->m_AniEndFrame	= m_RenderMatrix.m_AniEndFrame;
		pAniSep->m_PlayMode		= m_bLoop?APM_LOOP_PLAY:APM_PLAY_ONCE;

		if(pAniSep->m_CurAnimate == m_RenderMatrix.m_CurAnimate)
		{				
			if(AniSpeed>0.0f)
				pAniSep->m_AniSpeed = AniSpeed;
			return true;
		}

		pAniSep->m_AnimationName = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate)->GetName();
		pAniSep->m_CurAnimate	= m_RenderMatrix.m_CurAnimate;
		pAniSep->m_State		= APS_FADEIN;		
		pAniSep->m_BlendFrame	= (WORD)( DelayTime/m_ParentFrameRatio );
		pAniSep->m_CurFrame		= m_RenderMatrix.m_CurFrame + pAniSep->m_BlendFrame;
		pAniSep->m_AniSpeed		= AniSpeed>0.0f?AniSpeed:1.0f;
		return true;
	}

	pAniSep = m_RenderMatrix.m_pSkeletalSepList[id] = new CAnimateSeparation(id);

	pAniSep->m_AnimationName = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate)->GetName();
	pAniSep->m_AniEndFrame	= m_RenderMatrix.m_AniEndFrame;
	pAniSep->m_PlayMode		= m_bLoop?APM_LOOP_PLAY:APM_PLAY_ONCE;
	pAniSep->m_CurAnimate	= m_RenderMatrix.m_CurAnimate;
	pAniSep->m_State		= APS_NORMAL;		
	pAniSep->m_BlendFrame	= (WORD)( DelayTime/m_ParentFrameRatio );
	pAniSep->m_CurFrame		= m_RenderMatrix.m_CurFrame;
	pAniSep->m_AniSpeed		= AniSpeed>0.0f?AniSpeed:1.0f;
	return true;
}

//设置动作分离
bool CModel::SetAniSep( IDNAME AniName, int32 id, bool bLoop, int32 DelayTime, float AniSpeed )
{
	if( id<0 || m_pAnimationGroup->GetAnimateNum() == 0 || size_t(id) >= m_RenderMatrix.m_pSkeletalSepList.size() )
		return false;

	CAnimateSeparation* pAniSep = NULL;
	pAniSep = m_RenderMatrix.m_pSkeletalSepList[id];

	if ( pAniSep == NULL )
		pAniSep = m_RenderMatrix.m_pSkeletalSepList[id] = new CAnimateSeparation(id);


	int32 AniNo = m_pAnimationGroup->GetAniIDbyName( AniName );

	if ( FAILED( AniNo ) )
		return false;

	if(pAniSep->m_CurAnimate == static_cast<WORD>(AniNo))
	{
		if(AniSpeed>0.0f)
			pAniSep->m_AniSpeed = AniSpeed;
		return true;
	}

	CAnimate* Ani = m_pAnimationGroup->GetAniById(AniNo);

	pAniSep->m_AnimationName = AniName;
	pAniSep->m_AniEndFrame	= (WORD)Ani->GetFrameNum();
	pAniSep->m_PlayMode		= bLoop?APM_LOOP_PLAY:APM_PLAY_ONCE;
	pAniSep->m_CurAnimate	= static_cast<WORD>(AniNo);
	pAniSep->m_State		= APS_FADEIN;		
	pAniSep->m_BlendFrame	= (WORD)( DelayTime/m_ParentFrameRatio );
	pAniSep->m_CurFrame		= 0;
	pAniSep->m_AniSpeed		= AniSpeed>0.0f?AniSpeed:1.0f;
	return true;
}

//设置动作分离
bool CModel::SetAniSep( IDNAME AniName, bool bLoop, int32 DelayTime, float AniSpeed )
{
	int id = GetSkeletalIdByName("Bip SpinePro");
	if(-1==id)
		id = GetSkeletalIdByName("Bip01 Spine1");
	else
		id = GetSkeletalIdByName("Bip01 Spine");
	return SetAniSep( AniName,id,bLoop,DelayTime,AniSpeed );
}

void CModel::ClearAniSep( int32 PartID ,int32 DelayTime )
{
	if( PartID < 0 || size_t(PartID) >=m_RenderMatrix.m_pSkeletalSepList.size())
		return;

	CAnimateSeparation * pAniSep = NULL;
	pAniSep = m_RenderMatrix.m_pSkeletalSepList[PartID];
	if(NULL!=pAniSep && pAniSep->m_State!= APS_FADEOUT )
	{
		pAniSep->m_State		= APS_FADEOUT;
		pAniSep->m_CurFrame		= 0;
		pAniSep->m_BlendFrame	= (WORD)( DelayTime/m_ParentFrameRatio );
	}
}

CAnimate* CModel::GetCurAni(int32 PartID)
{
	if( PartID < 0 || size_t(PartID) >=m_RenderMatrix.m_pSkeletalSepList.size())
		return false;

	CAnimateSeparation * pAniSep = NULL;
	pAniSep = m_RenderMatrix.m_pSkeletalSepList[PartID];
	if(pAniSep)
		return m_pAnimationGroup->GetAniById(pAniSep->m_CurAnimate);
	else
		return NULL;
}

bool CModel::SetAniFromSep( int32 PartID,int32 DelayTime )
{
	if( PartID < 0 || size_t(PartID) >=m_RenderMatrix.m_pSkeletalSepList.size())
		return false;

	CAnimateSeparation * pAniSep = NULL;
	pAniSep = m_RenderMatrix.m_pSkeletalSepList[PartID];
	if(NULL!=pAniSep && pAniSep->m_State!= APS_FADEOUT && 
		m_pAnimationGroup->GetAnimateNum() > 0 &&
		pAniSep->m_CurAnimate!=m_RenderMatrix.m_CurAnimate )
	{
		m_Velocity = pAniSep->m_AniSpeed;

		//bool  isHasAni = false;
		for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
		{
			it->second.SetNextAnimation( pAniSep->m_AnimationName );
			//nMaxFrame = (uint16)max( nMaxFrame, (int)it->second.m_MaxVerAniNo );
		}

		CAnimate* Ani = m_pAnimationGroup->GetAniById(pAniSep->m_CurAnimate);

		m_bLoop = pAniSep->m_PlayMode == APM_LOOP_PLAY;
		if ( DelayTime <= 0 )
		{
			bool bContinue =
				m_RenderMatrix.m_CurAnimate  == (WORD)pAniSep->m_CurAnimate &&
				m_RenderMatrix.m_Result.m_IsAniEnd && !DelayTime;

			m_RenderMatrix.m_CurAnimate    = pAniSep->m_CurAnimate ;
			m_RenderMatrix.m_IsBlendStart  = FALSE;
			m_RenderMatrix.m_IsAniBlend    = FALSE;
			m_RenderMatrix.m_AniStartFrame = 0;//FmPair.nStart;
			m_RenderMatrix.m_AniEndFrame   = WORD(pAniSep->m_AniEndFrame);

			if ( !bContinue )
			{
				//m_RenderMatrix.m_PreFrameNO = m_RenderMatrix.m_CurFrame;
				m_RenderMatrix.m_CurFrame = pAniSep->m_CurFrame;
			}
		}
		else
		{
			m_RenderMatrix.m_BlendFrame	   = (WORD)( DelayTime/m_ParentFrameRatio );
			m_RenderMatrix.m_CurAnimate    = (WORD)pAniSep->m_CurAnimate;
			m_RenderMatrix.m_IsBlendStart  = FALSE;
			m_RenderMatrix.m_IsAniBlend    = m_Amalgamation;
			//m_RenderMatrix.m_PreFrameNO    = m_RenderMatrix.m_CurFrame;
			m_RenderMatrix.m_CurFrame      = 0.0f;
			m_RenderMatrix.m_AniStartFrame = WORD(pAniSep->m_CurFrame + 0.5f + m_RenderMatrix.m_BlendFrame);
			m_RenderMatrix.m_AniEndFrame   = WORD(pAniSep->m_AniEndFrame);
		}
		return true;
	}
	return false;
}

bool CModel::IsSepflag( int32 id )
{
	CAnimateSeparation* pAniSep =  m_RenderMatrix.m_pSkeletalSepList[id];
	return NULL!=pAniSep;
}

void CModel::StopAni( int32 PartID )
{
	if( -1 == PartID )
	{
		m_RenderMatrix.m_nStopAni = true;
	}
	else if( STOPALLANI == PartID )
	{
		m_RenderMatrix.m_nStopAni = true;
		CAnimateSeparationPtrVector& ppAniSep = m_RenderMatrix.m_pSkeletalSepList;
		for ( uint32 i = 0; i <= m_RenderMatrix.m_MaxUseSkeletalID; ++i )
		{
			if(NULL!=ppAniSep[i])
				ppAniSep[i]->m_nStopAni = true;
		}
	}
	else 
	{
		CAnimateSeparation* pAniSep =  m_RenderMatrix.m_pSkeletalSepList[ PartID ];
		if( pAniSep!= NULL )
			pAniSep->m_nStopAni = true;
	}
}

void CModel::ResumeAni( int32 PartID )
{
	if( -1 == PartID )
	{
		m_RenderMatrix.m_nStopAni = false;
	}
	else if( STOPALLANI == PartID )
	{
		m_RenderMatrix.m_nStopAni = false;
		CAnimateSeparationPtrVector& ppAniSep = m_RenderMatrix.m_pSkeletalSepList;
		for ( uint32 i = 0; i <= m_RenderMatrix.m_MaxUseSkeletalID; i++ )
		{
			if(NULL!=ppAniSep[i])
				ppAniSep[i]->m_nStopAni = false;
		}
	}
	else 
	{
		CAnimateSeparation* pAniSep =  m_RenderMatrix.m_pSkeletalSepList[ PartID ];
		if( pAniSep!= NULL )
			pAniSep->m_nStopAni = false;
	}
}

void CModel::SetVisible(bool isVisible)
{
	if ( m_isVisible!= isVisible )
	{
		CBaseModel::SetVisible(isVisible);
		if(m_isVisible)
		{
			if(m_BoxMap.size()>0 && m_pSkeFrame!=NULL)
					m_OrgAABB.setNull();
		}
	}
}

//设置下一个动画，延迟时间小于5ms则不进行动画融合
bool CModel::SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop, int32 DelayTime, float AniSpeed )
{
	//for ( GMap< WORD, CRenderPair >::iterator it = m_SocketLink.begin(); it != m_SocketLink.end(); ++it )
	//	(*it).second.m_pRenderObj->SetNextAnimation( AniName, FmPair, bLoop, DelayTime );
	//for ( GMap< BYTE, CRenderPair >::iterator it = m_SkeletalLink.begin(); it != m_SkeletalLink.end(); ++it )
	//	(*it).second.m_pRenderObj->SetNextAnimation( AniName, FmPair, bLoop, DelayTime );
	//for ( GMap< GString, CRenderPair >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); ++it )
	//	(*it).second.m_pRenderObj->SetNextAnimation( AniName, FmPair, bLoop, DelayTime );
	uint16 nMaxFrame = 0;
	if( AniSpeed >= 0.0f )
		m_Velocity = AniSpeed;

	//bool  isHasAni = false;	
	//if(!m_BoxMap.empty() && m_isVisible)
	if(!m_BoxMap.empty() && m_pSkeFrame && m_isVisible) // weikun box
		m_OrgAABB.setNull();

	//for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
	//{
	//	it->second.SetNextAnimation( AniName );
	//	nMaxFrame = (uint16)max( nMaxFrame, (int)it->second.m_MaxVerAniNo );
	//}

	if ( m_pAnimationGroup->GetAnimateNum() > 0 )
	{
		uint32 AniNo = m_pAnimationGroup->GetAniIDbyName( AniName );

		if ( FAILED( AniNo ) )
			return false;

		CAnimate* Ani = m_pAnimationGroup->GetAniById(AniNo);

		m_CurAniFootStepFrames = Ani->GetFootStepFrames();

		m_bLoop = bLoop;
		if ( DelayTime <= 0 )
		{
			bool bContinue =
				m_RenderMatrix.m_CurAnimate  == (WORD)AniNo &&
				m_RenderMatrix.m_AniEndFrame == FmPair.nStart &&
				m_RenderMatrix.m_Result.m_IsAniEnd && !DelayTime;

			m_RenderMatrix.m_CurAnimate    = (WORD)AniNo;
			m_RenderMatrix.m_IsBlendStart  = FALSE;
			m_RenderMatrix.m_IsAniBlend    = FALSE;
			m_RenderMatrix.m_AniStartFrame = FmPair.nStart;
			m_RenderMatrix.m_AniEndFrame = min( uint16( FmPair.nEnd ), (uint16)Ani->GetFrameNum() );

			if ( !bContinue )
			{
				int32 Frame = (int32)( abs( DelayTime )/m_ParentFrameRatio );
				int32 FrameNum = m_RenderMatrix.m_AniEndFrame - m_RenderMatrix.m_AniStartFrame;
				Frame = m_RenderMatrix.m_AniEndFrame == m_RenderMatrix.m_AniStartFrame ?
					0 : Frame%( FrameNum );
				if ( Frame )
					Frame = FrameNum - Frame;

				//m_RenderMatrix.m_PreFrameNO = m_RenderMatrix.m_CurFrame;
				m_RenderMatrix.m_CurFrame = (float)( FmPair.nStart + Frame );

			}
		}
		else
		{
			m_RenderMatrix.m_BlendFrame    = (WORD)( DelayTime/m_ParentFrameRatio );
			m_RenderMatrix.m_CurAnimate    = (WORD)AniNo;
			m_RenderMatrix.m_IsBlendStart  = FALSE;
			m_RenderMatrix.m_IsAniBlend    = m_Amalgamation;
			//m_RenderMatrix.m_PreFrameNO    = m_RenderMatrix.m_CurFrame;
			m_RenderMatrix.m_CurFrame      = 0.0f;
			m_RenderMatrix.m_AniStartFrame = FmPair.nStart;
			m_RenderMatrix.m_AniEndFrame   = min( uint16( FmPair.nEnd ), (uint16)Ani->GetFrameNum());
		}
		return true;
	}
	else
	{
		m_RenderMatrix.m_AniStartFrame = FmPair.nStart;
		m_RenderMatrix.m_AniEndFrame = min( uint16( FmPair.nEnd ), nMaxFrame );

		if ( DelayTime <= 0 )
		{
			int32 Frame = (int32)( abs( DelayTime )/m_ParentFrameRatio );
			int32 FrameNum = m_RenderMatrix.m_AniEndFrame - m_RenderMatrix.m_AniStartFrame;
			Frame = m_RenderMatrix.m_AniEndFrame == m_RenderMatrix.m_AniStartFrame ?
				0 : Frame%( FrameNum );
			if ( Frame )
				Frame = FrameNum - Frame;

			m_RenderMatrix.m_CurFrame = (float)( FmPair.nStart + Frame );
		}
	}

	return false;
	//return isHasAni;
}

//void CModel::ShowPieceBuffer(const char* szPieceName,bool bShow)
//{
//	m_showPieceBuffer[szPieceName] = bShow;
//}

void CModel::ShowPiece(const char* szPieceName,bool bShow/*,bool& isDel*/)
{
	bool isChange = false;
	SQR_TRY
	{		
		if ( szPieceName && szPieceName[0] != 0 )
		{
			GString szClassName = szPieceName;
			bool noNum = true;
			for ( int i = (int)szClassName.size() - 1; i > 0 ; i-- )
			{
				if ( szClassName[i] < '0' || szClassName[i] > '9' )
					break;
				noNum = false;
				szClassName.erase( i, 1 );
			}
			GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.find( szClassName );
			if ( it != m_MeshsPiece.end() )
			{
				int size= it->second.m_Pieces.size();
				for (int i = 0;i<size;i++)
				{
					if ( noNum || string(it->second.m_Pieces[i].m_pPiece->GetPieceName()).find(szPieceName) != string::npos)
					{
						isChange |= it->second.m_Pieces[i].m_bShow != bShow;
						it->second.m_Pieces[i].m_bShow = bShow ;
						//isDel = true;
						break;
					}
				}
			}
		}
		else
		{
			for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
			{	
				isChange |= it->second.m_bShow != bShow;
				it->second.m_bShow = bShow;
			}
		}
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;

	if(isChange)
		ClearModelRender();
}

void CModel::_UpdateMesh(SKELETAL_MASK& SkeletalMask)
{
	m_BoxMap.clear();
	CBaseModel::_UpdateMesh(SkeletalMask);
	set<uint32>::iterator it,eit = m_AddSkePool.end();
	for( it = m_AddSkePool.begin(); it!=eit; ++it )
		SkeletalMask.AddSkeletal( *it );
	m_RenderMatrix.SetSkeletalMask( SkeletalMask );
}

void CModel::_AddAABB(CPiece* pPiece)
{
	if (m_pSkeFrame)// weikun box
	{
		CMesh::BoxMap::iterator bit = pPiece->GetMesh()->m_BoxMap.begin();
		CMesh::BoxMap::iterator bite = pPiece->GetMesh()->m_BoxMap.end();
		for (;bit!=bite;++bit)
		{
			m_BoxMap.push_back(BoxPair(bit->first,&bit->second));
		}
	} 
	else
	{
		m_BoxMap.push_back(BoxPair(0,&pPiece->GetAABox()));
	}
}

bool CModel::GetSocketXYZ( const char* strSocketName, CVector3f & p)
{
	if ( !this->IsValid() )
	{
		p.x = p.y = p.z = 0;
		return false;
	}

	if ( m_pSkeFrame == NULL )
	{
		const CVector3f & scp = this->GetBoxScale();
		p.x = scp.x / 2.0f;
		p.y = scp.y / 2.0f;
		p.z = scp.z / 2.0f;
		return false;
	}

	int SocketId = 0;
	int SkeletalId = m_pSkeFrame->GetSkeletalIDBySocketName( IDNAME(strSocketName), SocketId );
	if ( SkeletalId == -1 )
	{
		const  CVector3f & scp = this->GetBoxScale();
		p.x = scp.x / 2.0f;
		p.y = scp.y / 2.0f;
		p.z = scp.z / 2.0f;
		return false;
	}
	CMatrix SocketMatrix;
	this->GetMatrix( SocketMatrix, SkeletalId, SocketId );
	p.x = SocketMatrix._41;
	p.y = SocketMatrix._42;
	p.z = SocketMatrix._43;
	return true;
}

uint32 CModel::GetSkeletalCount(void)
{
	return m_RenderMatrix.GetSkeletalCount();
}

BYTE CModel::GetMaxUseSkeletal(void)
{
	return m_RenderMatrix.m_MaxUseSkeletalID;
}

//------------------------------------------------------------------------------
void CModel::ClearRsMask(bool bClear)
{
	for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
	{
		if (it->second.m_RS.m_nMask != 0)
		{
			memset( &it->second.m_RS, 0, sizeof( CPieceRS ) );
		}
	}

	if(bClear)
	{
		ClearModelRender();
	}
}


void CModel::SetFixMatrix(const CMatrix& parent, const CMatrix& fix)
{
	SetParentMatrix(parent);
	m_RenderMatrix.m_MatWorld = fix * GetWorldMatrix();
}

////////////////////////////////////////////////////////////////////////
const AniResult CModel::Render( uint32 uCurTime, RenderObjStyle* pRORS )
{
	if ( m_RenderMatrix.m_CurrentTime != uCurTime )
	{
		if(m_RenderMatrix.m_CurrentTime!=0xFFFFFFFF)
			CalculateMatrix( uCurTime - m_RenderMatrix.m_CurrentTime );
		else
			CalculateMatrix(0);
		m_RenderMatrix.m_CurrentTime = uCurTime;
	}
	if (!m_isVisible || pRORS == NULL)
		return m_RenderMatrix.m_Result;

	RenderObjStyle* pRenderRS = (pRORS && pRORS->m_eType == ROS_UNKNOWN) ? NULL : pRORS;

	if ( m_eRenderStyle!=pRORS->m_eType)
		ClearModelRender();

	if (NULL != pRenderRS)
	{
		if (m_bNeedPushRender)
		{
			m_eRenderStyle = pRORS->m_eType;
			sMaterial* material = (sMaterial*)(pRORS->m_nParam2);
			if (NULL != material)
			{
				m_Material = *(sMaterial*)(pRORS->m_nParam2);
			}
			else
			{
				pRORS->m_nParam2 = (uint32)&m_Material;
			}

			CGraphic::GetInst()->BeginModel(this);
			if ( pRORS->m_bRenderMesh )
				RenderMesh(  pRenderRS );
			CGraphic::GetInst()->EndModel();
			m_bNeedPushRender= false;
		}
		else
		{
			//if(bNeedRefresh)
			UpdateRenderStyle(pRenderRS);
		}
	}

	//if ( !pRORS || pRORS->m_bRenderLink )
	//	RenderLink( uCurTime, pRenderRS );

	if(m_OrgAABB.isNull())
		UpdateDynamicBox();

	return m_RenderMatrix.m_Result;
}

void CModel::UpdateBox(void)
{
	//if(m_OrgAABB.isNull())
	//	return;
	
	//m_CurAABB = m_OrgAABB;

	if( IsVaildSkeId(DEF_CENTER_ID) && m_RenderMatrix.m_CurrentTime != 0 )
	{		
		m_CenterMatrix = ((*m_pSkeFrame)[DEF_CENTER_ID]->m_InverseMatrix *
			(*m_RenderMatrix.m_pMatrixUnit[DEF_CENTER_ID])).GetMATRIX();
		m_CenterMatrix *= m_RenderMatrix.m_MatWorld;
	}
	else
		m_CenterMatrix = m_RenderMatrix.m_MatWorld;
	 
}

void   CModel::ClearModelRender(void)
{
	m_vRenderNode.clear();
	m_bNeedPushRender = true;
}

void  CModel::RenderImme(void)
{
	if (m_vRenderNode.empty())
		return;
	NodePool::iterator it = m_vRenderNode.begin();
	NodePool::iterator eit = m_vRenderNode.end();
	CRenderPipeline* crp = CRenderPipeline::GetInst();
	for (;it!=eit;++it)
	{
		(*it).param_render(*it);
		crp->Render(&(*it),&((*it).m_RP));
	}
	crp->SetTextureMatrixImmediate(0, NULL);//fix a d3d error
}

#define ANI_TRY_CATCH
//////////////////////////////////////////////////////////////////////////
void CModel::CalculateMatrix( int32 DeltaTime )
{
	m_RenderMatrix.m_Result.m_SepFrameString	= NULL;
	CAnimate* pAni = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);
	float FrameDelta = DeltaTime/m_ParentFrameRatio;

	if ( 0 == m_pAnimationGroup->GetAnimateNum() || NULL == m_pSkeFrame || NULL == pAni || !pAni->IsValid() )
	{
		m_RenderMatrix.m_CurFrame += FrameDelta * m_Velocity;
		return;
	}
	m_Amalgamation  = true;
	CAnimateSeparationPtrVector& ppAniSep =  m_RenderMatrix.m_pSkeletalSepList;
	for ( uint32 i = 0; i <= m_RenderMatrix.m_MaxUseSkeletalID; i++ )
	{
		if(!ppAniSep.empty()&&NULL!=ppAniSep[i])
			ppAniSep[i]->Update( FrameDelta, m_RenderMatrix );
	}

	FrameDelta *= m_Velocity;
	m_RenderMatrix.m_Result.m_FrameString	= NULL;
	m_RenderMatrix.m_Result.m_IsAniEnd		= true;
	m_RenderMatrix.m_Result.m_IsReset		= true;

	if ( m_RenderMatrix.m_AniEndFrame - m_RenderMatrix.m_CurFrame < 0.001f )
	{
		//m_RenderMatrix.m_PreFrameNO = m_RenderMatrix.m_CurFrame;
		m_RenderMatrix.m_CurFrame = m_RenderMatrix.m_AniEndFrame;
		m_RenderMatrix.m_Result.m_IsAniEnd		= true;
		m_RenderMatrix.m_Result.m_IsReset		= true;
	}
	else
	{
		//m_RenderMatrix.m_PreFrameNO = m_RenderMatrix.m_CurFrame;
	/*	if ( m_bFrameSynch )
		{
			m_RenderMatrix.m_IsAniBlend = m_IsParentAniBlend;
			m_RenderMatrix.m_CurFrame = m_ParentFrame;
		}
		else */
		if( !m_RenderMatrix.m_nStopAni )
		{
			m_RenderMatrix.m_CurFrame += FrameDelta;
		}
		//printf( "frame = %f\n", m_RenderMatrix.m_CurFrame );
	}
	
	m_RenderMatrix.m_Result.m_PreAnimationName = pAni->GetName();

	//如果需要动作混合但并没开始
	if ( m_RenderMatrix.m_IsAniBlend && !m_RenderMatrix.m_IsBlendStart )
	{
		m_RenderMatrix.m_CurFrame = FrameDelta;
		m_RenderMatrix.m_IsBlendStart = TRUE;
		//如果完成，返回告诉调用者：下一帧这个模型需要渲染第1帧
	}

	

#ifdef ANI_TRY_CATCH
	SQR_TRY	
	{
#endif

	m_RenderMatrix.CalculateMatrix();
	m_pSkeFrame->GetRootSkeletal().CalculateMatrix( CMatrixUnit::CMatrixUnitZero, m_RenderMatrix, *m_pAnimationGroup, *m_pSkeFrame );

#ifdef ANI_TRY_CATCH
	}
	SQR_CATCH(exp)
	{
		stringstream strTmp;
		CAnimate* pAni = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);
		strTmp << "动画:" << pAni->GetName() << " 包:" << pAni->GetRootGroup()->GetName();
		exp.AppendType("动画运算错误");
		exp.AppendMsg(strTmp.str());
		string strAgpName = pAni->GetRootGroup()->GetName();
		string::size_type index = strAgpName.rfind(".agp");
		string strAniName;
		if (index != string::npos)
		{
			strAniName  = strAgpName.substr(0, index);
			strAniName.append("_");
			strAniName.append(pAni->GetName());
			strAniName.append(".ani");
		}
		GfkVerifyPkg(exp, strAniName.c_str());	
	}
	SQR_TRY_END;
#endif

	if ( m_RenderMatrix.m_CurFrame < m_RenderMatrix.m_AniEndFrame )
	{
		m_RenderMatrix.m_Result.m_IsAniEnd = false;
		m_RenderMatrix.m_Result.m_IsReset  = false;
	}
	if ( m_RenderMatrix.m_Result.m_IsAniEnd )
	{
		//如果完成，返回告诉调用者：下一帧这个模型需要渲染第ResetFrame帧
		if ( m_bLoop || m_RenderMatrix.m_IsAniBlend )
		{
			int32 nEndFrame = m_RenderMatrix.m_IsAniBlend ? m_RenderMatrix.m_BlendFrame : m_RenderMatrix.m_AniEndFrame;
			int32 nDelFrame = (int32)( m_RenderMatrix.m_CurFrame - nEndFrame );
			int32 nTotalFrame = m_RenderMatrix.m_AniEndFrame - m_RenderMatrix.m_AniStartFrame;

			if ( nTotalFrame )
			{
				m_RenderMatrix.m_CurFrame = (float)( nDelFrame%nTotalFrame + m_RenderMatrix.m_AniStartFrame );
			}
			else
				m_RenderMatrix.m_CurFrame = m_RenderMatrix.m_AniStartFrame;

			//m_RenderMatrix.m_PreFrameNO			= m_RenderMatrix.m_AniStartFrame;
			m_RenderMatrix.m_IsAniBlend			= false;
			m_RenderMatrix.m_Result.m_IsAniEnd	= false;
			m_RenderMatrix.m_Result.m_IsReset	= true;
		}
		else
		{
			m_RenderMatrix.m_CurFrame = (float)m_RenderMatrix.m_AniEndFrame;
			if(m_RenderMatrix.m_pHandler)
			{
				SQR_TRY
				{
					m_RenderMatrix.m_pHandler->OnAnimationEnd( (const char*)pAni->GetName() );
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
	m_RenderMatrix.m_Result.m_FrameString = pAni->GetFrameString((WORD)m_RenderMatrix.m_CurFrame);
	//以下是顶点动画,暂时不支持
	//else
	//{
	//	int nMaxFrame = 0;
	//	GMap< GString, CRenderPiece >::iterator it;
	//	for (it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
	//	{
	//		nMaxFrame = max( nMaxFrame, (int)it->second.m_MaxVerAniNo );
	//		if ( it->second.m_MaxVerAniNo > m_RenderMatrix.m_CurFrame )
	//			break;
	//	}
	//	if ( it == m_MeshsPiece.end() )
	//	{
	//		if ( m_bLoop || m_RenderMatrix.m_IsAniBlend )
	//		{
	//			int32 nEndFrame = m_RenderMatrix.m_IsAniBlend ? m_RenderMatrix.m_BlendFrame : m_RenderMatrix.m_AniEndFrame;
	//			int32 nDelFrame = (int32)( m_RenderMatrix.m_CurFrame - nEndFrame );
	//			int32 nTotalFrame = nEndFrame - m_RenderMatrix.m_AniStartFrame;

	//			if ( nTotalFrame )
	//			{
	//				/*if(m_InvLoop)
	//				m_RenderMatrix.m_CurFrame = (float)( nEndFrame - nDelFrame%nTotalFrame);
	//				else*/
	//				m_RenderMatrix.m_CurFrame = (float)( nDelFrame%nTotalFrame + m_RenderMatrix.m_AniStartFrame );
	//			}
	//			else
	//				m_RenderMatrix.m_CurFrame = m_RenderMatrix.m_AniStartFrame;
	//		}
	//		else
	//			m_RenderMatrix.m_CurFrame = (float)m_RenderMatrix.m_AniEndFrame;
	//	}
	//	else
	//	{
	//		m_RenderMatrix.m_Result.m_IsAniEnd = false;
	//		m_RenderMatrix.m_Result.m_IsReset  = false;
	//	}
	//}
	m_RenderMatrix.m_Result.m_CurFrame		= m_RenderMatrix.m_CurFrame;
	m_RenderMatrix.m_Result.m_InAniBlend = m_RenderMatrix.m_IsAniBlend;
	m_RenderMatrix.m_IsRended = true;
}

////////////////////////////////////////////////////////////////////////
// 功能：渲染模型MESH
// 参数：D3DXMATRIX& matWorld    世界矩阵,
//       CVector3f& CameraPos  摄像机空间位置
//       float DeltaTime         和上一帧的时间差
// 返回：略
//////////////////////////////////////////////////////////////////////////
int CModel::RenderMesh( RenderObjStyle* pRORS )
{
	float Lengh = 0; /*D3DXVec3Length( &Dist );*/

	for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); ++it )
		(*it).second.Render( Lengh, m_RenderMatrix, pRORS );
	return 0;
};

int32 CModel::AddSkeletal( const char* name )
{
	if(NULL==m_pSkeFrame)
		return -1;

	int32 id = m_pSkeFrame->GetSkeletalIDByName( name );
	if( id == -1 )
		return -1;
	m_AddSkePool.insert((uint32)id);
	if( !m_RenderMatrix.m_SkeletalMask.HaveSkeletal( (uint32)id ) )
	{
		m_RenderMatrix.m_SkeletalMask.AddSkeletal( (uint32)id );
		if(id > m_RenderMatrix.m_MaxUseSkeletalID)
		{
			if ( m_pSkeFrame )
				m_pSkeFrame->SetSkeletalMask( m_RenderMatrix.m_SkeletalMask );
			m_RenderMatrix.SetSkeletalMask(m_RenderMatrix.m_SkeletalMask);
		}
		GfkSafeDelete( m_RenderMatrix.m_pMatrixUnit[id] );
		GfkSafeDelete( m_RenderMatrix.m_pMatrixUnitOrg[id] );
		m_RenderMatrix.m_pMatrixUnit[id]	= new CMatrixUnit;
		m_RenderMatrix.m_pMatrixUnitOrg[id] = new CMatrixUnit;
	}
	return id;
}

void CModel::DelSkeletal( const char* name )
{
	if(NULL==m_pSkeFrame)
		return;

	int32 id = m_pSkeFrame->GetSkeletalIDByName( name );
	m_RenderMatrix.m_SkeletalMask.DelSkeletal( id );
	m_AddSkePool.erase(id);
}

void CModel::ClearAddSke(void)
{
	set<uint32>::iterator it,eit = m_AddSkePool.end();
	for( it = m_AddSkePool.begin(); it!=eit; ++it )
		m_RenderMatrix.m_SkeletalMask.DelSkeletal( *it );
	m_AddSkePool.clear();
}

void CModel::DelFramework()
{
	ClearModelRender();
	m_RenderMatrix.Release();
	m_pSkeFrame = NULL;
	m_pAnimationGroup->Destroy();
	m_RenderMatrix.m_pMatrixUnit.resize(1,new CMatrixUnit);//       = new CMatrixUnit*[1];
	m_RenderMatrix.m_pMatrixUnitOrg.resize(1,new CMatrixUnit);//   = new CMatrixUnit*[1];
	m_RenderMatrix.m_pSkeletalSepList.resize(1,NULL);//  = new CAnimateSeparation*[1];
	m_RenderMatrix.m_pAniCtrlList.resize(1,NULL);// 	   = new CAnimationController*[1];
	m_RenderMatrix.m_MaxUseSkeletalID  = 0;
	m_RenderMatrix.m_CurrentTime = 0xFFFFFFFF;
	m_Amalgamation	   = false;

	m_AddSkePool.clear();
}

float CModel::GetCurFrameNum()
{
	return m_RenderMatrix.m_CurFrame;
}

//------------------------------------------------------------------------------
void
CModel::UpdateRenderStyle( RenderObjStyle* pRORS )
{
	NodePool::iterator it = m_vRenderNode.begin();
	NodePool::iterator ite = m_vRenderNode.end();
	float fCurFrame = m_RenderMatrix.m_CurFrame;
	sMaterial* material  = (sMaterial*)(pRORS->m_nParam2);
	if ( material && m_Material!=*material)
	{
		m_Material = *material;
	}

	for (;it!=ite;++it)
	{
		STATIC_RS& renderState = (*it).m_RP.m_RS;
		CRenderPiece* pRenderPiece = (CRenderPiece*)(*it).pUser;
		(*it).m_RP.m_Mat = m_RenderMatrix.m_MatWorld;
		(*it).param_fun((*it),m_RenderMatrix);

		if ((*it).m_eType==ROS_SHADOW)
		{
			(*it).m_ParentGroup->GetShadowFilter()->Update(&(*it));
			renderState.m_Material.Diffuse.a = m_Material.Diffuse.a;

			if ( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
				renderState.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
			continue;
		}
		SHADER_HANDLE SID		= renderState.m_ShaderType;
		ITexture* texture0		= renderState.m_Texture_S0;
		DWORD LightEnable		= renderState.m_LightEnable;
		DWORD ZTestEnable		= renderState.m_ZTestEnable;
		DWORD AlphaTestEnable	= renderState.m_AlphaTestEnable;
		DWORD ZWrite			= renderState.m_ZWrite;
		DWORD RCull				= renderState.m_Cull;

		renderState.TexRelease();
		(*it).pRenderStyle->GetCurRS( renderState, m_pSkeFrame == NULL? m_RenderMatrix.m_CurrentTime:uint32(fCurFrame * m_ParentFrameRatio), m_RenderMatrix.m_CurrentTime,(*it).pUser );

		renderState.m_ShaderType		= SID;
		renderState.m_Texture_S0		= texture0;
		renderState.m_LightEnable		= LightEnable;
		renderState.m_ZTestEnable		= ZTestEnable;
		renderState.m_ZWrite			= ZWrite;
		renderState.m_AlphaTestEnable	= AlphaTestEnable;
		renderState.m_Cull				= RCull;	

		if ( ROS_ALPHA & (*it).m_eType )
		{
			renderState.m_Material.Diffuse.a	= m_Material.Diffuse.a;
			renderState.m_Alpharef				= (uint32)( renderState.m_Alpharef*renderState.m_Material.Diffuse.a );
			renderState.m_AlpOP_S0				= TOP_MODULATE;
			renderState.m_AlpArg1_S0			= TA_TEXTURE;
			renderState.m_AlpArg2_S0			= TA_DIFFUSE;
			//renderState.m_AlpOP_S1				= TOP_DISABLE;
			if ( renderState.m_AlphaBlendEnable == FALSE )
			{
				renderState.m_AlphaBlendEnable	= TRUE;
				renderState.m_SrcBlend 			= BLEND_SRCALPHA;
				renderState.m_DestBlend 		= BLEND_INVSRCALPHA;
			}
		}

		if ( ((*it).m_eType & ROS_TRANS) == ROS_TRANS )
		{
			renderState.m_Alpharef				= 0x1;
		}

		if ( pRORS->m_nParam1&DELTARS_DIFFUSE )
		{
			renderState.m_Material.Diffuse.r *= m_Material.Diffuse.r;
			renderState.m_Material.Diffuse.g *= m_Material.Diffuse.g;
			renderState.m_Material.Diffuse.b *= m_Material.Diffuse.b;
		}		
		if ( pRORS->m_nParam1&DELTARS_AMBIENT )
		{
			renderState.m_Material.Ambient.r *= m_Material.Ambient.r;
			renderState.m_Material.Ambient.g *= m_Material.Ambient.g;
			renderState.m_Material.Ambient.b *= m_Material.Ambient.b;
		}
		if ( pRORS->m_nParam1&DELTARS_SPECULAR )
		{
			renderState.m_Material.Specular.r *= m_Material.Specular.r;
			renderState.m_Material.Specular.g *= m_Material.Specular.g;
			renderState.m_Material.Specular.b *= m_Material.Specular.b;
		}
		if ( pRORS->m_nParam1&DELTARS_EMISSIVE )
		{
			renderState.m_Material.Emissive.r *= m_Material.Emissive.r;
			renderState.m_Material.Emissive.g *= m_Material.Emissive.g;
			renderState.m_Material.Emissive.b *= m_Material.Emissive.b;
		}
		if ( pRORS->m_nParam1&DELTARS_SPECPOWER )
		{
			renderState.m_Material.Power *= m_Material.Power;
		}

		if ( pRenderPiece->m_RS.m_nMask&PIECERS_DIFFUSE )
		{
			renderState.m_Material.Diffuse.a *= float(pRenderPiece->m_RS.m_Diffuse.A) / 255.0f;
			renderState.m_Material.Diffuse.r *= float(pRenderPiece->m_RS.m_Diffuse.R) / 255.0f;
			renderState.m_Material.Diffuse.g *= float(pRenderPiece->m_RS.m_Diffuse.G) / 255.0f;
			renderState.m_Material.Diffuse.b *= float(pRenderPiece->m_RS.m_Diffuse.B) / 255.0f;

			if ( renderState.m_Material.Diffuse.a < 1.0f )
			{
				renderState.m_AlphaBlendEnable	= TRUE;
				renderState.m_SrcBlend			= BLEND_SRCALPHA;
				renderState.m_DestBlend 		= BLEND_INVSRCALPHA;
				renderState.m_AlpOP_S0			= TOP_MODULATE;
				renderState.m_Alpharef			= max(min(renderState.m_Alpharef,DWORD(renderState.m_Material.Diffuse.a * 255)),5);
			}
		}
		if ( pRenderPiece->m_RS.m_nMask&PIECERS_AMBIENT )
		{
			renderState.m_Material.Ambient.r *= float(pRenderPiece->m_RS.m_Ambient.R) / 255.0f;
			renderState.m_Material.Ambient.g *= float(pRenderPiece->m_RS.m_Ambient.G) / 255.0f;
			renderState.m_Material.Ambient.b *= float(pRenderPiece->m_RS.m_Ambient.B) / 255.0f;
		}
		if ( pRenderPiece->m_RS.m_nMask&PIECERS_SPECULAR )
		{
			renderState.m_Material.Specular.r = max( float(pRenderPiece->m_RS.m_Specular.R) / 255.0f, renderState.m_Material.Specular.r );
			renderState.m_Material.Specular.g = max( float(pRenderPiece->m_RS.m_Specular.G) / 255.0f, renderState.m_Material.Specular.g );
			renderState.m_Material.Specular.b = max( float(pRenderPiece->m_RS.m_Specular.B) / 255.0f, renderState.m_Material.Specular.b );
			renderState.m_Material.Power	    = pRenderPiece->m_RS.m_fPower;
			renderState.m_SpecularEnable		= TRUE;
		}
		if ( pRenderPiece->m_RS.m_nMask&PIECERS_SECONDTEX )
		{
			renderState.m_Texture_S1		= pRenderPiece->m_RS.m_pTexture;
			renderState.m_Texturefactor	= pRenderPiece->m_RS.m_BlendAlpha;
			renderState.m_TexOP_S1		= TOP_BLENDFACTORALPHA;
			renderState.m_TexArg1_S1	= TA_TEXTURE;
			renderState.m_TexArg2_S1	= TA_CURRENT;
			renderState.m_AlpOP_S1		= TOP_SELECTARG1;
			renderState.m_AlpArg1_S1	= TA_CURRENT;
		}

		renderState.TexAddRef();

		renderState.m_fRefractIndex = pRenderPiece->m_RS.m_fRefractIndex;
		if (pRenderPiece->m_RS.m_nMask &PIECERS_CLIPPLANE)
		{
			renderState.m_ClipPlane = pRenderPiece->m_RS.m_ClipPlane;
		}
		else
		{
			ZeroMemory(&renderState.m_ClipPlane, sizeof(renderState.m_ClipPlane));
		}
	}
}

void CModel::SetPieceClassRS( const TCHAR* szClassName, const CPieceRS& rs )
{
	GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.find( szClassName ? szClassName : "" );
	if ( it != m_MeshsPiece.end() )
		it->second.SetPieceClassRS( rs );
	else
	{
		if(szClassName==NULL || string(szClassName) == "" )
		{
			for ( GMap< GString, CRenderPiece >::iterator it = m_MeshsPiece.begin(); it != m_MeshsPiece.end(); it++ )
				it->second.SetPieceClassRS( rs );
		}
	}
}

IntersectFlag CModel::IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir )
{
	if(m_OrgAABB.isNull())
		return IF_IMPOSSIBLE;

	CRenderPipeline* pRenderPipe = CRenderPipeline::GetInst();
	//#ifdef _DEBUG
	//ShowBox = true;
	//#endif
	/*;
	if( m_pSkeFrame	&& DEF_CENTER_ID < m_RenderMatrix.m_MaxUseSkeletalID && NULL != (*m_pSkeFrame)[DEF_CENTER_ID]
	&& m_RenderMatrix.m_CurrentTime != 0 )
	{		
		TempMat = (((*m_pSkeFrame)[DEF_CENTER_ID]->m_InverseMatrix *
			(*m_RenderMatrix.m_pMatrixUnit[DEF_CENTER_ID])).GetMATRIX());
	}

	TempMat *= m_RenderMatrix.m_MatWorld;*/

	CVector3f Pos = rayPos;
	CVector3f Dir = rayDir;

	CMatrix TempMat = m_CenterMatrix;
	TempMat.Invert();
	Pos.FastMultiply(TempMat);
	Dir.Rotate(TempMat);

	if ( pRenderPipe->boxProbe( m_OrgAABB.getMinimum(), m_OrgAABB.getMaximum(), Pos, Dir ) )
	{
		//if( m_BoxMap.size() )
		if( m_BoxMap.size() && m_pSkeFrame ) // weikun box
		{
			BoxPairPool::iterator it = m_BoxMap.begin();
			BoxPairPool::iterator eit = m_BoxMap.end();
			CMatrix	TempMat;

			for (int i = 0;it!=eit;++it,++i)
			{
				GetMatrix(TempMat,(*it).BoxID,0xFFFFFFFF);
				CVector3f Pos = rayPos;
				CVector3f Dir = rayDir;

				TempMat.Invert();
				Pos.FastMultiply(TempMat);
				Dir.Rotate(TempMat);
				if (pRenderPipe->boxProbe((*it).pAABB->m_vMinimum , (*it).pAABB->m_vMaximum,
					Pos , Dir))
				{
					//#ifdef _DEBUG
					//ShowBox = false;
					//#endif
					return IF_EXCELLENT;
				}
			}
		}
		return IF_POSSIBLE;
	}
	return IF_IMPOSSIBLE;
}

void CModel::UpdateDynamicBox(void)
{
	if (m_BoxMap.size()>0 && m_pSkeFrame!=NULL)
	{
		m_OrgAABB.setNull();

		CMatrix InvMat;
		if( IsVaildSkeId(DEF_CENTER_ID) && m_RenderMatrix.m_CurrentTime != 0 )
		{		
			InvMat = ((*m_pSkeFrame)[DEF_CENTER_ID]->m_InverseMatrix *
				(*m_RenderMatrix.m_pMatrixUnit[DEF_CENTER_ID])).GetMATRIX();
			InvMat.Invert();
		}

		BoxPairPool::iterator it = m_BoxMap.begin();
		BoxPairPool::iterator eit = m_BoxMap.end();
		CMatrix	TempMat;
		for (;it!=eit;++it)
		{
			CAxisAlignedBox aabb((*it).pAABB->m_vMinimum,(*it).pAABB->m_vMaximum);
			GetLocalMatrix(TempMat,(*it).BoxID,uint(-1));
			TempMat = TempMat * InvMat;
			aabb.transformBox(TempMat);
			m_OrgAABB.mergeBox(aabb);
		}
	}
}

//void CModel::RenderDebugLine(void)
//{
//	if (!ShowBox || m_OrgAABB.isNull())
//		return;
//
//	VerColor3D boxPoints[24];
//	CVector3f center = (m_OrgAABB.getMaximum()+m_OrgAABB.getMinimum())/2;
//	CVector3f extent = (m_OrgAABB.getMaximum()-m_OrgAABB.getMinimum())/2;
//
//	boxPoints[0].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[2].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//
//	boxPoints[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//
//	boxPoints[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*m_CenterMatrix;
//	boxPoints[20].p = (center + CVector3f( extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//	boxPoints[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*m_CenterMatrix;
//
//	boxPoints[0].diffuse = boxPoints[5].diffuse = boxPoints[10].diffuse = boxPoints[15].diffuse = boxPoints[20].diffuse =
//		boxPoints[1].diffuse = boxPoints[6].diffuse = boxPoints[11].diffuse = boxPoints[16].diffuse = boxPoints[21].diffuse =
//		boxPoints[2].diffuse = boxPoints[7].diffuse = boxPoints[12].diffuse = boxPoints[17].diffuse = boxPoints[22].diffuse =
//		boxPoints[3].diffuse = boxPoints[8].diffuse = boxPoints[13].diffuse = boxPoints[18].diffuse = boxPoints[23].diffuse =
//		boxPoints[4].diffuse = boxPoints[9].diffuse = boxPoints[14].diffuse = boxPoints[19].diffuse = 0xff00ff00;
//	CGraphic::GetInst()->DrawPrimitive(PT_LINELIST, boxPoints, 24);
//
//
//	if (m_BoxMap.size()>0)
//	{
//
//		VerColor3D Box[24];
//		CMesh::BoxMap::iterator it = m_BoxMap.begin();
//		CMesh::BoxMap::iterator eit = m_BoxMap.end();
//		CMatrix	TempMat;
//
//		for (int i = 0;it!=eit;++it,++i)
//		{
//			CVector3f center = (it->second.m_vMinimum + it->second.m_vMaximum)/2;
//			CVector3f extent = (it->second.m_vMaximum - it->second.m_vMinimum)/2;
//			GetMatrix(TempMat,it->first,uint(-1));
//			Box[0].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
//			Box[1].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
//			Box[2].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
//			Box[3].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
//			Box[4].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[5].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
//			Box[6].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[7].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
//
//			Box[8].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
//			Box[9].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
//			Box[10].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
//			Box[11].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;
//			Box[12].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[13].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
//			Box[14].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[15].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;
//
//			Box[16].p = (center + CVector3f( extent.x, extent.y, -extent.z ))*TempMat;
//			Box[17].p = (center + CVector3f( -extent.x, extent.y, -extent.z ))*TempMat;
//			Box[18].p = (center + CVector3f( extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[19].p = (center + CVector3f( -extent.x, -extent.y, -extent.z ))*TempMat;
//			Box[20].p = (center + CVector3f( extent.x, extent.y, extent.z ))*TempMat;
//			Box[21].p = (center + CVector3f( -extent.x, extent.y, extent.z ))*TempMat;
//			Box[22].p = (center + CVector3f( extent.x, -extent.y, extent.z ))*TempMat;
//			Box[23].p = (center + CVector3f( -extent.x, -extent.y, extent.z ))*TempMat;
//
//			Box[0].diffuse = Box[5].diffuse = Box[10].diffuse = Box[15].diffuse = Box[20].diffuse =
//				Box[1].diffuse = Box[6].diffuse = Box[11].diffuse = Box[16].diffuse = Box[21].diffuse =
//				Box[2].diffuse = Box[7].diffuse = Box[12].diffuse = Box[17].diffuse = Box[22].diffuse =
//				Box[3].diffuse = Box[8].diffuse = Box[13].diffuse = Box[18].diffuse = Box[23].diffuse =
//				Box[4].diffuse = Box[9].diffuse = Box[14].diffuse = Box[19].diffuse = 0xff0000ff;
//			CGraphic::GetInst()->DrawPrimitive( PT_LINELIST,Box,24);
//		}
//	}
//}
