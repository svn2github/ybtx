#include "stdafx.h"
#include "CCfgPropertyCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "CPropertyCfg.h"

CPropertyCfg::MapClassType	CPropertyCfg::ms_mapClassType;

struct LevelZone
{
	uint8 uMinLevel;
	uint8 uMaxLevel;
};

bool CCfgPropertyCheck::Check(const TCHAR* cfgFile)
{
	CPropertyCfg::InitMapClassType();
	CTxtTableFile TabFile;
	CfgChk::SetTabFile(TabFile, "人物属性表");
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
		return false;
	
	static const uint32 s_uTableFileWide = 27;
	if (TabFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TabFile.GetWidth() << " 列";
		CfgChk::GenExpInfo(ExpStr.str());
	}

	for (int32 i = 1; i < TabFile.GetHeight(); ++i)
	{
		// 职业 & 等级 检测
		string strClass = TabFile.GetString(i, "职业");
		CPropertyCfg::MapClassType::iterator it = CPropertyCfg::ms_mapClassType.find(strClass);
		if (it == CPropertyCfg::ms_mapClassType.end())
		{
			stringstream ExpStr;
			ExpStr << "配置表[人物属性表] "<< strClass <<" 不存在";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		
		CCfgCalc* pLevel = new CCfgCalc(TabFile.GetString(i, "等级"));
		uint8 uLeftLevel, uRightLevel;
		if (pLevel->GetStringCount() != 2)
		{
			stringstream ExpStr;
			ExpStr << "配置表[人物属性表] "<< strClass << " 等级区间填写错误，必须有两项最小到最大等级";
			CfgChk::GenExpInfo(ExpStr.str());
		}
		else
		{
			uLeftLevel = atoi(pLevel->GetString(0).c_str());
			uRightLevel = atoi(pLevel->GetString(1).c_str());
		}

		static string strReadingClass = "";
		typedef map<string, LevelZone> MapClassLevelZone;
		static MapClassLevelZone mapReadClass;
		MapClassLevelZone::iterator itr = mapReadClass.find(strClass);
		if (strClass != strReadingClass)
		{
			if (itr != mapReadClass.end())
			{
				stringstream ExpStr;
				ExpStr << "配置表[人物属性表] "<< strClass <<" 已读取过，请查看是否重复填写";
				CfgChk::GenExpInfo(ExpStr.str());
			}
			else
			{
				LevelZone levelZone;
				levelZone.uMinLevel = uLeftLevel;
				levelZone.uMaxLevel = uRightLevel;
				mapReadClass.insert(make_pair(strClass, levelZone));
				strReadingClass = strClass;
			}

			if (uLeftLevel != 1)
			{
				stringstream ExpStr;
				ExpStr << "配置表[人物属性表] "<< strClass <<" 开始等级必须为1";
				CfgChk::GenExpInfo(ExpStr.str());
			}
		}
		else
		{
			if (uLeftLevel != (*itr).second.uMaxLevel + 1)
			{
				stringstream ExpStr;
				ExpStr << "配置表[人物属性表] "<< strClass <<" 填写的等级 不连续 或 重叠";
				CfgChk::GenExpInfo(ExpStr.str());
			}
			else
			{
				(*itr).second.uMaxLevel = uRightLevel;
			}
		}
		// 职业 & 等级 END

		CCfgCalc* pMaxHP					= new CCfgCalc(TabFile.GetString(i, "生命上限"));
		CCfgCalc* pMaxMP					= new CCfgCalc(TabFile.GetString(i, "魔法上限"));
		CCfgCalc* pDPS						= new CCfgCalc(TabFile.GetString(i, "DPS"));
		CCfgCalc* pMagicDamage				= new CCfgCalc(TabFile.GetString(i, "法伤"));
		CCfgCalc* pDefence					= new CCfgCalc(TabFile.GetString(i, "护甲值"));
		CCfgCalc* pPhysicalDodge			= new CCfgCalc(TabFile.GetString(i, "物理躲避值"));
		CCfgCalc* pStrikeValue				= new CCfgCalc(TabFile.GetString(i, "暴击值"));
		CCfgCalc* pMagicDodge				= new CCfgCalc(TabFile.GetString(i, "法术躲避值"));
		CCfgCalc* pNatureResistance			= new CCfgCalc(TabFile.GetString(i, "自然抗值"));
		CCfgCalc* pDestructionResistance	= new CCfgCalc(TabFile.GetString(i, "破坏抗性"));
		CCfgCalc* pEvilResistance			= new CCfgCalc(TabFile.GetString(i, "暗黑抗性"));
		CCfgCalc* pAccuratenessValue		= new CCfgCalc(TabFile.GetString(i, "精准值"));			
		CCfgCalc* pMagicHitValue			= new CCfgCalc(TabFile.GetString(i, "法术命中"));
		CCfgCalc* pStrikeMultiValue			= new CCfgCalc(TabFile.GetString(i, "暴击上限"));			
		CCfgCalc* pResilienceValue			= new CCfgCalc(TabFile.GetString(i, "韧性值"));	
		CCfgCalc* pStrikeResistanceValue			= new CCfgCalc(TabFile.GetString(i, "免暴值"));	
		CCfgCalc* pParryValue				= new CCfgCalc(TabFile.GetString(i, "招架值"));
		CCfgCalc* pNatureSmashValue			= new CCfgCalc(TabFile.GetString(i, "自然碾压值"));
		CCfgCalc* pDestructionSmashValue	= new CCfgCalc(TabFile.GetString(i, "破坏碾压值"));
		CCfgCalc* pEvilSmashValue			= new CCfgCalc(TabFile.GetString(i, "暗黑碾压值"));
		CCfgCalc* pDefenceSmashValue		= new CCfgCalc(TabFile.GetString(i, "护甲碾压值"));
		pMaxHP->SetTypeExpression();
		pMaxMP->SetTypeExpression();
		pDPS->SetTypeExpression();
		pMagicDamage->SetTypeExpression();
		pDefence->SetTypeExpression();
		pPhysicalDodge->SetTypeExpression();
		pStrikeValue->SetTypeExpression();
		pMagicDodge->SetTypeExpression();
		pNatureResistance->SetTypeExpression();
		pDestructionResistance->SetTypeExpression();
		pEvilResistance->SetTypeExpression();
		pAccuratenessValue->SetTypeExpression();
		pMagicHitValue->SetTypeExpression();
		pStrikeMultiValue->SetTypeExpression();
		pStrikeResistanceValue->SetTypeExpression();
		pResilienceValue->SetTypeExpression();
		pParryValue->SetTypeExpression();
		pNatureSmashValue->SetTypeExpression();
		pDestructionSmashValue->SetTypeExpression();
		pEvilSmashValue->SetTypeExpression();
		pDefenceSmashValue->SetTypeExpression();

		delete pLevel;
		delete pMaxHP;
		delete pMaxMP;
		delete pDPS;
		delete pMagicDamage;
		delete pDefence;
		delete pPhysicalDodge;
		delete pStrikeValue;
		delete pMagicDodge;
		delete pNatureResistance;
		delete pDestructionResistance;
		delete pEvilResistance;
		delete pAccuratenessValue;
		delete pMagicHitValue;
		delete pStrikeMultiValue;
		delete pResilienceValue;
		delete pStrikeResistanceValue;
		delete pParryValue;
		delete pNatureSmashValue;
		delete pDestructionSmashValue;
		delete pEvilSmashValue;
		delete pDefenceSmashValue;
	}

	return true;
}

void CCfgPropertyCheck::EndCheck()
{
	CPropertyCfg::ms_mapClassType.clear();
}
