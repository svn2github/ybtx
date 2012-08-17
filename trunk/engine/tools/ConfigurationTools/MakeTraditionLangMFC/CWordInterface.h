#pragma once
#include "stdafx.h"
#include "TSingleton.h"
#include "CApplication.h"
#include "CDocuments.h"
#include "CDocument0.h"
#include "CSelection.h"

class CWordInterface
	:public TSingleton<CWordInterface>
{
public:
	CWordInterface();
	~CWordInterface();
	//test();
	//static test();
	bool ConvertSCTC(LPCTSTR szSrc, TCHAR*& szTgt, size_t& uSize);
	void Release();

private:
	CApplication	m_wordApp;
	CDocuments		m_wordDocs;
	CDocument0		m_wordDoc;
	CSelection		m_wordSelection;
	COleDispatchDriver m_wordBasic;

	OLECHAR FAR*	m_szMethod[4];
	DISPID			m_dispid[4];
	DISPPARAMS		m_dp;
	VARIANT			m_vArgs[3];
};