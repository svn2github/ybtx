#include "stdafx.h"
#include "CCfgSkillRuleCheck.h"
#include "CCfgColChecker.inl"
#include "CTxtTableFile.h"
#include "CCfgSkillCheck.h"
#include "CCfgProcessCheck.h"
#include "CCfgAllStateCheck.h"

CCfgSkillRuleCheck::MapSkillRuleName CCfgSkillRuleCheck::ms_mapSkillRuleName;
TypeName2SRTarget CCfgSkillRule::ms_mapSRTargetType;

bool CCfgSkillRuleCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;
	
	bool bSucc = true;
	CTxtTableFile TableFile;
	SetTabFile(TableFile, "SkillRuleBaseData_Server");
	if (!TableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << " 配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}
	bool bNewSec = true;
	for(int32 i = 1; i < TableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName;
		ReadItem(strName, "名字",	CANEMPTY);
		if( strName.empty() )
		{
			bNewSec = true;
			continue;
		}

		string strSkillName, strTarget, strSkillArg;
		ReadItem(strSkillName, "技能名称",	CANEMPTY);
		ReadItem(strTarget,"施法目标",CANEMPTY);
		if(strSkillName == "多选一" || strSkillName == "周围地点释放技能" || strSkillName == "呼救" 
			|| strSkillName == "返回呼救" || strSkillName == "场景坐标释放" || strSkillName == "无特效呼救"
			|| strSkillName == "呼救警戒" || strSkillName == "状态条件释放")
		{
			ReadItem(strSkillArg,"技能参数",MUSTFILL);
		}

		string	strBeginCond;
		ReadItem(strBeginCond,"开始条件",CANEMPTY);
		string	strContinueCond;
		ReadItem(strContinueCond,"运行方式",CANEMPTY);
		int32 iSkillPage = 0;
		ReadItem(iSkillPage,"阶段");
		if (iSkillPage != -1 && iSkillPage < 0)
		{
			stringstream ExpStr;
			ExpStr << "第" << i << "行的" << "阶段" << iSkillPage << "不合法，必须大于0或为-1";
			GenExpInfo(ExpStr.str());
		}
		if (!CreateRuleCondData(strName,strBeginCond,strContinueCond,strSkillName,iSkillPage))
		{
			bSucc = false;
		}
		
		string	strDistance;
		ReadItem(strDistance,"技能距离",CANEMPTY);
		if (!CreateRuleDistance(strName, strSkillName, strDistance))
		{
			bSucc = false;
		}

		trimend(strName);
		static CCfgSkillRuleCheck* pCfgSkillRuleCheck;
		if (bNewSec)
		{
			bNewSec = false;
			MapSkillRuleName::iterator iter = ms_mapSkillRuleName.find(strName);
			if (iter != ms_mapSkillRuleName.end())
			{
				stringstream ExpStr;
				ExpStr << "第" << i << "行的" << "全局技能参数" << strName << "重复";
				GenExpInfo(ExpStr.str());
			}
			else
			{
				pCfgSkillRuleCheck = new CCfgSkillRuleCheck();
				ms_mapSkillRuleName.insert(make_pair(strName, pCfgSkillRuleCheck));
			}
		}

		CCfgSkillRule* pCfgSkillRule = CreateCfgSkillRule(strName, strSkillName, strTarget, strSkillArg);
		if (!pCfgSkillRule)
		{
			bSucc = false;
		}
		else
		{
			pCfgSkillRuleCheck->m_vecChild.push_back(pCfgSkillRule);
		}
	}
	if (!CheckRuleSkill())
	{
		GenExpInfo("【SkillRuleBaseData_Server】配置表填写错误");
	}

	return bSucc;
}

void CCfgSkillRuleCheck::EndCheck()
{
	ClearMap(ms_mapSkillRuleName);
}

CCfgSkillRuleCheck::~CCfgSkillRuleCheck()
{
	ClearVector(m_vecChild);
}

