#pragma once
#include "TSingleton.h"
#include "CSplitInfo.h"
#include "stdafx.h"

class CQuestRedundancyResolver : public TSingleton<CQuestRedundancyResolver>
{
public:
	CQuestRedundancyResolver();
	~CQuestRedundancyResolver();
	static FTW_RESULT ResolveRedundancy( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT ResolveDesc( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	bool BeInDescResolveList( string& strFileName );
	bool BeInResolveList( string& strFileName);
	string GetFileNameFromPath( const string& strFileName );
	void DoResolve( string& strFileName );
	void DoResolveDesc( string& strFileName );
	void WriteToQuestStringFile( string& strQuestStrFile );

private:
	CSplitInfo* m_pSplitInfo;
	static vector<string> s_vecDialog;
};
