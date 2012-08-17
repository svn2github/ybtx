#include "StdAfx.h"
#include "SceneRegionMgr.h"
#include "CMapEditApp.h"
#include "Operator.h"
#include "Operator.inl"
#include "TerrainMesh.h"
#include "TerrainAlgorithm.h"
#include "BaseHelper.h"
#include "TraceHelper.h"
#include "SceneEnvMgr.h"
#include "SceneEnvMgr.inl"
#include "CRegionAmbientEffectMgr.h"
#include "ToolHideTileRegionInView.h"
#include "ToolRegion.h"

//------------------------------------------------------------------------------
namespace sqr
{
	const float compareValue = GRID_SPACE * REGION_DIVIDESPACE;

enum
{
	ADD_FOG_VERSION = 0,
	ADD_SPECULAR_VERSION = 1,
	SPECULAR_POWER_VERSION = 2,
	AMBIENT_EFFECT_VERSION = 3,
};

#define LOCAL_SCENEREGIONVERSION 0x54
#define LOCAL_SCENEREGION_ADD_VERSION 0x55 //增加了渲染区域索引设置读写
#define LOCAL_NEW_SCENEREGION_ADD_VERSION 0x56 //增加了渲染区域索引设置新的读写

CSceneRegionMgr::CSceneRegionMgr()
{
}

CSceneRegionMgr::~CSceneRegionMgr()
{
	Release();
}

void CSceneRegionMgr::Release()
{
	SafeDelCtn2(m_mapRegion);
}

void CSceneRegionMgr::AddSceneRegion(const string &name, SceneRegion *sceneRegion)
{
	m_mapRegion.insert(make_pair(name, sceneRegion));
}

void CSceneRegionMgr::AddSceneRegion( const string& name )
{
	SceneRegion *pCurSceneRegion = new SceneRegion;

 	SceneRegion *pDefaultSceneRgn = NULL;
 	pDefaultSceneRgn = this->m_mapRegion["default"];
 	if( pDefaultSceneRgn )
 	{
		pCurSceneRegion->lightParam.globalSpecular = 0;
		pCurSceneRegion->lightParam.specularPower  = 5.0f;
		pCurSceneRegion->lightParam = pDefaultSceneRgn->lightParam;
		float fFogDensity = pDefaultSceneRgn->GetSceneRgnFogDensity();
		pCurSceneRegion->SetFogDensity(fFogDensity);
 	}

	m_mapRegion.insert(make_pair(name, pCurSceneRegion));
}

bool CSceneRegionMgr::DeleteSceneRegion(const string &name)
{
	map<string, SceneRegion*>::iterator iter = m_mapRegion.find(name);

	if ( iter != m_mapRegion.end() )
	{
		set<int> deletergns = iter->second->_RegionUnitList;

		vector<int> vecdeletergns;
		set<int>::iterator beg = deletergns.begin();
		set<int>::iterator end = deletergns.end();
		for ( beg; beg != end; ++beg )
		{
			vecdeletergns.push_back(*beg);
		}
		bool bDelete = CToolRegion::GetInst()->CanleSecneRegion(vecdeletergns);
		if( !bDelete )
			return bDelete;

		delete iter->second;

		iter->second = NULL;

		m_mapRegion.erase(iter);

		this->ReBackRegionInfo(deletergns);
		return true;
	}
	else
		return false;
}

SceneRegion* CSceneRegionMgr::GetSceneRegion(const string &name)
{
	map<string, SceneRegion*>::iterator iter = m_mapRegion.find(name);

	if ( iter != m_mapRegion.end() )
		return iter->second;
	else
		return NULL;
}

bool CSceneRegionMgr::AddSceneRegionUnits(const string& name, set<int> &units)
{
	SceneRegion *sceneRegion = GetSceneRegion( name );

	if ( sceneRegion )
	{
		for ( set<int>::iterator iter = units.begin(); iter != units.end(); ++iter )
			sceneRegion->_RegionUnitList.insert(*iter);

		this->SetRegionIsSetEnv( units, true );

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::AddSceneRegionUnit(const string& name, const int& unit)
{
	SceneRegion *sceneRegion = GetSceneRegion( name );

	if ( sceneRegion )
	{
		if ( sceneRegion->_RegionUnitList.find(unit) == sceneRegion->_RegionUnitList.end() )
			sceneRegion->_RegionUnitList.insert(unit);

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::DeleteSceneRegionUnit(const string& name, const int& unit)
{
	SceneRegion *sceneRegion= GetSceneRegion( name );
	set<int> deletergns;

	if ( sceneRegion )
	{
		set<int>::iterator iter = sceneRegion->_RegionUnitList.find(unit);

		if ( iter != sceneRegion->_RegionUnitList.end() )
		{
			deletergns.insert(unit);

			sceneRegion->_RegionUnitList.erase(iter);

			this->ReBackRegionInfo(deletergns);

			return true;
		}
		else
			return false;
	}
	else
		return false;
}

bool CSceneRegionMgr::DeleteSceneRegionUnits( const string& name, set<int>& units )
{
	SceneRegion *sceneRegion = GetSceneRegion( name );
	int unit = 0;

	if ( sceneRegion )
	{
		set<int>::iterator iter, diter;

		for ( diter = units.begin(); diter != units.end(); ++diter )
		{
			unit = *diter;
			iter = sceneRegion->_RegionUnitList.find(unit);

			if ( iter != sceneRegion->_RegionUnitList.end() )
			{
				sceneRegion->_RegionUnitList.erase(iter);
			}
			else
				continue;
		}

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::SetSceneRgnMusicName( const string& rgnname, const string& musicname )
{
	SceneRegion *sceneRegion = GetSceneRegion( rgnname );

	if ( sceneRegion )
	{
		sceneRegion->SetSceneRgnMusicName(musicname);
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, rgnname);

		return true;
	}
	else
		return false;
}

void CSceneRegionMgr::SetSceneRgnAmbient(const string& name, const DWORD color)
{
	SceneRegion *sceneRegion = GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.globalAmbient = color;
	}
}

DWORD CSceneRegionMgr::GetSceneRgnAmbient( const string& name )
{
	SceneRegion *sceneRegion = GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.globalAmbient;
	return 0;
}

void CSceneRegionMgr::SetSceneRgnDiffuse(const string& name, const DWORD color)
{
	SceneRegion *sceneRegion =  this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.globalDiffuse = color;
	}
}

void CSceneRegionMgr::SetSceneRgnSpecular( const string& name, DWORD color)
{
	SceneRegion *sceneRegion =  this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.globalSpecular = color;
	}
}

void CSceneRegionMgr::SetSceneRgnSpecularPower( const string& name, float fPower)
{
	SceneRegion *sceneRegion =  this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.specularPower = fPower;
	}
}

DWORD CSceneRegionMgr::GetSceneRgnDiffuse( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.globalDiffuse;
	return 0;
}

DWORD CSceneRegionMgr::GetSceneRgnSpecular( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.globalSpecular;
	return 0;
}

float CSceneRegionMgr::GetSceneRgnSpecularPower( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.specularPower;
	return 0.0f;
}

void CSceneRegionMgr::SetPlayerRgnAmbient(const string& name, const DWORD color)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.playerAmbient = color;
	}
}

