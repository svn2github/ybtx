#pragma once
#include "TSingleton.h"


namespace sqr
{
	class CTObject;

	class CMapRegionAmbientEffectMgr : public Singleton<CMapRegionAmbientEffectMgr>
	{
	private:
		typedef map<string, map<string, CTObject *>> RegionAmbFxObjectMap;
		RegionAmbFxObjectMap	m_RgnAmbientFxObjectList;

		string m_strLastRgnName;
		vector<CTObject *> m_lastFxObjectList;
	public:

		CMapRegionAmbientEffectMgr();
		virtual ~CMapRegionAmbientEffectMgr();

		void CreateRgnAmbientFxObject( const string& rgnname, const string& effectname);
		void AddRgnAmbientFxObject( const string& rgnname, const string& effectname, CTObject * p );
		void DeleteRgnAmbientFxObject( const string& rgnname, const string& effectname );
		void SetAmientFxPlayTime( const string& regname, const string& effectname, const DWORD dwPlayTimeMin, const DWORD dwPlayTimeMax );
		void GetAmientFxPlayTime( const string& regname, const string& effectname, DWORD& dwPlayTimeMin, DWORD& dwPlayTimeMax );
		void SetAmientFxIntervalTime( const string& regname, const string& effectname, const DWORD dwIntervalTimeMin, const DWORD dwIntervalTimeMax );
		void GetAmientFxIntervalTime( const string& regname, const string& effectname, DWORD& dwIntervalTimeMin, DWORD& dwIntervalTimeMax );
		
		void SwitchShowAllRgnAmbientEffect(const bool b);
	
		void RenderAllReginAmbientFx();
		void RenderLogicReginAmbFxByPlayer();

		void SaveRgnAmbientFxInfo( FILE *fp, bool bResize, bool bSplit );
		void LoadRgnAmbientFxInfoInfo( FILE * fp );

		void SaveRgnAmbientFXInfoAsClient( FILE *fp, map<string, string>& rgnEffectFileNameList, map<string, int>& rgnEffectNameIndexList );
	};
}
