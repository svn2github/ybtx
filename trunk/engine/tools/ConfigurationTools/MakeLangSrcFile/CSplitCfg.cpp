#include "stdafx.h"
#include "CSplitCfg.h"
#include "CSplitInfo.h"
#include "CodeCvs.h"
#include "common.h"
#include "COriFileOperate.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

#pragma warning(push)
#pragma warning(disable:4018)

#define CHUNKNAME "MakeLangSrcFile"

CSplitCfg::CSplitCfg()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	CRecordManager::SetChunkName(CHUNKNAME);
}

CSplitCfg::~CSplitCfg()
{

}

void CSplitCfg::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("拆分配置表");
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), SplitDif, NULL);
}

FTW_RESULT CSplitCfg::SplitDif( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CSplitCfg* pSplit = CSplitCfg::GetInst();
	if (pSplit->BeInSplitList(strFileName) && CRecordManager::GetInst()->BeChanged(strFileName))
	{
		pSplit->SplitDif(strFileName);
	}
	return _FTW_CONTINUNE;
}

bool CSplitCfg::SplitNpc_Common(string& strFileName)
{
	string strOldFileName = GetFileNameFromPath(strFileName);
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	string strCompare = "designer_Npc_Common" + strLuaCfgExtend;
	if (strOldFileName != strCompare)
		return false;
	CConsoleInfo::GetInst()->BeginToPrint(strOldFileName.c_str());
	COriFileOperate oriFile(strFileName);
	COriFileOperate destFile;
	oriFile.CleanBlanRow();
	oriFile.CleanNoteRow();
	vector<string> vecOldCol;
	vecOldCol.push_back("Title");
	vecOldCol.push_back("NpcHonour");
	vector<string> vecFirstRow;
	oriFile.GetOneRow(vecFirstRow,0);
	m_pSplitInfo->GetSourceTransCol(strOldFileName,vecOldCol);
	vector<int> vecSrcNumber;
	GetSplitColNumber(vecFirstRow,vecOldCol,vecSrcNumber);
	string strLangIndex = "Index";
	destFile.AddData_toTab(strLangIndex,0,0);
	destFile.AddData_toTab(vecOldCol[0],0,1);
	destFile.AddData_toTab(vecOldCol[1],0,2);
	for (int i=1;i<oriFile.GetRowCount();++i)
	{
		string strIndex = oriFile.GetDataByRowCol(i,0);
		string strTitle = oriFile.GetDataByRowCol(i,vecSrcNumber[0]);
		string strHonour = oriFile.GetDataByRowCol(i,vecSrcNumber[1]);
		if (strTitle == "")
			strTitle = strIndex;
		int nowRow = destFile.GetRowCount();
		strIndex = mbcs_to_MemH64(strIndex);
		destFile.AddData_toTab(strIndex,nowRow,0);
		destFile.AddData_toTab(strTitle,nowRow,1);
		destFile.AddData_toTab(strHonour,nowRow,2);
	}
	oriFile.DeleteCol(vecSrcNumber[1]);
	vector<string> vecChild;
	if (m_pSplitInfo->BeMakeAppFile(strOldFileName))
	{
		string strAppName = GetAppWholePath(strOldFileName);
		CreateFileDirectory(strAppName);
		oriFile.SetName(strAppName);
		oriFile.SaveFile();
		vecChild.push_back(strAppName);
	}
	string strTranName = GetLangWholePath(strOldFileName);
	CreateFileDirectory(strTranName);
	destFile.SetName(strTranName);
	destFile.SaveFile();
	vecChild.push_back(strTranName);
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (int i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
	CConsoleInfo::GetInst()->EndToPrint();
	return true;
}

void CSplitCfg::SplitDif(string& strFileName)
{
	//if (SplitNpc_Common(strFileName))
	//	return;
	string strOldFileName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldFileName.c_str());
	COriFileOperate oriFile(strFileName);
	COriFileOperate destFile;
	oriFile.CleanBlanRow();
	oriFile.CleanNoteRow();
	vector<string> vecOldCol;
	vector<string> vecNewCol;
	vector<string> vecCommon;
	vector<string> vecLangColName;
	m_pSplitInfo->GetSourceTransCol(strOldFileName,vecOldCol);
	m_pSplitInfo->GetDestTransCol(strOldFileName,vecNewCol);
	m_pSplitInfo->GetCommonIndex(strOldFileName,vecCommon);
	m_pSplitInfo->GetDestTransCol(strOldFileName,vecLangColName);
	bool bAllSplit = vecCommon.empty() ? false : true;
	vector<int> vecSrcNumber;
	vector<string> vecFirstRow;
	oriFile.GetOneRow(vecFirstRow,0);
	GetSplitColNumber(vecFirstRow,vecOldCol,vecSrcNumber);

	vector<int> vecCommonNumber;
	if(bAllSplit)
		GetSplitColNumber(vecFirstRow,vecCommon,vecCommonNumber);
	vector<int> vecOldIndex;
	GetSplitColNumber(vecFirstRow,vecOldCol,vecOldIndex);

	SQR_TRY
		for (size_t i=0;i<vecSrcNumber.size();++i)
		{
			destFile.AddData_toTab(vecNewCol[i],0,i);
		}
		map<int,string> mapIndexName;
		CreateIndexToNameMap(vecFirstRow,mapIndexName);
		vector<map<string,string> > vecAttribute;
		bool bHaveExcept = m_pSplitInfo->BeHaveExceptInfo(strOldFileName);
		vector<int> vecNote;
		for (int i=1;i<(int)oriFile.GetRowCount();++i)
		{
			bool bSatifyExcept = false;
			if(bHaveExcept)
			{
				string eleTemp;
				vector<string> vecCurRow;
				oriFile.GetOneRow(vecCurRow,i);
				if (m_pSplitInfo->CreateExceptIndex(strOldFileName,vecCurRow,mapIndexName,eleTemp))
				{
					destFile.AddRow_Back();
					int curRow = destFile.GetRowCount()-1;
					int curCol = destFile.GetColCountByRow(curRow);
					ReplaceWildcard(eleTemp);
					destFile.AddData_toTab(eleTemp,curRow,curCol);
					bSatifyExcept = true;
				}
			}
			if (!bSatifyExcept && bAllSplit)
			{
				destFile.AddRow_Back();
				if (vecCommonNumber.size()>1)
				{
					vector<string> vecCurRow;
					oriFile.GetOneRow(vecCurRow,i);
					string strNewIndex = CreateCommonIndex(vecCurRow,vecCommonNumber);
					destFile.AddData_toTab(strNewIndex,destFile.GetRowCount()-1,destFile.GetColCountByRow(destFile.GetRowCount()-1));
				}
				else
				{
					string strTemp = oriFile.GetDataByRowCol(i,0);
					if (oriFile.BeNumber(i,0))
					{
						destFile.AddData_toTab(strTemp,destFile.GetRowCount()-1,destFile.GetColCountByRow(destFile.GetRowCount()-1),true);
					}
					else
					{
						strTemp = mbcs_to_MemH64(strTemp);
						destFile.AddData_toTab(strTemp,destFile.GetRowCount()-1,destFile.GetColCountByRow(destFile.GetRowCount()-1));
					}
				}
			}
			else if(!bSatifyExcept)
				continue;
			for (int v=0;v<(int)vecOldIndex.size();++v)
			{
				string strTemp = oriFile.GetDataByRowCol(i,vecOldIndex[v]);
				ReplaceWildcard(strTemp);
				if (oriFile.BeNumber(i,vecOldIndex[v]))
				{
					destFile.AddData_toTab(strTemp,destFile.GetRowCount()-1,destFile.GetColCountByRow(destFile.GetRowCount()-1),true);
				}
				else
				{
					destFile.AddData_toTab(strTemp,destFile.GetRowCount()-1,destFile.GetColCountByRow(destFile.GetRowCount()-1));
				}
			}
			if (!vecNote.empty())
			{
				for (int ndex =(int)vecNote.size()-1;ndex>=0;--ndex)
				{
					destFile.InsertData_toTab(oriFile.GetDataByRowCol(i,vecNote[ndex]),destFile.GetRowCount()-1,1);
				}
			}
			if (bAllSplit)
			{
				//修补BUG,加进来的,由于拆分时表里填的列名并不一定按顺序添写,这里就会出问题,如果
				//拆分前进行就会导致翻译表内容列顺序变更,老外已经进行翻译了,所以尽量保持语言表列不变
				vector<int> vecCry = vecOldIndex;
				vector<string> vecCryStr = vecOldCol;
				for (int m=0;m<vecCry.size();++m)
				{
					for (int n=m+1;n<vecCry.size();++n)
					{
						if (vecCry[n]<vecCry[m])
						{
							int minTemp = vecCry[n];
							vecCry[n] = vecCry[m];
							vecCry[m] = minTemp;
							string strTemp = vecCryStr[n];
							vecCryStr[n] =  vecCryStr[m];
							vecCryStr[m] = strTemp;
						}
					}
				}
				for (int v=(int)vecCry.size()-1;v>=0;--v)
				{
					if (m_pSplitInfo->BeDeleteCol(strOldFileName,vecCryStr[v]))
					{
						oriFile.DeleteColByRow(i,vecCry[v]);
					}
				}
			}
			else
			{
				for (int v=(int)vecOldIndex.size()-1;v>=0;--v)
				{
					if (m_pSplitInfo->BeDeleteCol(strOldFileName,vecOldCol[v]))
					{
						oriFile.DeleteData(i,vecOldIndex[v]);
					}
				}
			}
		}
		string strIndex =  m_pSplitInfo->GetLangIndexName(strOldFileName);
		destFile.InsertData_toTab(strIndex,0,0);
		if (bAllSplit)
		{
			//修补BUG,由于拆分时表里填的列名并不一定按顺序添写,这里就会出问题,如果
			//拆分前进行排序就会导致翻译表内容列顺序变更,老外已经进行翻译了,所以尽量保持语言表列不变
			vector<int> vecCry = vecOldIndex;
			vector<string> vecCryStr = vecOldCol;
			for (int i=0;i<vecCry.size();++i)
			{
				for (int j=i+1;j<vecCry.size();++j)
				{
					if (vecCry[j]<vecCry[i])
					{
						int minTemp = vecCry[j];
						vecCry[j] = vecCry[i];
						vecCry[i] = minTemp;
						string strTemp = vecCryStr[j];
						vecCryStr[j] =  vecCryStr[i];
						vecCryStr[i] = strTemp;
					}
				}
			}

			for (int v=(int)vecCry.size()-1;v>=0;--v)
			{
				if (m_pSplitInfo->BeDeleteCol(strOldFileName,vecCryStr[v]))
				{
					oriFile.DeleteColByRow(0,vecCry[v]);
				}
			}
		}
		vector<string> vecChild;
		if (m_pSplitInfo->BeMakeAppFile(strOldFileName))
		{
			string strAppName = GetAppWholePath(strOldFileName);
			CreateFileDirectory(strAppName);
			oriFile.SetName(strAppName);
			oriFile.SaveFile();
			vecChild.push_back(strAppName);
		}
		string strTranName = GetLangWholePath(strOldFileName);
		CreateFileDirectory(strTranName);
		destFile.SetName(strTranName);
		destFile.SaveFile();
		vecChild.push_back(strTranName);
		CRecordManager::GetInst()->AddFileRecord(strFileName);
		CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
		for (int i=0;i<vecChild.size();++i)
		{
			CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
		}
		CConsoleInfo::GetInst()->EndToPrint();
		SQR_CATCH(exp)
		{
			string strMsg = strFileName + "在拆分通用表时发生错误，请检查所添加的内容是否正确" + exp.ErrorTitle();
			if(GetLogToFile())
			{
				PrintLog(strMsg);
			}
			else
			{
				MessageBox(0,strMsg.c_str(),NULL,0);
			}
			exit(1);
		}
		SQR_TRY_END

}