DWORD CSceneRegionMgr::GetPlayerRgnAmbient( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.playerAmbient;
	return 0;
}

void CSceneRegionMgr::SetPlayerRgnDiffuse(const string& name, const DWORD color)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.playerDiffuse = color;
	}
}

void CSceneRegionMgr::SetPlayerRgnSpecular( const string& name, DWORD color)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.playerSpecular = color;
	}
}

DWORD CSceneRegionMgr::GetPlayerRgnDiffuse( const string& name)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.playerDiffuse;
	return 0;
}

DWORD CSceneRegionMgr::GetPlayerRgnSpecular( const string& name)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.playerSpecular;
	return 0;
}

DWORD CSceneRegionMgr::GetSceneRgnFog( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.fogColor;
	return 0;
}

string CSceneRegionMgr::GetSceneRgnMusicName(const string& name)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->GetSceneRgnMusicName();
	else
		return "";
}

void CSceneRegionMgr::SetSceneRgnFog(const string& name, const DWORD color)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.fogColor = color;
	}
}

inline bool is_black(const CColor& color)
{
	return (0 == color.R) && (0 == color.G) && (0 == color.B);
}

void CSceneRegionMgr::SaveSceneRegionInfo(FILE *fp, bool bSplit)
{
	COperator *pOperator = CMapEditApp::GetInst()->GetOperator();
	if ( pOperator == NULL )
		return;

	bool bResizeMap = pOperator->GetIsResizeMap();

	DWORD ambient = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
	DWORD diffuse = CSceneEnvMgr::GetInst()->GetSceneDirColor();
	DWORD shadow = CSceneEnvMgr::GetInst()->GetShadowColor();
	DWORD playerambient = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
	DWORD playerdiffuse = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
	DWORD fog = CSceneEnvMgr::GetInst()->GetFogColor();
	DWORD sceneSpecular = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
	DWORD playerSpecular = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();

	map<int, int>::iterator beg;
	// version
	DWORD dwVersion = LOCAL_NEW_SCENEREGION_ADD_VERSION;
	fwrite(&dwVersion,sizeof(DWORD),1,fp);

	size_t size = m_mapRegion.size();
	fwrite(&size,sizeof(size_t),1,fp);

	if ( size == 0 )
		return;
	map<string, SceneRegion*>::iterator iter = m_mapRegion.begin();
	for ( iter; iter != m_mapRegion.end(); ++iter )
	{
		SceneRegion *sceneRegion = iter->second;

		//场景区域名
		const string& strSceneRgnName = iter->first;
		if( bSplit && strSceneRgnName != "default" )
			continue;

		int NameLen = strSceneRgnName.length();
		fwrite(&NameLen,sizeof(int),1,fp);
		fwrite(strSceneRgnName.c_str(),NameLen,1,fp);

		//该区域环境光值
		CColor color = sceneRegion->lightParam.globalAmbient;
		if ( is_black(color) )
			color = ambient;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);

		//DebugOut("save ambient %s %u\n", strSceneRgnName.c_str(), color);
		color = sceneRegion->lightParam.globalDiffuse;
		if ( is_black(color) )
			color = diffuse;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);
		//DebugOut("save diffuse %s %u\n", strSceneRgnName.c_str(), color);

		color = sceneRegion->lightParam.globalShadow;
		//DebugOut("save shadow %s %u\n", strSceneRgnName.c_str(), color);
		if ( is_black(color) )
			color = shadow;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);

		color = sceneRegion->lightParam.playerAmbient;
		if ( is_black(color) )
			color = playerambient;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);

		color = sceneRegion->lightParam.playerDiffuse;
		if ( is_black(color) )
			color = playerdiffuse;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);

		color = sceneRegion->lightParam.fogColor;
		if ( is_black(color) )
			color = fog;
		fwrite(&color.R,sizeof(BYTE),1,fp);
		fwrite(&color.G,sizeof(BYTE),1,fp);
		fwrite(&color.B,sizeof(BYTE),1,fp);

		///region music
		string musicName = sceneRegion->GetSceneRgnMusicName();
		NameLen = musicName.length();
		fwrite(&NameLen,sizeof(int),1,fp);
		fwrite(musicName.c_str(),NameLen,1,fp);

		size = sceneRegion->_RegionUnitList.size();
		fwrite(&size,sizeof(size_t),1,fp);

		int rgnUnit = 0;
		for ( set<int>::iterator unit = sceneRegion->_RegionUnitList.begin(); unit != sceneRegion->_RegionUnitList.end(); ++unit )
		{
			rgnUnit = *unit;

			if( bResizeMap )
			{
				beg = pOperator->m_OldMapRegionIndexMap.find(rgnUnit);
				if( beg != pOperator->m_OldMapRegionIndexMap.end() )
					rgnUnit = beg->second;
			}

			fwrite(&rgnUnit, sizeof(rgnUnit), 1, fp);
		}

		///add fog param
		DWORD dwNewVersion = AMBIENT_EFFECT_VERSION;
		fwrite(&dwNewVersion,sizeof(dwNewVersion),1,fp);

		///region ambient effect
		SaveRegionAmbinetFx(fp, sceneRegion);

		//////////////////////////////////////////////////////////////////////////
		float fPower = sceneRegion->lightParam.specularPower;
		fwrite(&fPower,sizeof(fPower),1,fp);
		//////////////////////////////////////////////////////////////////////////

		///specular
		SaveRegionSpecularParam(fp, sceneRegion, sceneSpecular, playerSpecular);

		///fog
		SaveRegionFogParam(fp, sceneRegion);
	}

	//save 
	CToolHideTileRegionInView::GetInst()->SaveTileRegionInfo(fp);
}

