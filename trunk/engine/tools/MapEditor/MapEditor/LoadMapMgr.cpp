#include "StdAfx.h"
#include "LoadMapMgr.h"
#include "LocalMapResMgr.h"
#include "ChunkData/CEditorBlockCoder.h"
#include "ChunkData/CGameBlockCoder.h"
#include "ChunkData/CEditorAmpArsCoder.h"
#include "ChunkData/CGameAmpArsCoder.h"
#include "ChunkData/CEditorTerrainCoder.h"
#include "ChunkData/CGameTerrainCoder.h"
#include "ChunkData/CEditorAudioCoder.h"
#include "ChunkData/CGameAudioCoder.h"
#include "ChunkData/CEditorLightCoder.h"
#include "ChunkData/CGameLightCoder.h"
#include "ChunkData/CGameObjectCoder.h"
#include "ChunkData/CEditorObjectCoder.h"
#include "ChunkData/CreateSceneObjectMgr.h"
#include "IoSystem.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "Operator.h"
#include "Operator.inl"
#include "CMapEditObject.h"
#include "ModelGroup.h"
#include "TerrainMeshObject.h"
#include "IoSystem.h"
#include "StringHelper.h"
#include "SceneAudioMgr.h"
#include "SceneRegionMgr.h"
#include "SceneRegion.h"
#include "Render.h"
#include "CGraphic.h"
#include "LocalVersion.h"
#include "Engine.h"
#include "ToolSetTileBlock.h"

CLoadSavedMapMgr::CLoadSavedMapMgr( void )
{
	this->strMapName = "";
	if( !CLoadMapResInfo::HasInst() )
		CLoadMapResInfo::Create();

	if( !CreateSceneObjectMgr::HasInst() )
		CreateSceneObjectMgr::Create();
}

CLoadSavedMapMgr::~CLoadSavedMapMgr( void )
{

}

void CLoadSavedMapMgr::LoadMapRegionSize(string strMapName, int& nRegionInWidth, int& nRegionInDepth )
{
	string dir = IoSystem::GetInst()->GetFullPath("srcmap:");
	size_t npos = dir.rfind("\\");
	if( npos != -1 )
	{
		dir = dir.substr(0, npos);
		npos = dir.rfind("\\");
		if( npos != -1 )
		{
			dir = dir.substr(0, npos+1);
			dir = dir + strMapName + "\\";
		}
	}
	string strFileName = dir + strMapName + ".size_s";

	DWORD dwWidth = CMapEditApp::GetInst()->GetEditingMesh()->GetWidthInRegion();
	DWORD dwDepth = CMapEditApp::GetInst()->GetEditingMesh()->GetDepthInRegion();

	FILE * fp = NULL;
	fp = fopen(strFileName.c_str(),"rb");
	if (!fp)
	{
		string log = strFileName + "无法保存";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	} 	
	fread(&nRegionInWidth,sizeof(int),1,fp);
	fread(&nRegionInDepth,sizeof(int),1,fp);

	fclose(fp);
}

void CLoadSavedMapMgr::SaveMapRegionSize()
{
	string dir = IoSystem::GetInst()->GetFullPath("srcmap:");
	string strFileName = dir + this->strMapName + ".size_s";
	
	DWORD dwWidth = CMapEditApp::GetInst()->GetEditingMesh()->GetWidthInRegion();
	DWORD dwDepth = CMapEditApp::GetInst()->GetEditingMesh()->GetDepthInRegion();

	FILE * fp = NULL;
	fp = fopen(strFileName.c_str(),"wb");
	if (!fp)
	{
		string log = strFileName + "无法保存";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
		return;
	} 	
	fwrite( &dwWidth, sizeof(DWORD), 1, fp );
	fwrite( &dwDepth, sizeof(DWORD), 1, fp );
	
	fclose(fp);
}

bool CLoadSavedMapMgr::LoadBlockInfo()
{
	string dir = IoSystem::GetInst()->GetFullPath("srcmap:");
	string strFileName = dir + this->strMapName + ".blk_s";
	CEditorBlcokFormat editorFormat;
	if( !editorFormat.LoadBlockFile(strFileName) )
		return false;

	dir = IoSystem::GetInst()->GetFullPath("tgtmap:");
	strFileName = dir + this->strMapName + ".blk";
	CGameBlcokFormat gameFormat;
	if( !gameFormat.LoadBlcokFile(strFileName) )
		return false;

	return true;
}

