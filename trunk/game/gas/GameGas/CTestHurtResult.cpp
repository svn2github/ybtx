#include "stdafx.h"
#include "CTestHurtResult.h"
#include "MagicOps_Damage.h"
#include "CScriptAppServer.h"
#include "CTxtTableFile.h"
#include "CodeCvs.h"

namespace sqr
{
	extern const wstring PATH_ALIAS_CFG;
}

void CTestHurtResult::CalPhysicsProbability()
{
	string path  = utf8_to_gbk(CScriptAppServer::Inst()->GetCfgFilePath(""));
	string fileName = path + "/skill/Calculate/CalPhysicsProbability.txt";
	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), fileName.c_str()))
		return;
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		PropertyOfPhysicsCal pro;
		string strExampleName		= TabFile.GetString(i, "实例名");
		pro.m_ePhysicalAttackType	= (EPhysicalAttackType)TabFile.GetInteger(i, "普攻类型", 0);
		pro.m_uAttackerLevel		= TabFile.GetInteger(i, "攻击者等级", 0);
		pro.m_fMissRate				= TabFile.GetFloat(i, "未命中率", 0.0f);
		pro.m_uAccuratenessValue	= TabFile.GetInteger(i, "精准值", 0);
		pro.m_uStrikeValue			= TabFile.GetInteger(i, "暴击值", 0);
		pro.m_fExtraStrikeRate		= TabFile.GetFloat(i, "附加暴击率", 0.0f);

		pro.m_uTargetLevel			= TabFile.GetInteger(i, "目标等级", 0);
		pro.m_uPhysicalDodgeValue	= TabFile.GetInteger(i, "闪躲值", 0);
		pro.m_fExtraPhysicDodgeRate	= TabFile.GetFloat(i, "附加闪躲率", 0.0f);
		pro.m_uParryValue			= TabFile.GetInteger(i, "招架值", 0);
		pro.m_fExtraParryRate		= TabFile.GetFloat(i, "附加招架率", 0.0f);
		pro.m_fBlockRate			= TabFile.GetFloat(i, "格挡率", 0.0f);

		EHurtResult eResult;
		uint32 uMissCnt(0), uPhydodgeCnt(0), uParryCnt(0), uBlockCnt(0), uStrikeCnt(0), uHitCnt(0);

		for (int32 i = 0; i < 1000; ++i)
		{
			eResult = CCalculatePhysicsHurtMop::CalProbability(pro);
			switch (eResult)
			{
			case eHR_Miss:		++uMissCnt;		break;
			case eHR_PhyDodge:	++uPhydodgeCnt; break;
			case eHR_Parry:		++uParryCnt;	break;
			case eHR_Block:		++uBlockCnt;	break;
			case eHR_Strike:	++uStrikeCnt;	break;
			case eHR_Hit:		++uHitCnt;		break;
			default: 
				break;
			}
		}

		//cout << "测试用例 : " << strExampleName << endl;
		//cout << "攻击者等级: " << pro.m_uAttackerLevel << "\t 普攻类型: " << pro.m_ePhysicalAttackType << "\t 未命中率: " << pro.m_fMissRate
		//	 << "\t 精准值: " << pro.m_uAccuratenessValue << "\t 暴击值: " << pro.m_uStrikeValue << "\t 附加暴击率: " << pro.m_fExtraStrikeRate << endl;
		//cout << "被攻击者等级: " << pro.m_uTargetLevel << "\t 闪躲值: " << pro.m_uPhysicalDodgeValue << "\t 附加闪躲率: " << pro.m_fExtraPhysicDodgeRate
		//	<< "\t 招架值: " << pro.m_uParryValue << "\t 附加招架率: " << pro.m_fExtraParryRate << "\t 格挡率: " << pro.m_fBlockRate << endl;
		//printf("物理攻击 1000 次结果为:\n");
		//printf("[未命中]: %4d 次, 约占 %5.2f%%\n", uMissCnt, (float)uMissCnt*100/(float)1000);
		//printf("[闪  躲]: %4d 次, 约占 %5.2f%%\n", uPhydodgeCnt, (float)uPhydodgeCnt*100/(float)1000);
		//printf("[招  架]: %4d 次, 约占 %5.2f%%\n", uParryCnt, (float)uParryCnt*100/(float)1000);
		//printf("[格  挡]: %4d 次, 约占 %5.2f%%\n", uBlockCnt, (float)uBlockCnt*100/(float)1000);
		//printf("[暴  击]: %4d 次, 约占 %5.2f%%\n", uStrikeCnt, (float)uStrikeCnt*100/(float)1000);
		//printf("[命  中]: %4d 次, 约占 %5.2f%%\n", uHitCnt, (float)uHitCnt*100/(float)1000);
	}
}

