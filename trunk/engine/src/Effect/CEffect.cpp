#include "stdafx.h"
#include "CEffect.h"
#include "CEffectmanager.h"
#include "CEffectGroup.h"
#include "TimeHelper.h"
#include "ExpHelper.h"
#include <float.h>
#include "CModel.h"
#include "TSqrAllocator.inl"

float CEffect::st_EffectLod = 1.0f;
bool CEffect::st_CameraShake = true;
CEffect::CEffect( CEffectManager* pEffectMgr, CEffectProp* pEffectProp ) 
	: m_pEffectMgr( pEffectMgr )
	, m_CurTime(0xFFFFFFFF)
	, m_fCurTime( 0.0f )
	, m_pModel( NULL )
	, m_PreSkeFrame( NULL )
	, m_IsAniEnd( false )
	//, m_ParentFrame( 0.0f )
	, m_FrameString( NULL )
	, m_bVisible( true )
	, m_pEffectProp(NULL)
{ 
	InitEffect(pEffectProp);
	m_Result.m_IsAniEnd = true;
	AddRef();
}

CEffect::CEffect( CEffectManager* pEffectMgr)
	: m_pEffectMgr( pEffectMgr )
	, m_CurTime(0xFFFFFFFF)
	, m_fCurTime( 0.0f )
	, m_pModel( NULL )
	, m_PreSkeFrame( NULL )
	, m_IsAniEnd( false )
	//, m_ParentFrame( 0.0f )
	, m_FrameString( NULL )
	, m_bVisible( true )
	, m_pEffectProp(NULL)
{
	ResetUnitStartTime(0);
	m_Result.m_IsAniEnd = true;
	AddRef();
	
}

void CEffect::InitEffect(CEffectProp* pEffectProp )
{
	Ast( pEffectProp );
	
	if (pEffectProp != m_pEffectProp)
	{
		m_pEffectProp = pEffectProp;
		m_pEffectProp->AddRef();
	}

	//设置帧同步信息
	for( int i = 0; i < m_pEffectProp->GetTracksNum(); ++i )
	{
		m_Tracks.push_back( EMap< int, EFFECTKEY >() );
		EMap< int, IEffectUnitProp* >::iterator it = m_pEffectProp->GetTrack(i).begin();
		for( ; it != m_pEffectProp->GetTrack(i).end(); ++it )
		{
			EFFECTKEY& DestKey = m_Tracks[i][it->first];
			DestKey.pEffectUnit = m_pEffectMgr->CreateEffectUnit( it->second );

			m_OrgAABB.mergeBox(DestKey.pEffectUnit->GetAABB());
		}
	}

	for( size_t i = 0; i < m_Tracks.size(); ++i )
	{
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
		{
			AttachKey Key = m_pEffectProp->GetAttachKey( it->second.pEffectUnit->GetProp() );
			it->second.pParentEffectUnit = Key.nTrack < 0 ? NULL : m_Tracks[Key.nTrack][Key.nKeyStartTime].pEffectUnit;
		}
	}

	SetRefInfo(this->GetFullEffectName());
}

CEffect::~CEffect(void)
{
	//ClearLink();
	size_t n = m_Tracks.size();
	for( size_t i = 0; i < n; ++i )
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
			SafeRelease( it->second.pEffectUnit );
	SafeRelease( m_pEffectProp );
}


