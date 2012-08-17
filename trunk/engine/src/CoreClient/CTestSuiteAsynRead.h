#pragma once
#include "TTestSuiteRenderBase.h"


class CTestSuiteAsynRead
	: public TestFixture
	, public TTestSuiteRenderBase<CTestSuiteAsynRead>
{
private:
	typedef TTestSuiteRenderBase<CTestSuiteAsynRead>	Parent_t;

	ICoreGraphic*	m_pCoreGraphic;
public:
	ICoreGraphic* GetGraphic()const;

	void setUp();
	void tearDown();
	void Run();
	void UpdateFrame();

	CPPUNIT_TEST_SUITE(CTestSuiteAsynRead);
	CPPUNIT_TEST(Run);
	CPPUNIT_TEST_SUITE_END();
};
