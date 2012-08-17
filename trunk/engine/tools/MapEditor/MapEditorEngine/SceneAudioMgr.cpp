#include "stdafx.h"
#include "SceneAudioMgr.h"
#include "IAudioModule.h"
#include "EditStateOpertiaonMgr.h"
#include "CWindowTarget.h"
#include "CCamera.h"
#include "CMapEditApp.h"
#include "TerrainMesh.h"
#include "TerrainMeshObject.h"
#include "ModelGroup.h"

namespace sqr
{
	string g_strDefaultMaterialName("");
	const float fSpeedOfSoundInit = 343.5f;
	const float fSpeedOfSoundStep = fSpeedOfSoundInit * 0.25f;

	CSceneAudioMgr::CSceneAudioMgr()
	{
		m_nCueRange				  = 0;
		m_nCurSeleteMaterialIndex = -1;
		m_strPhyleName			  = "";

		//RenderSystem中已经创建音效系统
		this->createAudioSystem();
	}

	CSceneAudioMgr::~CSceneAudioMgr()
	{
		m_vecIndividualSoundObjects.clear();

		if(GetAudioModule() && GetAudioModule()->GetAudioSystem() && GetAudioModule()->GetAudioSystem()->IsOpen())
		{
			GetAudioModule()->GetAudioSystem()->Close();
		}
	}

	void CSceneAudioMgr::createAudioSystem()
	{
		//如果音频系统已经启动 就直接返回
		if(GetAudioModule()->GetAudioSystem()->IsOpen())
			return;

		if (!GetAudioModule()->GetAudioSystem()->Open())
		{
			MessageBox(NULL,"AudioSystem打开失败","提示",MB_OK);
			return;
		}

		SQR_TRY
		{
			GetAudioModule()->GetAudioSystem()->LoadSoundBankFromXml("sound/config.xml");
		}
		SQR_CATCH(exp)
		{
			string str = exp.ErrorMsg();
			MessageBox(NULL, "sound/config.xml 载入失败", "提示", MB_OK);
		}
		SQR_TRY_END;
	}

	void CSceneAudioMgr::UpdateAudio()
	{
		CVector3f pos;
		CVector3f dir = CMainWindowTarget::GetInst()->GetCamera()->getDIRECTION();
		CVector3f up  = CMainWindowTarget::GetInst()->GetCamera()->getUP();

		if ( CEditStateOpertiaonMgr::GetInst()->GetEditState() == CEditStateOpertiaonMgr::EES_RUN )
			pos = CEditStateOpertiaonMgr::GetInst()->GetCurPlayerPosition();
		else
		{
			int nGridHot = CEditStateOpertiaonMgr::GetInst()->GetHotGridIndex();
			if( nGridHot == INVALID )
				return;

			SGrid &grid = CMapEditApp::GetInst()->GetEditingMesh()->GetGrid(nGridHot);
			pos = grid.vCenter;
		}

		static CVector3f lastdir, lastup, lastpos;

		if( lastpos != pos || lastdir != dir || lastup != up )
		{
			GetAudioModule()->GetAudioListener()->SetTransform(dir, up, pos);
		}

		lastpos = pos;
		lastup	= up;
		lastdir	= dir;
	}

	void CSceneAudioMgr::SetPhyleName( const string& strPhyleName, const string& strDefaultMatName )
	{
		m_strPhyleName			 = strPhyleName;
		g_strDefaultMaterialName = strDefaultMatName;
	}

	std::string CSceneAudioMgr::GetPhyleMaterialName( int8 index )
	{
		if( m_strPhyleName == "" )
			return "";

		string str = "";
		map<int8,string>::iterator iter = m_mapIndexMaterials.find(index);

		if( iter != m_mapIndexMaterials.end() )
			str = iter->second;

		if( str == "" )
			str = g_strDefaultMaterialName;

		str = str + "_" + m_strPhyleName;
		return str;
	}

	void CSceneAudioMgr::SetMaterialIndex( string& name, int8 index )
	{
		m_mapMaterialIndexs[name]  = index;
		m_mapIndexMaterials[index] = name;
	}

	int8 CSceneAudioMgr::GetMaterialIndex( string& name )
	{
		map<string,int8>::iterator iter = m_mapMaterialIndexs.find(name);
		if(	iter != m_mapMaterialIndexs.end() )
			m_nCurSeleteMaterialIndex = iter->second;
		else
			m_nCurSeleteMaterialIndex = -1;
		
		return m_nCurSeleteMaterialIndex;
	}

	string	CSceneAudioMgr::GetMaterialName(const int8 index) const
	{
		map<int8,string>::const_iterator iter = m_mapIndexMaterials.find(index);
		if( iter != m_mapIndexMaterials.end() )
			return iter->second;
		else
			return "";
	}

