#include "stdafx.h"
#include "CMakeReferRelation.h"
#include "common.h"
#include "CSplitInfo.h"
#include "CodeCvs.h"
#include "COriFileOperate.h"
#include "ConsoleInfo.h"
#include <sstream>
#include "CXmlConfig.inl"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

CMakeReferRelation::CMakeReferRelation()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	m_strReferPath = m_pSplitInfo->GetReferPath();
	ReadItemTable();
	FileTreeWalk( gbk_to_utf16(m_pSplitInfo->GetSrcPath()).c_str(),MakeDifList, NULL );
	FileTreeWalk( gbk_to_utf16(m_pSplitInfo->GetCfgTempPath()).c_str(),MakeDifList, NULL );
	BeginCreateQuestReferData();
}

FTW_RESULT CMakeReferRelation::MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring wstr = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	if( _wcsicmp( szFileName + nLen - wstr.length(), wstr.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CMakeReferRelation* pManager = CMakeReferRelation::GetInst();
	pManager->RecordDifPath(strFileName);
	return _FTW_CONTINUNE;
}

void CMakeReferRelation::ReadItemTable()
{
	m_ItemTable.clear();
	//m_ItemTable.insert(make_pair(1,"Lan_BasicItem_Common"));
	//m_ItemTable.insert(make_pair(2,"Lan_PlayerBag_Common"));
	//m_ItemTable.insert(make_pair(3,"Lan_SkillItem_Common"));

	//m_ItemTable.insert(make_pair(5,"Lan_Equip_Weapon_Common"));
	//m_ItemTable.insert(make_pair(6,"Lan_Equip_Armor_Common"));
	//m_ItemTable.insert(make_pair(7,"Lan_Equip_Shield_Common"));
	//m_ItemTable.insert(make_pair(8,"Lan_Equip_Ring_Common"));
	//m_ItemTable.insert(make_pair(9,"Lan_Equip_Jewelry_Common"));
	//m_ItemTable.insert(make_pair(10,"Lan_TongProdItem_Common"));

	//m_ItemTable.insert(make_pair(14,"Lan_Stone_Common"));
	//m_ItemTable.insert(make_pair(15,"Lan_HoleMaterial_Common"));
	//m_ItemTable.insert(make_pair(16,"Lan_QuestItem_Common"));
	//m_ItemTable.insert(make_pair(17,"Lan_Mail_Common"));
	//m_ItemTable.insert(make_pair(18,"Lan_WhiteStone_Common"));
	//m_ItemTable.insert(make_pair(19,"Lan_SoulPearl_Common"));
	//m_ItemTable.insert(make_pair(23,"Lan_TongSmeltItem_Common"));
	//m_ItemTable.insert(make_pair(24,"Lan_BoxItem_Common"));
	//m_ItemTable.insert(make_pair(25,"Lan_TextItem_Common"));
	//m_ItemTable.insert(make_pair(26,"Lan_BattleArrayBooks_Common"));
	//m_ItemTable.insert(make_pair(27,"Lan_EmbryoItem_Common"));
	//m_ItemTable.insert(make_pair(28,"Lan_QualityMaterialItem_Common"));
	//m_ItemTable.insert(make_pair(29,"Lan_CofcTruck_Common"));
	//m_ItemTable.insert(make_pair(30,"Lan_OreAreaMap_Common"));
	//m_ItemTable.insert(make_pair(31,"Lan_EquipIdentifyScroll_Common"));
	//m_ItemTable.insert(make_pair(32,"Lan_FlowersItem_Common"));

	//m_ItemTable.insert(make_pair(34,"Lan_ArmorPieceEnactment_Common"));
	//m_ItemTable.insert(make_pair(36,"Lan_EquipSmeltSoulScroll_Common"));
	//m_ItemTable.insert(make_pair(37,"Lan_MercCardItem_Common"));
	//m_ItemTable.insert(make_pair(38,"Lan_AdvanceStone_Common"));
	//m_ItemTable.insert(make_pair(39,"Lan_TongChallengeItem_Common"));
	//m_ItemTable.insert(make_pair(40,"Lan_CreateNpcItem_Common"));
	//m_ItemTable.insert(make_pair(41,"Lan_PetEgg_Common"));
	//m_ItemTable.insert(make_pair(42,"Lan_PetSkillStone_Common"));
	//m_ItemTable.insert(make_pair(43,"Lan_EquipIntensifyBackItem_Common"));
	//m_ItemTable.insert(make_pair(45,"Lan_PickOreItem_Common"));
	//m_ItemTable.insert(make_pair(46,"Lan_MatchGameItem_Common"));
	//m_ItemTable.insert(make_pair(47,"Lan_TurntableItem_Common"));
	//m_ItemTable.insert(make_pair(48,"Lan_Exp_Soul_Bottle_Common"));
	//m_ItemTable.insert(make_pair(49,"Lan_EquipRefineItem_Common"));
	//m_ItemTable.insert(make_pair(50,"Lan_EquipSuperaddItem_Common"));
	string ItemTableFile = m_pSplitInfo->GetTgtPath()+"/"+m_pSplitInfo->GetItemFolder()+"/ItemTable.xml";
	ifstream ifsGmConflict(ItemTableFile.c_str());
	if (!ifsGmConflict)
	{
		return;
	}

	CXmlConfig* pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig("root", ifsGmConflict);
	ifsGmConflict.close();

	TiXmlNode* pXmlNodeFileTeam = pXmlCfg->Get<TiXmlElement*>("File_Team");
	if (!pXmlNodeFileTeam)
	{
		return;
	}

	for (TiXmlNode* pXmlNodeFileSection = pXmlNodeFileTeam->FirstChild("FileName"); pXmlNodeFileSection; pXmlNodeFileSection = pXmlNodeFileSection->NextSibling("FileName"))
	{
		TiXmlElement* pXmlElementFileName = pXmlNodeFileSection->ToElement();
		int id = atoi(pXmlElementFileName->Attribute("Id"));
		const char* szFileName = pXmlElementFileName->Attribute("LanName");
		m_ItemTable.insert(make_pair(id, szFileName));
	}

}

void CMakeReferRelation::CreateQuestReference(string& strFile)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string curQuest = oriFile.GetDataByRowCol(i,0);
		string secondCol = oriFile.GetDataByRowCol(i,1);
		if (secondCol == "触发任务方式")
		{
			BulitSprintTypeRefer(strFile,i,"触发任务方式");
		}
		else if (secondCol == "结束任务方式")
		{
			BulitSprintTypeRefer(strFile,i,"结束任务方式");
		}
		else if (secondCol == "物品需求")
		{
			BulitItemRequireRefer(strFile,i,"物品需求");
		}
		else if (secondCol == "杀怪需求")
		{
			BulitMonsterRequireRefer(strFile,i,"杀怪需求");
		}
		else if (secondCol == "交互对象使用需求")
		{
			BulitIntObjRequireeRefer(strFile,i,"使用交互对象");
		}
		else if (secondCol == "开启掉落")
		{
			BulitDropRefer(strFile,i,"开启掉落");
		}
		else if (secondCol == "物品奖励")
		{
			BulitItemAwardRefer(strFile,i,"物品奖励");
		}
		else if (secondCol == "获得物品")
		{
			BulitReceiveItemRefer(strFile,i,"获得物品");
		}
		else if (secondCol == "任务接取对话索引" || secondCol == "任务描述索引")
		{
			BulitNewDescRefer(strFile,i);
		}
	}
}
void CMakeReferRelation::BulitSprintTypeRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string curType = oriFile.GetDataByRowCol(row,3);
	string curContent = oriFile.GetDataByRowCol(row,4);
	string FileName = GetReferFileName(curType,curContent);
	//需要贾志旭核对,触发任务NPC是否直接是真实名
	string strIndex = mbcs_to_MemH64(curContent);
	string secondCol = oriFile.GetDataByRowCol(row,1);
	string strDescript;
	if (curType == "Npc")
	{
		if (strFun == "触发任务方式")
			strDescript = "接取任务NPC";
		else
			strDescript = "交任务NPC";
	}
	else
	{
		if(strFun == "触发任务方式")
			strDescript = "接取任务物品";
		else
			strDescript = "交任务对象";
	}
	CreateQuestReferMap(curQuest,FileName,curContent,strDescript);
}
void CMakeReferRelation::BulitItemRequireRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strItem = oriFile.GetDataByRowCol(row,2);
	int itemType = GetItemTypeFromString(oriFile.GetDataByRowCol(row,4));
	CheckItemType(itemType);
	string FileName = m_ItemTable[itemType] + ".txt";
	string strDescript = strFun;
	CreateQuestReferMap(curQuest,FileName,strItem,strDescript);
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitItemRequireRefer(strFile,row+1,strFun);
	}

}

