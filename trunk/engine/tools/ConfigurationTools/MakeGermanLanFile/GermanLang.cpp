#include "stdafx.h"
#include "GermanLang.h"
#include "CXmlConfig.inl"
#include "CSplitInfo.h"
#include "common.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

CGermanLang::CGermanLang()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	ReadMappedXML();
	ReadItemTable();
	FileTreeWalk( gbk_to_utf16(m_pSplitInfo->GetSrcPath()).c_str(),MakeDifList, NULL );
	FileTreeWalk( gbk_to_utf16(m_pSplitInfo->GetCfgTempPath()).c_str(),MakeDifList, NULL );
	PostProcessWithLangTbl();
	m_bEnablePatch = false;
}

CGermanLang::~CGermanLang()
{

}

bool CGermanLang::ReadMappedXML()
{
	string strCfg = "NameCompare.xml";
	ifstream config(strCfg.c_str());
	if(!config)
		return false;

	CXmlConfig *pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig( "root" , config );
	config.close();

	TiXmlNode* pTableNode = pXmlCfg->Get<TiXmlElement*>("table");
	if( pTableNode == NULL )
	{
		delete pXmlCfg;
		if(GetLogToFile())
		{
			PrintLog("NameCompare.xml解析失败，不存在table节点");
		}
		else
		{
			MessageBox(0,"NameCompare.xml解析失败，不存在table节点",NULL,1);
		}
		return false; 
	}
	for(TiXmlNode* pNode = pTableNode->FirstChild("entry");pNode;pNode=pNode->NextSibling("entry"))
	{
		TiXmlElement* pElement = pNode->ToElement();
		string strName = pElement->Attribute("Name");
		string strMap = pElement->Attribute("MappedName");
		m_mapName.insert(make_pair(strName,strMap));
	}
	return true;
}

FTW_RESULT CGermanLang::FormatUtf2Utf16( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CGermanLang* pManager = CGermanLang::GetInst();
	pManager->FormatUtf8Vec(strFileName);
	return _FTW_CONTINUNE;
}

void CGermanLang::FormatUtf8Vec(string& strFileName)
{
	vector<vector<string> > vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);
	AddNoteLevel(strFileName,vecBackUp);
	SortLangDataByGrade(vecBackUp);
	if (AdjustBeQuestTbl(strFileName))
	{
		AddDescriptInfoToQuestTxt(vecBackUp);
		AddLeveToQuest(strFileName,vecBackUp);
		AddSceneNameToQuest(strFileName,vecBackUp);
		AddCoordinateToQuest(strFileName,vecBackUp);
	}
	Write2Utf16Txt(strFileName,vecBackUp);
}

void CGermanLang::Write2Utf16Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	string strSaveName = strFileName;
	const string& strSearch = CSplitInfo::GetInst()->GetLangSrcFolder();	//"langsrc";
	//string strDest = "german";
	const string& strGermanFolder = CSplitInfo::GetInst()->GetTransExportPath();
	int langsrcPos = strSaveName.find(strSearch);
	strSaveName = strSaveName.substr(langsrcPos+7,strSaveName.length()-langsrcPos-7);
	strSaveName = strGermanFolder + "/cfg/" +strSaveName;
	//replace_all_distinct(strSaveName,strSearch,strDest,0);
	if (strSaveName.find('/') != -1)
	{
		CreateFileDirectory(strSaveName);
	}
	strSaveName = GetPatchName(strSaveName);
	ofstream outFile(strSaveName.c_str(),ios_base::out | ios_base::binary);
	char headArray[2];
	headArray[0] = uint8(15*16+15);
	headArray[1] = uint8(15*16+14);
	outFile.write(headArray,2);
	char enterArray[4];
	enterArray[0] = 13;
	enterArray[1] = 0;
	enterArray[2] = 10;
	enterArray[3] = 0;
	char tableArray[2];
	tableArray[0] = 9;
	tableArray[1] = 0;
	wstring wstrTemp;
	for (int i=0;i<vecRef.size();++i)
	{
		for (int j=0;j<vecRef[0].size();++j)
		{
			wstrTemp = Utf8ToUCS2(vecRef[i][j]);
			outFile.write((char*)wstrTemp.c_str(),wstrTemp.size()*2);
			if (j<vecRef[0].size()-1)
			{
				outFile.write(tableArray,2);
			}
			else
			{
				outFile.write(enterArray,4);
			}
		}
	}
	outFile.close();
}

void CGermanLang::AddDescriptInfoToQuestTxt(vector<vector<string> >& vecRef)
{
	 for (int i=1;i<vecRef.size();++i)
	 {
		 string strIndex = vecRef[i][0];
		 int firstUnderline = strIndex.find('_');
		 int secondUnderline = strIndex.find('_',firstUnderline+1);
		 string strFun;
		 if (secondUnderline != -1)
		 {
			strFun  = strIndex.substr(firstUnderline+1,secondUnderline-firstUnderline-1);
		 }
		 else
		 {
			 strFun  = strIndex.substr(firstUnderline+1,strIndex.length()-firstUnderline-1);
		 }
		 
		 string strFunGBK = MemH64_to_mbcs(strFun);
		 string strReplace = m_mapName[strFunGBK];
		 replace_all_distinct(strIndex,strFun,strReplace,0);
		 vecRef[i][0] = strIndex;
	 }
}

