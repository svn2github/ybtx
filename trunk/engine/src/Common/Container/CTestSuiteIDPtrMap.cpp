#include "stdafx.h"
#include "CTestSuiteIDPtrMap.h"
#include "CommonTest.h"
#include "CTimeCheckPoint.h"

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4267)
#endif

#include "TIDPtrMap.inl"
#include "TSqrAllocator.inl"

namespace sqr
{
	const char* RegTestSuiteIDPtrMap()
	{
		static TestSuiteFactory<CTestSuiteIDPtrMap> m_factory;
		const char* szSuiteName="CTestSuiteIDPtrMap";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	void CTestSuiteIDPtrMap::setUp()
	{

	}


	void CTestSuiteIDPtrMap::tearDown()
	{

	}

	void CTestSuiteIDPtrMap::TestIteration()
	{
		const uint32 uElementNum = 100;
		uint32 arrayUint32[uElementNum];
		for(uint32 i = 0; i < uElementNum; ++i)
		{
			arrayUint32[i] = i;
		}

		TIDPtrMap<uint32> mapTest;
		for(uint32 i = 1; i < uElementNum; ++i)
		{
			mapTest.Add(arrayUint32 + i);
			CPPUNIT_ASSERT(mapTest.m_queGapID.size() >= 10);
		}

		TIDPtrMapIter<uint32> it = mapTest.Begin(); 
		uint32 uLastValue = 0;
		for(; it != mapTest.End(); ++it)
		{
			uint32 uValue = *it;
			CPPUNIT_ASSERT_EQUAL(uLastValue, uValue - 1);
			uLastValue = uValue;
			CPPUNIT_ASSERT(mapTest.Del(uValue));	//因为是第一次往IDPtrMap中插入元素,所以id == value
		}

		CPPUNIT_ASSERT_EQUAL(uint32(mapTest.m_vecValue.size()), uElementNum - 1 + 10);
	}

	void CTestSuiteIDPtrMap::TestPerformance()
	{
		cout << endl;
		cout << " Test Insertion " << endl;
		CTimeCheckPoint tcp;
		const uint32 uMaxInsertion = 100;

		TIDPtrMap<uint32> idmapTest;
		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			idmapTest.Add(&i);
		}
		cout << "IDMap elapsed time " << tcp.GetHDElapse() << endl;

		list<uint32> listTest;
		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			listTest.push_front(i);
		}
		cout << "list elapsed time " << tcp.GetHDElapse() << endl;

		map<uint32, uint32> mapTest;
		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			mapTest.insert(make_pair(i, i));
		}
		cout << "map elapsed time " << tcp.GetHDElapse() << endl;

		hash_map<uint32, uint32> hashmapTest;
		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			hashmapTest.insert(make_pair(i, i));
		}
		cout << "hash_map elapsed time " << tcp.GetHDElapse() << endl;

		cout << endl;
		cout << "Test Read" << endl;
		uint32* uRet = NULL;
		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			idmapTest.Get(uRet, i);
		}
		cout << "IDMap elapsed time " << tcp.GetHDElapse() << endl;

		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			mapTest.find(i);
		}
		cout << "map elapsed time " << tcp.GetHDElapse() << endl;

		tcp.SetCheckPoint();
		for(uint32 i = 1; i <= uMaxInsertion; ++i)
		{
			hashmapTest.find(i);
		}
		cout << "hash_map elapsed time " << tcp.GetHDElapse() << endl;
	}
}

#ifdef _WIN32
#pragma warning(pop)
#endif
