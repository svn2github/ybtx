#include "stdafx.h"
#include "../ChunkData/CGameAudioCoder.h"
#include "../ChunkData/CreateSceneObjectMgr.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "LoadMapMgr.h"
#include "SceneAudio.h"
#include "SceneAudioMgr.h"
#include "LocalMapResMgr.h"

BEGIN_VERSION_MAP(CGameAudioCoder)
	ON_VERSION( 1 , DeCode_1 )
END_VERSION_MAP()

bool sqr::CGameAudioCoder::Code( WriteDataInf& wdi , OBJ_HANDLE handle )
{
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nRegionIndex = *(static_cast<int*>(handle));
	STileRegion   *pTileRegion = pTerrain->GetTileRegion(nRegionIndex);
	int			  nGridIndex = 0;

	vector<CTObjectPtr> vecAudioObejects;
	size_t size = vecAudioObejects.size();
	wdi.Write(&size,		sizeof(size));

	vector< CTObjectPtr >::iterator beg = vecAudioObejects.begin();
	vector< CTObjectPtr >::iterator end = vecAudioObejects.end();
	for (beg; beg != end; ++beg)
	{
		CTObject * p = (*beg);
		CEditModelGroup *pModelGroup = p->GetModelGroup();

		CMatrix mat;
		pModelGroup->GetFinalMatrix(mat);

		///play len ane mark
		DWORD dwPlayTimeLenMin = pModelGroup->GetPlayTimeLenMin();
		DWORD dwPlayTimeLenMax = pModelGroup->GetPlayTimeLenMax();

		///二段播放间隔长度最小最大值
		DWORD dwPlayIntervalMin = pModelGroup->GetPlayIntervalMin();
		DWORD dwPlayIntervalMax = pModelGroup->GetPlayIntervalMax();

		bool bPlayNotRepeat = dwPlayTimeLenMin != 0 && dwPlayTimeLenMax != 0 && dwPlayIntervalMin != 0 && dwPlayIntervalMax != 0 ;
		wdi.Write(&bPlayNotRepeat,		  sizeof(bPlayNotRepeat));
		if( bPlayNotRepeat )///有播放机制才保存播放长度和间隔
		{
			wdi.Write(&dwPlayTimeLenMin,  sizeof(dwPlayTimeLenMin));
			wdi.Write(&dwPlayTimeLenMax,  sizeof(dwPlayTimeLenMax));
			wdi.Write(&dwPlayIntervalMin, sizeof(dwPlayIntervalMin));
			wdi.Write(&dwPlayIntervalMax, sizeof(dwPlayIntervalMax));
		}

		///audio world pos
		CVector3f worldPos = CVector3f(mat._41, mat._42, mat._43);
		wdi.Write(&worldPos,			  sizeof(worldPos));

		// audio index      
		WORD w = p->GetResCueIndex();
		wdi.Write(&w,					  sizeof(w));

		// local grid index
		nGridIndex = pModelGroup->GetCenterGridIndex();
		wdi.Write(&nGridIndex,			  sizeof(nGridIndex));

		//DebugOut("save %f, %f, %f, grid %d, index %d\n", worldPos.x, worldPos.y, worldPos.z, nGridIndex, w);
	}

	return true;
}

