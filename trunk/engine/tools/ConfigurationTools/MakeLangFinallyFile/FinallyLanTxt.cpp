#include "stdafx.h"
#include "FinallyLanTxt.h"
#include "CSplitInfo.h"
#include "common.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"
#include "CExpLog.h"

//#define CFG_VER_BEFORE_172921

#pragma warning(push)
#pragma warning(disable:4018)

#define CHUNKNAME "MakeLangFinallyFile"

string strGuiFolder = "";

CSplitInfo* CFinallyLanTxt::m_pSplitInfo = NULL;

CFinallyLanTxt::CFinallyLanTxt()
{
	m_pSplitInfo = CSplitInfo::GetInst();
	m_vecLanguage.push_back(CSplitInfo::GetInst()->GetOriLang());
	m_vecLanguage.push_back(CSplitInfo::GetInst()->GetForeLang());
	//m_vecLanguage.push_back(CSplitInfo::GetInst()->GetAutoConvLang());
	CRecordManager::SetChunkName(CHUNKNAME);
	strGuiFolder = strGuiFolder + "/" + CSplitInfo::GetInst()->GetGuiFolder() + "/";
}

CFinallyLanTxt::~CFinallyLanTxt()
{
}

void CFinallyLanTxt::StartProcess()
{
	CConsoleInfo::GetInst()->PrintFunction("语言中立表txt转utf8_txt及utf8_lua");
	//string strPath = "../../cfg/data/src/language/";
	string strPath = CSplitInfo::GetInst()->GetLanguageTempPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), Txt2TgtTxt, NULL);
}

FTW_RESULT CFinallyLanTxt::Txt2TgtTxt( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
{
	int nLen = wcslen( szFileName );
	if( !_wcsicmp( szFileName + nLen - 4, L".svn" ) && eFlag == _FTW_DIR )
		return _FTW_IGNORE;
	if( eFlag != _FTW_FILE || nLen <= 4 )
		return _FTW_CONTINUNE;	
	if( _wcsicmp( szFileName + nLen - 4, L".txt" ) )
		return _FTW_CONTINUNE;
	string strFileName = utf16_to_gbk(szFileName);
	if(!CRecordManager::GetInst()->BeChanged(strFileName))
		return _FTW_CONTINUNE;
	CFinallyLanTxt* pManager = CFinallyLanTxt::GetInst();
	string strOldName = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strOldName.c_str());
	pManager->CreateFinallyTxt(strFileName);
//#ifdef CFG_VER_BEFORE_172921
	if(m_pSplitInfo->GetNeedLuaFiles())
		pManager->CreateFinallyLua(strFileName);
//#endif
	CConsoleInfo::GetInst()->EndToPrint();
	return _FTW_CONTINUNE;
}