void CSceneRegionMgr::LoadSceneRegionInfo(FILE *fp)
{

	DWORD version = 0;
	///读取第一个标志位
	fread(&version,sizeof(DWORD),1,fp);

	///读取旧的文件格式
	if ( version < LOCAL_SCENEREGIONVERSION )
	{
		LoadSceneRegionOldFile(fp);
	}
	else
	{
		///读取新的文件格式
		LoadSceneRegionNewFile(fp);

		if( version == LOCAL_SCENEREGION_ADD_VERSION )
		{
			//read
			CToolHideTileRegionInView::GetInst()->LoadOldTileRegionInfo(fp);
		}
		else
		{

			if( version == LOCAL_NEW_SCENEREGION_ADD_VERSION )
			{
				//read
				CToolHideTileRegionInView::GetInst()->LoadTileRegionInfo(fp);
			}
		}
	}
}

void CSceneRegionMgr::LoadSceneRegionNewFile(FILE *fp)
{
	string strSceneRgnName = "", strRgnMusicName = "";
	int NameLen = 0, rgnUnit = 0;
	float x = 0.0f, y = 0.0f, z = 0.0f;
	size_t size = 0, unitsize = 0;
	BYTE r = 0, g = 0, b = 0;

	fread(&size,sizeof(size_t),1,fp);
	for ( size_t i = 0; i < size; ++i )
	{
		SceneRegion *sceneRegion = NULL;
		sceneRegion = new SceneRegion;

		fread(&NameLen,sizeof(DWORD),1,fp);
		strSceneRgnName.resize(NameLen);
		fread((TCHAR*)strSceneRgnName.data(),NameLen,1,fp);
		sceneRegion->SetSceneRgnName(strSceneRgnName);

		this->AddSceneRegion(strSceneRgnName, sceneRegion);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.globalAmbient = CColor(0, r, g, b);

		DebugOut("load ambient %s, %d, %d, %d\n", strSceneRgnName.c_str(), r, g, b);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.globalDiffuse = CColor(0, r, g, b);

		DebugOut("load diffuse %s, %d, %d, %d\n", strSceneRgnName.c_str(), r, g, b);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.globalShadow = CColor(0, r, g, b);

		DebugOut("load shadow %s, %d, %d, %d\n", strSceneRgnName.c_str(), r, g, b);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.playerAmbient = CColor(0, r, g, b);
		DebugOut("load player ambient %s, %d, %d, %d\n", strSceneRgnName.c_str(), r, g, b);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.playerDiffuse = CColor(0, r, g, b);
		DebugOut("load player diffuse %s, %d, %d, %d\n", strSceneRgnName.c_str(), r, g, b);

		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.fogColor = CColor(0, r, g, b);

		///region music
		fread(&NameLen,sizeof(DWORD),1,fp);
		strRgnMusicName.resize(NameLen);
		fread((TCHAR*)strRgnMusicName.data(),NameLen,1,fp);
		sceneRegion->SetSceneRgnMusicName(strRgnMusicName);

		fread(&unitsize,sizeof(size_t),1,fp);
		for ( size_t j = 0; j < unitsize; ++j )
		{
			fread(&rgnUnit, sizeof(int), 1, fp);

			sceneRegion->_RegionUnitList.insert(rgnUnit);
		}

		///add fog prarm
		DWORD dwVersion;
		fread(&dwVersion,sizeof(dwVersion),1,fp);
		switch (dwVersion)
		{
		case AMBIENT_EFFECT_VERSION:
			LoadRegionAmbinetFx(fp, sceneRegion);

		case SPECULAR_POWER_VERSION:
			{
				float fPower = 0.0f;
				fread(&fPower,sizeof(fPower),1,fp);
				sceneRegion->lightParam.specularPower = fPower;
			}

		case ADD_SPECULAR_VERSION:
			LoadRegionSpecularParam(fp, sceneRegion);

		case ADD_FOG_VERSION:
			LoadRegionFogParam(fp, sceneRegion);
			break;

		default:
			break;
		}

		this->SetLoadInfo(sceneRegion->_RegionUnitList, strSceneRgnName);
	}
}

