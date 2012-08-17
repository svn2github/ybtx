#include "stdafx.h"
#include "CFileWatcherData.h"
#include "BaseHelper.h"

CFileWatcherData::CFileWatcherData()
{

}

CFileWatcherData::CFileWatcherData(const CFileWatcherData& data)
{
	size_t stSize = data.m_vecFileData.size();
	for (size_t i = 0; i <stSize; i++)
	{
		m_vecFileData.push_back(data.m_vecFileData[i]);
	}
	m_bLuaFile = data.m_bLuaFile;
}

CFileWatcherData::~CFileWatcherData()							
{
	m_vecFileData.clear();
}

const char* CFileWatcherData::GetFileName(uint32 uIndex)
{
	return m_vecFileData[uIndex].c_str();
}

uint32 CFileWatcherData::GetCount()
{
	return (uint32)m_vecFileData.size();
}


void CFileWatcherData::PushBack(const string& sFileName)
{
	m_vecFileData.push_back(sFileName);
}

void CFileWatcherData::Pop(const string& sFileName)
{
	vector<string>::iterator itr = m_vecFileData.begin();
	for (; itr != m_vecFileData.end(); ++itr)
	{
		if (*itr == sFileName)
		{
			m_vecFileData.erase(itr);
			return;
		}
	}
}

bool CFileWatcherData::Empty()
{
	return m_vecFileData.empty();
}

void CFileWatcherData::SetIsLuaFile(bool bIsLuaFile)
{
	m_bLuaFile = bIsLuaFile;
}

bool CFileWatcherData::GetIsLuaFile()
{
	return m_bLuaFile;
}

void CFileWatcherData::Release()
{
	delete this;
}