bool CCfgSkillRuleCheck::CheckRuleSkill()
{
	bool bSuccess = true;
	MapSkillRuleName::const_iterator itr = ms_mapSkillRuleName.begin();
	for (; itr != ms_mapSkillRuleName.end(); ++itr)
	{
		const string sRuleName = (*itr).first;
		VecChild::const_iterator itr_lst = ((*itr).second)->m_vecChild.begin();
		for (; itr_lst != ((*itr).second)->m_vecChild.end(); ++itr_lst)
		{
			ESRTargetType eTargetType = (*itr_lst)->m_eSRTargetType;
			string strTempSkillName = (*itr_lst)->m_strSkillName;
			if (strTempSkillName == "切换阶段" || strTempSkillName == "随机改变目标" || strTempSkillName == "呼救" 
				|| strTempSkillName == "返回呼救" || strTempSkillName == "无特效呼救" || strTempSkillName == "呼救警戒"
				|| strTempSkillName == "随机改变非Npc目标")
			{
				continue;
			}
			else if (strTempSkillName == "多选一" || strTempSkillName == "周围地点释放技能" )
			{
				const CCfgSkillRule::SkillNameLst lstSkillName = (*itr_lst)->m_lstMultiArgs;
				CCfgSkillRule::SkillNameLst::const_iterator itr_skill = lstSkillName.begin();
				for (; itr_skill != lstSkillName.end(); ++itr_skill)
				{
					string sSkillName = *itr_skill;
					CCfgSkillCheck::MapSkill::iterator itr = CCfgSkillCheck::ms_mapSkill.find(sSkillName);
					if (itr == CCfgSkillCheck::ms_mapSkill.end())
					{
						cout<<"Npc技能规则表中规则【"<<sRuleName<<"】释放的技能【"<<sSkillName<<"】在【NpcSkill.ndf】中不存在！"<<endl;
						bSuccess = false;
					}
					if (!CheckRuleSkillTargetType(sRuleName, sSkillName, eTargetType))
						bSuccess = false;
				}
			}
			else if (strTempSkillName == "场景坐标释放" || strTempSkillName == "状态条件释放")
			{
				const CCfgSkillRule::SkillNameLst lstSkillName = (*itr_lst)->m_lstMultiArgs;
				CCfgSkillRule::SkillNameLst::const_iterator itr_skill = lstSkillName.end();
				string skillName = *(--itr_skill);
				CCfgSkillCheck::MapSkill::iterator itr = CCfgSkillCheck::ms_mapSkill.find(skillName);
				if (itr == CCfgSkillCheck::ms_mapSkill.end())
				{
					cout<<"Npc技能规则表中规则【"<<sRuleName<<"】释放的技能【"<<strTempSkillName<<"】的参数【" << skillName <<  "】在【NpcSkill.ndf】中不存在！"<<endl;
					bSuccess = false;
				}

				if (strTempSkillName == "状态条件释放" )
				{
					string strStateName = *(--itr_skill);
					if(!CCfgMagicStateCheck::CheckExist(strStateName))
					{
						cout << "Npc技能规则表中规则【"<<sRuleName<<"】参数魔法状态名【" << strTempSkillName << "】的参数【" << strStateName << "】在魔法状态表中不存在" <<endl;
						bSuccess = false;
					}
				}
			}
			else
			{
				CCfgSkillCheck::MapSkill::iterator itr = CCfgSkillCheck::ms_mapSkill.find(strTempSkillName);
				if (itr == CCfgSkillCheck::ms_mapSkill.end())
				{
					cout<<"Npc技能规则表中规则【"<<sRuleName<<"】释放的技能【"<<strTempSkillName<<"】在【NpcSkill.ndf】中不存在！"<<endl;
					bSuccess = false;
				}
				if (!CheckRuleSkillTargetType(sRuleName, strTempSkillName, eTargetType))
					bSuccess = false;
			}
		}
	}
	
	return bSuccess;
}

bool CCfgSkillRuleCheck::CheckRuleSkillTargetType(const string sRuleName, string sSkillName, ESRTargetType eTargetType)
{
	bool bSuccess = true;
	if (CCfgSkillCheck::IsNPCSkillNameValid(sSkillName.c_str()))
	{
		CCfgSkillCheck::MapSkill::iterator itr = CCfgSkillCheck::ms_mapSkill.find(sSkillName);
		if (itr != CCfgSkillCheck::ms_mapSkill.end())
		{
			CCfgMagicEffCheck* pNpcMagicEff = (*itr).second->GetMagicEff();
			if (pNpcMagicEff && !pNpcMagicEff->CheckNpcSkillFilterObj2MagicEff(sRuleName, sSkillName, eTargetType))
				bSuccess = false;

			CCfgProcessCheck::MapProcess::iterator itr = CCfgProcessCheck::ms_mapProcess.find(sSkillName);
			if (itr != CCfgProcessCheck::ms_mapProcess.end())
			{
				CCfgMagicEffCheck* pCastSkillMagicEff = (*itr).second->GetMagicEff();
				if (pCastSkillMagicEff && !pCastSkillMagicEff->CheckNpcSkillFilterObj2MagicEff(sRuleName, sSkillName, eTargetType))
					bSuccess = false;
			}
		}
	}
	return bSuccess;
}