///区域缩小四分之一
void CSceneRegionMgr::LoadSceneRegionOldFile(FILE *fp)
{
	string strSceneRgnName = "", strRgnMusicName = "";
	int NameLen = 0, rgnUnit = 0;
	float x = 0.0f, y = 0.0f, z = 0.0f;
	size_t size = 0, unitsize = 0;
	BYTE r = 0, g = 0, b = 0;

	fread(&size,sizeof(size_t),1,fp);
	for ( size_t i = 0; i < size; ++i )
	{
 		SceneRegion *sceneRegion = new SceneRegion;
 
 		fread(&NameLen,sizeof(DWORD),1,fp);
 		strSceneRgnName.resize(NameLen);
 		fread((TCHAR*)strSceneRgnName.data(),NameLen,1,fp);
 		sceneRegion->SetSceneRgnName(strSceneRgnName);
 
 		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.globalAmbient = CColor(0, r, g, b);
 
		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.globalDiffuse = CColor(0, r, g, b);
		sceneRegion->lightParam.globalShadow = CSceneEnvMgr::GetInst()->GetShadowColor();
 
		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.playerAmbient = CColor(0, r, g, b);
 
		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.playerDiffuse = CColor(0, r, g, b);
 
		fread(&r,sizeof(BYTE),1,fp);
		fread(&g,sizeof(BYTE),1,fp);
		fread(&b,sizeof(BYTE),1,fp);
		sceneRegion->lightParam.fogColor = CColor(0, r, g, b);
 
 		///region music
 		fread(&NameLen,sizeof(DWORD),1,fp);
 		strRgnMusicName.resize(NameLen);
 		fread((TCHAR*)strRgnMusicName.data(),NameLen,1,fp);
 		sceneRegion->SetSceneRgnMusicName(strRgnMusicName);
 
 		fread(&unitsize,sizeof(size_t),1,fp);
 		for ( size_t j = 0; j < unitsize; ++j )
 		{
 			fread(&rgnUnit, sizeof(int), 1, fp);

			sceneRegion->_RegionUnitList.insert(rgnUnit);
 		}

		///add fog prarm
		DWORD dwVersion;
		fread(&dwVersion,sizeof(dwVersion),1,fp);
		switch (dwVersion)
		{
		case SPECULAR_POWER_VERSION:
			{
				float fPower = 0.0f;
				fread(&fPower,sizeof(fPower),1,fp);
				sceneRegion->lightParam.specularPower = fPower;
			}

		case ADD_SPECULAR_VERSION:
			LoadRegionSpecularParam(fp, sceneRegion);

		case ADD_FOG_VERSION:
			LoadRegionFogParam(fp, sceneRegion);
			break;

		default:
			break;
		}
 
 		this->AddSceneRegion(strSceneRgnName, sceneRegion);
 
 		this->SetLoadInfo(sceneRegion->_RegionUnitList, strSceneRgnName);
	}
}

bool CSceneRegionMgr::DecideRegionIsSet(const int& regionindex, string& rgnName)
{
	map<string, SceneRegion*>::iterator iter = m_mapRegion.begin();
	set<int>::iterator iiter;

	for ( iter; iter != m_mapRegion.end(); ++iter )
	{
		SceneRegion* sceneRegion = NULL;

		sceneRegion = this->GetSceneRegion(iter->first);

		if ( sceneRegion )
		{
			iiter = sceneRegion->_RegionUnitList.find(regionindex);

			if ( iiter != sceneRegion->_RegionUnitList.end() )
			{
				rgnName = iter->first;
				return true;
			}
		}
	}

	return false;
}

bool CSceneRegionMgr::SetFogType( const string& name,  const uint8 fogType )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		sceneRegion->SetFogType(fogType);

		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::SetFogDensity( const string& name,  const float density )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		sceneRegion->SetFogDensity(density);

		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::SetFogStart( const string& name,  const float start )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		sceneRegion->SetFogStart(start);

		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);

		return true;
	}
	else
		return false;
}

bool CSceneRegionMgr::SetFogEnd( const string& name,  const float end )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		sceneRegion->SetFogEnd(end);

		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);

		return true;
	}
	else
		return false;
}

uint8 CSceneRegionMgr::GetSceneRgnFogType( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->GetSceneRgnFogType();
	else
		return 0;
}

float CSceneRegionMgr::GetSceneRgnFogDensity( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->GetSceneRgnFogDensity();
	else
		return 0.0f;
}

float CSceneRegionMgr::GetSceneRgnFogStart( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->GetSceneRgnFogStart();
	else
		return 0.0f;
}

float CSceneRegionMgr::GetSceneRgnFogEnd( const string& name ) 
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->GetSceneRgnFogEnd();
	else
		return 0.0f;
}

void CSceneRegionMgr::InsertRegionEffect( const string& regname, const string& effectname )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
		sceneRegion->InsertRegionEffect(effectname);
}

void CSceneRegionMgr::DeleteRegionEffect( const string& regname, const string& effectname )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
	{
		sceneRegion->DeleteRegionEffect(effectname);

		CMapRegionAmbientEffectMgr::GetInst()->DeleteRgnAmbientFxObject(regname, effectname);
	}
}

void CSceneRegionMgr::SetAmientFxPlayTime( const string& regname, const string& effectname, const DWORD nTimeMin, const DWORD nTimeMax )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
	{
		sceneRegion->SetAmientFxPlayTime(effectname, nTimeMin, nTimeMax);
		CMapRegionAmbientEffectMgr::GetInst()->SetAmientFxPlayTime(regname, effectname, nTimeMin, nTimeMax);
	}
}

void CSceneRegionMgr::GetAmientFxPlayTime( const string& regname, const string& effectname, DWORD& nTimeMin, DWORD& nTimeMax )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
		sceneRegion->GetAmientFxPlayTime(effectname, nTimeMin, nTimeMax);
}

void CSceneRegionMgr::SetAmientFxIntervalTime( const string& regname, const string& effectname, const DWORD dwIntervalTimeMin, const DWORD dwIntervalTimeMax  )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
	{
		sceneRegion->SetAmientFxIntervalTime(effectname, dwIntervalTimeMin, dwIntervalTimeMax);
		CMapRegionAmbientEffectMgr::GetInst()->SetAmientFxIntervalTime(regname, effectname, dwIntervalTimeMin, dwIntervalTimeMax);
	}
}

void CSceneRegionMgr::GetAmientFxIntervalTime( const string& regname, const string& effectname, DWORD& dwIntervalTimeMin, DWORD& dwIntervalTimeMax )
{
 	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
 	if ( sceneRegion )
 		sceneRegion->GetAmientFxIntervalTime(effectname, dwIntervalTimeMin, dwIntervalTimeMax);
}

