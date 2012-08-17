#pragma once
#include "TSingleton.h"
//#include <string>
//#include <vector>
//#include "CPathMgr.h"
//
//using namespace std;

typedef vector<string>	VecString;
typedef vector<int>		VecIndex;

class CSplitInfo;

//namespace sqr
//{
	class CUpgradeFinallyTxt
		:public TSingleton<CUpgradeFinallyTxt>
	{
	public:
		CUpgradeFinallyTxt();
		~CUpgradeFinallyTxt();
		void StartProcess();
		static FTW_RESULT TxtUpgrade( const wchar_t* szFileName, const struct _stat *, FTW_FLAG eFlag, void* );
		void CreateFinallyFile(string& strFileName);
		void ReadUtf8Txt(ifstream& inFile, vector<vector<string> >& vecRef);
		void ReadUtf8Txt(string& strFileName,vector<vector<string> >& vecRef);
		void ReadUtf8TxtFromPkg(stringstream& inFile, vector<vector<string> >& vecRef);
		void ReadUtf8TxtFromPkg(string& strFileName,vector<vector<string> >& vecRef);
		void BuildColMap(const VecString& vecBase, const VecString& vecRefer, VecIndex& vecColMap);
		string GetRequireName(string& strIncName);
		void Write_toLua(string strLuaName,string strLuaIncName,string strRequireName,vector<vector<string> >& vecRef);
		bool IsNumberString(string& str);
		void Change_toLuaFormat(string& str);

	protected:
		CSplitInfo* m_pSplitInfo;
		vector<string> m_vecLanguage;
	};
//
//}

