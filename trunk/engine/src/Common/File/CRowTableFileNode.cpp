#include "stdafx.h"
#include "CRowTableFileNode.h"
#include "ErrLogHelper.h"
#include "ExpHelper.h"
#include "CRowTableFileImp.h"
#include "CRowTableFileKey.h"
#include "TSqrAllocator.inl"
#include "CVariant.h"
#include "CDataFileMallocObject.h"
#include "CSqrAllocPool.h"
#include "CTableFileValue.h"
#include "CRowLeafNodeDataMeta.h"
#include "TSqrAllocObject.inl"

inline CVariant* CRowTableNodeKeys::CreateVariant()
{
	void* buffer = SqrNew(sizeof(CVariant), CDataFileMallocObject::GetPool());
	CVariant* pVariant = new (buffer)CVariant;
	return pVariant;
}

inline void CRowTableNodeKeys::DestroyVariant(CVariant* pVariant)
{
	pVariant->~CVariant();
	SqrDelete(pVariant, CDataFileMallocObject::GetPool());
}


CRowTableNodeKeys::CRowTableNodeKeys()
{

}


CRowTableNodeKeys::~CRowTableNodeKeys()
{
	Clear();

	while (!m_stkKeyBank.empty())
	{
		CVariant* pVar = m_stkKeyBank.top();
		DestroyVariant(pVar);
		m_stkKeyBank.pop();
	}
}


void CRowTableNodeKeys::AddKey(const char* szKey)
{
	CVariant* pVar = NULL;
	if (m_stkKeyBank.empty())
	{
		pVar = CreateVariant();
	}
	else
	{
		pVar = m_stkKeyBank.top();
		m_stkKeyBank.pop();
	}

	pVar->SetCStr(szKey);
	m_vecKey.push_back(pVar);		
}

void CRowTableNodeKeys::Clear()
{
	size_t stSize = m_vecKey.size();
	for (size_t i = 0; i < stSize; i++)
	{
		if (m_stkKeyBank.size() > 100)
		{
			DestroyVariant(m_vecKey[i]);
		}
		else
		{
			m_stkKeyBank.push(m_vecKey[i]);
		}
	}
	m_vecKey.clear();
}

void CRowTableNodeKeys::ReserveKey(size_t stSize)
{
	Clear();
	m_vecKey.reserve(stSize);
}

//////////////////////////////////////////////////////////

inline bool CRowTableFileNode::LessNode(const PairIndex2Node_t& Left, const PairIndex2Node_t& Right)
{
	return (size_t)Left.first < (size_t)Right.first;
}

inline bool CRowTableFileNode::LessData(const PairIndex2Data_t& Left, const PairIndex2Data_t& Right)
{
	return (size_t)Left.first < (size_t)Right.first;
}


CRowTableFileNode::CRowTableFileNode() 
{


}

CRowTableFileNode::CRowTableFileNode(CRowTableFileImp* pImp, const char* szFileName)
: m_eType(eRTNT_Tree)
, m_pImp(pImp)
, m_szNodeFileName(szFileName)
{
	memset(&m_Content, 0, sizeof(Content));
}


CRowTableFileNode::~CRowTableFileNode()
{
	if (m_eType == eRTNT_Tree)
	{
		size_t stSize = m_Content.m_pTreeNode->m_vecIndex2Node.size();

		for (size_t i = 0; i < stSize; i++)
		{
			PairIndex2Node_t& PairData = m_Content.m_pTreeNode->m_vecIndex2Node[i];
			DestroyNode(PairData.second);
		}

		DestroyTreeNode(m_Content.m_pTreeNode);
	}
	else
	{
		//size_t stSize = m_Content.m_pLeafNode->m_vecIndex2Data.size();

		if (m_eType != eRTNT_LeafNoData)
		{
			DestroyLeafNode((LeafDataNode*)m_Content.m_pLeafNode);
		}
	}
}

inline CRowTableFileNode* CRowTableFileNode::CreateNode(CRowTableFileImp* pImp, const char* szFileName)
{
	void* buffer = SqrNew(sizeof(CRowTableFileNode), CDataFileMallocObject::GetPool());
	return new(buffer)CRowTableFileNode(pImp, szFileName);
}

inline void CRowTableFileNode::DestroyNode(CRowTableFileNode* pNode)
{
	pNode->~CRowTableFileNode();
	SqrDelete(pNode, CDataFileMallocObject::GetPool());
}

