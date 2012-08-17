#pragma once
#include "TSingleton.h"
#include "SceneRegion.h"
#include "CVector3.h"
//------------------------------------------------------------------------------
namespace sqr
{

class CSceneRegionMgr : public Singleton<CSceneRegionMgr>
{
public:
	CSceneRegionMgr(void);
	~CSceneRegionMgr(void);
	void Release();

	map<string, SceneRegion*> m_mapRegion;

	//添加场景区域
	void AddSceneRegion( const string& name, SceneRegion* sceneRegion );
	void AddSceneRegion( const string& name );
	//删除区域
	bool DeleteSceneRegion( const string& name );

	//添加场景区域号
	bool AddSceneRegionUnits( const string& name, set<int>& units );
	bool AddSceneRegionUnit( const string& name, const int& unit );

	//删除场景区域号
	bool DeleteSceneRegionUnit( const string& name, const int& unit );
	bool DeleteSceneRegionUnits( const string& name, set<int>& units );

	SceneRegion* GetSceneRegion( const string& name );

	void SetSceneRgnAmbient( const string& name, DWORD color);
	void SetSceneRgnDiffuse( const string& name, DWORD color);
	void SetSceneRgnSpecular( const string& name, DWORD color);
	void SetSceneRgnSpecularPower( const string& name, float fPower);
	void SetPlayerRgnAmbient( const string& name, DWORD color);
	void SetPlayerRgnDiffuse( const string& name, DWORD color);
	void SetPlayerRgnSpecular( const string& name, DWORD color);
	void SetSceneRgnFog(const string& name, DWORD color);
	void SetSceneRgnShadow(const string& name, DWORD color);
	bool SetSceneRgnMusicName( const string& rgnname, const string& musicname );

	///fog
	bool SetFogType( const string& name,  const uint8 fogType );
	bool SetFogDensity( const string& name,  const float density );
	bool SetFogStart( const string& name,  const float start );
	bool SetFogEnd( const string& name,  const float end );

	DWORD GetSceneRgnAmbient( const string& name );
	DWORD GetSceneRgnDiffuse( const string& name );
	DWORD GetSceneRgnSpecular( const string& name );
	float GetSceneRgnSpecularPower( const string& name );
	DWORD GetPlayerRgnAmbient( const string& name );
	DWORD GetPlayerRgnDiffuse( const string& name );
	DWORD GetPlayerRgnSpecular( const string& name );
	DWORD GetSceneRgnFog( const string& name );
	DWORD GetSceneRgnShadow(const string& name);

	int    GetSceneRgnTransCnt( const string& name );
	string GetSceneRgnMusicName( const string& name );

	uint8 GetSceneRgnFogType( const string& name ) ;
	float GetSceneRgnFogDensity( const string& name );
	float GetSceneRgnFogStart( const string& name ) ;
	float GetSceneRgnFogEnd( const string& name );

	///ambient effect
	void InsertRegionEffect( const string& regname, const string& effectname );
	void DeleteRegionEffect( const string& regname, const string& effectname );
	void SetAmientFxPlayTime( const string& regname, const string& effectname, const DWORD nTimeMin, const DWORD nTimeMax );
	void GetAmientFxPlayTime( const string& regname, const string& effectname, DWORD& nTimeMin, DWORD& nTimeMax );
	void SetAmientFxIntervalTime( const string& regname, const string& effectname, const DWORD nTimeMin, const DWORD nTimeMax );
	void GetAmientFxIntervalTime( const string& regname, const string& effectname, DWORD& nTimeMin, DWORD& nTimeMax );
	vector<string> GetRegionEffectNameList(const string& regname);

	set<int> GetCurrentRegions( const string& name);

	void SaveSceneRegionInfo(FILE * fp, bool bSplit = false);
	void LoadSceneRegionInfo(FILE * fp);
	void LoadSceneRegionOldFile(FILE * fp);
	void LoadSceneRegionNewFile(FILE * fp);
	void SaveRegionFogParam( FILE * fp, SceneRegion *pSceneRgn );
	void LoadRegionFogParam(FILE * fp, SceneRegion *pSceneRgn);
	void SaveRegionAmbinetFx( FILE * fp, SceneRegion *pSceneRgn );
	void LoadRegionAmbinetFx(FILE * fp, SceneRegion *pSceneRgn);
	void SaveRegionSpecularParam( FILE * fp, SceneRegion *pSceneRgn, const DWORD sceneSpecular, const DWORD playerSpecular );
	void LoadRegionSpecularParam(FILE * fp, SceneRegion *pSceneRgn);

	///区域计算
	void CalcRegionLightByRegionDistance(CVector3f v);
	void SetSceneRgnIndexName( set<int>& regionunits, const string& name  );
	void ReBackRegionInfo(set<int>& regionunits);
	void SetSceneRgnMusicName( set<int>& regionunits, const string& name) ;

	void SetLoadInfo( set<int>& regionunits, const string& strDivideRgnName );

	inline bool SceneRegionsIsEmpty() const
	{
		return m_mapRegion.empty();
	}

	bool DecideRegionIsSet(const int& regionindex, string& rgnName);

	// 标志场景区域是否设置了环境光
	void SetRegionIsSetEnv( set<int>& regions, bool b );
	void SubRegionIsNeedRealCal();
};

}// namespace sqr