void CMakeReferRelation::CheckItemType(int itemType)
{
	if (!m_ItemTable.count(itemType))
	{
		stringstream strStream;
		strStream<<"物品新添加了大类:"<<itemType<<endl;
		string strMsg;
		strStream>>strMsg;
		if(GetLogToFile())
		{
			PrintLog(strMsg);
		}
		else
		{
			GenErr("工具需要添加物品大类",strMsg.c_str());
		}
	}
}

void CMakeReferRelation::BulitMonsterRequireRefer(string& strFile,int row,string strFun)
{
	//针对杀怪需求均改为真实名的情况
	/*string curQuest = ProduceCommonStr(vecSourceCol[row][1].vecElement);
	string &sevenCol = vecSourceCol[row][7].vecElement[1];
	int firstDQuoPos = sevenCol.find("\"\"",1);
	int secondDQuoPos = sevenCol.find("\"\"",firstDQuoPos+2);
	string strNpcSet = sevenCol.substr(firstDQuoPos+2,secondDQuoPos-firstDQuoPos-2);
	string FileName = "Lan_Npc_Common.txt";
	string strKey = GetKeyPrefix(FileName);
	string strProjekt = GetProjekt(FileName);
	string strDescript = "Hunted monster for quest";
	while (strNpcSet.find(",",0) != -1)
	{
	int pos = strNpcSet.find(",",0);
	string strTemp = strNpcSet.substr(0,pos);
	strNpcSet = strNpcSet.substr(pos+1,strNpcSet.length()-pos-1);
	CleanEndSpace(strTemp);
	CleanOnePairQuotation(strTemp);
	string strIndex = ConverseStrTo64Radix(strTemp);
	strIndex = strKey + "_" + strIndex;
	CreateQuestReferMap(curQuest,FileName,strTemp,strProjekt,strIndex,strDescript);
	}
	CleanEndSpace(strNpcSet);
	CleanOnePairQuotation(strNpcSet);
	string strLastIndex = ConverseStrTo64Radix(strNpcSet);
	strLastIndex = strKey + "_" +strLastIndex;
	CreateQuestReferMap(curQuest,FileName,strNpcSet,strProjekt,strLastIndex,strDescript);
	if (row<vecSourceCol.size()-1 && ProduceCommonStr(vecSourceCol[row+1][1].vecElement) == curQuest && ProduceCommonStr(vecSourceCol[row+1][2].vecElement) == "\"\"")
	{
	BulitMonsterRequireRefer(vecSourceCol,row+1);
	}*/
	//由于贾志旭任务表永远不改NPC真实名,所以转表工具永远要写两份代码
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strNPC = oriFile.GetDataByRowCol(row,2);
	vector<string> vecNpc;
	QueryNpcIDByRealOrDisplay(strNPC,vecNpc);
	string FileName = "Lan_Npc_Common.txt";
	string strDescript = strFun;
	for (int i=0;i<vecNpc.size();++i)
	{
		string strIndex = vecNpc[i];
		string strRealName = MemH64_to_mbcs(strIndex);
		CreateQuestReferMap(curQuest,FileName,strRealName,strDescript);
	}
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitMonsterRequireRefer(strFile,row+1,strFun);
	}

}

