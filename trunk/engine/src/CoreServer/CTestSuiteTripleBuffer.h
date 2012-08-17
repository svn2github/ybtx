#pragma once 

namespace sqr
{
	class CTrMirrorBuffer;
	class CMirrorBuffer;

	class CTestSuiteTripleBuffer : public TestFixture
	{
	public:
		void setUp();
		void tearDown();

		CPPUNIT_TEST_SUITE(CTestSuiteTripleBuffer);

		CPPUNIT_TEST(TestCreateAndDelete);
		CPPUNIT_TEST(TestIdleSlot);
		CPPUNIT_TEST(TestPos);
		CPPUNIT_TEST(TestQuad);
		CPPUNIT_TEST(TestQueryObject);

		CPPUNIT_TEST_SUITE_END();
	private:
		void TestCreateAndDelete();
		void TestIdleSlot();
		void TestPos();
		void TestQuad();
		void TestQueryObject();


	private:
		CTrMirrorBuffer* CreatePosBufferMgr(uint32 uDataNum);
		CTrMirrorBuffer* CreateQuadBufferMgr(uint32 uDataNum);

		uint32 m_uGridSpan;
		uint32 m_uMinSplite;
		uint32 m_uPrecision;
	};
}
