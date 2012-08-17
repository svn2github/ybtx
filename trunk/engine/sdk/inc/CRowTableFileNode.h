#pragma once 
#include "CommonDefs.h"
#include "CShareObject.h"
#include "CDataFileAllocator.h"
#include "TableFileHelper.h"
#include "CStaticObject.h"
#include <stack>

namespace sqr 
{
	class CRowTableFileField;
	class CRowTableFileImp;
	class CVariant;
	class CConstStringPool;
	class CTableFileValue;
	class CLeafNodeDataMeta;


	class COMMON_API CRowTableNodeKeys
		: public CStaticObject
		, public CDataFileMallocObject
	{
	public:
		CRowTableNodeKeys();
		~CRowTableNodeKeys();
		
		vector<CVariant*>* GetKeys() { return &m_vecKey; }
		void Clear();


	private:
		friend class CRowTableFileNode ;
		void AddKey(const char* szName);
		void ReserveKey(size_t stSize);

		CVariant* CreateVariant();
		void DestroyVariant(CVariant* pVariant);

	private:
		vector<CVariant*>		m_vecKey;

		//因为这个类可以重复使用，我们为了效率就用一个对象池来保存variant
		stack<CVariant*, deque<CVariant*, CDataFileAllocator<CVariant*> >	> m_stkKeyBank;
	};


	class COMMON_API CRowTableFileNode
		: public CShareObject
		, public CDataFileMallocObject
	{
	public:
		CRowTableFileNode();
		virtual ~CRowTableFileNode();


		//查找该节点本来属于的文件
		const char* GetFileName();

		ERowTableNodeType GetType() { return m_eType; }
		
		//for tree data
		CRowTableFileNode* GetSubNode(const char* szSubKey);
		vector<CVariant*>* GetSubNodeKeys(CRowTableNodeKeys* pDataKeys);
		int32 GetSubNodeNum();

		bool HasSubNode(const char* szSubKey);

		//for leaf data
		ETableFileFieldType GetFieldType(const char* szFieldName);
		const char* GetString(const char* szFieldName);
		template<typename ValueType>
		ValueType GetNumber(const char* szFieldName);
		
		//下面是可变参数列表
		ERowTableNodeType GetType(vector<CVariant*>* pArgList);

		CRowTableFileNode* GetSubNode(vector<CVariant*>* pArgList);
		
		vector<CVariant*>* GetSubNodeKeys(CRowTableNodeKeys* pKeys, vector<CVariant*>* pArgList);
		
		int32 GetSubNodeNum(vector<CVariant*>* pArgList);
		
		bool HasSubNode(vector<CVariant*>* pArgList);

		//下面这几个函数跟上面不一样的载入arglist的最后一个参数必须为fieldname，这样才能取到数据
		ETableFileFieldType GetFieldType(vector<CVariant*>* pArgList);
		const char* GetString(vector<CVariant*>* pArgList);		
		template<typename ValueType>
		ValueType GetNumber(vector<CVariant*>* pArgList);
		
		//这个函数可以获取到node或者data，返回一个variant，这个variant会绑定这些数据
		CVariant* GetValue(CTableFileValue* pValue, vector<CVariant*>* pArgList);

		CVariant* GetValue(CTableFileValue* pValue);
		CVariant* GetValue(CTableFileValue* pValue, const char* szIndex);

	private:
		friend class CRowTableFileImp;
		friend class CRowTableNodeKeys;

		CRowTableFileNode(CRowTableFileImp* pImp, const char* szFileName);

		void SetType(ERowTableNodeType eType) { m_eType = eType; }

		CRowTableFileNode* AddSubNode(const char* szSubName, const char* szFileName);
		void SetFieldData(int32 nRow, int32 nColOffset, 
			CRowTableFileField& RowField, CLeafNodeDataMeta* pMeta,const char * FILENAME,int32 ROW1);

		void ResetFieldData(int32 nRow, int32 nColOffset, CRowTableFileField& RowField);

		void SortIndex2Node();

		void AstTreeNodeType(const char* szMsg);
		void AstLeafNodeType(const char* szMsg);
		
		CVariant* GetDataByName(CTableFileValue* pValue, const char* szName);
		
		CRowTableFileNode* IntGetLeafNode(vector<CVariant*>* pArgList);

		CConstStringPool* GetStringPool();

		void GenNodeErr(const char* szType, const char* szMsg);
		void GenLoadErr(int32 nRow, const char* szType, const char* szMsg = NULL);

	private:
	
		typedef pair<const char*, CRowTableFileNode*>	PairIndex2Node_t;
		typedef vector<PairIndex2Node_t, CDataFileAllocator<PairIndex2Node_t> >	VecIndex2Node_t;
		
		struct TreeNode : public CDataFileMallocObject
		{
			VecIndex2Node_t	m_vecIndex2Node;
		};
	
		struct LeafNodeData : public CDataFileMallocObject
		{
			LeafNodeData() : m_eFieldType(eTFFT_Empty), m_pData(NULL){}
			ETableFileFieldType	m_eFieldType;
			void*				m_pData;
		};

		CVariant* GetLeafNodeData(CTableFileValue* pValue, LeafNodeData& data);
		LeafNodeData GetDataByName(const char* szName);

		typedef pair<const char*, LeafNodeData>	PairIndex2Data_t;
		typedef vector<PairIndex2Data_t, CDataFileAllocator<PairIndex2Data_t> >	VecIndex2Data_t;
		
		static bool LessNode(const PairIndex2Node_t& Left, const PairIndex2Node_t& Right);
		static bool LessData(const PairIndex2Data_t& Left, const PairIndex2Data_t& Right);

		typedef vector<void*, CDataFileAllocator<void*> >	VecLeafData_t;
		struct LeafNode : public CDataFileMallocObject
		{};

		struct LeafDataNode : public LeafNode 
		{
			VecLeafData_t		m_vecData;
			CLeafNodeDataMeta*	m_pMeta;			
		};

		struct LeafNoDataNode : public LeafNode 
		{};

		union Content
		{
			TreeNode*		m_pTreeNode;
			LeafNode*		m_pLeafNode;
		};

		CRowTableFileNode* CreateNode(CRowTableFileImp* pImp, const char* szFileName);
		void DestroyNode(CRowTableFileNode* pNode);
		TreeNode* CreateTreeNode();
		void DestroyTreeNode(TreeNode* pNode);
		LeafDataNode* CreateLeafNode();
		void DestroyLeafNode(LeafDataNode* pNode);

	
	private:
		ERowTableNodeType	m_eType;
		
		Content			m_Content;

		CRowTableFileImp*	m_pImp;

		const char*			m_szNodeFileName;
		
	};

}
