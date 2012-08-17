#pragma once
#include "stdafx.h"
#include "TSingleton.h"

class CConsoleInfo : public TSingleton<CConsoleInfo>
{
public:
	CConsoleInfo();
	~CConsoleInfo();
	void BeginToPrint(const char* szFileName);
	void EndToPrint();
	void PrintFunction(const string& strFun);
	void DrawBox(SMALL_RECT &rc);
	bool BeXpSystem();

private:
	uint64 m_uStartTime;
	HANDLE m_hOut;
};