inline CRowTableFileNode::TreeNode* CRowTableFileNode::CreateTreeNode()
{
	void* buffer = SqrNew(sizeof(TreeNode), CDataFileMallocObject::GetPool());
	return new(buffer)TreeNode();

}

inline void CRowTableFileNode::DestroyTreeNode(CRowTableFileNode::TreeNode* pNode)
{
	pNode->~TreeNode();
	SqrDelete(pNode, CDataFileMallocObject::GetPool());

}

inline CRowTableFileNode::LeafDataNode* CRowTableFileNode::CreateLeafNode()
{
	void* buffer = SqrNew(sizeof(LeafDataNode), CDataFileMallocObject::GetPool());
	return new(buffer)LeafDataNode();

}

inline void CRowTableFileNode::DestroyLeafNode(CRowTableFileNode::LeafDataNode* pNode)
{
	pNode->~LeafDataNode();
	SqrDelete(pNode, CDataFileMallocObject::GetPool());
}

const char* CRowTableFileNode::GetFileName()
{
	return m_szNodeFileName;
}


CConstStringPool* CRowTableFileNode::GetStringPool()
{
	//如果节点为跟节点，那么我们使用txt pool
	if (this == m_pImp->m_pRowTableRootNode)
	{
		return m_pImp->GetTxtIndexPool();
	}

	return m_pImp->GetXmlIndexPool();
}

void CRowTableFileNode::GenLoadErr(int32 nRow, const char* szType, const char* szMsg)
{
	ostringstream strm;

	if (szMsg)
	{
		strm << szMsg << "，";
	}

	strm << "该行数据:【 ";

	int32 nWidth = m_pImp->GetWidth();
	for (int32 k = 0; k < nWidth; k++)
	{
		strm << m_pImp->GetString(nRow, k) << " ";
	}

	if (m_szNodeFileName)
	{
		strm << "】对应文件: " << m_szNodeFileName;
	}


	GenErr(szType, strm.str().c_str());
}

CRowTableFileNode* CRowTableFileNode::AddSubNode(const char* szSubName, const char* szFileName)
{
	Ast(szSubName);

	if (m_eType == eRTNT_Tree)
	{
		if (!m_Content.m_pTreeNode)
		{
			m_Content.m_pTreeNode = CreateTreeNode();
		}

		const char* szIndex = m_pImp->GetIndexByName(szSubName, GetStringPool(), true);	

		VecIndex2Node_t::iterator iter = lower_bound(m_Content.m_pTreeNode->m_vecIndex2Node.begin(),
			m_Content.m_pTreeNode->m_vecIndex2Node.end(), PairIndex2Node_t(szIndex, NULL), LessNode);

		CRowTableFileNode* pSubNode = NULL;
		if (iter == m_Content.m_pTreeNode->m_vecIndex2Node.end())
		{
			pSubNode = CreateNode(m_pImp, szFileName);
			m_Content.m_pTreeNode->m_vecIndex2Node.push_back(
				make_pair(szIndex, pSubNode));	
			return pSubNode;
		}
		else
		{
			if ((size_t)iter->first == (size_t)szIndex)
			{
				//GenErr("节点已经存在，不允许重复添加", szSubName);
				return iter->second;
			}
			else
			{
				pSubNode = CreateNode(m_pImp, szFileName);
				m_Content.m_pTreeNode->m_vecIndex2Node.insert(iter, make_pair(szIndex, pSubNode));
				return pSubNode;
			}
		}				
	}
	else
	{
		ostringstream strm;
		strm << "欲添加节点: " << szSubName << endl;
		GenNodeErr("节点为叶子节点，不允许在添加节点", strm.str().c_str());
		return NULL;
	}
}

CRowTableFileNode* CRowTableFileNode::GetSubNode(const char* szSubName)
{
	Ast(szSubName);
	AstTreeNodeType("GetSubNode Error 没有子节点");	

	const char* szIndex = m_pImp->GetIndexByName(szSubName, GetStringPool());

	VecIndex2Node_t::iterator iter = lower_bound(m_Content.m_pTreeNode->m_vecIndex2Node.begin(),
		m_Content.m_pTreeNode->m_vecIndex2Node.end(), PairIndex2Node_t(szIndex, NULL), LessNode);

	if (iter != m_Content.m_pTreeNode->m_vecIndex2Node.end())
	{
		if ((size_t)iter->first == (size_t)szIndex)
		{
			return iter->second;
		}
	}

	return NULL;
}

