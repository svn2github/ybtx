#include "stdafx.h"
#include "CCurLangTxtTableFileImp.h"

#include "PkgStream.h"
#include "TSqrAllocator.inl"
#include "TBaseTableFileImp.inl"

template class TBaseTableFileImp<CCurLangTxtTableFileImp>;

CCurLangTxtTableFileImp::CCurLangTxtTableFileImp()
: Parent_t(true)
, m_bBuildKeyMapFinished(false)
{

};

CCurLangTxtTableFileImp::~CCurLangTxtTableFileImp()
{
	//ClearMap();
};

void CCurLangTxtTableFileImp::ClearMap()
{
	//for(MapKey2Row_t::iterator itr = m_mapKeyToRowNo.begin(); itr != m_mapKeyToRowNo.end(); ++itr)
	//{
	//	if(itr->first)
	//	{
	//		delete [] itr->first;
	//	}
	//}
	//m_mapKeyToRowNo.clear();
}

void CCurLangTxtTableFileImp::BuildKeyMap()
{
	Lock();
	if(m_bBuildKeyMapFinished)
	{
		UnLock();
		return;
	}
	m_bBuildKeyMapFinished = true;
	//ClearMap();
	m_mapKeyToRowNo.clear();

	int32 uTableDataHeight = m_TableData.nHeight;
	int32 uTableDataWidth = m_TableData.nWidth;
	for(int32 i = 0; i < uTableDataHeight; ++i)
	{
		int32 iOffset = m_TableData.vecOffset[uTableDataWidth * i];
		const char* szKeyName = &m_TableData.vecBuf[iOffset];
		//int32 iCount = 0;
		////const char * szKeyName = NULL;
		//for(; m_TableData.vecBuf[iCount + iOffset]; ++iCount)
		//{
		//	
		//}
		//char* szKeyName = new char[iCount + 1];
		//for(int j = 0; j <= iCount; ++j)
		//{
		//	szKeyName[j] = m_TableData.vecBuf[j + iOffset];
		//}
		//if (i == 0)
		//{
		//	int k = 0;
		//}
		////if(bPringLog)
		////{
		////	cout << "i = " << i << ", szKeyName = " << szKeyName << ", m_mapKeyToRowNo.size() = " << m_mapKeyToRowNo.size();
		////	cout << " szKeyName = " << szKeyName;
		////}

		//if(m_mapKeyToRowNo.find(szKeyName) != m_mapKeyToRowNo.end())
		//{
		//	delete [] szKeyName;
		//	szKeyName = NULL;
		//	continue;
		//}
		////if(bPringLog)
		////	cout << " insert";
		pair<MapKey2Row_t::iterator, bool> pt = m_mapKeyToRowNo.insert(make_pair(szKeyName, i));
		//if(bPringLog)
		//	cout << " end\n";
		//因为_load_list在游戏新版本中的组合可能发生变化，导致旧语言包用新组合可能发生键值重复，为了兼容旧语言包就不能报错
		//if(!pt.second)
		//{
		//	ostringstream strm;
		//	strm << "第" << i << "[" << szKeyName << "]";
		//	//GenLoadError("翻译语言表键值重复", strm.str().c_str());
		//	GenErr("翻译语言表键值重复", strm.str().c_str());
		//}
	}
	UnLock();
}

int32 CCurLangTxtTableFileImp::GetRowNo(const char* szKeyName)
{
	MapKey2Row_t::iterator it = m_mapKeyToRowNo.find(szKeyName);
	if(it != m_mapKeyToRowNo.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}

int32 CCurLangTxtTableFileImp::GetColNo(const char* szTitleName)
{
	MapName2Col_t::iterator it = m_TableData.mapName2Col.find(szTitleName);
	if(it != m_TableData.mapName2Col.end())
	{
		return it->second;
	}
	else
	{
		return -1;
	}
}
