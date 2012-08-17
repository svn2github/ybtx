#include "stdafx.h"
#include "CSplitInfo.h"
#include "CXmlConfig.inl"
#include "common.h"
#include "CodeCvs.h"
#include "CRecordManager.h"

bool CSplitInfo::ExcepIndexData::CheckCondition(vector<map<string,string> >& vecAttribute)
{
	bool bResult = true;
	for (int i = 0;i<(int)vecAttribute.size();++i)
	{
		const string& strIndex = vecAttribute[0].begin()->first;
		const string& strContent  = vecAttribute[0].begin()->second;
		for(int j=0;j<(int)vecExcept.size();++j)
		{
			const string& strExceptIndex = vecExcept[j].begin()->first;
			if (strIndex == strExceptIndex)
			{
				string& strValue = vecExcept[j].begin()->second;
				if (strValue == "" || strValue == strContent)
					continue;
				else
					bResult = false;
			}
		}
	}
	return bResult;
}

string CSplitInfo::ExcepIndexData::ProduceIndex(vector<map<string,string> >& vecAttribute)
{
	string strResult;
	for(int i=0;i<(int)vecIndex.size();++i)
	{
		string& strCol = vecIndex[i];
		for (int j=0;j<(int)vecAttribute.size();++j)
		{
			if (vecAttribute[j].begin()->first == strCol)
			{
				string strTemp = vecAttribute[j].begin()->second;
				strTemp = mbcs_to_MemH64(strTemp);
				if (strResult == "")
					strResult = strTemp;
				else
					strResult = strResult + GetLangIndexConnSymbol() + strTemp;
			}
		}
	}
	return strResult;
}

vector<string> CSplitInfo::ExcepIndexData::GetExceptCol()
{
	vector<string> vecExpCol;
	for (int i=0;i<(int)vecExcept.size();++i)
	{
		vecExpCol.push_back(vecExcept[i].begin()->first);
	}
	return vecExpCol;
}

CSplitInfo::CSplitInfo()
{
	//ReadSplitXML();
	//if (m_bEnableInternational)
	//{
	//	CRecordManager::GetInst()->SetAllFileChange();
	//}
}

void CSplitInfo::Init(int argc, _TCHAR* argv[])
{
	string strCfg = argc > 1 ?  argv[1] : "WorkPath.xml";
	
	ReadSplitXML(strCfg);
	if (m_bEnableInternational)
	{
		CRecordManager::GetInst()->SetAllFileChange();
	}
}


