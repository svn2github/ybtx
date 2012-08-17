#include "stdafx.h"
#include "CNdf2Txt.h"
#include "CSplitInfo.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"
#include "common.h"
#include "COriFileOperate.h"

CNdf2Txt::CNdf2Txt()
{
	m_pSplitInfo = CSplitInfo::GetInst();
}

CNdf2Txt::~CNdf2Txt()
{

}

void CNdf2Txt::StartProcess()
{
	string strPostfix = CSplitInfo::GetInst()->GetCppCfgExtend();
	strPostfix.erase(strPostfix.begin());
	strPostfix = strPostfix + "转txt";
	CConsoleInfo::GetInst()->PrintFunction(strPostfix);
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), Ndf2Txt, NULL);
}

FTW_RESULT CNdf2Txt::Ndf2Txt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetCppCfgExtend();
	if( _wcsicmp( szFileName + nLen - strPostfix.length(),strPostfix.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	if (!CRecordManager::GetInst()->BeChanged(strFileName))
		return _FTW_CONTINUNE;
	CNdf2Txt* pManager = CNdf2Txt::GetInst();
	pManager->Change2Txt(strFileName);
	return _FTW_CONTINUNE;
}

void CNdf2Txt::Change2Txt(string& strFileName)
{
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	string strTgt = m_pSplitInfo->GetTgtPath();
	string strSrc = m_pSplitInfo->GetSrcPath();
	string strDstName = strFileName;
	string strCppCfgExtend = CSplitInfo::GetInst()->GetCppCfgExtend();
	replace_all_distinct(strDstName,strSrc,strTgt,0);
	replace_all_distinct(strDstName,strCppCfgExtend,".txt",0);
	SQR_TRY
		ofstream outFile(strDstName.c_str());
	if (!outFile)
	{
		string strMsg = strDstName + "在打开时发生错误，请检查文件是否可写";
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	COriFileOperate oriFlie(strFileName);
	int startRow = 0;
	int allRowNum = oriFlie.GetRowCount();
	int allColNum = 0;
	allColNum = oriFlie.GetColCountByRow(0);
	for (int i=startRow;i<allRowNum;++i)
	{
		for (int j=0;j<allColNum;++j)
		{
			string strContent = oriFlie.GetDataByRowCol(i,j);
			AddExcelTxtDoubleQuotation(strContent);
			outFile<<strContent;
			if (j<allColNum-1)
				outFile<<"\t";
		}
		outFile<<endl;
	}
	outFile.close();
	SQR_CATCH(exp)
	{
		string strMsg = "CDif2Txt生成txt文件:" + strDstName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END
		vector<string> vecChild;
	vecChild.push_back(strDstName);
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (size_t i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CNdf2Txt::AddExcelTxtDoubleQuotation(string& str)
{
	if (str == "")
		return;
	if (str.find(',',0) == -1)
	{
		replace_all_distinct(str, "\"", "\"\"",0);
		if (str.find("\"\"",0) != -1)
		{
			str = "\"" + str + "\"";
		}
	}
	else
	{
		replace_all_distinct(str, "\"", "\"\"",0);
		str = "\"" + str + "\"";
	}
}
