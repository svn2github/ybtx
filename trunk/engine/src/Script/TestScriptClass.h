#pragma once

#include "CScript.h"
#include "TestScriptType.h"
#include "TScriptRefPtr.h"
#include "CallBackX.h"
#include <set>
#include "CDynamicObject.h"

using namespace std;


class CTestNormalFunClass
	:public virtual sqr::CDynamicObject
{
public:
	void		SetInt( int value )			{ m_nInt = value;	}
	int			GetInt()					{ return m_nInt;	}

protected:
	int		m_nInt;
};

class CTestClassBase
	:public virtual sqr::CDynamicObject
{
public:
	virtual int		GetIndex()				{ return 0;		}
	int				Get100()				{ return 100;	}
	int				Get1000()				{ return 1000;	}
	virtual			~CTestClassBase()		{}
};

class CTestClassChild : public CTestClassBase
{
public:
	virtual int		GetIndex()				{ return 1;		}
	virtual			~CTestClassChild()		{}
};

void ClassTestRegist( CScript& script );

class CStaticFunClass
	:public virtual sqr::CDynamicObject
{
public:
	static const char* GetName()	{ return "CStaticFunClass"; }
	static uint8 GetNum()			{ return 35; }
};

class CCallBackBase
	:public virtual sqr::CDynamicObject
{
public:
	const char* GetEnvironment()			{ return Environment(); }
	virtual const char* Environment()		{ 
		CALL_CLASS_CALLBACK_RET(const char*)	
		return "C++"; 
	}
};

class CCallBackChildNotOver : public CCallBackBase
{};

class CCallBackChildOver : public CCallBackBase
{
public:
	virtual const char* Environment()		{ return "C++, child"; }
};

class IMessage
	:public virtual sqr::CDynamicObject
{
public:
	IMessage()						{ ++s_num; }
	virtual ~IMessage()				{ --s_num; }
	virtual const char* MessageName() {return NULL;};
	virtual int GetMessageId() {return 0;};
	static  int GetNum()			{ return s_num; }

	const char* GetEnvironment()			{ return Environment(); }
	virtual const char* Environment()		{ 
		CALL_CLASS_CALLBACK_RET(const char*)	
		return "C++"; 
	}
protected:
	static int s_num;
};

class IMessageNotOver : public IMessage
{
public:
	const char* GetClassName()					{ return "IMessageNotOver"; }
};

class IMessageOver : public IMessage
{
public:
	virtual const char* Environment()			{ return "C++, child"; }
	const char* GetClassName()					{ return "IMessageOver"; }
};

class IWnd
	:public virtual sqr::CDynamicObject
{
public:
	IWnd()							{ ++s_num; }
	virtual ~IWnd()					{ --s_num; }
	virtual const char* WndName() {return NULL;};
	virtual int WndProc( IMessage* pMessage ) {return 0;};
	virtual IWnd* Clone() {return NULL;};
	static  int GetNum()			{ return s_num; }
protected:
	static int s_num;
};

class IFactory
	:public virtual sqr::CDynamicObject
{
public:
	virtual IWnd*	CreateWnd() {return NULL;};
	virtual IMessage* CreateMessage() {return NULL;};
	virtual IMessage* CreateMessageNotOver() {return NULL;};
	virtual IMessage* CreateMessageOver() {return NULL;};
};

IFactory*	GetFactory();

class CMultiClassBase1
	:public virtual sqr::CDynamicObject
{
public:
	const char* who() { return "who"; }
};

class CMultiClassBase2
	:public virtual sqr::CDynamicObject
{
public:
	const char* how() { return "how"; }
};

class CMultiClassBase3
	:public virtual sqr::CDynamicObject
{
public:
	const char* when() { return "when"; }
};

class CMultiClassBase4
	:public virtual sqr::CDynamicObject
{
public:
	const char* what() { return "what"; }
};

class CMultiClassBase5
	:public virtual sqr::CDynamicObject
{
public:
	const char* where() { return "where"; }
};

class CMultiChild : public CMultiClassBase1, 
	public CMultiClassBase2, public CMultiClassBase3, public CMultiClassBase4, public CMultiClassBase5
{};

class RunSciptExp : public std::exception
{
	virtual const char* what() const throw()
	{
		return "Run Scipt Expcetion";
	}
};

void TestRunFunction( CScript& Script );

class CTestNormalObjContainer
	:public virtual sqr::CDynamicObject
{
public:
	CTestNormalObjContainer()
		:m_int(0)
	{
		m_NormalFunObj.SetInt( 99999 );
	}

	CTestNormalFunClass* getTestNormalObjWeak()	
	{
		return &m_NormalFunObj; 
	}

protected:
	int						m_int;
	CTestNormalFunClass		m_NormalFunObj;
};

CTestNormalFunClass* getTestNormalObj();