uint32 CCfgSkillRuleCheck::GetArgNum(ESRTargetType eSRTargetType)
{
	if (eSRTargetType > eSRTT_NoArg && eSRTargetType < eSRRT_OneArg)
	{
		return 0;
	}
	else if ( eSRTargetType > eSRRT_OneArg && eSRTargetType < eSRTT_TwoArg)
	{
		return 1;
	}
	else
		return 2;
}

bool CCfgSkillRuleCheck::CreateRuleCondData(string &strName,string &strBegin,string &strContiune,string &strSkillName, int32 iSkillPage)
{
	bool bRet1 = ParseRuleCond(strName,strBegin,eRCK_Begin,strSkillName,iSkillPage);
	bool bRet2 = ParseRuleCond(strName,strContiune,eRCK_Continue,strSkillName,iSkillPage);
	return bRet1 && bRet2;
}

bool CCfgSkillRuleCheck::ParseRuleCond(string &strName,string& strCond,ERuleCondKind eRuleKind,string &strSkillName, int32 iSkillPage)
{
	if(strCond.empty())
		return true;
	bool bSucc = true;
	erase(strCond, " ");
	erase(strCond,"\"");
	replace(strCond, "，", ",");
	replace(strCond, "（", "(");
	replace(strCond, "）", ")");
	int32 startPos = -1;
	string strNameTemp;
	string strArgTemp;
	string strTemp;
	while (true)
	{
		if(strCond.empty())
			return true;
		startPos = strCond.find(',');
		if(-1 == startPos)
		{
			int32 lbracketPos,rbracketPos;
			lbracketPos = strCond.find('(');
			rbracketPos = strCond.find(')');
			if (-1 == lbracketPos || -1 == rbracketPos)
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">里触发条件左右两个括号缺少或不匹配"<< endl;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			strNameTemp = strCond.substr(0,lbracketPos);
			strArgTemp	= strCond.substr(lbracketPos+1,rbracketPos-lbracketPos-1);
			if(!CCfgSkillRule::ms_mapSRTargetType.mapCondType.count(strNameTemp))
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的规则条件<"<<strNameTemp<<">不存在!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			return ParseRuleCondArg(strName,CCfgSkillRule::ms_mapSRTargetType.mapCondType[strNameTemp],strArgTemp,strSkillName,iSkillPage);
		}
		strTemp = strCond.substr(0,startPos);
		if(strTemp.empty())
		{
			if(startPos < int32(strCond.length())-1)
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的规则条件<"<<strNameTemp<<">格式不对，出现连续 ,, 的情况"<< endl;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			else
				return true;
		}
		else
		{
			int32 lbracketPos,rbracketPos;
			lbracketPos = strTemp.find('(');
			rbracketPos = strTemp.find(')');
			if (-1 == lbracketPos || -1 == rbracketPos)
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的规则条件<"<<strNameTemp<<">里左右两个括号缺少或不匹配"<< endl;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			strNameTemp = strTemp.substr(0,lbracketPos);
			strArgTemp	= strTemp.substr(lbracketPos+1,rbracketPos-lbracketPos-1);
			if(!CCfgSkillRule::ms_mapSRTargetType.mapCondType.count(strNameTemp))
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的规则条件<"<<strNameTemp<<">条件不存在!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			bSucc = ParseRuleCondArg(strName,CCfgSkillRule::ms_mapSRTargetType.mapCondType[strNameTemp],strArgTemp,strSkillName,iSkillPage);
		}
		strCond = strCond.substr(startPos+1,strCond.length());
	}
	return bSucc;
}

