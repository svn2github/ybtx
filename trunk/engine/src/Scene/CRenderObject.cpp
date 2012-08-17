#include "stdafx.h"
#include "IEffectMgrModule.h"
#include "CRenderObject.h"
#include "IEffectGroup.h"
#include "IEffect.h"
#include "IEffectManager.h"
#include "IEffectUnit.h"
#include "CRenderRegion.h"
#include "CRenderScene.h"
#include "TPart.inl"
#include "BaseHelper.h"
#include "CCamera.h"
#include "CRenderObjectAsynFun.h"
#include "CRenderDialog.h"
#include "CGraphicExp.h"
#include "CSkeletalsMgr.h"
#include "CEffectPlayer.h"
#include "TSqrAllocator.inl"
#include "IModelHandler.h"
#include "CAnimation.h"
#include "CEffectPlayer.h"
#include "TIDPtrMapIter.h"
#include "TIDPtrMap.inl"
#include "TIDPtrMapIter.inl"

#define WAIT_RANGE_TIME 100
#define DEL_ID -2
const DWORD CRenderObject::st_RenderObjectType = 'REOB';

//void NoneActEffect::ClearInfo()
//{
//	if( pInfo )
//	{
//	/*	mapFxHandler::iterator it = pInfo->mapHandler.begin();
//		for( ;it != pInfo->mapHandler.end(); it++ )
//			SafeRelease( it->second );*/
//		SafeRelease( pInfo->pEffectGroup );
//		GfkSafeDelete( pInfo );
//	}
//}




CRenderObject::CRenderObject(IRenderTime* pTime,ILogicHandler* pHandler)
: CModel(pHandler)
, m_uDesDirection(0)
, m_ActivePart(-1)
, m_uAlphaDes( 0xff )
, m_rAmbient( 0x80 )
, m_gAmbient( 0x80 )
, m_bAmbient( 0x80 )
, m_pWaterFx( NULL )
, m_pPortraitInfo(NULL)
, m_PortraitClassName("body")
, m_uAlpha(255)
, m_uCurDirection(0)
, m_rDiffuse( 0x80 )
, m_gDiffuse( 0x80 )
, m_bDiffuse( 0x80 )
, m_EffectVelocity( 1.0f )
, m_bTerrainheight( false )
, m_RenderStyle(ROS_ZFAILD_SHADOW_LIGHT2)
, m_IsAsynLoadPiece( true )
, m_fFinalScale(1.0f)
, m_uFinalTime(0)
, m_bScaleIsFinal(false)
, m_uLastFrameTime(0)
, m_uLastKeyFrame(0xffff)
, m_uCurTime(0)
, m_nMaterialIndex(-1)
, m_pTime(pTime)
, m_LapseTime(0)
, m_bCanBeSelect(true)
{
	m_IsVailedShow = true;
	if(m_pTime)
		m_pTime->AddRef();
}

CRenderObject::~CRenderObject(void)
{
	SafeRelease(m_pTime);
	Release();
}

void CRenderObject::Release()
{
	SetModelHandler(NULL);
	GfkSafeDelete( m_pWaterFx );
	ClearMeshPiece();
	SetVisible(false);

	for( SList<ResCreateInfo>::iterator it = m_ListResInCreate.begin(); it != m_ListResInCreate.end(); ++it )
		SafeRelease( it->pRes );
	m_ListResInCreate.clear();
	m_PartAni.clear();
}

void CRenderObject::SetRenderStyle( ERenderObjStyle Style )
{ m_RenderStyle = Style; }

ERenderObjStyle CRenderObject::GetRenderStyle() const
{
	return m_RenderStyle;
}

void CRenderObject::SetAsynLoadPieceSwitch(bool bEnable)
{
	m_IsAsynLoadPiece = bEnable;
}
//***************************************************************
// 功能: 供类外部调用重新创建渲染对象
// 参数: 等同于ResOnCreate结构
// 返回: true 创建成功, false 创建失败
//***************************************************************
bool CRenderObject::AddPiece( RES_TYPE eResType, const char* szFileName, const char* szClassName, const char* szParam )
{
	CGraphic* pGraphic = CGraphic::GetInst();
	IEffectManager* pEffectMgr = IEffectManager::GetInst();
	ResCreateInfo Info;
	Info.eResType = eResType;
	Info.szFileName = szFileName?szFileName:"";
	Info.szClassName = szClassName ? szClassName : "";
	Info.szParam = szParam ? szParam : "";
	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead( m_IsAsynLoadPiece );

	SQR_TRY
	{
		switch(eResType)
		{
		case RFT_ARP:
			{
				if( FAILED( pGraphic->CreatePieceGroup( szFileName, (CPieceGroup**)&Info.pRes ) ) )
				{
					CPkgFile::EnableAsyncRead( isAsyc );
					ostringstream strm;
					strm<<"CreatePieceGroup failed with file \""<<szFileName<<"\"";
					ArtErr(strm.str());
					return false;
				}
				CPkgFile::EnableAsyncRead( isAsyc );

				if( Info.pRes->IsValid() )
				{
					IntAddPiece( Info );
				}
				else
				{
					m_ListResInCreate.push_back( Info );
				}
			}
			break;
		case RFT_ARA:
			{
				CSkeletalFrame* pSkeFrame = NULL;
				if( FAILED( CSkeletalsMgr::GetInst()->GetSkeInPool( szFileName, &pSkeFrame ) ) )
				{
					SString sAgpFile = szFileName;
					sAgpFile = sAgpFile.substr(0,sAgpFile.size()-4)+".agp";
					
					if( FAILED( pGraphic->CreateAnimationGroup( sAgpFile.c_str(), (CAnimationGroup**)&Info.pRes ) ) )
					{
						CPkgFile::EnableAsyncRead( isAsyc );
						ostringstream strm;
						strm<<"CreateAnimationGroup failed with file \""<<sAgpFile.c_str()<<"\"";
						ArtErr(strm.str());
						return false;
					}
					CPkgFile::EnableAsyncRead( isAsyc );

					if( Info.pRes->IsValid() )
					{
						pSkeFrame = ((CAnimationGroup*)Info.pRes)->GetSkeletalFrame();
						SetSkeletalFrame( pSkeFrame );
						for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
							_UpdateAttachChild(*it);

						SetAnimateGroup( (CAnimationGroup*)Info.pRes );
						m_PartAni.clear();
						SafeRelease( Info.pRes );
					}
					else
					{
						m_ListResInCreate.push_back( Info );
					}

					return false;
				}
				else
				{
					CPkgFile::EnableAsyncRead( isAsyc );
					SetSkeletalFrame( pSkeFrame );

					for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
						_UpdateAttachChild(*it);

					//AGP
					SString sAgpFile = szFileName;
					sAgpFile = sAgpFile.substr(0,sAgpFile.size()-4)+".agp";

					//CPkgFile::EnableAsyncRead(false);
					if( FAILED( pGraphic->CreateAnimationGroup( sAgpFile.c_str(), (CAnimationGroup**)&Info.pRes ) ) )
					{
						ostringstream strm;
						strm<<"CreateAnimationGroup failed with file \""<<sAgpFile.c_str()<<"\"";
						ArtErr(strm.str());
						return false;
					}
					//CPkgFile::EnableAsyncRead(true);

					if( Info.pRes->IsValid() )
					{
						SetAnimateGroup( (CAnimationGroup*)Info.pRes );
						m_PartAni.clear();
						SafeRelease( Info.pRes );
					}
					else
					{
						Info.eResType = RFT_AGP;
						m_ListResInCreate.push_back( Info );
					}
				}				
			}
			break;
		//case RFT_ARE:
		//	{
		//		if( FAILED( pEffectMgr->CreateEffectGroup( szFileName, (IEffectGroup**)&Info.pRes ) ) )
		//		{
		//			CPkgFile::EnableAsyncRead( isAsyc );
		//			ostringstream strm;
		//			strm<<"CreateEffectGroup failed with file \""<<szFileName<<"\"";
		//			ArtErr(strm.str());
		//			return false;
		//		}
		//		CPkgFile::EnableAsyncRead( isAsyc );

		//		if( Info.pRes->IsValid() )
		//		{
		//			CreateBaseEffect( Info );
		//		}
		//		else
		//		{
		//			m_ListResInCreate.push_back( Info );
		//		}
		//	}
		//	break;
		case RFT_AGP:
			{
				if( FAILED( pGraphic->CreateAnimationGroup( szFileName, (CAnimationGroup**)&Info.pRes ) ) )
				{
					CPkgFile::EnableAsyncRead( isAsyc );
					ostringstream strm;
					strm<<"CreateAnimationGroup failed with file \""<<szFileName<<"\"";
					ArtErr(strm.str());
					return false;
				}
				CPkgFile::EnableAsyncRead( isAsyc );

				if( Info.pRes->IsValid() )
				{
					SetAnimateGroup( (CAnimationGroup*)Info.pRes );
					m_PartAni.clear();
					SafeRelease( Info.pRes );
				}
				else
				{
					m_ListResInCreate.push_back( Info );
				}
			}
			break;
		case RFT_SKE:
			{
				CSkeletalFrame* pSkeFrame = NULL;
				if( FAILED( CSkeletalsMgr::GetInst()->GetSkeInPool( szFileName, &pSkeFrame ) ) )
				{
					CPkgFile::EnableAsyncRead( isAsyc );
					ostringstream strm;
					strm<<"GetSkeInPool failed with file \""<<szFileName<<"\"";
					ArtErr(strm.str());
					return false;
				}
				CPkgFile::EnableAsyncRead( isAsyc );

				SetSkeletalFrame( pSkeFrame );

				for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
					_UpdateAttachChild(*it);
			}
			break;
		default:
			break;
		}


		CPkgFile::EnableAsyncRead( isAsyc );
		if( IsValid() )
		{
			if(m_pRenderAniInst->m_pHandler)
				m_pRenderAniInst->m_pHandler->OnResLoaded();
			Update();
		}
		return true;
	}
	SQR_CATCH(exp)
	{
		GfkLogExp(exp);
	}
	SQR_TRY_END;


	CPkgFile::EnableAsyncRead( isAsyc );
	return false;
}