vector<string> CSceneRegionMgr::GetRegionEffectNameList(const string& regname)
{
	SceneRegion *sceneRegion = this->GetSceneRegion( regname );
	if ( sceneRegion )
		return sceneRegion->GetRegionEffectNameList();

	static vector<string> nullNames;
	return nullNames;
}

void CSceneRegionMgr::SaveRegionFogParam( FILE * fp, SceneRegion *pSceneRgn )
{
	uint8 nFogType    = pSceneRgn->GetSceneRgnFogType();
	float fFogStart   = pSceneRgn->GetSceneRgnFogStart();
	float fFogEnd	  = pSceneRgn->GetSceneRgnFogEnd();
	float fFogDensity = pSceneRgn->GetSceneRgnFogDensity();

	fwrite(&nFogType,    sizeof(nFogType),    1, fp);
	fwrite(&fFogStart,   sizeof(fFogStart),   1, fp);
	fwrite(&fFogEnd,	 sizeof(fFogEnd),	  1, fp);
	fwrite(&fFogDensity, sizeof(fFogDensity), 1, fp);
}

void CSceneRegionMgr::SaveRegionAmbinetFx( FILE * fp, SceneRegion *pSceneRgn )
{
	RegionAmbientFxMap regionEffectUnitMap = pSceneRgn->GetRgnAmbinetFxList();
	size_t size = regionEffectUnitMap.size();
	fwrite(&size, sizeof(size), 1, fp);

	RegionAmbientFxMap::iterator beg = regionEffectUnitMap.begin();
	RegionAmbientFxMap::iterator end = regionEffectUnitMap.end();
	for ( beg; beg != end; ++beg )
	{
		sRegAmbientFxUnit *pTemp = beg->second;

		string strFxName = pTemp->strEffectName;
		int NameLen = strFxName.length();
		fwrite(&NameLen, sizeof(int), 1, fp);
		fwrite(strFxName.c_str(), NameLen, 1 ,fp);

		DWORD dwPlayMin = pTemp->dwPlayTimeMin;
		fwrite(&dwPlayMin, sizeof(dwPlayMin), 1, fp);

		DWORD dwPlayMax = pTemp->dwPlayTimeMax;
		fwrite(&dwPlayMax, sizeof(dwPlayMax), 1, fp);

		DWORD dwIntervalMin = pTemp->dwPlayIntervalMin;
		fwrite(&dwIntervalMin, sizeof(dwIntervalMin), 1, fp);

		DWORD dwIntervalMax = pTemp->dwPlayIntervalMax;
		fwrite(&dwIntervalMax, sizeof(dwIntervalMax), 1, fp);
	}
}

void CSceneRegionMgr::SaveRegionSpecularParam( FILE * fp, SceneRegion *pSceneRgn, const DWORD sceneSpecular, const DWORD playerSpecular )
{
	CColor color = pSceneRgn->lightParam.globalSpecular;
	if ( is_black(color) )
		color = sceneSpecular;
	fwrite(&color.R,sizeof(BYTE),1,fp);
	fwrite(&color.G,sizeof(BYTE),1,fp);
	fwrite(&color.B,sizeof(BYTE),1,fp);

	color = pSceneRgn->lightParam.playerSpecular;
	if ( is_black(color) )
		color = playerSpecular;
	fwrite(&color.R,sizeof(BYTE),1,fp);
	fwrite(&color.G,sizeof(BYTE),1,fp);
	fwrite(&color.B,sizeof(BYTE),1,fp);
}

void CSceneRegionMgr::LoadRegionSpecularParam(FILE * fp, SceneRegion *pSceneRgn)
{
	BYTE r = 0, g = 0, b = 0;

	fread(&r,sizeof(BYTE),1,fp);
	fread(&g,sizeof(BYTE),1,fp);
	fread(&b,sizeof(BYTE),1,fp);
	pSceneRgn->lightParam.globalSpecular = CColor(0, r, g, b);

	fread(&r,sizeof(BYTE),1,fp);
	fread(&g,sizeof(BYTE),1,fp);
	fread(&b,sizeof(BYTE),1,fp);
	pSceneRgn->lightParam.playerSpecular = CColor(0, r, g, b);
}

void CSceneRegionMgr::LoadRegionFogParam( FILE * fp, SceneRegion *pSceneRgn )
{
	uint8 nFogType  = 0;
	float fFogStart = 0.0f, fFogEnd	 = 0.0f, fFogDensity = 0.0f;

	float fEnvFogStart   = CSceneEnvMgr::GetInst()->GetFogStart();
	float fEnvFogEnd     = CSceneEnvMgr::GetInst()->GetFogEnd();
	float fEnvFogDensity = CSceneEnvMgr::GetInst()->GetDensity();

	fread(&nFogType,    sizeof(nFogType),    1, fp);
	fread(&fFogStart,   sizeof(fFogStart),   1, fp);
	fread(&fFogEnd,		sizeof(fFogEnd),	  1, fp);
	fread(&fFogDensity, sizeof(fFogDensity), 1, fp);

	if( fFogStart == 17.0f )
		fFogStart = fEnvFogStart;

	if( fFogEnd == 26.0f )
		fFogEnd = fEnvFogEnd;

	if( fFogDensity == 16.0f )
		fFogDensity = fEnvFogDensity;

	pSceneRgn->SetFogType(nFogType);
	pSceneRgn->SetFogStart(fFogStart);
	pSceneRgn->SetFogEnd(fFogEnd);
	pSceneRgn->SetFogDensity(fFogDensity);
}

