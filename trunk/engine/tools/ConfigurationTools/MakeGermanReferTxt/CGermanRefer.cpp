#include "stdafx.h"
#include "CGermanRefer.h"
#include "CSplitInfo.h"
#include "CXmlConfig.inl"
#include "common.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

CGermanRefer::CGermanRefer()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	ReadReferneceTxt();
	ReadMappedXML();
	FileTreeWalk( gbk_to_utf16(m_pSplitInfo->GetSrcPath()).c_str(),MakeDifList, NULL );
	FileTreeWalk(gbk_to_utf16(m_pSplitInfo->GetCfgTempPath()).c_str(),MakeDifList,NULL);
}

FTW_RESULT CGermanRefer::MakeDifList( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CGermanRefer* pManager = CGermanRefer::GetInst();
	pManager->RecordDifPath(strFileName);
	return _FTW_CONTINUNE;
}

void CGermanRefer::RecordDifPath(string& str)
{
	string strTemp = str;
	int pos = strTemp.rfind("/");
	strTemp = strTemp.substr(pos+1,strTemp.length()-pos-1);
	m_mapFileList.insert(make_pair(strTemp,str));
}

FTW_RESULT CGermanRefer::CreateRefer( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CGermanRefer* pManager = CGermanRefer::GetInst();
	pManager->CombinationQuestRefer(strFileName);
	pManager->CombinationIntObjRefer(strFileName);
	return _FTW_CONTINUNE;
}

void CGermanRefer::CreateFileMapData(string& strFileName)
{
	if (!m_referData.count(strFileName))
	{
		m_referData[strFileName] = vector<vector<wstring> >();
		m_referData[strFileName].push_back(vector<wstring>());
		m_referData[strFileName].back().push_back(L"Project 1");
		m_referData[strFileName].back().push_back(L"Key 1");
		m_referData[strFileName].back().push_back(L"Project 2");
		m_referData[strFileName].back().push_back(L"Key 2");
		m_referData[strFileName].back().push_back(L"Mode of Referency");
		m_referData[strFileName].back().push_back(L"Reference Description");
	}
}

void CGermanRefer::WriteToGermanRefer()
{
	string strGermanFolder = CSplitInfo::GetInst()->GetTransExportPath();
	char charEnter[4] = {0x0D,0x00,0x0A,0x00};
	char charTab[2] = {0x09,0x00};
	char charHead[2] ;
	charHead[0] = uint8(15*16+15);
	charHead[1] = uint8(15*16+14);
	map<string,vector<vector<wstring> > >::iterator iter = m_referData.begin();
	for (;iter != m_referData.end();++iter)
	{
		vector<vector<wstring> >& vecRef = iter->second;
		string strFile = strGermanFolder + "/" + iter->first;
		CreateFileDirectory(strFile);
		ofstream outFile(strFile.c_str(),ios_base::binary | ios_base::out);
		if (!outFile)
		{
			string strError = strFile + "打开时发生错误";
			if(GetLogToFile())
			{
				PrintLog(strError);
			}
			else
			{
				MessageBox(0,strError.c_str(),"Error",NULL);
			}
			exit(0);
		}
		outFile.write(charHead,2);
		for (int i=0;i<vecRef.size();++i)
		{
			for (int j=0;j<vecRef[i].size();++j)
			{
				outFile.write((char*)vecRef[i][j].c_str(),vecRef[i][j].length()*2);
				if (j<vecRef[i].size()-1)
					outFile.write(charTab,2);
			}
			outFile.write(charEnter,4);
		}
		outFile.close();
	}
}