bool CSplitInfo::ReadSplitXML(const string& strCfg)
{
	{
	ifstream config(strCfg.c_str());
	if(!config)
		return false;

	CXmlConfig *pXmlCfg = new CXmlConfig();
	pXmlCfg->LoadConfig( "root" , config );
	config.close();

		TiXmlNode* pWPathNode = pXmlCfg->Get<TiXmlElement*>("Work_Path");
	if( pWPathNode == NULL )
	{
		delete pXmlCfg;
		return false; 
	}
	TiXmlElement* pSrcElement = pWPathNode->FirstChild("Source")->ToElement();
	m_strSrcPath = pSrcElement->GetText();
	TiXmlElement* pTgtElement = pWPathNode->FirstChild("Destination")->ToElement();
	m_strTgtPath = pTgtElement->GetText();
	TiXmlElement* pCfgTempElement = pWPathNode->FirstChild("CfgTemp")->ToElement();
	m_strCfgTempPath = pCfgTempElement->GetText();

	size_t uPos = m_strSrcPath.rfind('/');
	if(uPos == string::npos)
	{
		m_strSrcFolder = m_strSrcPath;
	}
	else
	{
		++uPos;
		m_strSrcFolder = m_strSrcPath.substr(uPos, m_strSrcPath.size() - uPos);
	}

	uPos = m_strTgtPath.rfind('/');
	if(uPos == string::npos)
	{
		m_strTgtFolder = m_strTgtPath;
	}
	else
	{
		++uPos;
		m_strTgtFolder = m_strTgtPath.substr(uPos, m_strTgtPath.size() - uPos);
	}


	//转表造作的配置表后缀名这里读取
	TiXmlElement* pLuaCfgElement  = pWPathNode->FirstChild("LuaCfgExtend")->ToElement();
	m_strLuaExtend = pLuaCfgElement->GetText();
	TiXmlElement* pCppCfgElement  = pWPathNode->FirstChild("CppCfgExtend")->ToElement();
	m_strCppExend = pCppCfgElement->GetText();

	TiXmlElement* pInternational = pWPathNode->FirstChild("EnableInternational")->ToElement();
	string strBeInternat = pInternational->GetText();
	tolowerCase(strBeInternat);
	if (strBeInternat == "false")
	{
		m_bEnableInternational = false;
	}
	else
	{
		m_bEnableInternational = true;
	}

	TiXmlElement* pSplitBase= pWPathNode->FirstChild("BeSplitItemDesc")->ToElement();
	string strSplitBse = pSplitBase->GetText();
	tolowerCase(strSplitBse);
	if (strSplitBse == "false")
	{
		m_bSplitBaseItemDesc = false;
	}
	else
	{
		m_bSplitBaseItemDesc = true;
	}

	TiXmlElement* pLogToFile= pWPathNode->FirstChild("LogToFile")->ToElement();
	string strLogToFile = pLogToFile->GetText();
	tolowerCase(strLogToFile);
	if (strLogToFile == "false")
	{
		m_bLogToFile = false;
	}
	else
	{
		m_bLogToFile = true;
	}

	TiXmlElement* pNeedLuaFiles= pWPathNode->FirstChild("NeedLuaFiles")->ToElement();
	string strNeedLuaFiles = pNeedLuaFiles->GetText();
	tolowerCase(strNeedLuaFiles);
	if (strNeedLuaFiles == "false")
	{
		m_bNeedLuaFiles = false;
	}
	else
	{
		m_bNeedLuaFiles = true;
	}

	m_strLangSrcFolder = "langsrc";
	m_strReferFolder = "refer";

	TiXmlElement* pForeLangElement  = pWPathNode->FirstChild("TransForeignLang")->ToElement();
	m_strForeLang = pForeLangElement->GetText();

	TiXmlElement* pOriLangElement  = pWPathNode->FirstChild("TransOriginalLang")->ToElement();
	m_strOriLang = pOriLangElement->GetText();
	
	TiXmlElement* pAutoConvLangElement  = pWPathNode->FirstChild("AutoConvLang")->ToElement();
	m_strAutoConvLang = pAutoConvLangElement->GetText();

	TiXmlElement* pLangElement  = pWPathNode->FirstChild("LanguagePath")->ToElement();
	m_strLangPath = pLangElement->GetText();
	
	m_strLangTablePath = m_strLangPath + "/" + GetOriLang() + "/table";

	TiXmlElement* pLanguageTempPathElement  = pWPathNode->FirstChild("LanguageTempPath")->ToElement();
	m_strLanguageTempPath = pLanguageTempPathElement->GetText();
	
	m_strLangSrcPath = GetLanguageTempPath() + "/" + GetLangSrcFolder();
	m_strReferPath = GetLanguageTempPath() + "/" + GetReferFolder();

	TiXmlElement* pSceneElement = pWPathNode->FirstChild("SceneFolder")->ToElement();
	m_strScenePath = pSceneElement->GetText();
	m_strScenePath = GetTgtPath() + "/" + m_strScenePath;

	TiXmlElement* pGuiFolderElement = pWPathNode->FirstChild("GuiFolder")->ToElement();
	m_strGuiFolder = pGuiFolderElement->GetText();

	TiXmlElement* pQuestElement = pWPathNode->FirstChild("QuestFolder")->ToElement();
	m_strQuestFolder = pQuestElement->GetText();

	m_strQuestPath = GetSrcPath() + "/" + GetQuestFolder();
	m_strQuestTempPath = GetCfgTempPath()+"/"+GetQuestFolder();

	TiXmlElement* pItemElement = pWPathNode->FirstChild("ItemFolder")->ToElement();
	m_strItemFolder = pItemElement->GetText();

	m_strItemPath = GetSrcPath() + "/" + GetItemFolder();
	m_strItemTempPath = GetCfgTempPath() + "/" + GetItemFolder();

	TiXmlElement* pTransForeignPathElement= pWPathNode->FirstChild("TransForeignPath")->ToElement();
	m_strTransForeignPath = pTransForeignPathElement->GetText();

	m_strTransForeignPath = m_strTransForeignPath + "/" + GetForeLang();

	TiXmlElement* pTransChinesePathElement= pWPathNode->FirstChild("TransChinesePath")->ToElement();
	m_strTransChinesePath = pTransChinesePathElement->GetText();

	size_t pos = m_strTransChinesePath.rfind('/');
	if(pos == string::npos)
	{
		m_sTransExportFolder = m_strTransChinesePath;
	}
	else
	{
		m_sTransExportFolder = m_strTransChinesePath.substr(pos + 1, m_strTransChinesePath.size() - pos - 1);
	}

	m_strTransChinesePath = m_strTransChinesePath + "/" + GetForeLang();

	TiXmlElement* pProjectStructerFileNameElement= pWPathNode->FirstChild("ProjectStructerFileName")->ToElement();
	m_strProjectStructerFileName = pProjectStructerFileNameElement->GetText();

	TiXmlElement* pm_strGuiStaticTextElement= pWPathNode->FirstChild("GUIStaticText")->ToElement();
	m_strGuiStaticText = pm_strGuiStaticTextElement->GetText();

	delete pXmlCfg;
	pXmlCfg = NULL;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	{
		string strCfg = "File_KeepBlankLine.xml";
		ifstream config(strCfg.c_str());
		if(!config)
			return false;

		CXmlConfig *pXmlCfg = new CXmlConfig();
		pXmlCfg->LoadConfig( "root" , config );
		config.close();
		TiXmlNode* pFileKeepBlankLine = pXmlCfg->Get<TiXmlElement*>("File_KeepBlankLine");
		if( pFileKeepBlankLine == NULL )
		{
			delete pFileKeepBlankLine;
			return false; 
		}
		for(TiXmlNode* pFileName = pFileKeepBlankLine->FirstChild("FileName");pFileName;pFileName = pFileName->NextSibling("FileName"))
		{
			TiXmlElement* pNameElement = pFileName->ToElement();
			m_setFileKeepBlankLine.insert(pNameElement->Attribute("Name"));
		}
		delete pXmlCfg;
		pXmlCfg = NULL;

	}
//	if(strCfg=="CPPReadTable.xml")
	{
		string strCfg = "CPPReadTable.xml";
		ifstream config(strCfg.c_str());
		if(!config)
			return false;

		CXmlConfig *pXmlCfg = new CXmlConfig();
		pXmlCfg->LoadConfig( "root" , config );
		config.close();

		TiXmlNode* pCReadTable = pXmlCfg->Get<TiXmlElement*>("CReadTable");
		if( pCReadTable == NULL )
		{
			delete pCReadTable;
			return false; 
		}
		for(TiXmlNode* pFileName = pCReadTable->FirstChild("FileName");pFileName;pFileName = pFileName->NextSibling("FileName"))
		{
			TiXmlElement* pNameElement = pFileName->ToElement();
			m_setCReadTable.insert(pNameElement->Attribute("Name"));
		}
		delete pXmlCfg;
		pXmlCfg = NULL;
	}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	if(strCfg=="FileTeam.xml")
	{
		string strCfg = "FileTeam.xml";
		ifstream config(strCfg.c_str());
		if(!config)
			return false;

		CXmlConfig *pXmlCfg = new CXmlConfig();
		pXmlCfg->LoadConfig( "root" , config );
		config.close();

		TiXmlNode* pFileTeam = pXmlCfg->Get<TiXmlElement*>("File_Team");
		if( pFileTeam == NULL )
		{
			delete pFileTeam;
			return false; 
		}
		for(TiXmlNode* pFileSection = pFileTeam->FirstChild("File_Section");pFileSection;pFileSection = pFileSection->NextSibling("File_Section"))
		{
			TiXmlElement* pNameElement = pFileSection->FirstChild("FileName")->ToElement();
			string strFileName = pNameElement->Attribute("Name");
			pNameElement = pFileSection->FirstChild("AppFileName")->ToElement();
			string strAppName = pNameElement->Attribute("Name");
			pNameElement = pFileSection->FirstChild("LangFileName")->ToElement();
			string strLangName = pNameElement->Attribute("Name");
			pNameElement = pFileSection->FirstChild("AppSavePath")->ToElement();
			string strAppPath = pNameElement->Attribute("Path");
			pNameElement = pFileSection->FirstChild("LangSavePath")->ToElement();
			string strLangPath = pNameElement->Attribute("Path");
			m_mapFileSaveInfo[strFileName] = map<ESaveInfor,string>();
			m_mapFileSaveInfo[strFileName][eSIF_AppName] = strAppName;
			m_mapFileSaveInfo[strFileName][eSIF_LangName] = strLangName;
			m_mapFileSaveInfo[strFileName][eSIF_AppSavePath] = strAppPath;
			m_mapFileSaveInfo[strFileName][eSIF_LangSavePath] = strLangPath;
			TiXmlNode* pIndexNode = pFileSection->FirstChild("Index_Content");
			for (TiXmlNode *pCommon = pIndexNode->FirstChild("CommonIndex");pCommon;pCommon = pCommon->NextSibling("CommonIndex"))
			{
				TiXmlElement* pComElement = pCommon->ToElement();
				string strTempIndex = pComElement->Attribute("Name");
				if (m_mapMainIndex.count(strFileName)==0)
				{
					m_mapMainIndex[strFileName] = map<ESplitInfor,vector<string> >();
				}
				if (m_mapMainIndex[strFileName].count(eSTIF_CommonIndex)==0)
				{
					m_mapMainIndex[strFileName][eSTIF_CommonIndex] = vector<string>();
				}
				m_mapMainIndex[strFileName][eSTIF_CommonIndex].push_back(strTempIndex);
			}
			for(TiXmlNode* pNoteNode = pIndexNode->FirstChild("NoteIndex");pNoteNode;pNoteNode = pNoteNode->NextSibling("NoteIndex"))
			{
				TiXmlElement* pNoteElement = pNoteNode->ToElement();
				string strScolName = pNoteElement->Attribute("Name");
				string strDisColName = pNoteElement->Attribute("DisplayName");
				if (m_mapNoteInfo.count(strFileName)==0)
				{
					m_mapNoteInfo[strFileName] = map<string,string>();
				}
				m_mapNoteInfo[strFileName].insert(make_pair(strScolName,strDisColName));
			}
			TiXmlNode* pDestIndexNode = pIndexNode->FirstChild("LangIndex");
			string strlangIndex = pDestIndexNode->ToElement()->Attribute("Name");
			if (m_mapMainIndex[strFileName].count(eSTIF_LangIndex)==0)
			{
				m_mapMainIndex[strFileName][eSTIF_LangIndex] = vector<string>();
			}
			m_mapMainIndex[strFileName][eSTIF_LangIndex].push_back(strlangIndex);
			TiXmlNode* pContent = pIndexNode->FirstChild("Content");
			for (TiXmlNode* pConIndex = pContent->FirstChild("Content_Index");pConIndex;pConIndex = pConIndex->NextSibling("Content_Index"))
			{
				TiXmlElement* pConElement = pConIndex->ToElement();
				string strContent = pConElement->Attribute("Name");
				string strDestContent = pConElement->Attribute("LangName");
				string strDelete = pConElement->Attribute("Delete");
				if (m_mapMainIndex[strFileName].count(eSTIF_Content)==0)
				{
					m_mapMainIndex[strFileName][eSTIF_Content] = vector<string>();
				}
				m_mapMainIndex[strFileName][eSTIF_Content].push_back(strContent);
				if (m_mapMainIndex[strFileName].count(eSTIF_LangColName)==0)
				{
					m_mapMainIndex[strFileName][eSTIF_LangColName] = vector<string>();
				}	
				m_mapMainIndex[strFileName][eSTIF_LangColName].push_back(strDestContent);
				if (m_mapMainIndex[strFileName].count(eSTIF_Delete)==0)
				{
					m_mapMainIndex[strFileName][eSTIF_Delete] = vector<string>();
				}
				m_mapMainIndex[strFileName][eSTIF_Delete].push_back(strDelete);
			}	
			for (TiXmlNode* pExcept = pIndexNode->FirstChild("Exception");pExcept;pExcept = pExcept->NextSibling("Exception"))
			{
				ExcepIndexData exceptData;
				for (TiXmlNode* pExceptIndex = pExcept->FirstChild("IndexName");pExceptIndex;pExceptIndex = pExceptIndex->NextSibling("IndexName"))
				{
					TiXmlElement* pIndexName = pExceptIndex->ToElement();
					string strExceptIndex = pIndexName->Attribute("Name");
					string strExceptValue = pIndexName->Attribute("Value");
					int size = exceptData.vecExcept.size();
					exceptData.vecExcept.push_back(map<string,string>());
					exceptData.vecExcept[size].insert(make_pair(strExceptIndex,strExceptValue));
				}
				TiXmlNode* pMainIndex = pExcept->FirstChild("MainIndex");
				for (TiXmlNode* pSpecial_Index = pMainIndex->FirstChild("Special_Index");pSpecial_Index;pSpecial_Index = pSpecial_Index->NextSibling("Special_Index"))
				{
					TiXmlElement* pSIndexElement = pSpecial_Index->ToElement();
					string strSIndex = pSIndexElement->Attribute("Name");
					exceptData.vecIndex.push_back(strSIndex);
				}
				if (m_mapExceptInfo.count(strFileName)==0)
				{
					m_mapExceptInfo[strFileName] = vector<ExcepIndexData>();
				}
				m_mapExceptInfo[strFileName].push_back(exceptData);
			}	
		}
		delete pXmlCfg;
		pXmlCfg = NULL;
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//	delete pXmlCfg;
//	pXmlCfg = NULL;

	return true;
}

bool CSplitInfo::BeInSplitList(string strName)
{
	string strReal = strName;
	int pos = strReal.rfind("/");
	if (pos != -1)
	{
		strReal = strReal.substr(pos+1,strReal.length()-pos-1);
	}
	if (m_mapFileSaveInfo.count(strReal))
		return true;
	else
		return false;
}

bool CSplitInfo::BeLangDif(string strName)
{
	string strReal = strName;
	int pos = strName.rfind("/");
	if (pos != -1)
	{
		strReal = strName.substr(pos+1,strName.length()-pos-1);
	}
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	replace_all_distinct(strReal,strLuaCfgExtend,".txt",0);
	map<string,map<ESaveInfor,string> >::iterator iter = m_mapFileSaveInfo.begin();
	for (;iter != m_mapFileSaveInfo.end();++iter)
	{
		string strLang = iter->second[eSIF_LangName];
		if (strLang == strReal)
		{
			return true;
		}
	}
	return false;
}
bool CSplitInfo::BeAppDif(string strName)
{
	string strReal = strName;
	int pos = strName.rfind("/");
	if (pos != -1)
	{
		strReal = strName.substr(pos+1,strName.length()-pos-1);
	}
	map<string,map<ESaveInfor,string> >::iterator iter = m_mapFileSaveInfo.begin();
	for (;iter != m_mapFileSaveInfo.end();++iter)
	{
		string strApp = iter->second[eSIF_AppName];
		if (strApp == strReal)
		{
			return true;
		}
	}
	return false;
}
bool CSplitInfo::BeMakeAppFile(string strName)
{
	string strAppName = m_mapFileSaveInfo[strName][eSIF_AppName];
	if (strAppName == "")
		return false;
	else
		return true;
}

string CSplitInfo::GetAppSavePath(const string& strFileName)
{
	string strPath = m_mapFileSaveInfo[strFileName][eSIF_AppSavePath];
	return strPath;
}

string CSplitInfo::GetLangSavePath(const string& strFileName)
{
	string strPath = m_mapFileSaveInfo[strFileName][eSIF_LangSavePath];
	return strPath;
}

string CSplitInfo::GetAppName(const string& strFileName)
{
	string strName = m_mapFileSaveInfo[strFileName][eSIF_AppName];
	return strName;
}

string CSplitInfo::GetLangName(const string& strFileName)
{
	string strName = m_mapFileSaveInfo[strFileName][eSIF_LangName];
	return strName;
}

void CSplitInfo::GetSplitInfoArray(const string& strFileName,ESplitInfor eSInfo,vector<string> & vecArg)
{
	if (m_mapMainIndex[strFileName].count(eSInfo) == 0)
		return;
	vecArg = m_mapMainIndex[strFileName][eSInfo];
}

void CSplitInfo::GetSourceTransCol(const string& strFileName,vector<string> & vecArg)
{
	vecArg = m_mapMainIndex[strFileName][eSTIF_Content];
}

void CSplitInfo::GetDestTransCol(const string& strFileName,vector<string> & vecArg)
{
	vecArg = m_mapMainIndex[strFileName][eSTIF_LangColName];
}

void CSplitInfo::GetCommonIndex(const string& strFileName,vector<string> & vecArg)
{
	vecArg = m_mapMainIndex[strFileName][eSTIF_CommonIndex];
}

bool CSplitInfo::BeHaveExceptInfo(const string& strFileName)
{
	if(m_mapExceptInfo.count(strFileName))
		return true;
	else
		return false;
}

void CSplitInfo::CreateExceptMap(ExcepIndexData& referData,vector<string>& vecEleRef,map<int,string>& mapIndexName,vector<map<string,string> >& vecAttribute)
{
	vecAttribute.clear();
	vector<string> vecEName = referData.GetExceptCol();
	for (map<int,string>::iterator iter = mapIndexName.begin();iter != mapIndexName.end();++iter)
	{
		string strColName = iter->second;
		if (find(vecEName.begin(),vecEName.end(),strColName) != vecEName.end())
		{
			string strContent = vecEleRef[iter->first];
			vecAttribute.push_back(map<string,string>());
			vecAttribute[vecAttribute.size()-1].insert(make_pair(strColName,strContent));
		}
	}
}

void CSplitInfo::CreateExcepIndexAttribute(ExcepIndexData& referData,vector<string>& vecEleRef,map<int,string>& mapIndexName,vector<map<string,string> >& vecAttribute)
{
	vecAttribute.clear();
	vector<string> vecEName = referData.vecIndex;
	for (int i=0;i<(int)vecEName.size();++i)
	{
		string strColName = vecEName[i];
		for (map<int,string>::const_iterator iter = mapIndexName.begin();iter!=mapIndexName.end();++iter)
		{
			if (strColName == iter->second)
			{
				string strContent = vecEleRef[iter->first];
				vecAttribute.push_back(map<string,string>());
				vecAttribute[vecAttribute.size()-1].insert(make_pair(strColName,strContent));
				break;
			}
		}
	}
}

bool CSplitInfo::CreateExceptIndex(const string& strFileName,vector<string>& vecEleRef,map<int,string>& mapIndexName,string& element)
{
	vector<ExcepIndexData> vecExcept = m_mapExceptInfo[strFileName];
	vector<map<string,string> > vecAttribute;
	for (int j=0;j<(int)vecExcept.size();++j)
	{
		ExcepIndexData& refData = vecExcept[j];
		CreateExceptMap(refData,vecEleRef,mapIndexName,vecAttribute);
		if (refData.CheckCondition(vecAttribute))
		{
			vector<map<string,string> > vecExAttribute;
			CreateExcepIndexAttribute(refData,vecEleRef,mapIndexName,vecExAttribute);
			element = refData.ProduceIndex(vecExAttribute);
			return true;
		}
	}
	return false;
}

bool CSplitInfo::BeDeleteCol(const string& strFileName,const string& strColName)
{
	vector<string>& vecDelete = m_mapMainIndex[strFileName][eSTIF_Delete];
	vector<string>& vecContent = m_mapMainIndex[strFileName][eSTIF_Content];
	int pos = find(vecContent.begin(),vecContent.end(),strColName) - vecContent.begin();
	string strTemp = vecDelete[pos];
	char* pChar = const_cast<char*>(strTemp.c_str());
	strTemp = _strlwr(pChar);
	if (strTemp == "yes")
		return true;
	else
		return false;
}

string CSplitInfo::GetLangIndexName(const string& strFileName)
{
	return m_mapMainIndex[strFileName][eSTIF_LangIndex][0];
}

string CSplitInfo::GetSrcDifNameWithLangName(string strLangName)
{
	map<string,map<ESaveInfor,string> >::iterator iter = m_mapFileSaveInfo.begin();
	for (;iter != m_mapFileSaveInfo.end();++iter)
	{
		if (strLangName == iter->second[eSIF_LangName])
		{
			return iter->first;
		}
	}
	string strMsg = strLangName + "未找到原始文件";
	MessageBox(NULL,strMsg.c_str(),"错误",0);
	exit(0);
}

string CSplitInfo::GetDifColNameWithLangName(string strLangName,string lanCol)
{
	string strDif = GetSrcDifNameWithLangName(strLangName);
	vector<string>& vecRef = m_mapMainIndex[strDif][eSTIF_LangColName];
	int pos = -1;
	for (int i=0;i<(int)vecRef.size();++i)
	{
		if (lanCol == vecRef[i])
		{
			pos = i;
		}
	}
	return m_mapMainIndex[strDif][eSTIF_Content][pos];
}

void CSplitInfo::tolowerCase(string& str)
{
	transform(str.begin(),str.end(),str.begin(),tolower);
}

string CSplitInfo::GetLangIndexConnSymbol()
{
	return "_";
}


bool CSplitInfo::GetBeKeepBlankLine(const string& strFileName)
{
	return m_setFileKeepBlankLine.find(strFileName) != m_setFileKeepBlankLine.end();
}
bool CSplitInfo::GetCReadTable(const string& strFileName)
{
	return m_setCReadTable.find(strFileName) != m_setCReadTable.end();
}

