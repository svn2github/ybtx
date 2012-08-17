#include "stdafx.h"
#include "ScriptX.h"
#include "ScriptDef.h"
#include "TestScriptClass.h"
#include "float.h"
#include "math.h"

class IPureClass
	:public virtual sqr::CDynamicObject
{
public:
	static const char* GetName( IPureClass* pPureClass )
	{
		return pPureClass->OnGetName( 3, 4, "ABCDEFG" );
	}

	virtual const char* OnGetName( int32 a, int32 b, const char* c ) {
		CALL_CLASS_CALLBACK_3_RET(const char*, a, b, c)
		return NULL;
	} ;
};

class CMessage : public IMessage
{
public:
	virtual const char* MessageName()			{ return "ImplementMessage"; }
	virtual int GetMessageId()					{ return 0; }
};

class CMessageNotOver : public IMessageNotOver
{
public:
	virtual const char* MessageName()			{ return "ImplementMessage"; }
	virtual int GetMessageId()					{ return 0; }
};

class CMessageOver : public IMessageOver
{
public:
	virtual const char* MessageName()			{ return "ImplementMessage"; }
	virtual int GetMessageId()					{ return 0; }
};

class CTestScriptWnd : public IWnd
{
public:
	virtual const char* WndName()				{ return "ImplementWnd"; }
	virtual int WndProc( IMessage* pMessage )	{ return pMessage->GetMessageId(); }
	virtual IWnd* Clone()						{ return new CTestScriptWnd; }
};

class CFactory : public IFactory
{
public:
	CFactory() : m_MessageNum(0),m_WndNum(0)	{}
	IWnd*	CreateWnd()
	{
		++m_MessageNum;
		return new CTestScriptWnd;
	}

	IMessage* CreateMessage()
	{
		++m_WndNum;
		return new CMessage;
	}

	IMessage* CreateMessageNotOver()
	{
		++m_MessageNum;
		return new CMessageNotOver;
	}

	IMessage* CreateMessageOver()
	{
		++m_MessageNum;
		return new CMessageOver;
	}

	int GetMessageNum()							{ return m_MessageNum; }
	int GetWndNum()								{ return m_WndNum; }
protected:
	int m_MessageNum;
	int m_WndNum;
};

IFactory*	GetFactory()
{
	static CFactory Factory;	
	return &Factory;		//这种不在LUA垃圾回收的时候删除的对象，用weak object
}

int IWnd::s_num = 0;
int IMessage::s_num = 0;


class CPack
{
public:
	HPACKAGE hPack;
	HPACKAGE hUnPack;
	UNPACK_FUNCTION pFun;
};

CPack Test_Pack;

bool TestPack( uint32 uID, void* pCppContext, void* pCallContext, void* pBuf, size_t size )
{
	int32 n = Test_Pack.pFun( uID, Test_Pack.hUnPack, pCallContext, pBuf, size);
	return n >= 0;
}


CTestNormalFunClass* getTestNormalObj()	
{
	return new CTestNormalFunClass;
}


struct Rect
{
	int32 left;
	int32 right;
	int32 top;
	int32 bottom;
};

class CPos 
{
public:
	int32 x;
	int32 y;

	CPos() :x(-23), y(12){}
	bool Check()
	{
		return x == 54 && y == 63;
	}
};

CTestCBuf::CTestCBuf()
{
	char* p = m_buf;
	const char* str = "CTestCBuf";

	*((int*)p) = 1345121;
	p += sizeof(int);

	*((bool*)p) = false;
	p += sizeof(bool);

	*((double*)p) = 1.2154567;
	p += sizeof(double);

	strcpy( p, str );
	p += strlen(str)+1 ;

	*((bool*)p) = false	;
}

void* CTestCBuf::GetBuf()
{
	return (void*)m_buf;
}

bool CTestCBuf::Check()
{
	char* p = m_buf;
	const char* str = "has change";

	if( *((int*)p) != 543121 )
		return false;
	p += sizeof(int);

	if( *((bool*)p) != true )
		return false;
	p += sizeof(bool);

	if( *((double*)p) != 1.2154567 )
		return false;
	p += sizeof(double);

	if( strcmp( p, str ) != 0 )
		return false;
	p += strlen(str)+1 ;

	if( *((bool*)p) != false )
		return false;

	return true;
}