FTW_RESULT CGermanLang::MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring wstr = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	if(  _wcsicmp( szFileName + nLen - wstr.length(), wstr.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CGermanLang* pManager = CGermanLang::GetInst();
	pManager->RecordDifPath(strFileName);
	return _FTW_CONTINUNE;
}

void CGermanLang::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
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

void CGermanLang::AddNoteLevel(string& strFileName,vector<vector<string> >& vecRef)
{
	string strReal = strFileName;
	int pos = strReal.rfind('/');
	if (pos != -1)
	{
		strReal = strReal.substr(pos+1,strReal.length()-pos-1);
	}
	AddGradeColToLangData(strReal,vecRef);
}

void CGermanLang::RecordDifPath(string& str)
{
	string strTemp = str;
	int pos = strTemp.rfind("/");
	strTemp = strTemp.substr(pos+1,strTemp.length()-pos-1);
	m_mapFileList.insert(make_pair(strTemp,str));
}

COriFileOperate& CGermanLang::GetOriFile(string& strFile)
{
	if (!m_mapOriFile.count(strFile))
	{
		string strPath = GetFilePath(strFile);
		m_mapOriFile[strFile] = new COriFileOperate(strPath);
		m_mapOriFile[strFile]->CleanBlanRow();
		m_mapOriFile[strFile]->CleanNoteRow();
	}
	return *m_mapOriFile[strFile];
}

string CGermanLang::GetFilePath(string& strName)
{
	return m_mapFileList[strName];
}

bool CGermanLang::AdjustBeQuestTbl(string& strFileName)
{
	if (strFileName.find("Lan_HeadHelp_Client.txt") != -1)
		return false;
	string strSearch = CSplitInfo::GetInst()->GetQuestFolder() + "/";	//"quest/" ;
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	int pos = strFileName.find(strSearch.c_str());
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

void CGermanLang::BeginCreateQuestGrade()
{
	string strFunc = "开始生成任务表相关Item Npc Obj等级信息";
	CConsoleInfo::GetInst()->PrintFunction(strFunc);
	map<string,string>::iterator iter = m_mapFileList.begin();
	for (;iter != m_mapFileList.end();++iter)
	{
		string str = iter->second;
		if (AdjustBeQuestTbl(str) && str.find("designer") != -1)
		{
			string strReal = iter->first;
			CreateQuestTableGradeInfor(strReal);
		}
	}
}

void CGermanLang::CreateQuestTableGradeInfor(string& strFile)
{
	CConsoleInfo::GetInst()->BeginToPrint(strFile.c_str());
	COriFileOperate& oriFile = GetOriFile(strFile);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string curQuest = oriFile.GetDataByRowCol(i,0);
		string secondCol = oriFile.GetDataByRowCol(i,1);
		if (secondCol == "接任务最小等级")
		{
			string strGrade = oriFile.GetDataByRowCol(i,3);
			int level = atoi(strGrade.c_str());
			RecordQuestLevel(curQuest,level);
			continue;
		}
	}
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string curQuest = oriFile.GetDataByRowCol(i,0);
		string secondCol = oriFile.GetDataByRowCol(i,1);
		if (secondCol == "触发任务方式" || secondCol == "结束任务方式")
		{
			BulitSprintTypeGrade(strFile,i);
		}
		else if (secondCol == "物品需求")
		{
			BulitItemRequireGrade(strFile,i);
		}
		else if (secondCol == "杀怪需求")
		{
			//针对杀怪需求均改为真实名的情况
			BulitMonsterRequireGrade(strFile,i);
		}
		else if (secondCol == "交互对象使用需求")
		{
			BulitIntObjRequireGrade(strFile,i);
		}
		else if (secondCol == "开启掉落")
		{
			BulitDropGrade(strFile,i);
		}
		else if (secondCol == "物品奖励")
		{
			BulitItemAwardGrade(strFile,i);
		}
		else if (secondCol == "获得物品")
		{
			BulitReceiveItemGrade(strFile,i);
		}
		else if (secondCol == "交互对象使用需求")
		{
			BuiltIntObjUseRequireGrade(strFile,i);
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::BulitSprintTypeGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string curType = oriFile.GetDataByRowCol(row,3);
	string curContent = oriFile.GetDataByRowCol(row,4);
	int itemType = 0;
	string FileName = GetReferFileName(curType,curContent);
	RecordGrade(FileName,curContent,GetQuestLevel(curQuest));
}

void CGermanLang::BulitItemRequireGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strItem = oriFile.GetDataByRowCol(row,2);
	int itemType = GetItemTypeFromString(oriFile.GetDataByRowCol(row,4));
	string FileName = m_ItemTable[itemType] + ".txt";
	RecordGrade(FileName,strItem,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 &&  oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitItemRequireGrade(strFile,row+1);
	}
}

void CGermanLang::BulitMonsterRequireGrade(string& strFile,int row)
{
	//针对杀怪需求均改为真实名的情况
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string &sevenCol = oriFile.GetDataByRowCol(row,6);
	int firstDQuoPos = sevenCol.find("\"",0);
	int secondDQuoPos = sevenCol.find("\"",firstDQuoPos+1);
	string strNpcSet = sevenCol.substr(firstDQuoPos+1,secondDQuoPos-firstDQuoPos-1);
	string FileName = "Lan_Npc_Common.txt";
	while (strNpcSet.find(",",0) != -1)
	{
		int pos = strNpcSet.find(",",0);
		string strTemp = strNpcSet.substr(0,pos);
		strNpcSet = strNpcSet.substr(pos+1,strNpcSet.length()-pos-1);
		RecordGrade(FileName,strTemp,GetQuestLevel(curQuest));
	}
	RecordGrade(FileName,strNpcSet,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitMonsterRequireGrade(strFile,row+1);
	}
}

void CGermanLang::BulitIntObjRequireGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string FileName = "Lan_IntObj_Common.txt";
	string strObj = oriFile.GetDataByRowCol(row,2);
	RecordGrade(FileName,strObj,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitIntObjRequireGrade(strFile,row+1);
	}
}

void CGermanLang::BulitDropGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,3);
	string FileName;
	int itemType = GetItemTypeFromString(oriFile.GetDataByRowCol(row,4));
	string strItemFileName = m_ItemTable[itemType] + ".txt";
	string strItem = GetItemNameFromStr(oriFile.GetDataByRowCol(row,4));
	if (BeInNpc_Common(strContent))
	{
		FileName = "Lan_Npc_Common.txt";
	}
	else
	{
		FileName = "Lan_IntObj_Common.txt";
	}
	RecordGrade(FileName,strContent,GetQuestLevel(curQuest));
	RecordGrade(strItemFileName,strItem,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitDropGrade(strFile,row+1);
	}
}

