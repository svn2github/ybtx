#pragma once
#include "TSingleton.h"
#include "CSplitInfo.h"

class CCommonSeqReference : public TSingleton<CCommonSeqReference>
{
public:
	CCommonSeqReference(void);
	~CCommonSeqReference(void);
	static FTW_RESULT WriteItemDescToVector( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	static FTW_RESULT WriteQuestDescToVector( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
	void ProduceDescCommSeq();
	void ProduceRefrence();
	void OutputDescCommSeq( const char* szFileName );
	void OutputRefrence(const char* szFileName );

	void BeginItemDesc();
	void BeginQuestDesc();
	void CleanMemData();

private:
	bool BeInResolveList( const string& strFileName );
	void DoWriteItemDesc( const string& strFileName );
	void DoWriteQuestDesc( const string& strFileName );
	uint32 GetCommDescIndex( const string& strCommDesc);
	string GetProject( string& strFileName );
	string GetKeyPrefix( string& strFileName );
	void ReadReferneceTxt();
	void ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef );
	void ReplaceWildcard(string& str);

private:
	struct ReferenceInfor 
	{
		string m_strProjekt;
		string m_strPreFix;
	};
	CSplitInfo* m_pSplitInfo;
	hash_multimap<string, string> s_hmapDesc2SrcIndex;
	hash_map<string, string> s_hmapSrcIndex2FileName;
	hash_map<string, uint32> s_hmapDesc2CommSeqPos;
	hash_map<string, uint32> s_hmapSrcIndex2CommSeqIndex;
	vector<string> s_vecDesc;
	vector<string> s_vecDescCommSeq;
	hash_map<uint32, string> s_hmapCommSeq;
	map<string,ReferenceInfor> m_mapReference;
	string m_strCommonFileName;
};