//递归将tree的子节点vector排序
void CRowTableFileNode::SortIndex2Node()
{
	if(m_eType != eRTNT_Tree)
		return;

	sort(m_Content.m_pTreeNode->m_vecIndex2Node.begin(), 
		m_Content.m_pTreeNode->m_vecIndex2Node.end(), LessNode);

	size_t stSize = m_Content.m_pTreeNode->m_vecIndex2Node.size();

	for (size_t i = 0; i <stSize; i++)
	{
		PairIndex2Node_t& PairData = m_Content.m_pTreeNode->m_vecIndex2Node[i];
		PairData.second->SortIndex2Node();
	}
}


void CRowTableFileNode::SetFieldData(int32 nRow, int32 nColOffset, 
									 CRowTableFileField& RowField, CLeafNodeDataMeta* pMeta,const char * FILENAME,int32 ROW1)
{
	if (m_Content.m_pLeafNode)
	{
		GenLoadErr(nRow, "TableFile存在重复行级索引");
	}
	
	ERowTableNodeType eNodeType = RowField.LeafNodeType();

	SetType(eNodeType);

	AstLeafNodeType("不允许绑定TableFile数据");


	switch(eNodeType)
	{
	case eRTNT_LeafNoData:
		{
			static LeafNoDataNode ls_NoDataNode;
			m_Content.m_pLeafNode = &ls_NoDataNode;
			return;
		}
		break;
	case eRTNT_LeafOneData:
	case eRTNT_LeafMoreData:
		{
			LeafDataNode* pNode = CreateLeafNode();
			pNode->m_pMeta = pMeta;
			m_Content.m_pLeafNode = pNode ;
		}
		break;
	default:
		break;
	}	

	LeafDataNode* pNode = (LeafDataNode*)m_Content.m_pLeafNode;
	
	int32 nDataNum = pMeta->DataNum();
	
	VecLeafData_t& vecData = pNode->m_vecData;
	vecData.reserve(nDataNum);	

	int32 nWidth = m_pImp->GetWidth();
	
	for (int32 j = nColOffset; j < nWidth; j++)
	{
		int32 nCol = j - nColOffset;
		ETableFileFieldType eType = RowField.GetFieldType(nCol);

		int32 nOffsetIndex = nRow * m_pImp->GetWidth() + j;
		const char* szData = m_pImp->GetString(nOffsetIndex);
		
		switch(eType)
		{
		case eTFFT_Empty:
			{
				if (szData[0] != '\0')
				{
					ostringstream strm;
					strm << "Data: " << szData << "，Field Name: " << RowField.GetFieldKey(nCol) <<"，FileName:"<<FILENAME<<"，行号："<<ROW1+1<<"列号："<<nCol+3<<endl;
					GenLoadErr(nRow, "Field的类型为Empty，但是TableFile文件里面有值", strm.str().c_str());

				}
			}
			break;
		case eTFFT_Number:
			{
				vecData.push_back(m_pImp->AllocDataNumber(szData));
			}
			break;
		case eTFFT_String:
			{
				vecData.push_back(m_pImp->AllocDataString(szData));
			}
			break;
		}
	}

}

