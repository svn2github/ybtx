#pragma once
#include "stdafx.h"

class CDifInfo
{
public:
	CDifInfo(const char* szFileName);
	~CDifInfo();
	bool BeMultiRowTable();

private:
	string GetXmlFileName();

private:
	char* m_szFileName;
};