class CTestAsynType
	:public virtual sqr::CDynamicObject
{
public:
	CTestAsynType()
		: m_pInt( GetScriptForTest() )
		, m_pBool( GetScriptForTest() )
		, m_pFloat( GetScriptForTest() )
		, m_str( GetScriptForTest() )
		, m_strlen(0)
	{}

	void bindInt( int* pInt )
	{
		m_pInt = pInt; 
	}
	void bindBool( bool* pBool )		
	{
		m_pBool = pBool; 
	}

	void bindStr( char* str, int len )
	{
		m_str = str;
		m_strlen = len;
	}

	void bindFloat( float* pFloat )
	{
		m_pFloat = pFloat;
	}

	void unbindInt()
	{
		m_pInt = NULL;
	}

	void unbindBool()
	{
		m_pBool = NULL;
	}

	void unbindStr()
	{
		m_str = NULL;
		m_strlen = 0;
	}

	void update()
	{
		const char str[] = "Hello";
		if( m_pInt )
			*m_pInt = 592;
		if( m_pBool )
			*m_pBool = false;
		if( m_str )
		{
			int len =(int)( strlen(str) < m_strlen ? strlen(str) : m_strlen  );
			memcpy( (void*)( m_str.addr() ), str, len );
			if( len != 0 )
				m_str[len] = '\0';
		}
		if( m_pFloat )
			*m_pFloat = 23.0125f;
	}

protected:
	TScriptRefPtr<int> m_pInt;
	TScriptRefPtr<bool> m_pBool;
	TScriptRefPtr<float>			m_pFloat;

	TScriptRefPtr<char*> m_str;
	size_t m_strlen;
};

class CTestAsynNumber
	:public virtual sqr::CDynamicObject
{
public:
	CTestAsynNumber()
		: m_pInt32( GetScriptForTest() )
		, m_pUint32( GetScriptForTest() )
		, m_pInt16( GetScriptForTest() )
		, m_pUint16( GetScriptForTest() )
		, m_pInt8( GetScriptForTest() )
		, m_pUint8( GetScriptForTest() )
		, m_pDouble( GetScriptForTest() )
		, m_pFloat( GetScriptForTest() )
	{}

	void bindInt32	( int32* pInt32 )			{	m_pInt32	= pInt32;	}
	void bindUint32	( uint32* pUint32 )			{	m_pUint32	= pUint32;	}
	void bindInt16	( int16* pInt16 )			{	m_pInt16	= pInt16;	}
	void bindUint16	( uint16* pUint16 )			{	m_pUint16	= pUint16;	}
	void bindInt8	( int8* pInt8 )				{	m_pInt8		= pInt8;	}
	void bindUint8	( uint8* pUint8 )			{	m_pUint8	= pUint8;	}

	void bindDouble	( double* pDouble )			{	m_pDouble	= pDouble;	}
	void bindFloat  ( float*  pFloat )
	{
		m_pFloat	= pFloat;	
	}

	void update()
	{
		if( m_pInt32 )
			*m_pInt32	= -32;
		if( m_pUint32 )
			*m_pUint32	= 32;

		if( m_pInt16 )
			*m_pInt16	= -16;
		if( m_pUint16 )
			*m_pUint16	= 16;

		if( m_pInt8 )
			*m_pInt8	= -8;
		if( m_pUint8 )
			*m_pUint8	= 8;

		if( m_pDouble )
			*m_pDouble = -1.7823;

		if( m_pFloat )
			*m_pFloat = 5.3214f;
	}
protected:
	TScriptRefPtr<int32>		m_pInt32;
	TScriptRefPtr<uint32>	m_pUint32;
	TScriptRefPtr<int16>		m_pInt16;
	TScriptRefPtr<uint16>	m_pUint16;
	TScriptRefPtr<int8>		m_pInt8;
	TScriptRefPtr<uint8>		m_pUint8;
	TScriptRefPtr<double>	m_pDouble;
	TScriptRefPtr<float>		m_pFloat;
};

namespace sqr
{

	class CTestRefObj
		:public virtual sqr::CDynamicObject
	{
	public:
		CTestRefObj()
			: m_pObj( GetScriptForTest() )
		{}


		void bindObj( CTestNormalFunClass* pObj )		{ m_pObj = pObj; }
		void unbindObj()								{ m_pObj = NULL; }
		CTestNormalFunClass*	getObj()				{ return m_pObj.m_pData; }
	protected:
		TScriptRefPtr<CTestNormalFunClass>			m_pObj;
	};

}

class CTestRefLuaValue
	:public virtual sqr::CDynamicObject
{
public:
	CTestRefLuaValue()
		: m_pLuaValue( GetScriptForTest() )
	{}

	void bindLuaValue( void * pLuaValue)				{ m_pLuaValue = pLuaValue; }

protected:
	TScriptRefPtr<void>		m_pLuaValue;
};