void CRowTableFileNode::ResetFieldData(int32 nRow, int32 nColOffset, 
									 CRowTableFileField& RowField)
{
	AstLeafNodeType("不允许绑定TableFile数据");
	if(m_eType == eRTNT_LeafNoData)
	{
		return;
	}

	LeafDataNode* pNode = (LeafDataNode*)m_Content.m_pLeafNode;
	//CLeafNodeDataMeta* pMeta = pNode->m_pMeta;
	VecLeafData_t& vecData = pNode->m_vecData;

	int32 nWidth = m_pImp->GetWidth();

	int32 nOffset = 0;
	for (int32 j = nColOffset; j < nWidth; j++)
	{
		int32 nCol = j - nColOffset;
		ETableFileFieldType eType = RowField.GetFieldType(nCol);

		int32 nOffsetIndex = nRow * m_pImp->GetWidth() + j;
		const char* szData = m_pImp->GetString(nOffsetIndex);

		switch(eType)
		{
		case eTFFT_Empty:
			{
				if (szData[0] != '\0')
				{
					ostringstream strm;
					strm << "Data: " << szData << "\tField Name: " << RowField.GetFieldKey(nCol) << endl;
					GenLoadErr(nRow, "Field的类型为Empty，但是TableFile文件里面有值", strm.str().c_str());
				}
			}
			break;
		case eTFFT_Number:
			{
				double dNewData = atof(szData);
				if (dNewData != *(double*)vecData[nOffset])
				{
					memcpy(vecData[nOffset], &dNewData, sizeof(double));
				}
				nOffset++;
			}
			break;
		case eTFFT_String:
			{
				char* szOldData = (char*)vecData[nOffset];
				if (strcmp(szOldData, szData) != 0)
				{
					size_t stOldLen = strlen(szOldData);
					size_t stNewLen = strlen(szData);
					if (stOldLen >= stNewLen)
					{
						memcpy(vecData[nOffset], szData, stNewLen + 1);
					}
					else
					{
						vecData[nOffset] = m_pImp->AllocDataString(szData);
					}
				}
				nOffset++;
			}
			break;
		}
	}
}

int32 CRowTableFileNode::GetSubNodeNum()
{
	if(m_eType != eRTNT_Tree)
	{
		return 0;
	}

	if(!m_Content.m_pTreeNode)
	{
		return 0;
	}

	return (int32)m_Content.m_pTreeNode->m_vecIndex2Node.size();
}

vector<CVariant*>* CRowTableFileNode::GetSubNodeKeys(CRowTableNodeKeys* pKeys)
{
	if(m_eType != eRTNT_Tree)
	{
		return NULL;
	}	
	
	size_t stSize = m_Content.m_pTreeNode->m_vecIndex2Node.size();
	pKeys->ReserveKey(stSize);
	for (size_t i = 0; i < stSize; i++)
	{
		const char* szIndex = m_Content.m_pTreeNode->m_vecIndex2Node[i].first;
		pKeys->AddKey(szIndex);
	}

	return pKeys->GetKeys();
}

bool CRowTableFileNode::HasSubNode(const char* szSubKey)
{
	CRowTableFileNode* pNode = GetSubNode(szSubKey);
	return (pNode != NULL);
}

void CRowTableFileNode::GenNodeErr(const char* szType, const char* szMsg)
{
	if (m_szNodeFileName)
	{
		ostringstream strm;
		strm << szMsg << " Node对应文件: " << m_szNodeFileName;
		GenErr(szType, strm.str().c_str());
	}
	else
	{
		GenErr(szType, szMsg);
	}
}

void CRowTableFileNode::AstTreeNodeType(const char* szMsg)
{
	if (m_eType != eRTNT_Tree)
	{
		GenNodeErr("RowTableFileNode的类型不是Tree", szMsg);
	}
}


void CRowTableFileNode::AstLeafNodeType(const char* szMsg)
{
	if (m_eType == eRTNT_Tree)
	{
		GenNodeErr("RowTableFileNode的类型不是Leaf", szMsg);
	}
}

CVariant* CRowTableFileNode::GetLeafNodeData(CTableFileValue* pValue, LeafNodeData& NodeData)
{
	switch(NodeData.m_eFieldType) 
	{
	case eTFFT_Number:
		{
			double dNum = *(double*)NodeData.m_pData;
			pValue->SetNumber(dNum);	
		}
		break;
	case eTFFT_String:
		{
			pValue->SetCStr((const char*)NodeData.m_pData);
		}
		break;
	case eTFFT_Empty:
		{
			return NULL;
		}
		break;
	}

	return pValue->GetValue();	
}

CRowTableFileNode::LeafNodeData CRowTableFileNode::GetDataByName(const char* szName)
{
	LeafNodeData NodeData;

	if (m_eType == eRTNT_LeafNoData)
	{
		return NodeData;
	}

	LeafDataNode* pNode = (LeafDataNode*)m_Content.m_pLeafNode;
	CLeafNodeDataMeta* pMega = pNode->m_pMeta;

	const char* szIndex = m_pImp->GetIndexByName(szName, GetStringPool());

	CLeafFieldInfo info = pMega->GetFieldInfo(szIndex);

	if (info.eFieldType != eTFFT_Empty)
	{
		NodeData.m_eFieldType = info.eFieldType;
		NodeData.m_pData = pNode->m_vecData[info.nDataOffset];
		return NodeData;
	}

	return NodeData;
}

