#pragma once
#include "RegionLightParam.h"

//------------------------------------------------------------------------------
namespace sqr
{
struct sRegAmbientFxUnit
{
	string strEffectName;
	DWORD  dwPlayTimeMin;
	DWORD  dwPlayTimeMax;
	DWORD  dwPlayIntervalMin;
	DWORD  dwPlayIntervalMax;

	sRegAmbientFxUnit()
	{
		strEffectName = "";
		dwPlayTimeMin     = 0;
		dwPlayTimeMax	  = 0;
		dwPlayIntervalMin = 0;
		dwPlayIntervalMax = 0;
	}
};

typedef map<string , sRegAmbientFxUnit*> RegionAmbientFxMap;

class SceneRegion
{
private:
	//场景区域名
	string m_strSceneRgnName;

	//区域音乐名
	string m_strRgnMusicName;

	///区域雾效类型
	uint8  m_uFogType;

	///区域特效
	RegionAmbientFxMap _RegionEffectUnitMap; 
public:
	set<int>	_RegionUnitList;
	
	RegionLightParam lightParam;

public:
	SceneRegion();
	~SceneRegion();

	void SetSceneRgnName( const string& name );
	void SetSceneRgnMusicName( const string& name );
	void SetFogType( const uint8 fogType );
	void SetFogDensity( const float density );
	void SetFogStart( const float start );
	void SetFogEnd( const float end );

	string GetSceneRgnName();
	string GetSceneRgnMusicName();
	uint8 GetSceneRgnFogType() const;
	float GetSceneRgnFogDensity() const;
	float GetSceneRgnFogStart() const;
	float GetSceneRgnFogEnd() const;

	void InsertRegionEffect( const string& effectname );
	void DeleteRegionEffect( const string& effectname );
	void SetAmientFxIntervalTime( const string& effectname, const DWORD dwMin, const DWORD dwMax );
	void GetAmientFxIntervalTime( const string& effectname, DWORD& dwMin, DWORD& dwMax );
	void SetAmientFxPlayTime( const string& effectname, const DWORD dwMin, const DWORD dwMax );
	void GetAmientFxPlayTime( const string& effectname, DWORD& dwMin, DWORD& dwMax );
	vector<string> GetRegionEffectNameList();
	RegionAmbientFxMap GetRgnAmbinetFxList() const;
};

//------------------------------------------------------------------------------
inline void
SceneRegion::SetSceneRgnName( const string& name )
{
	m_strSceneRgnName = name;
}

//------------------------------------------------------------------------------
inline void
SceneRegion::SetSceneRgnMusicName( const string& name )
{
	m_strRgnMusicName = name;
}

//------------------------------------------------------------------------------
inline void
SceneRegion::SetFogType( const uint8 fogType )
{
	m_uFogType = fogType;
}

//------------------------------------------------------------------------------
inline void
SceneRegion::SetFogDensity( const float density )
{
	this->lightParam.fogDensity = density;
}

//------------------------------------------------------------------------------
inline void
SceneRegion::SetFogStart( const float start )
{
	this->lightParam.fogStart = start;
}

//------------------------------------------------------------------------------
inline void
SceneRegion::SetFogEnd( const float end )
{
	this->lightParam.fogEnd = end;
}

//------------------------------------------------------------------------------
inline std::string
SceneRegion::GetSceneRgnName()
{
	return m_strSceneRgnName;
}

//------------------------------------------------------------------------------
inline std::string
SceneRegion::GetSceneRgnMusicName()
{
	return m_strRgnMusicName;
}

//------------------------------------------------------------------------------
inline sqr::uint8
SceneRegion::GetSceneRgnFogType() const
{
	return m_uFogType;
}

//------------------------------------------------------------------------------
inline float
SceneRegion::GetSceneRgnFogDensity() const
{
	return this->lightParam.fogDensity;
}

//------------------------------------------------------------------------------
inline float
SceneRegion::GetSceneRgnFogStart() const
{
	return this->lightParam.fogStart;
}

//------------------------------------------------------------------------------
inline float
SceneRegion::GetSceneRgnFogEnd() const
{
	return this->lightParam.fogEnd;
}

}// namespace sqr