void CSceneRegionMgr::LoadRegionAmbinetFx(FILE * fp, SceneRegion *pSceneRgn)
{
	size_t size = 0;
	fread(&size,sizeof(size_t),1,fp);
	for ( size_t i = 0; i < size; ++i )
	{
		string strFxName = "";
		int NameLen = 0;
		DWORD dwPlayMin = 0, dwPlayMax = 0, dwIntervalMin = 0, dwIntervalMax = 0;

		fread(&NameLen, sizeof(NameLen), 1, fp);
		strFxName.resize(NameLen);
		fread((TCHAR*)strFxName.data(), NameLen, 1, fp);

		fread(&dwPlayMin, sizeof(dwPlayMin), 1, fp);
		fread(&dwPlayMax, sizeof(dwPlayMax), 1, fp);
		fread(&dwIntervalMin, sizeof(dwIntervalMin), 1, fp);
		fread(&dwIntervalMax, sizeof(dwIntervalMax), 1, fp);

		CMapRegionAmbientEffectMgr::GetInst()->CreateRgnAmbientFxObject(pSceneRgn->GetSceneRgnName(), strFxName);
 		pSceneRgn->SetAmientFxPlayTime(strFxName, dwPlayMin, dwPlayMax);
 		pSceneRgn->SetAmientFxIntervalTime(strFxName, dwIntervalMin, dwIntervalMax);
	}
}

//------------------------------------------------------------------------------
void
CSceneRegionMgr::SetSceneRgnShadow( const string& name, DWORD color )
{
	SceneRegion *sceneRegion =  this->GetSceneRegion( name );

	if ( sceneRegion )
	{
		this->SetSceneRgnIndexName(sceneRegion->_RegionUnitList, name);
		sceneRegion->lightParam.globalShadow = color;
	}
}

//------------------------------------------------------------------------------
DWORD
CSceneRegionMgr::GetSceneRgnShadow( const string& name )
{
	SceneRegion *sceneRegion = this->GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->lightParam.globalShadow;
	return 0;
}

//------------------------------------------------------------------------------
set<int>
CSceneRegionMgr::GetCurrentRegions( const string& name )
{
	SceneRegion *sceneRegion = NULL;

	sceneRegion = GetSceneRegion( name );

	if ( sceneRegion )
		return sceneRegion->_RegionUnitList;
	else
	{
		set<int> returnSet;

		return returnSet;
	}
}

//------------------------------------------------------------------------------
void
CSceneRegionMgr::CalcRegionLightByRegionDistance(CVector3f v)
{
	COperator *pOperator = NULL;
	pOperator = CMapEditApp::GetInst()->GetOperator();
	if ( pOperator == NULL )
		return;

	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetTerrain();
	if ( pTerrainMesh == NULL )
		return;

	if ( CSceneRegionMgr::GetInst()->SceneRegionsIsEmpty() )
		return;

	int hotRegionIndex = pTerrainMesh->GetRegionHot();
	int nHotGridIndex = pOperator->GetGridIndexHot();
	int dwWidthInRegion = pTerrainMesh->GetWidthInRegion();
	int dwDepthInRegion = pTerrainMesh->GetDepthInRegion();
	int regionLimit = dwDepthInRegion * dwWidthInRegion;
	DWORD EnvAmbient = CSceneEnvMgr::GetInst()->GetSceneAmbientColor();
	DWORD EnvDiffuse = CSceneEnvMgr::GetInst()->GetSceneDirColor();
	DWORD EnvSpecular = CSceneEnvMgr::GetInst()->GetSceneSpecurlarColor();
	DWORD EnvPlayerAmbient = CSceneEnvMgr::GetInst()->GetPlayerAmbientColor();
	DWORD EnvPlayerDiffuse = CSceneEnvMgr::GetInst()->GetPlayerDirColor();
	DWORD EnvPlayerSpecular = CSceneEnvMgr::GetInst()->GetPlayerSpecurlarColor();
	DWORD EnvGloalShadow    = CSceneEnvMgr::GetInst()->GetShadowColor();
	DWORD EnvFog = CSceneEnvMgr::GetInst()->GetFogColor();
	float fEnvFogSatrt = CSceneEnvMgr::GetInst()->GetFogStart();
	float fEnvFogEnd = CSceneEnvMgr::GetInst()->GetFogEnd();
	float fEnvFogDensity = CSceneEnvMgr::GetInst()->GetDensity();
	float fSpecularPower = CSceneEnvMgr::GetInst()->GetSpecularReflectionPower(0);
	string strEnvBackMusicName = CSceneEnvMgr::GetInst()->GetBackGroundMusic();

	SRegion &hotRegion = pTerrainMesh->GetRegion(hotRegionIndex);

	SGrid &hotGrid = pTerrainMesh->GetGrid(nHotGridIndex);
	int nHotDivideRgnIndex = hotGrid.m_nDivideRegionIndex;

	vector<DWORD> regionlist = pTerrainMesh->_RegionAjenctList[nHotDivideRgnIndex];

	size_t size = regionlist.size();
	bool b = false;

	for ( size_t i = 0; i < size; ++i )
	{
		SDivideRegion *pDivideRegion = pTerrainMesh->GetDivideRegion(regionlist[i]);
		b = pDivideRegion->m_bSetRegionEnv || b;
	}

	SDivideRegion *pDivideRegion = pTerrainMesh->GetDivideRegion(nHotDivideRgnIndex);
	if( pDivideRegion->m_bSetRegionEnv )
	{
		SceneRegion *pSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion(pDivideRegion->m_strRegionName);

		string strRegionMusicName = pSceneRgn->GetSceneRgnMusicName();
		CSceneEnvMgr::GetInst()->SetBackGroundMusic(strRegionMusicName, true);
	}
	else
		CSceneEnvMgr::GetInst()->SetBackGroundMusic(strEnvBackMusicName, false);

	//有一个设置了也要进行下面的计算
	if ( !b )
	{
		CSceneEnvMgr::GetInst()->RgnReSetPlayerColor(EnvPlayerAmbient, EnvPlayerDiffuse);
		CSceneEnvMgr::GetInst()->RgnReSetColorInfo(EnvAmbient, EnvDiffuse, EnvFog);
		CSceneEnvMgr::GetInst()->RgnReSetFogParam(3, fEnvFogSatrt, fEnvFogEnd, fEnvFogDensity);
		CSceneEnvMgr::GetInst()->RgnReSetSpecularColorInfo(EnvSpecular, EnvPlayerSpecular, fSpecularPower);
		return;
	}

	RegionLightParam lightParam;
	RegionLightParam maxLightParam;
	bool isChanged = false;
	for ( size_t i = 0; i < size; ++i )
	{
		SDivideRegion *pDivideRegion =  pTerrainMesh->GetDivideRegion(regionlist[i]);

		if ( !pDivideRegion )
			continue;
		string strReginName = pDivideRegion->m_strRegionName;

		SceneRegion *pSceneRgn = CSceneRegionMgr::GetInst()->GetSceneRegion(strReginName);
		if ( !pSceneRgn )
			continue;
		float x = pDivideRegion->m_vCenter.x - v.x;
		float z = pDivideRegion->m_vCenter.z - v.z;
		float dis = sqrt(x * x + z * z);

		float ratio = 1 - dis / compareValue;
		if ( ratio <= 0.0f )
			continue;

		lightParam += pSceneRgn->lightParam * ratio;
		maxLightParam = RegionLightParam::Max(pSceneRgn->lightParam, maxLightParam);
		isChanged = true;
	}

	if (isChanged)
	{
		lightParam = RegionLightParam::Min(lightParam, maxLightParam);

		CSceneEnvMgr::GetInst()->RgnReSetColorInfo(lightParam.globalAmbient, lightParam.globalDiffuse, lightParam.fogColor);
		//DebugOut("ambient %d, %d, %d\n", lightParam.globalAmbient.R, lightParam.globalAmbient.G, lightParam.globalAmbient.B);
		//DebugOut("diffuse %d, %d, %d\n", lightParam.globalDiffuse.R, lightParam.globalDiffuse.G, lightParam.globalDiffuse.B);
		CSceneEnvMgr::GetInst()->RgnReSetPlayerColor(lightParam.playerAmbient, lightParam.playerDiffuse);
		CSceneEnvMgr::GetInst()->RgnReSetFogParam(3, lightParam.fogStart, lightParam.fogEnd, lightParam.fogDensity);
		CSceneEnvMgr::GetInst()->SetShadowColor(lightParam.globalShadow);
	}
}

