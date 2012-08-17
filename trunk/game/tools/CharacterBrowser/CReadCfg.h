#pragma once

class CReadCfg
{
public:
	CReadCfg();
	void InitData();
	void ReadNpcRes_Client();
	void ReadNpcRes_Common();
	void ReadPropRes_Common();
	void ReadWeaponRes_Common();
	void ReadPropRes();
private:
	struct PropResData
	{
		string m_sName;
		string m_sJob;
		string m_sPart;
		string m_sCovering;
		string m_sModName;
		string m_sEffectFileName;
		string m_sManner;
	};	
};
