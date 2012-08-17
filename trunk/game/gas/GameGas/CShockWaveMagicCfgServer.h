#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

class CCfgCalc;

DefineCfgSharedPtr(CShockWaveMagicCfgServer)

class CShockWaveMagicCfgServer
	:public CMagicCfgServer
{
public:
	static bool LoadShockWaveMagicConfig(const string& szFileName);
	static void UnloadShockWaveMagicConfig();
	static CShockWaveMagicCfgServerSharedPtr& GetShockWaveMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CShockWaveMagicCfgServer();
	CShockWaveMagicCfgServer(const CShockWaveMagicCfgServer& cfg);
	~CShockWaveMagicCfgServer();

	CCfgCalc*	 		GetRange()		{return m_pRange;}
	CCfgCalc*			GetAngle()		{return m_pAngle;}
	CCfgCalc*	 		GetAmount()		{return m_pAmount;}
	EBulletTrackType	GetBulletTrackType()	{return m_eBulletTrackType;}
	bool				IsDisappearWhenEnough()	{return m_bDisappearWhenEnough;}
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CShockWaveMagicCfgServerSharedPtr ms_NULL;

private:
	typedef TStringRefer<CShockWaveMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;	
	typedef map<string, CShockWaveMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CShockWaveMagicCfgServerSharedPtr* > > >		MapShockWaveMagicCfgServer;
	typedef map<string, EBulletTrackType, less<string>, TConfigAllocator<pair<string, EBulletTrackType > > > MapBulletTrackType;
	typedef map<uint32, MagicEffServerStringRefer*, less<uint32>, 
		TConfigAllocator<pair<uint32, CShockWaveMagicCfgServerSharedPtr* > > >		MapMagicEffServer;

	static MapShockWaveMagicCfgServer					ms_mapShockWaveMagic;
	static MapBulletTrackType							ms_mapBulletTrackType;

	inline static bool InitMapBulletTrackType()	
	{	
		ms_mapBulletTrackType["平飞"]				= eBTT_Line;
		ms_mapBulletTrackType["随机移动"]			= eBTT_Random;
		return true;
	}

	CCfgCalc*	m_pRange;		// 子弹射程
	CCfgCalc*	m_pAngle;		// 夹角
	CCfgCalc*	m_pAmount;		// 子弹数量
	EBulletTrackType		m_eBulletTrackType;		// 轨迹
	bool		m_bDisappearWhenEnough;
	MagicEffServerStringRefer*	m_pMagicEff;
};