void CMakeReferRelation::BulitIntObjRequireeRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string FileName = "Lan_IntObj_Common.txt";
	string strObj = oriFile.GetDataByRowCol(row,2);
	string strDescript = strFun;
	CreateQuestReferMap(curQuest,FileName,strObj,strDescript);
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitIntObjRequireeRefer(strFile,row+1,strFun);
	}
}

void CMakeReferRelation::BulitDropRefer(string& strFile,int row,string strFun)
{

}
void CMakeReferRelation::BulitItemAwardRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,4);
	while (strContent.find("{",0) != -1)
	{
		int firstPos = strContent.find("{",0);
		int secondPos = strContent.find("}",firstPos);
		string strTemp = strContent.substr(firstPos+1,secondPos-firstPos-1);
		int itemType = GetItemTypeFromString(strTemp);
		CheckItemType(itemType);
		string strItemFileName = m_ItemTable[itemType] + ".txt";
		string strItemName = GetItemNameFromStr(strTemp);
		strContent = strContent.substr(secondPos+1,strContent.length()-secondPos-1);
		string strDescript = strFun;
		CreateQuestReferMap(curQuest,strItemFileName,strItemName,strDescript);
	}
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitItemAwardRefer(strFile,row+1,strFun);
	}
}
void CMakeReferRelation::BulitReceiveItemRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,3);
	string FileName;
	if (strContent == "")
	{
		return;
	}
	int itemType = GetItemTypeFromString(strContent);
	CheckItemType(itemType);
	FileName = m_ItemTable[itemType] + ".txt";
	string strItem = GetItemNameFromStr(strContent);
	if (strItem != "")
	{
		string strDescript = strFun;
		CreateQuestReferMap(curQuest,FileName,strItem,strDescript);
	}
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitReceiveItemRefer(strFile,row+1,strFun);
	}
}
void CMakeReferRelation::BuiltIntObjUseRequireRefer(string& strFile,int row,string strFun)
{
	
}