string CSplitCfg::GetAppWholePath(const string& strFileName)
{
	string strSrcPath = m_pSplitInfo->GetCfgTempPath();
	string strPath = m_pSplitInfo->GetAppSavePath(strFileName);
	string strName = m_pSplitInfo->GetAppName(strFileName);
	string strResult = strPath + "/" + strName;
	CleanUpFileName(strResult);
	strResult = strSrcPath + "/" + strResult;
	return strResult;
}

string CSplitCfg::GetLangWholePath(const string& strFileName)
{
	string strSrcPath = m_pSplitInfo->GetCfgTempPath();
	string strPath = m_pSplitInfo->GetLangSavePath(strFileName);
	const string& strOriLang = CSplitInfo::GetInst()->GetOriLang();
	const string& strLangSrcFolder = CSplitInfo::GetInst()->GetLangSrcFolder();
	replace_all_distinct(strPath, strOriLang, strLangSrcFolder, 0);
	string strName = m_pSplitInfo->GetLangName(strFileName);
	string strResult = strPath + "/" + strName;
	CleanUpFileName(strResult);
	const string& strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	replace_all_distinct(strResult,".txt",strLuaCfgExtend,0);
	strResult = strSrcPath + "/" + strResult;
	return strResult;
}

string CSplitCfg::GetFileNameFromPath(const string& strFileName)
{
	string strName = strFileName;
	const TCHAR* szSperate = max( strrchr( strFileName.c_str(), '\\' ), strrchr( strFileName.c_str(), '/' ) );
	if( szSperate )
	{
		int subPos = static_cast<int>(szSperate-strFileName.c_str());
		strName = strFileName.substr(subPos+1,strFileName.length()-subPos-1);
	}
	return strName;
}