class CTestClassOverLoadSimple
	:public virtual sqr::CDynamicObject
{
public:
	int get( int a )		{	return a;}
	int get( int a, int b)	{	return a+b; }
};

class CTestClassOverLoadBase1
	:public virtual sqr::CDynamicObject
{
public:
	void	set( int n )		
	{
		m_int = n;		
	}
	int		get()
	{
		return m_int;		
	}
protected:
	int		m_int;
};

class CTestClassOverLoadBase2
	:public virtual sqr::CDynamicObject
{
public:
	void	set( bool b )
	{
		m_bool = b;		
	}
	bool	get()				{ return m_bool;	}
protected:
	bool	m_bool;
};

class CTestClassOverLoadBase3
	:public virtual sqr::CDynamicObject
{
public:
	void	set( float f )		{ m_float = f;		}
	float	get()				{ return m_float;	}
protected:
	float	m_float;
};

class CTestClassOverLoadBase4
	:public virtual sqr::CDynamicObject
{
public:
	void	set( const char* str )		{ m_str = str;	}
	const char*	get()						{ return m_str;	}
protected:
	const char*	m_str;
};

class CTestClassOverLoadBase5
	:public virtual sqr::CDynamicObject
{
public:
	void	set( double d )		{ m_double = d;		}
	double	get()				{ return m_double;	}
protected:
	double	m_double;
};

class CTestClassOverLoadChild
	: public CTestClassOverLoadBase1
	, public CTestClassOverLoadBase2
	, public CTestClassOverLoadBase3
	, public CTestClassOverLoadBase4
	, public CTestClassOverLoadBase5
{
public:
	void	set( int8 n )		{ m_int8 = n;			}
	int8	get()				{ return m_int8;		}
protected:
	int8		m_int8;
};


class IAbstracClass1
	:public virtual sqr::CDynamicObject
{
public:
	virtual void OnPrint1( const char* szName ) {
		CALL_CLASS_CALLBACK_1(szName)
	};
};


class IAbstracClass2
	:public virtual sqr::CDynamicObject
{
public:
	virtual void OnPrint2( const char* szName )  {
		CALL_CLASS_CALLBACK_1(szName)
	};
	void Print2( const char* szName ){};
};

class IAbstracClass3 : public IAbstracClass1, public IAbstracClass2
{
public:
	virtual void OnPrint3( const char* szName )  {
		CALL_CLASS_CALLBACK_1(szName)
	};
};

class CPrintAbstra
	:public virtual sqr::CDynamicObject
{
public:
	void PrintAbstract( IAbstracClass3* pClass )
	{
		pClass->OnPrint1( "IAbstracClass1" );
		pClass->OnPrint2( "IAbstracClass2" );
		pClass->OnPrint3( "IAbstracClass3" );
	}
};


class CTestReleaseClass
	:public virtual sqr::CDynamicObject
{
public:
	CTestReleaseClass()
		: m_bAfterInit( false )
	{}

	void setAfterInitState()	{	m_bAfterInit = true;	}
	bool IsAfterInit()			{	return m_bAfterInit;	}
	void Release()				{	GetScriptForTest().UnlinkCppObj(this); delete this;	}
protected:
	int buf[1024];
	bool m_bAfterInit;
};

class CTestReleaseClassFactory
	:public virtual sqr::CDynamicObject
{
public:
	CTestReleaseClassFactory()
		:m_bCreateFromTheSameAddr ( false )
	{}

	CTestReleaseClass* createCTestReleaseClass()
	{
		CTestReleaseClass* r = new CTestReleaseClass;
		if( m_setObj.find( r ) != m_setObj.end() )
			m_bCreateFromTheSameAddr = true;
		else
			m_setObj.insert( r );

		return r ;
	}

	bool hasCreateFromTheSameAddr()
	{
		return m_bCreateFromTheSameAddr;
	}

protected:
	set<CTestReleaseClass*> m_setObj;
	bool m_bCreateFromTheSameAddr;
};

class CTestCallBackParamMix
	:public virtual sqr::CDynamicObject
{
public:
	bool  Call()
	{
		int a = 13434;
		float f = 1.25451f;
		bool b = false;
		double d = 4278124286.0;
		const char* s = "Hello";

		int r = CallBack( a, f, b, d, s );
		if( r != 903748914 )
			return false;

		return true;
	}

	virtual int CallBack( int i, float f, bool b, double d, const char* str )
	{
		CALL_CLASS_CALLBACK_5_RET(int, i, f, b, d, str)
		return 390134;
	}
};


class CTestCBuf
	:public virtual sqr::CDynamicObject
{
public:
	CTestCBuf();
	bool Check();
	void* GetBuf();
protected:
	char m_buf[100];
};