void CMakeReferRelation::BulitNewDescRefer(string& strFile,int row,string strFun)
{
	COriFileOperate& oriFile = GetDifData(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	if (oriFile.BeNumber(row,3))
	{
		string strNum = oriFile.GetDataByRowCol(row,3);
		int num = atoi(strNum.c_str());
		AddQuestDescNum(curQuest,num);
	}
}

void CMakeReferRelation::CreateQuestReferMap(string& strQuest,string& strFileName,string& strName,string& strDescript)
{
	QuestReferData data;
	data.m_strFile = strFileName;
	data.m_strName = strName;
	data.m_strReferRelation = strDescript;
	data.m_strQuestName = strQuest;
	string strQuestFileName = GetCurQuestFile();
	if (!m_mapQuestRefer.count(strQuestFileName))
	{
		m_mapQuestRefer[strQuestFileName] = vector<QuestReferData>();
	}
	vector<QuestReferData>& vecRef = m_mapQuestRefer[strQuestFileName];
	if (find(vecRef.begin(),vecRef.end(),data) == vecRef.end())
	{
		vecRef.push_back(data);
	}
}

void CMakeReferRelation::RelateTwoWayRefer(string& strQuest,string& strQuestIndex,string& strLangContent)
{
	string strQuestFName = GetCurQuestFile();
	vector<QuestReferData>& vecRef = m_mapQuestRefer[strQuestFName];
	for (int i=0;i<vecRef.size();++i)
	{
		QuestReferData& data = vecRef[i];
		if (data.m_strQuestName == strQuest)
		{
			string strSearch = GetReferDataDisplayName(data);
			if (strLangContent.find(strSearch.c_str()) != -1)
			{
				string strSaveName = strQuestFName + "#" + data.m_strFile;
				InitTwoReferVecTab(strSaveName,true);
				vector<vector<string> >& vecReferRef = m_MapTwoRefer[strSaveName];
				vecReferRef.push_back(vector<string>());
				string strIndex = mbcs_to_MemH64(data.m_strName);
				vecReferRef.back().push_back(strQuestIndex);
				vecReferRef.back().push_back(strIndex);
				vecReferRef.back().push_back(data.m_strReferRelation);
			}
		}
	}
}

string CMakeReferRelation::GetReferDataDisplayName(QuestReferData& data)
{
	string strFileName = data.m_strFile;
	string strSrc;

	strSrc = m_pSplitInfo->GetSrcDifNameWithLangName(strFileName);

	COriFileOperate& oriFile = GetDifData(strSrc);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	string strResult;
	int pos;
	string strTemp = data.m_strName;
	if (data.m_strFile == "Lan_Npc_Common.txt")
	{
		string strSearch = m_pSplitInfo->GetDifColNameWithLangName(data.m_strFile,"Title");
		pos = GetColPosByName(strSearch,vecColName);
	}
	else
	{
		string strSearch = m_pSplitInfo->GetDifColNameWithLangName(data.m_strFile,"DisplayName");
		pos = GetColPosByName(strSearch,vecColName);
	}
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (strTemp == oriFile.GetDataByRowCol(i,0))
		{
			strResult = oriFile.GetDataByRowCol(i,pos);
			break;
		}
	}
	return strResult;
}