void CGermanLang::BulitItemAwardGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,4);
	while (strContent.find("{",0) != -1)
	{
		int firstPos = strContent.find("{",0);
		int secondPos = strContent.find("}",firstPos);
		string strTemp = strContent.substr(firstPos+1,secondPos-firstPos-1);
		int itemType = GetItemTypeFromString(strTemp);
		string strItemFileName = m_ItemTable[itemType] + ".txt";
		string strItemName = GetItemNameFromStr(strTemp);
		RecordGrade(strItemFileName,strItemName,GetQuestLevel(curQuest));
		strContent = strContent.substr(secondPos+1,strContent.length()-secondPos-1);
	}
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitItemAwardGrade(strFile,row+1);
	}
}

void CGermanLang::BulitReceiveItemGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,3);
	string FileName;
	int itemType = GetItemTypeFromString(strContent);
	FileName = m_ItemTable[itemType] + ".txt";
	string strItem = GetItemNameFromStr(strContent);
	RecordGrade(FileName,strItem,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BulitReceiveItemGrade(strFile,row+1);
	}
}

void CGermanLang::BuiltIntObjUseRequireGrade(string& strFile,int row)
{
	COriFileOperate& oriFile = GetOriFile(strFile);
	string curQuest = oriFile.GetDataByRowCol(row,0);
	string strContent = oriFile.GetDataByRowCol(row,2);
	string FileName = "Lan_IntObj_Common.txt";
	RecordGrade(FileName,strContent,GetQuestLevel(curQuest));
	if (row<oriFile.GetRowCount()-1 && oriFile.GetDataByRowCol(row+1,0) == curQuest && oriFile.GetDataByRowCol(row+1,1) == "")
	{
		BuiltIntObjUseRequireGrade(strFile,row+1);
	}
}

int CGermanLang::GetQuestLevel(string& strQuest)
{
	if (!m_mapQuestLevel.count(strQuest))
	{
		string strMsg = strQuest + "任务不存在等级信息，这是不允许的";
		if(GetLogToFile())
		{
			PrintLog(strMsg);
		}
		else
		{
			MessageBox(0,strMsg.c_str(),"Error",NULL);
		}
	}
	return m_mapQuestLevel[strQuest];
}

void CGermanLang::RecordQuestLevel(string& strQuest,int level)
{
	if (m_mapQuestLevel.count(strQuest))
	{
		m_mapQuestLevel[strQuest] = level;
	}
	else
	{
		m_mapQuestLevel.insert(make_pair(strQuest,level));
	}
}

string CGermanLang::GetReferFileName(string& strType,string& strContent,int itemType)
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

void CGermanLang::RecordGrade(string strFileName,string strIndex,int grade)
{
	GradeData data;
	data.m_etype = GradeData::eIT_String;
	data.m_strIndex = strIndex;
	data.m_grade = grade;
	if (!m_mapGrade.count(strFileName))
	{
		m_mapGrade[strFileName] = vector<GradeData>();
	}
	vector<GradeData>& vecGrade = m_mapGrade[strFileName];
	vector<GradeData>::iterator iter = vecGrade.end();
	for (int i=0;i<vecGrade.size();++i)
	{
		if (vecGrade[i].m_etype == data.m_etype && vecGrade[i].m_strIndex == data.m_strIndex)
		{
			iter = vecGrade.begin() + i;
			break;
		}
	}
	if (iter == m_mapGrade[strFileName].end())
	{
		m_mapGrade[strFileName].push_back(data);
	}
	else
	{
		if (iter->m_grade > grade)
		{
			iter->m_grade = grade;
		}
	}
}

void CGermanLang::RecordGrade(string strFileName,int index,int grade)
{
	GradeData data;
	data.m_etype = GradeData::eIT_Number;
	data.m_index = index;
	data.m_grade = grade;
	if (!m_mapGrade.count(strFileName))
	{
		m_mapGrade[strFileName] = vector<GradeData>();
	}
	vector<GradeData>& vecGrade = m_mapGrade[strFileName];
	vector<GradeData>::iterator iter = vecGrade.end();
	for (int i=0;i<vecGrade.size();++i)
	{
		if (vecGrade[i].m_etype == data.m_etype && vecGrade[i].m_index == data.m_index)
		{
			iter = vecGrade.begin() + i;
			break;
		}
	}
	if (iter == m_mapGrade[strFileName].end())
	{
		m_mapGrade[strFileName].push_back(data);
	}
	else
	{
		if (iter->m_grade > grade)
		{
			iter->m_grade = grade;
		}
	}
}

int CGermanLang::GetItemTypeFromString(string str)
{
	int commaPos = str.find(',');
	str = str.substr(0,commaPos);
	return atoi(str.c_str());
}

void CGermanLang::ReadItemTable()
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

string CGermanLang::GetItemNameFromStr(string str)
{
	int commaPos = str.find(',');
	int secondPos = str.find(",",commaPos+1);
	string strResult = str.substr(commaPos+1,secondPos-commaPos-1);
	return strResult;
}

bool CGermanLang::BeInNpc_Common(string strName)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string str = "designer_Npc_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(str);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (strName == oriFile.GetDataByRowCol(i,0))
		{
			return true;
		}
	}
	return false;
}

int CGermanLang::QueryQuestItemType(string& strItem)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strName = "designer_QuestItem_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (oriFile.GetDataByRowCol(i,0) == strItem)
		{
			return 16;
		}
	}
	return 1;
}

