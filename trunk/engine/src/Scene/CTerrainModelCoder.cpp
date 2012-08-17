#include "stdafx.h"
#include "CTerrainModelCoder.h"
#include "CMetaRegion.h"
#include "CRenderModel.h"
#include "CRenderAudio.h"
#include "BaseHelper.h"
#include "CRenderScene.h"
#include "CBakedColorData.h"
#include "CSceneManagerClient.h"
#include "StringHelper.h"
//---------------------------------
#include "TSqrAllocator.inl"
//---------------------------------
#define STATIC_OBJ_SHADOW			0x01  ///阴影标志
#define STATIC_OBJ_TRANS			0x02  ///消隐标志
#define STATIC_OBJ_RECIEVESHADOW	0x04  ///是否接收阴影标志
#define STATIC_OBJ_ONPLAYANI		0x08  ///动画开关标志
#define STATIC_OBJ_SETGRIDMASK		0x10

BEGIN_VERSION_MAP(CTerrainModelCoder)
	ON_VERSION( 0 , DeCode_Ver_0 )
END_VERSION_MAP()

bool CTerrainModelCoder::_CElementDeCode( const DeCodeParams& Params )
{
	CDataChunk tempChunk;
	uint16		ver = 0;
	bool		isDeCode = false;

	//SetDeCodeVersion(ver);
	tempChunk.SetVersion(ver);
	isDeCode = DeCode(tempChunk,Params.m_pNode);	
	return isDeCode;
}

bool CTerrainModelCoder::DeCode_Ver_0( const CDataChunk& inData ,OBJ_HANDLE handle)
{
	CSceneTreeNode* pTerrainModel = (CSceneTreeNode*)handle;
	CMetaRegion* pRegion = NULL;//m_pDataScene->GetRegion( x<<4, y<<4 );
	uint32 regionId;
	int32 w = pTerrainModel->GetID().X;// % m_pDataScene->GetWidth();
	int32 h = pTerrainModel->GetID().Z;// / m_pDataScene->GetWidth();
	uint32 GridIndex = 0;
	w *= 4;
	h *= 4;
	uint32 nWidthGrid = m_pDataScene->GetWidthInGrid();
	CTerrainModelLoader::CTerrainObjList	ObjList;
	uint32 localX = 0, localY = 0;
	for( int x = 0; x<4 ; ++x )
	{
		for( int y = 0; y<4; ++y )
		{
			uint32 deltaWidth = w + x;
			uint32 deltaHeight = y + h;
			regionId = deltaWidth + deltaHeight*m_pDataScene->GetWidthInRegion();
			pRegion = m_pDataScene->GetRegion(regionId);
			if(pRegion == NULL)
				continue;
			
			SVector<CRegionMI>& ModelInfo = pRegion->GetModelInfo();
			size_t size = ModelInfo.size();
			for ( size_t i = 0; i < size; ++i )
			{
				int nGridIndex = ModelInfo[i].GridIndex;
				localX = nGridIndex % eRegionSpan;
				localY = nGridIndex / eRegionSpan;
				GridIndex =( deltaWidth * eRegionSpan +  localX )
					+ (deltaHeight * eRegionSpan +localY) * nWidthGrid;
				if(ModelInfo[i].fScaleX == 0|| ModelInfo[i].fScaleY == 0 || ModelInfo[i].fScaleZ == 0 )
					continue;
				CRenderModel* pObj = new CRenderModel();
				pTerrainModel->AddChild(pObj);

				if(!(ModelInfo[i].nFlag&STATIC_OBJ_SETGRIDMASK))
				{
					pObj->RotateX(ModelInfo[i].fxRotation);
					pObj->RotateZ(ModelInfo[i].fzRotation);
					pObj->RotateY(ModelInfo[i].fyRotation);					
				}				
				pObj->SetPosition(ModelInfo[i].cX,ModelInfo[i].cY,ModelInfo[i].cZ);
				
				pObj->SetScale(ModelInfo[i].fScaleY * 0.2f);
				pObj->setScale(ModelInfo[i].fScaleX * 0.2f,ModelInfo[i].fScaleY * 0.2f,ModelInfo[i].fScaleZ * 0.2f);
				pObj->SetDiffuse(CColor(255,ModelInfo[i].r,ModelInfo[i].g,ModelInfo[i].b));

				uint8 bPlayAni = ModelInfo[i].nFlag&STATIC_OBJ_ONPLAYANI;


				if(  gIsZero(pRegion->GetOffsetHeight(localX,localY)) )//[i].nFlag&STATIC_OBJ_RECIEVESHADOW )
					pObj->SetRenderStyle(ROS_LIGHTMAT1);
				else
					pObj->SetRenderStyle(ROS_SHADOWRECEIVE_LIGHT1);//*/


				pObj->m_bCameraCollision = ModelInfo[i].bCameraCollision; 
				
				SVector<CREATE_ITEM>& CreateParam	= m_pDataScene->GetSetUnit(  ModelInfo[i].SetIndex ).m_UnitPart[0].Items;
				CTerrainModelLoader::CTerrainCoderInf	CInf;
				CInf.pTerrainObj	= pObj;
				CInf.GridIndex		= GridIndex;
				CInf.RSIndex		= 0;
			
				for( size_t j = 0; j < CreateParam.size(); ++j )
				{
					if( CreateParam[j].dwResType ==  RFT_ARP )
					{
						CInf.FileName = CreateParam[j].szFileName;
						CInf.RSIndex = max(CInf.RSIndex,uint32(CreateParam[j].szParam[0]));
						pObj->AddPiece((RES_TYPE)CreateParam[j].dwResType,CreateParam[j].szFileName.c_str(),"",NULL);
					}
					else
					{
						if( CreateParam[j].dwResType ==  RFT_ARE )
						{
							SString szFileName = CreateParam[j].szFileName;
							pObj->AddEffect(szFileName.c_str(),CreateParam[j].szParam.c_str(),CEffectPlayer::PLAY_LOOP,NULL);
						}
						else
						{
							SString szFileName = CreateParam[j].szFileName;
							if( CreateParam[j].dwResType ==  RFT_ARA )
								szFileName = szFileName.substr(0,szFileName.size()-4)+".ske";

							if( !bPlayAni )
								continue;

							pObj->AddPiece((RES_TYPE)CreateParam[j].dwResType,szFileName.c_str(),CreateParam[j].szClassName.c_str(),CreateParam[j].szParam.c_str());
						}

						//continue;
					}
					SyncCoder_BreakPoint();
				}
				ObjList.push_back(CInf); 
			}
			SyncCoder_BreakPoint();

			SVector<CRegionAI>& AudioInfo = pRegion->GetAudioInfo();
			size = AudioInfo.size();
			for ( size_t i = 0; i < size; ++i )
			{
				CRenderAudio* pObj = new CRenderAudio(AudioInfo[i],((CSceneManagerClient*)m_pDataScene)->GetAudioUnit( AudioInfo[i].SetIndex ) );
				pTerrainModel->AddChild(pObj);
			}
			SyncCoder_BreakPoint();
		}
	}
	CTerrainModelLoader* pCoder = new CTerrainModelLoader(pTerrainModel,m_pDataScene);
	pCoder->m_ObjList = ObjList;
	pTerrainModel->SetValid(true);
	return true;
}