bool CEffect::IsFollowBone(CRenderable* pParentObj)
{
	Ast(NULL != pParentObj);
	CSkeletalFrame* pSkeFrame = NULL;
	m_pModel = ( pParentObj->GetType() == ROT_MODEL ) ? (CBaseModel*)pParentObj : NULL;
	if( m_pModel )
		pSkeFrame = m_pModel->GetSketetalFrame();
	else 
		return false;

	for( int i = 0; i < (int)m_Tracks.size(); ++i )
	{
		EMap< int, EFFECTKEY >& CurTrack = m_Tracks[i];
		for( EMap< int, EFFECTKEY >::iterator it = CurTrack.begin(); it != CurTrack.end(); ++it )
		{
			int nSkeletalID = -1;
			int nSocketID = -1;
			IEffectUnit* pEffectUnit = it->second.pEffectUnit;
			IEffectUnitProp* pEffectUnitProp = pEffectUnit->GetProp();
			eUpdatePosType eType = pEffectUnitProp->GetUpdatePosType();
			if( pSkeFrame )
			{
				if( eType == UP_NODEOFFSET || eType == UP_NODE )
					nSkeletalID = pSkeFrame->GetSkeletalIDByName( pEffectUnitProp->GetAttachName() );
				else if( eType == UP_SOCKETOFFSET || eType == UP_SOCKET )
					pSkeFrame->GetIDBySocketName( nSkeletalID, nSocketID, pEffectUnitProp->GetAttachName() );
			}
			if( -1 != nSkeletalID )
				return true;
		}
	}
	return false;
}

const char* CEffect::GetEffectName()	
{ 
	return m_pEffectProp->GetEffectName(); 
}

const char* CEffect::GetFullEffectName()	
{ 
	return m_pEffectProp->GetFullEffectName();
}

int CEffect::GetAttachAniNum()
{
	return m_pEffectProp->GetAttachAniNum();
}

const char* CEffect::GetAttachAniName( int Num )
{
	return m_pEffectProp->GetAttachAniName( Num );
}

const char* CEffect::GetUserDesc()
{
	return m_pEffectProp->GetUserDesc();
}

int CEffect::GetTimeRange()
{
	return m_pEffectProp->GetTimeRange();
}

void CEffect::SetRenderCallback( IEffectUnitHandler* pUnitHandler, const char* szUnitName )
{
	for( size_t i = 0; i < m_Tracks.size(); ++i )
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
			if( !_stricmp( it->second.pEffectUnit->GetProp()->GetCustomName(), szUnitName ) )
				return it->second.pEffectUnit->SetRenderCallback( pUnitHandler );
}

void CEffect::SetRenderCallback( IEffectUnitHandler* pUnitHandler, EBaseEffectUnit EUnit )
{
	for( size_t i = 0; i < m_Tracks.size(); ++i )
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
			if( it->second.pEffectUnit->GetEffectUniType() == EUnit )
				it->second.pEffectUnit->SetRenderCallback( pUnitHandler );
}

//const CVector3f& CEffect::GetBoxScale()
//{
//	return m_vecScale;
//}
//
//const CVector3f& CEffect::GetBoxOff()
//{
//	return m_vecOff;
//}

//void CEffect::ClearLink(void)
//{
//	for( EMap< EString, CRenderable* >::iterator it = m_CenterLink.begin(); it != m_CenterLink.end(); ++it )
//	{
//		it->second->OnUnlinked( this, it->first.c_str() );
//		SafeRelease( it->second );
//	}
//	m_CenterLink.clear();
//}
//
//int CEffect::AddLinkObj( CRenderable* MObj, const char* szAttachName, bool bFrameSynch )
//{
//	if( !MObj )
//		return -1;
//	m_pModel = ( MObj->GetType() == ROT_MODEL ) ? (CBaseModel*)MObj : NULL;
//	EMap< EString, CRenderable* >::iterator it = m_CenterLink.find( szAttachName );
//	if( it != m_CenterLink.end() )
//	{
//		it->second->OnUnlinked( this, szAttachName );
//		SafeRelease( it->second );
//		it->second = MObj;
//	}
//	else
//		m_CenterLink[szAttachName] = MObj;
//	MObj->AddRef();
//	MObj->OnLinked( this, szAttachName, bFrameSynch );
//	m_EffectName = szAttachName;
//	return -1;
//}
//
//void CEffect::DeleteLinkObj( const char* szAttachName )
//{
//	EMap< EString, CRenderable* >::iterator it = m_CenterLink.find( szAttachName );
//	if( it != m_CenterLink.end() )
//	{
//		it->second->OnUnlinked( this, szAttachName );
//		SafeRelease( it->second );
//		m_CenterLink.erase( it );
//	}
//}
//
//int CEffect::RefLinkObj(  const char* szAttachName, CRenderable** ppRenderObj )
//{
//	*ppRenderObj = NULL;
//	EMap< EString, CRenderable* >::iterator it = m_CenterLink.find( szAttachName );
//	if( it != m_CenterLink.end() )
//	{
//		*ppRenderObj = it->second;
//		(*ppRenderObj)->AddRef();
//		return 0;
//	}
//
//	return GERROR;
//}

