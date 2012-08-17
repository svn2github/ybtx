#pragma once
#include "PropResChecker.h"

inline string DeQuot(const string& sText)
{
	size_t l = sText.length();
	if(l >= 2 && sText[0] == '"' && sText[l - 1] == '"')
	{
		return sText.substr(1, l - 2);
	}
	else
	{
		return sText;
	}
}

class CSkillCfgCheck;
typedef map<string, CSkillCfgCheck*> MapSkillCfgCheck;
class CSkillCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName);
private:
	static MapSkillCfgCheck			m_mapSkill;
	//static MapChkPart				m_mapAnimChkPart;

	uint32							m_uId;
	string 							m_sName;							//技能名称
	string							m_sBeginSkillAni;					//技能开始动作
	string							m_sSkillFX;							//技能特效
};

class CCastingCfgCheck;
typedef map<string, CCastingCfgCheck*>		MapCastingCfgCheck;
class CCastingCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName);
private:
	static MapCastingCfgCheck		m_mapCasting;
	//static MapChkPart				m_mapAnimChkPart;
	
	uint32							m_uId;
	string							m_sName;
	string							m_sAni;
	string							m_sFX;
};

class CMagicOpCfgCheck;
typedef map<uint32, CMagicOpCfgCheck*>		MapMagicOpCfgCheck;
class CMagicOpCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName);
private:
	static MapMagicOpCfgCheck		m_mapMagicOp;
	
	uint32							m_uId;
	string							m_sName;
	string							m_sFX;
};

class CStateCfgCheck;
typedef map<string, CStateCfgCheck*>		MapStateCfgCheck;
class CStateCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName, const string& szTypeName);
private:
	static MapStateCfgCheck			m_mapState;
	
	uint32							m_uId;
	string							m_sName;
	string							m_sModel;
	string							m_sFX;
};

class CStanceSkillCfgCheck;
typedef map<string, CStanceSkillCfgCheck*>	MapStanceSkillCfgCheck;
class CStanceSkillCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName);
private:
	static MapStanceSkillCfgCheck	m_mapStanceSkill;
	
	uint32							m_uId;
	string							m_sName;
	string							m_sFX;
};

class CBulletMagicCfgCheck;
typedef map<string, CBulletMagicCfgCheck*>	MapBulletMagicCfgCheck;
class CBulletMagicCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName);
private:
	static MapBulletMagicCfgCheck	m_mapBulletMagic;
	static uint32					m_uMaxId;

	uint32							m_uId;
	string							m_sName;
	string							m_sModel;
	string							m_sFX;
	string							m_sExplodeFX;
};

class CMagicCfgCheck;
typedef map<string, CMagicCfgCheck*>	MapMagicCfgCheck;
class CMagicCfgCheck
{
public:
	static bool						CheckConfig(const string& szFileName, const string& szTypeName);
private:
	static MapMagicCfgCheck			m_mapMagic;
	static uint32					m_uMaxId;

	uint32							m_uId;
	string							m_sName;
	string							m_sModel;
	string							m_sFX;
};


bool CheckAllConfig();
