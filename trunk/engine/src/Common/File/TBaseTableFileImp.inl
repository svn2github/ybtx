#pragma once 
#include "TBaseTableFileImp.h"
#include "CVariant.h"
#include "CLock.inl"
#include "ErrLogHelper.h"
#include "CodeCvs.h"
#include "CrcFileCheck.h"
#ifdef _WIN32
#include "CBaseScriptApp.h"
#endif
#include "StringHelper.h"
#define CONFIGPKG "../../config.pkg"
#define CRC32_FILES_CHECK L"/etc/gac/CrcCode.txt"
namespace sqr 
{

template<typename T>
CLock TBaseTableFileImp<T>::ms_Lock;

template<typename T>
typename TBaseTableFileImp<T>::MapKey2Imp_t	TBaseTableFileImp<T>::ms_mapKey2Imp;

template<typename T>
TBaseTableFileImp<T>::TBaseTableFileImp(bool bFieldNameEnabled)
: m_uRefCount(0)
{
	m_TableData.nWidth = 0;
	m_TableData.nHeight = 0;
	m_TableData.bFieldNameEnabled = bFieldNameEnabled;
}

template<typename T>
TBaseTableFileImp<T>::~TBaseTableFileImp()
{
	if (!m_TableData.mapName2Col.empty())
	{
		typename MapName2Col_t::iterator iter = m_TableData.mapName2Col.begin();
		while (iter != m_TableData.mapName2Col.end())
		{
			SqrDelete(const_cast<char*>(iter->first), CDataFileMallocObject::GetPool());
			++iter;
		}
	}

	if(m_sKey.empty())
		return;

	ms_mapKey2Imp.erase(m_sKey);
}

template<typename T>
void TBaseTableFileImp<T>::Lock()
{
	ms_Lock.Lock();
}

template<typename T>
void TBaseTableFileImp<T>::UnLock()
{
	ms_Lock.Unlock();
}

template<typename T>
pair<T*, bool> TBaseTableFileImp<T>::GetImp(const wchar_t* szAliasPath, VecFileName_t& vecFileName)
{
	String_t sKey;
	
	sKey = utf16_to_utf8(szAliasPath).c_str();

	size_t stSize = vecFileName.size();
	for(size_t i = 0; i < stSize; i++)
	{
		sKey.append(vecFileName[i]);
	}

	T* pImp = NULL;
	String_t sFileName;

	typename MapKey2Imp_t::iterator iter = ms_mapKey2Imp.find(sKey);
	if(iter == ms_mapKey2Imp.end())
	{
		pImp = new T;
		pImp->m_sKey = sKey;
		pImp->m_sAliasPath = szAliasPath;

		for(size_t i = 0; i < stSize; i++)
		{
			sFileName = vecFileName[i];
			size_t stPos = sFileName.find_last_of('.');
			if (stPos != string::npos)
			{
				sFileName.erase(stPos);
			}

			pImp->m_setFileName.insert(sFileName);
		}
		
		ms_mapKey2Imp.insert(make_pair(sKey, pImp));
		return make_pair(pImp, true);
	}
	else
	{
		return make_pair(iter->second, false);
	}
}


template<typename T>
void TBaseTableFileImp<T>::AddRef()
{
	m_uRefCount++;
}

template<typename T>
void TBaseTableFileImp<T>::DelRef()
{
	m_uRefCount--;

	if (m_uRefCount == 0 )
	{
		delete static_cast<T*>(this);
	}
}

template<typename T>
bool TBaseTableFileImp<T>::TestAndHandleFirstRow(const char* szFileName, const char* szName, 
												 int32 nCol, size_t stOffset, bool bFirst,
												 TableFileData& data)
{
	MapName2Col_t& mapName2Col = data.mapName2Col;
	VecOffset_t& vecOffset = data.vecOffset;
	if (bFirst)
	{
		typename MapName2Col_t::iterator iter = mapName2Col.find(szName);
		if (iter == mapName2Col.end())
		{
			char* str = (char*)SqrNew(strlen(szName) + 1, CDataFileMallocObject::GetPool());
			strcpy(str, szName);
			mapName2Col.insert(make_pair(str, nCol));
		}
		else
		{
			//如果szName不是一个空串，那就重复字段，报错
			if (szName[0] != '\0')
			{
				ostringstream strm;
				strm << "存在重复的字段名字: " << szName;
				GenLoadError(szFileName, "TableFile Field Name错误", strm.str().c_str());
				return false;
			}
		}

		vecOffset.push_back((int32)stOffset);

		return true;
	}
	else
	{
		typename MapName2Col_t::iterator iter = mapName2Col.find(szName);
		if (iter->second == nCol )
		{
			return true;
		}
		else
		{
			ostringstream strm;
			strm << "字段名字" << szName << "在其他同类型的配置表里面是不存在的";
			GenLoadError(szFileName, "TableFile Field Name错误", strm.str().c_str());
			return false;
		}
	}

	return false;
}

template<typename T>
bool TBaseTableFileImp<T>::ParseFirstRow(const char* szFileName, size_t& stNextPos, TableFileData& data)
{
	VecBuf_t& vecBuf = data.vecBuf;
	MapName2Col_t& mapName2Col = data.mapName2Col;
	int32& nHeight = data.nHeight;
	int32& nWidth = data.nWidth;

	bool bFieldNameEnabled = data.bFieldNameEnabled;

	VecOffset_t vecOffset;

	size_t i = stNextPos;
	size_t stOffset = stNextPos;
	size_t stSize = vecBuf.size();
	bool bHitNewLine = false;

	for (; i < stSize; i++ )
	{
		if( vecBuf[i] == 0x0a || 
			( vecBuf[i] == 0x0d && vecBuf[i+1] == 0x0a ) ||
			vecBuf[i] == 0x09 )
		{
			char cFlag = vecBuf[i];

			if (bFieldNameEnabled)
			{
				vecBuf[i] = 0;
			}
	
			if (cFlag == 0x09)
			{
				vecOffset.push_back((int32)stOffset);
				stOffset = i + 1;
				continue;
			}
			else if(cFlag == 0x0a)
			{
				i++;
				bHitNewLine = true;
				break;
			}
			else
			{
				bHitNewLine = true;
				i += 2;
				break;
			}
		}
	}

	vecOffset.push_back((int32)stOffset);
	
	if (i == stSize)
	{
		if (!bHitNewLine)
		{
			vecBuf.push_back(0);
			i++;
		}
	}

	if (nWidth == 0)
	{
		nWidth = (int32)vecOffset.size();
	}
	else
	{
		if (nWidth != (int32)vecOffset.size())
		{
			GenLoadError(szFileName, "载入的TableFile文件存在表宽度不一致的数据");
		}
	}

	if (bFieldNameEnabled)
	{
		//如果第一行是field name，那我们就要忽略第一行数据了
		stNextPos = i;

		bool bFirst = false;
		if (mapName2Col.empty())
		{
			bFirst = true;
		}

		int32 stSize = (int32)vecOffset.size();
		for (int32 i = 0; i < stSize; i++)
		{
			int32 stOffset = vecOffset[i];
			bool bRet = TestAndHandleFirstRow(szFileName, &vecBuf[stOffset], i, stOffset, bFirst, data);
			if (!bRet)
			{
				return false;
			}
		}
		
		if (bFirst)
		{
			nHeight = 1;
		}
	}

	return true;
}

#define	AstTxt(filename, exp) \
	{if(!(exp)) GenLoadError(filename, "TableFile数据格式不正确，解析出错", "Assertion failure of expresion '"#exp"'");}

template<typename T>
bool TBaseTableFileImp<T>::ParseLeftRow(const char* szFileName, size_t stNextPos, TableFileData& data)
{
	VecBuf_t& vecBuf = data.vecBuf;
	VecOffset_t& vecOffset = data.vecOffset;
	int32& nHeight = data.nHeight;
	int32& nWidth = data.nWidth;

	size_t stOffset = stNextPos;
	int32 nCol = 0;

	size_t stSize = vecBuf.size();
	if (stNextPos == stSize)
	{
		return true;
	}

	for( size_t i = stNextPos; i < stSize; i++ )
	{
		if(vecBuf[i] == 0x0a || 
			( vecBuf[i] == 0x0d && vecBuf[i+1] == 0x0a ) ||
			vecBuf[i] == 0x09 )
		{
			char cFlag = vecBuf[i];
			vecBuf[i] = 0;
			
			int32 nCurCol = nCol++;
			if (nCol == nWidth)
			{
				AstTxt(szFileName, cFlag == 0x0d || cFlag == 0x0a);
				nCol = 0;
			}

			AstTxt(szFileName, vecOffset.size() == (size_t)(nWidth * nHeight + nCurCol));
			vecOffset.push_back((int32)stOffset);

			if( cFlag == 0x09 )
			{
				stOffset  = i + 1;
			}
			else if( cFlag == 0x0a )
			{ 
#ifdef _WIN32
				stringstream strMsg;
				strMsg<<szFileName<<"中出现了单独的换行符0x0a，可能包已损坏";
				wstring wpath = utf8_to_utf16(CBaseScriptApp::Inst()->GetRootFilePath(""));
				wpath = wpath + L"/config.pkg";
				wstring wCrcCodeTxt = wpath + CRC32_FILES_CHECK;
				CrcFileCheck::GetInst()->ReadCrcCodeToMap(wCrcCodeTxt);
				uint32 CrcCode = CrcFileCheck::GetInst()->DoCrc32Check(wpath);
				int64 OriCrcCode = CrcFileCheck::GetInst()->GetFileCrcCode(CONFIGPKG);
				strMsg<<",config.pkg的crc32码是"<<CrcCode<<",标准的crc32码应该是"<<OriCrcCode;
				GenErr(strMsg.str());
#else
				AstTxt(szFileName, nCol == 0);
				stOffset  = i + 1;
				nHeight++;
#endif
				if( stOffset < vecBuf.size() )
				{
					continue;
				}
				else
				{
					return true;
				}
			}
			else
			{
				AstTxt(szFileName, nCol == 0);
				stOffset = ( i++ ) + 2;
				nHeight++;
				if( stOffset < vecBuf.size() )
				{
					continue;
				}
				else
				{
					return true;
				}
			}
		}
	}

	AstTxt(szFileName, vecOffset.size() == (size_t)(nWidth * nHeight + nCol));
	vecOffset.push_back((int32)stOffset);

	vecBuf.push_back(0);

	nHeight++;

	return true;
}

#undef AstTxt

template<typename T>
bool TBaseTableFileImp<T>::LoadTableFile(const wchar_t* szAliasPath, const char* szFileName, 
										 TableFileData& data)
{
	VecBuf_t& vecBuf = data.vecBuf;

	CPkgFile	TabFile;
	if ( TabFile.Open(szAliasPath, szFileName,true) != eFE_SUCCESS )
		return false;

	if ( TabFile.Size() == 0 )
		return false;

	size_t stFileSize = TabFile.Size();
	size_t stNextPos = vecBuf.size();
	vecBuf.reserve(stNextPos + stFileSize + 1);
	vecBuf.insert( vecBuf.end(), stFileSize, 0 );
	TabFile.read( &vecBuf[stNextPos], TabFile.Size() );

	bool bRet = ParseFirstRow(szFileName, stNextPos, data);
	if (!bRet)
	{
		return false;
	}
	
	bRet = ParseLeftRow(szFileName, stNextPos, data);
	if(!bRet)
	{
		return false;
	}


	return true;
}

template<typename T>
bool TBaseTableFileImp<T>::Load()
{
	const wchar_t* szAliasPath = m_sAliasPath.c_str();

	typename SetFileName_t::iterator iter = m_setFileName.begin();
	typename SetFileName_t::iterator iter_end = m_setFileName.end();


	SQR_TRY
	{
		String_t sFileName;
	
		while(iter != iter_end)
		{
			sFileName.assign((*iter));
			sFileName.append(".xml");
			bool bRet = static_cast<T*>(this)->LoadXMLFileDerived(szAliasPath, sFileName.c_str());
			if (!bRet)
			{
				return false;
			}

			++iter;
		}

		iter = m_setFileName.begin();
		while(iter != iter_end)
		{
			sFileName.assign((*iter));
			sFileName.append(".txt");

			static_cast<T*>(this)->SetFileRowOffsetDerived(iter->c_str());

			bool bRet = LoadTableFile(szAliasPath, sFileName.c_str(), m_TableData);
			if (!bRet)
			{
				return false;
			}

			++iter;
		}
		bool ret = static_cast<T*>(this)->LoadDerived();

		return ret;
	}
	SQR_CATCH(exp)
	{
		static_cast<T*>(this)->OnExceptionDerived(exp);
		LogExp(exp);
	}
	SQR_TRY_END;
	
	return false;
}


template<typename T>
void TBaseTableFileImp<T>::GenLoadError(const char* szFileName, const string& sType)
{
	GenLoadError(szFileName, sType, "");
}

template<typename T>
void TBaseTableFileImp<T>::GenLoadError(const char* szFileName, const string& sType, const string& sMsg)
{
	ostringstream strm;
	if (!sMsg.empty())
	{
		strm << sMsg.c_str() << endl;
	}

	if (szFileName)
	{
		strm << "出错文件: " << szFileName << endl;
	}
	else
	{
		strm << " FileNames: ";

		typename SetFileName_t::iterator iter = m_setFileName.begin();
		typename SetFileName_t::iterator iter_end = m_setFileName.end();

		while(iter != iter_end)
		{
			strm << iter->c_str() << " ";
			++iter;
		}
	}

	GenErr(sType.c_str(), strm.str().c_str());
}

template<typename T>
bool TBaseTableFileImp<T>::LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName)
{
	(void)szAliasPath;
	(void)szFileName;

	return true;
}

template<typename T>
void TBaseTableFileImp<T>::SetFileRowOffsetDerived(const char* szFileName)
{
	(void)szFileName;
}

template<typename T>
bool TBaseTableFileImp<T>::LoadDerived()
{
	return true;
}

template<typename T>
bool TBaseTableFileImp<T>::Reload(VecFileName_t& vecFileName)
{
	if (vecFileName.empty())
	{
		return true;
	}

	size_t stSize = vecFileName.size();

	typename SetFileName_t::iterator it;

	String_t sFileName;

	for(size_t i = 0; i < stSize; i++)
	{
		sFileName = vecFileName[i];
	
		it = m_setFileName.find(sFileName);
	
		if( it == m_setFileName.end())
		{
			GenErr("Reload的Table File文件名跟原先的不一致", sFileName.c_str());
			return false;
		}

	}

	return static_cast<T*>(this)->ReloadDerived(vecFileName);	
}

template<typename T>
bool TBaseTableFileImp<T>::ReloadDerived(VecFileName_t& vecFileName)
{
	TableFileData ReloadData;
	ReloadData.nHeight = 0;
	ReloadData.nWidth = 0;
	ReloadData.bFieldNameEnabled = m_TableData.bFieldNameEnabled;

	typename SetFileName_t::iterator it;

	String_t sFileName;

	it = m_setFileName.begin();
	typename SetFileName_t::iterator it_end = m_setFileName.end();

	SQR_TRY
	{
		while(it != it_end)
		{
			sFileName.assign((*it).c_str());
			sFileName.append(".txt");

			bool bRet = LoadTableFile(m_sAliasPath.c_str(), sFileName.c_str(), ReloadData);
			if (!bRet)
			{
				return false;
			}

			++it;
		}

		//重新载入完成之后，我们跟原先的数据进行比较，如果有不一致的地方，载入失败
		if(ReloadData.nWidth != m_TableData.nWidth)
		{
			ostringstream strm;
			strm << "原先width: " << m_TableData.nWidth << "  现在width: " << ReloadData.nWidth << endl;
			GenErr("Reload TableFile Error", strm.str().c_str());
		}

		if(ReloadData.nHeight != m_TableData.nHeight)
		{
			ostringstream strm;
			strm << "原先height: " << m_TableData.nHeight << "  现在height: " << ReloadData.nHeight << endl;
			GenErr("Reload TableFile Error", strm.str().c_str());
		}

		if(ReloadData.vecOffset.size() != m_TableData.vecOffset.size())
		{
			ostringstream strm;			
			strm << "Table File vecoffset的size不一致，修改文件进行了结构的改变" << endl;
			GenErr("Reload TableFile Error", strm.str().c_str());
		}

		if(ReloadData.bFieldNameEnabled)
		{
			int32 nWidth = m_TableData.nWidth;
			for(int32 i = 0; i < nWidth; i++)
			{
				const char* szField1 = &(m_TableData.vecBuf[m_TableData.vecOffset[i]]);
				const char* szField2 = &(ReloadData.vecBuf[ReloadData.vecOffset[i]]);
				if(strcmp(szField1, szField2) != 0)
				{					
					ostringstream strm;
					strm << "Table File Field Name不一样 原先Name: " << szField1 << "  现在Name: " << szField2 << endl;
					GenErr("Reload TableFile Error", strm.str().c_str());
				}
			}
		}

		//将载入的新的数据存到buffer中
		m_TableData.vecBuf.swap(ReloadData.vecBuf);
		m_TableData.vecOffset.swap(ReloadData.vecOffset);

	}
	SQR_CATCH(exp)
	{
		LogExp(exp);
		return false;
	}
	SQR_TRY_END;


	return true;
}


template<typename T>
void TBaseTableFileImp<T>::OnExceptionDerived(CError& error)
{
	(void)error;
}

template<typename T>
int32 TBaseTableFileImp<T>::GetHeight()
{
	return m_TableData.nHeight;
}

template<typename T>
int32 TBaseTableFileImp<T>::GetWidth()
{
	return m_TableData.nWidth;
}

template<typename T>
const char* TBaseTableFileImp<T>::GetString(int32 nRow, int32 nCol)
{
	if (nRow < m_TableData.nHeight && nCol < m_TableData.nWidth)
	{
		int32 nOffset = m_TableData.vecOffset[nRow * m_TableData.nWidth + nCol];
		return &m_TableData.vecBuf[nOffset];
	}

	return "";
}

template<typename T>
const char* TBaseTableFileImp<T>::GetString(int32 nRow, const char* szColName)
{
	int32 nCol = GetCol(szColName);
	if (nCol == -1)
	{
		return "";
	}

	if (nRow < m_TableData.nHeight)
	{
		int32 nOffset = m_TableData.vecOffset[nRow * m_TableData.nWidth + nCol];
		return &m_TableData.vecBuf[nOffset];
	}

	return "";
}

template<typename T>
const char* TBaseTableFileImp<T>::GetString(int32 nOffsetIndex)
{
	if(nOffsetIndex >= 0 && nOffsetIndex < (int32)m_TableData.vecOffset.size())
	{
		return &m_TableData.vecBuf[m_TableData.vecOffset[nOffsetIndex]];
	}
	
	return "";
}

template<typename T>
int32 TBaseTableFileImp<T>::GetCol(const char* szColName)
{
	if(!m_TableData.bFieldNameEnabled)
	{
		GenErr("TableFile没有列名，不能使用列名进行访问");
	}

	typename MapName2Col_t::iterator iter = m_TableData.mapName2Col.find(szColName);
	if (iter != m_TableData.mapName2Col.end())
	{
		return iter->second;
	}

	return -1;
}
template<typename T>
const char * TBaseTableFileImp<T>::GetNamefromCol(int32 Col)
{
	typename MapName2Col_t::iterator iter = m_TableData.mapName2Col.begin();
	for(;iter!=m_TableData.mapName2Col.end();iter++)
	{
		if(iter->second==Col)
		{
			return iter->first;
		}
	}
	GenErr("列号没有对应的列名");
	return "ERROR";
}
template<typename T>
void TBaseTableFileImp<T>::ClearData()
{
	m_TableData.nWidth = 0;
	m_TableData.nHeight = 0;
	VecBuf_t vecBufTemp;
	VecOffset_t vecOffsetTemp;
	m_TableData.vecBuf.swap(vecBufTemp);
	m_TableData.vecOffset.swap(vecOffsetTemp);
}

}