void CMakeReferRelation::InitTwoReferVecTab(string& strFileName,bool bQuest)
{
	if (m_MapTwoRefer.count(strFileName))
		return;
	vector<vector<string> > vecTemp;
	vecTemp.push_back(vector<string>());
	vecTemp[0].push_back("Key 1");
	vecTemp[0].push_back("Key 2");
	if (bQuest)
		vecTemp[0].push_back("Reference Description ");
	m_MapTwoRefer[strFileName] = vecTemp;
}

void CMakeReferRelation::CreateObjAndItemTwoWay()
{
	string strFunc = "创建中立Obj与Item之间关联信息";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strObjFile = "Lan_IntObj_Common.txt";
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strDif = "designer_IntObj_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetDifData(strDif);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	int typePos = GetColPosByName("TriggerType",vecColName);
	int argPos = GetColPosByName("TriggerAction",vecColName);
	int actionArgPos = GetColPosByName("ActionArg",vecColName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strObj = oriFile.GetDataByRowCol(i,0);
		string strType = oriFile.GetDataByRowCol(i,typePos);
		string strArg = oriFile.GetDataByRowCol(i,argPos);
		if (strType == "点击")
		{
			if(strArg == "可拾取的Obj")
			{
				string strIndex = mbcs_to_MemH64(strObj);
				int fileType = atoi(oriFile.GetDataByRowCol(i,actionArgPos).c_str());
				CheckItemType(fileType);
				string strFile = m_ItemTable[fileType] + ".txt";
				string strResultFileName;
				if (strObjFile < strFile)
					strResultFileName = strObjFile + "#" + strFile;
				else
					strResultFileName = strFile + "#" + strObjFile;

				if (strResultFileName == "Lan_IntObj_Common.txt")
				{
					int error = 1;
					string strError = "产生错误文件";
					if(GetLogToFile())
					{
						PrintLog(strError);
					}
					else
					{
						MessageBox(0,strError.c_str(),"error",NULL);
					}
				}
				InitTwoReferVecTab(strResultFileName);
				vector<vector<string> >& vecReferRef = m_MapTwoRefer[strResultFileName];
				vecReferRef.push_back(vector<string>());
				vecReferRef.back().push_back(strIndex);
				vecReferRef.back().push_back(strIndex);
			}
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CMakeReferRelation::BuiltQuestTwoWayRefer(string strFileName,vector<vector<string> >& vecRef)
{
	if (AdjustBeQuestTbl(strFileName))
	{
		string strReal = strFileName;
		int slashPos = strReal.rfind("/");
		if (slashPos != -1)
		{
			strReal = strReal.substr(slashPos+1,strReal.length()-slashPos-1);
		}
		SetCurQuestFile(strReal);
		for (int i=1;i<vecRef.size();++i)
		{
			string strKey = vecRef[i][0];
			string strQuestName = GetQuestNameFromMulitKey(strKey);
			strQuestName = strQuestName;
			string strContent = vecRef[i][1];
			wstring wcs_tmp = Utf8ToUCS2(strContent);
			strContent =  utf16_to_gbk(wcs_tmp);
			RelateTwoWayRefer(strQuestName,strKey,strContent);
		}
	}
}

void CMakeReferRelation::JudgeAndCreateRefer(string strFileName)
{
	string strLang = m_pSplitInfo->GetLangName(strFileName);
	SetCurQuestFile(strLang);
	CreateQuestReference(strFileName);
}

void CMakeReferRelation::BeginCreateQuestReferData()
{
	map<string,string>::iterator iter = m_mapFileList.begin();
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	for (;iter != m_mapFileList.end();++iter)
	{
		string strFile = iter->second;
		if (AdjustBeQuestTbl(strFile) && strFile.find("designer") != -1 && strFile.find(strLuaCfgExtend.c_str()) != -1)
		{
			JudgeAndCreateRefer(iter->first);
			//break;	//这里不能加break，否则会少生成文件
		}
	}
}

void CMakeReferRelation::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	string str;
	while(getline(inFile,str))
	{
		vecRef.push_back(vector<string>());
		int pos = -1;
		while ((pos = str.find('\t')) != -1)
		{
			string strTemp = str.substr(0,pos);
			vecRef[vecRef.size()-1].push_back(strTemp);
			str = str.substr(pos+1,str.length()-pos-1);
		}
		vecRef[vecRef.size()-1].push_back(str);
	}
	inFile.close();
}

void CMakeReferRelation::QueryNpcIDByRealOrDisplay(string& npc,vector<string>& result)
{
	result.clear();
	int titlePos = 2;
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFile = "designer_Npc_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetDifData(strFile);
	for (int i=1;i<oriFile.GetColCountByRow(0);++i)
	{
		if (oriFile.GetDataByRowCol(0,i) == "Title")
		{
			titlePos = i;
			break;
		}
	}
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (oriFile.GetDataByRowCol(i,titlePos) == npc || oriFile.GetDataByRowCol(i,0) == npc)
		{
			string name = oriFile.GetDataByRowCol(i,0);
			string ID = mbcs_to_MemH64(name);
			result.push_back(ID);
		}
	}
}

string CMakeReferRelation::GetReferFileName(string& strType,string& strContent,int itemType)
{
	if (strType == "Npc")
	{
		return "Lan_Npc_Common.txt";
	}
	else if (strType == "Obj")
	{
		return "Lan_IntObj_Common.txt";
	}
	else if (strType == "Goods")
	{
		if (itemType == 0)
		{
			int realType = QueryQuestItemType(strContent);
			string strResult = m_ItemTable[realType] + ".txt";
			return strResult;
		}
		else
			return m_ItemTable[itemType];
	}
	else
	{
		string strError = strType + ":任务触发方式并不是Npc,Obj,Goods三者之一";
		if(GetLogToFile())
		{
			PrintLog(strError);
		}
		else
		{
			MessageBox(0,strError.c_str(),NULL,0);
		}
		exit(0);
	}
}

int CMakeReferRelation::GetItemTypeFromString(string str)
{
	int commaPos = str.find(',');
	str = str.substr(0,commaPos);
	return atoi(str.c_str());
}

string CMakeReferRelation::GetItemNameFromStr(string str)
{
	int commaPos = str.find(',');
	int secondPos = str.find(",",commaPos+1);
	string strResult = str.substr(commaPos+1,secondPos-commaPos-1);
	if (strResult.length()>1 && strResult[0] == '\"' && strResult[strResult.length()-1] == '\"')
	{
		strResult.erase(strResult.begin());
		strResult.erase(strResult.begin()+strResult.length()-1);
	}
	return strResult;
}

int CMakeReferRelation::QueryQuestItemType(string& strItem)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strName = "designer_QuestItem_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetDifData(strName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (oriFile.GetDataByRowCol(i,0) == strItem)
		{
			return 16;
		}
	}
	return 1;
}