void CGermanRefer::CombinationQuestRefer(string strFileName)
{
	vector<string> vecName;
	SplitFileName(strFileName,vecName);
	if (vecName.size() != 2)
		return;
	if (!BeQuestLangFile(vecName[0]))
		return;
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strFileName,vecTemp);
	string strReferName = "QuestTwoRefer.txt";
	CreateFileMapData(strReferName);
	string projekt1 = GetProjekt(vecName[0]);
	string key1 = GetKeyPrefix(vecName[0]);
	string projekt2 = GetProjekt(vecName[1]);
	string key2 = GetKeyPrefix(vecName[1]);
	vector<vector<wstring> >& vecRef = m_referData[strReferName];
	wstring wprojekt1 = utf8_to_utf16(projekt1);
	wstring wprojekt2 = utf8_to_utf16(projekt2);
	wstring wTwoWay = L"two-way";
	for (int i=1;i<vecTemp.size();++i)
	{
		string index1 = vecTemp[i][0];
		ReplaceKeyword(index1);
		string index2 = vecTemp[i][1];
		string strDesc = vecTemp[i][2];
		strDesc = utf8_to_gbk(strDesc);
		strDesc = GetQuestDescription(strDesc);
		index1 = key1 + index1;
		index2 = key2 + index2;
		wstring wIndex1 = utf8_to_utf16(index1);
		wstring wIndex2 = utf8_to_utf16(index2);
		wstring wDesc = gbk_to_utf16(strDesc);
		vecRef.push_back(vector<wstring>());
		vecRef.back().push_back(wprojekt2);
		vecRef.back().push_back(wIndex2);
		vecRef.back().push_back(wprojekt1);
		vecRef.back().push_back(wIndex1);
		vecRef.back().push_back(wTwoWay);
		vecRef.back().push_back(wDesc);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanRefer::CombinationIntObjRefer(string strFileName)
{
	vector<string> vecName;
	SplitFileName(strFileName,vecName);
	string strLanIntObj = "Lan_IntObj_Common.txt";
	if (vecName.size() != 2 || vecName[0] != strLanIntObj && vecName[1] != strLanIntObj || BeQuestLangFile(vecName[0]) )
		return;
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strFileName,vecTemp);
	string strReferName = "ItemObjTwoRefer.txt";
	CreateFileMapData(strReferName);
	string projekt1 = GetProjekt(vecName[0]);
	string key1 = GetKeyPrefix(vecName[0]);
	string projekt2 = GetProjekt(vecName[1]);
	string key2 = GetKeyPrefix(vecName[1]);
	vector<vector<wstring> >& vecRef = m_referData[strReferName];
	wstring wprojekt1 = utf8_to_utf16(projekt1);
	wstring wprojekt2 = utf8_to_utf16(projekt2);
	wstring wTwoWay = L"two-way";
	string strDesc = "just_for_info";
	wstring wDesc = gbk_to_utf16(strDesc);
	for (int i=1;i<vecTemp.size();++i)
	{
		string index1 = vecTemp[i][0];
		string index2 = vecTemp[i][1];
		index1 = key1 + index1;
		index2 = key2 + index2;
		wstring wIndex1 = utf8_to_utf16(index1);
		wstring wIndex2 = utf8_to_utf16(index2);
		vecRef.push_back(vector<wstring>());
		vecRef.back().push_back(wprojekt1);
		vecRef.back().push_back(wIndex1);
		vecRef.back().push_back(wprojekt2);
		vecRef.back().push_back(wIndex2);
		vecRef.back().push_back(wTwoWay);
		vecRef.back().push_back(wDesc);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGermanRefer::CombinationQuestDesc(string strFileName)
{
	vector<string> vecName;
	SplitFileName(strFileName,vecName);
	if (vecName[0] != "Lan_QuestString.txt")
		return;
	string strSaveFile = "QuestStringRefer.txt";
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strFileName,vecTemp);
	CreateFileMapData(strSaveFile);
	string projekt1 = GetProjekt(vecName[0]);
	string key1 = GetKeyPrefix(vecName[0]);
	string projekt2 = GetProjekt(vecName[1]);
	string key2 = GetKeyPrefix(vecName[1]);
	vector<vector<wstring> >& vecRef = m_referData[strSaveFile];
	wstring wprojekt1 = utf8_to_utf16(projekt1);
	wstring wprojekt2 = utf8_to_utf16(projekt2);
	wstring wTwoWay = L"two-way";
	string strDesc = "just_for_info";
	wstring wDesc = gbk_to_utf16(strDesc);
	for (int i=1;i<vecTemp.size();++i)
	{
		string index1 = vecTemp[i][0];
		string index2 = vecTemp[i][1];
		index1 = key1 + index1;
		index2 = key2 + index2;
		wstring wIndex1 = utf8_to_utf16(index1);
		wstring wIndex2 = utf8_to_utf16(index2);
		vecRef.push_back(vector<wstring>());
		vecRef.back().push_back(wprojekt1);
		vecRef.back().push_back(wIndex1);
		vecRef.back().push_back(wprojekt2);
		vecRef.back().push_back(wIndex2);
		vecRef.back().push_back(wTwoWay);
		vecRef.back().push_back(wDesc);
	}
}

void CGermanRefer::ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef)
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