class CTestStaticOverLoad
{
public:
	static double add( double a, double b )
	{
		return a + b;
	}

	static int add( int a, int b )
	{
		return a + b;
	}
};

class CTestCallBackOverLoad
	:public virtual sqr::CDynamicObject
{
public:
	virtual int add( double a, double b )
	{
		CALL_CLASS_CALLBACK_2_RET_OVERLOAD("add_d", int, a, b)
		return (int)(a + b);
	}

	virtual int add( int a, int b )
	{
		CALL_CLASS_CALLBACK_2_RET_OVERLOAD("add_i", int, a, b)
		return a + b;
	}

	bool callback()
	{
		int i = add( 1 , 2 );
		if( i != 4 )		//overload in lua
			return false;

		int d = add( 1.11, 3.33 );
		if(  d != 5 )	//overload in lua
			return false;

		return true;
	}
};

class CTestCallBackReturn
	:public virtual sqr::CDynamicObject
{
public:
	virtual int Onri()				{	
		CALL_CLASS_CALLBACK_RET(int)
		return 3234134;		
	}
	virtual bool Onrb()				
	{	
		CALL_CLASS_CALLBACK_RET(bool)
		return true;		
	}
	virtual float Onrf()			
	{	
		CALL_CLASS_CALLBACK_RET(float)
		return 2.102345f;	
	}
	virtual double Onrd()			
	{	
		CALL_CLASS_CALLBACK_RET(double)
		return 45.41312124;	
	}
	virtual const char* Onrstr()	
	{	
		CALL_CLASS_CALLBACK_RET(const char*)
		return "rstr";		
	}

#ifdef _WIN32
#pragma warning(push)
#pragma warning(disable:4748)
#endif
	bool callback()
	{
		if( Onri() != 45612313 )
			return false;

		if( Onrb() != false )
			return false;

		float f = Onrf();
		
		if( fabs( f - 1.02487415f ) > FLT_EPSILON )
		        return false;

		if( Onrd() != 3.1542454451 )
		        return false;

		string str = Onrstr();
		if( str != "rlua" )
			return false;

		return true;
	}
#ifdef _WIN32
#pragma warning(pop)
#endif
};

#include "CVariant.h"

class TestVarObject : public virtual sqr::CDynamicObject
{
public:
	TestVarObject()
	{
		m_var = new CVariant; 
		m_pArgList = new vector<CVariant*>();
		CVariant *pVar = new CVariant;
		pVar->SetString("CallBack");
		m_pArgList->push_back(pVar);
		pVar = new CVariant;
		pVar->SetNumber(100);
		m_pArgList->push_back(pVar);	
	}

	~TestVarObject() 
	{ 
		delete m_var; 
		vector<CVariant*>::iterator iter = m_pArgList->begin();
		while (iter != m_pArgList->end())
		{
			delete (*iter);
			iter++;
		}

		delete m_pArgList;
	}
	void DoVarObject(CVariant *var)
	{
		EValueVariantType eType = var->GetType();

		if (eType == eVVT_Double)
		{
			double dVal = var->GetNumber<double>();
			printf("%f\n", dVal);
		}
		else if (eType == eVVT_Bool)
		{
			bool bVal = var->GetNumber<bool>();
			printf("%d\n", bVal);
		}
		else if (eType == eVVT_String)
		{
			const char *str = var->GetString();
			printf("%s\n", str);
		}
		
	}
	CVariant* GetVarObjectNum(){
		m_var->SetNumber(100);
		return m_var;
	}
	CVariant* GetVarObjectStr()
	{
		m_var->SetString("Hello World");
		return m_var;
	}

	void DoVarListObject(vector<CVariant*> *vecVar)
	{
		vector<CVariant*>::iterator it = vecVar->begin();
		while (it != vecVar->end())
		{
			DoVarObject((*it));
			it++;
		}
		
	}