void CSplitCfg::GetSplitColNumber(vector<string>& vecRef,vector<string>& vecSrc,vector<int>& vecArg)
{
	for (int i=0;i<(int)vecSrc.size();++i)
	{
		string strTemp = vecSrc[i];
		for (int j=0;j<(int)vecRef.size();++j)
		{
			if (vecRef[j] == strTemp)
			{
				vecArg.push_back(j);
			}
		}
	}
}

void CSplitCfg::CleanUpFileName(string& strFileName)
{
	if (strFileName.find("./") == 0)
	{
		strFileName.erase(0,2);
	}
}

void CSplitCfg::CreateIndexToNameMap(vector<string>& vecRef,map<int,string>& mapIndexName)
{
	for (int i=0;i<(int)vecRef.size();++i)
	{
		mapIndexName.insert(make_pair(i,vecRef[i]));
	}
}

string CSplitCfg::CreateCommonIndex(vector<string>& vecRef,vector<int>& vecIndexNumber)
{
	string strIndex;
	string strTemp;
	sort(vecIndexNumber.begin(),vecIndexNumber.end(),less<int>());
	for (int i=0;i<(int)vecIndexNumber.size();++i)
	{
		strTemp = mbcs_to_MemH64(vecRef[vecIndexNumber[i]]);
		if(strIndex == "")
			strIndex = strTemp;
		else
			strIndex = strIndex + GetLangIndexConnSymbol() + strTemp;
	}
	return strIndex;
}