bool sqr::CGameAudioCoder::DeCode_1( const CDataChunk& inData , OBJ_HANDLE handle )
{
	CBufFile tData( inData.GetBuffer(), inData.GetSize() );
	
	CTerrainMesh  *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int			  nGridIndex = 0;
	string		  strCueName("");

	vector<string>	&vecCueNameUnit = CLoadMapResInfo::GetInst()->m_CueNameUnit;

	size_t audiosize = CLoadMapResInfo::GetInst()->vecEditorAudioOffsetInfo.size();
	size_t size = 0, editorInfoIndex = 0;
	tData.SafeRead( &size, sizeof(size) );
	for ( size_t i = 0; i < size; ++i )
	{
		///play len ane mark
		DWORD dwPlayTimeLenMin = 0, dwPlayTimeLenMax = 0;

		///二段播放间隔长度最小最大值
		DWORD dwPlayIntervalMin = 0, dwPlayIntervalMax = 0;

		bool bPlayNotRepeat = false ;
		tData.SafeRead( &bPlayNotRepeat, sizeof(bPlayNotRepeat) );
	
		if( bPlayNotRepeat )///有播放机制才保存播放长度和间隔
		{
			tData.SafeRead( &dwPlayTimeLenMin,  sizeof(dwPlayTimeLenMin) );
			tData.SafeRead( &dwPlayTimeLenMax,  sizeof(dwPlayTimeLenMax) );
			tData.SafeRead( &dwPlayIntervalMin, sizeof(dwPlayIntervalMin) );
			tData.SafeRead( &dwPlayIntervalMax, sizeof(dwPlayIntervalMax) );
		}

		///audio world pos
		CVector3f worldPos;
		tData.SafeRead( &worldPos,  sizeof(worldPos) );

		// audio index      
		WORD w = 0;
		tData.SafeRead( &w, sizeof(w) );
		string strCueName = vecCueNameUnit[w];

		// local grid index
		nGridIndex = 0;
		tData.SafeRead( &nGridIndex, sizeof(nGridIndex) );
		
		///create audio object
		MoldeSaveProperty sModelSavePro;
		sModelSavePro.vPosition			= worldPos;
		sModelSavePro.nCenterGridIndex  = nGridIndex;
		
		if( editorInfoIndex < audiosize )
		{
			CLoadMapResInfo::EditorOffsetInfo *pEditorOffsetInfo = CLoadMapResInfo::GetInst()->vecEditorAudioOffsetInfo[editorInfoIndex];
			sModelSavePro.vUserOffsetEx = pEditorOffsetInfo->vUserOffsetEx;
			++editorInfoIndex;
		}
		//DebugOut("load audio %f, %f, %f, grid %d, %s\n", sModelSavePro.vUserOffsetEx.x, sModelSavePro.vUserOffsetEx.y, sModelSavePro.vUserOffsetEx.z, nGridIndex, strCueName.c_str());

		if(CreateSceneObjectMgr::GetInst()->Rebirth(&sModelSavePro, "effectdummy", "tile/地编专用/model/effectdummy.mod", "", "",
													0, strCueName, false, false, true, false))
		{
			SGrid &grid = pTerrain->GetGrid(nGridIndex);
			CEditModelGroup *pModelGroup = NULL;

			size_t objsize = grid.vecCenterObjects.size();
			CTObjectPtr p = grid.vecCenterObjects[objsize-1];
			pModelGroup = p->GetModelGroup();
			if( pModelGroup )
			{
				pModelGroup->m_pSceneAudio = new SceneAudio;

				pModelGroup->SetPlayTimeLenMin(dwPlayTimeLenMin);
				pModelGroup->SetPlayTimeLenMax(dwPlayTimeLenMax);
				pModelGroup->SetPlayIntervalMin(dwPlayIntervalMin);
				pModelGroup->SetPlayIntervalMax(dwPlayIntervalMax);

				CSceneAudioMgr::GetInst()->AddIndividualSoundObject(p);
			}
		}
	}

	return true;
}

class CAnyObject;


CGameAudioFormat::CGameAudioFormat(void)
: CFileFormatMgr( 'TAFT', 0 )
{
	InitAudioCode();
}

CGameAudioFormat::~CGameAudioFormat(void)
{ /*do nothing~~~*/ }

void sqr::CGameAudioFormat::InitAudioCode()
{
	RegisterCoder(new CGameAudioCoder);
}

bool sqr::CGameAudioFormat::LoadAudioFile( const string& strFile, int nRegionIndex )
{
	return Load( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) );
}

bool sqr::CGameAudioFormat::SaveAudioFile( const string& strFile, int nRegionIndex )
{	
	Save( strFile, static_cast<OBJ_HANDLE>(&nRegionIndex) ); 
	return true;
}