void CFinallyLanTxt::CreateFinallyTxt(string& strFileName)
{
	vector<vector<string> > vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);
	if(strFileName.find(strGuiFolder) == string::npos)
	{
		FormatIndex(vecBackUp);
	}
	string strSaveName = MakeSavePath(strFileName);
	if(strSaveName.empty())
		return;
	CreateFileDirectory(strSaveName);
	SQR_TRY
	ofstream outFile(strSaveName.c_str());
	if (!outFile)
	{
		string strMsg = strSaveName + "在打开时发生错误，请检查文件是否可写";
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
	int allRowNum = vecBackUp.size();
	int allColNum = vecBackUp[0].size();
	for (int i=0;i<allRowNum;++i)
	{
		for (int j=0;j<allColNum;++j)
		{
			if(vecBackUp[i].empty() || vecBackUp[i].size() == 1 && vecBackUp[i][0].empty())
				continue;
			CleanExcelDifQuotationToSimpleTxt(vecBackUp[i][j]);
			outFile<<vecBackUp[i][j];
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
		string strMsg = "CFinallyLanTxt生成txt文件:" + strSaveName + "时发生错误,错误类型为:" + exp.ErrorTitle();
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
	vector<string> vecChild;
	vecChild.push_back(strSaveName);
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (int i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
}

void CFinallyLanTxt::CreateFinallyLua(string& strFileName)
{
	string strSaveName = MakeSavePath(strFileName);
	if(strSaveName.empty())
		return;
	string strLua = replace_all_distinct(strSaveName,".txt",".lua",0);
	string strLuaInc = strLua;
	strLuaInc = replace_all_distinct(strLuaInc,".lua","Inc.lua",0);
	string strRequireName = GetRequireName(strLuaInc);
	SQR_TRY
	vector<vector<string> > vecBackUp;
	ReadUtf8Txt(strFileName,vecBackUp);
	if(strFileName.find(strGuiFolder) == string::npos)
	{
		FormatIndex(vecBackUp);
	}
	CleanExcelFormat(vecBackUp);
	Write_toLua(strLua,strLuaInc,strRequireName,vecBackUp);
	SQR_CATCH(exp)
	{
		string strMsg = "CFinallyLanTxt生成lua文件:" + strLua + "时发生错误,错误类型为:" + exp.ErrorTitle();
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

CFinallyLanTxt::ECharacterType CFinallyLanTxt::AdjustCharacterType(char c[2])
{
	if(c[0] == 9 && c[1] == 0)
		return eCT_Table;
	else if (c[0] == 13 && c[1] == 0)
		return	eCT_Enter;
	else
		return eCT_Normal;
}

void CFinallyLanTxt::ReadUtf16Txt(string& strFileName,vector<vector<wstring> >& vecRef)
{
	SQR_TRY
	FILE* pFile;
	pFile = fopen(strFileName.c_str(),"rb");
	fseek(pFile,0,SEEK_END);
	int fileLength = ftell(pFile);
	char* fileArray = new char[fileLength];
	fseek(pFile,0,SEEK_SET);
	fread(fileArray,1,fileLength,pFile);
	fclose(pFile);
	vecRef.clear();
	ECharacterType eCType;
	int iCurRow = 1;
	char charArray[2];
	wstring wstrResult;
	wstring wstrTemp;
	int i=2;
	while (i<fileLength)
	{
		charArray[0] = fileArray[i];
		charArray[1] = fileArray[i+1];
		eCType = AdjustCharacterType(charArray);
		if ((int)vecRef.size()<iCurRow)
		{
			vecRef.push_back(vector<wstring>());
		}
		switch(eCType)
		{
		case eCT_Normal:
			wstrTemp.resize(1);
			memcpy((void*)(wstrTemp.c_str()),charArray,2);
			wstrResult = wstrResult + wstrTemp;
			break;
		case eCT_Enter:
			vecRef[iCurRow-1].push_back(wstrResult);
			wstrResult = L"";
			iCurRow++;
			i = i+2;
			break;
		case eCT_Table:
			vecRef[iCurRow-1].push_back(wstrResult);
			wstrResult = L"";
			break;
		}
		i = i+2;
	}
	SQR_CATCH(exp)
	{
		string strMsg = strFileName + "在读取文件后转换时发生错误" + exp.ErrorTitle();
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

void CFinallyLanTxt::ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef)
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

void CFinallyLanTxt::FormatIndex(vector<vector<string> >& vecRef)
{
	for (int i=1;i<vecRef.size();++i)
	{
		string strIndex = vecRef[i][0];
		if (strIndex.find('_') != -1)
		{
			string strResult;
			int pos = -1;
			while ( (pos = strIndex.find('_')) != -1)
			{
				string strTemp = strIndex.substr(0,pos);
				strResult = strResult + MemH64_to_mbcs(strTemp);
				strIndex = strIndex.substr(pos+1,strIndex.length()-pos-1);
			}
			strResult = strResult + MemH64_to_mbcs(strIndex);
			strResult = mbcs_to_MemH64(strResult);
			if (IsNumberString(strResult))
			{
				strResult = strResult;
			}
			vecRef[i][0] = strResult;
		}
	}
}

void CFinallyLanTxt::CleanExcelTxtQuotation(string& str)
{
	if (str.find(',',0) != -1 || str.find("\"\"",1) != -1)
	{
		CleanOnePairQuotation(str);
	}
	if(str.find("\"\"",0) != -1)
	{
		replace_all_distinct(str,"\"\"","\"",0);
	}
}

void CFinallyLanTxt::CleanOnePairQuotation(string& str)
{
	if(str != "" && str[0] == '\"' && str[str.length()-1] == '\"')
	{
		str.erase(str.length()-1,1);
		str.erase(0,1);
	}
}

void CFinallyLanTxt::CleanExcelFormat(vector<vector<string> >& vecRef)
{
	vector<string>& vecAttribute = vecRef[0];
	uint32 uIndex = 1;
	vector<vector<string>>::iterator it = vecRef.begin()+1;
	vector<string>::iterator vec_it_title = vecAttribute.begin();
	for(;it != vecRef.end();++it)
	{
		if( (*it).empty() )
			continue;	
		vector<string>::iterator vec_it = vecAttribute.begin();
		uint32 vecIndex = 0;
		for(;vec_it != vecAttribute.end();++vec_it)
		{
			string sText;
			if (vecIndex<it->size())
			{
				sText = (*it)[vecIndex];
			}
			CleanExcelTxtQuotation(sText);
			(*it)[vecIndex] = sText;
			vecIndex++;
		}
	}
}

bool CFinallyLanTxt::IsNumberString(string& str)
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

string CFinallyLanTxt::MakeSavePath(string& strFileName)
{
	string strResult = "";
	string strTemp;
	string strLangPath = m_pSplitInfo->GetLangTablePath();
	if (BeExistLangPath(strFileName))
	{
		string strLanguage = GetLanguage(strFileName);
		const string& strOriLang = CSplitInfo::GetInst()->GetOriLang();
		strLangPath = replace_all_distinct(strLangPath, strOriLang, strLanguage, 0);
		int pos = strFileName.find(strLanguage.c_str());
		string str  = strFileName.substr(pos+strLanguage.length(),strFileName.length()-pos-strLanguage.length());
		strResult = strLangPath + "/" + str;
	}
	else
	{
		const string& strSub = CSplitInfo::GetInst()->GetLangSrcFolder();
		int pos = strFileName.find(strSub.c_str());
		if(pos != string::npos)
		{
			string strTemp = strFileName.substr(pos+strSub.length(),strFileName.length()-pos-strSub.length());
			strResult = strLangPath + strTemp;
		}
	}
	return strResult;
}

bool CFinallyLanTxt::BeExistLangPath(string& strFileName)
{
	bool bResult = false;
	for(int i = 0;i<m_vecLanguage.size();++i)
	{
		if (strFileName.find(m_vecLanguage[i]) != -1)
		{
			bResult = true;
			break;
		}
	}
	return bResult;
}

string CFinallyLanTxt::GetLanguage(string& strFileName)
{
	if (BeExistLangPath(strFileName))
	{
		for(int i = 0;i<m_vecLanguage.size();++i)
		{
			if (strFileName.find(m_vecLanguage[i]) != -1)
			{
				return m_vecLanguage[i];
			}
		}
	}
	return CSplitInfo::GetInst()->GetOriLang();
}

void CFinallyLanTxt::CleanExcelDifQuotationToSimpleTxt(string& str)
{
	replace_all_distinct(str, "\"\"", "\"",1);
	CleanOnePairQuotation(str);
}

string CFinallyLanTxt::GetRequireName(string& strIncName)
{
	string strSearch = "table/";
	int pos = strIncName.find(strSearch.c_str());
	string strTemp = strIncName.substr(pos + strSearch.length(),strIncName.length()-pos-strSearch.length());
	int pos2 = strTemp.find(".lua");
	strTemp = strTemp.substr(0,pos2);
	return strTemp;
}

void CFinallyLanTxt::Change_toLuaFormat(string& str)
{
	replace_all_distinct(str, "\\", "\\\\",0);
	replace_all_distinct(str, "\"", "\\\"",0);
	str = "\"" + str + "\"";
}

void CFinallyLanTxt::Write_toLua(string strLuaName,string strLuaIncName,string strRequireName,vector<vector<string> >& vecRef)
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
		CreateFileDirectory(string( szFileName, szSperate - szFileName ));
		//MakeDirectory( gbk_to_utf16(string( szFileName, szSperate - szFileName )).c_str() );
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


#pragma warning(pop)
