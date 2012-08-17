#pragma once
#include "TSingleton.h"

class CLackFileCopy : public TSingleton<CLackFileCopy>
{
public:
	void StartProcess();
	static FTW_RESULT WalkCopyFile( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT WalkCheckFileIndependently( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT WalkCheckFileContrastively( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	bool StartCopy(string& strFileName);
	void CheckFileIndependently(string& strFileName);
	void CheckFileContrastively(string& strFileName);
	void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
};
