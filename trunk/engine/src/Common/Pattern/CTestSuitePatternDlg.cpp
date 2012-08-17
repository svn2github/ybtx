#include "stdafx.h"
#include "CTestSuitePatternDlg.h"
#include "CommonTest.h"
#include "PatternDlg.h"

namespace sqr
{
	const char* RegTestSuitePatternDlg()
	{
		static TestSuiteFactory<CTestSuitePatternDlg> m_factory;
		const char* szSuiteName="CTestSuitePatternDlg";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}

void CTestSuitePatternDlg::setUp()
{
}

void CTestSuitePatternDlg::tearDown()
{
}

void CTestSuitePatternDlg::TestAdd()
{
    typedef TPtManagedDlg1<int&> Dlg;
    int a = 0;
    class Callback : public Dlg::CallbackType
    {
        int exec(int &a)
        {
            ++a;
            return true;
        }
    };
    Dlg ev;
    ev += new Callback;
    ev(a);
    CPPUNIT_ASSERT_EQUAL(a, 1);
};
void CTestSuitePatternDlg::TestRemove()
{
    typedef TPtManagedDlg1<int&> Dlg1;
    typedef TPtManagedDlg0 Dlg0;
    typedef TPtManagedDlg2<int&, int&> Dlg2;
    typedef TPtManagedDlg3<int&, int&, int&> Dlg3;
    int a = 0;
    typedef void (*fp) (int*& a);
    fp fp1 = NULL;
    fp fp2 = NULL;
    if (fp1 == fp2)
    {
    }
    struct Func
    {
        static int func(int &a)
        {
            a += 2;
            return 0;
        }
    };
    class Callback : public Dlg1::CallbackType
    {
        int exec(int &a)
        {
            ++a;
            return true;
        }
    };
    Dlg1 ev;
    Callback *cb = new Callback;
    ev += cb;
    ev(a);
    CPPUNIT_ASSERT_EQUAL(a, 1);
    ev += Func::func;
    ev(a);
    CPPUNIT_ASSERT_EQUAL(a, 4);
    ev -= cb;
    ev(a);
    CPPUNIT_ASSERT_EQUAL(a, 6);
    ev -= &Func::func;
    ev(a);
    CPPUNIT_ASSERT_EQUAL(a, 6);
};