string CSplitCfg::GetLangIndexConnSymbol()
{
	return "_";
}

void CSplitCfg::ReplaceWildcard(string& str)
{
	int percentPos = 0;
	int argNum = 1;
	while ((percentPos = str.find('%',percentPos)) != -1)
	{
		if (percentPos == str.length()-1)
			break;
		if (str[percentPos+1] == '%')
		{
			percentPos += 2;
			continue;
		}
		if (percentPos>(int)str.length()-1)
			break;
		int nextPercentPos = str.find('%',percentPos+1);
		int wellPos = str.find('#',percentPos+1);
		int dPos = str.find('d',percentPos+1);
		int fPos = str.find('f',percentPos+1);
		int sPos = str.find('s',percentPos + 1);
		int nearPos = -1;
		if (dPos == -1 && fPos ==-1 && sPos == -1)
			break;
		if (dPos != -1 && fPos != -1)
		{
			nearPos = dPos < fPos ? dPos : fPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else if(dPos != -1)
		{
			nearPos = dPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else if (fPos != -1)
		{
			nearPos = fPos;
			if (sPos != -1)
			{
				nearPos = sPos < nearPos ? sPos : nearPos;
			}
		}
		else
		{
			nearPos = sPos;
		}
		if (nearPos == -1)
		{
			break;
		}
		bool bContinue = false;
		for (int i = percentPos+1; i< nearPos; ++i)
		{
			if (str[i] != '.' && !(str[i] >= '0' && str[i] <= '9'))
			{
				bContinue = true;
			}
		}
		if (bContinue)
		{
			++percentPos;
			continue;
		}

		string strWildcard;
		char numArray[10];
		numArray[9] = '\0';
		_itoa(argNum,numArray,10);
		string strHead = "#arg";
		strHead = strHead + numArray;
		strHead = strHead + "(";
		strWildcard = str.substr(percentPos,nearPos-percentPos+1);
		strWildcard = strHead + strWildcard;
		strWildcard = strWildcard + ")";
		str = str.replace(percentPos,nearPos-percentPos+1,strWildcard);
		++argNum;
		percentPos = percentPos + strWildcard.length();
	}
}

bool CSplitCfg::BeInSplitList(const string& strFileName)
{
	string strName = GetFileNameFromPath(strFileName);
	if (m_pSplitInfo->BeInSplitList(strName))
		return true;
	else
		return false;
}


#pragma warning(pop)
