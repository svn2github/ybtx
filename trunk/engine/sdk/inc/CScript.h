#pragma once
//---------------------------------------------------------Script--------------------------------------------------------
//
// 脚本和c++的接口
//
// ScriptFunction : c++函数返回值为脚本函数返回值个数
// szFormat :		大写字母无符号，小写有符号
//					t/T				size_t			//一般用于把指针的值传递给lua使用
//					z:				int64			//新添加的数据类型，只能支持(-1e+15,1e+15)范围内的数据，超过这个范围会出错
//					Z:				uint64          //同上，只是从0开始
//					i:				int32
//					I:				uint32
//					h:				int16
//					H:				uint16
//					c:				int8
//					C:				uint8
//					b:				bool
//					f:				float
//					d:				double
//					s:				const char *
//					m:				Memory				//Script提供的访问连续内存的类
//					(CSomeClass):	CSomeClass *		//目前还不确认是否能用于返回值以及回调
//					a:              CVariant            //支持任意基本类型
//					l:              vector<CVariant*>   //支持可变参数的基本类型
//
//					RunScript 中szFormat = "ii:ss"格式符表示调用的函数参数为两个整数，函数输出结果为两个字符串
// 对lua虚拟机的修改：
//					@.修改了global_State结构在末尾增加了一个void* context变量，用于保存调试器
//					@.修改了luaFile的一系列函数，使得文件可以从包文件系统读入
//					@.关闭了sethook和gethook两个函数的注册，使得无法在lua里面修改hook函数
// 注意：			从脚本中获取数据（例如RunScript的结果和GetArgFromHandle获得的参数）必须传数据地址，用法类似scanf，
//					而将数据传给脚本（例如RunScript的参数和PushResultToHandle输出的结果）只能传数值，用法类似printf。
//
//---------------------------------------------------------------------------------------------------------------------------

/*
脚本库需要注意规则

1，注册子类时填写基类的顺序必须和c++中派生的顺序一致。
2，不能同时把有继承关系的类注册为自己父类。
3，注册时不允许棱形继承。
4，Unsafe类不允许在c++里面创建对象之后传入lua使用。
5，注册时派生自Unsafe类的类型也是Unsafe类
6，在lua创建对象的c++类，禁止使用析构函数，或者只用空的析构函数。原因是析构函数由垃 圾回收机制来控制调用，执行时机不能确定，容易产生很难解决的bug。类型析构操作应放在一 个Release()函数中，在lua显式调用。
7，在c++中创建对象并传递给lua使用的c++类，可以使用析构函
*/

#include "ScriptDef.h"
#include "ThreadTypes.h"

struct lua_State;

namespace sqr
{
	enum EScriptHelper {eSH_None,eSH_Debuger,eSH_Profiler};

	class CSqrAllocPool;

	class SCRIPT_API CScript
	{
	public:
		CScript(EScriptHelper eScriptHelper,
			float fCallbackGCRatio=1,float fNormalUnpackGCRatio=1,float fSerialUnpackGCRatio=1);
		~CScript();

		static int Compile(int nArgc,char* aryArgv[]);


		EScriptHelper GetHelperType()const;

		//得到Script数据
		CScriptData* GetData() const;
	
		//调用脚本文件
		std::string RunFile( const char* szScript, const char* szFormat=NULL, ... );
		//执行一段代码
		std::string	RunString( const char* szString, const char* szFormat=NULL, ... );
		//执行一个函数
		std::string	RunFunction( const char* szFunction, const char* szFormat=NULL, ... );

		//设置Script的AliasPath以及Path
		void SetLoadPath(const char* szPathAlias, const char* szPath);
		const char* GetLoadPath(const char* szPathAlias);

		//注册单个被脚本调用的函数
		template<typename Func>
		void RegCallFunc(const char* szArg, Func pFunc, const char* FunctionName );

		// 注册类成员函数
		template<typename Func>
		void RegClassCallFunc(const char* szArg, Func pFunc, const char* szClass, const char* FunctionName);

		template<typename Func>
		void RegClassCallback(const char* szArg, Func pFunc, const char* szClass, const char* szFunName, bool bPureCallBackFunc);