CVariant* CRowTableFileNode::GetDataByName(CTableFileValue* pValue, const char* szName)
{
	LeafNodeData NodeData = GetDataByName(szName);
	
	return GetLeafNodeData(pValue, NodeData);
}

ETableFileFieldType CRowTableFileNode::GetFieldType(const char* szFieldName)
{
	AstLeafNodeType("GetFieldType Error");
		
	LeafNodeData NodeData = GetDataByName(szFieldName);
	
	return NodeData.m_eFieldType;
}

const char* CRowTableFileNode::GetString(const char* szFieldName)
{
	AstLeafNodeType("CRowTableFileNode GetString Error");
	
	LeafNodeData NodeData = GetDataByName(szFieldName);
	
	return (const char*)NodeData.m_pData;
}


template<typename ValueType>
ValueType CRowTableFileNode::GetNumber(const char* szFieldName)
{
	AstLeafNodeType("CRowTableFileNode GetNumber Error");
	
	LeafNodeData NodeData = GetDataByName(szFieldName);

	if (NodeData.m_eFieldType != eTFFT_Empty)
	{
		return (ValueType)(*(double*)(NodeData.m_pData));
	}
	
	return 0;
}

template int8 CRowTableFileNode::GetNumber<int8>(const char*);
template uint8 CRowTableFileNode::GetNumber<uint8>(const char*);
template int16 CRowTableFileNode::GetNumber<int16>(const char*);
template uint16 CRowTableFileNode::GetNumber<uint16>(const char*);
template int32 CRowTableFileNode::GetNumber<int32>(const char*);
template uint32 CRowTableFileNode::GetNumber<uint32>(const char*);
template int64 CRowTableFileNode::GetNumber<int64>(const char*);
template uint64 CRowTableFileNode::GetNumber<uint64>(const char*);
template float CRowTableFileNode::GetNumber<float>(const char*);
template double CRowTableFileNode::GetNumber<double>(const char*);

//----------------------------------------------------------------

ERowTableNodeType CRowTableFileNode::GetType(vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetType();
	}

	CRowTableFileNode* pSubData = GetSubNode(pArgList);
	return pSubData->GetType();
}

CRowTableFileNode* CRowTableFileNode::GetSubNode(vector<CVariant*>* pArgList)
{
	AstTreeNodeType("GetSubNode Error，没有子节点");
	Ast(pArgList);

	size_t stSize = pArgList->size();
	Ast(stSize > 0);

	CRowTableFileNode* pSubData = this;
	for (size_t i = 0; i < stSize; i++)
	{
		if (!pSubData)
		{
			ostringstream strm;
			strm << "不存在对应的节点, 输入索引: ";
			for(size_t i = 0; i < stSize; i++)
			{
				CVariant* pVar = pArgList->at(i);
				strm << pVar->GetCStr() << "、";
			}
			GenNodeErr("CRowTableFileNode::GetSubNode Error", strm.str().c_str());
		}

		CVariant* pVariant = pArgList->at(i);
		const char* szIndexName = pVariant->GetCStr();

		pSubData = pSubData->GetSubNode(szIndexName);
	}

	return pSubData;
}


vector<CVariant*>* CRowTableFileNode::GetSubNodeKeys(CRowTableNodeKeys* pKeys, vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetSubNodeKeys(pKeys);
	}

	CRowTableFileNode* pSubData = GetSubNode(pArgList);
	if (!pSubData)
	{
		return NULL;
	}

	return pSubData->GetSubNodeKeys(pKeys);
}

int32 CRowTableFileNode::GetSubNodeNum(vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetSubNodeNum();
	}

	CRowTableFileNode* pSubData = GetSubNode(pArgList);
	if (!pSubData)
	{
		return 0;
	}

	return pSubData->GetSubNodeNum();
}

bool CRowTableFileNode::HasSubNode(vector<CVariant*>* pArgList)
{
	CRowTableFileNode* pNode = GetSubNode(pArgList);
	return (pNode != NULL);
}