//------------------------------------------------------------------------------
void
CSceneRegionMgr::SetSceneRgnIndexName(set<int>& regionunits, const string& name)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	for ( set<int>::iterator iter = regionunits.begin(); iter != regionunits.end(); ++iter )
	{
		SDivideRegion *pDivideSceneRgn = NULL;
		pDivideSceneRgn = pTerrainMesh->GetDivideRegion(*iter);
		if ( pDivideSceneRgn )
			pDivideSceneRgn->m_strRegionName = name;
	}
}

//------------------------------------------------------------------------------
void
CSceneRegionMgr::SetSceneRgnMusicName(set<int>& regionunits, const string& name)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;
}

//------------------------------------------------------------------------------
void
CSceneRegionMgr::ReBackRegionInfo(set<int> &regionunits)
{
	CTerrainMesh *pTerrainMesh;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	for ( set<int>::iterator iter = regionunits.begin(); iter != regionunits.end(); ++iter )
	{
		SDivideRegion *pDivideRegion = NULL;
		pDivideRegion = pTerrainMesh->GetDivideRegion(*iter);
		if ( pDivideRegion )
		{
			pDivideRegion->m_bSetRegionEnv = false;
			pDivideRegion->m_strRegionName = "default";
		}
	}
}

void CSceneRegionMgr::SetLoadInfo( set<int>& regionunits, const string& strDivideRgnName )
{
	CTerrainMesh *pTerrainMesh = NULL;
	pTerrainMesh = CMapEditApp::GetInst()->GetEditingMesh();
	if ( pTerrainMesh == NULL )
		return;

	for ( set<int>::iterator iter = regionunits.begin(); iter != regionunits.end(); ++iter )
	{
		SDivideRegion *pDivideRgn = NULL;
		pDivideRgn = pTerrainMesh->GetDivideRegion(*iter);
		if ( pDivideRgn )
		{
			pDivideRgn->m_strRegionName = strDivideRgnName;
			pDivideRgn->m_bSetRegionEnv = true;
			pDivideRgn->bNeedCalRegion = true;
		}
	}
}

void CSceneRegionMgr::SetRegionIsSetEnv( set<int>& regions, bool b )
{
	if( regions.empty() )
		return;

	static CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();

	set<int>::iterator iter = regions.begin();
	set<int>::iterator etier = regions.end();
	for( iter; iter != etier; ++iter )
	{
		SDivideRegion *pDivideRgn = NULL;
		pDivideRgn = pTerrain->GetDivideRegion(*iter);

		if( pDivideRgn )
			pDivideRgn->m_bSetRegionEnv = b;
	}

	vector<DWORD> RegionAll;
	SDivideRegion *pDivideRgn = NULL;
	int nRegionCnt = pTerrain->GetRegionCount(), subRegIndex = 0;
	for ( int m = 0; m < nRegionCnt; ++m )
	{
		SRegion &region = pTerrain->GetRegion(m);

		for( int i = 0; i < REGION_DIVIDE;++i )
		{
			subRegIndex = region.divideRgnIndices[i];
			pDivideRgn = pTerrain->GetDivideRegion(subRegIndex);
			bool b = false;

			RegionAll = pTerrain->_RegionAjenctList[m];
			for( size_t i = 0; i < RegionAll.size(); ++i )
			{
				SDivideRegion *pDivideRegion = NULL;
				pDivideRegion = pTerrain->GetDivideRegion(RegionAll[i]);
				if( pDivideRegion )
					b = pDivideRegion->m_bSetRegionEnv || b;
			}

			pDivideRgn->bNeedCalRegion = b;
		}
	}
}