//////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
void
CRenderObject::SetPortraitPiece( const char* className )
{
	m_PortraitClassName = className;
}
//------------------------------------------------------------------------------
PortraitInfo*
CRenderObject::GetPortraitInfo() const
{
	return m_pPortraitInfo;
}

//********************************************************************
// 功能: 增加模型
// 参数: 略
// 返回: true 创建成功, false 创建失败
//**********************************************************************
void CRenderObject::IntAddPiece( ResCreateInfo& Info )
{
	CGraphic* pGraphic = CGraphic::GetInst();
	CPieceGroup* pPieceGroup = (CPieceGroup*)Info.pRes;

	uint32 rsNum=0;

	rsNum = GetID( pPieceGroup, Info.szClassName.c_str(),Info.szParam.c_str());

	if( !Info.szClassName.empty() )
	{
		DeleteMeshPiece( Info.szClassName.c_str() );
		AddMeshPiece( pPieceGroup, Info.szClassName.c_str(), rsNum );
		if (Info.szClassName == this->m_PortraitClassName)
			this->m_pPortraitInfo = pPieceGroup->GetPortraitInfo();
	}
	else
	{
		for( size_t i = 0; i < pPieceGroup->GetPieceClassNum(); i++ )
		{
			const char* szClassName = pPieceGroup->GetPieceClassName(i);
			DeleteMeshPiece( szClassName );
			AddMeshPiece( pPieceGroup, szClassName, rsNum );
		}
	}

	SafeRelease( Info.pRes );
}

//移除渲染对象的主体部件
bool CRenderObject::RemovePiece( const char* szClassName )
{
	for( SList<ResCreateInfo>::iterator it = m_ListResInCreate.begin(); it != m_ListResInCreate.end(); )
	{
		if( (*it).eResType == RFT_AGP && (szClassName == NULL || (*it).szClassName == szClassName) )
		{
			GraphicRes* pRes = (*it).pRes;
			m_ListResInCreate.erase( it++ );
			SafeRelease( pRes );
		}
		else
			++it;
	}

	m_AsynParams.Delete(FunShowPiece::stGetType(), szClassName);

	if (szClassName && szClassName == m_PortraitClassName)
		m_pPortraitInfo = NULL;

	DeleteMeshPiece( szClassName );


	if (szClassName==NULL)
		ClearAllEfx();
	return true;
}

//显示或隐藏部件
//void CRenderObject::ShowPieceClass( const char* szClassName, bool bShow )
//{
//	if( GetType() != ROT_MODEL )
//		return;
//	CModelEx* pModel = (CModelEx*)m_pRenderObj;
//	pModel->ShowPiece( szClassName, bShow );
//}
//
//void CRenderObject::ShowPieceClassBuffer(const char* szClassName,bool bShow)
//{
//	if( m_pRenderObj && GetType() != ROT_MODEL )
//		return;
//
//	CModelEx* pModel = NULL;
//	pModel = (CModelEx*)m_pRenderObj;
//	if( pModel == NULL )
//		return;
//
//	if(pModel->IsMeshsPieceEmpty())
//		pModel->ShowPieceBuffer( szClassName, bShow );
//	else
//		pModel->ShowPiece( szClassName, bShow );
//}

void CRenderObject::ShowPiece(const char* szPieceName,bool bShow)
{
	if(!IsValid())
	{
		m_AsynParams.Insert( new FunShowPiece(this,szPieceName,bShow) );
		return;
	}
	CModel::ShowPiece(szPieceName,bShow);
}

