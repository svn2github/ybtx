#include "stdafx.h"
#include "CCfgAllStateCheck.h"
#include "CTxtTableFile.h"
#include "CCfgColChecker.inl"
#include "LoadSkillCommon.h"
#include "CCfgMagicEffCheck.h"

CCfgMagicStateCheck::SetRowName CCfgMagicStateCheck::ms_setRowName;
CCfgAllStateCheck::MapState CCfgAllStateCheck::ms_mapState;

bool CCfgMagicStateCheck::Check(const TCHAR* cfgFile)
{
	using namespace CfgChk;

	CTxtTableFile TxtTableFile;
	SetTabFile(TxtTableFile, "魔法状态");
	if (!TxtTableFile.Load(PATH_ALIAS_CFG.c_str(), cfgFile))
	{
		stringstream ExpStr;
		ExpStr << "配置表 加载失败，请查看文件名[" << cfgFile << "]是否正确，或文件是否存在";
		GenExpInfo(ExpStr.str());
	}

	static const uint32 s_uTableFileWide = 16;
	if (TxtTableFile.GetWidth() != s_uTableFileWide)
	{
		stringstream ExpStr;
		ExpStr << "配置表 列数错误 应为: " << s_uTableFileWide << " 列，实为: " << TxtTableFile.GetWidth() << " 列";
		GenExpInfo(ExpStr.str());
	}

	for(int32 i = 1; i < TxtTableFile.GetHeight(); ++i)
	{
		SetLineNo(i);
		string strName,	strCancelEff, strDotEff, strFinalEff, strCascadeType, strCascadeMax;
		bool bPersistent;
		ReadItem(strName,			szMagicState_Name,				CANEMPTY);
		ReadItem(bPersistent,		szState_Persistent,				CANEMPTY,	NO);
		ReadItem(strCancelEff,		szMagicState_CancelableMagicEff,CANEMPTY);
		ReadItem(strDotEff,			szMagicState_DotMagicEff,		CANEMPTY);
		ReadItem(strFinalEff,		szMagicState_FinalMagicEff,		CANEMPTY);
		ReadItem(strCascadeType,	szMagicState_CascadeType,		CANEMPTY);
		ReadItem(strCascadeMax,		szMagicState_CascadeMax,		CANEMPTY);
		trimend(strName);


		//2
		if (!strDotEff.empty())
		{
			size_t comma = strDotEff.find(',');
			SetItemTitle(szMagicState_DotMagicEff);
			if(comma != string::npos)
			{
				string strTemp = strDotEff;
				strDotEff = strDotEff.substr(0, comma);
				CCfgMagicEffCheck::CheckMagicEffExist(strDotEff);

				float fDotInterval = 1.0f;
				SetValue(fDotInterval, strTemp.substr(comma + 1));
				if(fDotInterval <= 0.0f)
				{
					GenExpInfo("违反列约束：间隔效果时间必须为正数\n");
				}
				else if(fDotInterval / 1.0f != float(int32(fDotInterval / 1.0f)) && fDotInterval != 0.5f)
				{
					GenExpInfo("违反列约束：间隔效果时间必须为整数或0.5\n");
				}
			}
			else
			{
				CCfgMagicEffCheck::CheckMagicEffExist(strDotEff);
			}
		}

		//3
		if( (!strCancelEff.empty() && !strDotEff.empty() && strCancelEff == strDotEff) ||
			(!strFinalEff.empty() && !strDotEff.empty() && strFinalEff == strDotEff) ||
			(!strCancelEff.empty() && !strFinalEff.empty() && strCancelEff == strFinalEff) )
		{
			stringstream str;
			str << "\n第" << i << "行违反表约束：同一行的不同魔法效果不能同名";
			GenExpInfo(str.str());
		}

		//4
		string strModel;
		ReadItem(strModel,			szTplState_Model,			CANEMPTY);
		if(!strModel.empty() && "唯一" != strCascadeType)
		{
			SetItemTitle(szMagicState_CascadeType);
			GenExpInfo("在有模型的魔法状态里不能为唯一以外的取值", strCascadeType);
		}

		//5
		stringstream ExpStr;
		CCfgCalc* calcFX= new CCfgCalc;
		CCfgCalc* calcCascadeMax = new CCfgCalc;
		ReadMixedItem(calcFX,		szTplState_FXID,			CANEMPTY,	"");
		calcFX->SetStringSplitter(';');
		ReadItem(calcCascadeMax,	szMagicState_CascadeMax,		GE,			-1);
		CCfgCalc* calcTime = new CCfgCalc;
		ReadItem(calcTime,				szMagicState_Time,				GE,			-1);
		if(calcFX->GetStringCount() > 1)
		{
			if(!calcCascadeMax->IsSingleNumber())
			{
				ExpStr << "分层特效的叠加上限层数[" << calcCascadeMax->GetTestString() << "不能是表达式";
				GenExpInfo(ExpStr.str(), calcFX->GetTestString());
			}
			else if((uint32)calcCascadeMax->GetIntValue() != (uint32)calcFX->GetStringCount())
			{
				ExpStr << "分层特效的字符串数[" << calcFX->GetStringCount() << "]与叠加上限层数[" << calcCascadeMax->GetIntValue() << "]不相等";
				GenExpInfo(ExpStr.str(), calcFX->GetTestString());
			}
		}

		//6
		string strReplaceType;
		ReadItem(strReplaceType,	szMagicState_ReplaceRuler,	CANEMPTY);
		SetItemTitle(szMagicState_ReplaceRuler);
		if(strReplaceType == "否" && calcCascadeMax->IsSingleNumber() && calcCascadeMax->GetIntValue() > 1)
		{
			ExpStr.clear();
			ExpStr << "叠加层数不为1的魔法状态替换规则不能为否\n";
			GenExpInfo(ExpStr.str(), strReplaceType);
		}

		delete calcFX;
		delete calcCascadeMax;
		delete calcTime;

		SetRowName::iterator iter = ms_setRowName.find(strName);
		if (iter != ms_setRowName.end())
		{
			stringstream ExpStr;
			ExpStr << " 配置表: " << g_sTabName << " 第 " << i << " 行的 " << szMagicState_Name << "["<< strName << "]" << "重复";
			GenExpInfo(ExpStr.str());
		}
		else
		{
			ms_setRowName.insert(strName);
			CCfgAllStateCheck* pState = new CCfgAllStateCheck;
			pState->m_strName = strName;
			pState->m_eType = eBST_State;
			pState->m_bPersistent = bPersistent;
			pState->m_strCascadeType = strCascadeType;
			pState->m_strCascadeMax = strCascadeMax;
			if (!strCancelEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strCancelEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strCancelEff);
				pState->m_pCancelEff = (*itr).second;
			}
			if (!strDotEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strDotEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strDotEff);
				pState->m_pDotEff = (*itr).second;
			}
			if (!strFinalEff.empty() && CCfgMagicEffCheck::CheckMagicEffExist(strFinalEff))
			{
				CCfgMagicEffCheck::MapMagicEff::iterator itr = CCfgMagicEffCheck::ms_mapMagicEff.find(strFinalEff);
				pState->m_pFinalEff = (*itr).second;
			}
			CCfgAllStateCheck::ms_mapState.insert(make_pair(strName, pState));
		}

		// 特效名检查
		CCfgCalc* pCascadeMax = NULL;
		ReadItem(pCascadeMax, szMagicState_CascadeMax);
		pCascadeMax->SetTypeExpression();
		int32 nCascadeMax = pCascadeMax->GetIntValue();
		delete pCascadeMax;

		string strFX = TxtTableFile.GetString(i, szTplState_FXID);
		if (strFX != "")
		{
			vector<string> sMultilayerFX = CCfgMagicOp::Split(strFX, ";");
			if (sMultilayerFX.size() == 1)
			{
				vector<string> sFXTable = CCfgMagicOp::Split(strFX, ",");
				if (sFXTable.size() == 2 || sFXTable.size() == 3)
				{
					string strFXName	= sFXTable[1];
					if (strFXName == "")
					{
						stringstream ExpStr;
						ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
						GenExpInfo(ExpStr.str());
					}
				}
				else
				{
					stringstream ExpStr;
					ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
					GenExpInfo(ExpStr.str());
				}
			}
			else
			{
				for (int i = 0; i < nCascadeMax; ++i)
				{
					if (sMultilayerFX[i] != "")
					{
						vector<string> sFXTable = CCfgMagicOp::Split(sMultilayerFX[i], ",");
						if (sFXTable.size() == 2 || sFXTable.size() == 3)
						{
							string strFXName	= sFXTable[1];
							if (strFXName == "")
							{
								stringstream ExpStr;
								ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 逗号后必须有特效名!";
								GenExpInfo(ExpStr.str());
							}
						}
						else
						{
							stringstream ExpStr;
							ExpStr << "第" << i << "行的 特效名 " << strFX << " 错误, 必须为逗号隔开的两项!";
							GenExpInfo(ExpStr.str());
						}
					}
				}
			}
		}
	}
	return true;
}

void CCfgMagicStateCheck::EndCheck()
{
	ms_setRowName.clear();
}

bool CCfgMagicStateCheck::CheckExist(const string& strName)
{
	SetRowName::iterator iter = ms_setRowName.find(strName);
	if (iter == ms_setRowName.end())
	{
		stringstream ExpStr;
		ExpStr << " 违反魔法操作约束: 魔法状态" << " [" << strName << "] 不存在 ";
		CfgChk::GenExpInfo(ExpStr.str());
		return false;
	}
	return true;
}