void CEffect::OnLinked( CRenderable* pParentObj )
{
	//m_EffectName=szAttachName;
	CBaseModel* pPreModel = m_pModel;
	m_pModel = ( pParentObj->GetType() == ROT_MODEL ) ? (CBaseModel*)pParentObj : NULL;

	CSkeletalFrame* pSkeFrame = NULL;
	if( m_pModel )
		pSkeFrame = m_pModel->GetSketetalFrame();
	if( m_PreSkeFrame == pSkeFrame && pPreModel == m_pModel )
		return;
	m_PreSkeFrame = pSkeFrame;

	for( int i = 0; i < (int)m_Tracks.size(); ++i )
	{
		EMap< int, EFFECTKEY >& CurTrack = m_Tracks[i];
		for( EMap< int, EFFECTKEY >::iterator it = CurTrack.begin(); it != CurTrack.end(); ++it )
		{
			IEffectUnit* pEffectUnit = it->second.pEffectUnit;
			pEffectUnit->OnLinked( pParentObj );
			if(NULL==it->second.pParentEffectUnit)
			{
				int nSkeletalID = -1;
				int nSocketID = -1;

				IEffectUnitProp* pEffectUnitProp = pEffectUnit->GetProp();
				eUpdatePosType eType = pEffectUnitProp->GetUpdatePosType();
				
				if( pSkeFrame )
				{
					if( eType == UP_NODEOFFSET || eType == UP_NODE )
						nSkeletalID = pSkeFrame->GetSkeletalIDByName( pEffectUnitProp->GetAttachName() );
					else if( eType == UP_SOCKETOFFSET || eType == UP_SOCKET )
						pSkeFrame->GetIDBySocketName( nSkeletalID, nSocketID, pEffectUnitProp->GetAttachName() );
				}

				pEffectUnit->SetBindID((int16)nSkeletalID,(uint8)nSocketID);
			}

		}
	}
}

void CEffect::OnUnlinked( void )
{
	m_pModel = NULL;
	m_PreSkeFrame = NULL;
	for( int i = 0; i < (int)m_Tracks.size(); ++i )
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
			if( it->second.pEffectUnit )
				it->second.pEffectUnit->OnUnlinked();
}

//void CEffect::OnPreRender( const AniResult& ParentAniRes, float FrameRatio )
//{
//	if( m_IsAniEnd && m_pEffectProp->GetFrameSynch() )
//		m_fCurTime = ParentAniRes.m_CurFrame * FrameRatio;
//
//	m_IsAniEnd		|= ParentAniRes.m_IsAniEnd || ParentAniRes.m_IsReset;
//	m_InAniBlend	= ParentAniRes.m_InAniBlend;
//	m_ParentFrame	= ParentAniRes.m_CurFrame;
//	m_FrameString	= ParentAniRes.m_FrameString;
//	m_FrameRatio	= FrameRatio;
//	m_InvFrameRatio	= 1.0f/FrameRatio;
//}

void CEffect::GetCurAnimationInfo( float& CurFrame, uint32& CurTime )
{
	CurFrame = 0;
	CurTime = m_CurTime;
}

