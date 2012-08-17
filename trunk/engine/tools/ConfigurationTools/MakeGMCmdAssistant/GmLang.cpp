#include "stdafx.h"
#include "GmLang.h"
#include "CXmlConfig.inl"
#include "ConsoleInfo.h"
#include "CRecordManager.h"
#include "common.h"
#include "CSplitInfo.h"
#include "CExpLog.h"

#define CHUNKNAME ""

CGmLang::CGmLang()
{
	m_bXmlRight = ReadGmConflictXml();
	m_pOldFile = NULL;
	m_bNeedSave = false;
	if (m_bXmlRight && !BeGmChange(m_strGmPath))
	{
		m_pOldFile = new COriFileOperate(m_strGmPath);
	}
	CRecordManager::SetChunkName(CHUNKNAME);
}

CGmLang::~CGmLang()
{
	typedef hash_map<string, hash_map<EGmInfo, char*> >::iterator IterHSH;
	typedef hash_map<EGmInfo, char*>::iterator IterHEC;
	for (IterHSH iterHsh=m_hmapGmInfo.begin(); iterHsh!=m_hmapGmInfo.end(); ++iterHsh)
	{
		for (IterHEC iter=iterHsh->second.begin(); iter!=iterHsh->second.end(); ++iter)
		{
			delete[] iter->second;
			iter->second = NULL;
		}
	}
	hash_map<string,COriFileOperate*>::iterator iterEL = m_hmapElementCol.begin();
	for (;iterEL != m_hmapElementCol.end();++iterEL)
	{
		delete iterEL->second;
		iterEL->second = NULL;
	}
	if (m_pOldFile)
		delete m_pOldFile;
}

void CGmLang::GmLang()
{	
	unsigned long uStartTime = GetCurrentTime();
	printf("[%d]MakeGMCmdAssistant工具处理开始\n", uStartTime);
	
	CGmLang* pGmLangInst = CGmLang::GetInst();
	if (!pGmLangInst->m_bXmlRight)
		return;
	string strPrint = CSplitInfo::GetInst()->GetLuaCfgExtend();
	strPrint = "designer_MorePointOne_Common" + strPrint;
	CConsoleInfo::GetInst()->PrintFunction(strPrint);
	CConsoleInfo::GetInst()->BeginToPrint("正在进行......");
	pGmLangInst->InitMapGmElementInfo();
	hash_map<string, hash_map<EGmInfo, char*> >::iterator iterHmap;

	printf("\n[%d]第一步文件遍历开始\n", GetCurrentTime());
	for (iterHmap = pGmLangInst->m_hmapGmInfo.begin(); iterHmap != pGmLangInst->m_hmapGmInfo.end(); ++iterHmap)
	{
		string strFileName = iterHmap->first;
		char* szFilePath = (iterHmap->second)[eGIF_Path];
		char* szDisplayColName = (iterHmap->second)[eGIF_DisplayCol];
		strFileName = pGmLangInst->m_strSrcPath + "/" + szFilePath + "/" + strFileName;
		pGmLangInst->ParseElementCol(szDisplayColName, strFileName);
	}
	printf("[%d]第二遍文件遍历开始\n", GetCurrentTime());
	typedef hash_map<string, GmGroupInfo>::iterator IterHmapSGm;
	typedef vector<hash_map<string, string> >::iterator IterVecHmap;
	typedef hash_map<string, string>::iterator IterHmapSS;
	for (IterHmapSGm iterGm = pGmLangInst->m_hmapGmGroupInfo.begin(); iterGm != pGmLangInst->m_hmapGmGroupInfo.end(); ++iterGm)
	{
		string strGroupName = iterGm->first;
		GmGroupInfo& gmGroupInfo = iterGm->second;
		for (IterVecHmap iterVec = gmGroupInfo.m_vecFileInfo.begin(); iterVec != gmGroupInfo.m_vecFileInfo.end(); ++iterVec)
		{
			for (IterHmapSS iterHmap = iterVec->begin(); iterHmap != iterVec->end(); ++iterHmap)
			{
				string strFileName = iterHmap->first;
				string strFilePath = iterHmap->second;
				strFileName = pGmLangInst->m_strSrcPath + strFilePath + "/" + strFileName;
				pGmLangInst->MergeElementCol(strGroupName, strFileName);
			}
		}
		pGmLangInst->ParseMultiElementCol(strGroupName);
	}

	printf("[%d]保存文件开始\n", GetCurrentTime());
	pGmLangInst->CopyNewData_toOld();
	if (pGmLangInst->m_pOldFile)
	{
		SQR_TRY
		if(pGmLangInst->m_bNeedSave)
			pGmLangInst->m_pOldFile->SaveFile();
		SQR_CATCH(exp)
		{
			string strMsg = exp.ErrorTitle();
			int rowCount = pGmLangInst->m_pOldFile->GetRowCount();
			char num[10];
			_itoa(rowCount,num,10);
			strMsg = strMsg + "老表行数=" + num;
			pGmLangInst->PrintLogInfo(strMsg);
			if(GetLogToFile())
			{
				PrintLog(strMsg);
			}
			else
			{
				//MessageBox(0,strMsg.c_str(),"错误",NULL);
			}
		}
		SQR_TRY_END
	}
	else
	{
		SQR_TRY
		pGmLangInst->m_destFile.SetName(pGmLangInst->m_strGmPath);
		pGmLangInst->m_destFile.SaveFile();
		SQR_CATCH(exp)
		{
			string strMsg = exp.ErrorTitle();
			int rowCount = pGmLangInst->m_destFile.GetRowCount();
			char num[10];
			_itoa(rowCount,num,10);
			strMsg = strMsg + "新表行数=" + num;
			pGmLangInst->PrintLogInfo(strMsg);
			if(GetLogToFile())
			{
				PrintLog(strMsg);
			}
			else
			{
				//MessageBox(0,strMsg.c_str(),"错误",NULL);
			}
		}
		SQR_TRY_END
	}
	printf("[%d]MakeGMCmdAssistant工具处理结束。消耗时间%d毫秒\n", GetCurrentTime(), GetCurrentTime() - uStartTime);
	CConsoleInfo::GetInst()->EndToPrint();
}