//创建基础特效，指动作和场景特效
//void CRenderObject::CreateBaseEffect( ResCreateInfo& Info )
//{
//	IEffectManager* pEffectMgr = IEffectManager::GetInst();
//	IEffectGroup* pEffectGroup = (IEffectGroup*)Info.pRes;
//	SVector<IEffect*> pEffect;
//	SVector<IEffectGroup*> EffectGroups;
//
//	if( Info.szParam.empty() )
//	{
//		Ast( Info.szClassName.empty() );
//		pEffect.resize( pEffectGroup->GetEffectNum() );
//		//对于加入的某一个特效
//		for( uint32 uEffect = 0; uEffect < pEffectGroup->GetEffectNum(); uEffect++ )
//			pEffectMgr->CreateEffect( pEffectGroup, pEffectGroup->GetEffectName(uEffect), &pEffect[uEffect] );
//	}
//	else
//	{
//		pEffect.resize( 1 );
//		pEffectMgr->CreateEffect( pEffectGroup, Info.szParam.c_str(), &pEffect[0] );
//	}
//
//	//如果不是作为部件，则按动作将特效规类
//	if( Info.szClassName.empty() )
//	{
//		for( uint32 uEffect = 0; uEffect < pEffect.size(); uEffect++ )
//		{
//			//得到特效名
//			SVector<const char*> szAniName;
//			if(pEffect[uEffect] == NULL)
//				continue;
//
//			//const char* szEffectName = pEffect[uEffect]->GetEffectName();
//			//int AttachAniNum = pEffect[uEffect]->GetAttachAniNum();
//			////按动作将特效规类
//			//for( int i = 0; i < AttachAniNum; i++ )
//			//{
//			//	const char* szName = pEffect[uEffect]->GetAttachAniName( i );
//			//	if( szName && szName[0] )
//			//		szAniName.push_back( szName );
//			//}
//			//if( szAniName.empty() )
//			//	szAniName.push_back( szEffectName );
//
//			//for( size_t i = 0; i < szAniName.size(); i++ )
//			//{
//			//	//SafeRelease( m_Effects[szAniName[i]] );
//			//	//m_Effects[szAniName[i]] = pEffect[uEffect];
//			//	//增加引用计数，相当于连续创建了多个特效
//			//	//AddLinkObj( pEffect[uEffect], Info.szParam.c_str(), true );		
//			//	//pEffect[uEffect]->AddRef();
//			//}
//			
//			AddEffect(pEffect[uEffect]);
//			m_OrgAABB.mergeBox(pEffect[uEffect]->GetAABB());
//			SafeRelease( pEffect[uEffect] );
//		}
//	}
//	//如果是作为部件，连接到模型上
//	else
//	{
//		AddEffect(pEffect[0]);
//		m_OrgAABB.mergeBox(pEffect[0]->GetAABB());
//		SafeRelease( pEffect[0] );
//	}
//
//	SafeRelease( pEffectGroup );
//}




//删除不被动作同步的特效
//void CRenderObject::DeleteEffect( uint32 nMask )
//{
	/*if( !IsValid() )
	{
		m_AsynParams.Insert( new FunDeleteNoneActEffect(this,nMask) );
		return;
	}*/
//}

//设定特效单元回调
//void CRenderObject::SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler, const char* szUnitName )
//{
//	//if( !IsValid() )
//	//{
//	//	m_AsynParams.Insert(new SetEffectHandlerName(this,nIndex,pUnitHandler,szUnitName));
//	//	return;
//	//}
//
//	SMap< uint32, NoneActEffect >::iterator it = m_nNoneActEffectFlag.find( nIndex );
//	if( it == m_nNoneActEffectFlag.end() )
//		return;
//	
//	if( it->second.pInfo != NULL && !it->second.pInfo->pEffectGroup->IsValid() )
//	{
//		
//		return;
//	}
//
//	szUnitName = szUnitName ? szUnitName : "";
//	CRenderable* pRenderObj = NULL;
//	//EndDuke fix
//	//if( nIndex < MAX_ADD_EFFECT && SUCCEEDED( RefLinkObj( g_ActIndependenceEffectLinkName[nIndex], &pRenderObj ) ) )
//	//		pRenderObj->Release();
//
//	if( pRenderObj && pRenderObj->GetType() == ROT_EFFECT )
//		((IEffect*)pRenderObj)->SetRenderCallback( pUnitHandler, szUnitName );
//}
//
////设定特效单元回调
//void CRenderObject::SetEffectUnitHandler( uint32 nIndex, IEffectUnitHandler* pUnitHandler,const char* szUnitName,uint32 eEffectUnitType)
//{
//	SMap< uint32, NoneActEffect >::iterator it = m_nNoneActEffectFlag.find( nIndex );
//	if( it == m_nNoneActEffectFlag.end() )
//		return;
//
//	if( it->second.pInfo && !it->second.pInfo->pEffectGroup->IsValid() )
//	{
//		m_AsynParams.Insert(new SetEffectHandlerType(this,nIndex,pUnitHandler,szUnitName,eEffectUnitType));
//		return;
//	}
//
//	szUnitName = szUnitName ? szUnitName : "";
//	CRenderable* pRenderObj = NULL;
//	//EndDuke fix
//	//if( nIndex < MAX_ADD_EFFECT &&
//	//	SUCCEEDED( RefLinkObj( g_ActIndependenceEffectLinkName[nIndex], &pRenderObj ) ) )
//	//	pRenderObj->Release();
//
//	if( pRenderObj && pRenderObj->GetType() == ROT_EFFECT )
//		((IEffect*)pRenderObj)->SetRenderCallback( pUnitHandler, (EBaseEffectUnit)eEffectUnitType);
//}

int32 CRenderObject::GetAniMaxFrame( const char* Action )
{
	if( !IsValid() )
		return 0;

	CAnimationGroup* pAni = GetAnimateGroup();
	if( !pAni )
		return 0;
	return pAni->GetAnimationMaxFrame( pAni->GetAnimationNum( Action ) );	
}

float CRenderObject::GetGroundSpeed( const char* Action )
{
	if( !IsValid() )
		return 0;

	CAnimationGroup* pAni = GetAnimateGroup();
	if( !pAni )
		return 0;
	return pAni->GetGroundSpeed(pAni->GetAnimationNum( Action ));
}
int32 CRenderObject::GetFrameCountToFrameStr( const char* Action, const char* szFrameStr, int nFrameStart, int nFrameEnd, int nStr )
{
	if( !IsValid() )
		return -1;

	CAnimationGroup* pAni = GetAnimateGroup();
	if( !pAni )
		return -1;
	return pAni->GetFrameCountToFrameStr( pAni->GetAnimationNum( Action ), szFrameStr, nFrameStart, nFrameEnd, nStr );

}

//void CRenderObject::DoAction( const char* Action, int32 partid, uint8 PlayMode /* = 4 */, bool bLoop /* = true */, uint32 DelayTime /* = 200 */, int16 nStartFrame /* = 0 */, int16 nEndFrame /* = -1 */ )
//{
// if( 4 == PlayMode )
// Do( Action, bLoop, DelayTime, nStartFrame, nEndFrame );
// else
// DoSep( Action, partid, PlayMode, DelayTime );
//}

bool CRenderObject::DoAni( const char* szAniFileName, bool bLoop, int16 nStartFrame,float AniSpeed )
{
	return DoAni( szAniFileName, -1, bLoop, 100, nStartFrame, -1, AniSpeed );
}

bool CRenderObject::DoAni( const char* szAniFileName, bool bLoop,float AniSpeed )
{
	return DoAni( szAniFileName, -1, bLoop, 100, 0, -1, AniSpeed );
}

bool CRenderObject::DoAni(const char* szAniFileName,int32 nPartId,bool bLoop, float AniSpeed )
{
	return DoAni( szAniFileName, nPartId, bLoop, 100, 0, -1, AniSpeed );
}

