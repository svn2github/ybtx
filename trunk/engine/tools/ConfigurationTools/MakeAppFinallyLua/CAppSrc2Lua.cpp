#include "stdafx.h"
#include "CAppSrc2Lua.h"
#include "CSplitInfo.h"
#include "common.h"
#include "COriFileOperate.h"
#include "CRecordManager.h"
#include "ConsoleInfo.h"


#define CHUNKNAME "MakeAppFinallyLua"

CDif2Lua::CDif2Lua()
:m_bLang(false)
{
	m_pSplitInfo = CSplitInfo::GetInst();
	CRecordManager::SetChunkName(CHUNKNAME);
}

CDif2Lua::~CDif2Lua()
{

}

void CDif2Lua::StartProcess()
{
	wstring wstrPostfix = CSplitInfo::GetInst()->WGetLuaCfgExtend();
	string strPostfix = utf16_to_gbk(wstrPostfix);
	strPostfix.erase(strPostfix.begin());
	strPostfix = strPostfix + "转lua";
	CConsoleInfo::GetInst()->PrintFunction(strPostfix);
	string strPath = m_pSplitInfo->GetSrcPath();
	wstring wstrPath = gbk_to_utf16(strPath);
	FileTreeWalk(wstrPath.c_str(), Dif2Lua, NULL);
}

 FTW_RESULT CDif2Lua::Dif2Lua( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* )
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
	 CDif2Lua* pManager = CDif2Lua::GetInst();
	 if (pManager->BeRequireToLua(strFileName) && CRecordManager::GetInst()->BeChanged(strFileName))
	 {
		 pManager->Change2Lua(strFileName);
	 }
	 return _FTW_CONTINUNE;
 }

void CDif2Lua::Change2Lua(string& strFileName)
{
	string strNameWithoutPath = GetFileNameFromPath(strFileName);
	CConsoleInfo::GetInst()->BeginToPrint(strNameWithoutPath.c_str());
	m_bLang = false;
	COriFileOperate oriFile(strFileName);
	oriFile.CleanBlanRow();
	oriFile.CleanNoteRow();
	oriFile.CleanBlankCol();
	string strLuaName = GetLuaFileName(strFileName);
	string strLuaIncName = GetLuaIncFileName(strFileName);
	string strRequireName = GetRequireStr(strFileName);
	string strTableName = strLuaName;
	int pos = strTableName.rfind("/");
	strTableName.erase(0,pos+1);
	pos = strTableName.rfind(".");
	strTableName.erase(pos,4);

	vector<string> vecChild;
	vecChild.push_back(strLuaName);
	vecChild.push_back(strLuaIncName);

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

	vector<string> vecAttribute;
	oriFile.GetOneRow(vecAttribute,0);

	if (m_bLang)
	{
		strTemp1 = "lan_require( \"" + strRequireName + "\" )\n";
	}
	else
	{
		strTemp1 = "cfg_require( \"" + strRequireName + "\" )\n";
	}
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
	int row = oriFile.GetRowCount();
	int col = oriFile.GetColCountByRow(0);
	uint32 uIndex = 1;
	for(int i=1;i<row;++i)
	{
		if(!oriFile.GetColCountByRow(i))
			continue;			
		string strTemp4 = "{";
		luaStream<<strTemp4;
		vector<string>::iterator vec_it = vecAttribute.begin();
		for(int j=0;j<col;++j)
		{
			string strTemp5 = oriFile.GetDataByRowCol(i,j);
			if (!oriFile.BeNumber(i,j))
				ChangeWithLuaFormat(strTemp5);
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
		string strMsg = "CDif2Lua生成lua文件:" + strLuaName + "时发生错误,错误类型为:" + exp.ErrorTitle();
		MessageBox(0,strMsg.c_str(),NULL,0);
		exit(1);
	}
	SQR_TRY_END
	CRecordManager::GetInst()->AddFileRecord(strFileName);
	CRecordManager::GetInst()->AddFileChildRecord(strFileName,vecChild);
	for (size_t i=0;i<vecChild.size();++i)
	{
		CRecordManager::GetInst()->AddFileRecord(vecChild[i]);
	}
	CConsoleInfo::GetInst()->EndToPrint();
}

void CDif2Lua::ChangeWithLuaFormat(string&str)
{
	replace_all_distinct(str, "\\", "\\\\",0);
	replace_all_distinct(str, "\"", "\\\"",0);
	str = "\"" + str + "\"";
}

string CDif2Lua::GetLuaFileName(string& strFileName)
{
	string strLuaName;
	int bigPos = max(int(strFileName.rfind('\\')),int(strFileName.rfind('/')));
	string strTgt = m_pSplitInfo->GetTgtPath();
	string strSrc = m_pSplitInfo->GetSrcPath();
	strLuaName = strTgt + strFileName.substr(strSrc.size(),strFileName.size()-strSrc.size());
	strLuaName.erase(strLuaName.size() - 4, 4 ) += ".lua";
	return strLuaName;
}

string CDif2Lua::GetLuaIncFileName(string& strFileName)
{
	string strLuaIncName = GetLuaFileName(strFileName);
	strLuaIncName.erase(strLuaIncName.size() - 4, 4 ) += "Inc.lua";
	return strLuaIncName;
}

string CDif2Lua::GetRequireStr(string& strFileName)
{
	string strRequire;
	string strLuaIncName = GetLuaIncFileName(strFileName);
	string strTgt = m_pSplitInfo->GetTgtPath();
	const TCHAR* szInc = strLuaIncName.c_str()+strTgt.size()+1;
	strRequire = szInc;
	strRequire.erase(strRequire.size() - 4, 4 );
	return strRequire;
}

bool CDif2Lua::BeRequireToLua(const string& strFileName)
{
	string strName = GetFileNameFromPath(strFileName);
	if (m_pSplitInfo->BeInSplitList(strName) || m_pSplitInfo->BeLangDif(strFileName))
		return false;
	return true;
}