void CGmLang::PrintLogInfo(string strMsg)
{
	string strFile = "GMlog.txt";
	ofstream outFile(strFile.c_str());
	if (!outFile)
		return;
	outFile<<strMsg<<endl;
	for (int i=0;i<(int)m_vecFileChange.size();++i)
	{
		outFile<<m_vecFileChange[i]<<endl;
	}
	outFile.close();
	exit(1);
}

void CGmLang::InitMapGmElementInfo()
{
	string strName = "Name";
	string strDisPlay = "DisplayNameForGM";
	string strThird = "";
	m_destFile.AddData_toTab(strName,0,0);
	m_destFile.AddData_toTab(strDisPlay,0,1);
	m_destFile.AddData_toTab(strThird,0,2);
}

bool CGmLang::ReadGmConflictXml()
{
	if (!m_hmapGmInfo.empty())
	{
		return true;
	}

	ifstream ifsGmConflict("GmConflictTable.xml");
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

	m_strGmPath = pXmlNodeWorkPath->FirstChild("GmFilePath")->ToElement()->GetText();
	m_strSrcPath = pXmlNodeWorkPath->FirstChild("SrcPath")->ToElement()->GetText();

	TiXmlNode* pXmlNodeFileTeam = pXmlCfg->Get<TiXmlElement*>("File_Team");
	if (!pXmlNodeFileTeam)
	{
		return false;
	}

	for (TiXmlNode* pXmlNodeFileSection = pXmlNodeFileTeam->FirstChild("File_Section"); pXmlNodeFileSection; pXmlNodeFileSection = pXmlNodeFileSection->NextSibling("File_Section"))
	{
		TiXmlElement* pXmlElementFileName = pXmlNodeFileSection->FirstChild("FileName")->ToElement();
		const char* szFileName = pXmlElementFileName->Attribute("Name");
		if (m_hmapGmInfo.find(szFileName) == m_hmapGmInfo.end())
		{
			m_hmapGmInfo.insert(make_pair(szFileName, hash_map<EGmInfo, char*>()));
		}

		TiXmlElement* pXmlElementFilePath = pXmlNodeFileSection->FirstChild("FilePath")->ToElement();
		const char* szFilePath = pXmlElementFilePath->Attribute("Path");
		m_hmapGmInfo[szFileName][eGIF_Path] = new char[strlen(szFilePath) + 1];
		strcpy(m_hmapGmInfo[szFileName][eGIF_Path], szFilePath);

		TiXmlElement* pXmlElementDisplayCol = pXmlNodeFileSection->FirstChild("DisplayCol")->ToElement();
		const char* szDisplayCol = pXmlElementDisplayCol->Attribute("Name");
		m_hmapGmInfo[szFileName][eGIF_DisplayCol] = new char[strlen(szDisplayCol) + 1];
		strcpy(m_hmapGmInfo[szFileName][eGIF_DisplayCol], szDisplayCol);
	}

	TiXmlNode* pXmlNodeGroupTeam = pXmlCfg->Get<TiXmlElement*>("Group_Team");
	if (!pXmlNodeGroupTeam)
	{
		return false;
	}
	for (TiXmlNode* pXmlNodeGroup=pXmlNodeGroupTeam->FirstChild("Group"); pXmlNodeGroup; pXmlNodeGroup=pXmlNodeGroup->NextSibling("Group"))
	{
		TiXmlElement* pXmlElementGroup = pXmlNodeGroup->ToElement();
		string strGroupName = pXmlElementGroup->Attribute("Name");
		GmGroupInfo groupInfo;
		for (TiXmlNode* pXmlNodeFile=pXmlNodeGroup->FirstChild("File"); pXmlNodeFile; pXmlNodeFile=pXmlNodeFile->NextSibling("File"))
		{
			TiXmlElement* pXmlElementFile = pXmlNodeFile->ToElement();
			string strFileName = pXmlElementFile->Attribute("Name");
			string strFilePath = pXmlElementFile->Attribute("Path");
			groupInfo.m_vecFileInfo.push_back(hash_map<string, string>());
			int nSize = groupInfo.m_vecFileInfo.size();
			groupInfo.m_vecFileInfo[nSize - 1].insert(make_pair(strFileName, strFilePath));
		}
		for (TiXmlNode* pXmlNodeIndex=pXmlNodeGroup->FirstChild("Index"); pXmlNodeIndex; pXmlNodeIndex=pXmlNodeIndex->NextSibling("Index"))
		{
			TiXmlElement* pXmlElementIndex = pXmlNodeIndex->ToElement();
			string strIndexName = pXmlElementIndex->Attribute("Name");
			string strIndexValue = pXmlElementIndex->Attribute("Value");
			groupInfo.m_vecIndexInfo.push_back(hash_map<string, string>());
			int nSize = groupInfo.m_vecIndexInfo.size();
			groupInfo.m_vecIndexInfo[nSize - 1].insert(make_pair(strIndexName, strIndexValue));
		}
		TiXmlNode* pXmlNodeDisplayCol = pXmlNodeGroup->FirstChild("DisplayCol");
		TiXmlElement* pXmlElementDisplayCol = pXmlNodeDisplayCol->ToElement();
		string strDisplayColName = pXmlElementDisplayCol->Attribute("Name");
		groupInfo.m_strDisplayCol = strDisplayColName;
		m_hmapGmGroupInfo.insert(make_pair(strGroupName, groupInfo));
	}
	return true;
}

