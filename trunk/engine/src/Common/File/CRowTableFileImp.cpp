#include "stdafx.h"
#include "CRowTableFileImp.h"
#include "TSqrAllocator.inl"
#include <fstream>
#include "CXmlConfig.inl"
#include "PkgStream.h"
#include "CodeCvs.h"
#include "TBaseTableFileImp.inl"
#include "ErrLogHelper.h"
#include <algorithm>
#include "CRowTableFileKey.h"
#include "CRowTableFileNode.h"
#include "CConstString.h"
#include "CRowLeafNodeDataMeta.h"

template class TBaseTableFileImp<CRowTableFileImp>;

CRowTableFileImp::CRowTableFileImp()
: Parent_t(false)
, m_pRowTableRootNode(NULL)
, m_pRowTableFileKey(NULL)
{
	/////////////////////////
	FileName1="";
	/////////////////////////
	m_pXmlIndexPool = CConstString::CreatePool();
	m_pTxtIndexPool = CConstString::CreatePool();

	m_pLoadingPool = new CMemPool(10240, CDataFileMallocObject::GetPool());

	m_pDataBuffer = new CMemPool(1024, CDataFileMallocObject::GetPool());
}

CRowTableFileImp::~CRowTableFileImp()
{
	if (m_pRowTableRootNode)
	{
		delete m_pRowTableRootNode;
		m_pRowTableRootNode = NULL;
	}

	if (m_pDataBuffer)
	{
		delete m_pDataBuffer;
		m_pDataBuffer = NULL;
	}

	size_t stSize = m_vecLeafNodeMeta.size();
	for (size_t i = 0; i < stSize; i++)
	{
		delete m_vecLeafNodeMeta[i];
	}

	CConstString::DestroyPool(m_pXmlIndexPool);
	CConstString::DestroyPool(m_pTxtIndexPool);
}

const char* CRowTableFileImp::GetIndexByName(const char* szName, CConstStringPool* pStringPool, bool bForceInsert)
{
	CConstString ConstString(szName, pStringPool, bForceInsert);

	return ConstString.GetStr();
}


void CRowTableFileImp::EndLoad()
{
	m_pRowTableRootNode->SortIndex2Node();

	delete m_pLoadingPool;
	m_pLoadingPool = NULL;

	m_pRowTableFileKey = NULL;

	ClearData();
}

bool CRowTableFileImp::LoadXMLFileDerived(const wchar_t* szAlias, const char* szXmlFileName)
{
	//载入xml
	ipkgstream ifXml(szAlias, szXmlFileName);
	if (!ifXml)
	{
		GenLoadError(szXmlFileName, "RowTable没有对应的XML文件");	
		return false;
	}

	CXmlConfig XmlCfg;
	XmlCfg.LoadConfig("config", ifXml);
	ifXml.close();

	TiXmlNode* pRootNode = XmlCfg.GetRootNode();
	if (pRootNode == NULL)
	{
		return false;
	}

	const char* szType = pRootNode->FirstChild("TableType")->ToElement()->GetText();
	if (szType[0] != 'M' && szType[0] != 'm')
	{
		GenLoadError(szXmlFileName, "XML文件类型不正确，TableType不是M或者m");
		return false;
	}

	TiXmlNode* pIndexNode = pRootNode->FirstChild("IndexKey");
	Ast(pIndexNode);

	if (!m_pRowTableFileKey)
	{
		m_pRowTableFileKey = new (m_pLoadingPool)CRowTableFileTreeKey(pIndexNode, this, m_pLoadingPool);
	}
	else
	{
		CRowTableFileTreeKey* pKey = new (m_pLoadingPool)CRowTableFileTreeKey(pIndexNode, this, m_pLoadingPool);
		if (!m_pRowTableFileKey->Equal(pKey))
		{
			ostringstream strm;
			strm << "不一致的文件为: " << szXmlFileName << endl;
			GenLoadError(szXmlFileName, "载入的文件列表里面存在格式不一致的XML配置文件", strm.str().c_str());
		}
	}

	return true;
}

CRowTableFileNode* CRowTableFileImp::ParseFirstCol(int32 nRow, vector<const char*>& vecLastRowData, const char* szFileName)
{
	CRowTableFileNode* pData = m_pRowTableRootNode;

	const char* szData = GetString(nRow, 0);
	if (szData[0] != '\0')
	{
		if (strcmp(szData, vecLastRowData[0]) == 0)
		{
			pData = pData->GetSubNode(szData);
		}
		else
		{
			pData = pData->AddSubNode(szData, szFileName);

			vecLastRowData[0] = szData;
		}
	}
	else
	{
		//last row data绝对不能为空
		szData = vecLastRowData[0];
		Ast(szData[0] != '\0');

		pData = pData->GetSubNode(szData);
	}

	return pData;
}