//bool CEffect::SetNextAnimation( IDNAME AniName, FramePair FmPair, bool bLoop, int32 DelayTime/*毫秒*/, float AniSpeed )
//{
//	m_AniStartTime		= uint32(GetProcessTime()) + DelayTime;
//	m_CurAnimation		= (const TCHAR*)AniName;
//
//	if( m_bFrameSynch && abs( FmPair.nStart - m_fCurTime * m_InvFrameRatio ) > 3 )
//		m_IsAniEnd = true;
//	return true;
//}

//void CEffect::ResetVelocityUnit(uint32 uCurTime)
//{
//	for( size_t i = 0; i < m_Tracks.size(); ++i )
//		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
//		{
//			IEffectUnitProp* pProp = it->second.pEffectUnit->GetProp();
//			if( pProp->IsUseVelocity() )
//				it->second.pEffectUnit->SetStartTime( uCurTime , it->first );
//		}
//}
//
//void CEffect::ResetNormalUnit(uint32 uCurTime)
//{
//	for( size_t i = 0; i < m_Tracks.size(); ++i )
//		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
//			if( !it->second.pEffectUnit->GetProp()->IsUseVelocity() )
//				it->second.pEffectUnit->SetStartTime( uCurTime , it->first );
//}

void CEffect::ResetUnitStartTime(uint32 uCurTime)
{
	for( size_t i = 0; i < m_Tracks.size(); ++i )
		for( EMap< int, EFFECTKEY >::iterator it = m_Tracks[i].begin(); it != m_Tracks[i].end(); ++it )
			it->second.pEffectUnit->SetStartTime( uCurTime , it->first );
}

bool CEffect::UpdateEfx(uint32 uCurTime, bool bReset)
{
	if(m_Result.m_IsAniEnd)
	{
		ResetUnitStartTime(0);
		m_Result.m_IsAniEnd = false;
	}

	if( m_CurTime != uCurTime )
	{
		if( uCurTime < m_CurTime )
		{
			m_CurTime = uCurTime;
		}

		float nRange = float(m_pEffectProp->GetTimeRange());
		if( m_pEffectProp->GetFrameSynch() )
		{
			//这里用来控制融合。如果动作处在融合阶段。那么特效不播放
			if(m_fCurTime>uCurTime)
			{
				ResetUnitStartTime(0);
				m_Result.m_IsAniEnd = true;
			}
			m_fCurTime = float(uCurTime);
		}
		else
		{
			m_fCurTime	+= uCurTime - m_CurTime;// * m_InvFrameRatio;
			if( m_fCurTime > nRange )
			{
				if(bReset)
				{
					ResetUnitStartTime(0);
					m_fCurTime = 0;//fmod(m_fCurTime,nRange);
				}
				else
					m_fCurTime = nRange;
				m_Result.m_IsAniEnd = true;	
			}
		}
	}

	m_CurTime = uCurTime;
	return m_Result.m_IsAniEnd;
}

bool CEffect::UpdateTime(uint32 uCurTime)
{
	return UpdateEfx(uCurTime,true);
}

