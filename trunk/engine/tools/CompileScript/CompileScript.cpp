// CompileLua.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CodeCvs.h"


void ManageAllPath(string lpTempName);
void CompileLua(const char* strFileName);
FTW_RESULT ParserFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *pNull );


int main(int argc, TCHAR* argv[])
{
	int len = 0;
	string ConfigFile("../../etc/common/SearchDir.xml");
	ifstream ifs;
	ifs.open (ConfigFile.c_str());


	ifs.seekg(0,ios::end); // ios::cur
	len = ifs.tellg();
	ifs.seekg(0,ios::beg);

	string FileContent;
	FileContent.resize(len);

	ifs.read(&FileContent[0], len);
	//cout <<FileContent<<"\n\n"<<endl;

	size_t iStart = 0;
	size_t iEnd = 0;
	size_t iRealStart = 0;
	size_t iRealEnd = 0;
	while ((iStart = FileContent.find("<path>", iEnd+7)) != string::npos)
	{
		iRealStart = iStart + 6; // 加6是跳过<path>6个字符
		iEnd   = FileContent.find("</path>", iEnd+7);
		iRealEnd = iEnd - 6; // 跳过 /?.lua 6个字符
		
		string strPath("../../");
		strPath += FileContent.substr(iRealStart, iRealEnd-iRealStart);
		FileTreeWalk( utf8_to_utf16(strPath).c_str(), ParserFile, NULL );
	}

	return 0;
}




FTW_RESULT ParserFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void *pNull )
{
#ifdef _WIN32
	if( eFlag == _FTW_DIR && !_wcsicmp( szFileName + wcslen( szFileName ) - 4, L".svn" ) )
		return _FTW_IGNORE;

	if( eFlag == _FTW_FILE && _wcsicmp( szFileName + wcslen( szFileName ) - 4, L".lua" )==0) // 处理文件
		CompileLua(utf16_to_utf8(szFileName).c_str());

	return _FTW_CONTINUNE;
#else
	
	if( eFlag == _FTW_DIR && !_stricmp( utf16_to_utf8(szFileName + wcslen( szFileName ) - 4).c_str(), ".svn" ) )
		return _FTW_IGNORE;

	if( eFlag == _FTW_FILE && _stricmp( utf16_to_utf8(szFileName + wcslen( szFileName ) - 4).c_str(), ".lua" )==0) // 处理文件
		CompileLua(utf16_to_utf8(szFileName).c_str());

	return _FTW_CONTINUNE;
#endif
}


void CompileLua(const char* strFileName)
{
	char *sFileName = (char*)strFileName;
	char *szArgv[3] = { "", "-p", sFileName };
	CScript::Compile(3, szArgv);
}
