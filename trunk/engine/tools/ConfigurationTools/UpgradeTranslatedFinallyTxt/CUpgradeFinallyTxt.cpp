#include "stdafx.h"
#include "CUpgradeFinallyTxt.h"
#include "CSplitInfo.h"
#include "common.h"
#include "ConsoleInfo.h"
#include "CPkgFile.h"

#pragma warning(push)
#pragma warning(disable:4018)

#define MULTILANG_VER_BEFORE_20110708

CUpgradeFinallyTxt::CUpgradeFinallyTxt()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	//m_vecLanguage.push_back("chinese_simple");
	m_vecLanguage.push_back("german");
	//m_vecLanguage.push_back("chinese_tradition");
}

CUpgradeFinallyTxt::~CUpgradeFinallyTxt()
{
}

void CUpgradeFinallyTxt::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("翻译语言包txt升级到最新版txt及lua");
	wstring wstrPath = gbk_to_utf16(m_pSplitInfo->GetLangTablePath());
	FileTreeWalk(wstrPath.c_str(), TxtUpgrade, NULL);
	string strGUIPath = m_pSplitInfo->GetLangTablePath();
	replace_all_distinct(strGUIPath, "/table", "/gui", 0);
	wstring wstrGUIPath = gbk_to_utf16(strGUIPath);
	FileTreeWalk(wstrGUIPath.c_str(), TxtUpgrade, NULL);
}

FTW_RESULT CUpgradeFinallyTxt::TxtUpgrade( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);

	CUpgradeFinallyTxt* pManager = CUpgradeFinallyTxt::GetInst();

	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	
	pManager->CreateFinallyFile(strFileName);
	
	CConsoleInfo::GetInst()->EndToPrint();
	
	return _FTW_CONTINUNE;
}

void CUpgradeFinallyTxt::CreateFinallyFile(string& strFileName)
{
	vector<vector<string> > vecBackUp;
	//vector<vector<string> > vecBackUp1;
	//strFileName = Chinese Language
	//如果有直接文件，则读直接文件，否则读打包文件
	//游戏逻辑读语言文件也应该如此
	//ReadUtf8Txt(strFileName,vecBackUp1);
	ReadUtf8TxtFromPkg(strFileName,vecBackUp);

	for(int uLang = 0; uLang < m_vecLanguage.size(); ++uLang)
	{
		//strCompTxtName = German Language
		string& strLanguage = m_vecLanguage[uLang];
		string strCompTxtName = strFileName;
		replace_all_distinct(strCompTxtName,"chinese_simple",strLanguage,0);
		string strBaseName = strFileName.substr(0, strFileName.size() - 4);

		string strLanguageUpgrade = strLanguage + "_upgrade";
		//strSaveTxtName = Mixed Language
		string strSaveTxtName = strFileName;
		replace_all_distinct(strSaveTxtName,"chinese_simple",strLanguageUpgrade,0);
		string strSaveBaseName = strSaveTxtName.substr(0, strSaveTxtName.size() - 4);
#ifdef MULTILANG_VER_BEFORE_20110708
		string strSaveLuaName = strSaveBaseName + ".lua";
#endif
		
		CreateFileDirectory(strSaveTxtName);

		ifstream inFile(strCompTxtName.c_str());
		if(inFile)
		{
			vector<vector<string> > vecComp;
			ReadUtf8Txt(inFile, vecComp);
			inFile.close();

			vector<vector<string> > vecTarget;
			//合并文件
			SQR_TRY
				ofstream outFile(strSaveTxtName.c_str());
			if (!outFile)
			{
				string strMsg = strSaveTxtName + "在打开时发生错误，请检查文件是否可写";
				MessageBox(0,strMsg.c_str(),NULL,0);
				exit(1);
			}
			int allRowNum = vecBackUp.size();
			int allColNum = vecBackUp[0].size();
			int allCompColNum = vecComp[0].size();
			VecIndex vecColMap;
			BuildColMap(vecBackUp[0], vecComp[0], vecColMap);
			vecTarget.resize(allRowNum);
			for (int iBackUp = 0;iBackUp < allRowNum; ++iBackUp)
			{
				if(vecBackUp[iBackUp].empty())
					continue;
				string& strKey = vecBackUp[iBackUp][0];
				vecTarget[iBackUp].resize(allColNum);
				int iComp = 0;
				for(; iComp < vecComp.size(); ++iComp)
				{
					if(vecComp[iComp].empty())
						continue;
					if(vecComp[iComp][0] == strKey)
						break;
				}
				if(iComp < vecComp.size())
				{
					//找到翻译行
					for (int iCol = 0; iCol < allColNum; ++iCol)
					{
						if(vecColMap[iCol] != -1)
						{
							vecTarget[iBackUp][iCol] = vecComp[iComp][vecColMap[iCol]];
						}
						else
						{
							vecTarget[iBackUp][iCol] = vecBackUp[iBackUp][iCol];
						}
						outFile << vecTarget[iBackUp][iCol];
						if (iCol < allColNum - 1)
						{
							outFile<<"\t";
						}
					}
				}
				else
				{
					//没找到翻译行
					for (int iCol = 0; iCol < allColNum; ++iCol)
					{
						vecTarget[iBackUp][iCol] = vecBackUp[iBackUp][iCol];
						outFile << vecBackUp[iBackUp][iCol];
						if (iCol < allColNum - 1)
						{
							outFile<<"\t";
						}
					}
				}
				outFile<<"\n";
			}
			outFile.close();
			SQR_CATCH(exp)
			{
				string strMsg = "CUpgradeFinallyTxt生成txt文件:" + strSaveTxtName + "时发生错误,错误类型为:" + exp.ErrorTitle();
				MessageBox(0,strMsg.c_str(),NULL,0);
				exit(1);
			}
			SQR_TRY_END

#ifdef MULTILANG_VER_BEFORE_20110708
			string strSaveLuaIncName = strSaveBaseName + "Inc.lua";
			string strRequireName = GetRequireName(strSaveLuaIncName);
			SQR_TRY
			Write_toLua(strSaveLuaName,strSaveLuaIncName,strRequireName,vecTarget);
			SQR_CATCH(exp)
			{
				string strMsg = "CUpgradeFinallyTxt生成lua文件:" + strSaveLuaName + "时发生错误,错误类型为:" + exp.ErrorTitle();
				MessageBox(0,strMsg.c_str(),NULL,0);
				exit(1);
			}
			SQR_TRY_END
#endif
		}
		else
		{
			//复制文件（考虑到后面还要执行CopyLackFile，这块可以不执行）
			CopyFile(strFileName.c_str(), strSaveTxtName.c_str(), false);
#ifdef MULTILANG_VER_BEFORE_20110708
			string strLuaName = strBaseName + ".lua";
			CopyFile(strLuaName.c_str(), strSaveLuaName.c_str(), false);
			string strLuaIncName = strBaseName + "Inc.lua";
			string strSaveLuaIncName = strSaveBaseName + "Inc.lua";
			CopyFile(strLuaIncName.c_str(), strSaveLuaIncName.c_str(), false);
#endif
		}

		//复制文件（考虑到后面还要执行CopyLackFile，这块可以不执行）
		string strXmlName = strBaseName + ".xml";
		string strSaveXmlName = strSaveBaseName + ".xml";
		CopyFile(strXmlName.c_str(), strSaveXmlName.c_str(), false);
		
	}

}