void CGmLang::ParseElementCol(const string& strDisplayColName, string& strFile)
{
	if (!BeGmChange(strFile) && m_pOldFile)
		return;
	m_vecFileChange.push_back(strFile);
	hash_map<string, RowInfo> mapGmDisplayCol;
	string strColName = "";
	COriFileOperate oriFile(strFile);
	oriFile.CleanNoteRow();
	int nRowNum = oriFile.GetRowCount();
	int nColNum = oriFile.GetColCountByRow(0);
	int nDisplayColNo = -1;
	string strBeautifulPath = FormatFileFullPath(strFile);
	for (int nCol = 0; nCol < nColNum; nCol++)
	{
		strColName = oriFile.GetDataByRowCol(0,nCol);
		if (strColName == strDisplayColName)
		{
			nDisplayColNo = nCol;
			break;
		}
	}
	if (nDisplayColNo == -1)
	{
		return;
	}

	string strColValue = "";
	for (int nRow = 1; nRow <= nRowNum - 1; nRow++)
	{
		strColValue = oriFile.GetDataByRowCol(nRow,nDisplayColNo);
		//---------------------------------临时添加，NPC表暂时title可能为空
		if (strColValue == "")
		{
			continue;
		}
		//-----------------------------------
		if (mapGmDisplayCol.find(strColValue) == mapGmDisplayCol.end())
		{
			mapGmDisplayCol[strColValue] = RowInfo();
			mapGmDisplayCol[strColValue].nRowOffset = nRow;
			mapGmDisplayCol[strColValue].bFlag = false;
		}
		else
		{
			if (!mapGmDisplayCol[strColValue].bFlag)
			{
				int nRow1 = mapGmDisplayCol[strColValue].nRowOffset;
				string strIndex = oriFile.GetDataByRowCol(nRow1,0);
				AddNewData_toGm(strIndex,strBeautifulPath,strColValue);
				mapGmDisplayCol[strColValue].bFlag = true;
			}
			string strIndex = oriFile.GetDataByRowCol(nRow,0);
			AddNewData_toGm(strIndex,strBeautifulPath,strColValue);
		}
	}
}

