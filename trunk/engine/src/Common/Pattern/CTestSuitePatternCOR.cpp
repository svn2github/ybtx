#include "stdafx.h"
#include "CTestSuitePatternCOR.h"
#include "CommonTest.h"
#include "PatternCOR.h"



namespace sqr
{
	const char* RegTestSuitePatternCOR()
	{
		static TestSuiteFactory<CTestSuitePatternCOR> m_factory;
		const char* szSuiteName="CTestSuitePatternCOR";
		TestFactoryRegistry::getRegistry(szSuiteName).registerFactory(&m_factory);
		return szSuiteName;
	}
}

class CTestObserver : public CPtCORHandler
{
public:
	uint32	m_uCounter;
	CTestObserver():m_uCounter(0){}
	void OnCOREvent(CPtCORSubject* theChangedSubject,uint32 eType,void*){m_uCounter++;};
};







void CTestSuitePatternCOR::setUp()
{

}


void CTestSuitePatternCOR::tearDown()
{

}


void CTestSuitePatternCOR::TestCreation()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
}
//detach 多次，看是否有问题
void CTestSuitePatternCOR::TestDetachMore()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
	CPPUNIT_ASSERT(subject.Detach(&observer,1));
	CPPUNIT_ASSERT(!subject.Detach(&observer,1));
	CPPUNIT_ASSERT(!subject.Detach(&observer,1));
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
}
//先用type detach 在单独detach
void CTestSuitePatternCOR::TestDetachAll()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
	subject.DetachAll();
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
}
//在多个事件上注册
void CTestSuitePatternCOR::TestDetachDiff()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Attach(&observer,2);
	subject.Notify(1);
	subject.Notify(2);
	CPPUNIT_ASSERT_EQUAL((uint32)2,observer.m_uCounter);
	subject.Detach(&observer,1);
	subject.Notify(1);
	subject.Notify(2);
	CPPUNIT_ASSERT_EQUAL((uint32)3,observer.m_uCounter);
}

//在多个事件上注册,并且自己主动释放
void CTestSuitePatternCOR::TestDetachAllInDiff()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Attach(&observer,2);
	subject.Notify(1);
	subject.Notify(2);
	CPPUNIT_ASSERT_EQUAL((uint32)2,observer.m_uCounter);
	observer.DetachAll();
	subject.Notify(1);
	subject.Notify(2);
	CPPUNIT_ASSERT_EQUAL((uint32)2,observer.m_uCounter);
}


void CTestSuitePatternCOR::TestAttachDetach()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
	subject.Detach(&observer,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
}
void CTestSuitePatternCOR::TestAttachDel()
{
	CTestObserver *observer = new CTestObserver;
	CPtCORSubject *subject = new CPtCORSubject(5);
	subject->Attach(observer,1);
	delete observer;
	subject->Notify(1);
	delete subject;
}

void CTestSuitePatternCOR::TestDetachHandler()
{
	CTestObserver observer;
	CPtCORSubject subject(5);
	subject.Attach(&observer,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
	observer.DetachAll();
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer.m_uCounter);
}

void CTestSuitePatternCOR::TestAttachMore()
{
	CTestObserver observer1,observer2;
	CPtCORSubject subject(5);
	subject.Attach(&observer1,1);
	subject.Attach(&observer2,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)1,observer2.m_uCounter);
}

void CTestSuitePatternCOR::TestAttach2Join()
{
	CTestObserver observer1,observer2;
	CPtCORSubject *subject1 = new CPtCORSubject(5);
	CPtCORSubject subject2(5);
	subject1->Attach(&observer1,1);
	subject1->Attach(&observer2,1);
	subject2.Attach(&observer1,1);
	subject2.Attach(&observer2,1);
	subject1->Notify(1);
	subject2.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)2,observer1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)2,observer2.m_uCounter);
	delete subject1;
	subject2.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)3,observer1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)3,observer2.m_uCounter);
}