	void TestVariantCallBack(){
		m_var->SetNumber(11);
		
		DoVarCallBack(m_var, m_pArgList);
		
	}

	vector<CVariant*>* TestRetArgList()
	{
		return m_pArgList;
	}

	virtual void DoVarCallBack(CVariant *var, vector<CVariant*> *pArg)
	{
		CALL_CLASS_CALLBACK_2(var, pArg)
	}

private:
	CVariant *m_var;
	vector<CVariant*> *m_pArgList;
};

class TestCo : public virtual CDynamicObject
{
public:
	virtual void DoNone(){
		CALL_CLASS_CALLBACK()
	}
};

class A :public virtual CDynamicObject {};

class B: public A 
{
public:
	char* getname()
	{
		return "I am B !";
	}
};

class C :public B 
{
public:
	char* getname()
	{
		return "I am C !";
	}
};

class E :public virtual CDynamicObject {};

A* getObjectB()
{
	static A* p = new B;
	return p;
}
A* getObjectC()
{
	return new C;
}

void ClassTestRegist( CScript& script )
{	
	REGIST_B_CLASS(script, A);
	REGIST_B_CLASS(script, E);
	REGIST_D_CLASS(script, B, A);
	REGIST_D_CLASS(script, C, B);
	REGIST_CLASSFUNCTION(script, B, getname, ":s")
	REGIST_CLASSFUNCTION(script, C, getname, ":s")
	REGIST_GLOBALFUNCTION(script, getObjectB, ":(A)")
	REGIST_GLOBALFUNCTION(script, getObjectC, ":(A)")


	REGIST_B_CLASS(script, TestCo);
	REGIST_CALLBACKFUNCTION(script, TestCo, DoNone, "");
	

	REGIST_B_CLASS(script, TestVarObject);
	REGIST_CLASSFUNCTION(script, TestVarObject, DoVarObject, "a");
	REGIST_CLASSFUNCTION(script, TestVarObject, GetVarObjectNum, ":a");
	REGIST_CLASSFUNCTION(script, TestVarObject, GetVarObjectStr, ":a");
	REGIST_CLASSFUNCTION(script, TestVarObject, DoVarListObject, "l:");
	REGIST_CLASSFUNCTION(script, TestVarObject, TestVariantCallBack, "");
	REGIST_CLASSFUNCTION(script, TestVarObject, TestRetArgList, ":l");
	REGIST_CALLBACKFUNCTION(script, TestVarObject, DoVarCallBack, "al");

	REGIST_B_CLASS( script, IPureClass );

	REGIST_B_CLASS( script, CTestNormalFunClass );

	REGIST_B_CLASS( script, CTestClassBase );
	REGIST_D_CLASS( script, CTestClassChild, CTestClassBase );
	//CStaticFunClass,只导出静态变量函数，无需声明此类d
	REGIST_B_CLASS( script, IMessage );
	REGIST_D_CLASS( script, IMessageNotOver, IMessage );
	REGIST_D_CLASS( script, IMessageOver, IMessage );
	REGIST_B_CLASS( script, IFactory );
	REGIST_B_CLASS( script, IWnd );

	REGIST_B_CLASS( script, CCallBackBase );
	REGIST_D_CLASS( script, CCallBackChildOver, CCallBackBase );
	REGIST_D_CLASS( script, CCallBackChildNotOver, CCallBackBase );

	REGIST_B_CLASS( script, CMultiClassBase1 );
	REGIST_B_CLASS( script, CMultiClassBase2 );
	REGIST_B_CLASS( script, CMultiClassBase3 );
	REGIST_B_CLASS( script, CMultiClassBase4 );
	REGIST_B_CLASS( script, CMultiClassBase5 );
	REGIST_D_CLASS_5( script, CMultiChild, CMultiClassBase1, CMultiClassBase2, CMultiClassBase3,
		CMultiClassBase4, CMultiClassBase5 );

	REGIST_B_CLASS( script, CTestNormalObjContainer );
	REGIST_B_CLASS( script, CTestAsynType );
	REGIST_B_CLASS( script, CTestAsynNumber );
	REGIST_B_CLASS( script, CTestRefObj );
	REGIST_B_CLASS( script, CTestRefLuaValue );

	REGIST_B_CLASS( script, CTestClassOverLoadSimple );

	REGIST_B_CLASS( script, CTestClassOverLoadBase1 );
	REGIST_B_CLASS( script, CTestClassOverLoadBase2 );
	REGIST_B_CLASS( script, CTestClassOverLoadBase3 );
	REGIST_B_CLASS( script, CTestClassOverLoadBase4 );
	REGIST_B_CLASS( script, CTestClassOverLoadBase5 );
	REGIST_D_CLASS_5( script, CTestClassOverLoadChild, CTestClassOverLoadBase1, CTestClassOverLoadBase2, CTestClassOverLoadBase3,
		CTestClassOverLoadBase4, CTestClassOverLoadBase5 );

	REGIST_B_CLASS( script, CTestReleaseClass );
	REGIST_B_CLASS( script, CTestReleaseClassFactory );
	
	REGIST_B_CLASS( script, CTestCallBackParamMix );


	//=================================		IPureClass	========================================
	REGIST_CALLBACKFUNCTION( script, IPureClass, OnGetName, "iis:s");
	REGIST_STATICFUNCTION( script, IPureClass, GetName, "(IPureClass):s" );


	//=================================		CTestNormalFunClass	========================================
	REGIST_CLASSFUNCTION( script, CTestNormalFunClass, SetInt, "i");
	REGIST_CLASSFUNCTION( script, CTestNormalFunClass, GetInt, ":i");

	//=================================		CTestClassBase	========================================
	REGIST_CLASSFUNCTION( script, CTestClassBase, GetIndex, ":i");
	REGIST_CLASSFUNCTION( script, CTestClassBase, Get100, ":i");
	REGIST_CLASSFUNCTION( script, CTestClassBase, Get1000, ":i");

	//=================================		CTestClassChild	========================================
	//子类能直接使用基类里注册了的函数，包括虚函数（支持C++的虚函数的概念，子类调用的是子类的实现）

	//=================================		CStaticFunClass	========================================
	REGIST_STATICFUNCTION( script, CStaticFunClass, GetName, ":s" );
	REGIST_STATICFUNCTION( script, CStaticFunClass, GetNum, ":C" );

	//=================================		IWnd	========================================
	REGIST_CLASSFUNCTION( script, IWnd, WndName, ":s" );
	REGIST_CLASSFUNCTION( script, IWnd, WndProc, "(IMessage):i" );
	REGIST_CLASSFUNCTION( script, IWnd, Clone, ":(IWnd)" );
	REGIST_STATICFUNCTION( script, IWnd, GetNum, ":i" );

	//=================================		IMessage	========================================
	REGIST_CLASSFUNCTION( script, IMessage, GetMessageId, ":i" );
	REGIST_CLASSFUNCTION( script, IMessage, MessageName, ":s" );
	REGIST_STATICFUNCTION( script, IMessage, GetNum, ":i" );

	REGIST_CLASSFUNCTION( script, IMessage, GetEnvironment, ":s" );
	REGIST_CALLBACKFUNCTION( script, IMessage, Environment, ":s" );

	REGIST_CLASSFUNCTION( script, IMessageNotOver, GetClassName, ":s" );
	REGIST_CLASSFUNCTION( script, IMessageOver, GetClassName, ":s" );

	//=================================		IFactory	========================================
	REGIST_CLASSFUNCTION( script, IFactory, CreateMessage, ":(IMessage)" );
	REGIST_CLASSFUNCTION( script, IFactory, CreateMessageNotOver, ":(IMessage)" );
	REGIST_CLASSFUNCTION( script, IFactory, CreateMessageOver, ":(IMessage)" );
	REGIST_CLASSFUNCTION( script, IFactory, CreateWnd, ":(IWnd)" );

	REGIST_GLOBALFUNCTION( script, GetFactory, ":(IFactory)" );

	//=================================		CCallBackBase	========================================
	REGIST_CLASSFUNCTION( script, CCallBackBase, GetEnvironment, ":s" );
	REGIST_CALLBACKFUNCTION( script, CCallBackBase, Environment, ":s" );

	//=================================		Multi Class	========================================
	REGIST_CLASSFUNCTION( script, CMultiClassBase1, who, ":s");
	REGIST_CLASSFUNCTION( script, CMultiClassBase2, how, ":s");
	REGIST_CLASSFUNCTION( script, CMultiClassBase3, when, ":s");
	REGIST_CLASSFUNCTION( script, CMultiClassBase4, what, ":s");
	REGIST_CLASSFUNCTION( script, CMultiClassBase5, where, ":s");

	//==================================	for class reference			===========================
	REGIST_CLASSFUNCTION( script, CTestNormalObjContainer, getTestNormalObjWeak, ":(CTestNormalFunClass)" );
	REGIST_GLOBALFUNCTION( script, getTestNormalObj, ":(CTestNormalFunClass)" );
	
	//=================================		Asyn type					============================
	REGIST_CLASSFUNCTION( script, CTestAsynType, bindStr, "mi");
	REGIST_CLASSFUNCTION( script, CTestAsynType, update, "");
	REGIST_CLASSFUNCTION( script, CTestAsynType, unbindStr, "");

	
	//=================================		Ref Obj						=============================
	REGIST_CLASSFUNCTION( script, CTestRefObj, bindObj, "(CTestNormalFunClass)");
	REGIST_CLASSFUNCTION( script, CTestRefObj, unbindObj, "");
	REGIST_CLASSFUNCTION( script, CTestRefObj, getObj, ":(CTestNormalFunClass)");

	
	//=================================		class function overload				=============================
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadSimple, int(CTestClassOverLoadSimple::*)(int), get, get1, "i:i");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadSimple, int(CTestClassOverLoadSimple::*)(int,int),get, get2, "ii:i");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase1, int(CTestClassOverLoadBase1::*)(void), get, get_int, ":i");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase1, void(CTestClassOverLoadBase1::*)(int), set, set_int, "i");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase2, bool(CTestClassOverLoadBase2::*)(void), get, get_bool, ":b");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase2, void(CTestClassOverLoadBase2::*)(bool), set, set_bool, "b");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase3, float(CTestClassOverLoadBase3::*)(void), get, get_float, ":f");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase3, void(CTestClassOverLoadBase3::*)(float), set, set_float, "f");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase4, const char*(CTestClassOverLoadBase4::*)(void), get, get_str, ":s");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase4, void(CTestClassOverLoadBase4::*)(const char*), set, set_str, "s");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase5, double(CTestClassOverLoadBase5::*)(void), get, get_double, ":d");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadBase5, void(CTestClassOverLoadBase5::*)(double), set, set_double, "d");

	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadChild, int8(CTestClassOverLoadBase1::*)(void), get, get_int8, ":c");
	REGIST_CLASSFUNCTION_OVERLOAD( script, CTestClassOverLoadChild, void(CTestClassOverLoadBase1::*)(int8), set, set_int8, "c");


	//============================== test AbstracClass		=======================================
	REGIST_B_CLASS( script, IAbstracClass1 );
	REGIST_B_CLASS( script, IAbstracClass2 );
	REGIST_D_CLASS_2( script, IAbstracClass3, IAbstracClass1, IAbstracClass2 );
	REGIST_CALLBACKFUNCTION( script, IAbstracClass1, OnPrint1, "s");
	REGIST_CALLBACKFUNCTION( script, IAbstracClass2, OnPrint2, "s");
	REGIST_CALLBACKFUNCTION( script, IAbstracClass3, OnPrint3, "s");

	REGIST_B_CLASS( script, CPrintAbstra );
	REGIST_CLASSFUNCTION( script, CPrintAbstra, PrintAbstract, "(IAbstracClass3)");


	//============================== test release class	=======================================
	REGIST_CLASSFUNCTION( script, CTestReleaseClass, setAfterInitState, "" );
	REGIST_CLASSFUNCTION( script, CTestReleaseClass, IsAfterInit, ":b" );
	REGIST_CLASSFUNCTION( script, CTestReleaseClass, Release, "" );
	REGIST_CLASSFUNCTION( script, CTestReleaseClassFactory, createCTestReleaseClass, ":(CTestReleaseClass)" );
	REGIST_CLASSFUNCTION( script, CTestReleaseClassFactory, hasCreateFromTheSameAddr, ":b" );

	REGIST_CLASSFUNCTION( script, CTestCallBackParamMix, Call, ":b" );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackParamMix, CallBack, "ifbds:i" );

	//============================== CPos	=======================================
	REGIST_ISOLATED_CLASS( script, Rect );
	REGIST_CLASSMEMBER( script, Rect, left, "i" );
	REGIST_CLASSMEMBER( script, Rect, right, "i" );
	REGIST_CLASSMEMBER( script, Rect, top, "i" );
	REGIST_CLASSMEMBER( script, Rect, bottom, "i" );
	REGIST_ISOLATED_CLASS( script, CPos );
	REGIST_CLASSMEMBER( script, CPos, x, "i" );
	REGIST_CLASSMEMBER( script, CPos, y, "i" );
	REGIST_CLASSFUNCTION( script, CPos, Check, ":b" );

	//============================== CTestCBuf	=======================================
	REGIST_B_CLASS( script, CTestCBuf );
	REGIST_CLASSFUNCTION( script, CTestCBuf, Check, ":b" );
	REGIST_CLASSFUNCTION( script, CTestCBuf, GetBuf, ":m" );

	REGIST_PACKFUNCTIONTABLE( script, Test_Pack.hPack, TestPack, (void*)56, "Test_Pack" );
	REGIST_PACKCALLBACKTABLE( script, Test_Pack.hUnPack, &Test_Pack.pFun, "Test_Pack" );
	REGIST_PACKFUNCTION( script, Test_Pack.hPack, "goto", 0, true, "(CMultiChild)iis" );
	REGIST_PACKCALLBACK( script, Test_Pack.hUnPack, "ongoto", 0, true, "(CMultiChild)iis");

	//============================== CTestStaticOverLoad	=======================================
	REGIST_STATICFUNCTION_OVERLOAD( script, CTestStaticOverLoad, int(*)(int,int), add, add_i, "ii:i" );
	REGIST_STATICFUNCTION_OVERLOAD( script, CTestStaticOverLoad, double(*)(double, double), add, add_d, "dd:d" );

	//============================== CTestCallBackOverLoad	=======================================
	REGIST_B_CLASS( script, CTestCallBackOverLoad );
	REGIST_CALLBACKFUNCTION_OVERLOAD( script, CTestCallBackOverLoad, int(CTestCallBackOverLoad::*)(double, double), add, add_d, "dd:i" );
	REGIST_CALLBACKFUNCTION_OVERLOAD( script, CTestCallBackOverLoad, int(CTestCallBackOverLoad::*)(int, int), add, add_i,  "ii:i" );
	REGIST_CLASSFUNCTION( script, CTestCallBackOverLoad, callback, ":b" );

	//============================== CTestCallBackOverLoad	=======================================
	REGIST_B_CLASS( script, CTestCallBackReturn );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackReturn, Onri, ":i" );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackReturn, Onrb, ":b" );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackReturn, Onrf, ":f" );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackReturn, Onrd, ":d" );
	REGIST_CALLBACKFUNCTION( script, CTestCallBackReturn, Onrstr, ":s" );
	REGIST_CLASSFUNCTION( script, CTestCallBackReturn, callback, ":b" );
}

void TestRunFunction( CScript& Script )
{
	int x;
	char* str;
	float fl;
	float diff;

	Script.RunFunction( "TestRun", "(IFactory)ii:isf", (CBaseObject*)GetFactory(), 256, 1010000, &x, &str, &fl );
	diff = fl - 1.025f;
	if( x == 10254 )
		if( string("Test") == str )
			if( diff <0.0001 && diff >-0.0001 )
				return ;

	throw RunSciptExp();
}