bool CRenderObject::DoAni(const char* szAniFileName,int32 nPartId,bool bLoop,uint32 uDelayTime,int16 nStartFrame,int16 nEndFrame, float AniSpeed )
{
	if (strcmp(szAniFileName, "") == 0)
		return false;
	
	if(!IsValid())
	{
		m_AsynParams.Insert( new FunDoAni(this,szAniFileName,nPartId,bLoop,uDelayTime,nStartFrame,nEndFrame,AniSpeed) );
		_ClearEffect(nPartId);
		return true;
	}

	DoAniIsValidResult AniResult;
	if(!GetSketetalFrame() || GetMaxUseSkeletal() == 0 || (AniResult = m_pAnimationGroup->IsAniValid(szAniFileName)) == eAni_NoExist)
	{
		return false;
	}

	m_AsynParams.Delete(FunDoAni::stGetType(), &nPartId);
	if(AniResult == eAni_InValid)
	{
		m_IsVailedShow = false;
		m_AsynParams.Insert( new FunDoAni(this,szAniFileName,nPartId,bLoop,uDelayTime,nStartFrame,nEndFrame,AniSpeed) );
		_ClearEffect(nPartId);
		return true;
	}

	m_IsVailedShow = true;
	bool DoResult = false;
	PartAniMap::iterator it = m_PartAni.find(nPartId);
	if(it == m_PartAni.end() || it->second.m_PreAction != szAniFileName || it->second.m_nFrameStart != nStartFrame || it->second.m_nFrameEnd != nEndFrame || !bLoop )
	{
		m_ActivePart = nPartId;
		if( m_ActivePart == -1 )
		{
			DoResult = SetNextAnimation( szAniFileName, FramePair( 0, -1 ), bLoop,uDelayTime,AniSpeed );

			m_FootStepFrameList.clear();
			GMap<WORD, GString> footstep = m_CurAniFootStepFrames;
			GMap<WORD, GString>::iterator beg = footstep.begin();
			GMap<WORD, GString>::iterator end = footstep.end();
			WORD wFrameKey(0);
			for ( beg; beg != end; ++beg )
			{
				wFrameKey    = beg->first;
				m_FootStepFrameList[wFrameKey]  = false;
			}


			for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
				(*it)->Do( szAniFileName, bLoop, uDelayTime, nStartFrame, nEndFrame );
		}
		else
		{
			DoResult = SetAniSep( IDNAME(szAniFileName), nPartId, bLoop, uDelayTime,AniSpeed );
		}

		_ClearEffect(nPartId);
		AniParam& tParam = m_PartAni[nPartId];
		tParam.m_PreAction = szAniFileName;
		tParam.m_nFrameStart = nStartFrame;
		tParam.m_nFrameEnd = nEndFrame;
	}

	if( AniSpeed>= 0.0f )
		SetRenderVelocity( AniSpeed , nPartId );

	return DoResult;
}

bool CRenderObject::DoAniFromPart( int32 nPartId, uint32 uDelayTime )
{
	if( !IsValid() ||  -1 == nPartId )
		return false;

	PartAniMap::iterator it = m_PartAni.find(nPartId);
	if(it != m_PartAni.end())
	{
		m_PartAni[-1] = it->second;
		m_PartAni.erase(it);
	}


	_ClearEffect(-1);
	{
		TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
		for( it = m_pEfxMap.Begin(); it!=eit; ++it )
		{
			if(it->iTime != 0)
				continue;

			if( it->_Int32 ==nPartId)
				it->_Int32 = -1;
		}
	}

	m_ActivePart = -1;
	return SetAniFromSep( nPartId, uDelayTime );
}

bool CRenderObject::DoDefaultAni(int32 nPartId, uint32 uDelayTime, float AniSpeed )
{
	if( !IsValid() || -1 == nPartId)
	{
		return false;
	}

	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		if(it->iTime != 0)
			continue;

		if( it->_Int32 == -1)
		{
			it->_Int32 = nPartId;
		}
	}

	m_ActivePart = nPartId;
	m_PartAni[nPartId] = m_PartAni[-1];
	return SetDefaultAniSep( nPartId, uDelayTime, AniSpeed );
}

void CRenderObject::StopAni( int32 nParId )
{
	StopAni( nParId, 200 );
}

void CRenderObject::StopAni( int32 nParId, int32 DelayTime )
{
	if( !IsValid())
		return;

	if( m_ActivePart == nParId )
		m_ActivePart = -1;

	_ClearEffect(nParId);
	m_PartAni.erase(nParId);
	ClearAniSep( nParId, DelayTime );
}

bool CRenderObject::_RegisterEffect( EffectNode* pEfxNode )
{
	if( pEfxNode )
	{
		pEfxNode->_Uint32 = m_ActivePart;
	}
	return true;
}

void CRenderObject::_ClearEffect(int32 nPart)
{
	if(m_AsynParams.IsActive())
		return;

	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		if(it->iTime != 0 || it->_Int32 != nPart)
			continue;

		if( it->pEffect )
		{
			if( it->pEffect->IsFrameSynch() )
			{
				uint32 idx = it->iIndex - 1;
				m_fxDelPool.insert(&(*it));
				m_pEfxMap.Del(idx);
			}
		}
		else
		{
			it->_Int32 = DEL_ID;
		}
	}
}

bool CRenderObject::_LoadedEffect( EffectNode* pEfxNode )
{
	if( pEfxNode->pEffect->IsFrameSynch() && pEfxNode->_Int32 == DEL_ID )
		return false;
	pEfxNode->pEffect->OnLinked(this);
	pEfxNode->pEffect->SetVisible(IsVisible());
	if(m_OrgAABB.isNull() && m_BoxMap.empty())
		m_OrgAABB.mergeBox(pEfxNode->pEffect->GetAABB());
	return true;
}


//产生水特效
void CRenderObject::MakeWaterFx( CRenderScene* pRenderScene )
{
	if( !m_pWaterFx == !pRenderScene )
		return;

	if( pRenderScene )
	{
		m_pWaterFx = new WaterFx;
		m_pWaterFx->pScene = pRenderScene;
		m_pWaterFx->uPrePosX = (uint16)m_v3Position.x;
		m_pWaterFx->uPrePosY = (uint16)m_v3Position.z;
		m_pWaterFx->uPreTime = (uint32)::GetProcessTime();
	}
	else
		GfkSafeDelete( m_pWaterFx );
}

bool CRenderObject::GetWillRender()
{
	if( !IsValid() || !IsVisible() || !m_pParentSpace )
		return false;
	return true;
}

void CRenderObject::Update(void)
{
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->Update();
}

bool CRenderObject::UpdateTime(uint32 uCurTime)
{
	OnPreRender(uCurTime,NULL);
	bool isTrue = CModel::UpdateTime(uCurTime);

	TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
	AniResult EffectResult;
	for( it = m_pEfxMap.Begin(); it!=eit; ++it )
	{
		EffectNode& EfxNode = (*it);
		switch( EfxNode.iTime )
		{
		case 0:
			if(EfxNode.pEffect && EfxNode.pEffect->UpdateTime( m_uCurTime ))
			{
				DelEffect(EfxNode.iIndex);
				if(	m_pRenderAniInst->m_pHandler)
					m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd("");
			}
			break;
		case -1:
			{

			}
			break;
		default:
			{
				EfxNode.iTime = max( 0,uint32( EfxNode.iTime - m_uFrameTime ) );
				if(EfxNode.iTime == 0)
				{
					DelEffect(EfxNode.iIndex);
					if(	m_pRenderAniInst->m_pHandler)
						m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd("");
				}
			}
			break;
		}	
	}

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->UpdateTime(uCurTime);
	return isTrue;
}

