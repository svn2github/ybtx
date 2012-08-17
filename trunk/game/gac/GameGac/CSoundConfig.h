#pragma once

enum ESoundTypeName
{
	eSTN_Strike,
	eSTN_Attack,
	eSTN_Suffer,
	eSTN_Die,
	eSTN_EnterBattle,
};

class CSoundConfig
{
public:
	static bool	LoadConfig(const TCHAR* szPlayerCfgFile, const TCHAR* szNPCCfgFile);
	static void	UnloadConfig();

	static CSoundConfig* GetSoundCfg(string strName);

	static string GetMeterialString(uint32 uMeterialID);

	string GetSufferSound() const		{return m_strSuffer;}
	string GetDieSound() const			{return m_strDie;}
	string GetFootstepSound() const		{return m_strFootstep;}
	string GetEnterBattleSound() const	{return m_strEnterBattle;}
	string GetWeaponType() const		{return m_strWeaponType;}
	string GetArmorType() const			{return m_strArmorType;}

private:
	typedef map<string, CSoundConfig*>	MapSoundCfgByName;
	static MapSoundCfgByName			ms_mapSoundCfgByName;

	string		m_strIndexName;
	string		m_strSuffer;
	string		m_strDie;
	string		m_strFootstep;
	string		m_strEnterBattle;
	string		m_strWeaponType;
	string		m_strArmorType;
};