	void CSceneAudioMgr::SetTerMaterialIndex( vector<string>& names, int8 index )
	{
		size_t size = names.size();
		string name = "";

		for ( size_t i = 0; i < size; ++i )
		{
			name = names[i];
			m_mapTerMaterialsIndexs[name] = index;
		}
	}

	int8 CSceneAudioMgr::GetTerMaterialIndex( string& name )
	{
		map<string,int8>::iterator iter = m_mapTerMaterialsIndexs.find(name);
		if( iter != m_mapTerMaterialsIndexs.end() )
			return iter->second;
		else
			return -1;
	}

	void CSceneAudioMgr::CalTerrainMaterialIndex()
	{
		DWORD terrainCnt = 0, alpha = 0, alphaV0 = 0, alphaV1 = 0, alphaV2 = 0, alphaV3 = 0;;
		string texfirstname = "", texsecondname = "";
		map<string,int8>::iterator iter;

		CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

		terrainCnt = pTerrain->GetGridCount();
		for (DWORD i = 0; i < terrainCnt; ++i )
		{
			SGrid &grid = pTerrain->GetGrid(i);
			if( grid.bMatNotEffectByTileSets )
				continue;

			int8  nFristMatIndex = -1, nSecondMatIndex = -1;
			texfirstname = grid.nodeInfo.strTextureNames[0];
			texsecondname = grid.nodeInfo.strTextureNames[1];

			iter = this->m_mapTerMaterialsIndexs.find(texfirstname);
			if( iter != this->m_mapTerMaterialsIndexs.end() )
				nFristMatIndex = iter->second;

			iter = this->m_mapTerMaterialsIndexs.find(texsecondname);
			if( iter != this->m_mapTerMaterialsIndexs.end() )
				nSecondMatIndex = iter->second;

			SVertex &vertex0 = pTerrain->GetVertex(grid.dwVertexIndex[0]);
			SVertex &vertex1 = pTerrain->GetVertex(grid.dwVertexIndex[3]);
			SVertex &vertex2 = pTerrain->GetVertex(grid.dwVertexIndex[1]);
			SVertex &vertex3 = pTerrain->GetVertex(grid.dwVertexIndex[2]);

			alphaV0 = vertex0.oColor >> 24;
			alphaV1 = vertex1.oColor >> 24;
			alphaV2 = vertex2.oColor >> 24;
			alphaV3 = vertex3.oColor >> 24;

			alpha = (alphaV0 + alphaV1+ alphaV2 + alphaV3) / 4;

			///如果第一层占的比重大，就用第一层的纹理音效材质；否则用第二层的纹理音效材质
			if ( alpha < 127 )
				grid.nMaterialIndex = nFristMatIndex;
			else
				grid.nMaterialIndex = nSecondMatIndex;

			// 			if( grid.nMaterialIndex != -1 )
			// 				DebugOut("grid %d, %d, %s, %s\n", i, grid.nMaterialIndex, texfirstname.c_str(), texsecondname.c_str());
		}
	}

	void CSceneAudioMgr::SwitchSoundObjectsShow( bool bRenderSmallMap /*= false*/ )
	{
		static bool bHideModel = true;

		if( bRenderSmallMap == true )
			bHideModel = true;

		size_t size = m_vecIndividualSoundObjects.size();

		if( bHideModel )
		{
			for ( size_t i = 0; i < size; ++i )
			{
				CTObject * pT = m_vecIndividualSoundObjects[i];
				pT->GetModelGroup()->HideSelf();
			}
		}
		else
		{
			for ( size_t i = 0; i < size; ++i )
			{
				CTObject * pT = m_vecIndividualSoundObjects[i];
				pT->GetModelGroup()->ShowSelf();
			}
		}

		bHideModel = !bHideModel;
	}

	void CSceneAudioMgr::SwitchSoundPlay()
	{
		static bool bStopSound = true;

		size_t size = m_vecIndividualSoundObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CTObject * pT = m_vecIndividualSoundObjects[i];
			pT->GetModelGroup()->StopSceneCue(bStopSound);
		}

		bStopSound = !bStopSound;
	}

	void CSceneAudioMgr::ChangeSoundRange()
	{
		size_t size = m_vecIndividualSoundObjects.size();
		for ( size_t i = 0; i < size; ++i )
		{
			CTObject * pT = m_vecIndividualSoundObjects[i];
			//pT->GetModelGroup()->UpdateBoundingBox(true);
		}
	}

	void CSceneAudioMgr::SetSpeedOfSoundVariable( float fValue )
	{
		IAudioSystem *audioSystem = GetAudioModule()->GetAudioSystem();
		if( audioSystem )
		{
			audioSystem->SetSpeedOfSoundVariable(fValue);
		}
	}

	void CSceneAudioMgr::SetAmbientVolume( const float fValue )
	{
		IAudioSystem *audioSystem = GetAudioModule()->GetAudioSystem();
		if( audioSystem )
			audioSystem->SetGlobalVariable("AmbientVolume",  fValue);
	}
}