bool CCfgSkillRuleCheck::ParseRuleCondArg(string &strName,ERCondType eRCType,string& strArgTemp,string &strSkillName,int32 iSkillPage)
{
	bool bSucc = true;
	if(strArgTemp.empty())
	{
		if(eRCT_EnterBattle != eRCType && eRCT_EnterPhase != eRCType
			&& eRCT_OnDeadCond != eRCType && eRCT_EnemyDead != eRCType)
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">规则条件缺少参数!"<<endl;;
			cout<<strm.str()<<endl;
			bSucc = false;
		}
	}
	else
	{
		if (eRCT_RangeTimingCond == eRCType)
		{
			uint32 uArgPos = strArgTemp.find(',');
			if(-1 == uArgPos)
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">范围定时规则条件参数应该为两个!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			string strOne = strArgTemp.substr(0,uArgPos);
			string strTwo = strArgTemp.substr(uArgPos+1,strArgTemp.length());
			if (strOne.empty())
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">范围定时规则条件第一个参数为空!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			if (strTwo.empty())
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">范围定时规则条件第二个参数为空!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
		}
		else if(eRCT_WanderTimeGreater == eRCType)
		{
			if (!CCfgSkillCheck::IsNPCSkillNameValid(strSkillName.c_str()))
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">不存在!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
			if (CCfgSkillCheck::IsNPCSkillDoSkillRule(strSkillName.c_str()))
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">的技能<"<<strSkillName<<">是走技能规则的,不能用于待机规则!"<<endl;;
				cout<<strm.str()<<endl;
				bSucc = false;
			}
		}
	}
	
	if (iSkillPage == -1)
	{
		if (eRCType != eRCT_EnterWander && eRCType != eRCT_WanderTimeGreater && eRCType != eRCT_OnDeadCond)
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<<">的阶段<"<<iSkillPage<<">必须对应待机或死亡规则"<<endl;;
			cout<<strm.str()<<endl;
			bSucc = false;
		}
	}
	if (eRCType == eRCT_EnterWander || eRCType == eRCT_WanderTimeGreater || eRCType == eRCT_OnDeadCond)
	{
		if (iSkillPage != -1)
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<< ">待机或死亡规则对应的阶段必须为-1, 但这里为<" << iSkillPage << ">" <<endl;;
			cout<<strm.str()<<endl;
			bSucc = false;
		}
	}
	
	return bSucc;
}

bool CCfgSkillRuleCheck::CreateRuleDistance(string& strName, string& strSkillName, string& strDistace)
{
	bool bSuccess = true;
	if (strDistace.empty())
	{
		ostringstream strm;
		strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">技能距离不能为空!"<< endl;
		cout<<strm.str()<<endl;
		bSuccess = false;
	}
	erase(strDistace, " ");
	erase(strDistace,"\"");
	replace(strDistace, "，", ",");
	replace(strDistace, "（", "(");
	replace(strDistace, "）", ")");
	int32 lbracketPos,rbracketPos;
	int32 commaPos;
	lbracketPos = strDistace.find('(');
	rbracketPos = strDistace.find(')');
	commaPos = strDistace.find(',');
	if (-1 == lbracketPos || -1 == rbracketPos || -1 == commaPos)
	{
		ostringstream strm;
		strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">技能距离格式添写不对!"<< endl;
		cout<<strm.str()<<endl;
		bSuccess = false;
	}
	string strOne = strDistace.substr(lbracketPos+1,commaPos-lbracketPos-1);
	string strTwo = strDistace.substr(commaPos+1,rbracketPos-commaPos-1);
	if (strOne.empty() || strTwo.empty())
	{
		ostringstream strm;
		strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">技能距离缺少参数!"<< endl;
		cout<<strm.str()<<endl;
		bSuccess = false;
	}
	return bSuccess;
}

