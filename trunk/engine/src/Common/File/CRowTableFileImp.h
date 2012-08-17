#pragma once 
#include "TBaseTableFileImp.h"
#include "TMemPoolAllocator.h"
#include "CDataFileAllocator.h"


namespace sqr 
{
	class CRowTableFileKey;
	class CRowTableFileField;
	class CRowTableFileNode;
	class CLeafNodeDataMeta;
	class CMemPool;
	class CConstStringPool;

	class CRowTableLoadingData;

	class CRowTableFileImp : public TBaseTableFileImp<CRowTableFileImp>
	{
	public:	
		typedef TBaseTableFileImp<CRowTableFileImp> Parent_t;
		friend class TBaseTableFileImp<CRowTableFileImp>;
		friend class CRowTableFileNode;
		friend class CRowTableFile;
		friend class CRowTableFileKey;
		friend class CRowTableFileTreeKey;
		friend class CRowTableFileLeafKey;
		friend class CRowTableFileField;
		

		CRowTableFileImp();
		~CRowTableFileImp();

		bool LoadXMLFileDerived(const wchar_t* szAliasPath, const char* szFileName);
		void SetFileRowOffsetDerived(const char* szFileName);
		bool LoadDerived();

		typedef vector<const char*, CDataFileAllocator<const char*> > VecFileName_t;

		bool ReloadDerived(VecFileName_t& vecFileName);

		const char* GetIndexByName(const char* szName, CConstStringPool* pStringPool, bool bForceInsert = false );

		const char* GetFileName(uint32 uRow);

	private:
		CRowTableFileNode* ParseFirstCol(int32 nRow, vector<const char*>& vecLastRowData, const char* szFileName);
				
		void EndLoad();

		CMemPool* GetLoadingPool() { return m_pLoadingPool; }

		CConstStringPool* GetXmlIndexPool() { return m_pXmlIndexPool; }
		CConstStringPool* GetTxtIndexPool() { return m_pTxtIndexPool; }

		double* AllocDataNumber(const char* szData);
		char* AllocDataString(const char* szData);
		
	private:
		CRowTableFileNode*			m_pRowTableRootNode;
		CRowTableFileKey*			m_pRowTableFileKey;

		/*
			我们使用两个string pool来管理我们row table file的索引
			第一个是用在解析xml文件里面使用的index name
			而第二个则是剩下的解析txt文件里面用的index name

			一般来说，对于多级索引表来说，第一级索引是由txt文件定义的
			余下的就是由xml文件定义的，也就是root node对应的为txt，剩下的所有节点
			对应的为xml
		*/
		CConstStringPool*			m_pXmlIndexPool;
		CConstStringPool*			m_pTxtIndexPool;

		//存放loading的时候解析的相关数据的，load完成之后删除
		CMemPool*					m_pLoadingPool;

		/*
			每个文件起始行号
		*/
		typedef pair<const char*, uint32>	PairFileRowOffset;
		typedef vector<PairFileRowOffset, CDataFileAllocator<PairFileRowOffset> >	VecFileRowOffset_t;
		VecFileRowOffset_t	m_vecFileRowOffset;

		static bool LessFileRow(const PairFileRowOffset& left, const PairFileRowOffset& right);

		CMemPool*					m_pDataBuffer;	//叶子数据buffer
	
		typedef vector<CLeafNodeDataMeta*, CDataFileAllocator<CLeafNodeDataMeta*> >	VecLeafNodeMeta_t;
		VecLeafNodeMeta_t			m_vecLeafNodeMeta;
		string FileName1;
	};
}