void CGmLang::ParseMultiElementCol( const string& strGroupName )
{
	if (!m_hmapElementCol.count(strGroupName))
		return;
	COriFileOperate& eleFile = *m_hmapElementCol[strGroupName];
	string strBeautifulPath = FormatFileFullPath(strGroupName);
	GmGroupInfo& gmGroupInfo = m_hmapGmGroupInfo[strGroupName];
	vector<string>& vecStr = gmGroupInfo.GetIndexCol();
	string strDisplayColName = gmGroupInfo.m_strDisplayCol;
	int nDisplayCol = -1;
	int nRowNum = (int)eleFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum > 0)
	{
		nColNum = eleFile.GetColCountByRow(0);
	}
	hash_map<string, int> hmapSI;
	for (int nCol = 0; nCol < nColNum; nCol++)
	{
		string strColName = eleFile.GetDataByRowCol(0,nCol);
		hmapSI.insert(make_pair(strColName, nCol));
		if (strColName == strDisplayColName)
		{
			nDisplayCol = nCol;
		}
	}

	vector<hash_map<string, string> > vecAttribute;
	hash_map<string, RowInfo> mapGmDisplayCol;
	string strDisplayColValue;
	for (int nRow = 0; nRow < nRowNum; nRow++)
	{
		vecAttribute.clear();
		for (int i = 0; i < (int)vecStr.size(); i++)
		{
			int nCol = hmapSI[vecStr[i]];
			string strContent;
			strContent = eleFile.GetDataByRowCol(nRow,nCol);
			int nSize = (int)vecAttribute.size();
			vecAttribute.push_back(hash_map<string, string>());
			vecAttribute[nSize].insert(make_pair(vecStr[i], strContent));
		}
		if (gmGroupInfo.CheckCondition(vecAttribute))
		{
			strDisplayColValue = eleFile.GetDataByRowCol(nRow,nDisplayCol);
			if (mapGmDisplayCol.find(strDisplayColValue) == mapGmDisplayCol.end())
			{
				mapGmDisplayCol[strDisplayColValue] = RowInfo();
				mapGmDisplayCol[strDisplayColValue].nRowOffset = nRow;
				mapGmDisplayCol[strDisplayColValue].bFlag = false;
			}
			else
			{
				if (!mapGmDisplayCol[strDisplayColValue].bFlag)
				{
					int nRow1 = mapGmDisplayCol[strDisplayColValue].nRowOffset;
					string strIndex = eleFile.GetDataByRowCol(nRow1,0);
					string strFileName = strGroupName;
					AddNewData_toGm(strIndex,strFileName,strDisplayColValue);
					mapGmDisplayCol[strDisplayColValue].bFlag = true;
				}
				string strFileName = strGroupName;
				string strIndex = eleFile.GetDataByRowCol(nRow,0);
				AddNewData_toGm(strIndex,strFileName,strDisplayColValue);
			}
		}
	}
}

