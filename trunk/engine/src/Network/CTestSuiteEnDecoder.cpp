#include "stdafx.h"
#include <cstring>
#include "CTestSuiteEnDecoder.h"
#include "CByteEncoder.inl"
#include "CByteDecoder.inl"

namespace sqr
{
	const char* GetTestSuiteEnDecoderName()
	{
		static TestSuiteFactory<CTestSuiteEnDecoder> m_factory;
		const char* szSuiteName="CTestSuiteEnDecoder";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}

void CTestSuiteEnDecoder::setUp()
{

}

void CTestSuiteEnDecoder::tearDown()
{

}

void CTestSuiteEnDecoder::TestEnCodeAndDecode()
{
	struct Content
	{
		uint8	uID;
		char szTest[20];
		uint32	uTest;
	};

	CByteEncoder encoder;
	CByteDecoder decoder;

	Content Test;
	strcpy(Test.szTest, "hello world!");
	Test.uID = 50;
	Test.uTest = 4423424;

	unsigned key = encoder.Reset();
	decoder.Reset(key);

	uint8* pSrc = (uint8*)&Test;
	uint8* pDest = new uint8[sizeof(Content)];
	uint8* pEn = pDest;

	for(size_t n=0; n < sizeof(Content); n++)
	{
		uint8 uEncode = encoder.Encode(*pSrc++);
		*pEn++ = decoder.Decode(uEncode);
	}

	CPPUNIT_ASSERT_EQUAL((int)0,memcmp(pDest,&Test,sizeof(Content)));
	delete [] pDest;
}
