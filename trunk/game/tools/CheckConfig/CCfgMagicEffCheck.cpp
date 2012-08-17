#include "stdafx.h"
#include "CCfgMagicEffCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CCfgMagicOp.h"
#include "CSkillCfg.h"

CCfgMagicEffCheck::MapMagicEff		CCfgMagicEffCheck::ms_mapMagicEff;
CCfgMagicEffCheck::MapMagicOp		CCfgMagicEffCheck::ms_mapMagicOp;
uint32								CCfgMagicEffCheck::ms_uMaxLineNum = 0;

bool CCfgMagicEffCheck::Check(const TCHAR* cfgFile, bool bFirstFile)
{
	using namespace CfgChk;
	if (bFirstFile)
	{
		CSkillCfg::BuildObjFilterMap();
		BuildMOPMap();
		ms_mapMagicEff.clear();
	}

	CTxtTableFile TableFile;
	string strCfgFile(cfgFile);
	if (strCfgFile.find("PlayerMagicEffect.txt") != -1)
	{
		SetTabFile(TableFile, "Player魔法效果");
	}
	else if (strCfgFile.find("PlayerActivityMagicEffect.txt") != -1)
	{
		SetTabFile(TableFile, "Player玩法魔法效果");
	}
	else if (strCfgFile.find("NpcMagicEffect.txt") != -1)
	{
		SetTabFile(TableFile, "Npc魔法效果");
	}
	else if (strCfgFile.find("NpcActivityMagicEffect.txt") != -1)
	{
		SetTabFile(TableFile, "副本Npc魔法效果");
	}
	else if (strCfgFile.find("NpcGameMagicEffect.txt") != -1)
	{
		SetTabFile(TableFile, "Npc玩法魔法效果");
	}

	
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 8;
	if (TableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	bool bNewSec = true;
	for(uint32 i = ms_uMaxLineNum+1; i < TableFile.GetHeight()+ms_uMaxLineNum; ++i)
	{
		uint32 uLineNum = i-ms_uMaxLineNum;
		SetLineNo(uLineNum);
		string strName;
		ReadItem(strName, szMagicEff_Name,	CANEMPTY);
		if( strName.empty() )
		{
			bNewSec = true;
			continue;
		}
		trimend(strName);
		static CCfgMagicEffCheck* pCfgMagicEff;
		if (bNewSec)
		{
			bNewSec = false;
			MapMagicEff::const_iterator iter = ms_mapMagicEff.find(strName);
			if (iter != ms_mapMagicEff.end())
			{
				stringstream ExpStr;
				ExpStr << "第" << uLineNum << "行的魔法效果: " << strName << " 重复";
				GenExpInfo(ExpStr.str());
			}
			else
			{
				pCfgMagicEff = new CCfgMagicEffCheck();
				pCfgMagicEff->m_strName = strName;
				ms_mapMagicEff.insert(make_pair(strName, pCfgMagicEff));
			}
		}

		if (!bNewSec && pCfgMagicEff->m_strName != strName)
		{
			stringstream ExpStr;
			ExpStr << "第" << uLineNum << "行的同一魔法效果，与上一行不同名: " << strName;
			GenExpInfo(ExpStr.str());
		}

		// 操作执行条件检查
		string strExecCond;
		ReadItem(strExecCond,			szMagicEff_MOPExecCond, CANEMPTY);
		if (!strExecCond.empty())
		{
			trimend(strExecCond);
			if(strExecCond.compare("#") == 0)
				break;
			TCHAR* token;
			string whole = strExecCond;
			token = strtok( const_cast<char*>(whole.c_str()), "(),\"" );

			vector<string> vecExecCond;
			int32 index = 1;//标识条件依赖关系中的数字下标。奇数下标的数字表示节点号，偶数下标的数字表示MOP执行返回结果
			int32 iNum = 0;
			int32 iRet = 0;
			while( token != NULL )	//初始化当前节点的条件依赖关系
			{
				if((index % 2) == 1)
				{
					bool bIsExist = false;
					VecChild::iterator itrChild = pCfgMagicEff->m_vecChild.begin();
					for (; itrChild != pCfgMagicEff->m_vecChild.end(); ++ itrChild)
					{
						if(strcmp(token, ((*itrChild)->GetMOPType()).c_str()) == 0)
						{
							bIsExist = true;
							break;
						}
					}

					if (!bIsExist)
					{
						stringstream ExpStr;
						ExpStr << "第" << uLineNum << "行的" << " 操作执行条件 " << token << " 填错";
						GenExpInfo(ExpStr.str());
					}
					
					bIsExist = false;
					for (size_t i = 0; i < vecExecCond.size(); ++i)
					{
						if(strcmp(token, vecExecCond[i].c_str()) == 0)
						{
							bIsExist = true;
							break;
						}
					}
					if (bIsExist)
					{
						stringstream ExpStr;
						ExpStr << "第" << uLineNum << "行的" << " 操作执行条件 " << token << " 填错";
						GenExpInfo(ExpStr.str());
					}
					vecExecCond.push_back(token);
				}
				else
				{
					if ( !(strcmp(token, "失败") == 0 || strcmp(token, "成功") == 0 || strcmp(token, "命中") == 0 
						|| strcmp(token, "未命中") == 0 || strcmp(token, "暴击") == 0 || strcmp(token, "物理闪避") == 0
						|| strcmp(token, "魔法闪避") == 0 || strcmp(token, "抵抗") == 0 || strcmp(token, "完全抵抗") == 0
						|| strcmp(token, "招架") == 0 || strcmp(token, "格挡") == 0 || strcmp(token, "免疫") == 0 ))
					{
						stringstream ExpStr;
						ExpStr << "第" << uLineNum << "行的" << " 操作执行条件 " << token << " 填错";
						GenExpInfo(ExpStr.str());
					}
				}
				++index;
				// Get next token: 
				token = strtok( NULL, "(),\"" );
			}
			vecExecCond.clear();
		}

		CCfgMagicOp* pCfgMagicOp = new CCfgMagicOp(i);
		pCfgMagicEff->m_vecChild.push_back(pCfgMagicOp);

		// 特效名检查
		string strFX = TableFile.GetString(uLineNum, szMagicEff_FXName);
		if (strFX != "")
		{
			vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
			if (sFXTable.size() == 2)
			{
				string strFXName	= sFXTable[1];
				if (strFXName == "")
				{
					stringstream ExpStr;
					ExpStr << "第" << uLineNum << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
					GenExpInfo(ExpStr.str());
				}
			}
			else
			{
				stringstream ExpStr;
				ExpStr << "第" << uLineNum << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
				GenExpInfo(ExpStr.str());
			}
		}
	}
	ms_uMaxLineNum = ms_uMaxLineNum + TableFile.GetHeight();
	return true;
}

void CCfgMagicEffCheck::EndCheck()
{
	ClearMap(ms_mapMagicEff);
	ClearMap(ms_mapMagicOp);
}

CCfgMagicEffCheck::~CCfgMagicEffCheck()
{
	ClearVector(m_vecChild);
}

bool CCfgMagicEffCheck::CheckMagicEffExist(const string& sValue)
{
	if (sValue.empty())
		return false;

	MapMagicEff::iterator iter = ms_mapMagicEff.find(sValue);
	if (iter == ms_mapMagicEff.end())
	{
		CfgChk::GenExpInfo("违反列约束：魔法效果不存在", sValue);
		return false;
	}

	return true;
}

bool CCfgMagicEffCheck::CheckMagicOpAvail()
{
	CfgChk::SetTabName("魔法效果");
	MapMagicEff::iterator itr = ms_mapMagicEff.begin();
	for(; itr != ms_mapMagicEff.end(); ++itr)
	{
		VecChild::iterator itrChild = itr->second->m_vecChild.begin();
		for (; itrChild != itr->second->m_vecChild.end(); ++ itrChild)
		{
			if(!(*itrChild)->CheckMagicOpAvail())
				return false;
		}
	}
	CfgChk::SetTabNull();
	
	return true;
}

EValueMagicOpType CCfgMagicEffCheck::GetFirstOpValueMagicOpType()
{
	VecChild::iterator itrChild = m_vecChild.begin();
	for (; itrChild != m_vecChild.end(); ++ itrChild)
	{
		EValueMagicOpType eValueMagicOpType = (*itrChild)->GetOpValueMagicOpType();
		if(eValueMagicOpType != eVMOT_End)
		{
			return eValueMagicOpType;
		}
	}
	return eVMOT_None;
}

void CCfgMagicEffCheck::BuildMOPMap()
{	
	ms_mapMagicOp["治疗计算"]					= new CCureMop;
	ms_mapMagicOp["主手物理攻击伤害计算"]		= new CCalculateMainHandPhysicsHurtMop;
	ms_mapMagicOp["副手物理攻击伤害计算"]		= new CCalculateAssistantPhyicsHurtMop;
	ms_mapMagicOp["自然系法术攻击伤害计算"]		= new CCalNatureMagHurtMop;
	ms_mapMagicOp["破坏系法术攻击伤害计算"]		= new CCalDestructionMagHurtMop;
	ms_mapMagicOp["暗黑系法术攻击伤害计算"]		= new CCalEvilMagHurtMop;
	ms_mapMagicOp["DOT伤害计算"]				= new CCalDOTHurtMop;
	ms_mapMagicOp["无类型伤害计算"]				= new CCalNonTypeHurtMop;

	ms_mapMagicOp["主手物理攻击暴击伤害计算"]		= new CCalculateMainHandPhysicsStrikeMop;
	ms_mapMagicOp["主手物理攻击无暴击伤害计算"]		= new CCalculateMainHandPhysicsNoStrikeHurtMop;
	ms_mapMagicOp["主手物理攻击只命中暴击伤害计算"]	= new CCalculateMainHandPhysicsOnlyHitAndStrikeHurtMop;
	ms_mapMagicOp["自然系法术攻击无暴击伤害计算"]	= new CCalNatureMagNoStrikeHurtMop;
	ms_mapMagicOp["破坏系法术攻击无暴击伤害计算"]	= new CCalDestructionMagNoStrikeHurtMop;
	ms_mapMagicOp["暗黑系法术攻击无暴击伤害计算"]	= new CCalEvilMagNoStrikeHurtMop;
	ms_mapMagicOp["破坏系法术攻击无视抗性伤害计算"]	= new CCalDestructionMagNoResistHurtMop;
	ms_mapMagicOp["DOT无暴击伤害计算"]				= new CCalDOTNoStrikeHurtMop;
	ms_mapMagicOp["造成任务非战斗伤害"]			= new CTaskNonBattleHurtMOP;
	ms_mapMagicOp["造成任务炸弹伤害"]			= new CTaskBombHurtMOP;
	ms_mapMagicOp["造成任务特殊伤害"]			= new CTaskSpecialHurtMOP;
	ms_mapMagicOp["造成非国战建筑伤害"]			= new CTaskNonNationBattleBuildHurtMOP;

	ms_mapMagicOp["重生满血蓝"]					= new CFillHPMPMOP;
	ms_mapMagicOp["改变生命值"]					= new CChangeHPMOP;
	ms_mapMagicOp["吸收魔法值"]					= new CAbsorbManaPointMOP;
	ms_mapMagicOp["改变魔法值"]					= new CChangeManaPointMOP;
	ms_mapMagicOp["改变怒气值"]					= new CChangeRagePointMOP;
	ms_mapMagicOp["吸收怒气值"]					= new CAbsorbRagePointMOP;
	ms_mapMagicOp["改变能量值"]					= new CChangeEnergyPointMOP;
	ms_mapMagicOp["改变连击点"]					= new CChangeComboPointMOP;
	ms_mapMagicOp["目标改变生命值"]				= new CTargetChangeHPMOP;
	ms_mapMagicOp["吸血"]						= new CSuckBloodMOP;
	ms_mapMagicOp["消耗生命值"]					= new CNoEventChangeHPMOP;	
	ms_mapMagicOp["天赋改变魔法值"]				= new CTalentChangeManaPointMOP;
	ms_mapMagicOp["药品改变生命值"]				= new CDrugChangeHPMOP;
	ms_mapMagicOp["无视免疫改变生命值"]			= new CIgnoreImmuneChangeHPMOP;

	ms_mapMagicOp["改变生命值上限"]				= new CChangeMaxHealthPointAdderMOP;
	ms_mapMagicOp["改变生命值上限百分比"]		= new CChangeMaxHealthPointMultiplierMOP;
	ms_mapMagicOp["改变魔法值上限"]				= new CChangeMaxManaPointAdderMOP;
	ms_mapMagicOp["改变魔法值上限百分比"]		= new CChangeMaxManaPointMultiplierMOP;

	ms_mapMagicOp["改变生命值恢复百分比"]		= new CChangeHPUpdateRateMultiplierMOP;
	ms_mapMagicOp["改变魔法值恢复百分比"]		= new CChangeMPUpdateRateMultiplierMOP;
	ms_mapMagicOp["改变怒气值衰减速度"]			= new CChangeRPUpdateValueAdderMOP;
	ms_mapMagicOp["改变怒气值衰减百分比"]		= new CChangeRPUpdateValueMultiplierMOP;
	ms_mapMagicOp["改变能量值恢复速度"]			= new CChangeEPUpdateValueAdderMOP;
	ms_mapMagicOp["改变能量值恢复百分比"]		= new CChangeEPUpdateValueMultiplierMOP;
	ms_mapMagicOp["改变魔法消耗系数"]			= new CChangeMPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变怒气消耗系数"]			= new CChangeRPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变能量消耗系数"]			= new CChangeEPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变自然系魔法消耗系数"]		= new CChangeNatureMPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变破坏系魔法消耗系数"]		= new CChangeDestructionMPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变黑暗系魔法消耗系数"]		= new CChangeEvilMPConsumeRateMultiplierMOP;
	ms_mapMagicOp["改变天赋修正获得怒气倍数"]	= new CChangeRPProduceRateMultiplierMOP;

	ms_mapMagicOp["改变护甲值"]					= new CChangeDefenceAdderMOP;
	ms_mapMagicOp["改变护甲值百分比"]			= new CChangeDefenceMultiplierMOP;
	ms_mapMagicOp["改变攻击力"]					= new CChangePhysicalDPSAdderMOP;
	ms_mapMagicOp["改变攻击力百分比"]			= new CChangePhysicalDPSMultiplierMOP;
	ms_mapMagicOp["改变韧性值"]					= new CChangeResilienceValueAdderMOP;
	ms_mapMagicOp["改变韧性值百分比"]			= new CChangeResilienceValueMultiplierMOP;
	ms_mapMagicOp["改变免暴值"]					= new CChangeStrikeResistanceValueAdderMOP;
	ms_mapMagicOp["改变免暴值百分比"]			= new CChangeStrikeResistanceValueMultiplierMOP;
	ms_mapMagicOp["改变精准值"]					= new CChangeAccuratenessValueAdderMOP;
	ms_mapMagicOp["改变精准值百分比"]			= new CChangeAccuratenessValueMultiplierMOP;
	ms_mapMagicOp["改变爆击值"]					= new CChangeStrikeValueAdderMOP;
	ms_mapMagicOp["改变爆击值百分比"]			= new CChangeStrikeValueMultiplierMOP;
	ms_mapMagicOp["改变爆击倍数值"]				= new CChangeStrikeMultiValueAdderMOP;
	ms_mapMagicOp["改变爆击倍数值百分比"]		= new CChangeStrikeMultiValueMultiplierMOP;
	ms_mapMagicOp["改变格挡率百分比"]			= new CChangeBlockRateMultiplierMOP;
	ms_mapMagicOp["改变格挡减免值"]				= new CChangeBlockDamageAdderMOP;
	ms_mapMagicOp["改变格挡减免值百分比"]		= new CChangeBlockDamageMultiplierMOP;
	ms_mapMagicOp["改变物理躲避值"]				= new CChangePhysicalDodgeValueAdderMOP;
	ms_mapMagicOp["改变物理躲避值百分比"]		= new CChangePhysicalDodgeValueMultiplierMOP;
	ms_mapMagicOp["改变招架值"]					= new CChangeParryValueAdderMOP;
	ms_mapMagicOp["改变招架值百分比"]			= new CChangeParryValueMultiplierMOP;
	ms_mapMagicOp["改变法术躲避值"]				= new CChangeMagicDodgeValueAdderMOP;
	ms_mapMagicOp["改变法术躲避值百分比"]		= new CChangeMagicDodgeValueMultiplierMOP;
	ms_mapMagicOp["改变法术命中值"]				= new CChangeMagicHitValueAdderMOP;
	ms_mapMagicOp["改变法术命中值百分比"]		= new CChangeMagicHitValueMultiplierMOP;

	ms_mapMagicOp["改变移动速度百分比"]			= new CChangePercentMoveSpeedMOP;
	ms_mapMagicOp["改变步行速度百分比"]			= new CChangePercentWalkSpeedMOP;
	ms_mapMagicOp["改变普通攻击速度百分比"]		= new CChangePercentNormalAttackSpeedMOP;

	ms_mapMagicOp["改变附加穿刺伤害值"]			= new CChangePunctureDamageAdderMOP;
	ms_mapMagicOp["改变附加穿刺伤害值百分比"]	= new CChangePunctureDamageMultiplierMOP;
	ms_mapMagicOp["改变附加砍斫伤害值"]			= new CChangeChopDamageAdderMOP;
	ms_mapMagicOp["改变附加砍斫伤害值百分比"]	= new CChangeChopDamageMultiplierMOP;
	ms_mapMagicOp["改变附加钝击伤害值"]			= new CChangeBluntDamageAdderMOP;
	ms_mapMagicOp["改变附加钝击伤害值百分比"]	= new CChangeBluntDamageMultiplierMOP;
	ms_mapMagicOp["改变附加法术伤害值"]			= new CChangeMagicDamageValueAdderMOP;
	ms_mapMagicOp["改变附加法术伤害值百分比"]	= new CChangeMagicDamageValueMultiplierMOP;
	ms_mapMagicOp["改变附加自然系伤害值"]		= new CChangeNatureDamageValueAdderMOP;
	ms_mapMagicOp["改变附加自然系伤害值百分比"]	= new CChangeNatureDamageValueMultiplierMOP;
	ms_mapMagicOp["改变附加破坏系伤害值"]		= new CChangeDestructionDamageValueAdderMOP;
	ms_mapMagicOp["改变附加破坏系伤害值百分比"]	= new CChangeDestructionDamageValueMultiplierMOP;
	ms_mapMagicOp["改变附加暗黑系伤害值"]		= new CChangeEvilDamageValueAdderMOP;
	ms_mapMagicOp["改变附加暗黑系伤害值百分比"]	= new CChangeEvilDamageValueMultiplierMOP;

	ms_mapMagicOp["改变自然系抗性值"]			= new CChangeNatureResistanceValueAdderMOP;
	ms_mapMagicOp["改变自然系抗性值百分比"]		= new CChangeNatureResistanceValueMultiplierMOP;
	ms_mapMagicOp["改变暗黑系抗性值"]			= new CChangeEvilResistanceValueAdderMOP;
	ms_mapMagicOp["改变暗黑系抗性值百分比"]		= new CChangeEvilResistanceValueMultiplierMOP;
	ms_mapMagicOp["改变破坏系抗性值"]			= new CChangeDestructionResistanceValueAdderMOP;
	ms_mapMagicOp["改变破坏系抗性值百分比"]		= new CChangeDestructionResistanceValueMultiplierMOP;

	ms_mapMagicOp["改变技能附加物理伤害"]			= new CChangePhysicalDamageAdderMOP;
	ms_mapMagicOp["改变技能附加物理伤害百分比"]		= new CChangePhysicalDamageMultiplierMOP;
	ms_mapMagicOp["改变技能附加副手物理伤害"]		= new CChangeAssistPhysicalDamageAdderMOP;
	ms_mapMagicOp["改变技能附加副手物理伤害百分比"]	= new CChangeAssistPhysicalDamageMultiplierMOP;
	ms_mapMagicOp["改变技能附加法术伤害"]			= new CChangeMagicDamageAdderMOP;
	ms_mapMagicOp["改变自身技能附加法术伤害"]		= new CChangeSelfMagicDamageAdderMOP;
	ms_mapMagicOp["改变技能附加法术伤害百分比"]		= new CChangeMagicDamageMultiplierMOP;
	ms_mapMagicOp["改变技能附加DOT伤害"]			= new CChangeDOTDamageAdderMOP;
	ms_mapMagicOp["改变技能附加DOT伤害百分比"]		= new CChangeDOTDamageMultiplierMOP;

	ms_mapMagicOp["改变施法流程抗中断率"]			= new CChangeResistCastingInterruptionRateMultiplierMOP;
	ms_mapMagicOp["改变穿透值"]						= new CChangePenetrationValueAdderMOP;
	ms_mapMagicOp["改变穿透值百分比"]				= new CChangePenetrationValueMultiplierMOP;
	ms_mapMagicOp["改变防护值"]						= new CChangeProtectionValueAdderMOP;
	ms_mapMagicOp["改变防护值百分比"]				= new CChangeProtectionValueMultiplierMOP;

	ms_mapMagicOp["无视免疫安装魔法状态"]	= new CIgnoreImmuneSetupSpecialStateMOP;
	ms_mapMagicOp["被动安装魔法状态"]		= new CPassiveSetupMagicStateMOP;
	ms_mapMagicOp["安装魔法状态"]			= new CSetupMagicStateMOP;
	ms_mapMagicOp["安装触发器状态"]			= new CSetupTriggerStateMOP;
	ms_mapMagicOp["安装伤害变更状态"]		= new CSetupDamageChangeStateMOP;
	ms_mapMagicOp["安装积累触发状态"]		= new CSetupCumuliTriggerStateMOP;
	ms_mapMagicOp["安装特殊状态"]			= new CSetupSpecialStateMOP;
	ms_mapMagicOp["只安装不撤销魔法状态"]	= new CSetupUncancellableMagicStateMOP;
	ms_mapMagicOp["按最高技能等级安装魔法状态"]	= new CSetupMagicStateByMaxSkillLevelMOP;
	ms_mapMagicOp["撤销魔法状态叠加层数"]	= new CCancelMagicStateCascadeMOP;
	ms_mapMagicOp["按效果撤销魔法状态叠加层数"]	= new CCancelMagicStateCascadeByEffectMOP;
	ms_mapMagicOp["触发触发器状态"]			= new CTriggerStateMessageEventMOP; 
	ms_mapMagicOp["刷新魔法状态时间"]		= new CResetMagicStateTimeMOP;
	ms_mapMagicOp["增加魔法状态时间"]		= new CAddMagicStateTimeMOP;
	ms_mapMagicOp["设置伤害变更状态累积值"] = new CSetDamageChangeStateAccumulateMOP;
	ms_mapMagicOp["设置积累触发状态累积值"] = new CSetCumuliTriggerStateAccumulateMOP;
	ms_mapMagicOp["安装或清除姿态"]			= new CSetupOrEraseAureMagicMOP;
	ms_mapMagicOp["安装姿态"]				= new CSetupAureMagicMOP;
	ms_mapMagicOp["切换姿态"]				= new CChangeAureMagicMOP;
	ms_mapMagicOp["清除姿态"]				= new CEraseAureMagicMOP;
	ms_mapMagicOp["撤销时清除姿态"]			= new CEraseAureMagicOnCancellingMOP;
	ms_mapMagicOp["施放位移魔法"]			= new CRelMoveMagicMOP;
	ms_mapMagicOp["施放可撤销位移魔法"]		= new CCancelableRelMoveMagicMOP;
	ms_mapMagicOp["施放地点位移魔法"]		= new CRelPosMoveMagicMOP;
	ms_mapMagicOp["施放可传递魔法"]			= new CRelTransferMagicMOP;
	ms_mapMagicOp["施放子弹魔法"]			= new CRelBulletMagicMOP;
	ms_mapMagicOp["施放延时地点魔法"]		= new CDelayRelPositionMagicMOP;
	ms_mapMagicOp["施放延时子弹魔法"]		= new CDelayRelBulletMagicMOP;
	ms_mapMagicOp["施放地点子弹魔法"]		= new CRelPosBulletMagicMOP;
	ms_mapMagicOp["施放冲击波魔法"]			= new CRelShockWaveMagicMOP;
	ms_mapMagicOp["施放延时冲击波魔法"]		= new CDelayRelShockWaveMagicMOP;
	ms_mapMagicOp["施放地点魔法"]			= new CRelPositionMagicMOP;
	ms_mapMagicOp["按初始朝向和距离施放地点魔法"]	= new CRelPositionMagicByDirAndDistanceMOP;
	ms_mapMagicOp["按目标位置施放地点魔法"]	= new CRelPositionMagicAtTargetPosMOP;
	ms_mapMagicOp["按目标位置施放冲击波魔法"]	= new CRelShockWaveMagicAtTargetPosMOP;
	ms_mapMagicOp["范围内随机释放地点魔法"]	= new CRelPositionMagicRandomInAreaMOP;
	ms_mapMagicOp["朝冲击波目的点施放位移魔法"]	= new CRelMoveMagicAtShockwaveMagicPosMOP;
	ms_mapMagicOp["施放延时魔法效果"]		= new CRelTickMOP;
	ms_mapMagicOp["施放图腾魔法"]			= new CCreateTotemMOP;
	ms_mapMagicOp["按初始朝向和距离施放图腾魔法"]	= new CCreateTotemByDirAndDistanceMOP;
	ms_mapMagicOp["销毁图腾魔法"]			= new CDestroyServantMOP;
	ms_mapMagicOp["施放阵法魔法"]			= new CRelBattleArrayMagicMOP;
	ms_mapMagicOp["创建宠物"]				= new CCreateServantMOP;
	ms_mapMagicOp["创建宠物并设置目标"]		= new CCreateServantWithTargetLevelFollowMasterMOP;
	ms_mapMagicOp["设置怪物卡技能等级为主人技能等级"]		= new CSetMonsterCardSkillLevelByMasterSkillLevelMOP;
	ms_mapMagicOp["销毁宠物"]				= new CDestroyServantMOP;
	ms_mapMagicOp["控制宠物"]				= new CControlServantMOP;
	ms_mapMagicOp["撤销时销毁宠物"]			= new CDestroyServantOnCancellingMOP;
	ms_mapMagicOp["创建分身"]				= new CCreateShadowMOP;
	ms_mapMagicOp["自身变身为目标的镜像"]	= new CBecomeTargetShadowMOP;
	ms_mapMagicOp["设置宠物生存时间"]		= new CSetServantLeftTimeMOP;
	ms_mapMagicOp["清除召唤兽仇恨"]			= new CRemoveServantEnemyMOP;
	ms_mapMagicOp["地点特效"]				= new CPostionPlayFXMOP;

	ms_mapMagicOp["使用普通坐骑"]			= new CUseNormalHorseMOP;
	ms_mapMagicOp["使用战斗坐骑"]			= new CUseBattleHorseMOP;
	ms_mapMagicOp["召唤战斗坐骑"]			= new CCallBattleHorseMOP;
	ms_mapMagicOp["收回骑乘坐骑"]			= new CTakeBackRidingHorseMOP;

	ms_mapMagicOp["设置召唤兽目标"]			= new CSetServantTargetMOP;
	ms_mapMagicOp["使召唤兽攻击目标"]		= new CLetServantAttackTargetMOP;
	ms_mapMagicOp["清除召唤兽对目标的仇恨"] = new CRemoveEnemyFromServantEnmityListMOP;
	ms_mapMagicOp["创建NPC"]				= new CCreateNPCMOP;	
	ms_mapMagicOp["删除NPC"]				= new CDestoryNPCMOP;
	ms_mapMagicOp["创建被动NPC"]			= new CCreatePassiveNPCMOP;	
	ms_mapMagicOp["地点创建NPC"]			= new CPosCreateNPCMOP;
	ms_mapMagicOp["地点创建NPC等级随主角"]	= new CPosCreateNPCLevelSameMOP;
	ms_mapMagicOp["指定地点创建NPC"]		= new CTargetPosCreateNPCMOP;
	ms_mapMagicOp["指定地点创建NPC等级随主角"] = new CTargetPosCreateNPCLevelSameMOP;
	ms_mapMagicOp["目标位置创建NPC"]		= new CCreateNPCOnTargetPosMOP;
	ms_mapMagicOp["指定地点延时创建NPC"]	= new CPosDelayCreateNPCMOP;
	ms_mapMagicOp["地点创建尸体"]			= new CPosCreateDeadNPCMOP;	
	ms_mapMagicOp["安装或清除触发器状态"]	= new CSetupOrEraseTriggerStateMOP;
	ms_mapMagicOp["安装或清除魔法状态"]		= new CSetupOrEraseMagicStateMOP;
	ms_mapMagicOp["安装多层魔法状态"]		= new CSetupMultiMagicStateMOP;
	ms_mapMagicOp["停止触发事件"]			= new CDisTriggerEventMOP;
	ms_mapMagicOp["设置相同目标"]			= new CSetSameTargetMOP;
	ms_mapMagicOp["设置目标为自身"]			= new CSetTargetSelfMOP;
	ms_mapMagicOp["清除目标"]				= new CSetTargetNULLMOP;
	ms_mapMagicOp["替换技能"]				= new CReplaceSkillMOP;
	ms_mapMagicOp["指定地点创建Npc并设置主人"]	= new CPosCreateNpcWithMasterMOP;

	// CLEAN UP START
	// 目标共用
	ms_mapMagicOp["播放全身动作"]			= new CDoAllBodyActionMOP;
	ms_mapMagicOp["目标脱离战斗状态"]		= new CTargetLeftBattleStateMOP;
	ms_mapMagicOp["目标禁止移动"]			= new CTargetForbitMoveMOP;
	ms_mapMagicOp["目标禁止转向"]			= new CTargetForbitTurnAroundMOP;
	ms_mapMagicOp["目标撤销普攻"]			= new CTargetCancelNAMOP;
	ms_mapMagicOp["目标暂停普攻"]			= new CTargetSuspendNAMOP;
	ms_mapMagicOp["目标禁止普通攻击"]		= new CTargetForbitNormalAttackMOP;
	ms_mapMagicOp["目标禁止施放技能"]		= new CTargetForbitUseSkillMOP;
	ms_mapMagicOp["目标禁止施放自然系法术"]	= new CTargetForbitUseNatureSkillMOP;
	ms_mapMagicOp["目标禁止施放破坏系法术"]	= new CTargetForbitUseDestructionSkillMOP;
	ms_mapMagicOp["目标禁止施放暗黑系法术"]	= new CTargetForbitUseEvilSkillMOP;
	ms_mapMagicOp["目标禁止客户端操作"]		= new CTargetForbitClientOperateMOP;
	ms_mapMagicOp["目标禁止施放职业技能"]	= new CTargetForbitUseClassSkillMOP;
	ms_mapMagicOp["目标禁止施放药品技能"]	= new CTargetForbitUseDrugItemSkillMOP;
	ms_mapMagicOp["目标禁止施放非战斗技能"]	= new CTargetForbitUseNotFightSkillMOP;
	ms_mapMagicOp["目标禁止施放玩法物品技能"]	= new CTargetForbitUseMissionItemSkillMOP;

	// 目标特殊
	ms_mapMagicOp["假死"]					= new CFeignDeathMOP;
	ms_mapMagicOp["法术反射"]				= new CReflectMagicMOP;
	ms_mapMagicOp["嘲讽"]					= new CTauntMOP;
	ms_mapMagicOp["重生复活"]				= new CRenascenceMOP;
	ms_mapMagicOp["原地重生"]				= new CRebornWhereYouBeMOP;
	ms_mapMagicOp["允许复活"]				= new CPermitRebornMOP;
	ms_mapMagicOp["持续回蓝"]				= new CContinueAddMPMOP;
	ms_mapMagicOp["停止运动"]				= new CStopMoveMOP;
	ms_mapMagicOp["暴气"]					= new CBreakOutMOP;
	ms_mapMagicOp["灵魂链接"]				= new CSoulLinkMOP;
	ms_mapMagicOp["玩家施放被动技能"]		= new CRelPassiveSkillMOP;
	ms_mapMagicOp["NPC施放技能"]			= new CRelNPCSkillMOP;
	ms_mapMagicOp["NPC施放方向技能"]		= new CRelNPCDirSkillMOP;
	ms_mapMagicOp["缴械"]					= new CForbitUseWeaponMOP;
	ms_mapMagicOp["普攻产生怒气"]			= new CNormalAttackProduceRagePoint;
	ms_mapMagicOp["被伤害产生怒气"]			= new CBeDamagePreduceRagePoint;
	ms_mapMagicOp["互换位置"]				= new CSwapPositionMOP;
	ms_mapMagicOp["对经验拥有者奖励魂"]		= new CBurstSoulPrizeToExpOwnerMOP;
	ms_mapMagicOp["对经验拥有者奖励魂附加"]	= new CCBurstSoulExtraPrizeToExpOwnerMOP;
	ms_mapMagicOp["增加爆魂蓄力值"]			= new CAddBurstSoulTimesMOP;
	ms_mapMagicOp["增加爆魂连击数"]			= new CAddContinuousBurstTimesMOP;
	ms_mapMagicOp["对经验拥有者增加爆魂蓄力值"]	= new CAddBurstSoulTimesToExpOwnerMOP;
	ms_mapMagicOp["目标自己安装魔法状态"]	= new CTargetSetupMagicStateToSelfMOP;
	ms_mapMagicOp["隐身"]					= new CTargetSetHiddenMOP;
	ms_mapMagicOp["接管运输车"]				= new CTakeOverTRuckMop;
	ms_mapMagicOp["自身移动到目标位置"]		= new CMoveSelfToTargetPosMop;


	// 地点特殊
	ms_mapMagicOp["猎杀"]					= new CPursueKillMOP;
	ms_mapMagicOp["自杀式爆炸"]				= new CSelfMurderBlastMOP;
	ms_mapMagicOp["施放障碍"]				= new CSetupDynamicBarrierMOP;
	ms_mapMagicOp["使用尸体"]				= new CUseDeadBobyMOP;
	// CLEAN UP END

	ms_mapMagicOp["施放世界技能"]		= new CDoWorldSkillMOP;
	ms_mapMagicOp["施放阵营技能"]				= new CDoCampSkillMOP;
	ms_mapMagicOp["施放佣兵团技能"]				= new CDoArmyCorpsSkillMOP;
	ms_mapMagicOp["施放佣兵小队技能"]				= new CDoTongSkillMOP;

	ms_mapMagicOp["改变普攻动作速率"]		= new CChangeNAActionRhythmMOP;
	ms_mapMagicOp["特效操作"]				= new COnlyPlayFX;
	ms_mapMagicOp["文字特效操作"]				= new CDisplayMsgMOP;

	ms_mapMagicOp["清除位移魔法"]			= new CEraseMoveMagicMOP;
	ms_mapMagicOp["清除敌对地点魔法"]		= new CEraseEnemyPositionMagicMOP;
	ms_mapMagicOp["清除地点魔法"]			= new CErasePositionMagicMOP;
	ms_mapMagicOp["清除所有地点魔法"]		= new CEraseAllPositionMagicMOP;
	ms_mapMagicOp["替换地点魔法"]			= new CReplacePositionMagicMOP;
	ms_mapMagicOp["清除所有状态"]			= new CEraseAllBufferMOP;
	ms_mapMagicOp["清除所有负面状态"]		= new CEraseAllDecreaseStateMOP;
	ms_mapMagicOp["清除移动限制负面状态"]	= new CEraseMoveDecreaseStateMOP;
	ms_mapMagicOp["清除减速负面状态"]		= new CEraseSpeedDecreaseStateMOP;
	ms_mapMagicOp["清除正面状态"]			= new CEraseIncreaseStateMOP;
	ms_mapMagicOp["清除负面状态"]			= new CEraseStateByDecreaseTypeMOP;
	ms_mapMagicOp["随机清除一个负面状态"]	= new CRandEraseAllDecreaseStateMOP;
	ms_mapMagicOp["随机清除一个正面状态"]	= new CRandEraseIncreaseStateMOP;
	ms_mapMagicOp["清除坐骑状态"]			= new CEraseRidingStateMOP;
	ms_mapMagicOp["清除状态"]				= new CEraseStateMOP;
	ms_mapMagicOp["清除自身的状态"]			= new CEraseSelfStateMOP;
	ms_mapMagicOp["撤销时清除状态"]			= new CEraseStateOnCancellingMOP;
	ms_mapMagicOp["抵抗控制状态"]			= new CResistControlMOP;
	ms_mapMagicOp["抵抗定身状态"]			= new CResistPauseMOP;
	ms_mapMagicOp["抵抗减速状态"]			= new CResistCripplingMOP;
	ms_mapMagicOp["抵抗DOT状态"]			= new CResistDOTMOP;
	ms_mapMagicOp["反射负面状态"]			= new CReflectStateByDecreaseTypeMOP;
	ms_mapMagicOp["重置单项冷却时间"]		= new CResetSingleCoolDownTimeMOP;
	ms_mapMagicOp["所有技能冷却结束"]		= new CAllSkillReadyMOP;
	ms_mapMagicOp["所有战斗技能冷却结束"]	= new CAllFightSkillReadyMOP;
	ms_mapMagicOp["下个法术无吟唱时间"]		= new CSetNoSingTimeAboutNextSkill;
	ms_mapMagicOp["清除全部法术吟唱时间"]	= new CSetNoSingTimeForever;
	ms_mapMagicOp["锁定易变值"]				= new CLockAgileValueMOP;
	ms_mapMagicOp["改变目标为障碍"]			= new CChangeTargetToBarrierMOP;

	ms_mapMagicOp["执行手部动作"]			= new CHandActionMOP;
	ms_mapMagicOp["执行脚部动作"]			= new CFootActionMOP;

	ms_mapMagicOp["目标施放子弹魔法"]		= new CTargetRelBulletMagicMOP;
	ms_mapMagicOp["爆魂特效"]				= new CTargetBurstSoulFXMOP;
	ms_mapMagicOp["目标爆魂特效"]			= new CBurstSoulFXMOP;
	ms_mapMagicOp["爆魂弹球特效"]			= new CBurstSoulBallFXMOP;
	ms_mapMagicOp["按等级消耗爆魂蓄力值"]	= new CConsumeBurstSoulTimesMOP;
	ms_mapMagicOp["归阵"]					= new CReturnBattleArrayMOP;
	ms_mapMagicOp["改变阵营"]				= new CChangeCampMOP;

	//特殊相关
	ms_mapMagicOp["非战斗行为操作"]			= new CNonCombatBehaviorMOP;
	ms_mapMagicOp["非战斗行为状态操作"]		= new CNonCombatStateMagicOp;
	ms_mapMagicOp["改变最终经验比率"]		= new CChangeExpRateMOP;
	ms_mapMagicOp["改变副本经验比率"]		= new CChangeExpRateInFBMOP;
	ms_mapMagicOp["改变最终魂值比率"]		= new CChangeFetchRateMOP;
	ms_mapMagicOp["回城"]					= new CChangeSceneMOP;
	ms_mapMagicOp["改变隐身值"]				= new CChangeStealthMOP;
	ms_mapMagicOp["屏蔽隐身值修改"]			= new CDisableStealthMOP;
	ms_mapMagicOp["改变敏捷值"]				= new CChangeKeennessMOP;
	ms_mapMagicOp["屏蔽敏捷值修改"]			= new CDisableKeennessMOP;
	//End

	ms_mapMagicOp["改变自然碾压值"]				= new CChangeNatureSmashValueAdderMOP;
	ms_mapMagicOp["改变自然碾压值百分比"]		= new CChangeNatureSmashValueMultiplierMOP;
	ms_mapMagicOp["改变破坏碾压值"]				= new CChangeDestructionSmashValueAdderMOP;
	ms_mapMagicOp["改变破坏碾压值百分比"]		= new CChangeDestructionSmashValueAdderMOP;
	ms_mapMagicOp["改变暗黑碾压值"]				= new CChangeEvilSmashValueAdderMOP;
	ms_mapMagicOp["改变暗黑碾压值百分比"]		= new CChangeEvilSmashValueMultiplierMOP;
	ms_mapMagicOp["改变护甲碾压值"]				= new CChangeDefenceSmashValueAdderMOP;
	ms_mapMagicOp["改变护甲碾压值百分比"]		= new CChangeDefenceSmashValueMultiplierMOP;

	ms_mapMagicOp["改变最终附加DOT伤害加成百分比"]		= new CChangeExtraDOTDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加穿刺伤害加成百分比"]		= new CChangeExtraPunctureDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加砍斫伤害加成百分比"]		= new CChangeExtraChopDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加钝击伤害加成百分比"]		= new CChangeExtraBluntDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加自然伤害加成百分比"]		= new CChangeExtraNatureDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加暗黑伤害加成百分比"]		= new CChangeExtraEvilDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加破坏伤害加成百分比"]		= new CChangeExtraDestructionDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加弓武器伤害加成百分比"]	= new CChangeExtraBowDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加弩武器伤害加成百分比"]	= new CChangeExtraCrossBowDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加双手武器伤害加成百分比"] = new CChangeExtraTwohandWeaponDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加长柄武器伤害加成百分比"] = new CChangeExtraPolearmDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加骑士武器伤害加成百分比"] = new CChangeExtraPaladinWeaponDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加副手武器伤害加成百分比"] = new CChangeExtraAssistWeaponDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加远程武器伤害加成百分比"] = new CChangeExtraLongDistDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终附加近程武器伤害加成百分比"] = new CChangeExtraShortDistDamageRateMultiplierMOP;
	ms_mapMagicOp["改变远程武器近身伤害修正比例百分比"] = new CChangeLongDistWeaponDamageRateMultiplierMOP;
	ms_mapMagicOp["改变最终伤害减免百分比"]				= new CChangeDamageDecsRateMultiplierMOP;
	ms_mapMagicOp["改变最终治疗值加成百分比"]			= new CChangeExtraCureValueRateMultiplierMOP;
	ms_mapMagicOp["改变最终被治疗值加成百分比"]			= new CChangeExtraBeCuredValueRateMultiplierMOP;
	ms_mapMagicOp["改变控制状态抵抗率百分比"]			= new CChangeControlDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变定身状态抵抗率百分比"]			= new CChangePauseDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变减速状态抵抗率百分比"]			= new CChangeCripplingDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变Debuff状态抵抗率百分比"]			= new CChangeDebuffDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变DOT状态抵抗率百分比"]			= new CChangeDOTDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变特殊状态抵抗率百分比"]			= new CChangeSpecialDecreaseResistRateMultiplierMOP;
	ms_mapMagicOp["改变控制时间百分比"]					= new CChangeControlDecreaseTimeRateMultiplierMOP;
	ms_mapMagicOp["改变定身时间百分比"]					= new CChangePauseDecreaseTimeRateMultiplierMOP;
	ms_mapMagicOp["改变减速时间百分比"]					= new CChangeCripplingDecreaseTimeRateMultiplierMOP;
	ms_mapMagicOp["改变特殊时间百分比"]					= new CChangeSpecialDecreaseTimeRateMultiplierMOP;
	ms_mapMagicOp["改变最终物理防护率加成百分比"]		= new CChangeExtraPhysicDefenceRateMultiplierMOP;
	ms_mapMagicOp["改变最终物理躲避率加成百分比"]		= new CChangeExtraPhysicDodgeRateMultiplierMOP;
	ms_mapMagicOp["改变最终招架率加成百分比"]			= new CChangeExtraParryRateMultiplierMOP;
	ms_mapMagicOp["改变最终暴击率加成百分比"]			= new CChangeExtraStrikeRateMultiplierMOP;
	ms_mapMagicOp["改变最终法术躲避加成百分比"]			= new CChangeExtraMagicDodgeRateMultiplierMOP;
	ms_mapMagicOp["改变最终法术抵抗率加成百分比"]		= new CChangeExtraMagicResistanceRateMultiplierMOP;
	ms_mapMagicOp["改变最终自然抵抗率加成百分比"]		= new CChangeExtraNatureResistanceRateMultiplierMOP;
	ms_mapMagicOp["改变最终破坏抵抗率加成百分比"]		= new CChangeExtraDestructionResistanceRateMultiplierMOP;
	ms_mapMagicOp["改变最终暗黑抵抗率加成百分比"]		= new CChangeExtraEvilResistanceRateMultiplierMOP;
	ms_mapMagicOp["改变最终完全抵抗率加成百分比"] 		= new CChangeExtraCompleteResistanceRateMultiplierMOP;
	ms_mapMagicOp["改变未命中百分比"]					= new CChangeMissRateMultiplierMOP;

	ms_mapMagicOp["改变施法时间百分比"]					= new CChangeCastingProcessTimeRatioMultiplierMOP;

	ms_mapMagicOp["改变NPC攻击范围成"]					= new CChangeNpcAttackScopeToMOP;

	ms_mapMagicOp["改变免疫率百分比"]					= new CChangeImmuneRateMultiplierMOP;
	ms_mapMagicOp["免疫穿刺伤害"]						= new CImmunePunctureDamageMOP;
	ms_mapMagicOp["免疫砍斫伤害"]						= new CImmuneChopDamageMOP;
	ms_mapMagicOp["免疫钝击伤害"]						= new CImmuneBluntDamageMOP;
	ms_mapMagicOp["免疫自然伤害"]						= new CImmuneNatureDamageMOP;
	ms_mapMagicOp["免疫破坏伤害"]						= new CImmuneDestructionDamageMOP;
	ms_mapMagicOp["免疫暗黑伤害"]						= new CImmuneEvilDamageMOP;
	ms_mapMagicOp["免疫控制状态"]						= new CImmuneControlDecreaseMOP;
	ms_mapMagicOp["免疫定身状态"]						= new CImmunePauseDecreaseMOP;
	ms_mapMagicOp["免疫减速状态"]						= new CImmuneCripplingDecreaseMOP;
	ms_mapMagicOp["免疫负面状态"]						= new CImmuneDebuffDecreaseMOP;
	ms_mapMagicOp["免疫伤害状态"]						= new CImmuneDOTDecreaseMOP;
	ms_mapMagicOp["免疫特殊状态"]						= new CImmuneSpecialDecreaseMOP;
	ms_mapMagicOp["免疫任务类型伤害"]					= new CImmuneTaskTypeDamageMOP;
	ms_mapMagicOp["免疫治疗"]							= new CImmuneCureMOP;
	ms_mapMagicOp["免疫位移魔法"]						= new CImmuneMoveMagicMOP;
	ms_mapMagicOp["免疫无类型物理伤害"]					= new CImmuneNonTypePhysicsDamageMOP;
	ms_mapMagicOp["免疫无类型伤害"]						= new CImmuneNonTypeDamageMOP;
	ms_mapMagicOp["免疫无类型治疗"]						= new CImmuneNonTypeCureMOP;
	ms_mapMagicOp["免疫打断"]							= new CImmuneInterruptCastingProcessMOP;
	ms_mapMagicOp["关闭伤害显示"]						= new CClosePrintInfoMOP;
	ms_mapMagicOp["打断施法"]							= new CInterruptCastingProcessMagicMOP;
	ms_mapMagicOp["强制设置DOT伤害"]					= new CForceSetDOTDamageMOP;
	ms_mapMagicOp["绑定目标对象到自身"]					= new CBindTargetObjToSelfMOP;

	ms_mapMagicOp["生命值小于"]				= new CHealthPointLesserMCOND;
	ms_mapMagicOp["生命值大于等于"]			= new CHealthPointGreaterOrEqualMCOND;
	ms_mapMagicOp["魔法值小于"]				= new CManaPointLesserMCOND;
	ms_mapMagicOp["魔法值大于等于"]			= new CManaPointGreaterOrEqualMCOND;
	ms_mapMagicOp["魔法值最大值大于等于"]	= new CMaxManaPointGreaterOrEqualMCOND;
	ms_mapMagicOp["能量值大于等于"]			= new CEnergyPointGreaterOrEqualMCOND;
	ms_mapMagicOp["怒气值大于等于"]			= new CRagePointGreaterOrEqualMCOND;
	ms_mapMagicOp["连击点大于等于"]			= new CComboPointGreaterOrEqualMCOND;

	ms_mapMagicOp["护甲值小于"]				= new CDefenceLesserMCOND;
	ms_mapMagicOp["护甲值大于等于"]			= new CDefenceGreaterOrEqualMCOND;
	ms_mapMagicOp["自然抗值小于"]			= new CNatureResistanceValueLesserMCOND;
	ms_mapMagicOp["自然抗值大于等于"]		= new CNatureResistanceValueGreaterOrEqualMCOND;
	ms_mapMagicOp["破坏抗值小于"]			= new CDestructionResistanceValueLesserMCOND;
	ms_mapMagicOp["破坏抗值大于等于"]		= new CDestructionResistanceValueGreaterOrEqualMCOND;
	ms_mapMagicOp["暗黑抗值小于"]			= new CEvilResistanceValueLesserMCOND;
	ms_mapMagicOp["暗黑抗值大于等于"]		= new CEvilResistanceValueGreaterOrEqualMCOND;

	ms_mapMagicOp["概率小于"]						= new CProbabilityLesserCond;
	ms_mapMagicOp["施法者与目标距离小于"]			= new CDistOfAttackerAndTargetLesserCond;
	ms_mapMagicOp["施法者与目标距离大于等于"]		= new CDistOfAttackerAndTargetGreaterOrEqualCond;
	ms_mapMagicOp["魔法与目标距离小于"]				= new CDistOfMagicAndTargetLesserCond;
	ms_mapMagicOp["魔法与目标距离大于等于"]			= new CDistOfMagicAndTargetGreaterOrEqualCond;

	ms_mapMagicOp["处于魔法状态"]			= new CTestMagicStateMCOND;
	ms_mapMagicOp["非[处于魔法状态且自身不存在天赋]"]			= new CNotTestMagicStateAndSelfExistTalentMCOND;
	ms_mapMagicOp["不处于魔法状态"]			= new CTestNotInMagicStateMCOND;
	ms_mapMagicOp["不处于不可重复魔法状态"]	= new CTestNotInRepeatedMagicStateMCOND;
	ms_mapMagicOp["不处于伤害变更状态"]		= new CTestNotInDamageChangeStateMCOND;
	ms_mapMagicOp["处于自身的魔法状态"]		= new CTestSelfMagicStateMCOND;
	ms_mapMagicOp["处于触发器状态"]			= new CTestTriggerStateMCOND;
	ms_mapMagicOp["处于自身的触发器状态"]	= new CTestSelfTriggerStateMCOND;
	ms_mapMagicOp["处于伤害变更状态"]		= new CTestDamageChangeStateMCOND;
	ms_mapMagicOp["处于自身的伤害变更状态"] = new CTestSelfDamageChangeStateMCOND;
	ms_mapMagicOp["处于特殊状态"]			= new CTestSpecialStateMCOND;
	ms_mapMagicOp["不处于特殊状态"]			= new CTestNotInSpecialStateMCOND;
	ms_mapMagicOp["处于自身的特殊状态"]		= new CTestSelfSpecialStateMCOND;
	ms_mapMagicOp["不处于负面状态"]			= new CTestNotInStateByDecreaseTypeMCOND;
	ms_mapMagicOp["非[处于负面状态且自身不存在天赋]"]			= new CNotTestInStateByDecreaseTypeAndSelfExistTalentMCOND;
	ms_mapMagicOp["魔法状态叠加层数大于等于"] =	new CTestMagicStateCascadeGreaterOrEqualMCOND;
	ms_mapMagicOp["处于姿态技能"]			= new CTestAureMagicMCOND;
	ms_mapMagicOp["装备了盾牌"]				= new CTestShieldEquipMCOND;
	ms_mapMagicOp["主手装备了"]				= new CTestMainHandEquipMCOND;
	ms_mapMagicOp["副手装备了武器"]			= new CTestAssistHandEquipMCOND;
	ms_mapMagicOp["宠物存在"]				= new CTestIsExistPetMCOND;
	ms_mapMagicOp["不存在宠物"]				= new CTestIsNotExistPetMCOND;
	ms_mapMagicOp["图腾存在"]				= new CTestIsExistTotemMCOND;
	ms_mapMagicOp["坐骑存在"]				= new CTestIsExistHorseMCOND;
	ms_mapMagicOp["坐骑不存在"]				= new CTestIsNotExistHorseMCOND;
	ms_mapMagicOp["目标是否可被控制"]		= new CTestTargetCanBeControlledMCOND;
	ms_mapMagicOp["目标是否可被猎食"]		= new CTestTargetCanBeRavinMCOND;
	ms_mapMagicOp["目标是否为玩家"]			= new CTestTargetIsPlayerMCOND;
	ms_mapMagicOp["目标为友方"]				= new CTestTargetIsFriendMCOND;
	ms_mapMagicOp["目标为友方幻兽"]			= new CTestTargetIsFriendsServantMCOND;
	ms_mapMagicOp["目标为敌方"]				= new CTestTargetIsEnemyMCOND;
	ms_mapMagicOp["目标职业"]				= new CTestTargetClassMCOND;
	ms_mapMagicOp["目标是否为NPC"]			= new CTestTargetIsNPCMCOND;
	ms_mapMagicOp["目标是否为召唤兽"]		= new CTestTargetIsSummonMCOND;
	ms_mapMagicOp["目标等级小于等于自身等级"]= new CTestTargetLevelLesserOrEqualMCOND;
	ms_mapMagicOp["目标是否为自身"]			= new CTestTargetIsSelfCOND;
	ms_mapMagicOp["不处于战斗状态"]			= new CTestNotInBattleMCOND;
	ms_mapMagicOp["不处于玩法状态"]			= new CTestNotOnMissionMCOND;
	ms_mapMagicOp["非幻兽坐骑状态"]			= new CTestNotBattleHorseStateMCOND;
	ms_mapMagicOp["武器攻击类型是"]			= new CTestWeaponTypeMCOND;
	ms_mapMagicOp["骑乘状态检查"]			= new CTestRidingMCOND;
	ms_mapMagicOp["骑乘姿态检查"]			= new CTestRidingAureMagicMCOND;
	ms_mapMagicOp["三角路径上是否有障碍"]	= new CTestTrianglePathMCOND;
	ms_mapMagicOp["是否为可用尸体"]			= new CTestIsAvailableDeadBobyMCOND;
	ms_mapMagicOp["目标等级是否处于"]		= new CTestTargetLevelIsInScopeMCOND;
	ms_mapMagicOp["目标与自身距离小于"]		= new CTestTargetDistIsLesserThanMCOND;
	ms_mapMagicOp["目标是否归属自身"]		= new CTestTargetIsVestInSelfMCOND;
	ms_mapMagicOp["目标是否是团队成员"]		= new CTestTargetIsRaidmatesMCOND;
	ms_mapMagicOp["目标处于某系流程"]		= new CTestTargetInCastingProcess;
	ms_mapMagicOp["目标是否处于移动"]		= new CTestTargetIsMoving;
	ms_mapMagicOp["目标是否处于位移魔法"]	= new CTestTargetIsMoveMagic;
	ms_mapMagicOp["目标是否不处于位移魔法"]	= new CTestTargetNotIsMoveMagic;
	ms_mapMagicOp["目标阵营是"]				= new CTestTargetIsCamp;
	ms_mapMagicOp["爆魂蓄力等级大于等于"]	= new CTestBurstSoulTimesGreaterOrEqualMCOND;
	ms_mapMagicOp["爆魂连击数等于"]			= new CTestContinuousBurstSoulTimesEqualMCOND;
	ms_mapMagicOp["目标拥有地点魔法"]		= new CTestTargetHavePositionMagic;
	ms_mapMagicOp["目标经验归属自身并处于触发器状态"]	= new CTestExpIsVestInSelfAndTriggerStateMCOND;
	ms_mapMagicOp["目标学会技能"]			= new CTestTargetLearnSkillMCOND;
	ms_mapMagicOp["目标等级小于等于经验拥有者等级"] = new CTestTargetLevelLesserOrEqualToExpOwnerMCOND;
	ms_mapMagicOp["目标技能是否正在冷却"]	= new CTestTargetSkillIsCoolDownMCOND;	
	ms_mapMagicOp["目标不处于场景"]			= new CTestTargetInScene;
	ms_mapMagicOp["目标降仇恨"]				= new CTestDebaseEnmityMOP;
}