void CGmLang::MergeElementCol( string& strGroup, string& strFile )
{
	if (!BeGmChange(strFile) && m_pOldFile)
		return;
	m_vecFileChange.push_back(strFile);
	COriFileOperate oriFile(strFile);
	int nRow = 1;
	if (m_hmapElementCol.find(strGroup) == m_hmapElementCol.end())
	{
		m_hmapElementCol[strGroup] = new COriFileOperate() ;
	}
	COriFileOperate& destFile = *m_hmapElementCol[strGroup];
	if (!destFile.GetRowCount())
	{
		nRow = 0;
	}
	for (int i=nRow;i<(int)oriFile.GetRowCount();++i)
	{
		int nCur = destFile.GetRowCount();
		for (int j=0;j<(int)oriFile.GetColCountByRow(0);++j)
		{
			destFile.AddData_toTab(oriFile.GetDataByRowCol(i,j),nCur,j);
		}
	}
}

vector<string> CGmLang::GmGroupInfo::GetIndexCol()
{
	vector<string> vecIndex;
	for (int i = 0; i < (int)m_vecIndexInfo.size(); i++)
	{
		vecIndex.push_back(m_vecIndexInfo[i].begin()->first);
	}
	return vecIndex;
}

bool CGmLang::GmGroupInfo::CheckCondition(vector<hash_map<string,string> >& vecAttribute)
{
	bool bResult = true;
	for (int i = 0; i < (int)vecAttribute.size(); i++)
	{
		const string& strIndex = vecAttribute[0].begin()->first;
		const string& strContent  = vecAttribute[0].begin()->second;
		for(int j = 0; j < (int)m_vecIndexInfo.size(); j++)
		{
			const string& strExceptIndex = m_vecIndexInfo[j].begin()->first;
			if (strIndex == strExceptIndex)
			{
				string& strValue = m_vecIndexInfo[j].begin()->second;
				if (strValue == "" || strValue == strContent)
				{
					continue;
				}
				else
				{
					bResult = false;
					return bResult;
				}
			}
		}
	}
	return bResult;
}

bool CGmLang::BeGmChange(string& strFile)
{
	return true;
	string str = FormatFileFullPath(strFile);
	return CRecordManager::GetInst()->BeChanged(str);
}

string CGmLang::FormatFileFullPath(const string& strFile)
{
	string strReuslt = strFile;
	int pos = strReuslt.rfind("../");
	replace_all_distinct(strReuslt,"./","/",pos+3);
	replace_all_distinct(strReuslt,"//","/",0);
	return strReuslt;
}

void CGmLang::AddNewData_toGm(string& strIndex,string& strFileName,string& strDisplayName)
{
	string strPath = FormatFileFullPath(strFileName);
	if (BeExistSameData(strIndex,strFileName,strDisplayName))
		return;
	else if(BeExistSameIndexDiffName(strIndex,strFileName,strDisplayName))
	{
		DeleteOldData(strIndex,strFileName);
	}
	int num = m_destFile.GetRowCount();
	m_destFile.AddData_toTab(strIndex,num,0);
	m_destFile.AddData_toTab(strIndex,num,1);
	string strBlank;
	m_destFile.AddData_toTab(strBlank,num,2);
	int newNum = m_destFile.GetRowCount();
	m_destFile.AddData_toTab(strBlank,newNum,0);
	m_destFile.AddData_toTab(strPath,newNum,1);
	m_destFile.AddData_toTab(strDisplayName,newNum,2);
	m_bNeedSave = true;
}

