#include "stdafx.h"
#include "CommonTest.h"
#include "CTestSuiteSafeContainer.h"

#include "TSafeList.inl"
#include "TSafeMap.inl"
#include "TSafeSet.inl"

namespace sqr
{
	const char* RegTestSuiteSafeContainer()
	{
		static TestSuiteFactory<CTestSuiteSafeContainer> m_factory;
		const char* szSuiteName="CTestSuiteSafeContainer";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}

	void CTestSuiteSafeContainer::setUp()
	{
	}

	void CTestSuiteSafeContainer::tearDown()
	{
	}

	void CTestSuiteSafeContainer::TestListEmpty()
	{
		TSafeList<int> test;
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.size()));
		CPPUNIT_ASSERT(test.empty());
		CPPUNIT_ASSERT(test.begin() == test.end());
	}
	void CTestSuiteSafeContainer::TestListModify()
	{
		TSafeList<int> test1;
		test1.push_back(1);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test1.size()));
		test1.push_front(0);
		CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(test1.size()));
		size_t size1 = test1.size();
		TSafeList<int> test2(test1);
		CPPUNIT_ASSERT_EQUAL(test1.size(), test2.size());
		test1.clear();
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test1.size()));
		CPPUNIT_ASSERT_EQUAL(size1, test2.size());

		CPPUNIT_ASSERT_EQUAL(0, test2.front());
		CPPUNIT_ASSERT_EQUAL(1, test2.back());
		test2.pop_front();
		CPPUNIT_ASSERT_EQUAL(1, test2.front());
		CPPUNIT_ASSERT_EQUAL(1, test2.back());
		test2.pop_back();
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test2.size()));
	}
	void CTestSuiteSafeContainer::TestListComplexModify()
	{
		TSafeList<int> test;
		for(int i = 0; i < 5; i++)
			test.push_back(i);
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));

		// 测迭代器自动回收功能
		{
			TSafeList<int>::iterator it = test.begin();
			for(; it != test.end(); it++)
			{
				int v = *it;
				// 测嵌套修改功能
				test.erase(it);
				test.erase(it);
				test.push_front(v);
			}
			// 测删除备忘录
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_erase_note().size()));
			CPPUNIT_ASSERT_EQUAL(10, static_cast<int>(test.get_base_container().size()));
		}

		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.get_erase_note().size()));
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_base_container().size()));
	}

	/// @brief 确保map在增删时其他迭代器不会失效
	void CTestSuiteSafeContainer::TestSTLMapIteratorStable()
	{
		std::map<int, int> test;
		test[1024] = 1024;
		std::map<int, int>::iterator it = test.begin();
		for(int i = 0; i < 1024; i++)
			test[i] = i;
		for(int i = 1024 + 1; i < 2048; i++)
			test[i] = i;
		CPPUNIT_ASSERT_EQUAL(1024, it->second);
		for(int i = 1024 + 1; i < 2048; i++)
			test.erase(i);
		CPPUNIT_ASSERT_EQUAL(1024, it->second);
		for(int i = 0; i < 1024; i++)
			test.erase(i);
		CPPUNIT_ASSERT_EQUAL(1024, it->second);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test.size()));
	}

	void CTestSuiteSafeContainer::TestMapEmpty()
	{
		TSafeMap<int, int> test;
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.size()));
		CPPUNIT_ASSERT(test.empty());
		CPPUNIT_ASSERT(test.begin() == test.end());
	}
	void CTestSuiteSafeContainer::TestMapModify()
	{
		TSafeMap<int, int> test1;
		test1.insert(0, 0);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test1.size()));
		CPPUNIT_ASSERT_EQUAL(0, test1[0]);
		test1.insert(0, 1);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test1.size()));
		CPPUNIT_ASSERT_EQUAL(0, test1[0]);
		test1[0] = 1;
		CPPUNIT_ASSERT_EQUAL(1, test1[0]);
		test1.insert(1, 2);
		CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(test1.size()));
		size_t size1 = test1.size();
		TSafeMap<int, int> test2(test1);
		test1.clear();
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test1.size()));
		CPPUNIT_ASSERT_EQUAL(size1, test2.size());

		CPPUNIT_ASSERT_EQUAL(1, test2[0]);
		CPPUNIT_ASSERT_EQUAL(2, test2[1]);
		test2.erase(0);
		test2.erase(1);
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test2.size()));
	}
	void CTestSuiteSafeContainer::TestMapComplexModify()
	{
		TSafeMap<int, int> test;
		for(int i = 0; i < 5; i++)
			test[i] = i;
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));

		// 测迭代器自动回收功能
		{
			TSafeMap<int, int>::iterator it = test.begin();
			for(; it != test.end(); it++)
			{
				std::pair<int, int> v = *it;
				// 测嵌套修改功能
				test.erase(it);
				test.erase(it);
				test.insert(v);
			}
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
			// 测键值重用功能
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.get_erase_note().size()));
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_base_container().size()));
		}

		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.get_erase_note().size()));
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_base_container().size()));
	}

	void CTestSuiteSafeContainer::TestSetEmpty()
	{
		TSafeSet<int> test;
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.size()));
		CPPUNIT_ASSERT(test.empty());
		CPPUNIT_ASSERT(test.begin() == test.end());
	}
	void CTestSuiteSafeContainer::TestSetModify()
	{
		TSafeSet<int> test1;
		test1.insert(0);
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test1.size()));
		test1.insert(1);
		CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(test1.size()));
		test1.insert(1);
		CPPUNIT_ASSERT_EQUAL(2, static_cast<int>(test1.size()));

		size_t size1 = test1.size();
		TSafeSet<int> test2(test1);
		CPPUNIT_ASSERT_EQUAL(test1.size(), test2.size());
		test1.clear();
		CPPUNIT_ASSERT_EQUAL(size1, test2.size());

		CPPUNIT_ASSERT(test2.exist(0));
		CPPUNIT_ASSERT_EQUAL(1, static_cast<int>(test2.count(1)));
		test2.erase(0);
		test2.erase(1);
		CPPUNIT_ASSERT(test2.empty());
	}
	void CTestSuiteSafeContainer::TestSetComplexModify()
	{
		TSafeSet<int> test;
		for(int i = 0; i < 5; i++)
			test.insert(i);
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));

		// 测迭代器自动回收功能
		{
			TSafeSet<int>::iterator it = test.begin();
			for(; it != test.end(); ++it)
			{
				int v = *it;
				// 测嵌套修改功能
				test.erase(it);
				test.erase(it);
				test.insert(v);
			}
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
			// 测键值重用功能
			CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.get_erase_note().size()));
			CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_base_container().size()));
		}

		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.size()));
		CPPUNIT_ASSERT_EQUAL(0, static_cast<int>(test.get_erase_note().size()));
		CPPUNIT_ASSERT_EQUAL(5, static_cast<int>(test.get_base_container().size()));
	}
}