//渲染前调用
void CRenderObject::OnPreRender( uint32 uCurTime,CCamera* pCamera )
{
	if( m_pTime )
		uCurTime = m_pTime->GetCurTime();

	if( !IsValid() )
	{
		for( SList<ResCreateInfo>::iterator it = m_ListResInCreate.begin(); it != m_ListResInCreate.end(); )
		{
			if( (NULL != (*it).pRes) && !(*it).pRes->IsValid() )
			{
				for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
					(*it)->OnPreRenderEnd();
				return;
			}
				
			ResCreateInfo& Info = *it;

			SQR_TRY
			{
				switch(Info.eResType)
				{
				case RFT_ARP:
					{
						IntAddPiece( Info );
						break;
					}
				case RFT_ARA:
					{	
						SetSkeletalFrame( ((CAnimationGroup*)Info.pRes)->GetSkeletalFrame() );
						for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
							_UpdateAttachChild(*it);
					}
				case RFT_AGP:
					{
						SetAnimateGroup( (CAnimationGroup*)(*it).pRes );
						m_PartAni.clear();
						SafeRelease( (*it).pRes );
					}
				};
			}
			SQR_CATCH(exp)
			{
				GfkLogExp(exp);
			}
			SQR_TRY_END;

			m_ListResInCreate.erase( it++ );
		}

		if( !IsValid() )
		{
			for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
				(*it)->OnPreRenderEnd();
			CRenderNode::SetVisible(false);
			return;
		}
		
		
		if(m_pRenderAniInst && m_pRenderAniInst->m_pHandler )
		{
			m_pRenderAniInst->m_pHandler->OnResLoaded();
		}
		m_AsynParams.Do(FunDoAni::stGetType());
		Update();
	}

	if( m_pWaterFx )
	{
		CVector2f vecDir = CVector2f( (float)( m_v3Position.x - m_pWaterFx->uPrePosX ), (float)( m_v3Position.z - m_pWaterFx->uPrePosY ) );
		if( abs( vecDir.x ) + abs( vecDir.y ) > 10 )
		{
			m_pWaterFx->uPrePosX =(uint16) m_v3Position.x;
			m_pWaterFx->uPrePosY = (uint16)m_v3Position.z;
			m_pWaterFx->pScene->DoDimple( *this, vecDir*0.5f, true );
		}
		else if( uCurTime - m_pWaterFx->uPreTime > 700 )
		{
			m_pWaterFx->uPreTime = uCurTime;
			m_pWaterFx->pScene->DoDimple( *this, vecDir, false );
		}
	}

	AyncFun();
	m_uCurTime = uCurTime;
	//SetShowPiece();

	uint64 uNowTime = (uint64)m_uCurTime;//GetProcessTime();
	m_uFrameTime = uNowTime - m_uLastFrameTime;
	UpdateScale(uNowTime);
	m_uLastFrameTime = uNowTime;

	CMatrix tempMat;
	if(m_pParentSpace)
	{
		m_pParentSpace->GetAttachMatrix( tempMat, m_uLinkID );

		if(m_eLinkType==LT_SKELETAL_OFFSET)
		{
			CMatrix tmp = tempMat;
			tempMat._11 = tmp.GetAxis(0).Mag();
			tempMat._22 = tmp.GetAxis(1).Mag();
			tempMat._33 = tmp.GetAxis(2).Mag();

			tempMat._44 = 1.0f;
			tempMat._12 = tempMat._13 = tempMat._14 = 
				tempMat._21 = tempMat._23 = tempMat._24 = 
				tempMat._31 = tempMat._32 = tempMat._34 = 0.0f;
		}
	}

	SetFixMatrix(tempMat,m_FixMat);

	if(m_IsVailedShow&&pCamera)
		CRenderNode::SetVisible(pCamera);
	else 
		CRenderNode::SetVisible(m_IsVailedShow);

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->OnPreRenderEnd();

	CRenderSpace::OnPreRender( uCurTime,pCamera );
}

void CRenderObject::Render( void )
{
	
	SQR_TRY{
		_Render();	}
	SQR_CATCH(exp) {
		GfkLogExpOnce(exp);
	}
	SQR_TRY_END;
}

void CRenderObject::SetVisible(bool isVisible)
{
	if ( IsVisible()!= isVisible )
	{
		CModel::SetVisible(isVisible);
		for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		{
			if((*it)->GetNodeType() == CRenderObject::st_RenderObjectType)
			{
				CRenderObject* pRenderObj=class_cast<CRenderObject*>(*it);
				if(isVisible == false)
					pRenderObj->SetVisible(isVisible);
			}
		}

		for( TIDPtrMap< EffectNode >::Iterator it = m_pEfxMap.Begin(); it!=m_pEfxMap.End(); ++it )
		{
			EffectNode& EfxNode = (*it);
			if( EfxNode.pEffect )
			{
				EfxNode.pEffect->SetVisible(isVisible);
			}
		}
	}
}

