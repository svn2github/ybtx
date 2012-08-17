#include "stdafx.h"
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <stdexcept>
#ifdef _WIN32
#include <direct.h>
#else
#define _chdir chdir
#endif

#include "CScript.h"
#include "CXmlConfig.inl"
#include "CPathMgr.h"

//#include "NetworkTest.h"
#include "CoreServerTest.h"
#include "CommonTest.h"
#include "ScriptTest.h"
#include "DatabaseTest.h"
#include "CoreScriptTest.h"
#include "GraphicTest.h"
#include "CoreClientTest.h"
#include "CoreCommonTest.h"
#include "CIniFile.h"
#include "AoiTest.h"
#include "TripleBufferTest.h"

#include "CBaseScriptApp.h"
#include "ExpHelper.h"

class CEngineTester
	:public CBaseScriptApp
{
};


int main( int argc, TCHAR* argv[] )
{
	//设置不coredump，只抛异常
	ifstream TSfile("../../etc/tools/TestSuite.xml");
	if(!TSfile)
		GenErr("Can't open etc/tools/TestSuite.xml");

	CXmlConfig *pTestXmlFile = new CXmlConfig( "test_suite" , TSfile );
	TSfile.close();

	CEngineTester App;//这一句切换了路径，设置了脚本搜索路径
	
	//// Retreive test path from command line first argument. Default to "" which resolve
	//// to the top level suite.
	std::string testPath = (argc > 1) ? std::string(argv[1]) : std::string("");

	// Create the event manager and test controller
	CPPUNIT_NS::TestResult controller;

	// Add a listener that colllects test result
	CPPUNIT_NS::TestResultCollector result;
	controller.addListener( &result );        


	// Add a listener that print dots as test run.
	CPPUNIT_NS::TextTestProgressListener progress;
	controller.addListener( &progress );      

	//init test suite
	//RegTestSuiteIDPtrMap();
	//RegTestSuiteAoi();
	//RegTestSuiteMetaSceneServer();
	//RegTestSuiteTickMgr();
	//RegTestSuiteScript();
	//RegTestSuiteRpcCall();
	//RegTestSuitePkgFile();
	//RegTestSuiteNetwork();
	//RegTestSuiteDbThreadsMgr();
	//RegTestSuitePatternCOR();
	//RegTestSuiteIniFile();
	//RegTestSuiteTableFile();
	//RegTestSuitePatternDlg();
	//RegTestSuiteVariant();
	//RegTestSuiteTripleBuffer();

	//RegTestSuiteSafeContainer();
	RegTestSuiteRenderSlot();

	// Add the top suite to the test runner
	CPPUNIT_NS::TestRunner runner;

	TiXmlNode *pTestSuiteName = pTestXmlFile->Get<TiXmlElement*>("name");
	while(pTestSuiteName)
	{
		TiXmlElement *pElement = pTestSuiteName->ToElement();
		if (!pElement)
			continue;

		string strText = pElement->GetText();
		
		runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry(strText.c_str()).makeTest() );

		pTestSuiteName = pTestSuiteName->NextSibling("name");
	}

	delete pTestXmlFile;
	pTestXmlFile = NULL;
	//init test suite

	SQR_TRY
	{
		std::cout << "Running "  <<  testPath;
		runner.run( controller, testPath );

		std::cerr << std::endl;

		// Print test in a compiler compatible format.
		CPPUNIT_NS::CompilerOutputter outputter( &result, std::cerr );
		outputter.write(); 

		// Uncomment this for XML output
		//    std::ofstream file( "tests.xml" );
		//    CPPUNIT_NS::XmlOutputter xml( &result, file );
		//    xml.setStyleSheet( "report.xsl" );
		//    xml.write();
		//    file.close();
	}
	SQR_CATCH(e)  // Test path not resolved
	{
		std::cerr  <<  std::endl  
			<<  "ERROR: "  <<  e.ErrorMsg()
			<< std::endl;
		return 0;
	}
	SQR_TRY_END;
	getchar();

	return result.wasSuccessful() ? 0 : 1;
}
