#pragma once 
#include "CDataFileAllocator.h"
#include "TableFileHelper.h"

namespace sqr 
{
	/*
	对于每一个leaf node来说，它其实是有一个原型的，因为我们通过xml定义一个leaf node之后
	以后所有根据leaf node的配置来创建的leaf node，除了实际数据不一样之外，他的所有操作
	都是一样的
	*/

	class CLeafFieldInfo
	{
	public:
		CLeafFieldInfo()
			: nDataOffset(-1)
			, eFieldType(eTFFT_Empty) {}
		CLeafFieldInfo(int32 nOffset, ETableFileFieldType eType) 
			: nDataOffset(nOffset)
			, eFieldType(eType){}

		int32				nDataOffset;
		ETableFileFieldType eFieldType;
	};

	class CRowTableFileField;
	class CLeafNodeDataMeta : public CDataFileMallocObject
	{
		friend class CRowTableFileNode;

	public:
		CLeafNodeDataMeta(CRowTableFileField& RowField);
		~CLeafNodeDataMeta();

		int32 DataNum() { return (int32)m_vecDataFieldInfo.size(); }
		CLeafFieldInfo& GetFieldInfo(const char* szKey);
		
		typedef pair<const char*, CLeafFieldInfo>	PairKey2Field_t;
		typedef vector<PairKey2Field_t,
			CDataFileAllocator<PairKey2Field_t> >	VecKey2Field_t;
	
	private:
		static bool LessField(const PairKey2Field_t& Left, const PairKey2Field_t& Right);

		VecKey2Field_t	m_vecDataFieldInfo;
	};
}

