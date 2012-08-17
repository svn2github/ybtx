#pragma once

void PrintUsage();

void PrintMessage(const char* msg);

bool IsDir(const char* szFilePath);

bool IsFile(const char* szFileName);

void GetBaseName(const char* szFileName, string& strBaseName);

class CatFileNameData
{
public:
	std::string		m_strFileName;
	std::string		m_strBaseName;
	uint32			m_uVersion;
	std::string		m_strGuid;
	std::string		m_strUserName;
	std::string		m_strOutFile;
	bool			m_bGac;
	bool			m_bInfo;
};

int CheckFileName(CatFileNameData& data);

int CatFile(CatFileNameData& data);

int HandleDir(const char* szInput, const char* szOutput);
int HandleFile(const char* szInput, const char* szOutput);