void CRenderObject::_Render()
{
	if( !IsValid() || !m_pParentSpace )
	{
		SetVisible(false);
		for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		{
			if( (*it)->GetNodeType() == st_RenderObjectType )
			{
				((CRenderObject*)(*it))->SetVisible(false);
			}
		}
		return;
	}

	if(m_RenderMatrix.m_CurrentTime == 0)
	{
		CMatrix tempMat;
		if(m_pParentSpace)
			m_pParentSpace->GetAttachMatrix( tempMat, m_uLinkID );
		
		if(m_eLinkType==LT_SKELETAL_OFFSET)
		{
			CMatrix tmp = tempMat;
			tempMat._11 = tmp.GetAxis(0).Mag();
			tempMat._22 = tmp.GetAxis(1).Mag();
			tempMat._33 = tmp.GetAxis(2).Mag();
		
			tempMat._44 = 1.0f;
			tempMat._12 = tempMat._13 = tempMat._14 = 
			tempMat._21 = tempMat._23 = tempMat._24 = 
			tempMat._31 = tempMat._32 = tempMat._34 = 0.0f;
		}

		SetFixMatrix(tempMat,m_FixMat);
		SetVisible(true);
	}

	if(IsVisible())
	{	
		if( m_uAlphaDes != m_uAlpha )
		{
			if( abs(m_uAlphaDes - m_uAlpha) < 4)
				m_uAlpha = m_uAlphaDes;
			else if( m_uAlpha < m_uAlphaDes )
			{
				m_uAlpha += 3;
				limit<uint8>( m_uAlpha, 0, 255 );
			}
			else
			{
				m_uAlpha -= 3;
				limit<uint8>( m_uAlpha, 0, 255 );
			}
		}

		sMaterial material =
		{
			CColor4((uint32)CColor( m_uAlpha, m_rDiffuse, m_gDiffuse, m_bDiffuse ) ),
			CColor4((uint32)CColor( 0xff, m_rAmbient, m_gAmbient, m_bAmbient )),
			CColor4((uint32)0),
			CColor4((uint32)0xffffffff),
			0,
		};

		RenderObjStyle ros =
		{
			m_RenderStyle,
			true,
			true,
			DELTARS_DIFFUSE|DELTARS_AMBIENT,
			(uint32)&material
		};

		AniResult aniResult;
		if( m_uAlpha > 1 )
		{
			if( m_uAlpha < 255 )
			{
				ros.m_bRenderLink = true;
				ros.m_bRenderMesh = true;
				ros.m_eType = (ERenderObjStyle)((((DWORD)m_RenderStyle) | (DWORD)(ROS_ALPHA)) & ~(DWORD)(ROS_SHADOWRECEIVE) ); 
				ros.m_nParam1 |= DELTARS_ALPHADIF;
			}
			aniResult =  CModel::Render( m_uCurTime, &ros );


			TIDPtrMap< EffectNode >::Iterator it,eit = m_pEfxMap.End();
			uint32 EfxTime;
		
			for( it = m_pEfxMap.Begin(); it!=eit; ++it )
			{
				EffectNode& EfxNode = (*it);
				EfxTime = m_uCurTime;
				bool isEnd = false;
				if( EfxNode.pEffect )
				{
					if( EfxNode.pEffect->IsFrameSynch() )
					{
						if( EfxNode._Int32 == -1 )
						{
							if(m_pRenderAniInst->m_IsAniBlend)
							{
								if(m_pRenderAniInst->m_AniStartFrame == 0)
								{
									EfxNode.pEffect->UpdateEfx(0,false);
									continue;	
								}

								EfxTime = uint32((m_pRenderAniInst->m_CurFrame + m_pRenderAniInst->m_AniStartFrame -  m_pRenderAniInst->m_BlendFrame) * m_ParentFrameRatio);
							}
							else
							{
								EfxTime = uint32( m_pRenderAniInst->m_CurFrame* m_ParentFrameRatio);
							}
						}
						else
						{
							if( EfxNode._Int32 < 0 || size_t(EfxNode._Int32) >=m_pRenderAniInst->m_pSkeletalSepList.size())
								continue;
							CAnimateSeparation * pAniSep = NULL;
							pAniSep = m_pRenderAniInst->m_pSkeletalSepList[EfxNode._Int32];
						
							if(NULL!=pAniSep)
							{
								if(pAniSep->m_State == APS_NORMAL)	
									EfxTime =  uint32(pAniSep->m_CurFrame * m_ParentFrameRatio);
								else
								{
									EfxNode.pEffect->UpdateEfx(0,false);
									continue;
								}	
							}
						}
					}
					isEnd = EfxNode.pEffect->UpdateEfx(EfxTime,EfxNode.iTime!=0);
					EfxNode.pEffect->Render( GetWorldMatrix() ,&ros );
				}

				switch( EfxNode.iTime )
				{
				case 0:
					if(isEnd)
					{
						DelEffect(EfxNode.iIndex);
						if(	m_pRenderAniInst->m_pHandler)
							m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd("");
						continue;
					}
					break;
				case -1:
					{
						
					}
					break;
				default:
					{
						EfxNode.iTime = max( 0,int32( EfxNode.iTime - m_uFrameTime ) );
						if(EfxNode.iTime == 0)
						{
							DelEffect(EfxNode.iIndex);
							if(	m_pRenderAniInst->m_pHandler)
								m_pRenderAniInst->m_pHandler->OnLinkAnimationEnd("");
							continue;
						}
					}
					break;
				}	
			}
		}
		else
			SetVisible(false);


		if( m_pRenderAniInst->m_pHandler )
		{
			WORD dCurKeyFrame = (WORD)aniResult.m_CurFrame;

			if( aniResult.m_IsReset )
			{
				SMap<WORD, bool>::iterator iter;
				SMap<WORD, bool>::iterator end = m_FootStepFrameList.end();
				for ( iter = m_FootStepFrameList.begin(); iter != end; ++iter )
					iter->second = false;
			}

			if (m_uLastKeyFrame != dCurKeyFrame)
			{
				if( aniResult.m_SepFrameString )
				{
					SQR_TRY
					{
						m_pRenderAniInst->m_pHandler->OnSepKeyFrame( (const char*)aniResult.m_SepFrameString );
					}
					SQR_CATCH(exp)
					{	
						exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":OnSepKeyFrame");
						LogExp(exp);
					}
					SQR_TRY_END;
				}
				if( aniResult.m_FrameString )
				{
					SQR_TRY
					{
						m_pRenderAniInst->m_pHandler->OnKeyFrame( (const char*)aniResult.m_FrameString );
					}
					SQR_CATCH(exp)
					{	
						exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":OnKeyFrame");
						LogExp(exp);
					}
					SQR_TRY_END;
					m_uLastKeyFrame = dCurKeyFrame;	
				
					SMap<WORD, bool>::iterator iter = m_FootStepFrameList.find(m_uLastKeyFrame);
					if( iter != m_FootStepFrameList.end() )
					{
						iter->second = true;
					}
				}
				else
				{
					///处理脚步声掉帧情况
					ReCallFootStep(dCurKeyFrame);
					m_uLastKeyFrame = dCurKeyFrame;
				}
			}
		}
		CRenderSpace::Render();
	}
	else
	{	
		for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
			(*it)->UpdateTime(m_uCurTime);
	}
}

void CRenderObject::RenderSel( uint32 uCurTime, RenderObjStyle* pRS )
{
	if( !IsValid() )
		return;
	CModel::Render(  uCurTime, pRS );

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->RenderSel( uCurTime, pRS );
}

void CRenderObject::DelChild( CRenderSpaceNode* pChild )
{
	//m_AsynParams.Delete( FunAddChild::stGetType(),(void*)pChild );
	CRenderSpace::DelChild(pChild);
}

void CRenderObject::_UpdateAttachChild(CRenderSpaceNode* pChild)
{
	int32 nSkeletalID = -1, nSocketID = -1;
	pChild->m_uLinkID = MAKELONG( nSocketID, nSkeletalID );

	CSkeletalFrame* pSkeFrame = 		GetSketetalFrame();
	if( pSkeFrame )
	{
		switch(pChild->m_eLinkType)
		{
		case LT_SOCKET:
			pSkeFrame->GetIDBySocketName( nSkeletalID, nSocketID, pChild->m_AttachName.c_str() );
			break;
		case LT_SKELETAL:
		case LT_SKELETAL_OFFSET:
			nSkeletalID = CModel::AddSkeletal( pChild->m_AttachName.c_str() );
			break;
		case LT_CENTER:
			nSkeletalID = 0;
			break;
		}
		pChild->m_uLinkID = MAKELONG( nSocketID, nSkeletalID );
	}	
	pChild->Update();
}

void CRenderObject::AddChild( CRenderSpaceNode* pChild, eLinkType linkType, const char* szAttachName )
{
	Ast(pChild != this && "CRenderObject对象不能自己做自己的父");

	pChild->m_AttachName	= szAttachName;
	pChild->m_eLinkType		= linkType;
	CRenderSpace::AddChild( pChild, linkType, szAttachName );
	_UpdateAttachChild(pChild);
	Update();
	pChild->SetAlphaValue(m_uAlpha);

}

float CRenderObject::GetObjHeight()
{
	return IsValid() ? GetBoxScale().y : float(eGridSpan);
}

//void CRenderObject::GetBox( CVector3f& vecOff, CVector3f& vecScale )
//{
//	vecOff	 =	IsValid() ? GetBoxOff() : CVector3f( 0, float(eGridSpan/2), 0 );
//	vecScale =	IsValid() ? GetBoxScale() : CVector3f( float(eGridSpan), float(eGridSpan), float(eGridSpan) );
//	vecScale *= m_fFinalScale;
//}

uint8 CRenderObject::GetAlphaValue()
{
	return m_uAlpha;
}

void CRenderObject::SetAlphaValue( uint8 uAlpha )
{
	if(m_uAlphaDes == 0 && uAlpha > 0)
	{
		for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		_UpdateAttachChild(*it);
	}

	m_uAlphaDes = m_uAlpha = uAlpha;
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->SetAlphaValue( uAlpha );
}

void CRenderObject::SetAlphaValueEx( uint8 uAlpha )
{
	m_uAlphaDes = uAlpha;
}

void CRenderObject::SetDiffuse( CColor Color )
{
	m_rDiffuse = Color.R;
	m_gDiffuse = Color.G;
	m_bDiffuse = Color.B;
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->SetDiffuse( Color );
}

void CRenderObject::SetAmbient( CColor Color )
{
	m_rAmbient = Color.R;
	m_gAmbient = Color.G;
	m_bAmbient = Color.B;
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->SetAmbient( Color );
}

CColor CRenderObject::GetDiffuse(void)
{
	return CColor(m_uAlphaDes,m_rDiffuse,m_gDiffuse,m_bDiffuse);
}
		
CColor CRenderObject::GetAmbient(void)
{
	return CColor(m_uAlphaDes,m_rAmbient,m_gAmbient,m_bAmbient);
}


