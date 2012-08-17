#include "stdafx.h"
#include "CServantData.h"
#include "CTxtTableFile.h"
#include "TimeHelper.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

CServantData::CServantData(sqr::uint32 uID, const std::string &sName)
:m_uID(uID)
,m_sName(sName)
{

}

CServantDataMgr::CServantDataMgr()
{

}

CServantDataMgr::~CServantDataMgr()
{
	DeleteAll();
}

bool CServantDataMgr::LoadConfig( const string& sFileName )
{
	CTxtTableFile TabFile;
	CServantData*  pValue;
	uint64 uBeginTime = GetProcessTime();
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), sFileName.c_str())) return false;

	DeleteAll();

	for(int32 i=1; i<TabFile.GetHeight(); ++i)
	{   
		pValue = new CServantData(GetSize(), TabFile.GetString(i, "Name"));
		pValue->m_MaxHealthPoint	= TabFile.GetFloat(i, "生命上限", 0.0f);
		pValue->m_MaxManaPoint		= TabFile.GetFloat(i, "魔法上限", 0.0f);
		pValue->m_PhysicalDodgeValue 	= TabFile.GetFloat(i, "物理躲避值", 0.0f);
		pValue->m_StrikeValue					= TabFile.GetFloat(i, "暴击值", 0.0f);
		pValue->m_StrikeMultiValue		= TabFile.GetFloat(i, "暴击上限", 0.0f);		
		pValue->m_MagicDodgeValue	= TabFile.GetFloat(i, "法术躲避值", 0.0f);
		pValue->m_MagicHitValue			= TabFile.GetFloat(i, "法术命中", 0.0f);			
		pValue->m_PhysicalDPS				= TabFile.GetFloat(i, "攻击力", 0.0f);				
		pValue->m_MagicDamageValue	= TabFile.GetFloat(i, "法伤", 0.0f);	
		pValue->m_Defence					= TabFile.GetFloat(i, "护甲值", 0.0f);				
		pValue->m_NatureResistanceValue= TabFile.GetFloat(i, "自然抗性", 0.0f);		
		pValue->m_DestructionResistanceValue= TabFile.GetFloat(i, "破坏抗性", 0.0f);
		pValue->m_EvilResistanceValue= TabFile.GetFloat(i, "暗黑抗性", 0.0f);	
		pValue->m_ParryValue= TabFile.GetFloat(i, "招架值", 0.0f);	
		pValue->m_ResilienceValue= TabFile.GetFloat(i, "韧性值", 0.0f);	
		pValue->m_StrikeResistanceValue= TabFile.GetFloat(i, "免暴值", 0.0f);	
		pValue->m_AccuratenessValue= TabFile.GetFloat(i, "精准值", 0.0f);	
		AddEntity(pValue);
	}   
	uint64 uEndTime = GetProcessTime();
	cout << "读取配置表：【ServantProperty_Server】完毕！    耗时：" << (uEndTime - uBeginTime) << " 毫秒！\n";
	return true;
}

