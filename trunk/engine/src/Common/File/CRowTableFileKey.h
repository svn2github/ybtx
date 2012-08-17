#pragma once 
#include "TableFileHelper.h"
#include "CDataFileAllocator.h"
#include "TMemPoolAllocator.h"

class TiXmlNode;

namespace sqr 
{
	/*
		下面几个类是用来解析row table file对应的xml文件使用的
		每一个row table file都有一个xml文件，里面有这个row table file的
		关键字列表，我们首先解析这个关键字列表，然后在解析txt文件的时候
		通过这个关键字列表进行下一步的处理
	*/

	class CRowTableFileImp;
	class CLeafNodeDataMeta;
	class CMemPool;

	class CRowTableFileField
	{
	public:
		friend class CRowTableFileImp;

		CRowTableFileField(TiXmlNode* pNode, CRowTableFileImp* pImp, CMemPool* pPool);
		ETableFileFieldType GetFieldType(int32 nColIndex) { return m_vecFieldType[nColIndex]; }
		const char* GetFieldKey(int32 nColIndex) { return m_vecFieldKey[nColIndex]; }

		int32 FieldNum() { return (int32)m_vecFieldType.size(); }
		int32 DataNum() { return m_nDataNum; }

		bool Equal(const CRowTableFileField& Other);

		ERowTableNodeType LeafNodeType() { return m_LeafNodeType; }

	private:
		typedef TMemPoolAllocator<const char*>				MemIndexAlloc_t;
		typedef TMemPoolAllocator<ETableFileFieldType>		MemTypeAlloc_t;

		typedef vector<const char*, MemIndexAlloc_t>			VecKey_t;
		typedef vector<ETableFileFieldType, MemTypeAlloc_t>		VecType_t;

		VecKey_t				m_vecFieldKey;
		VecType_t				m_vecFieldType;
		CRowTableFileImp*		m_pImp;

		ERowTableNodeType		m_LeafNodeType;
		int32					m_nDataNum;
	};

	enum ETableFileKeyType
	{
		eTFKT_Tree = 0,	//表明这一个key是tree类型的，他的下面还有其子节点
		eTFKT_Leaf,			//这个key是leaf类型的，他的下面就是数据field的定义了
	};

	class CRowTableFileKey
	{
	public:
		friend class CRowTableFileImp;;

		virtual ~CRowTableFileKey(){}
		virtual ETableFileKeyType GetType() = 0;

		virtual bool Equal(const CRowTableFileKey* pOther) = 0;

		static void* operator new(size_t stSize, CMemPool* pPool);
		static void operator delete(void* pMem, CMemPool* pPool){ (void)pMem; (void)pPool; }
		static void operator delete(void* pMem) { (void)pMem; }
	};

	enum ETableFileIndexKeyType
	{
		eTFIKT_Name = 0,   //名字索引
		eTFIKT_Default,    //默认索引
		eTFIKT_Random,	   //任意索引
	};

	class CRowTableFileTreeKey : public CRowTableFileKey
	{
	public:
		friend class CRowTableFileKey;
		friend class CRowTableFileImp;

		CRowTableFileTreeKey (TiXmlNode* pRootNode, CRowTableFileImp* pImp, CMemPool* pPool); 
		~CRowTableFileTreeKey();

		ETableFileKeyType GetType() { return eTFKT_Tree; }					

		CRowTableFileKey* GetSubKey(const char* szKeyName);

		//是否是名字关键字还是默认关键字
		ETableFileIndexKeyType GetIndexKeyType() { return m_eIndexKeyType; }

		bool Equal(const CRowTableFileKey* pOther);

	private:
		CRowTableFileKey* CreateSubKey(TiXmlNode* pNode, CMemPool* pPool);

	private:
		typedef pair<const char*, CRowTableFileKey*>	PairIndex2Key_t;
		typedef TMemPoolAllocator<PairIndex2Key_t>	MemIndexKeyAlloc_t;

		typedef vector<PairIndex2Key_t, MemIndexKeyAlloc_t> VecIndex2Key_t;

		VecIndex2Key_t*					m_pVecIndex2Key;
		ETableFileIndexKeyType			m_eIndexKeyType;
		CRowTableFileImp*				m_pImp;
	};

	class CRowTableFileLeafKey : public CRowTableFileKey
	{
	public:
		friend class CRowTableFileKey;
		friend class CRowTableFileImp;

		CRowTableFileLeafKey(TiXmlNode* pRootNode, CRowTableFileImp* pImp, CMemPool* pPool);
		~CRowTableFileLeafKey();

		ETableFileKeyType GetType() { return eTFKT_Leaf; }

		CRowTableFileField& GetField() { return m_Field; }		

		bool Equal(const CRowTableFileKey* pOther);

	private:
		CRowTableFileField		m_Field;
	};

}