void CTestHurtResult::CalMagicProbability()
{
	string path  = utf8_to_gbk(CScriptAppServer::Inst()->GetCfgFilePath(""));
	string fileName = path + "/skill/Calculate/CalMagicProbability.txt";
	CTxtTableFile TabFile;
	if (!TabFile.Load(PATH_ALIAS_CFG.c_str(), fileName.c_str()))
		return;
	for( int32 i = 1; i < TabFile.GetHeight(); ++i )
	{
		PropertyOfMagicCal pro;
		string strExampleName		= TabFile.GetString(i, "实例名");

		pro.m_uAttackerLevel		= TabFile.GetInteger(i, "攻击者等级", 0);
		pro.m_uMagicHitValue		= TabFile.GetInteger(i, "法术命中值", 0);
		pro.m_uStrikeValue			= TabFile.GetInteger(i, "暴击值", 0);
		pro.m_fExtraStrikeRate		= TabFile.GetFloat(i, "附加暴击率", 0.0f);

		pro.m_uTargetLevel						= TabFile.GetInteger(i, "目标等级", 0);;
		pro.m_uMagicDodgeValue					= TabFile.GetInteger(i, "法术闪躲值", 0);;
		pro.m_fExtraMagicDodgeRate				= TabFile.GetFloat(i, "附加闪躲率", 0.0f);
		pro.m_uNatureResistanceValue			= TabFile.GetInteger(i, "自然抗性值", 0);;
		pro.m_fExtraNatureResistanceRate		= TabFile.GetFloat(i, "附加自然抗率", 0.0f);
		pro.m_uDestructionResistanceValue		= TabFile.GetInteger(i, "破坏抗性值", 0);;
		pro.m_fExtraDestructionResistanceRate	= TabFile.GetFloat(i, "附加破坏抗率", 0.0f);
		pro.m_uEvilResistanceValue				= TabFile.GetInteger(i, "暗黑抗性值", 0);;
		pro.m_fExtraEvilResistanceRate			= TabFile.GetFloat(i, "附加暗黑抗率", 0.0f);
		pro.m_fExtraMagicResistanceRate			= TabFile.GetFloat(i, "附加法术抗率", 0.0f);
		pro.m_fExtraCompleteResistanceRate		= TabFile.GetFloat(i, "附加完全抗率", 0.0f);

		EHurtResult eResult;
		uint32 uMagDodgeCnt(0), uComResistCnt(0), uResistCnt(0), uStrikeCnt(0), uHitCnt(0);

		for (int32 i = 0; i < 1000; ++i)
		{
			eResult = CCalculateMagicHurtMop::CalProbability(pro);
			switch (eResult)
			{
			case eHR_MagDodge:		++uMagDodgeCnt;		break;
			case eHR_ComResist:		++uComResistCnt;	break;
			case eHR_Resist:		++uResistCnt;		break;
			case eHR_Strike:		++uStrikeCnt;		break;
			case eHR_Hit:			++uHitCnt;			break;
			default: break;
			}
		}

		//cout << "测试用例 : " << strExampleName << endl;
		//cout << "攻击者等级: " << pro.m_uAttackerLevel << "\t 法术命中值: " << pro.m_uMagicHitValue <<  "\t 暴击值: " << pro.m_uStrikeValue << "\t 附加暴击率: " << pro.m_fExtraStrikeRate << endl;
		//cout << "被攻击者等级: " << pro.m_uTargetLevel << "\t 法术闪躲值: " << pro.m_uMagicDodgeValue << "\t 附加法术闪躲率: " << pro.m_fExtraMagicDodgeRate
		//	<< "\t 自然抗性值: " << pro.m_uNatureResistanceValue << "\t 附加自然抗率: " << pro.m_fExtraNatureResistanceRate 
		//	<< "\t 破坏抗性值: " << pro.m_uDestructionResistanceValue << "\t 附加破坏抗率: " << pro.m_fExtraDestructionResistanceRate 
		//	<< "\t 暗黑抗性值: " << pro.m_uEvilResistanceValue << "\t 附加暗黑抗率: " << pro.m_fExtraEvilResistanceRate 
		//	<< "\t 暗黑抗性值: " << pro.m_fExtraMagicResistanceRate << "\t 附加暗黑抗率: " << pro.m_fExtraCompleteResistanceRate << endl;
		//printf("法术攻击 1000 次结果为:\n");
		//printf("[法术闪躲]: %4d 次, 约占 %5.2f%%\n", uMagDodgeCnt, (float)uMagDodgeCnt*100/(float)1000);
		//printf("[完全抵抗]: %4d 次, 约占 %5.2f%%\n", uComResistCnt, (float)uComResistCnt*100/(float)1000);
		//printf("[抵  抗  ]: %4d 次, 约占 %5.2f%%\n", uResistCnt, (float)uResistCnt*100/(float)1000);
		//printf("[暴  击  ]: %4d 次, 约占 %5.2f%%\n", uStrikeCnt, (float)uStrikeCnt*100/(float)1000);
		//printf("[命  中  ]: %4d 次, 约占 %5.2f%%\n", uHitCnt, (float)uHitCnt*100/(float)1000);
	}
}
