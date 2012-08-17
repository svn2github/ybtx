#pragma once
#include <queue>
#include "TIDMapAllocator.h"

namespace sqr
{
	class CTestSuiteIDPtrMap;
	template<typename ValueType>
	class TIDPtrMapIter;

	//最大容量为2^20
	template<typename ValueType>
	class TIDPtrMap
		:public CIDMapMallocObject
	{
		friend class CTestSuiteIDPtrMap;
		friend class TIDPtrMapIter<ValueType>;
	public:
		typedef TIDPtrMapIter<ValueType> Iterator;

		TIDPtrMap();
		~TIDPtrMap();

		bool Get(ValueType*& pValue,uint32 uID);
		bool Get(const ValueType*& pValue,uint32 uID)const;

		static uint32 GetIndexByID(uint32 uID);
		
		uint32 Add(ValueType* pValue);
		bool Del(uint32 uID);

		Iterator Begin();
		Iterator End();
		uint32 Size()const;
		void Clear();
	protected:
		typedef pair<ValueType*,uint16>	ValuePair_t;
		typedef vector< ValuePair_t, TIDMapAllocator<ValuePair_t> >	 VecValue_t;
		VecValue_t		m_vecValue;
		
		typedef queue< uint32, deque<uint32, TIDMapAllocator<uint32> > >	QueGapID_t;
		QueGapID_t		m_queGapID;

		bool GetValuePair(ValuePair_t*& pPair, uint32 uIndex , uint16 uCount);
	private:
		void Init();

		uint32			m_uPtrNum;
	};

}

