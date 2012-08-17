#include "stdafx.h"
#include "LanDif2LanTxt.h"
#include "common.h"
#include "CSplitInfo.h"
#include "COriFileOperate.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

#define CHUNKNAME "MakeLangNeutralTxt"

CLanDif2LanTxt::CLanDif2LanTxt()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	CRecordManager::SetChunkName(CHUNKNAME);
}

CLanDif2LanTxt::~CLanDif2LanTxt()
{

}

void CLanDif2LanTxt::StartProcess()
{
	wstring wstrPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	string strPostfix = utf16_to_gbk(wstrPostfix);
	strPostfix.erase(strPostfix.begin());
	strPostfix = "语言表" + strPostfix + "转txt";
	CConsoleInfo::GetInst()->PrintFunction(strPostfix);
//	string strPath = m_pSplitInfo->GetSrcPath();
//	wstring wstrPath = gbk_to_utf16(strPath);
	string strLangSrcPath = m_pSplitInfo->GetLangSrcPath();
	wstring wstrLangSrcPath = gbk_to_utf16(strLangSrcPath);
	FileTreeWalk(wstrLangSrcPath.c_str(), LanDif2LanTxt, NULL);
}

FTW_RESULT CLanDif2LanTxt::LanDif2LanTxt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring strPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	if( _wcsicmp( szFileName + nLen - strPostfix.length(), strPostfix.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CLanDif2LanTxt* pManager = CLanDif2LanTxt::GetInst();
	if (pManager->BeLangDif(strFileName) && CRecordManager::GetInst()->BeChanged(strFileName))
	{
		pManager->Change2LanTxt(strFileName);
	}
	return _FTW_CONTINUNE;
}

void CLanDif2LanTxt::Change2LanTxt(string& strFileName)
{
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	COriFileOperate oriFile(strFileName);
	COriFileOperate destFile;
	string strDest = strFileName;
	string strLuaCfgExtend = CSplitInfo::GetInst()->GetLuaCfgExtend();
	replace_all_distinct(strDest,strLuaCfgExtend,".txt",0);
	ofstream outputFile(strDest.c_str(),ios_base::out | ios_base::binary);
	if (!outputFile)
	{
		string errorStr = strFileName;
		errorStr = errorStr + "打开时候发生错误，请检查是否有其他程序正在使用中！";
		if(GetLogToFile())
		{
			PrintLog(errorStr);
		}
		else
		{
			MessageBox(NULL,errorStr.c_str(),NULL,0);
		}
		exit(1);
	}
	int rowNum = oriFile.GetRowCount();
	int colNum = oriFile.GetColCountByRow(0);
	map<string, bool> mapIndexHaved;
	for (int i=0;i<rowNum;++i)
	{
		if (i>0)
		{
			string strIndex = oriFile.GetDataByRowCol(i,0);
			if (mapIndexHaved.count(strIndex) == 0)
			{
				mapIndexHaved[strIndex] = true;
			}
			else
			{
				string strMsg = strFileName + "索引为=" + strIndex + "发生重复";
				//MessageBox(0,strMsg.c_str(),"Error",NULL);
				//exit(0);
				//return;
				continue;
			}
		}
		for (int j=0;j<colNum;++j)
		{
			string strContent = oriFile.GetDataByRowCol(i,j);
			AddExcelTxtDoubleQuotation(strContent);
			strContent = gbk_to_utf8(strContent);
			outputFile.write((char*)strContent.c_str(),strContent.size());
			if (j != colNum-1)
			{
				outputFile.write("\t",1);
			}
		}
		outputFile.write("\n",1);
	}
	outputFile.close();
	vector<string> vecChild;
	vecChild.push_back(strDest);
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (size_t i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CLanDif2LanTxt::AddExcelTxtDoubleQuotation(string& str)
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

string CLanDif2LanTxt::gbk_to_utf8( const string& src )
{
	wstring wcs_tmp = gbk_to_utf16(src);
	return utf16_to_utf8(wcs_tmp);
}

bool CLanDif2LanTxt::BeLangDif(string& strFileName)
{
	if(m_pSplitInfo->BeLangDif(strFileName))
		return true;
	else
		return false;
}