COriFileOperate& CMakeReferRelation::GetDifData(string& strFile)
{
	if(!m_mapOri.count(strFile))
	{
		string strPath = GetFilePath(strFile);
		m_mapOri[strFile] = new COriFileOperate(strPath);
		m_mapOri[strFile]->CleanBlanRow();
		m_mapOri[strFile]->CleanNoteRow();
	}
	return *m_mapOri[strFile];
}

string CMakeReferRelation::GetFilePath(string& strName)
{
	return m_mapFileList[strName];
}

int CMakeReferRelation::GetColPosByName(string strCol,vector<string>& vecRef)
{
	string strName = strCol;
	int pos = 0;
	for (int i = 0;i<vecRef.size();++i)
	{
		if (strName == vecRef[i])
		{
			pos = i;
			break;
		}
	}
	return pos;
}

bool CMakeReferRelation::AdjustBeQuestTbl(string& strFileName)
{
	string strSearch = "quest/" ;
	int pos = strFileName.find(strSearch.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	if (pos != -1 && strFileName.find("pos_") == -1 )
	{
		if (strFileName.find("CallBack_Quest_Common") != -1)
			return true;
		int pos2 = strFileName.find("/",pos+strSearch.length());
		if (pos2 != -1)
			return true;
		else if (strFileName.find(strLuaCfgExtend.c_str()) == -1)
		{
			if (strFileName.find("MercenaryLevelAward_Client") == -1 && strFileName.find("MercenaryLevelDirect_Common") ==-1
				&& strFileName.find("QuestHandBookDescription") == -1)
			{
				return true;
			}
		}
	}
	return false;
}

string CMakeReferRelation::GetQuestNameFromMulitKey(string str)
{
	int pos = str.find('_');
	string strQuestName = str.substr(0,pos);
	strQuestName = MemH64_to_mbcs(strQuestName);
	return strQuestName;
}

void CMakeReferRelation::RecordDifPath(string& str)
{
	string strTemp = str;
	int pos = strTemp.rfind("/");
	strTemp = strTemp.substr(pos+1,strTemp.length()-pos-1);
	m_mapFileList.insert(make_pair(strTemp,str));
}

FTW_RESULT CMakeReferRelation::CreateRefer( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CMakeReferRelation* pManager = CMakeReferRelation::GetInst();
	pManager->OnMakeRefer(strFileName);
	return _FTW_CONTINUNE;
}

void CMakeReferRelation::OnMakeRefer(string& strFileName)
{
	vector<vector<string> > vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);
	BuiltQuestTwoWayRefer(strFileName,vecBackUp);
}