CRowTableFileNode* CRowTableFileNode::IntGetLeafNode(vector<CVariant*>* pArgList)
{
	Ast(pArgList);

	size_t stSize = pArgList->size();
	Ast(stSize > 0);

	if (stSize == 1)
	{
		return this;
	}
	else
	{
		CRowTableFileNode* pSubData = this;
		for (size_t i = 0; i < (stSize - 1); i++)
		{
			CVariant* pVariant = pArgList->at(i);
			const char* szIndexName = pVariant->GetCStr();

			pSubData = pSubData->GetSubNode(szIndexName);
			if(!pSubData)
			{
				ostringstream strm;
				strm << "不存在对应的节点, 输入索引: ";
				for(size_t i = 0; i < stSize; i++)
				{
					CVariant* pVar = pArgList->at(i);
					strm << pVar->GetCStr() << "、";
				}
				GenNodeErr("CRowTableFileNode::IntGetLeafNode Error", strm.str().c_str());
			}
		}
		return pSubData;
	}
}

static const char* szNullStr = 0;

ETableFileFieldType CRowTableFileNode::GetFieldType(vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetFieldType(szNullStr);
	}

	CRowTableFileNode* pSubData = IntGetLeafNode(pArgList);

	CVariant* pVariant = pArgList->at(pArgList->size() - 1);

	return pSubData->GetFieldType(pVariant->GetCStr());
}


const char* CRowTableFileNode::GetString(vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetString(szNullStr);
	}

	CRowTableFileNode* pSubData = IntGetLeafNode(pArgList);

	CVariant* pVariant = pArgList->at(pArgList->size() - 1);

	return pSubData->GetString(pVariant->GetCStr());

}

template<typename ValueType>
ValueType CRowTableFileNode::GetNumber(vector<CVariant*>* pArgList)
{
	if (!pArgList || pArgList->empty())
	{
		return GetNumber<ValueType>(szNullStr);
	}

	CRowTableFileNode* pSubData = IntGetLeafNode(pArgList);

	CVariant* pVariant = pArgList->at(pArgList->size() - 1);

	return pSubData->GetNumber<ValueType>(pVariant->GetCStr());
}


template int8 CRowTableFileNode::GetNumber<int8>(vector<CVariant*>* pArgList);
template uint8 CRowTableFileNode::GetNumber<uint8>(vector<CVariant*>* pArgList);
template int16 CRowTableFileNode::GetNumber<int16>(vector<CVariant*>* pArgList);
template uint16 CRowTableFileNode::GetNumber<uint16>(vector<CVariant*>* pArgList);
template int32 CRowTableFileNode::GetNumber<int32>(vector<CVariant*>* pArgList);
template uint32 CRowTableFileNode::GetNumber<uint32>(vector<CVariant*>* pArgList);
template int64 CRowTableFileNode::GetNumber<int64>(vector<CVariant*>* pArgList);
template uint64 CRowTableFileNode::GetNumber<uint64>(vector<CVariant*>* pArgList);
template float CRowTableFileNode::GetNumber<float>(vector<CVariant*>* pArgList);
template double CRowTableFileNode::GetNumber<double>(vector<CVariant*>* pArgList);

CVariant* CRowTableFileNode::GetValue(CTableFileValue* pValue)
{
	switch(m_eType)
	{
	case eRTNT_Tree:
	case eRTNT_LeafMoreData:
		{
			pValue->SetPtr(this);
			return pValue->GetValue();
		}
		break;
	case eRTNT_LeafNoData:
		{
			return NULL;
		}
		break;
	case eRTNT_LeafOneData:
		{
			return GetDataByName(pValue, NULL);
		}
		break;
	}

	return NULL;
}

CVariant* CRowTableFileNode::GetValue(CTableFileValue* pValue, const char* szIndex)
{
	if (m_eType == eRTNT_Tree)
	{
		CRowTableFileNode* pSubNode = GetSubNode(szIndex);
		if (pSubNode)
		{
			return pSubNode->GetValue(pValue);
		}
		return NULL;
	}
	else
	{
		return GetDataByName(pValue, szIndex);
	}
}

CVariant* CRowTableFileNode::GetValue(CTableFileValue* pValue, vector<CVariant*>* pArgList)
{
	if(!pArgList || pArgList->empty())
	{
		return GetValue(pValue);
	}

	CRowTableFileNode* pSubData = IntGetLeafNode(pArgList);

	CVariant* pVariant = pArgList->at(pArgList->size() - 1);

	return pSubData->GetValue(pValue, pVariant->GetCStr());
}