const CVector3f CRenderObject::GetCenterPosition()
{
	CMatrix mat;
	GetMatrix( mat, 1, 0xffffffff );
	return mat.GetLoc();
};

const CVector3f CRenderObject::GetSkeletalPositionByName(const char* szSkeletalName)
{
	CVector3f Off;
	GetRootPosition(&Off);

	CMatrix mat;
	int32 nSkeletalID = GetSkeletalIdByName(szSkeletalName);
	if (nSkeletalID < 1)
	{
		return Off;
	}

	if( GetMatrix( mat, nSkeletalID, 0xffffffff ) )
		Off = mat.GetLoc();
	
	return Off;
};

void CRenderObject::GetRootPosition( CVector3f* pVec )
{
	pVec->Init( m_v3Position.x, m_v3Position.y, m_v3Position.z );
};

//得到渲染对象的动作原点的平面坐标
CFPos CRenderObject::Get2DPosition()
{
	return CFPos( m_v3Position.x, m_v3Position.z );
}
const CDir& CRenderObject::GetDirection()
{
	return *(CDir*)&m_uCurDirection;
};

/*CDir对象为unsafe，我们不能直接通过GetDirection得到*/
void CRenderObject::GetDirection(CDir& dir)
{
	dir = GetDirection();
}

//CRenderable* CRenderObject::GetRenderObj()
//{
//	//return IsValid() ? m_pRenderObj : NULL;
//	return this;
//}

//设置当前的动作为上半身动作
// void CRenderObject::SetNowAnitoUpbodyAni( BYTE SkeletalFatherId, BYTE SkeletChildId )
// {
// 			SetNowAnitoUpbodyAni(SkeletalFatherId, SkeletChildId);
// }

// void CRenderObject::AbsolutecalculateUpBody( int32 boneid )
// {
// 			AbsolutecalculateUpBody( boneid );
// }

float CRenderObject::GetCurFrameNum()
{
	return 		GetCurFrameNum();
}

//void CRenderObject::SetPosition( const CVector3f& Pos, bool bHasLogicHeight )
//{
//	m_v3Position.x = Pos.x;
//	m_v3Position.y = Pos.y;
//	m_v3Position.z = Pos.z;
//	m_bNeedUpdate = true;
//}

void CRenderObject::SetNormal( const CVector3f& Normal )
{
	if( m_bTerrainheight )
	{	
		CVector2f vDir = CDir( m_uCurDirection );
		CVector3f norxz( Normal.x, 0.0f, Normal.z );
		CVector3f Dir( vDir.x,0.0f,vDir.y );
		SetRotate(CVector3f::UNIT_Y,(float)( m_uCurDirection*MATH_PI*2/256 ));
		Pitch(norxz.Dot( Normal ) * Dir.Dot(norxz));
		//Rotation.SetRotateX(norxz.Dot( Normal ) * Dir.Dot(norxz));
		//m_matWorld = Rotation * m_matWorld;
	}
}

void CRenderObject::SetDirection( const CDir& Dir )
{
	m_uCurDirection = Dir.uDir;
	SetRotate(CVector3f::UNIT_Y,(float)( m_uCurDirection*MATH_PI*2/256 ));

}

//const CMatrix& CRenderObject::GetMatrix()
//{
//	_update();
//	return m_matWorld;
//}

void CRenderObject::SetSize( float fSize )
{
	m_FixMat.SetScale(fSize,fSize,fSize);
	//m_fSize = fSize;
}

float CRenderObject::GetScale()
{
	return m_vScale.y;
}

float CRenderObject::GetFinalScale()
{
	return m_fFinalScale;
}

void CRenderObject::SetScale( float fScale )
{
	//cout << uint32(this) << "::SetScale(" << fScale << "), m_fFinalScale = " << m_fFinalScale << ")\n";
	setScale(fScale);
	m_fFinalScale = fScale;
	m_bScaleIsFinal = true;
}

void CRenderObject::SetFinalScale( float fScale, uint64 uFinalTime )
{	
	//cout << uint32(this) << "::SetFinalScale(" << fScale << ", " << uFinalTime << ")\n";
	m_fFinalScale = fScale;
	m_uFinalTime = uFinalTime;

	if(m_vScale.y == fScale)
	{
		m_bScaleIsFinal = true;
		return;
	}

	m_bScaleIsFinal = false;

	uint64 uNowTime = GetProcessTime();
	if(m_uFinalTime <= uNowTime || m_uFrameTime == 0)
	{
		m_bScaleIsFinal = true;
		setScale(m_fFinalScale);
		return;
	}

	float fCurScale = m_vScale.y;
	if(fCurScale == 0.0f)
	{
		if(m_fFinalScale != 0.0f)
		{
			fCurScale = min(m_fFinalScale, 0.1f);
		}
		return;
	}

	uint64 uLeftFrameCount = (m_uFinalTime - uNowTime) / m_uFrameTime;
	if(uLeftFrameCount == 0)
	{
		m_bScaleIsFinal = true;
		setScale(m_fFinalScale);
	}
	else
	{
		setScale(fCurScale * pow(m_fFinalScale / fCurScale, 1.0f / (float) uLeftFrameCount));
	}
	//cout << "Set m_fScale = " << m_fScale << endl;
}

void CRenderObject::UpdateScale( uint64 uCurTime )
{
	if(m_vScale.y == m_fFinalScale)
	{
		if(m_bScaleIsFinal == false)
		{
			m_bScaleIsFinal = true;
			//cout << uint32(this) << "::UpdateScale(" << uCurTime << "), m_fScale = " << m_fScale <<
			//	", m_fFinalScale = " << m_fFinalScale << ", m_bNeedUpdate = " << m_bNeedUpdate <<
			//	", m_bScaleIsFinal = " << m_bScaleIsFinal << endl;
		}
		return;
	}
	//cout << uint32(this) << "::UpdateScale(" << uCurTime << "), m_fScale = " << m_fScale <<
	//	", m_fFinalScale = " << m_fFinalScale << ", m_bNeedUpdate = " << m_bNeedUpdate <<
	//	", m_bScaleIsFinal = " << m_bScaleIsFinal << endl;

	uint64 uNowTime = uCurTime;
	//m_uFrameTime = uNowTime - (uint64)uCurTime;
	if(m_uFinalTime <= uNowTime || m_uFrameTime == 0)
	{
		if(m_bScaleIsFinal == false)
		{
			m_bScaleIsFinal = true;
			setScale(m_fFinalScale);
		}
		return;
	}

	float fCurScale = m_vScale.y;
	if(fCurScale == 0.0f)
	{
		if(m_fFinalScale != 0.0f)
		{
			fCurScale = min(m_fFinalScale, 0.1f);
		}
		return;
	}

	uint64 uLeftFrameCount = (m_uFinalTime - uNowTime) / m_uFrameTime;
	if(uLeftFrameCount == 0)
	{
		m_bScaleIsFinal = true;
		setScale(m_fFinalScale);
	}
	else
	{
		setScale(fCurScale * pow(m_fFinalScale / fCurScale, 1.0f / (float) uLeftFrameCount));
	}
	//cout << "Update m_fScale = " << m_fScale << ", " << fCurScale << " * pow(" << m_fFinalScale << "/" << fCurScale << ", 1.0f/" << uLeftFrameCount <<
	//		" = (" << m_uFinalTime << " - " << uNowTime << ") / " << m_uFrameTime << ")" << endl;
}

