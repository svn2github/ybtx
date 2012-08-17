#pragma once
#include "TSingleton.h"

class CGUIStaticText : public TSingleton<CGUIStaticText>
{
public:
	CGUIStaticText();
	void ReadUtf8Txt(const string& strFileName,vector<vector<string> >& vecRef);
	void ReadUtf16Txt(const string& strFileName,vector<vector<wstring> >& vecRef);
	void WriteUtf8ToUtf16(const string& strFileName,vector<vector<string> >& vecRef);
	void WriteUtf16ToUtf8(const string& strFileName,vector<vector<wstring> >& vecRef);
	void ConverseToGermanLangSrc();
	void ConverseToGermanLangTgt();
	string GetPatchName(string strName);
private:
	
	bool	m_btoUtf16;
	string m_strChinesePath;
	string m_strDestPath;
	string m_strFile;
	bool	m_bEnablePatch;
};
