#include "stdafx.h"
#include "CTransTradition.h"
#include "common.h"
#include "ConsoleInfo.h"
#include <sstream>
#include "CWordInterface.h"

#pragma warning(disable:4996)

CTransTradition::CTransTradition()
{
	//HRESULT hr;
	//CLSID clsid = {0x209ff,0,0,{0xc0,0,0,0,0,0,0,0x46}};
	//LPOLESTR lpwProgID = NULL;
	//hr = ::ProgIDFromCLSID(clsid, &lpwProgID);
	//if(SUCCEEDED(hr))
	//{
	//	::MessageBoxW(NULL, lpwProgID, L"ProgID", MB_OK);
	//	IMalloc* pMalloc = NULL;
	//	hr = ::CoGetMalloc(1, &pMalloc);
	//	if(SUCCEEDED(hr))
	//	{
	//		pMalloc->Free(lpwProgID);
	//		pMalloc->Release();
	//	}
	//}

	//CWordInterface::GetInst()->Start();
}

//CTransTradition::~CTransTradition()
//{
//	CWordInterface::GetInst()->Finish();
//}

FTW_RESULT CTransTradition::EnumTreeFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;
	wstring wstr = L".txt";
	if( _wcsicmp( szFileName + nLen - wstr.length(), wstr.c_str() ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	CTransTradition* pManager = CTransTradition::GetInst();
	pManager->CreateTraditionFileByLine(strFileName);
	return _FTW_CONTINUNE;
}

void CTransTradition::CreateTraditionFileByItem(string& strFileName)
{
	CConsoleInfo::GetInst()->BeginToPrint(strFileName.c_str());

	vector<vector<string> > vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);

	string strSaveName = MakeSavePath(strFileName);
	CreateFileDirectory(strSaveName);

	SQR_TRY
		ofstream outFile(strSaveName.c_str());
	if (!outFile)
	{
		string strMsg = strSaveName + "在打开时发生错误，请检查文件是否可写";
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	int allRowNum = vecBackUp.size();
	int allColNum = vecBackUp[0].size();
	for (int i=0;i<allRowNum;++i)
	{
		for (int j=0;j<allColNum;++j)
		{
			if(vecBackUp[i].empty())
				continue;
			string sSrcItemStr = utf8_to_gbk(vecBackUp[i][j]);
			//ToolsTCSCTranslate(1,0,1);	//ConvertToTradition(sSrcItemStr);
			TCHAR* szTgtItemStr = NULL;
			size_t uSize;
			CWordInterface::GetInst()->ConvertSCTC(sSrcItemStr.c_str(), szTgtItemStr, uSize);
			if(!szTgtItemStr)
			{
				string strMsg = "[" + sSrcItemStr + "]在简体转换繁体时发生错误，返回为空";
				MessageBox(0,strMsg.c_str(),NULL,0);
				exit(1);
			}
			string sTargetItemStr = gbk_to_utf8(szTgtItemStr);
			delete [] szTgtItemStr;

			outFile<<sTargetItemStr;
			if (j<allColNum-1)
			{
				outFile<<"\t";
			}
		}
		outFile<<"\n";
	}
	outFile.close();
	SQR_CATCH(exp)
	{
		string strMsg = "CTransTradition生成txt文件:" + strSaveName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END;

	CConsoleInfo::GetInst()->EndToPrint();	
}

void CTransTradition::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
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

void CTransTradition::CreateTraditionFileByLine(string& strFileName)
{
	CConsoleInfo::GetInst()->BeginToPrint(strFileName.c_str());

	vector<string> vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);

	string strSaveName = MakeSavePath(strFileName);
	CreateFileDirectory(strSaveName);

	SQR_TRY
		ofstream outFile(strSaveName.c_str());
	if (!outFile)
	{
		string strMsg = strSaveName + "在打开时发生错误，请检查文件是否可写";
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	int allRowNum = vecBackUp.size();
	int allColNum = vecBackUp[0].size();
	for (int i=0;i<allRowNum;++i)
	{
		if(vecBackUp[i].empty())
			continue;
		string sSrcItemStr = utf8_to_gbk(vecBackUp[i]);
		TCHAR* szTgtItemStr = NULL;
		size_t uSize;
		CWordInterface::GetInst()->ConvertSCTC(sSrcItemStr.c_str(), szTgtItemStr, uSize);
		if(!szTgtItemStr)
		{
			string strMsg = "[" + sSrcItemStr + "]在简体转换繁体时发生错误，返回为空";
			MessageBox(0,strMsg.c_str(),NULL,0);
			exit(1);
		}
		string sTargetItemStr = gbk_to_utf8(szTgtItemStr);
		delete [] szTgtItemStr;

		outFile<<sTargetItemStr;

		outFile<<"\n";
	}
	outFile.close();
	SQR_CATCH(exp)
	{
		string strMsg = "CTransTradition生成txt文件:" + strSaveName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END;

	CConsoleInfo::GetInst()->EndToPrint();	
}

void CTransTradition::ReadUtf8Txt(string& strFileName,vector<string>& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	string str;
	while(getline(inFile,str))
	{
		vecRef.push_back(str);
	}
	inFile.close();
}

void CTransTradition::CreateTraditionFile(string& strFileName)
{
	CConsoleInfo::GetInst()->BeginToPrint(strFileName.c_str());

	string strBackUp;
	ReadUtf8Txt(strFileName,strBackUp);

	string strSaveName = MakeSavePath(strFileName);
	CreateFileDirectory(strSaveName);

	SQR_TRY
		ofstream outFile(strSaveName.c_str());
	if (!outFile)
	{
		string strMsg = strSaveName + "在打开时发生错误，请检查文件是否可写";
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}

	if(!strBackUp.empty())
	{
		string sSrcItemStr = utf8_to_gbk(strBackUp);
		TCHAR* szTgtItemStr = NULL;
		size_t uSize;
		CWordInterface::GetInst()->ConvertSCTC(sSrcItemStr.c_str(), szTgtItemStr, uSize);
		if(!szTgtItemStr)
		{
			string strMsg = "[" + sSrcItemStr + "]在简体转换繁体时发生错误，返回为空";
			MessageBox(0,strMsg.c_str(),NULL,0);
			exit(1);
		}
		string sTargetItemStr = gbk_to_utf8(szTgtItemStr);
		delete [] szTgtItemStr;

		outFile<<sTargetItemStr;

	}
	outFile.close();
	SQR_CATCH(exp)
	{
		string strMsg = "CTransTradition生成txt文件:" + strSaveName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END;

	CConsoleInfo::GetInst()->EndToPrint();	
}

void CTransTradition::ReadUtf8Txt(string& strFileName, string& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	stringstream strStream;
	strStream << inFile.rdbuf();
	vecRef = strStream.str();

	inFile.close();
}

string CTransTradition::MakeSavePath(string& strFileName)
{
	string strResult;

	strResult = replace_all_distinct(strFileName, "langsrc", "chinese_tradition",0);

	return strResult;
}