void CSceneRegionMgr::SubRegionIsNeedRealCal()
{
	CTerrainMesh *pTerrain = CMapEditApp::GetInst()->GetEditingMesh();
	int nRegionCnt = pTerrain->GetRegionCount(), subRegIndex = 0;
	vector<DWORD> RegionAll;
	SDivideRegion *pDivideRgn = NULL;

	for ( int m = 0; m < nRegionCnt; ++m )
	{
		SRegion &region = pTerrain->GetRegion(m);

		for( int i = 0; i < REGION_DIVIDE;++i )
		{
			subRegIndex = region.divideRgnIndices[i];
			pDivideRgn = pTerrain->GetDivideRegion(subRegIndex);
			bool b = false;

			RegionAll = pTerrain->_RegionAjenctList[subRegIndex];
			for( size_t i = 0; i < RegionAll.size(); ++i )
			{
				SDivideRegion *pDivideRegion = NULL;
				pDivideRegion = pTerrain->GetDivideRegion(RegionAll[i]);
				if( pDivideRegion )
					b = pDivideRegion->m_bSetRegionEnv || b;
			}

			pDivideRgn->bNeedCalRegion = b;
		}
	}
}
//------------------------------------------------------------------------------
SceneRegion::SceneRegion( void ) : m_strSceneRgnName(""), m_strRgnMusicName(""), m_uFogType(3)
{
	this->lightParam.globalAmbient = 0;
	this->lightParam.globalDiffuse = 0;
	this->lightParam.globalSpecular = 0;

	this->lightParam.playerAmbient = 0;
	this->lightParam.playerDiffuse = 0;
	this->lightParam.playerSpecular = 0;

	this->lightParam.fogColor	   = 0;

	this->lightParam.fogDensity	   = (int)(FOGDENSITY*80000);
	this->lightParam.fogStart      = (int)(FOGSTART/60);
	this->lightParam.fogEnd        = (int)(FOGEND/300);

	this->lightParam.specularPower = 5.0f;
}

SceneRegion::~SceneRegion()
{
	RegionAmbientFxMap::iterator beg = this->_RegionEffectUnitMap.begin();
	RegionAmbientFxMap::iterator end = this->_RegionEffectUnitMap.end();
	for ( beg; beg != end; ++beg )
	{
		sRegAmbientFxUnit *pTemp = beg->second;
		if( pTemp )
			delete pTemp;
	}

	this->_RegionEffectUnitMap.clear();
}

void SceneRegion::InsertRegionEffect( const string& effectname )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter == this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = new sRegAmbientFxUnit;
		pTemp->strEffectName     = effectname;

		this->_RegionEffectUnitMap.insert(make_pair(effectname, pTemp));
	}
	else
	{
		string msg("");
		msg = m_strSceneRgnName + "区域中已存在特效：" + effectname;
		MessageBox(NULL, msg.c_str(), "提示", MB_OK);
	}
}

void SceneRegion::DeleteRegionEffect( const string& effectname )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter != this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = iter->second;

		this->_RegionEffectUnitMap.erase(iter);
		if( pTemp )
			delete pTemp;
	}
	else
	{
		string msg("");
		msg = m_strSceneRgnName + "区域中没有特效：" + effectname;
		MessageBox(NULL, msg.c_str(), "提示", MB_OK);
	}
}

void SceneRegion::SetAmientFxIntervalTime( const string& effectname , const DWORD dwMin, const DWORD dwMax )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter != this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = iter->second;
		if( pTemp )
		{
			pTemp->dwPlayIntervalMin = dwMin;
			pTemp->dwPlayIntervalMax = dwMax;
		}
	}
}

void SceneRegion::GetAmientFxIntervalTime( const string& effectname, DWORD& dwMin, DWORD& dwMax )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter != this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = iter->second;
		if( pTemp )
		{
			dwMin = pTemp->dwPlayIntervalMin;
			dwMax = pTemp->dwPlayIntervalMax;
		}
		else
		{
			string msg("");
			msg = m_strSceneRgnName + "区域中没有特效：" + effectname;
			MessageBox(NULL, msg.c_str(), "提示", MB_OK);
		}
	}
	else
	{
		string msg("");
		msg = m_strSceneRgnName + "区域中没有特效：" + effectname;
		MessageBox(NULL, msg.c_str(), "提示", MB_OK);
	}
}

void SceneRegion::SetAmientFxPlayTime( const string& effectname , const DWORD dwMin, const DWORD dwMax )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter != this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = iter->second;
		if( pTemp )
		{
			pTemp->dwPlayTimeMin = dwMin;
			pTemp->dwPlayTimeMax = dwMax;
		}
	}
}

void SceneRegion::GetAmientFxPlayTime( const string& effectname, DWORD& dwMin, DWORD& dwMax )
{
	RegionAmbientFxMap::iterator iter = this->_RegionEffectUnitMap.find(effectname);
	if( iter != this->_RegionEffectUnitMap.end() )
	{
		sRegAmbientFxUnit *pTemp = iter->second;
		if( pTemp )
		{
			dwMin = pTemp->dwPlayTimeMin;
			dwMax = pTemp->dwPlayTimeMax;
		}
		else
		{
			string msg("");
			msg = m_strSceneRgnName + "区域中没有特效：" + effectname;
			MessageBox(NULL, msg.c_str(), "提示", MB_OK);
		}
	}
	else
	{
		string msg("");
		msg = m_strSceneRgnName + "区域中没有特效：" + effectname;
		MessageBox(NULL, msg.c_str(), "提示", MB_OK);
	}
}

vector<string> SceneRegion::GetRegionEffectNameList()
{
	vector<string> regAmbientEffectNames;
	RegionAmbientFxMap::iterator beg = this->_RegionEffectUnitMap.begin();
	RegionAmbientFxMap::iterator end = this->_RegionEffectUnitMap.end();
	for ( beg; beg != end; ++beg )
	{
		regAmbientEffectNames.push_back(beg->first);
	}

	return regAmbientEffectNames;
}

RegionAmbientFxMap SceneRegion::GetRgnAmbinetFxList() const
{
	return this->_RegionEffectUnitMap;
}

}// namespace sqr