void CTestSuitePatternCOR::TestAttached()
{
	CTestObserver observer1,observer2;
	CPtCORSubject subject(5);
	subject.Attach(&observer1,1);
	subject.Attach(&observer2,2);
	subject.Attach(&observer1,3);
	subject.Attach(&observer2,4);

	CPPUNIT_ASSERT(subject.Attached(&observer1,1));
	CPPUNIT_ASSERT(!subject.Attached(&observer2,1));
	CPPUNIT_ASSERT(subject.Attached(1));
	CPPUNIT_ASSERT(subject.Attached(2));
	CPPUNIT_ASSERT(subject.Attached(3));
	CPPUNIT_ASSERT(subject.Attached(4));
	CPPUNIT_ASSERT(!subject.Attached(5));
}



class CDetachSefObserver : public CPtCORHandler
{
public:
	uint32	m_uCounter;
	CDetachSefObserver():m_uCounter(0){}
	void OnCOREvent(CPtCORSubject* theChangedSubject,uint32 eType,void*){m_uCounter++;theChangedSubject->Detach(this,eType); };
};

//在回掉中Detach自己
void CTestSuitePatternCOR::TestOnEventDetachSelf()
{
	CDetachSefObserver DetachSef;
	CPtCORSubject subject(5);
	subject.Attach(&DetachSef,1);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DetachSef.m_uCounter);
	subject.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DetachSef.m_uCounter);
}

class CDetachAllObserver : public CPtCORHandler
{
public:
	uint32	m_uCounter;
	CDetachAllObserver():m_uCounter(0){}
	void OnCOREvent(CPtCORSubject* theChangedSubject,uint32 eType,void*){m_uCounter++;this->DetachAll(); };
};

//在回掉中把和自己有关系的全部detach掉
void CTestSuitePatternCOR::TestOnEventDetachAll()
{
	CDetachAllObserver DetachAll;
	CPtCORSubject subject1(5),subject2(5);
	subject1.Attach(&DetachAll,1);
	subject2.Attach(&DetachAll,1);
	subject1.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DetachAll.m_uCounter);
	subject2.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DetachAll.m_uCounter);
}


//在回掉中把csubject del掉
class CDelSubject : public CPtCORHandler
{
public:
	uint32	m_uCounter;
	CDelSubject():m_uCounter(0){}
	void OnCOREvent(CPtCORSubject* theChangedSubject,uint32 eType,void*)
	{
		m_uCounter++;
		delete theChangedSubject; 
	}
};

void CTestSuitePatternCOR::TestOnEventDelSubject()
{
	CDelSubject DelSub;
	CPtCORSubject *subject1 = new CPtCORSubject(5);
	CPtCORSubject *subject2 = new CPtCORSubject(5);
	subject1->Attach(&DelSub,1);
	subject2->Attach(&DelSub,1);
	subject1->Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelSub.m_uCounter);
	subject2->Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)2,DelSub.m_uCounter);
}


//在回掉中删除别人的关系
class CDelOther : public CPtCORHandler
{
public:
	uint32	m_uCounter;
	CDelOther():m_uCounter(0){}
	void OnCOREvent(CPtCORSubject* theChangedSubject,uint32 eType,void*)
	{
		m_uCounter++;
		theChangedSubject->DetachAll(); 
	}
};

void CTestSuitePatternCOR::TestOnEventDetachOther()
{
	CDelOther DelOther1,DelOther2;
	CPtCORSubject subject1(5);
	subject1.Attach(&DelOther1,1);
	subject1.Attach(&DelOther2,1);
	subject1.Attach(&DelOther1,2);

	subject1.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther2.m_uCounter);

	subject1.Notify(1);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther2.m_uCounter);

	subject1.Notify(2);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther1.m_uCounter);
	CPPUNIT_ASSERT_EQUAL((uint32)1,DelOther2.m_uCounter);
}
