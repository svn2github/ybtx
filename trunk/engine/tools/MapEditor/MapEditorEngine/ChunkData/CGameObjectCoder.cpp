#include "stdafx.h"
#include "../ChunkData/CGameObjectCoder.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "DepathGroupModelInfo.h"
#include <bitset>
#include "CreateSceneObjectMgr.h"
#include "LoadMapMgr.h"
#include "LocalMapResMgr.h"
//#include "TraceHelper.h"

#define STATIC_OBJ_SHADOW			0x01  ///阴影标志
#define STATIC_OBJ_TRANS			0x02  ///消隐标志
#define STATIC_OBJ_RECIEVESHADOW	0x04  ///是否接收阴影标志
#define STATIC_OBJ_ONPLAYANI		0x08  ///动画开关标志

struct CRegionMI
{
	float   cX;
	float   cY;
	float   cZ;
	float   fxRotation;//x
	float   fyRotation;//y
	float   fzRotation;//z
	float	fScaleX;
	float	fScaleY;
	float	fScaleZ;
	float	r;
	float	g;
	float	b;
	uint16  SetIndex;
	int     GridIndex;
	uint8	nFlag;
	bool	bEffectByConfigur;
	bool	bCameraCollision;
};

BEGIN_VERSION_MAP(CGameSceneObjectCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameSceneObjectCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	size_t		  size = 0;
	
	//////////////////////////////////////////////////////////////////////////
	int cnt = 0;
	///复合图素
	wdi.Write(&cnt,		sizeof(size_t));

	size_t test = 0;
	DWORD dwGridIndex = 0;

	for ( DepathModelInfoMap::iterator beg = pTerrain->m_DepathModelMap.begin(); beg != pTerrain->m_DepathModelMap.end(); ++beg )
	{
		const DepathModelInfoArray& depathModelInfoList = beg->second;
		size_t depathModelSize = depathModelInfoList.size();
		for ( size_t mm = 0; mm < depathModelSize; ++mm )
		{
			const DepathModelInfoPtr& pSDepathModelInfo = depathModelInfoList[mm];

			WORD w = pSDepathModelInfo->nModelIndex;

			float fscalex = 0.0f,fscaley = 0.0f,fscalez = 0.0f;
			fscalex = pSDepathModelInfo->fScaleX;
			fscaley = pSDepathModelInfo->fScaleY;
			fscalez = pSDepathModelInfo->fScaleZ;

			// info
			bitset<8> bsObjectAttributes;
			bsObjectAttributes.reset();
			// shadow
			bsObjectAttributes.set(0,pSDepathModelInfo->bShadow?1:0); 
			// translucency
			bsObjectAttributes.set(1,pSDepathModelInfo->bTranslucent?1:0);
			// receive shadow
			bsObjectAttributes.set(2,pSDepathModelInfo->bRecieveShadow?1:0);
			// on/off play ani
			bsObjectAttributes.set(3,pSDepathModelInfo->bPlayAnimate?1:0);

			BYTE by = (BYTE)bsObjectAttributes.to_ulong();


			CRegionMI regionInfo;

			// position info
			regionInfo.cX			= pSDepathModelInfo->fX;
			regionInfo.cY			= pSDepathModelInfo->fY;
			regionInfo.cZ			= pSDepathModelInfo->fZ;

			// rotate info
			regionInfo.fxRotation	= pSDepathModelInfo->fRotationZ;
			regionInfo.fyRotation	= pSDepathModelInfo->fRotationY;
			regionInfo.fzRotation	= pSDepathModelInfo->fRotationZ;

			// scale info
			regionInfo.fScaleX		= fscalex;
			regionInfo.fScaleY		= fscaley;
			regionInfo.fScaleZ		= fscalez;

			// model index 和 local grid index 调转了位置
			regionInfo.SetIndex		= pSDepathModelInfo->nModelIndex;
			regionInfo.GridIndex	= dwGridIndex;

			// flag
			regionInfo.nFlag		= by;

			// ambient ratio
			regionInfo.r			= pSDepathModelInfo->r;
			regionInfo.g			= pSDepathModelInfo->g;
			regionInfo.b			= pSDepathModelInfo->b;

			// 物体隐藏是否受机器配置影响标志
			regionInfo.bEffectByConfigur = pSDepathModelInfo->bVisibleByConfigure;
			regionInfo.bCameraCollision = pSDepathModelInfo->bUseCameraCollision;

			wdi.Write(&regionInfo,		sizeof(regionInfo));
			++test;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	vector<CTObjectPtr> vecSingleObjects;
	size = vecSingleObjects.size();
	wdi.Write(&size,		sizeof(size));

//	DebugOut("save composize %d, single %d\n", test, size);
	///single object
	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecSingleObjects.end();
	for (beg = vecSingleObjects.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		if (pModelGroup->m_SL.m_bIAmbScenePointLight)  
			continue;
		if (pModelGroup->m_SpotLight.m_bIAmbSceneSpotLight)  
			continue;
		if (pModelGroup->GetIsSubUnit())            
			continue;
		if(pModelGroup->m_pSceneAudio)
			continue;
		if(pModelGroup->m_bLoginModel)
			continue;
		if(pModelGroup->m_bReginEffect)
			continue;

		WORD w = p->GetResSetIndex();

		float sX = 0.0f,sY = 0.0f,sZ = 0.0f;
		float x = 0.0f, y = 0.0f, z = 0.0f;
		float fRotation = 0.0f,fYaw = 0.0f,fPitch = 0.0f,fscalex = 1.0f,fscaley = 1.0f,fscalez = 1.0f;

		p->GetModelGroup()->GetImageOffsetToAdjustedCenterGrid(sX,sY,sZ,fRotation,fYaw,fPitch,fscalex,fscaley,fscalez);

		CRegionMI regionInfo;

		// position info
		regionInfo.cX			= sX;
		regionInfo.cY			= sY;
		regionInfo.cZ			= sZ;

				// rotate info
		regionInfo.fxRotation	= fYaw;
		regionInfo.fyRotation	= fRotation;
		regionInfo.fzRotation	= fPitch;

		// scale info
		regionInfo.fScaleX		= fscalex;
		regionInfo.fScaleY		= fscaley;
		regionInfo.fScaleZ		= fscalez;

		// model index 和 local grid index 调转了位置
		regionInfo.SetIndex		= p->GetResSetIndex();
		regionInfo.GridIndex	= pModelGroup->GetCenterGridIndex();

		// flag
		// info
		bitset<8> bsObjectAttributes;
		bsObjectAttributes.reset();
		
		// shadow
		bsObjectAttributes.set(0,p->IsCastShadow()?1:0);
		// translucency
		bsObjectAttributes.set(1,p->GetModelGroup()->GetIsTranslucent()?1:0);
		// receive shadow
		bsObjectAttributes.set(2,p->GetModelGroup()->IsReceiveShadow()?1:0);
		// on/off play ani
		bsObjectAttributes.set(3,p->GetModelGroup()->GetIsPlayAnimation()?1:0);

		BYTE by = (BYTE)bsObjectAttributes.to_ulong();
		regionInfo.nFlag = by;

		// ambient ratio
		float r,g,b;
		p->GetModelGroup()->GetOverallAmbientRatio(r,g,b);
		regionInfo.r			= r;
		regionInfo.g			= g;
		regionInfo.b			= b;

		// 物体隐藏是否受机器配置影响标志
		regionInfo.bEffectByConfigur = p->GetModelGroup()->GetIsVisibleByConfigure();
		regionInfo.bCameraCollision = p->GetModelGroup()->GetIsUseCameraCollison();

		wdi.Write(&regionInfo,		sizeof(regionInfo));
	}
	return true;
}

bool sqr::CGameSceneObjectCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nGridIndex = 0;
	size_t		  nResSize   = CLoadMapResInfo::GetInst()->m_SetUnit.size();
	size_t		  editormodelInfoIndex = 0;

	//////////////////////////////////////////////////////////////////////////
	///复合图素
	size_t size = 0;
	tData.SafeRead( &size,	sizeof(size) );
	for (size_t i = 0; i < size; ++i)
	{
		CRegionMI regionInfo;
		tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
	}

	size_t test = size;
	//////////////////////////////////////////////////////////////////////////
	///single object
	size = 0;
	tData.SafeRead( &size,	sizeof(size) );
//	DebugOut("load composize %d, single %d\n", test, size);

	for (size_t i = 0; i < size; ++i)
	{
		CRegionMI regionInfo;
		tData.SafeRead( &regionInfo,	sizeof(regionInfo) );

		MoldeSaveProperty sModelSavePro;
		sModelSavePro.vPosition				= CVector3f(regionInfo.cX, regionInfo.cY, regionInfo.cZ);
		sModelSavePro.vRotate				= CVector3f(regionInfo.fxRotation, regionInfo.fyRotation, regionInfo.fzRotation);
		sModelSavePro.vScale				= CVector3f(regionInfo.fScaleX, regionInfo.fScaleY, regionInfo.fScaleZ);
		sModelSavePro.nCenterGridIndex		= regionInfo.GridIndex;
		sModelSavePro.bUseCameraCollision   = regionInfo.bCameraCollision;
		sModelSavePro.bVisibleByConfigure	= regionInfo.bEffectByConfigur;
		sModelSavePro.bShadow				= (regionInfo.nFlag & STATIC_OBJ_SHADOW) > 0 ? true : false;
		sModelSavePro.bTranslucent			= (regionInfo.nFlag & STATIC_OBJ_TRANS) > 0 ? true : false;
		sModelSavePro.bReceiveShadow		= (regionInfo.nFlag & STATIC_OBJ_RECIEVESHADOW) > 0 ? true : false;
		sModelSavePro.bPlayAnimate			= (regionInfo.nFlag & STATIC_OBJ_ONPLAYANI) > 0 ? true : false;

		uint16 uUnitSetIndex = regionInfo.SetIndex;

		if( uUnitSetIndex >= nResSize )
		{
			continue;
		}

		vector<CLoadMapResInfo::CREATE_ITEM>& CreateParam = CLoadMapResInfo::GetInst()->m_SetUnit[uUnitSetIndex].m_UnitPart[0].Items;
		size_t paramSize = CreateParam.size();

		bool bEffect = false;
		string strModelName(""), strEffectFullName(""), strEffectName(""), strSkeName(""), strItemName("");
		for( size_t i = 0; i < paramSize; ++i )
		{
			switch( CreateParam[i].dwResType )
			{
			case RFT_MOD:
				{
					strModelName = CreateParam[i].szFileName;
					size_t npos = strModelName.rfind("\\");
					if( npos != -1 )
					{
						strItemName = strModelName.substr(npos+1, strModelName.length());
						npos = strItemName.rfind(".");
						if( npos != -1 )
							strItemName = strItemName.substr(0, npos);
					}
					break;
				}

			case RFT_SKE:
				strSkeName = CreateParam[i].szFileName;
				break;
			
			case RFT_EFX:
				strEffectFullName = CreateParam[i].szFileName;
				strEffectName	  = CreateParam[i].szParam;
				break;
			
			default:
				break;
			}
		}	

		if( strModelName.empty() && paramSize == 1)
		{
			strModelName = "tile/地编专用/model/effectdummy.mod";
			strItemName  = "effectdummy";
			bEffect = true;
		}

		if( strModelName.empty() && strItemName.empty() )
		{
			strItemName  = "effectdummy";
			strModelName ="tile/地编专用/model/effectdummy.mod";
		}

		int nAffectRatio = 100;
		CLoadMapResInfo::EditorModelInfo *pEditorModelInfo = NULL;
		if( editormodelInfoIndex < CLoadMapResInfo::GetInst()->editorSingleModelCnt )
		{
			pEditorModelInfo = CLoadMapResInfo::GetInst()->vecEditorSingleModelInfo[editormodelInfoIndex];

			sModelSavePro.vUserOffsetEx    = pEditorModelInfo->vUserOffsetEx;
			sModelSavePro.bBakingColorMark = pEditorModelInfo->bBakingColor;
			sModelSavePro.bTransState      = pEditorModelInfo->bTransState;
			sModelSavePro.nMirrorType	   = pEditorModelInfo->nMirrorType;
			nAffectRatio				   = pEditorModelInfo->nAffectRatio;
		}
		else
		{
			string log = strModelName + "偏移信息丢失";
			MessageBox(NULL, log.c_str(), "提示", MB_OK);
		}

// 		DebugOut("centerinde %d, %s, %s\n", sModelSavePro.nCenterGridIndex, strModelName.c_str(), strEffectName.c_str());
// 		DebugOut("scale  %f, %f, %f\n", sModelSavePro.vScale.x, sModelSavePro.vScale.y, sModelSavePro.vScale.z);
// 		DebugOut("rotate %f, %f, %f\n", sModelSavePro.vRotate.x, sModelSavePro.vRotate.y, sModelSavePro.vRotate.z);
// 		DebugOut("offset %f, %f, %f\n", sModelSavePro.vUserOffsetEx.x, sModelSavePro.vUserOffsetEx.y, sModelSavePro.vUserOffsetEx.z);

 		if(!CreateSceneObjectMgr::GetInst()->Rebirth(&sModelSavePro, strItemName, strModelName, strSkeName, strEffectName, nAffectRatio, "", false, bEffect, false, false))
 		{
			string log = strModelName + "加载失败";
			MessageBox(NULL, log.c_str(), "提示", MB_OK);
 		}
		++editormodelInfoIndex;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
BEGIN_VERSION_MAP(CGameLogicObjectCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameLogicObjectCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	DWORD		  dwGridCntInTileRegion = pTerrain->GetGridCountInTileRegion();
	int			  nGridIndex = 0;
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	
	vector<CTObjectPtr> vecLoginObejects;
	size_t		  size = vecLoginObejects.size();
	wdi.Write(&size,		sizeof(size));
	
	vector< CTObjectPtr >::iterator beg;
	vector< CTObjectPtr >::iterator end = vecLoginObejects.end();
	for (beg = vecLoginObejects.begin(); beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		WORD w = p->GetResSetIndex();

		float sX = 0.0f,sY = 0.0f,sZ = 0.0f;
		float x = 0.0f, y = 0.0f, z = 0.0f;
		float fRotation = 0.0f,fYaw = 0.0f,fPitch = 0.0f,fscalex = 1.0f,fscaley = 1.0f,fscalez = 1.0f;

		p->GetModelGroup()->GetImageOffsetToAdjustedCenterGrid(sX,sY,sZ,fRotation,fYaw,fPitch,fscalex,fscaley,fscalez);

		CRegionMI regionInfo;

		// position info
		regionInfo.cX			= sX;
		regionInfo.cY			= sY;
		regionInfo.cZ			= sZ;

		// rotate info
		regionInfo.fxRotation	= fYaw;
		regionInfo.fyRotation	= fRotation;
		regionInfo.fzRotation	= fPitch;

		// scale info
		regionInfo.fScaleX		= fscalex;
		regionInfo.fScaleY		= fscaley;
		regionInfo.fScaleZ		= fscalez;

		// model index 和 local grid index 调转了位置
		regionInfo.SetIndex		= p->GetResSetIndex();
		regionInfo.GridIndex	= pModelGroup->GetCenterGridIndex();;

		// flag
		// info
		bitset<8> bsObjectAttributes;
		bsObjectAttributes.reset();

		// shadow
		bsObjectAttributes.set(0,p->IsCastShadow()?1:0);
		// translucency
		bsObjectAttributes.set(1,p->GetModelGroup()->GetIsTranslucent()?1:0);
		// receive shadow
		bsObjectAttributes.set(2,p->GetModelGroup()->IsReceiveShadow()?1:0);
		// on/off play ani
		bsObjectAttributes.set(3,p->GetModelGroup()->GetIsPlayAnimation()?1:0);

		BYTE by = (BYTE)bsObjectAttributes.to_ulong();
		regionInfo.nFlag = by;

		// ambient ratio
		byte r,g,b;
		p->GetModelGroup()->GetOverallAmbientRatio(r,g,b);
		regionInfo.r			= r;
		regionInfo.g			= g;
		regionInfo.b			= b;

		// 物体隐藏是否受机器配置影响标志
		regionInfo.bEffectByConfigur = p->GetModelGroup()->GetIsVisibleByConfigure();
		regionInfo.bCameraCollision = p->GetModelGroup()->GetIsUseCameraCollison();

		wdi.Write(&regionInfo,		sizeof(regionInfo));
	}

	return true;
}

bool sqr::CGameLogicObjectCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	size_t nResSize   = CLoadMapResInfo::GetInst()->m_SetUnit.size();
	size_t size = 0, editormodelInfoIndex = 0;
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int	   nGridIndex = 0;
	
	tData.SafeRead( &size,	sizeof(size) );
	for (size_t i = 0; i < size; ++i)
	{
		CRegionMI regionInfo;

		tData.SafeRead( &regionInfo,	sizeof(regionInfo) );
		MoldeSaveProperty sModelSavePro;
		sModelSavePro.vPosition				= CVector3f(regionInfo.cX, regionInfo.cY, regionInfo.cZ);
		sModelSavePro.vRotate				= CVector3f(regionInfo.fxRotation, regionInfo.fyRotation, regionInfo.fzRotation);
		sModelSavePro.vScale				= CVector3f(regionInfo.fScaleX, regionInfo.fScaleY, regionInfo.fScaleZ);
		sModelSavePro.nCenterGridIndex		= regionInfo.GridIndex;
		sModelSavePro.bUseCameraCollision   = regionInfo.bCameraCollision;
		sModelSavePro.bVisibleByConfigure	= regionInfo.bEffectByConfigur;
		sModelSavePro.bShadow				= (regionInfo.nFlag & STATIC_OBJ_SHADOW) > 0 ? true : false;
		sModelSavePro.bTranslucent			= (regionInfo.nFlag & STATIC_OBJ_TRANS) > 0 ? true : false;
		sModelSavePro.bReceiveShadow		= (regionInfo.nFlag & STATIC_OBJ_RECIEVESHADOW) > 0 ? true : false;
		sModelSavePro.bPlayAnimate			= (regionInfo.nFlag & STATIC_OBJ_ONPLAYANI) > 0 ? true : false;

		uint16 uUnitSetIndex = regionInfo.SetIndex;

		if( uUnitSetIndex >= nResSize )
		{
			continue;
		}

		vector<CLoadMapResInfo::CREATE_ITEM>& CreateParam = CLoadMapResInfo::GetInst()->m_SetUnit[uUnitSetIndex].m_UnitPart[0].Items;
		size_t paramSize = CreateParam.size();

		bool bEffect = false;
		string strModelName(""), strEffectFullName(""), strEffectName(""), strSkeName(""), strItemName("");
		for( size_t i = 0; i < paramSize; ++i )
		{
			switch( CreateParam[i].dwResType )
			{
			case RFT_MOD:
				{
					strModelName = CreateParam[i].szFileName;
					size_t npos = strModelName.rfind("\\");
					if( npos != -1 )
					{
						strItemName = strModelName.substr(npos+1, strModelName.length());
						npos = strItemName.rfind(".");
						if( npos != -1 )
							strItemName = strItemName.substr(0, npos);
					}
					break;
				}

			case RFT_SKE:
				strSkeName = CreateParam[i].szFileName;
				break;
		
			case RFT_EFX:
				strEffectFullName = CreateParam[i].szFileName;
				strEffectName	  = CreateParam[i].szParam;
				break;
			
			default:
				break;
			}
		}	

		if( strModelName.empty() && paramSize == 1)
		{
			strModelName = "tile/地编专用/model/effectdummy.mod";
			strItemName  = "effectdummy";
			bEffect = true;
		}

		int nAffectRatio = 100;
		CLoadMapResInfo::EditorModelInfo *pEditorModelInfo = NULL;
		if( editormodelInfoIndex < CLoadMapResInfo::GetInst()->editorLoginModelCnt )
		{
			pEditorModelInfo = CLoadMapResInfo::GetInst()->vecEditorLoginModelInfo[editormodelInfoIndex];

			sModelSavePro.vUserOffsetEx    = pEditorModelInfo->vUserOffsetEx;
			sModelSavePro.bBakingColorMark = pEditorModelInfo->bBakingColor;
			sModelSavePro.bTransState      = pEditorModelInfo->bTransState;
			sModelSavePro.nMirrorType	   = pEditorModelInfo->nMirrorType;
			nAffectRatio				   = pEditorModelInfo->nAffectRatio;

			//pModelGroup->SetModelOffsetOverall(pEditorModelInfo->vOffsetOverall, pEditorModelInfo->bSubUnit);
		}

		if(!CreateSceneObjectMgr::GetInst()->Rebirth(&sModelSavePro, strItemName, strModelName, strSkeName, strEffectName, nAffectRatio, "", false, bEffect, false, false))
		{
			string log = strModelName + "加载失败";
			MessageBox(NULL, log.c_str(), "提示", MB_OK);
		}
		++editormodelInfoIndex;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
CGameObjectFormat::CGameObjectFormat(void)
: CFileFormatMgr( 'TOFT', 0 )
{
	InitObjectCode();
}

CGameObjectFormat::~CGameObjectFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameObjectFormat::InitObjectCode()
{
	RegisterCoder(new CGameSceneObjectCoder);
	RegisterCoder(new CGameLogicObjectCoder);
}

bool sqr::CGameObjectFormat::LoadObjectFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CGameObjectFormat::SaveObjectFile( const string& strFile, int nRegionIndex )
{
	return Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}