void CGmLang::DeleteOldData(string& strIndex,string& strFileName)
{
	if (!m_pOldFile)
		return;
	if(BeGmChange(m_strGmPath))
		return;
	string strFull = FormatFileFullPath(strFileName);
	int row = m_pOldFile->GetRowCount();
	for (int i=1;i<row-1;++i)
	{
		if (m_pOldFile->GetDataByRowCol(i,0) == strIndex 
			&& m_pOldFile->GetDataByRowCol(i+1,0) == "" 
			&& m_pOldFile->GetDataByRowCol(i+1,1) == strFull)
		{
			m_pOldFile->DeleteRow(i+1);
			m_pOldFile->DeleteRow(i);
			break;
		}
	}
}

void CGmLang::CopyNewData_toOld()
{
	if(!m_pOldFile)
		return;
	int row = m_pOldFile->GetRowCount();
	map<string,vector<string> > mapSave;
	for (int i=row-1;i>1;--i)
	{
		string strFirst = m_pOldFile->GetDataByRowCol(i,0);
		string strSecond = m_pOldFile->GetDataByRowCol(i,1);
		string strThird = m_pOldFile->GetDataByRowCol(i,2);
		bool bFound = false;
		if (strFirst == "" && strSecond != "")
		{
			if (mapSave.count(strSecond))
			{
				if(find(mapSave[strSecond].begin(),mapSave[strSecond].end(),strThird) != mapSave[strSecond].end())
					continue;
			}
			for (int j=i-1;j>1;--j)
			{
				string strInFirst = m_pOldFile->GetDataByRowCol(j,0);
				string strInSecond = m_pOldFile->GetDataByRowCol(j,1);
				string strInThird  = m_pOldFile->GetDataByRowCol(j,2);
				if (strInFirst == "" && strInSecond == strSecond && strInThird == strThird)
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				m_pOldFile->DeleteRow(i);
				m_pOldFile->DeleteRow(i-1);
				--i;
			}
			else
			{
				mapSave[strSecond].push_back(strThird);
			}
		}
	}
	int colNum = m_destFile.GetColCountByRow(0);
	for (int i=1;i<(int)m_destFile.GetRowCount();++i)
	{
		int nowRow = m_pOldFile->GetRowCount();
		for (int j=0;j<colNum;++j)
		{
			m_pOldFile->AddData_toTab(m_destFile.GetDataByRowCol(i,j),nowRow,j);
		}
	}
}

bool CGmLang::BeExistSameData(string& strIndex,string& strFileName,string& strDisplayName)
{
	static size_t s_Count = 0;
	++s_Count;
	printf("%d strIndex = %s, strFileName = %s, strDisplayName = %s\n", s_Count, strIndex.c_str(),
		strFileName.c_str(), strDisplayName.c_str());
	if (!m_pOldFile)
		return false;
	bool bExist = false;
	int row = m_pOldFile->GetRowCount();
	for (int i=1;i<(int)m_pOldFile->GetRowCount()-1;++i)
	{
		string strFirst = m_pOldFile->GetDataByRowCol(i,0);
		string strSecond = m_pOldFile->GetDataByRowCol(i+1,1);
		string strThird = m_pOldFile->GetDataByRowCol(i+1,2);
		if (strFirst == strIndex && strSecond == strFileName && strThird == strDisplayName)
		{
			bExist = true;
			break;
		}
	}
	return bExist;
}

bool CGmLang::BeExistSameIndexDiffName(string& strIndex,string& strFileName,string& strDisplayName)
{
	if (!m_pOldFile)
		return false;
	bool bExist = false;
	int row = m_pOldFile->GetRowCount();
	for (int i=1;i<(int)m_pOldFile->GetRowCount()-1;++i)
	{
		string strFirst = m_pOldFile->GetDataByRowCol(i,0);
		string strSecond = m_pOldFile->GetDataByRowCol(i+1,1);
		string strThird = m_pOldFile->GetDataByRowCol(i+1,2);
		if (strFirst == strIndex && strSecond == strFileName && strThird != strDisplayName)
		{
			bExist = true;
			break;
		}
	}
	return bExist;
}