void CMakeReferRelation::WriteReferTxt()
{
	map<string,vector<vector<string> > >::iterator iter = m_MapTwoRefer.begin();
	for (;iter != m_MapTwoRefer.end();++iter)
	{
		string strFileName = iter->first;
		replace_all_distinct(strFileName,".txt","",0);
		strFileName = strFileName + ".txt";
		strFileName = GetFileSavePath(strFileName);
		CreateFileDirectory(strFileName);
		ofstream outFile(strFileName.c_str());
		if (!outFile)
		{
			string strError = strFileName + "打开文件时发生错误";
			if(GetLogToFile())
			{
				PrintLog(strError);
			}
			else
			{
				MessageBox(0,strError.c_str(),"错误",NULL);
			}
		}
		vector<vector<string> >& vecRef = iter->second;
		for (int i=0;i<vecRef.size();++i)
		{
			for (int j=0;j<vecRef[i].size();++j)
			{
				string strTemp = vecRef[i][j];
				strTemp = gbk_to_utf8(strTemp);
				outFile<<strTemp;
				if(j < vecRef[i].size()-1)
					outFile<<"\t";
			}
			outFile<<"\n";
		}
		outFile.close();
	}
}

void CMakeReferRelation::WriteReferInfoToTxt()
{
	CMakeReferRelation* pManager = CMakeReferRelation::GetInst();
	pManager->WriteReferTxt();
}

string CMakeReferRelation::GetFileSavePath(string& strFileName)
{
	string strResult = strFileName;
	strResult = m_strReferPath + "/" + strResult;
	return strResult;
}

