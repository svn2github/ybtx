#include "stdafx.h"
#include "CMapFile.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <algorithm>
#include "TimeHelper.h"
#include "UtfCvs.h"
#include "CMallocPool.h"
#include <algorithm>

using namespace std;


//最大内存容量，如果大于这个值了，我们就不创建了
const uint32 UNIT_SYMBOL_BUFFER = 1024 * 1024 * 10;  //10mb

CMapFile::CMapFile()
{
	m_pSymbolPool = new CMallocPool(10240);
}

CMapFile::~CMapFile()
{
	delete m_pSymbolPool;
}

bool CMapFile::LessAddr(const PairAddr2Symbol_t& left, const PairAddr2Symbol_t& right)
{
	return left.first < right.first;
}

void CMapFile::HandleMapFile(const wchar_t* szFileName)
{
#ifdef _WIN32
	FILE* fp = _wfopen(szFileName, L"r");
#else
	char* strMapFile = U16_to_U8(szFileName);
	FILE* fp = fopen(strMapFile, "r");
	FreeUtfCvsDest(strMapFile);
#endif
	
	if (!fp)
		return;

	bool bSuc = m_setMapFile.insert(szFileName).second;
	//表明已经处理了这个文件了，我们不需要再次处理
	if(!bSuc)
	{
		return;
	}

	m_AddrSymbol.push_back(VecAddr2Symbol_t());
	VecAddr2Symbol_t& vecAddr2Symbol = m_AddrSymbol[m_AddrSymbol.size() - 1];

	char sep[] = " \t\n";
	int ch = ':';
	char* token;

	bool bHandle = false;

	char buf[3000];

	while (true)
	{
		if (fgets(buf, sizeof(buf), fp) == NULL)
			break;

		uint32 pos = 0;
		bool bFilter = false;

		token = strtok(buf, sep);

		char* szFunName = NULL;
		uint32 uFunAddr = 0;

		while (token != NULL)
		{
			if (bHandle)
			{
				if (bFilter)
				{
					//第二列就是我们的函数名称
					if(pos == 1)
					{
						uint32 uStrLen = strlen(token) + 1;
						
						szFunName = (char*)m_pSymbolPool->Alloc(uStrLen);
						
						strcpy(szFunName, token);
					}
					//第三列就是我们函数的地址
					if (pos == 2)
					{
						sscanf(token, "%x", &uFunAddr);
					}

					++pos;
				}
				else
				{
					//需要处理的一行，如果最开始字段是000x:xxxxxxxxx，那么这一行我们就要处理
					if (pos == 0 && strchr(token, ch) != NULL)
					{
						bFilter = true;
					}		
					++pos;
				}
			}
			else
			{
				//如果出现__ImageBase字段，那么从下一行语句开始，我们就要处理map文件了
				if (strncmp(token, "___ImageBase", strlen(token)) == 0)
				{
					bHandle = true;      //从下一条语句开始处理
				}	
			}

			token = strtok(NULL, sep);

		}

		if (szFunName != NULL)
		{
			vecAddr2Symbol.push_back(make_pair(uFunAddr, szFunName));
		}

	}

	sort(vecAddr2Symbol.begin(), vecAddr2Symbol.end(), LessAddr);

	fclose(fp);

}


const char* CMapFile::FindFunName(uint32 uFunAddr)
{
	if (m_AddrSymbol.empty())
	{
		return "";
	}

	uint32 uSize = (uint32)m_AddrSymbol.size();
	for (uint32 i = 0; i < uSize; i++)
	{
		VecAddr2Symbol_t& vecAddr2Symbol = m_AddrSymbol[i];

		VecAddr2Symbol_t::iterator iter = upper_bound(vecAddr2Symbol.begin(), vecAddr2Symbol.end(), 
			make_pair(uFunAddr, (char*)0),LessAddr);
		if (iter == vecAddr2Symbol.end())
		{
			continue;
		}
		else
		{
			if (iter == vecAddr2Symbol.begin())
			{
				continue;
			}

			char* szName = (--iter)->second;
			return szName;
		}
	}

	return "";
}

