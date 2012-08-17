#include "stdafx.h"
#include "CScript.h"
#include "ScriptX.h"
#include "TestScriptType.h"
#include "TestScriptClass.h"
#include "ExpHelper.h"
#include <iostream>
#include "TestSuiteScript.h"
#ifdef _WIN32
#include "float.h"
#endif

namespace sqr
{
	bool TestScript()
	{
		SQR_TRY
		{
#ifdef _WIN32
			_control87( _PC_64, MCW_PC );
#endif
			CScript& Script = GetScriptForTest();

			TypeTestRegist( Script );
			ClassTestRegist( Script );
			int r = 1000;
			string error;

			error = Script.RunFile( "common/Script/TestMain", NULL );
			if( error.empty() )
			{
				error = Script.RunFunction( "TestMain", ":i", &r );
				if( error.empty() || r == 0)
				{
					TestRunFunction( Script );
					return true;
				}
			}

			return false;
		}
		SQR_CATCH( ex )
		{
			cout << ex.ErrorMsg() << endl;
			return false;
		}
		SQR_TRY_END;

		return false;
	}

	const char* RegTestSuiteScript()
	{
		static TestSuiteFactory<CTestSuiteScript> factory;
		const char* szSuiteName="CTestSuiteScript";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&factory);

		return szSuiteName;
	}


	void CTestSuiteScript::setUp()
	{}

	void CTestSuiteScript::tearDown()
	{}

	void CTestSuiteScript::TestMain()
	{
		bool r = TestScript();
		CPPUNIT_ASSERT_EQUAL( r, true );
	}
}