		// 注册类
		void RegClass( int32 eClassType, REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun, size_t sizeOfClass, const char* szTypeClass, const char* szClass, ... );
		
		//注册isolated class
		void RegIsolatedClass( REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun, size_t sizeOfClass, const char* szTypeClass, const char* szClass );
		
		// 注册类成员数据
		void RegClassMember( const char* szArg, int32 nOffset, const char* szClass, const char* szMemberName );
		
		//注册函数参数打包函数集
		HPACKAGE RegPackPakage( PACK_FUNCTION pPackFun, void* pCppContext, const char* szTableName );
		
		//注册函数参数打包打包函数集的函数
		void RegPackFunction( HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat );
		
		//注册函数参数解包函数集
		HPACKAGE RegUnPackPakage( const char* szTableName, UNPACK_FUNCTION* pUnPackFun );
		
		//注册函数参数解包函数集的解包函数
		void RegUnPackFunction( HPACKAGE hPakageHandle, const char* szFunction, uint32 uID, bool bFirstArgAsCallContext, const char* szFormat );
		
		//注册serial函数参数打包函数
		void RegSerialPackFun(const char* szFunName, POST_PACKFUNCTION pPostPackFun);
		
		//注册serial函数参数解包函数
		void RegSerialUnPackFun(ON_UNPACKFUNCTION* pOnUnPackFun);

		//注册常量
		void RegConstant( const char* szTableName, const char* szFeild, int32 nValue );
		//得到注册的类信息
		CClass* GetClassInfo( const char* szClassName );
		//通过rtti的name得到的类信息
		CClass*	GetClassInfoWithType( const char* szClassType );
		
		//增加引用计数
		void RefScriptObj( void* pUserData );
		void RefScriptObj( CBaseObject* pObject );
		
		//减少引用计数
		void UnrefScriptObj( void* pUserData );
		void UnrefScriptObj( CBaseObject* pObject );
		
		//取消对象和script的连接关系
		void UnlinkCppObj( void* pObj );
		void UnlinkCppObj( CBaseObject* pObject );

		//垃圾回收
		void GCAll();

		//定时定量垃圾回收		
		void GCStep(uint32 uMinStep);

		//设置gc的属性
		void SetGCStepMul(int stepmul);
		void SetGCPause(int pause);

		//得到lua的内存占用
		int	GCCount();

		//取得回调函数对象
		void* GetCallBackData(CBaseObject* pObject, const char* szFunName);
		//取得回调函数相关信息
		void GetCallBackAttr(void* pCall, size_t& stBufSize, size_t*& pAyParamOffset, 
			size_t& iRetOffset);
		//调用回调函数
		bool CallClassCallBack(void* pCall, CBaseObject* pObject, char* pDataBuf);

		//获取当前的lua堆栈
		static void	GetStack(ostream& strm);

		static void Init(EScriptHelper eScriptHelper,
			float fCallbackGCRatio=1,float fNormalUnpackGCRatio=1,float fSerialUnpackGCRatio=1);
		static void Unit();
		static CScript* Inst();

		void AstInSameThread();

		void SetRunningThread(HTHREADID id);
		void DetachRunningThread();

		void CallBackProfileBegin(const char* szFunName);
		void CallBackProfileEnd(const char* szFunName);

	private:
		bool InSameThread();
		
		void _RegCallFunc(void* pContext, const char* szFunctionName, int32 (*pFunc)(lua_State*));
		void _RegClassCallFunc(void *pContext, const char* szClassName, const char* szFunctionName, int32 (*pFunc)(lua_State*));
		void _RegClassCallback(void* pContext);

		CClass* CreateClass(REG_CREATE_FUN pCreateFun, REG_DELETE_FUN pDeleteFun, 
			size_t sizeOfClass, const char* szTypeClass, 
			const char* szClass, int32 eClassType);

		
	
	private:

		CScriptData*		m_pData;
		CCallBackProfiler*	m_pCallBackProfiler;

		/*
		脚本库由于要进行多线程拆分了，所以ms_pInst只用于主线程的对象，
		而其他线程的script对象我们使用tls存储
		*/
		static CScript*	ms_pInst;

		static HTLSKEY	ms_tlsKey;

		HTHREADID			m_hThreadID;
	};
}


using namespace sqr;
