#include "stdafx.h"
#include "TxtTableFile.h"

TABLEFILE_HANDLE& GetTableFileHandle( void* hIniFile )
{
	return *((TABLEFILE_HANDLE*)hIniFile);
}

CTxtTableFile::CTxtTableFile() : m_hFileHandle(new TABLEFILE_HANDLE)
{
}

CTxtTableFile::~CTxtTableFile()
{
	Clear();
	delete ((TABLEFILE_HANDLE*)m_hFileHandle);
}

bool CTxtTableFile::Load( const char *szFileName )
{
	Clear();
	TABLEFILE_HANDLE& TableStruct = GetTableFileHandle( m_hFileHandle );
	//FILE* pFile = fopen(szFileName, "rb");
	FILE* pFile = fopen(szFileName, "rb");
	if (!pFile)
	{
		return false;
	}
	fseek(pFile, 0, SEEK_END);
	long uLength = ftell(pFile);
	fseek(pFile, 0, SEEK_SET);
	TableStruct.m_vecBuf.resize(uLength);
	fread(&TableStruct.m_vecBuf[0], sizeof(char), uLength, pFile);

	TableStruct.m_nOffsetByIndex.push_back( vector<size_t>() );
	size_t nOffset=0;
	for (size_t i=0; i<TableStruct.m_vecBuf.size(); i++)
	{
		size_t uCurLine = TableStruct.m_nOffsetByIndex.size() - 1;
		if (TableStruct.m_vecBuf[i] == 0x0a
			|| (TableStruct.m_vecBuf[i]==0x0d && TableStruct.m_vecBuf[i+1]==0x0a)
			|| TableStruct.m_vecBuf[i]==0x09)
		{
			char cFlag = TableStruct.m_vecBuf[i];
			TableStruct.m_vecBuf[i] = 0;
			if (!uCurLine)
				TableStruct.m_nOffsetByName[&TableStruct.m_vecBuf[0] + nOffset] = TableStruct.m_nOffsetByIndex[0].size();

			TableStruct.m_nOffsetByIndex[uCurLine].push_back(nOffset);

			if (cFlag == 0x09)
				nOffset = i + 1;
			else if (cFlag == 0x0a)
			{ 
				nOffset = i + 1;
				if (nOffset < TableStruct.m_vecBuf.size())
					TableStruct.m_nOffsetByIndex.push_back(vector<size_t>());
				else
					return true;
			}
			else
			{
				nOffset = i++ + 2;
				if (nOffset < TableStruct.m_vecBuf.size())
					TableStruct.m_nOffsetByIndex.push_back(vector<size_t>());
				else
					return true;
			}
		}
	}
	TableStruct.m_nOffsetByIndex[TableStruct.m_nOffsetByIndex.size()-1].push_back(nOffset);
	TableStruct.m_vecBuf.push_back(0);
	return true;
}

void CTxtTableFile::Clear()
{
	TABLEFILE_HANDLE& TableStruct = GetTableFileHandle( m_hFileHandle );
	TableStruct.m_vecBuf.clear();
	TableStruct.m_nOffsetByIndex.clear();
	TableStruct.m_nOffsetByName.clear();
}

// 得到行数
int CTxtTableFile::GetHeight()
{
	return (int)GetTableFileHandle(m_hFileHandle).m_nOffsetByIndex.size();
}

//得到列数
int CTxtTableFile::GetWidth()
{
	return (int)GetTableFileHandle(m_hFileHandle).m_nOffsetByIndex[0].size();
}

// 根据列号得到某行某列
const char* CTxtTableFile::GetString( int nRow, int nColumn )
{
	TABLEFILE_HANDLE& TableStruct = GetTableFileHandle(m_hFileHandle);
	if ((size_t)nRow<TableStruct.m_nOffsetByIndex.size() && (size_t)nColumn<TableStruct.m_nOffsetByIndex[nRow].size())
		return &TableStruct.m_vecBuf[TableStruct.m_nOffsetByIndex[nRow][nColumn]];
	return "";
}

// 根据列名得到某行某列
const char* CTxtTableFile::GetString( int nRow, const char* szColumnName )
{
	return GetString(nRow, (int)GetTableFileHandle(m_hFileHandle).m_nOffsetByName[szColumnName]);
}