void CUpgradeFinallyTxt::ReadUtf8Txt(ifstream& inFile, vector<vector<string> >& vecRef)
{
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
}

void CUpgradeFinallyTxt::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
{
	ifstream inFile(strFileName.c_str());
	if (!inFile)
		return;
	ReadUtf8Txt(inFile, vecRef);
	inFile.close();
}

void CUpgradeFinallyTxt::ReadUtf8TxtFromPkg(stringstream& inFile, vector<vector<string> >& vecRef)
{
	string str;
	while(getline(inFile,str))
	{
		if(str[str.size()-1] == 13)
			str.erase(str.size()-1);
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
}

void CUpgradeFinallyTxt::ReadUtf8TxtFromPkg(string& strFileName,vector<vector<string> >& vecRef)
{
	CPkgFile PkgFile;
	if( PkgFile.Open(L"lang", strFileName.c_str()) != eFE_SUCCESS )
		return;
		//GenErr(  "Can not open " + strFileName + "!\n" );
	char* pBuf = new char[PkgFile.Size() + 1];
	PkgFile.read( pBuf, PkgFile.Size() );
	pBuf[PkgFile.Size()] = '\0';
	//sqr::CBufFile File( pBuf, PkgFile.Size() );

	stringstream strsrc;
	strsrc << pBuf;
	string str;

	PkgFile.Close();

	ReadUtf8TxtFromPkg(strsrc, vecRef);

	delete [] pBuf;
}



void CUpgradeFinallyTxt::BuildColMap(const VecString& vecBase, const VecString& vecRefer, VecIndex& vecColMap)
{
	vecColMap.resize(vecBase.size(), -1);
	for(int i = 0; i < vecBase.size(); ++i)
	{
		for(int j = 0; j < vecRefer.size(); ++j)
		{
			if(vecRefer[j] == vecBase[i])
			{
				vecColMap[i] = j;
				break;
			}
		}
	}
}

string CUpgradeFinallyTxt::GetRequireName(string& strIncName)
{
	string strSearch = "table/";
	int pos = strIncName.find(strSearch.c_str());
	string strTemp = strIncName.substr(pos + strSearch.length(),strIncName.length()-pos-strSearch.length());
	int pos2 = strTemp.find(".lua");
	strTemp = strTemp.substr(0,pos2);
	return strTemp;
}

void CUpgradeFinallyTxt::Write_toLua(string strLuaName,string strLuaIncName,string strRequireName,vector<vector<string> >& vecRef)
{
	string strTableName = strLuaName;
	int pos = strTableName.rfind("/");
	strTableName.erase(0,pos+1);
	pos = strTableName.rfind(".");
	strTableName.erase(pos,4);

	SQR_TRY
		char szFileName[MAX_PATH];
	memset(szFileName, 0, sizeof(szFileName));
	strcpy_s(szFileName, sizeof(szFileName), strLuaName.c_str());
	const TCHAR* szSperate = max( strrchr( szFileName, '\\' ), strrchr( szFileName, '/' ) );
	if( szSperate )
		MakeDirectory( gbk_to_utf16(string( szFileName, szSperate - szFileName )).c_str() );
	ofstream luaStream(strLuaName.c_str());
	ofstream luaIncStream( strLuaIncName.c_str());
	string strTemp1 =  strTableName +  " = {}\n";
	luaIncStream<<strTemp1;

	vector<string> vecAttribute = vecRef[0];
	strTemp1 = "lan_require( \"" + strRequireName + "\" )\n";
	luaStream<<strTemp1;
	strTemp1 =  "local fun = function() error(\"attempt to update a read-only table\", 2) end\n";
	luaStream<<strTemp1;
	strTemp1 = "local tblName = {}\n";
	luaStream<<strTemp1;
	strTemp1 = "local setmt = setmetatable\n";
	luaStream<<strTemp1;
	strTemp1 = "local __tbl_index = \""+ vecAttribute[0] + "\"\n";
	luaStream<<strTemp1;
	strTemp1 = "local mt = {}\n";
	luaStream<<strTemp1;

	char tempArray[100];
	for (int i=0;i<(int)vecAttribute.size();++i)
	{
		string strTemp2;
		sprintf(tempArray," tblName.%s = %d\n",vecAttribute[i].c_str(),i+1);
		strTemp2 = tempArray;
		luaStream<<strTemp2;
	}

	string strTemp3 = "local __tbl = {\n";
	luaStream<<strTemp3;
	int row = vecRef.size();
	int col = vecRef[0].size();
	uint32 uIndex = 1;
	for(int i=1;i<row;++i)
	{
		string strTemp4 = "{";
		luaStream<<strTemp4;
		vector<string>::iterator vec_it = vecAttribute.begin();
		for(int j=0;j<col;++j)
		{
			string strTemp5 = vecRef[i][j];
			if (!j && !IsNumberString(strTemp5) || j)
				Change_toLuaFormat(strTemp5);
			luaStream<<strTemp5;
			if (j != col-1)
				luaStream<<",";
		}
		string strTemp6;
		if (i == row-1)
			strTemp6 = "}\n";
		else
			strTemp6 = "},\n";
		luaStream<<strTemp6;
		uIndex++;
	}

	string strTempEnd = "}\n";
	luaStream<<strTempEnd;

	char secondeArray[100];
	sprintf(secondeArray,"for i =1,%d do \n",uIndex-1);
	strTempEnd = secondeArray;
	luaStream<<strTempEnd;
	strTempEnd = "mt = {__index = function(table,key)  local j = i   return rawget(__tbl[j], tblName[key]) end }\n";
	luaStream<<strTempEnd;
	strTempEnd = "setmt(__tbl[i], mt )\n";
	luaStream<<strTempEnd;
	strTempEnd = "end\n";
	luaStream<<strTempEnd;

	sprintf(secondeArray,"for i =1,%d do \n",uIndex-1);
	strTempEnd = secondeArray;
	luaStream<<strTempEnd;
	sprintf(secondeArray,"%s[i] = {}\n",strTableName.c_str());
	strTempEnd = secondeArray;
	luaStream<<strTempEnd;
	strTempEnd = "mt  = {  __index = __tbl[i] , __newindex = fun } \n";
	luaStream<<strTempEnd;
	sprintf(secondeArray, "setmt( %s[i], mt )\n",strTableName.c_str());
	strTempEnd = secondeArray;
	luaStream<<strTempEnd;
	strTempEnd = "end\n";
	luaStream<<strTempEnd;
	strTempEnd = "__tbl.__tbl_index = __tbl_index \n";
	luaStream<<strTempEnd;
	strTempEnd = "mt  = {  __index = __tbl , __newindex = fun} \n";
	luaStream<<strTempEnd;
	sprintf(secondeArray,"setmt( %s, mt )\n",strTableName.c_str());
	strTempEnd = secondeArray;
	luaStream<<strTempEnd;
	luaStream.close();
	luaIncStream.close();
	SQR_CATCH(exp)
	{
		string strMsg = strLuaName + "在生成时发生错误" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END

}

bool CUpgradeFinallyTxt::IsNumberString(string& str)
{
	if (str.empty())
		return false;
	bool bNumber = true;
	for (int i=0;i<str.length();++i)
	{
		if (str[0] == '-')
			continue;
		if (str[i]<'0' || str[i]>'9')
		{
			bNumber = false;
			break;
		}
	}
	return bNumber;
}

void CUpgradeFinallyTxt::Change_toLuaFormat(string& str)
{
	replace_all_distinct(str, "\\", "\\\\",0);
	replace_all_distinct(str, "\"", "\\\"",0);
	str = "\"" + str + "\"";
}

#pragma warning(pop)