bool CRowTableFileImp::LoadDerived()
{
	//在这里我们根据生成的key tree来进行数据分析，生成data tree

	int32 nHeight = GetHeight();
	int32 nWidth = GetWidth();

	m_vecFileRowOffset.push_back(make_pair((const char*)0, nHeight));

	vector<const char*>	vecLastRowData(nWidth, "");	

	//创建一个跟节点
	m_pRowTableRootNode = new CRowTableFileNode(this, NULL);

	CRowTableFileKey* pKey = m_pRowTableFileKey;
	CRowTableFileNode* pData = m_pRowTableRootNode;

	map<string, CLeafNodeDataMeta*>	mapKey2Meta;
	string sMetaKey;
    int32 FileCow=0;
	for (int32 i = 0; i < nHeight; i++)
	{
		sMetaKey.clear();

		const char* szFileName = GetFileName(i);
		if(i>0)
		{
			if(szFileName!=FileName1)//切换文件FileCow初始化。
			{
				FileCow=0;
			}
			else 
				FileCow++;
		}
		FileName1=szFileName;
	
		//首先解析第一列，我们的第一列跟key tree没有关系
		pData = ParseFirstCol(i, vecLastRowData, szFileName);

		//每开始一行，我们会从跟节点重新解析
		pKey = m_pRowTableFileKey;

		//以此遍历直到key的类型为leaf为止
		int32 nFieldOffset = 0;
		Ast(pKey);
		for (int32 j = 1; j < nWidth; j++)
		{
			ETableFileKeyType eType = pKey->GetType();
			if (eType == eTFKT_Tree)
			{
				CRowTableFileTreeKey* pTreeKey = static_cast<CRowTableFileTreeKey*>(pKey);
				ETableFileIndexKeyType eIndexType = pTreeKey->GetIndexKeyType();
				Ast(pTreeKey);
				if(eIndexType == eTFIKT_Default)
				{

					//如果是默认索引，那么我们就要处理field数据了
					char buf[12];
					Ast(pData);
					int32 nNum = pData->GetSubNodeNum() + 1;
					sprintf(buf, "%d", nNum);
					pData = pData->AddSubNode(buf, szFileName);
					pKey = pTreeKey->GetSubKey("");
					nFieldOffset = j;
					break;
				}
				else
				{
					//如果是随机索引，那我们首先要读取字段，作为索引值了
					const char* szData = GetString(i, j);
					if (szData[0] != '\0')
					{
						pData = pData->AddSubNode(szData, szFileName);
						vecLastRowData[j] = szData;
					}
					else
					{
						szData = vecLastRowData[j];
						Ast(szData[0] != '\0');
						pData = pData->GetSubNode(szData);
					}	

					if(eIndexType == eTFIKT_Name)
					{
						pKey = pTreeKey->GetSubKey(szData);

						sMetaKey.append(szData);
					}
					else
					{
						pKey = pTreeKey->GetSubKey("");
					}
				}
								
			}
			else
			{
				nFieldOffset = j;
				break;
			}
		}
		Ast(pKey!=NULL);
		//下面开始处理field data
		ETableFileKeyType eType = pKey->GetType();
		//key一定为leaf
		Ast(eType == eTFKT_Leaf);

		CRowTableFileLeafKey* pLeafKey = static_cast<CRowTableFileLeafKey*>(pKey);

		CRowTableFileField& RowField = pLeafKey->GetField();
		int32 nFieldNum = RowField.FieldNum();

		if(nFieldNum != (nWidth - nFieldOffset))
		{
			pData->GenLoadErr(i, "RowTableFile 数据起始位置错误，可能配置文件填写错误");
		}
		Ast(pData!=NULL);
		if (RowField.LeafNodeType() != eRTNT_LeafNoData)
		{
			map<string, CLeafNodeDataMeta*>::iterator iter = mapKey2Meta.find(sMetaKey);
			CLeafNodeDataMeta* pMeta = NULL;
			if (iter == mapKey2Meta.end())
			{
				pMeta = new CLeafNodeDataMeta(RowField);
				m_vecLeafNodeMeta.push_back(pMeta);

				mapKey2Meta.insert(make_pair(sMetaKey, pMeta));
			}
			else
			{
				pMeta = iter->second;
			}
			pData->SetFieldData(i, nFieldOffset, RowField, pMeta,szFileName,FileCow);
		}
		else
		{
			pData->SetFieldData(i, nFieldOffset, RowField, NULL,szFileName,FileCow);
		}
	}

	EndLoad();

	return true;
}

void CRowTableFileImp::SetFileRowOffsetDerived(const char* szFileName)
{
	uint32 uHeight = GetHeight();
	m_vecFileRowOffset.push_back(make_pair(szFileName, uHeight));
}

inline bool CRowTableFileImp::LessFileRow(const PairFileRowOffset& left, const PairFileRowOffset& right)
{
	return left.second < right.second;
}

const char* CRowTableFileImp::GetFileName(uint32 uRow)
{
	VecFileRowOffset_t::iterator iter = lower_bound(m_vecFileRowOffset.begin(), 
		m_vecFileRowOffset.end(), make_pair((const char*)0, uRow), LessFileRow);

	if (iter == m_vecFileRowOffset.end())
	{
		return NULL;
	}

	if (iter->second == uRow)
	{
		return iter->first;
	}
	else
	{
		return (--iter)->first;
	}
}