void CMakeReferRelation::CreateQuestDescRefer()
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strQuestDesc = "designer_QuestDescription" + strLuaCfgExtend;
	string strDescLan = "Lan_QuestDescription.txt";
	COriFileOperate& oriFile = GetDifData(strQuestDesc);
	map<string,vector<QuestReferData> >::iterator iter = m_mapQuestRefer.begin();
	for (;iter != m_mapQuestRefer.end();++iter)
	{
		vector<QuestReferData>& vecDataRef = iter->second;
		for (int i=0;i<vecDataRef.size();++i)
		{
			QuestReferData& data = vecDataRef[i];
			string strSearch = GetReferDataDisplayName(data);
			for (int j=1;j<oriFile.GetRowCount();++j)
			{
				string strDesc = oriFile.GetDataByRowCol(j,1);
				string strNum = oriFile.GetDataByRowCol(j,0);
				int num = atoi(strNum.c_str());
				if (!BeInDescQuestLst(data.m_strQuestName,num))
					continue;
				string strFile = data.m_strFile;
				string strSaveFile;
				strSaveFile = strDescLan + "#" + strFile;
				replace_all_distinct(strSaveFile,".txt","",0);
				strSaveFile = strSaveFile + ".txt";
				string strDataIndex = mbcs_to_MemH64(data.m_strName);
				if (strDesc.find(strSearch) != -1)
				{
					InitTwoReferVecTab(strSaveFile);
					vector<vector<string> >& vecReferRef = m_MapTwoRefer[strSaveFile];
					vecReferRef.push_back(vector<string>());
					vecReferRef.back().push_back(strNum);
					vecReferRef.back().push_back(strDataIndex);
				}
			}
		}
	}
	
}
bool CMakeReferRelation::BeInDescQuestLst(string strQuest,int num)
{
	vector<string>& vecRef = m_mapDesc[num];
	if(find(vecRef.begin(),vecRef.end(),strQuest) != vecRef.end())
		return true;
	else
		return false;
}

void CMakeReferRelation::AddQuestDescNum(string strQuest,int num)
{
	if (!m_mapDesc.count(num))
	{
		m_mapDesc[num] = vector<string>();
	}
	vector<string>& vecRef = m_mapDesc[num];
	if (find(vecRef.begin(),vecRef.end(),strQuest) == vecRef.end())
	{
		vecRef.push_back(strQuest);
	}
}

void CMakeReferRelation::CreateItemWithDesc()
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strItemDesc = "designer_ItemCommonDescript" + strLuaCfgExtend;
	string strItemLandDesc = "Lan_ItemCommonDescript.txt";
	map<string,string>::iterator iter = m_mapFileList.begin();
	for (;iter != m_mapFileList.end();++iter)
	{
		string wholePath = iter->second;
		string strFileName = iter->first;
		if (wholePath.find(strItemDesc) != -1 || wholePath.find("item/designerItem/") == -1)
			continue;
		string strLang = m_pSplitInfo->GetLangName(strFileName);
		COriFileOperate& oriFile = GetDifData(strFileName);
		vector<string> vecColName;
		oriFile.GetOneRow(vecColName,0);
		int pos = GetColPosByName("Description",vecColName);
		for (int i=1;i<oriFile.GetRowCount();++i)
		{
			if (oriFile.BeNumber(i,pos))
			{
				string strNum = oriFile.GetDataByRowCol(i,pos);
				wstring wstrNum = gbk_to_utf16(strNum);
				string strIndex = oriFile.GetDataByRowCol(i,0);
				wstring wstrIndex = gbk_to_utf16(strIndex);
				strIndex = mbcs_to_MemH64(strIndex);
				string strSaveName;
				if (strItemLandDesc < strLang)
				{
					strSaveName = strItemLandDesc + "#" + strLang;
					replace_all_distinct(strSaveName,".txt","",0);
					strSaveName = strSaveName + ".txt";
					InitTwoReferVecTab(strSaveName);
					vector<vector<string> >& vecReferRef = m_MapTwoRefer[strSaveName];
					vecReferRef.push_back(vector<string>());
					vecReferRef.back().push_back(strNum);
					vecReferRef.back().push_back(strIndex);
				}
				else
				{
					strSaveName = strLang + "#" + strItemLandDesc;
					replace_all_distinct(strSaveName,".txt","",0);
					strSaveName = strSaveName + ".txt";
					InitTwoReferVecTab(strSaveName);
					vector<vector<string> >& vecReferRef = m_MapTwoRefer[strSaveName];
					vecReferRef.push_back(vector<string>());
					vecReferRef.back().push_back(strIndex);
					vecReferRef.back().push_back(strNum);
				}
			}
		}
	}
}


#pragma warning(pop)