bool CLoadSavedMapMgr::SaveBlockInfo()
{
	string dir = IoSystem::GetInst()->GetFullPath("srcmap:");
	string strFileName = dir + this->strMapName + ".blk_s";
	if( !this->FileIsReadOnly(strFileName) )
	{
		CEditorBlcokFormat editorFormat;
		if( !editorFormat.SaveBlockFile(strFileName) )
		{
			string log = strFileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = strFileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	dir = IoSystem::GetInst()->GetFullPath("tgtmap:");
	strFileName = dir + this->strMapName + ".blk";
	if( !this->FileIsReadOnly(strFileName) )
	{
		CGameBlcokFormat gameFormat;
		if(!gameFormat.SaveBlcokFile(strFileName))
		{
			string log = strFileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = strFileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	return true;
}

void CLoadSavedMapMgr::LoadMapResAmpInfo()
{
	string dir = IoSystem::GetInst()->GetFullPath("worksrc:");
	string strFileName = dir + "map\\" + this->strMapName + ".map_s";
	
	CEditorAmpArsFormat editorFormat;
	if(!editorFormat.LoadAmpArsFile(strFileName))
	{
		string log = strFileName + " 保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	dir = IoSystem::GetInst()->GetFullPath("worktgt:");
	strFileName = dir + "map\\" + this->strMapName + ".map";
	CGameAmpArsFormat gameFormat;
	if(!gameFormat.LoadAmpArsFile(strFileName))
	{
		string log = strFileName + " 保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void CLoadSavedMapMgr::SaveMapResAmpInfo()
{
	string dir = IoSystem::GetInst()->GetFullPath("worksrc:");
	string strFileName = dir + "map\\" + this->strMapName + ".map_s";

	if( !this->FileIsReadOnly(strFileName) )
	{
		CEditorAmpArsFormat editorFormat;
		if(!editorFormat.SaveAmpArsFile(strFileName))
		{
			string log = strFileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = strFileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	dir = IoSystem::GetInst()->GetFullPath("worktgt:");
	strFileName = dir + "map\\" + this->strMapName + ".map";
	if( !this->FileIsReadOnly(strFileName) )
	{
		CGameAmpArsFormat gameFormat;
		if(!gameFormat.SaveAmpArsFile(strFileName))
		{
			string log = strFileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = strFileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void CLoadSavedMapMgr::LoadRegionInfo(const int nodeIndex, const int i, const int j)
{
// 	string fileName = format("tgtreg:%d_%d.trn", i, j);
// 	fileName = IoSystem::GetInst()->GetFullPath(fileName);
// 	CGameRegionFormat gameFormat;
// 	if(!gameFormat.LoadRegionFile(fileName, nodeIndex))
// 	{
// 		string log = fileName + " 保存失败";
// 		MessageBox(NULL,log.c_str(),"提示",MB_OK);
// 	}
}

void CLoadSavedMapMgr::SaveRegionInfo(const int nodeIndex, const int i, const int j)
{
// 	string fileName = format("tgtreg:%d_%d.trn", i, j);
// 	fileName = IoSystem::GetInst()->GetFullPath(fileName);
// 
// 	CGameRegionFormat gameFormat;
// 	if(!gameFormat.SaveRegionFile(fileName, nodeIndex))
// 	{
// 		string log = fileName + " 保存失败";
// 		MessageBox(NULL,log.c_str(),"提示",MB_OK);
// 	}
}

void sqr::CLoadSavedMapMgr::LoadTerrainInfo(const int nodeIndex, const int i, const int j)
{
	string fileName = format("srcter:%d_%d.trn", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CEditorTerrainFormat editorFormat;
	if(!editorFormat.LoadTerrainFile(fileName, nodeIndex))
	{
		string log = fileName + " 保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtter:%d_%d.trn", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CGameTerrainFormat gameFormat;
	if(!gameFormat.LoadTerrainFile(fileName, nodeIndex))
	{
		string log = fileName + " 保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void sqr::CLoadSavedMapMgr::SaveTerrainInfo(const int nodeIndex, const int i, const int j)
{
	string fileName = format("srcter:%d_%d.trn", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CEditorTerrainFormat editorFormat;
		if(!editorFormat.SaveTerrainFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtter:%d_%d.trn", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CGameTerrainFormat gameFormat;
		if(!gameFormat.SaveTerrainFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void CLoadSavedMapMgr::LoadTerrainObjcetInfo(const int nodeIndex, const int i, const int j)
{
	STileRegion   *pTileRegion = CMapEditApp::GetInst()->GetEditingMesh()->GetTileRegion(nodeIndex);

	string fileName = format("srcobj:%d_%d.obj", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CEditorObjectFormat editorObjectFormat;
	if(!editorObjectFormat.LoadObjectFile(fileName, nodeIndex))
	{
		string log = fileName + " 保存场景物件文件失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtobj:%d_%d.obj", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CGameObjectFormat gameObjectFormat;
	if(!gameObjectFormat.LoadObjectFile(fileName, nodeIndex))
	{
		string log = fileName + " 读取场景物件文件失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
	//////////////////////////////////////////////////////////////////////////

	fileName = format("srclight:%d_%d.lgt", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CEditorLightFormat editorLightFormat;
	if(!editorLightFormat.LoadLightFile(fileName, nodeIndex))
	{
		string log = fileName + " 保存场景物件文件失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtlight:%d_%d.lgt", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CGameLightFormat gameLightFormat;
	if(!gameLightFormat.LoadLightFile(fileName, nodeIndex))
	{
		string log = fileName + " 读取场景灯文件失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////
	fileName = format("srcaud:%d_%d.ado", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CEditorAudioFormat editorAudioFormat;
	if(!editorAudioFormat.LoadAudioFile(fileName, nodeIndex))
	{
		string log = fileName + " 保存场景物件文件失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtaud:%d_%d.ado", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CGameAudioFormat gameAudioFormat;
	if(!gameAudioFormat.LoadAudioFile(fileName, nodeIndex))
	{
		string log = fileName + " 读取音效失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void CLoadSavedMapMgr::SaveTerrainObjcetInfo(const int nodeIndex, const int i, const int j)
{
	string fileName = format("srcobj:%d_%d.obj", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CEditorObjectFormat editorObjectFormat;
		if(!editorObjectFormat.SaveObjectFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存场景物件文件失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtobj:%d_%d.obj", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CGameObjectFormat gameObjectFormat;
		if(!gameObjectFormat.SaveObjectFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存场景物件文件失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////
	fileName = format("srclight:%d_%d.lgt", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	CEditorLightFormat editorLightFormat;
	if( !this->FileIsReadOnly(fileName) )
	{
		CEditorLightFormat editorLightFormat;
		if(!editorLightFormat.SaveLightFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存场景灯文件失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtlight:%d_%d.lgt", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CGameLightFormat gameLightFormat;
		if(!gameLightFormat.SaveLightFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存场景灯文件失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	//////////////////////////////////////////////////////////////////////////
	fileName = format("srcaud:%d_%d.ado", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CEditorAudioFormat editorAudioFormat;
		if(!editorAudioFormat.SaveAudioFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存音效失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}

	fileName = format("tgtaud:%d_%d.ado", i, j);
	fileName = IoSystem::GetInst()->GetFullPath(fileName);
	if( !this->FileIsReadOnly(fileName) )
	{
		CGameAudioFormat gameAudioFormat;
		if(!gameAudioFormat.SaveAudioFile(fileName, nodeIndex))
		{
			string log = fileName + " 保存音效失败";
			MessageBox(NULL,log.c_str(),"提示",MB_OK);
		}
	}
	else
	{
		string log = fileName + " 该文件已被他人使用，所以保存失败";
		MessageBox(NULL,log.c_str(),"提示",MB_OK);
	}
}

void CLoadSavedMapMgr::Save()
{
	this->progress->SetProgress(0.0f);

	CLoadMapResInfo::GetInst()->DealGridObjectsToTileRegion();
	CSceneAudioMgr::GetInst()->CalTerrainMaterialIndex();
	///重新再算一次区域是否要实时计算
	CSceneRegionMgr::GetInst()->SubRegionIsNeedRealCal();
	this->DealRegionSceneName();
	///计算音效材质
	CSceneAudioMgr::GetInst()->CalTerrainMaterialIndex();

	CLoadMapResInfo::GetInst()->DestroyAllContainerInfo();
	
	this->progress->SetProgress(0.1f);

	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetTerrain();
	int width = pTerrain->GetWidthInTileRegion();
	int depth = pTerrain->GetDepthInTileRegion();
	int nTileRegionCnt = width * depth;
	this->strMapName = pTerrain->m_strMapName;

	this->SaveMapResAmpInfo();
	this->progress->SetProgress(0.35f);
	
	this->SaveMapRegionSize();

	this->SaveBlockInfo();
	this->progress->SetProgress(0.4f);

	for (int j = 0; j < depth; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			index_t nodeIndex = j * width + i;

			CLoadMapResInfo::GetInst()->DestroyEditorModelInfo();

			CLoadMapResInfo::GetInst()->editorSingleModelCnt = 0;
			CLoadMapResInfo::GetInst()->editorLoginModelCnt = 0;

			//this->SaveRegionInfo(nodeIndex, i, j);
			this->SaveTerrainInfo(nodeIndex, i, j);
			
			CMapEditApp::GetInst()->GetTerrain()->DepathGroupModel(nodeIndex);
			this->SaveTerrainObjcetInfo(nodeIndex, i, j);

			this->progress->SetProgress(float(nodeIndex/nTileRegionCnt)+0.4f);
		}
	}
}

void sqr::CLoadSavedMapMgr::Load(const string& strFileName)
{
	this->progress->SetProgress(0.0f);

	this->strMapName = strFileName;
	size_t npos = this->strMapName.rfind("\\");
	if( npos != -1 )
	{
		this->strMapName = this->strMapName.substr(npos+1, this->strMapName.length());

		npos = this->strMapName.rfind(".");
		this->strMapName = this->strMapName.substr(0, npos);
	}
	CLoadMapResInfo::GetInst()->m_SetUnit.clear();

 	this->LoadMapResAmpInfo();
	this->progress->SetProgress(0.2f);

	CTerrainMesh *pTerrain = NULL;
	pTerrain = CMapEditApp::GetInst()->GetTerrain();
	if( pTerrain == NULL )
	{
		MessageBox(NULL,"地表对象没有创建成功","提示",MB_OK);
		return;
	}
	else
		pTerrain->m_strMapName = this->strMapName;

	this->LoadBlockInfo();
	this->progress->SetProgress(0.3f);

	int width = pTerrain->GetWidthInTileRegion();
	int depth = pTerrain->GetDepthInTileRegion();
	int nTileRegionCnt = width * depth;

	for (int j = 0; j < depth; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			index_t nodeIndex = j * width + i;
			
			CLoadMapResInfo::GetInst()->DestroyEditorModelInfo();

			CLoadMapResInfo::GetInst()->editorSingleModelCnt = 0;
			CLoadMapResInfo::GetInst()->editorLoginModelCnt = 0;

			//this->LoadRegionInfo(nodeIndex, i, j);
			this->LoadTerrainInfo(nodeIndex, i, j);
			this->LoadTerrainObjcetInfo(nodeIndex, i, j);

			this->progress->SetProgress(float(nodeIndex/nTileRegionCnt)+0.3f);
		}
	}

	CLoadMapResInfo::GetInst()->MakeTerrainTexture();

	this->progress->SetProgress( 1.0f );
}

void sqr::CLoadSavedMapMgr::DealRegionSceneName()
{
	map<string, SceneRegion*>::iterator iter = CSceneRegionMgr::GetInst()->m_mapRegion.begin();
	map<string, SceneRegion*>::iterator eiter = CSceneRegionMgr::GetInst()->m_mapRegion.end();
	sqr::uint16 nSceneRgnIndex = 0;
	for ( iter; iter != eiter; ++iter )
	{
		sceneRgnIndexs[iter->first] = nSceneRgnIndex;
		++nSceneRgnIndex;
	}
}

bool sqr::CLoadSavedMapMgr::FileIsReadOnly( const string& strFileName )
{
	if ( IoSystem::GetInst()->IsReadOnly(strFileName))
	{
		string log = strFileName;
		log += "该文件已被他人使用";

		MessageBox( NULL, log.c_str() , "无法保存该文件", MB_OK );
		return true;
	}
	return false;
}