double* CRowTableFileImp::AllocDataNumber(const char* szData)
{
	double dNum = atof(szData);
	double* buffer = (double*)m_pDataBuffer->Alloc(sizeof(double));
	memcpy(buffer, &dNum, sizeof(double));

	return buffer;
}

char* CRowTableFileImp::AllocDataString(const char* szData)
{
	size_t stSize = strlen(szData);
	char* buffer = (char*)m_pDataBuffer->Alloc(stSize + 1);
	memcpy(buffer, szData, stSize + 1);

	return buffer;
}

bool CRowTableFileImp::ReloadDerived(VecFileName_t& vecFileName)
{
	if (!m_pLoadingPool)
	{
		m_pLoadingPool = new CMemPool(1024, CDataFileMallocObject::GetPool());
	}

	String_t sFileName;

	size_t stSize = vecFileName.size();

	for (size_t i = 0; i < stSize; i++)
	{
		sFileName = vecFileName[i];
		sFileName.append(".txt");

		bool bRet = LoadTableFile(m_sAliasPath.c_str(), sFileName.c_str(), m_TableData);
		if (!bRet)
		{
			return false;
		}
	}

	sFileName = vecFileName[0];
	sFileName.append(".xml");

	LoadXMLFileDerived(m_sAliasPath.c_str(), sFileName.c_str());

	//对于reload来说，我们只能改变叶子节点的数据，不能修改其他任何的结构

	int32 nHeight = GetHeight();
	int32 nWidth = GetWidth();

	vector<const char*>	vecLastRowData(nWidth, "");	

	CRowTableFileKey* pKey = m_pRowTableFileKey;
	CRowTableFileNode* pData = m_pRowTableRootNode;

	typedef map<CRowTableFileNode*, int32>	MapNode2DefaultIndex_t;

	MapNode2DefaultIndex_t	mapNode2DefaultIndex;

	for (int32 i = 0; i < nHeight; i++)
	{
		//首先解析第一列，我们的第一列跟key tree没有关系
		pData = m_pRowTableRootNode;

		const char* szRowIndex = GetString(i, 0);
		if (szRowIndex[0] != '\0')
		{
			pData = pData->GetSubNode(szRowIndex);
			vecLastRowData[0] = szRowIndex;
		}
		else
		{
			szRowIndex = vecLastRowData[0];
			Ast(szRowIndex[0] != '\0');
			pData = pData->GetSubNode(szRowIndex);
		}	

		//每开始一行，我们会从跟节点重新解析
		pKey = m_pRowTableFileKey;

		//以此遍历直到key的类型为leaf为止
		int32 nFieldOffset = 0;

		for (int32 j = 1; j < nWidth; j++)
		{
			ETableFileKeyType eType = pKey->GetType();
			if (eType == eTFKT_Tree)
			{
				CRowTableFileTreeKey* pTreeKey = static_cast<CRowTableFileTreeKey*>(pKey);
				ETableFileIndexKeyType eIndexType = pTreeKey->GetIndexKeyType();

				if(eIndexType == eTFIKT_Default)
				{
					int32 nIndex = 1;
					//如果是默认索引，那么我们就要处理field数据了
					MapNode2DefaultIndex_t::iterator iter = mapNode2DefaultIndex.find(pData);	
					if (iter == mapNode2DefaultIndex.end())
					{
						mapNode2DefaultIndex.insert(make_pair(pData, nIndex));
					}
					else
					{
						nIndex = iter->second + 1;
						iter->second = nIndex;
					}

					char buf[12];

					sprintf(buf, "%d", nIndex);
					pData = pData->GetSubNode(buf);
					pKey = pTreeKey->GetSubKey("");

					nFieldOffset = j;
					break;
				}
				else
				{
					//如果是随机索引，那我们首先要读取字段，作为索引值了
					const char* szData = GetString(i, j);
					if (szData[0] != '\0')
					{
						pData = pData->GetSubNode(szData);
						vecLastRowData[j] = szData;
					}
					else
					{
						szData = vecLastRowData[j];
						Ast(szData[0] != '\0');
						pData = pData->GetSubNode(szData);
					}	

					if(eIndexType == eTFIKT_Name)
					{
						pKey = pTreeKey->GetSubKey(szData);
					}
					else
					{
						pKey = pTreeKey->GetSubKey("");
					}
				}

			}
			else
			{
				nFieldOffset = j;
				break;
			}
		}


		//下面开始处理field data
		ETableFileKeyType eType = pKey->GetType();
		//key一定为leaf
		Ast(eType == eTFKT_Leaf);

		CRowTableFileLeafKey* pLeafKey = static_cast<CRowTableFileLeafKey*>(pKey);

		CRowTableFileField& RowField = pLeafKey->GetField();
		int32 nFieldNum = RowField.FieldNum();

		if(nFieldNum != (nWidth - nFieldOffset))
		{
			pData->GenLoadErr(i, "RowTableFile 数据起始位置错误，可能配置文件填写错误");
		}

		pData->ResetFieldData(i, nFieldOffset, RowField);
	}

	m_pRowTableFileKey = NULL;

	delete m_pLoadingPool;
	m_pLoadingPool = NULL;

	ClearData();

	return true;
}