CCfgSkillRule* CCfgSkillRuleCheck::CreateCfgSkillRule(string strName, string strSkillName, string strTarget, string strSkillArg)
{
	bool bSuccess = true;
	if(strTarget.empty())
	{
		ostringstream strm;
		strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的释放目标为空，不允许!"<< endl;
		cout<<strm.str()<<endl;
		bSuccess = false;
	}

	string strSkillTargetName;
	uint32 uTargetArgs;
	erase(strTarget, " ");
	erase(strTarget,"\"");
	replace(strTarget, "，", ",");
	replace(strTarget, "（", "(");
	replace(strTarget, "）", ")");
	int32 lbracketPos, rbracketPos, commaPos;
	lbracketPos = strTarget.find('(');
	commaPos = strTarget.find(',');
	rbracketPos = strTarget.find(')');
	if (-1 == lbracketPos)
	{
		uTargetArgs = 0;
		strSkillTargetName = strTarget;
	}
	else
	{
		if (-1 == rbracketPos)
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">释放目标 左右括号不匹配!"<< endl;
			cout<<strm.str()<<endl;
			bSuccess = false;
		}

		string strRealTargetName = strTarget.substr(0,lbracketPos);
		strSkillTargetName = strRealTargetName;
		if (-1 == commaPos)
		{
			uTargetArgs = 1;
			string strArg = strTarget.substr(lbracketPos+1, rbracketPos-lbracketPos-1);
			if(strArg.empty())
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的目标类型参数为空，请查实!"<< endl;
				cout<<strm.str()<<endl;
				bSuccess = false;
			}
		}
		else
		{
			uTargetArgs = 2;
			string strArg1 = strTarget.substr(lbracketPos+1, commaPos-lbracketPos-1);
			string strArg2 = strTarget.substr(commaPos+1, rbracketPos-commaPos-1);
			if(strArg1.empty() || strArg2.empty())
			{
				ostringstream strm;
				strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的目标类型参数为空，请查实!"<< endl;
				cout<<strm.str()<<endl;
				bSuccess = false;
			}
		}
	}
	ESRTargetType eSRTargetType = eSRTT_None;
	if (CCfgSkillRule::ms_mapSRTargetType.mapTargetType.find(strSkillTargetName) == CCfgSkillRule::ms_mapSRTargetType.mapTargetType.end())
	{
		ostringstream strm;
		strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">不存在的目标类型<"<<strSkillTargetName<<"> 请查实！"<< endl;
		cout<<strm.str()<<endl;
		bSuccess = false;
	}
	else
	{
		eSRTargetType = CCfgSkillRule::ms_mapSRTargetType.mapTargetType[strSkillTargetName];
		uint32 uRealArgs = GetArgNum(eSRTargetType);
		if (uRealArgs != uTargetArgs)
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的目标类型<"<<strSkillTargetName<<">参数错误！实际需要参数<"
				<<uRealArgs<<"> 而你填写了<"<<uTargetArgs<<">个参数！"<<endl;
			cout<<strm.str()<<endl;
			bSuccess = false;
		}
	}
	
	if (bSuccess)
	{
		return new CCfgSkillRule(strName, strSkillName, eSRTargetType, strSkillArg);
	}
	return NULL;
}

bool CCfgSkillRuleCheck::CheckExist(string& strSkillRuleName)
{
	MapSkillRuleName::iterator iter = ms_mapSkillRuleName.find(strSkillRuleName);
	if (iter != ms_mapSkillRuleName.end())
	{
		return true;
	}
	return false;
}

CCfgSkillRule::CCfgSkillRule(string strName, string strSkillName, ESRTargetType eSRTargetType, string strSkillArg)
:m_strSkillName(strSkillName)
,m_eSRTargetType(eSRTargetType)
{
	if(strSkillName == "多选一" || strSkillName == "周围地点释放技能" || strSkillName == "呼救" 
		|| strSkillName == "返回呼救" || strSkillName == "场景坐标释放" 
		||strSkillName == "无特效呼救" || strSkillName == "呼救警戒" || strSkillName == "状态条件释放")
	{
		if(strSkillArg.empty())
		{
			ostringstream strm;
			strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">为多选一技能，缺少技能参数!"<<endl;;
			cout<<strm.str()<<endl;
		}
		erase(strSkillArg, " ");
		erase(strSkillArg,"\"");
		replace(strSkillArg, "，", ",");
		replace(strSkillArg, "（", "(");
		replace(strSkillArg, "）", ")");
		erase(strSkillArg, "(");
		erase(strSkillArg, ")");
		int32 startPos = -1;
		string strTemp;
		while (true)
		{
			if(strSkillArg.empty())
				return;
			startPos = strSkillArg.find(',');
			if (-1 == startPos)
			{
				m_lstMultiArgs.push_back(strSkillArg);
				return;
			}
			strTemp = strSkillArg.substr(0,startPos);
			if(strTemp.empty())
			{
				if(startPos < int32(strSkillArg.length())-1)
				{
					ostringstream strm;
					strm<<"技能规则<"<<strName<<">技能<"<<strSkillName<<">的技能参数格式不对，出现连续 ,, 的情况"<< endl;
					cout<<strm.str()<<endl;
				}
				else
					return;
			}
			m_lstMultiArgs.push_back(strTemp);
			strSkillArg = strSkillArg.substr(startPos+1,strSkillArg.length());
		}
	}
}

