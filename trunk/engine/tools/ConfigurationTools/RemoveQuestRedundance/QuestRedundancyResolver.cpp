#include "stdafx.h"
#include "QuestRedundancyResolver.h"
#include "COriFileOperate.h"
#include "CExpLog.h"

vector<string> CQuestRedundancyResolver::s_vecDialog = vector<string>();

CQuestRedundancyResolver::CQuestRedundancyResolver()
{
	m_pSplitInfo = CSplitInfo::GetInst();
}

CQuestRedundancyResolver::~CQuestRedundancyResolver()
{

}

FTW_RESULT CQuestRedundancyResolver::ResolveRedundancy( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CQuestRedundancyResolver* pInst = CQuestRedundancyResolver::GetInst();
	if (pInst->BeInResolveList(strFileName))
	{
		pInst->DoResolve(strFileName);
	}
	return _FTW_CONTINUNE;
}

FTW_RESULT CQuestRedundancyResolver::ResolveDesc( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	CQuestRedundancyResolver* pInst = CQuestRedundancyResolver::GetInst();
	if (pInst->BeInDescResolveList(strFileName))
	{
		pInst->DoResolveDesc(strFileName);
	}
	return _FTW_CONTINUNE;
}

string CQuestRedundancyResolver::GetFileNameFromPath( const string& strFileName )
{
	string strName = strFileName;
	const TCHAR* szSperate = max(strrchr(strFileName.c_str(), '\\'), strrchr(strFileName.c_str(), '/'));
	if (szSperate)
	{
		int subPos = static_cast<int>(szSperate - strFileName.c_str());
		strName = strFileName.substr(subPos + 1, strFileName.length() - subPos - 1);
	}
	return strName;
}

bool CQuestRedundancyResolver::BeInDescResolveList( string& strFileName )
{
	string strName = GetFileNameFromPath(strFileName);
	if (m_pSplitInfo->BeInSplitList(strName))
		return true;
	else
		return false;
}

bool CQuestRedundancyResolver::BeInResolveList( string& strFileName )
{
	if(m_pSplitInfo->BeInSplitList(strFileName))
		return true;
	else
		return false;
}

void CQuestRedundancyResolver::DoResolveDesc( string& strFileName )
{
	COriFileOperate oriFile(strFileName);
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strQuestField1;
	string strQuestField2;
	string strQuestDesc;
	string strQuestDia;
	for (int nRow = 1; nRow < nRowNum; nRow++)
	{
		if (!oriFile.BeNumber(nRow,1))
		{
			strQuestField1 = oriFile.GetDataByRowCol(nRow,1);
		}
		if (strQuestField1 == "任务接取对话")
		{
			if (!oriFile.BeNumber(++nRow,1))
			{
				strQuestField2 = oriFile.GetDataByRowCol(nRow,1);
			}
		}
		if (strQuestField1 == "任务接取对话" && strQuestField2 == "任务描述")
		{
			if (!oriFile.BeNumber(nRow-1,3))
			{
				strQuestDia = oriFile.GetDataByRowCol(nRow-1,3);
			}
			if (!oriFile.BeNumber(nRow,3))
			{
				strQuestDesc = oriFile.GetDataByRowCol(nRow,3);
			}
			if (strQuestDia == strQuestDesc)
			{
				string strBlank;
				oriFile.AddData_toTab(strBlank,nRow,3);
			}
		}
	}
	oriFile.SaveFile();
}

void CQuestRedundancyResolver::DoResolve( string& strFileName )
{
	COriFileOperate oriFile(strFileName);
	int nRowNum = (int)oriFile.GetRowCount();
	int nColNum = 0;
	if (nRowNum)
	{
		nColNum = (int)oriFile.GetColCountByRow(0);
	}
	string strQuestField;
	string strQuestContent;
	string strQuestFieldIndex;
	char szTemp[10];
	for (int nRow = 1; nRow < nRowNum; nRow++)
	{
		if (!oriFile.BeNumber(nRow,1))
		{
			strQuestField = oriFile.GetDataByRowCol(nRow,1);
		}
		if (strQuestField == "任务接取对话" || strQuestField == "任务描述")
		{
			if (!oriFile.BeNumber(nRow,3))
			{
				strQuestContent = oriFile.GetDataByRowCol(nRow,3);
			}
			if (strQuestContent != "")
			{
				vector<string>::iterator iter = find(s_vecDialog.begin(), s_vecDialog.end(), strQuestContent);
				if (iter == s_vecDialog.end())
				{
					s_vecDialog.push_back(strQuestContent);
					sprintf(szTemp, "%d", (int) s_vecDialog.size());
					string strTemp = szTemp;
					oriFile.AddData_toTab(strTemp,nRow,3,true);
					strQuestFieldIndex = strQuestField.substr(1, strQuestField.length() - 2);
					oriFile.AddData_toTab(strQuestFieldIndex,nRow,1);
				}
				else
				{
					int nNumber = (int)(iter - s_vecDialog.begin()) + 1;
					sprintf(szTemp, "%d", nNumber);
					string strTemp = szTemp;
					oriFile.AddData_toTab(strTemp,nRow,3,true);
					strQuestFieldIndex = strQuestField.substr(1, strQuestField.length() - 2);
					strQuestFieldIndex.append("索引");
					oriFile.AddData_toTab(strQuestFieldIndex,nRow,1);
				}
			}
			else
			{
				string strTemp = oriFile.GetDataByRowCol(nRow-1,3);
				if (oriFile.BeNumber(nRow-1,3))
				{
					oriFile.AddData_toTab(strTemp,nRow,3,true);
				}
				else
				{
					oriFile.AddData_toTab(strTemp,nRow,3);
				}
				strQuestFieldIndex = strQuestField.substr(1, strQuestField.length() - 2);
				strQuestFieldIndex.append("索引");
				oriFile.AddData_toTab(strQuestFieldIndex,nRow,1);
			}
		}
	}
	oriFile.SaveFile();
}

void CQuestRedundancyResolver::WriteToQuestStringFile( string& strQuestStrFile )
{
	COriFileOperate destFile;
	int curRow = destFile.GetRowCount();
	string strTemp = "Index";
	destFile.AddData_toTab(strTemp,curRow,0);
	strTemp = "Description";
	destFile.AddData_toTab(strTemp,curRow,1);
	strTemp = "WildArg";
	destFile.AddData_toTab(strTemp,curRow,2);

	for (int i = 0; i < (int)s_vecDialog.size(); i++)
	{
		int cur = destFile.GetRowCount();
		char szTemp[10];
		sprintf(szTemp, "%d", i + 1);
		string strNum = szTemp;
		destFile.AddData_toTab(strNum,cur,0,true);
		destFile.AddData_toTab(s_vecDialog[i],cur,1);
		string strBlank;
		destFile.AddData_toTab(strBlank,cur,2);
	}
	destFile.SetName(strQuestStrFile);
	destFile.SaveFile();
}