void CGermanLang::CreateNpcGrade()
{
	string strFunc = "根据Npc_Common表生成Npc等级信息";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strName = "designer_Npc_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strName);
	vector<string> vecName;
	oriFile.GetOneRow(vecName,0);
	int pos = GetColPosByName("MinLevel",vecName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strName = oriFile.GetDataByRowCol(i,0);
		int grade = atoi(oriFile.GetDataByRowCol(i,pos).c_str());
		RecordGrade("Lan_Npc_Common.txt",strName,grade);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

int CGermanLang::GetColPosByName(string strCol,vector<string>& vecRef)
{
	for (int i=0;i<vecRef.size();++i)
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

void CGermanLang::PostProcessWithLangTbl()
{
		BeginCreateQuestGrade();
		CreateNpcGrade();
		CreateAlcazarNpcGrade();
		CreateAlcazarObjGrade();
		CreateNpcDropItemObj();
		CreateNpcDropItem();
//		string strSearch("../../cfg/data/tgt/scene");
		string strSearch = m_pSplitInfo->GetScenePath();

		string strFunc = "关联场景中Obj等级与附近Npc等级一致";
		CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
		FileTreeWalk(gbk_to_utf16(strSearch).c_str(),StartEvaluateObj,NULL);
		CConsoleInfo::GetInst()->EndToPrint();

		RelationObjAndItem();
		CreateNpcShopGrade();
		
		CreateNpcTheaterGrade();
		CreateArmorPieceEnactment();
}

//各主城里NPC统一按等级1处理
void CGermanLang::CreateAlcazarNpcGrade()
{
	string strFunc = "主城中相关Npc等级统一设置为1";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strPath = m_pSplitInfo->GetScenePath();
	strPath = strPath+"/";
//	string strPath("../../cfg/data/tgt/scene/");
	vector<string> vecFile;
	vecFile.push_back("ShiKanBuLeiCheng.lua");
	vecFile.push_back("TianLiangDu.lua");
	vecFile.push_back("WuLuCheng.lua");
	vecFile.push_back("ShuWuJiuBa_AiMi.lua");
	vecFile.push_back("ShuWuJiuBa_ShenSheng.lua");
	vecFile.push_back("ShuWuJiuBa_XiuSi.lua");
	string strNpcFile("Lan_Npc_Common.txt");
	for (int i=0;i<vecFile.size();++i)
	{
		vector<int> vecRow;
		string strFileName = strPath + vecFile[i];
		GetLuaStartEndRow(strFileName,"local _npc = {","}",vecRow);
		ifstream inFile(strFileName.c_str());
		if (!inFile)
			continue;
		int rowCount=0;
		while (rowCount<vecRow[0])
		{
			string strTemp;
			getline(inFile,strTemp);
			rowCount++;
		}
		while (rowCount<vecRow[1])
		{
			string strTemp;
			getline(inFile,strTemp);
			int pos = strTemp.find(',');
			string strNpc = strTemp.substr(1,pos-1);
			RecordGrade(strNpcFile,strNpc,1);
			rowCount++;
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::CreateAlcazarObjGrade()
{
	string strFunc = "主城中相关Obj等级统一设置为1";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
//	string strPath("../../cfg/data/tgt/scene/");
	string strPath = m_pSplitInfo->GetScenePath();
	strPath = strPath+"/";
	vector<string> vecFile;
	vecFile.push_back("ShiKanBuLeiCheng.lua");
	vecFile.push_back("TianLiangDu.lua");
	vecFile.push_back("WuLuCheng.lua");
	vecFile.push_back("ShuWuJiuBa_AiMi.lua");
	vecFile.push_back("ShuWuJiuBa_ShenSheng.lua");
	vecFile.push_back("ShuWuJiuBa_XiuSi.lua");
	string strObjFile("Lan_IntObj_Common.txt");
	for (int i=0;i<vecFile.size();++i)
	{
		vector<int> vecRow;
		string strFileName = strPath + vecFile[i];
		GetLuaStartEndRow(strFileName,"local _obj = {","}",vecRow);
		ifstream inFile(strFileName.c_str());
		if (!inFile)
			continue;
		int rowCount=0;
		while (rowCount<vecRow[0])
		{
			string strTemp;
			getline(inFile,strTemp);
			rowCount++;
		}
		while (rowCount<vecRow[1])
		{
			string strTemp;
			getline(inFile,strTemp);
			int pos = strTemp.find(',');
			string strObj = strTemp.substr(1,pos-1);
			RecordGrade(strObjFile,strObj,1);
			rowCount++;
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::CreateNpcDropItemObj()
{
	string strFunc = "关联Npc掉落Obj等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "NpcDropItemObj_Server" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	string strNpcFile("Lan_Npc_Common.txt");
	string strObjFile("Lan_IntObj_Common.txt");
	string strSearch("DropTeam");
	vector<int> vecSearchPos;
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	GetSearchColPos(vecColName,strSearch,vecSearchPos);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strNpc = oriFile.GetDataByRowCol(i,0);
		int grade = GetComGrade(strNpcFile,strNpc);
		vector<string> vecObj;
		for (int j=0;j<vecSearchPos.size();++j)
		{
			string strGroup = oriFile.GetDataByRowCol(i,vecSearchPos[j]);
			if (strGroup != "")
			{
				GetDropItemObjName(strGroup,vecObj);
			}
		}
		for (int j=0;j<vecObj.size();++j)
		{
			RecordGrade(strObjFile,vecObj[j],grade);
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::CreateNpcDropItem()
{
	string strFunc = "关联Npc掉落Item等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "NpcDropItem_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	string strNpcFile("Lan_Npc_Common.txt");
	string strSearch("DropTeam");
	vector<int> vecSearchPos;
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	GetSearchColPos(vecColName,strSearch,vecSearchPos);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strNpc = oriFile.GetDataByRowCol(i,0);
		int grade = GetComGrade(strNpcFile,strNpc);
		vector<string> vecItem;
		vector<int> vecType;
		for (int j=0;j<vecSearchPos.size();++j)
		{
			string strGroup = oriFile.GetDataByRowCol(i,vecSearchPos[j]);
			if (strGroup != "")
			{
				GetDropItemNameType(strGroup,vecItem,vecType);
			}
		}
		for (int j=0;j<vecItem.size();++j)
		{
			string strFileName = m_ItemTable[vecType[j]] + ".txt";
			RecordGrade(strFileName,vecItem[j],grade);
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::RelationObjAndItem()
{
	string strFunc = "关联Obj点击触发相关Item Obj等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strIntObjFile = "designer_IntObj_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strIntObjFile);
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
		int grade = GetComGrade("Lan_IntObj_Common.txt",strObj);
		if (strType == "点击")
		{
			if (strArg.find("采集") != -1)
			{
				ObjRandomDropItem_Server(strObj,grade);
			}
			else if(strArg == "可拾取的Obj")
			{
				int fileType = atoi(oriFile.GetDataByRowCol(i,actionArgPos).c_str());
				string strFile = m_ItemTable[fileType] + ".txt";
				RecordGrade(strFile,strObj,grade);
			}
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::CreateNpcTheaterGrade()
{
	string strFunc = "生成Npc剧场表内容等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "designer_NpcTheater_Server" + strLuaCfgExtend;
	string strLangName("Lan_NpcTheater_Server.txt");
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	int pos = GetColPosByName("NpcName",vecColName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strNpc = oriFile.GetDataByRowCol(i,pos);
		if (strNpc != "")
		{
			int grade = GetComGrade("Lan_Npc_Common.txt",strNpc);
			string strIndex = oriFile.GetDataByRowCol(i,0);
			RecordGrade(strLangName,strIndex,grade);
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanLang::CreateArmorPieceEnactment()
{
	string strFunc = "生成ArmorPieceEnactment表等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "designer_ArmorPieceEnactment_Common" + strLuaCfgExtend;
	string strLangName("Lan_ArmorPieceEnactment_Common.txt");
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	int pos = GetColPosByName("BaseLevel",vecColName);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strGrade = oriFile.GetDataByRowCol(i,pos);
		int grade = atoi(strGrade.c_str());
		string strIndex = oriFile.GetDataByRowCol(i,0);
		RecordGrade(strLangName,strIndex,grade);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}


FTW_RESULT CGermanLang::StartEvaluateObj( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".lua" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CGermanLang* pManger = CGermanLang::GetInst();
	pManger->EvaluateObjGrade(strFileName);
	return _FTW_CONTINUNE;
}

void CGermanLang::EvaluateObjGrade(string& strFileName)
{
	vector<int> vecObjRow;
	vector<int> vecNpcRow;
	GetLuaStartEndRow(strFileName,"local _obj = {","}",vecObjRow);
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	if (vecObjRow.empty())
		return;
	string strObjFile("Lan_IntObj_Common.txt");
	int rowCount=0;
	while (rowCount<vecObjRow[0])
	{
		string strTemp;
		getline(inFile,strTemp);
		rowCount++;
	}
	while (rowCount<vecObjRow[1])
	{
		string strTemp;
		getline(inFile,strTemp);
		int pos = strTemp.find(',');
		string strObj = strTemp.substr(1,pos-1);
		vector<int> vecSelf;
		GetXYCoord(strTemp,vecSelf);
		int grade = GetNearestNpcGrade(strFileName,vecSelf[0],vecSelf[1]);
		RecordGrade(strObjFile,strObj,grade);
		rowCount++;
	}
}

int CGermanLang::GetNearestNpcGrade(string& strFileName,int x,int y)
{
	vector<int> vecNpcRow;
	GetLuaStartEndRow(strFileName,"local _npc = {","}",vecNpcRow);
	ifstream inFile(strFileName.c_str());
	if (!inFile || vecNpcRow.empty())
		return 999;
	int rowCount=0;
	int dst = 1000000;
	string strNpc;
	while (rowCount<vecNpcRow[0])
	{
		string strTemp;
		getline(inFile,strTemp);
		rowCount++;
	}
	while (rowCount<vecNpcRow[1])
	{
		string strTemp;
		getline(inFile,strTemp);
		int pos = strTemp.find(',');
		string strNpcTemp= strTemp.substr(1,pos-1);
		vector<int> vecXY;
		GetXYCoord(strTemp,vecXY);
		int xLength = vecXY[0] - x;
		int yLength = vecXY[1] - y;
		if (xLength*xLength+yLength*yLength<dst)
		{
			strNpc = strNpcTemp;
		}
		rowCount++;
	}
	if (strNpc == "")
		return 999;
	else
		return GetComGrade("Lan_Npc_Common.txt",strNpc);
}

void CGermanLang::GetXYCoord(string str,vector<int>& vecResult)
{
	int firstComma = str.find(',');
	int secondComma = str.find(',',firstComma+1);
	int thirdComma = str.find(',',secondComma+1);
	string strX = str.substr(firstComma+1,secondComma-firstComma-1);
	string strY = str.substr(secondComma+1,thirdComma-secondComma-1);
	int x = atoi(strX.c_str());
	int y = atoi(strY.c_str());
	vecResult.push_back(x);
	vecResult.push_back(y);
}

void CGermanLang::GetLuaStartEndRow(string& strFileName,string start,string end,vector<int>& vecResult)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
	{
		return;
	}
	int i=0;
	while (inFile.peek()!= EOF)
	{
		string strTemp;
		getline(inFile,strTemp);
		if (strTemp == start)
		{
			vecResult.push_back(i);
		}
		else if (strTemp == end && !vecResult.empty())
		{
			vecResult.push_back(i);
			return;
		}
		++i;
	}
}

void CGermanLang::AddGradeColToLangData(string& strName,vector<vector<string> >& vecRef)
{
	if (strName == "Lan_ArmorPieceEnactment_Common.txt")
	{
		int i=111;
	}
	string strNpcShowContent = "Lan_NpcShowContent_Common.txt";
	if (strName == strNpcShowContent)
	{
		CreateNpcShowContent_CommonGrade(vecRef);
	}
	else if (m_mapGrade.count(strName))
	{
		vecRef[0].push_back("note(Level)");
		vector<GradeData>& vecGrade = m_mapGrade[strName];
		vector<GradeData>::iterator iter = vecGrade.end();
		for (int i=1;i<vecRef.size();++i)
		{
			string strIndex = vecRef[i][0];
			strIndex = MemH64_to_mbcs(strIndex);
			for (int v = 0;v<vecGrade.size();++v)
			{
				if (vecGrade[v].m_strIndex == strIndex)
				{
					iter = vecGrade.begin()+v;
					break;
				}
			}
			if (iter != vecGrade.end())
			{
				int grade = iter->m_grade;
				char buff[10];
				_itoa(grade,buff,10);
				string strGrade= buff;
				vecRef[i].push_back(strGrade);
			}
			else
				vecRef[i].push_back("999");
		}
	}
}

void CGermanLang::SortLangDataByGrade(vector<vector<string> >& vecRef)
{
	int pos = -1;
	for (int i = 0;i<vecRef[0].size();++i)
	{
		if (vecRef[0][i] == "note(Level)")
		{
			pos = i;
			break;
		}
	}
	if (pos != -1)
	{
		for (int i=1;i<vecRef.size();++i)
		{
			for (int j=i+1;j<vecRef.size();++j)
			{
				int grade1 = atoi(vecRef[i][pos].c_str());
				int grade2 = atoi(vecRef[j][pos].c_str());
				if (grade1>grade2)
				{
					vector<string> vecTemp = vecRef[i];
					vecRef[i] = vecRef[j];
					vecRef[j] = vecTemp;
				}
			}
		}
	}
}

void CGermanLang::GetSearchColPos(vector<string>& vecRef,string strName,vector<int>& vecResult)
{
	for (int i=0;i<vecRef.size();++i)
	{
		string strColName = vecRef[i];
		if (strColName.find(strName.c_str()) != -1)
		{
			vecResult.push_back(i);
		}
	}
}

int CGermanLang::GetComGrade(string strFileName,string str)
{
	vector<GradeData>& vecGrade = m_mapGrade[strFileName];
	for (int i = 0;i<vecGrade.size();++i)
	{
		if (vecGrade[i].m_strIndex == str)
		{
			return vecGrade[i].m_grade;
		}
	}
	return 999;
}

void CGermanLang::GetDropItemObjName(string& strDropGroup,vector<string>& vecResult)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "DropItemObjTeam_Server" + strLuaCfgExtend;
	string strFind("DropName");
	GetSearchContent(strFileName,strFind,strDropGroup,vecResult);
}

void CGermanLang::GetSearchContent(string& strFileName,string& strFind,string &strGroup,vector<string>& vecResult)
{
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<int> vecInt;
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	GetSearchColPos(vecColName,strFind,vecInt);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		if (oriFile.GetDataByRowCol(i,0) == strGroup)
		{
			for (int j=0;j<vecInt.size();++j)
			{
				string strTemp = oriFile.GetDataByRowCol(i,vecInt[j]);
				if (strTemp != "")
				{
					vecResult.push_back(strTemp);
				}
			}
		}
	}
}

void CGermanLang::GetDropItemNameType(string& strDropGroup,vector<string>& vecName,vector<int>& vecType)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName =  "DropTeam_Server" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	string strSearch("DropName");
	vector<int> vecSearchPos;
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	GetSearchColPos(vecColName,strSearch,vecSearchPos);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strGroup =oriFile.GetDataByRowCol(i,0);
		if (strGroup == strDropGroup)
		{
			for (int j=0;j<vecSearchPos.size();++j)
			{
				string strItem = oriFile.GetDataByRowCol(i,vecSearchPos[j]);
				if (strItem != "")
				{
					string strLevel = oriFile.GetDataByRowCol(i,vecSearchPos[j]-1);
					int itemType = atoi(strLevel.c_str());
					vecName.push_back(strItem);
					vecType.push_back(itemType);
				}
			}
		}
	}
}

void CGermanLang::ObjRandomDropItem_Server(string& strDropGroup,int grade)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "ObjRandomDropItem_Server" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	vector<int> vecSearch;
	GetSearchColPos(vecColName,"GetItemTeam",vecSearch);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strName = oriFile.GetDataByRowCol(i,0);
		if (strName == strDropGroup)
		{
			for (int j=0;j<vecSearch.size();++j)
			{
				string strGroup = oriFile.GetDataByRowCol(i,vecSearch[j]);
				if (strGroup != "")
				{
					vector<string> vecItem;
					vector<int> vecType;
					GetObjDropNameType(strGroup,vecItem,vecType);
					for (int k=0;k<vecItem.size();++k)
					{
						string strFile = m_ItemTable[vecType[k]] + ".txt";
						RecordGrade(strFile,vecItem[k],grade);
					}
				}
			}
		}
	}
}

void CGermanLang::GetObjDropNameType(string& strDropGroup,vector<string>& vecName,vector<int>& vecType)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "ObjDropItemTeam_Server" +strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	string strSearch("ItemName");
	vector<int> vecSearchPos;
	GetSearchColPos(vecColName,strSearch,vecSearchPos);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strGroup = oriFile.GetDataByRowCol(i,0);
		if (strGroup == strDropGroup)
		{
			for (int j=0;j<vecSearchPos.size();++j)
			{
				string strItem = oriFile.GetDataByRowCol(i,vecSearchPos[j]);
				if (strItem != "")
				{
					string strLevel = oriFile.GetDataByRowCol(i,vecSearchPos[j]-1);
					int itemType = atoi(strLevel.c_str());
					vecName.push_back(strItem);
					vecType.push_back(itemType);
				}
			}
		}
	}
}

void CGermanLang::CreateNpcShowContent_CommonGrade(vector<vector<string> >& vecTemp)
{
	vecTemp[0].push_back("note(Level)");
	for (int i=1;i<vecTemp.size();++i)
	{
		string strNpc = GetFirstIndexFromBase64(vecTemp[i][0]);
		if (strNpc != "")
		{
			int grade = GetComGrade("Lan_Npc_Common.txt",strNpc);
			char p[10];
			_itoa(grade,p,10);
			vecTemp[i].push_back(p);
		}
	}
}

string CGermanLang::GetFirstIndexFromBase64(string& str)
{
	string strIndex = str;
	int pos = strIndex.find("_");
	if (pos != -1)
	{
		strIndex = strIndex.substr(0,pos);
	}
	strIndex = MemH64_to_mbcs(strIndex);
	return strIndex;
}

void CGermanLang::CreateNpcShopGrade()
{
	string strFunc = "生成Npc商店相关Item等级";
	CConsoleInfo::GetInst()->BeginToPrint(strFunc.c_str());
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strFileName = "NpcShop_Common" + strLuaCfgExtend;
	COriFileOperate& oriFile = GetOriFile(strFileName);
	vector<string> vecColName;
	oriFile.GetOneRow(vecColName,0);
	vector<int> vecSearch;
	GetSearchColPos(vecColName,"ItemName",vecSearch);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		for (int j=0;j<vecSearch.size();++j)
		{
			string strItem = oriFile.GetDataByRowCol(i,vecSearch[j]);
			if (strItem != "")
			{
				string strType = oriFile.GetDataByRowCol(i,vecSearch[j]-1);
				int itemType = atoi(strType.c_str());
				string strFile = m_ItemTable[itemType] + ".txt";
				RecordGrade(strFile,strItem,1);
			}
		}
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

string CGermanLang::GetQuestNameFromMulitKey(string str)
{
	int pos = str.find('_');
	string strQuestName = str.substr(0,pos);
	strQuestName = MemH64_to_mbcs(strQuestName);
	return strQuestName;
}

void CGermanLang::AddLeveToQuest(string& strFileName,vector<vector<string> >& vecRef)
{
	string strCur;
	bool bLast =false;
	for (int i=1;i<vecRef.size();++i)
	{
		if (bLast)
			break;
		string strKey = vecRef[i][0];
		string strQuest = GetQuestNameFromMulitKey(strKey);
		if (strCur == "")
		{
			strCur = strQuest;
		}
		if (strQuest != strCur || i == vecRef.size()-1)
		{
			if (i == vecRef.size()-1)
			{
				bLast = true;
			}
			string strTemp = strCur;
			int grade = GetQuestLevel(strTemp);
			char szgrade[10];
			_itoa(grade,szgrade,10);
			string strGrade = szgrade;
			string strNewKey = mbcs_to_MemH64(strCur);
			strNewKey = strNewKey + "_" + "note(level)";
			if (bLast)
			{
				vecRef.push_back(vector<string>());
				vecRef.back().push_back(strNewKey);
				vecRef.back().push_back(strGrade);
			}
			else
			{
				vecRef.insert(vecRef.begin()+i,vector<string>());
				vecRef[i].push_back(strNewKey);
				vecRef[i].push_back(strGrade);
			}
			strCur = strQuest;
		}
	}
	
	vector<vector<string> > vecResult;
	vecResult.push_back(vecRef[0]);
	vector<vector<string> > vecQuest;
	string strPreQuestName = "";
	int nPos = -1;
	for (int i = 1; i < (int)vecRef.size(); i++)
	{
		string strKey = vecRef[i][0];
		string strQuestName = GetQuestNameFromMulitKey(strKey);
		if (strPreQuestName == "")
		{
			strPreQuestName = strQuestName;
		}
		if (strQuestName != strPreQuestName)
		{
			int nCurGrade = atoi(vecQuest[vecQuest.size() - 1][1].c_str());
			int j;
			for (j = 1; j < (int)vecResult.size(); j++)
			{
				string strKey = vecResult[j][0];
				if (strKey.find("_Name") != string::npos)
				{
					nPos = j;
				}
				string strQuestName = GetQuestNameFromMulitKey(strKey);
				if (strKey.find("_note(level)") != string::npos)
				{
					int nGrade = atoi(vecResult[j][1].c_str());
					if (nCurGrade < nGrade)
					{
						vecResult.insert(vecResult.begin() + nPos, vecQuest.begin(), vecQuest.end());
						break;
					}
				}
			}
			if (j == (int)vecResult.size())
			{
				vecResult.insert(vecResult.end(), vecQuest.begin(), vecQuest.end());
			}
			vecQuest.clear();
			vecQuest.push_back(vecRef[i]);
			strPreQuestName = strQuestName;
		}
		else
		{
			vecQuest.push_back(vecRef[i]);
		}
	}

	// 处理最后一个任务
	string strKey;
	int nCurGrade = 999;
	if (vecQuest.size()>1)
	{
		strKey = vecQuest[vecQuest.size() - 1][0];
	}
	if (strKey.find("_note(level)") != string::npos)
	{
		nCurGrade = atoi(vecQuest[vecQuest.size() - 1][1].c_str());
	}
	int j = -1;
	for (j = 1; j < (int)vecResult.size(); j++)
	{
		string strKey = vecResult[j][0];
		if (strKey.find("_Name") != string::npos)
		{
			nPos = j;
		}
		string strQuestName = GetQuestNameFromMulitKey(strKey);
		if (strKey.find("_note(level)") != string::npos)
		{
			int nGrade = atoi(vecResult[j][1].c_str());
			if (nCurGrade < nGrade)
			{
				vecResult.insert(vecResult.begin() + nPos, vecQuest.begin(), vecQuest.end());
				break;
			}
		}
	}
	if (j == (int)vecResult.size())
	{
		vecResult.insert(vecResult.end(), vecQuest.begin(), vecQuest.end());
	}

	vecRef.clear();
	vecRef = vecResult;
}

void CGermanLang::AddSceneNameToQuest(string& strFileName,vector<vector<string> >& vecRef)
{
	string strCur;
	bool bLast = false;
	for (int i=1;i<vecRef.size();++i)
	{
		if (bLast)
			break;
		string strKey = vecRef[i][0];
		string strQuest = GetQuestNameFromMulitKey(strKey);
		if (strCur == "")
		{
			strCur = strQuest;
		}
		if (strQuest != strCur || i == vecRef.size()-1)
		{
			if (i == vecRef.size()-1)
			{
				bLast = true;
			}
			string strScene = GetQuestSecen(strFileName,strCur);
			if (strScene != "")
			{
				string strNewKey = mbcs_to_MemH64(strCur);
				strNewKey = strNewKey + "_" + "note(SceneName)";
				wstring wcs_tmp = gbk_to_utf16(strScene);
				strScene = utf16_to_utf8(wcs_tmp);
				if (bLast)
				{
					vecRef.push_back(vector<string>());
					vecRef.back().push_back(strNewKey);
					vecRef.back().push_back(strScene);
				} 
				else
				{
					vecRef.insert(vecRef.begin()+i,vector<string>());
					vecRef[i].push_back(strNewKey);
					vecRef[i].push_back(strScene);
				}
			}
			strCur = strQuest;
		}
	}
}

void CGermanLang::AddCoordinateToQuest(string& strFileName,vector<vector<string> >& vecRef)
{
	string strCur;
	bool bLast = false;
	for (int i=1;i<vecRef.size();++i)
	{
		if(bLast)
			break;
		string strKey = vecRef[i][0];
		string strQuest = GetQuestNameFromMulitKey(strKey);
		if (strCur == "")
		{
			strCur = strQuest;
		}
		if (strQuest != strCur || i == vecRef.size()-1)
		{
			if (i == vecRef.size()-1)
			{
				bLast = true;
			}
			string strCoord = GetQuestCoordinate(strFileName,strCur);
			if (strCoord != "")
			{
				string strNewKey = mbcs_to_MemH64(strCur);
				strNewKey = strNewKey + "_" + "note(Coordinate)";
				if (bLast)
				{
					vecRef.push_back(vector<string>());
					vecRef.back().push_back(strNewKey);
					vecRef.back().push_back(strCoord);
				}
				else
				{
					vecRef.insert(vecRef.begin()+i,vector<string>());
					vecRef[i].push_back(strNewKey);
					vecRef[i].push_back(strCoord);
				}
			}
			strCur = strQuest;
		}
	}
}

string CGermanLang::GetQuestSecen(string& strFileName,string& strQuest)
{
	string strResult;
	string strReal = strFileName;
	int pos = strReal.rfind("/");
	if (pos != -1)
	{
		strReal = strReal.substr(pos+1,strReal.length()-pos-1);
	}
	string strDifName = m_pSplitInfo->GetSrcDifNameWithLangName(strReal);
	string strPosDif = strDifName;
	replace_all_distinct(strPosDif,"designer","pos",0);

	COriFileOperate& oriFile = GetOriFile(strDifName);
	COriFileOperate& posFile = GetOriFile(strPosDif);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strTest = oriFile.GetDataByRowCol(i,0);
		string strFun = oriFile.GetDataByRowCol(i,1);
		if (strTest == strQuest && strFun == "触发任务方式")
		{
			if (oriFile.BeNumber(i,5))
			{
				string strPosIndex = oriFile.GetDataByRowCol(i,5);
				strResult = strPosIndex;
				break;
			}
			else
				return "";
		}
	}
	if (strResult == "")
		return "";
	for (int i=1;i<posFile.GetRowCount();++i)
	{
		if (posFile.BeNumber(i,0)&& strResult == posFile.GetDataByRowCol(i,0))
		{
			string strScene = posFile.GetDataByRowCol(i,1);
			return strScene;
		}
	}
	return "";
}

string CGermanLang::GetQuestCoordinate(string& strFileName,string& strQuest)
{
	string strResult;
	string strReal = strFileName;
	int pos = strReal.rfind("/");
	if (pos != -1)
	{
		strReal = strReal.substr(pos+1,strReal.length()-pos-1);
	}
	string strDifName = m_pSplitInfo->GetSrcDifNameWithLangName(strReal);
	string strPosDif = strDifName;
	replace_all_distinct(strPosDif,"designer","pos",0);

	COriFileOperate& oriFile = GetOriFile(strDifName);
	COriFileOperate& posFile = GetOriFile(strPosDif);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strTest = oriFile.GetDataByRowCol(i,0);
		string strFun = oriFile.GetDataByRowCol(i,1);
		if (strTest == strQuest && strFun == "触发任务方式")
		{
			if (oriFile.BeNumber(i,5))
			{
				string strPosIndex = oriFile.GetDataByRowCol(i,5);
				strResult = strPosIndex;
				break;
			}
			else
				return "";
		}
	}
	if (strResult == "")
		return "";
	for (int i=1;i<posFile.GetRowCount();++i)
	{
		if (posFile.BeNumber(i,0) && strResult == posFile.GetDataByRowCol(i,0))
		{
			string strX = posFile.GetDataByRowCol(i,2);
			string strY = posFile.GetDataByRowCol(i,3);
			string str = strX + "," + strY;
			return str;
		}
	}
	return "";
}

string CGermanLang::GetPatchName(string strName)
{
	if (m_bEnablePatch)
	{
		string strResult = strName;
		strResult = strResult + ".temp";
		return strResult;
	}
	return strName;
}


#pragma warning(pop)