const AniResult CEffect::Render( const CMatrix& matWorld,  RenderObjStyle* pRORS )
{
	SQR_TRY	
	{
 		CMatrix tMatWorld = matWorld;
		
		if(m_pEffectProp->GetForbidScale())
		{
			tMatWorld.SetWithRow( matWorld.GetAxis(0) / tMatWorld.GetAxis(0).Mag(),
								matWorld.GetAxis(1) / tMatWorld.GetAxis(1).Mag(), 
								matWorld.GetAxis(2) /  tMatWorld.GetAxis(2).Mag(),
								matWorld.GetAxis(3) );
		}

		for( size_t i = 0; i < m_Tracks.size(); ++i )
		{
			EMap< int, EFFECTKEY >& CurTrack = m_Tracks[i];
			for( EMap< int, EFFECTKEY >::iterator it = CurTrack.begin(); it != CurTrack.end(); ++it )
			{
				IEffectUnit* pEffectUnitRender = it->second.pEffectUnit;
				IEffectUnitProp* pProp = pEffectUnitRender->GetProp();

				if(pProp->GetEffectLod() <st_EffectLod)
				{	
					//pEffectUnitRender->SetVisible( m_bVisible );
					SetUnitVisibleOnRender(i,it->first);
					if( NULL == it->second.pParentEffectUnit )
					{
						if( NULL == m_pModel || pEffectUnitRender->GetBindSkeletalId() < 0 )
						{							
							pEffectUnitRender->Render( tMatWorld,(uint32)m_fCurTime, pRORS );
						}
						else
						{
							CMatrix World;
							m_pModel->GetMatrix( World, pEffectUnitRender->GetBindSkeletalId(), pEffectUnitRender->GetBindSocketId() );
							if( pEffectUnitRender->GetProp()->GetUpdatePosType() == UP_NODEOFFSET ||
								pEffectUnitRender->GetProp()->GetUpdatePosType() == UP_SOCKETOFFSET )
							{
								if(m_pEffectProp->GetForbidScale())
								{
									World._11 = World._22 = World._33 = 1.0f;
								}
								else
								{
									World._11 = matWorld.GetAxis(0).Mag();
									World._22 = matWorld.GetAxis(1).Mag();
									World._33 = matWorld.GetAxis(2).Mag();
								}
								World._44 = 1.0f;
								World._12 = World._13 = World._14 = 
									World._21 = World._23 = World._24 = 
									World._31 = World._32 = World._34 = 0.0f;
							}
							else
							{
								if(m_pEffectProp->GetForbidScale())
								{
									World.SetWithRow( World.GetAxis(0) / matWorld.GetAxis(0).Mag(),
													World.GetAxis(1) / matWorld.GetAxis(1).Mag(), 
													World.GetAxis(2) /  matWorld.GetAxis(2).Mag(),
													World.GetAxis(3) );
								}
							}
							pEffectUnitRender->Render( World,(uint32)m_fCurTime, pRORS );
						}	
					}
					else
					{		
						it->second.pParentEffectUnit->RenderChild(pEffectUnitRender,(uint32)m_fCurTime, pRORS);
					}
				}
				else
					pEffectUnitRender->SetVisible( false );
			}
		}

		return m_Result;
	}
	SQR_CATCH(exp)
	{
		string strFullEffectName = this->GetFullEffectName();

		exp.AppendMsg("特效:");
		exp.AppendMsg(strFullEffectName.c_str());
		exp.AppendMsg("出问题");

		GfkLogExp(exp);
	}
	SQR_TRY_END;

	m_Result.m_IsAniEnd = true;
	return m_Result;
}

void	CEffect::SetUnitVisibleOnRender(int track,int key)
{
	IEffectUnit* pEffectUnitRender = m_Tracks[track][key].pEffectUnit;
	pEffectUnitRender->SetVisible(m_bVisible);
}

//------------------------------------------------------------------------------
ERenderObjType
CEffect::GetType()
{
	return ROT_EFFECT;
}

//------------------------------------------------------------------------------
bool
CEffect::IsValid()
{
	return true;
}

//------------------------------------------------------------------------------
void
CEffect::SetVisible( bool isVisible )
{
	m_bVisible = isVisible;
	for( size_t i = 0; i < m_Tracks.size(); ++i )
	{
		EMap< int, EFFECTKEY >& CurTrack = m_Tracks[i];
		for( EMap< int, EFFECTKEY >::iterator it = CurTrack.begin(); it != CurTrack.end(); ++it )
			it->second.pEffectUnit->SetVisible( m_bVisible );
	}
}

//------------------------------------------------------------------------------
const float CEffect::GetAABBHeight()
{
	return m_OrgAABB.getMaximum().y;
}

bool CEffect::IsFrameSynch(void)
{
	return m_pEffectProp->GetFrameSynch();
}

//------------------------------------------------------------------------------
CEffect::EFFECTKEY::EFFECTKEY()
: pEffectUnit(NULL)
, pParentEffectUnit(NULL)
{

}