bool CGermanRefer::AdjustBeQuestTbl(string& strFileName)
{
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strSearch = "quest/" ;
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

bool CGermanRefer::BeQuestLangFile(string& strLang)
{
	string strDifName = m_pSplitInfo->GetSrcDifNameWithLangName(strLang);
	string strWholeName = m_mapFileList[strDifName];
	return AdjustBeQuestTbl(strWholeName);
}

string CGermanRefer::GetProjekt(string& strFileName)
{
	if (!m_mapReference.count(strFileName))
	{
		ofstream logFile("NoReferInfo.txt",ios_base::out | ios_base::app);
		logFile<<strFileName<<"无Projekt"<<endl;
		logFile.close();
		if(GetLogToFile())
		{
			stringstream strExpLog;
			strExpLog << strFileName << "无Projekt";
			PrintLog(strExpLog.str());
		}
		return "没有Projekt信息";
	}
	return m_mapReference[strFileName].m_strProjekt;
}

string CGermanRefer::GetKeyPrefix(string& strFileName)
{
	if (!m_mapReference.count(strFileName))
	{
		ofstream logFile("NoReferInfo.txt",ios_base::out | ios_base::app);
		logFile<<strFileName<<"无Keyprefix"<<endl;
		logFile.close();

		if(GetLogToFile())
		{
			stringstream strExpLog;
			strExpLog << strFileName << "无Keyprefix";
			PrintLog(strExpLog.str());
		}
		else
		{
			//MessageBox(0,"未到着索引","error",NULL);
		}
		return "没有Keyprefix信息";
	}
	return m_mapReference[strFileName].m_strPreFix;
}

void CGermanRefer::SplitFileName(string strFileName,vector<string>& vecResult)
{
	int rSlash = -1;
	rSlash = strFileName.rfind("/");
	if (rSlash != -1)
	{
		strFileName = strFileName.substr(rSlash+1,strFileName.length()-rSlash-1);
	}
	int pos = strFileName.find("#");
	if (pos != -1)
	{
		string strOne = strFileName.substr(0,pos);
		strOne = strOne + ".txt";
		string strTwo = strFileName.substr(pos+1,strFileName.length()-pos-1);
		vecResult.push_back(strOne);
		vecResult.push_back(strTwo);
	}
}

void CGermanRefer::ReadReferneceTxt()
{
	if (!m_mapReference.empty())
		return;
	const string& strName = CSplitInfo::GetInst()->GetProjectStructerFileName();//"Warrior King - Projectstructur.txt";
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strName,vecTemp);
	int colNum = vecTemp[0].size();
	vector<int> vecSIndex;
	vecSIndex.resize(3);
	vecSIndex[0] = find(vecTemp[0].begin(),vecTemp[0].end(),"File") - vecTemp[0].begin();
	vecSIndex[1] = find(vecTemp[0].begin(),vecTemp[0].end(),"Project") - vecTemp[0].begin();
	vecSIndex[2] = find(vecTemp[0].begin(),vecTemp[0].end(),"Keypr?fix") - vecTemp[0].begin();
	for (int i =1;i<vecTemp.size();++i)
	{
		string strFileName = vecTemp[i][vecSIndex[0]] + ".txt";
		string strProjekt = vecTemp[i][vecSIndex[1]];
		string strKeyPrefix = vecTemp[i][vecSIndex[2]];
		ReferenceInfor referData;
		referData.m_strProjekt = strProjekt;
		referData.m_strPreFix = strKeyPrefix;
		m_mapReference[strFileName] = referData;
	}
}

void CGermanRefer::ReadReferDescriptTxt()
{
	if (!m_mapReferDescript.empty())
		return;
	string strName("Reference Descriptions.txt");
	vector<vector<string> > vecTemp;
	ReadUtf8Txt(strName,vecTemp);
	for (int i=1;i<vecTemp.size();++i)
	{
		string strName = vecTemp[i][1];
		string number = vecTemp[i][0];
		m_mapReferDescript.insert(make_pair(strName,number));
	}
}

bool CGermanRefer::ReadMappedXML()
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

void CGermanRefer::ReplaceKeyword(string& strIndex)
{
	map<string,string>::iterator iter = m_mapName.begin();
	for (;iter != m_mapName.end();++iter)
	{
		string strName = iter->first;
		string strH64 = mbcs_to_MemH64(strName);
		if (strIndex.find(strH64) != -1)
		{
			string strDesc = iter->second;
			replace_all_distinct(strIndex,strH64,strDesc,0);
			return;
		}
	}
	string strError = strIndex + "出现没有关键词的错误";
	if(GetLogToFile())
	{
		PrintLog(strError);
	}
	else
	{
		MessageBox(0,strError.c_str(),"error",NULL);
	}
}

string CGermanRefer::GetQuestDescription(const string& str)
{
	string strResult;
	if (str.find("接取任务") != -1 || str.find("交任务") != -1)
	{
		if (str.find("NPC") != -1)
			strResult = "gives_takes_quest";
		else
			strResult = "quest_given_by_npc";
	}
	else if (str.find("物品需求") != -1)
	{
		strResult = "questitem";
	}
	else if (str.find("杀怪需求") != -1)
	{
		strResult = "hunt_monster_for_quest";
	}
	else if (str.find("使用交互对象") != -1)
	{
		strResult = "references";
	}
	else if (str.find("物品奖励") != -1)
	{
		strResult = "questreward";
	}
	else if (str.find("获得物品") != -1)
	{
		strResult = "questitem";
	}
	if (strResult == "")
	{
		string strError = str + "描述信息不存在，请检查";
		if(GetLogToFile())
		{
			PrintLog(strError);
		}
		else
		{
			MessageBox(0,strError.c_str(),"Error",NULL);
		}
	}
	return strResult;
}

#pragma warning(pop)