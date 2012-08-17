#include "stdafx.h"
#include "CSplitBaseItemDescript.h"
#include "CSplitInfo.h"
#include "CXmlConfig.inl"
#include "CExpLog.h"

CBatchBaseItemDescript::CBatchBaseItemDescript()
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	//m_strDestSave = "../../cfg/data/src/item/designerItem/designer_ItemType_Common" + strLuaCfgExtend;
	//m_strItemPath = "../../cfg/data/src/item/designerItem/";
	//m_vecFile.push_back("designer_EmbryoItem_Common");
	//m_vecFile.push_back("designer_SkillItem_Common");
	//m_vecFile.push_back("designer_BasicItem_Common");
	//m_vecFile.push_back("designer_QualityMaterialItem_Common");
	//m_vecFile.push_back("designer_BoxItem_Common");
	//m_vecFile.push_back("designer_FlowersItem_Common");
	//m_vecFile.push_back("designer_TongProdItem_Common");
	//m_vecFile.push_back("designer_EquipSmeltSoulScroll_Common");
	//m_vecFile.push_back("designer_OreAreaMap_Common");
	//m_vecFile.push_back("designer_PlayerBag_Common");
	//m_vecFile.push_back("designer_CreateNpcItem_Common");
	//m_vecFile.push_back("designer_SoulPearl_Common");
	//m_vecFile.push_back("designer_HoleMaterial_Common");
	ReadXml();
	for (int i=0;i<(int)m_vecFile.size();++i)
	{
		string& str = m_vecFile[i];
		str = m_strItemPath + str + strLuaCfgExtend;
	}
}

bool CBatchBaseItemDescript::ReadXml()
{
	ifstream ifsGmConflict("SplitItemDescInfo.xml");
	if (!ifsGmConflict)
	{
		return false;
	}

	CXmlConfig* pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig("root", ifsGmConflict);
	ifsGmConflict.close();

	TiXmlNode* pXmlNodeWorkPath = pXmlCfg->Get<TiXmlElement*>("Work_Path");
	if (!pXmlNodeWorkPath)
	{
		return false;
	}
	m_strDestSave = pXmlNodeWorkPath->FirstChild("DestSave")->ToElement()->GetText();
	m_strItemPath = pXmlNodeWorkPath->FirstChild("ItemPath")->ToElement()->GetText();

	TiXmlNode* pXmlNodeFileTeam = pXmlCfg->Get<TiXmlElement*>("File_Team");
	if (!pXmlNodeFileTeam)
	{
		return false;
	}

	for (TiXmlNode* pXmlNodeFileSection = pXmlNodeFileTeam->FirstChild("FileName"); pXmlNodeFileSection; pXmlNodeFileSection = pXmlNodeFileSection->NextSibling("FileName"))
	{
		TiXmlElement* pXmlElementFileName = pXmlNodeFileSection->ToElement();
		const char* szFileName = pXmlElementFileName->Attribute("Name");
		m_vecFile.push_back(szFileName);
	}

	return true;
}

void CBatchBaseItemDescript::BatchItemType()
{
	if(!CSplitInfo::GetInst()->GetBeSplitBaseItemDesc())
		return;
	for (int i=0;i<(int)m_vecFile.size();++i)
	{
		SplitBaseItemDescripType(m_vecFile[i]);
	}
	m_destFile.SetName(m_strDestSave);
	m_destFile.SaveFile();
}

void CBatchBaseItemDescript::SplitBaseItemDescripType(string strFileName)
{
	COriFileOperate oriFile(strFileName);
	oriFile.CleanBlankCol();
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	int descPos = GetColPosByName("Description",vecColName);
	if (descPos ==-1)
		return;
	string strNewType = "DescType";
	oriFile.AddData_toTab(strNewType,0,oriFile.GetColCountByRow(0));
	vector<string> vecNewType;

	for (int i=1;i<(int)oriFile.GetRowCount();++i)
	{
		string strDesc = oriFile.GetDataByRowCol(i,descPos);
		string strType = PickItemDisType(strDesc);
		if (strType != "" && strType != "？" && strType.find("。") == -1)
		{
			int searchPos = strDesc.find(strType);
			strDesc.replace(searchPos,strType.length(),"%s");

			oriFile.AddData_toTab(strDesc,i,descPos);
			oriFile.AddData_toTab(strType,i,oriFile.GetColCountByRow(i));

			if (find(vecNewType.begin(),vecNewType.end(),strType) == vecNewType.end())
			{
				vecNewType.push_back(strType);
			}
		}
		else
		{
			string strTemp;
			oriFile.AddData_toTab(strTemp,i,oriFile.GetColCountByRow(i));
		}
	}

	if (!m_destFile.GetRowCount())
	{
		string strIndex = "Name";
		string strDis = "DisplayName";
		m_destFile.AddData_toTab(strIndex,0,0);
		m_destFile.AddData_toTab(strDis,0,1);
	}

	for (int i=0;i<(int)vecNewType.size();++i)
	{
		int nowRow = m_destFile.GetRowCount();
		m_destFile.AddData_toTab(vecNewType[i],nowRow,0);
		m_destFile.AddData_toTab(vecNewType[i],nowRow,1);
	}
	oriFile.SaveFile();
}

string CBatchBaseItemDescript::PickItemDisType(string& str)
{
	if (str.find("%s") != -1)
	{
		int error = 1;
	}
	int pos = -1;
	pos = str.find("#r");
	if (pos != -1 && pos != 1)
	{
		int beginPos = -1;
		int endPos = pos;
		while (--endPos >= 0 && uint8(str[endPos])>127);
		string strType = str.substr(endPos+1,pos-endPos-1);
		return strType;
	}
	return "";
}

int CBatchBaseItemDescript::GetColPosByName(string strCol,vector<string>& vecRef)
{
	for (int i=0;i<(int)vecRef.size();++i)
	{
		if (strCol == vecRef[i])
		{
			return i;
		}
	}
	string strMsg = strCol + "没有找到这一列";
	if(GetLogToFile())
	{
		PrintLog(strMsg);
	}
	else
	{
		MessageBox(0,strMsg.c_str(),"错误",NULL);
	}
	return -1;
}