void CRenderObject::GetAttachMatrix( CMatrix& matLink, uint32 uChildLinkID )
{

 	if( uChildLinkID == (uint32)-1 )
 		matLink = GetWorldMatrix();
 	else
	{
		GetLocalMatrix( matLink, HIWORD( uChildLinkID ), LOWORD( uChildLinkID ) );
		matLink.getColumn(3).FastMultiply(m_FixMat);
		matLink*=GetWorldMatrix();
	}
}

void CRenderObject::SetTerrainInfluence( bool bValue )
{
	m_bTerrainheight = bValue;
}

EffectCheckResult	CRenderObject::CheckEffect(const char* szFileName,const char* szParam)
{
	if( !szFileName )
		return eECR_NoExist;

	EffectCheckResult eResult=eECR_NoExist;
	IEffectManager* pEffectMgr = IEffectManager::GetInst();
	IEffectGroup* pEfxGroup;
	bool isAsyc = CPkgFile::IsEnableAsyncRead();
	CPkgFile::EnableAsyncRead( m_IsAsynLoadPiece );
	if( FAILED( pEffectMgr->CreateEffectGroup( szFileName, (IEffectGroup**)&pEfxGroup ) ))
	{
		CPkgFile::EnableAsyncRead( isAsyc );
		return eECR_NoExist;
	}

	CPkgFile::EnableAsyncRead( isAsyc );
	IEffect* pEffect;
	SString str = szParam;
	if( !str.empty() )
	{
		HRESULT Result=pEffectMgr->CreateEffect( pEfxGroup, str.c_str(), &pEffect );
		if (Result==GERROR)
		{
			return eECR_NoExist;
		}
		eResult=pEffect&&pEffect->IsFollowBone(this)?eECR_BoneFollow:eECR_Normal;
		SafeRelease( pEffect );
	}
	SafeRelease(pEfxGroup);
	return eResult;
}

void CRenderObject::Delframework()
{
	CModel::DelFramework();
	m_PartAni.clear();
	for( SList<ResCreateInfo>::iterator it = m_ListResInCreate.begin(); it != m_ListResInCreate.end(); )
	{
		if( (*it).eResType == RFT_ARA || (*it).eResType == RFT_AGP )
		{
			SafeRelease( (*it).pRes );
			m_ListResInCreate.erase( it++ );
		}
		else
			++it;
	}

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		_UpdateAttachChild(*it);
}

void CRenderObject::RemoveAllPiece(void)
{
	ClearMeshPiece();
	for( SList<ResCreateInfo>::iterator it = m_ListResInCreate.begin(); it != m_ListResInCreate.end(); )
	{
		ResCreateInfo& Info = *it;
		if( Info.eResType == RFT_ARP )
		{
			SafeRelease( (*it).pRes );
			m_ListResInCreate.erase( it++ );
		}
		else
			++it;
	}
	m_AsynParams.Delete(FunShowPiece::stGetType(),NULL);
	m_pPortraitInfo = NULL;
}

bool CRenderObject::IsValid()
{
	return m_ListResInCreate.empty();
}

void CRenderObject::HoldAniStill( int32 Partid )
{
	CModel::StopAni( Partid );
}

CRenderDialog* CRenderObject::CreateRenderDialog(eLinkType linkType, const char* szAttachName)
{
	CRenderDialog* pRenderDialog = new CRenderDialog;
	if (NULL!=pRenderDialog)
		AddChild(pRenderDialog,linkType,szAttachName);
	return pRenderDialog;
}

void CRenderObject::DestroyRenderDialog(CRenderDialog* pWnd)
{
	delete pWnd;
}

CRenderObject* CRenderObject::CreateRenderObject(eLinkType linkType, const char* szAttachName)
{
	CRenderObject* pRenderObj = new CRenderObject(m_pTime,m_pLogicHandler);
	AddChild(pRenderObj,linkType,szAttachName);
	return pRenderObj;
	return NULL;
}

void	CRenderObject::DestroyRenderObject(CRenderObject* pObj)
{
	CRenderSpace::DestroyRenderChild(pObj);
}

void	CRenderObject::_DestoryRenderChild(CRenderSpaceNode* pObj)
{
	if(pObj->GetNodeType() == CRenderObject::st_RenderObjectType)
	{
		CRenderObject* pRenderObj=class_cast<CRenderObject*>(pObj);
		if(pRenderObj) pRenderObj->SetModelHandler(NULL);
	}
	delete pObj;
}

void sqr::CRenderObject::SetTerrainMaterialIndex( const int8 index )
{
	m_nMaterialIndex = index;
}

int8 sqr::CRenderObject::GetTerrainMaterialIndex() const
{
	return m_nMaterialIndex;
}

void CRenderObject::Activate()
{
	if (GetModelHandler())
	{
		GetModelHandler()->BeSelected();
	}

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		(*it)->Activate();
}

IntersectFlag	CRenderObject::IntersectRay( const CVector3f& rayPos, const CVector3f& rayDir )
{
	if(!m_bCanBeSelect)
		return IF_IMPOSSIBLE;

	IntersectFlag Flag = CModel::IntersectRay(rayPos,rayDir);

	if( Flag == IF_EXCELLENT )
		return Flag;

	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
	{
		Flag = max(Flag,(*it)->IntersectRay( rayPos,rayDir ));
		if( Flag == IF_EXCELLENT )
			return Flag;
	}
	return Flag;
}

///处理脚步声掉帧情况
void sqr::CRenderObject::ReCallFootStep(const WORD uCurKeyFrame)
{
	if( !m_FootStepFrameList.empty() )
	{
		static const IDNAME FootStep("f");

		SMap<WORD, bool>::iterator beg = m_FootStepFrameList.begin();
		SMap<WORD, bool>::iterator end = m_FootStepFrameList.end();
		

		for ( beg; beg != end; ++beg)
		{
			if( !beg->second && uCurKeyFrame >= beg->first )
			{
				WORD wFrameKey = beg->first;
				GMap<WORD, GString>::iterator iter = m_CurAniFootStepFrames.find(wFrameKey);
				if( iter != m_CurAniFootStepFrames.end() )
				{
					beg->second = true;
					SQR_TRY
					{
						m_pRenderAniInst->m_pHandler->OnKeyFrame(iter->second.c_str());
					}
					SQR_CATCH(exp)
					{
						exp.AppendType(GraphicErr::GetErrTypeStr(CallBack_Err)+":OnKeyFrame");
						LogExp(exp);
					}
					SQR_TRY_END;
					return;
				}
			}
		}
	}
}

int32 sqr::CRenderObject::AddSkeletal( const char* name )
{
	int32 sk = CModel::AddSkeletal(name);
	for( SpaceNodeList::iterator it = m_listNode.begin(); it != m_listNode.end(); ++it )
		_UpdateAttachChild(*it);
	return sk;
}

void CRenderObject::_UpdateAniGrp(void)
{
	CAnimate* pAni = NULL;
	pAni = m_pAnimationGroup->GetAniById(m_RenderMatrix.m_CurAnimate);

	if(!pAni)
		return;

	if(!pAni->IsLoaded())
		pAni->CreateResFile();
	if(!IsValidEx(pAni))
	{
		m_AsynParams.Insert(new FunUpdateAniGrp(this));
		return;
	}

	CModel::_UpdateAniGrp();
}

bool CRenderObject::IsValidEx(CAnimate* pAni)
{
	if(!pAni->IsLoaded())
		pAni->CreateResFile();
	if(!pAni->IsValid())
		return false;
	else
		return true;
}

CAxisAlignedBox* CRenderObject::GetOrgAABB()
{
	return &m_OrgAABB;
}

CMatrix& CRenderObject::GetCenterMatrix()
{
	return m_CenterMatrix;
}