CTerrainModelCoder::CTerrainModelCoder(CSceneManagerClient* pDataScene)
: m_pDataScene(pDataScene)
{
	m_eSpri = SPRI_Middle;
}

CTerrainModelLoader::CTerrainModelLoader(CSceneTreeNode* pTerMod,CSceneManagerClient* pScene)
: m_pDataScene(pScene)
{
	m_eSpri = SPRI_Middle;
	//m_pTerMod = pTerMod;
	Lock( pTerMod );
	CSyncSystem::PushCoder(this);
}

bool CTerrainModelLoader::AutoDeCode(void)
{
	CSceneTreeNode* pTerMod = (CSceneTreeNode*)m_pLockNode;
	CTerrainObjList::iterator it,eit = m_ObjList.end();
	for( it = m_ObjList.begin(); it!=eit; )
	{
		(*it).pTerrainObj->OnPreRender(0,NULL);
		if((*it).pTerrainObj->IsValid())
		{			
			CRenderModel* pModel = (*it).pTerrainObj;
			pModel->SetFixMatrix(CMatrix::UNIT_MATRIX);
			if(pModel->GetAABB().isNull())
				pModel->UpdateDynamicBox();

			pModel->UpdateBox();
			pTerMod->AddChild(pModel);
			
			//LoadDiffuse
			const GridColor* pGridColor = m_pDataScene->GetGridColor((*it).GridIndex);
			if(NULL!=pGridColor)
			{
				BakeMap<SString, ModelColor>::const_iterator itm = pGridColor->Map.find((*it).FileName);
				if (pGridColor->Map.end() != itm )
				{
					const ModelColor& modelColor = itm->second;
					for (BakeMap<SString, PieceColor>::const_iterator itp = modelColor.Map.begin();
						itp != modelColor.Map.end(); ++itp)
					{
						const SString& className = itp->first;
						const PieceColor& pieceColors = itp->second;
						for (index_t pieceIndex = 0; pieceIndex < pieceColors.size(); ++pieceIndex)
						{
							const ColorInfo& colors = pieceColors[pieceIndex];
							(*it).pTerrainObj->ModifyMesh(className.c_str(),
								m_pDataScene->GetColorBuffer(),colors.Offset, MO_ADD, pieceIndex,colors.VerNum
								, VES_DIFFUSE, VET_COLOUR);
							SyncCoder_BreakPoint();
						}
					}
				}
			}

			pModel->SetPieceRSIndex(NULL,(*it).RSIndex);
			it = m_ObjList.erase(it);

			CAnimationGroup* pAni = pModel->GetAnimateGroup();
			if( pAni && pAni->GetAnimationNum()>0 )
			{
				//int32 nDeltaTime = int32( pAni->GetAnimationMaxFrame( 0 )*33.3f*(GridIndex&7)/8 );
				pModel->SetNextAnimation( pAni->GetAnimationName(0), FramePair( 0, -1 ), true, 0);
			}
			else
			{
				pModel->SetNextAnimation( "Default", FramePair( 0, -1 ), true ,0 );
			}
		}
		else
			++it;

		SyncCoder_BreakPoint();
	}
	
	if(m_ObjList.size() == 0)
	{
		UnLock();	
		return true;
	}
	else
		return false;
}
