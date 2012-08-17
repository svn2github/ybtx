// TestFsPerf.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

static void PrintUsage();

static const TCHAR* GetDataFromArgs(int nArgCount, TCHAR* aryArg[], const TCHAR* szOption);
FTW_RESULT FileOperate(const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* countOfFiles);

int main(int argc, TCHAR* argv[])
{
	if(argc < 2)
	{
		PrintUsage();
		return 0;
	}
	string Path = argv[2];
	const TCHAR* szCountOfFiles = GetDataFromArgs(argc, argv, "-n");
	const TCHAR* szCountOfBytes = GetDataFromArgs(argc, argv, "-r");
	int countOfFiles = szCountOfFiles ? atoi(szCountOfFiles) : 0;
	int countOfBytes = szCountOfBytes ? atoi(szCountOfBytes) : 0;
	if(countOfFiles<0 || countOfBytes<0)
	{
		PrintUsage();
		return 0;
	}
//	const wchar_t* szDir = gbk_to_utf16(Path).c_str();
	int Data[2];
	Data[0] = countOfFiles;
	Data[1] = countOfBytes;
	DWORD stime = GetTickCount();
	FileTreeWalk( gbk_to_utf16(Path).c_str(), FileOperate, Data );
	DWORD etime = GetTickCount();
	cerr<<etime - stime<<" ms"<<endl;
	system("pause");
}

static void PrintUsage()
{
	cerr<<"Usage: TestFsPerf Path [Options Data]"<<endl
		<<"  Options:"<<endl
		<<"    -n		Number of test files, greater than 0"<<endl
		<<"    -r		How many bytes to read, greater than 0"<<endl;
	system("pause");
}

static const TCHAR* GetDataFromArgs(int nArgCount, TCHAR* aryArg[], const TCHAR* szOption)
{
	for(int i=1;i<nArgCount;++i)
	{
		if(strcmp(szOption,aryArg[i])==0)
			return aryArg[i+1];
	}
	return NULL;
}

FTW_RESULT FileOperate( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* Data )
{
	static int s_count = 0;
	int countOfFiles = ((int*)Data)[0];
	int countOfBytes = ((int*)Data)[1];	
	//cerr<<s_count<<endl;
	if(countOfFiles>0 && s_count == countOfFiles)
		return _FTW_STOP;
	if(eFlag == _FTW_FILE)
	{
		HANDLE hfile = CreateFileW((wstring(szFileName).c_str()), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		if(countOfBytes>0)
		{
			char buffer[1025];
			DWORD nBytesRead;
			BOOL b = ReadFile(hfile, buffer, countOfBytes, &nBytesRead, NULL);
			//cerr<<b<<endl;
		}
		++s_count;
	}
	return _FTW_CONTINUNE;
}