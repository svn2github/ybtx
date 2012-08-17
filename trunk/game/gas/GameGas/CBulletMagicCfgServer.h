#pragma once
#include "CMagicCfgServer.h"
#include "CMagicEffServer.h"
#include "TCfgSharedPtr.h"

DefineCfgSharedPtr(CBulletMagicCfgServer)

class CBulletMagicCfgServer
	:public CMagicCfgServer
{
public:
	static		bool LoadBulletMagicConfig(const string& szFileName);
	static		void UnloadBulletMagicConfig();
	static		CBulletMagicCfgServerSharedPtr& GetBulletMagic(const string& szName);
	static		void UpdateCfg(const string& strName);

	CBulletMagicCfgServer();
	CBulletMagicCfgServer(const CBulletMagicCfgServer& cfg);
	~CBulletMagicCfgServer();

	EBulletTrackType	GetBulletTrackType()		{return m_eBulletTrackType;};
	virtual const CMagicEffServerSharedPtr& GetMagicEff()const;

	static CBulletMagicCfgServerSharedPtr ms_NULL;
private:
	typedef map<string, CBulletMagicCfgServerSharedPtr*, less<string>, 
		TConfigAllocator<pair<string, CBulletMagicCfgServerSharedPtr* > > >		MapBulletMagicCfgServer;
	typedef map<string, EBulletTrackType, less<string>, 
		TConfigAllocator<pair<string, EBulletTrackType > > > MapBulletTrackType;
	typedef TStringRefer<CBulletMagicCfgServer, CMagicEffServer> MagicEffServerStringRefer;

	static bool	InitMapBulletTrackType()	
	{	
		ms_mapBulletTrackType["Æ½·É"]				= eBTT_Line;
		ms_mapBulletTrackType["Å×ÎïÏß"]				= eBTT_Parabola;
		ms_mapBulletTrackType["µ¯Çò±¬»ê"]			= eBTT_MarblesSoul;
		return true;
	}
	static MapBulletMagicCfgServer		ms_mapBulletMagic;
	static MapBulletTrackType			ms_mapBulletTrackType;

	EBulletTrackType					m_eBulletTrackType;     //¹ì¼£
	MagicEffServerStringRefer*			m_pMagicEff;
};	

