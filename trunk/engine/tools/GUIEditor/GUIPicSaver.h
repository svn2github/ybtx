#pragma once
#include "stdafx.h"
#include "GUIXmlResSaver.h"

class GUIPicSaver : public GUIXmlResSaver
{
public:
	GUIPicSaver();
	~GUIPicSaver();

	typedef set<string> StrSet;
	typedef map<Res_type, StrSet> StrSetMap;

	void	AddXmlResData(Res_type rt, string& str);
	void	DelXmlResData(Res_type rt, string& str);
	void	ClearXmlResData(Res_type rt);

	bool	Load(wstring& path);
	void	Save();

	void	ProcessSvnFile();
	void	DeleteSvnFile(string file);
	void	DeleteFile();
	set<string>& GetAllTex();
private:
	StrSetMap	m_PicRes;
	wstring		m_ResPath;
	bool		m_bLoaded;
	StrSet		m_AllTex;
	StrSet		m_DelFile;
	StrSet		m_DelFileFailed;
};