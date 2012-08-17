#include "stdafx.h"
#include "CClass.h"
#include "CScriptData.h"
#include "ScriptCommon.h"
#include "TSqrAllocator.inl"
#include "ExpHelper.h"
#include "ErrLogHelper.h"

//=============================== CClass ===============================

CClass::CClass( CScript* pScript, const char* szClassName )
: m_pScript( pScript ), m_sClassName( szClassName )
, m_bIsCallBack(false)
, m_iClassRef(LUA_NOREF)
, m_eType(eSCT_None)
{
}

CClass::~CClass()
{
}

inline bool CClass::IsAbstract()
{
	return !m_pFunCreateObj;
}

EScriptClassType CClass::GetType()
{
	return m_eType;
}

bool CClass::IsIsolatedClass()
{
	return (m_eType == eSCT_Isolated);
}

bool CClass::IsStaticClass()
{
	return (m_eType == eSCT_Static);
}

bool CClass::IsShareClass()
{
	return (m_eType == eSCT_Share);
}

bool CClass::IsDynamicClass()
{
	return (m_eType == eSCT_Dynamic);
}

void* CClass::Create(lua_State *pState)
{
	void* pObj = NULL;

	SQR_TRY
	{
		if(IsAbstract())
		{
			ostringstream strm;
			strm << GetClassName() << " 是注册给lua的抽象类，不允许在lua中创建！" << endl;
			GenErr(strm.str().c_str());
		}

		pObj = m_pFunCreateObj();
	}
	SQR_CATCH(exp)
	{
		ostringstream str;
		str << GetClassName() << "在lua中创建对象的时候发生错误" << endl;

		exp.AppendMsg(str.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;

	return pObj;
}

void CClass::Release(lua_State* pState, void * pObject )
{
	SQR_TRY
	{
		if(IsAbstract())
		{
			ostringstream strm;
			strm << GetClassName() << " 是注册给lua的抽象类，不允许在lua中析构！" << endl;
			GenErr(strm.str().c_str());
		}

		m_pFunDeleteObj(pObject);
	}
	SQR_CATCH(exp)
	{
		ostringstream str;
		str << GetClassName() << "在lua中析构对象的时候发生错误" << endl;

		exp.AppendMsg(str.str().c_str());
		LogExp(exp);
	}
	SQR_TRY_END;
}

const char* CClass::GetClassName()const
{
	return m_sClassName.c_str();
}

void* CClass::_FindCallBackFunc(const String_t& sFuncName)
{
	MapCallBackFunc_t::iterator iter = m_mapCallBackFunc.find(sFuncName);
	if(iter != m_mapCallBackFunc.end())
	{
		return iter->second;
	}

	for( size_t i = 0; i < m_vpBaseRegist.size(); i++ )
	{
		void* pFunc = m_vpBaseRegist[i]->_FindCallBackFunc(sFuncName);
		if(pFunc)
			return pFunc;
	}

	return NULL;
}

void* CClass::GetCallBackFunc(const char* szFunName)
{
	//首先在running callback里面查找，如果没有，我们就要从注册的callback里面搜索了
	//虽然第一个调用会比较慢，但是提高了以后的查询速度
	MapRunningCallBack_t::iterator iter = m_mapRunningCallBack.find(szFunName);
	if (iter != m_mapRunningCallBack.end())
	{
		return iter->second;
	}
		
	void* pFunc = _FindCallBackFunc(szFunName);

	m_mapRunningCallBack.insert(make_pair(szFunName, pFunc));
	
	return pFunc;
}

void CClass::_CheckRegCallBackFunc(CClass* pInfo, const char* szFunName)
{
	if(pInfo->m_bIsCallBack) //如果注册了回调，我们就处理，否则继续向上遍历
	{
		MapCallBackFunc_t::iterator iter = pInfo->m_mapCallBackFunc.find(szFunName);
		if(iter != pInfo->m_mapCallBackFunc.end())
		{
			stringstream str;
			str << GetClassName() << "类注册callback函数 " << szFunName << "出错"
				<< endl << "在父类" << pInfo->GetClassName() << " 上面已经注册了该函数" << endl;
			GenErr(str.str().c_str());
		}
	}

	for( size_t i = 0; i < pInfo->m_vpBaseRegist.size(); i++ )
	{
		_CheckRegCallBackFunc(pInfo->m_vpBaseRegist[i], szFunName);
	}
}

void CClass::RegCallBackFunc(const char* szFunName, void* pContext)
{
	if(IsIsolatedClass())
	{
		ostringstream strm;
		strm << "类" << m_sClassName 
			<< "是isolated 对象，只允许作为基本数据类型使用，不允许注册callback回调函数"
			<< endl;
		GenErr(strm.str());
	}

	//首先查找注册的父类上面是不是也有同名的函数，如果有，就要报错了，
	//继承链上面不允许同名的callback
	for( size_t i = 0; i < m_vpBaseRegist.size(); i++ )
	{
		_CheckRegCallBackFunc(m_vpBaseRegist[i], szFunName);
	}
	
	m_bIsCallBack = true;

	Ver(m_mapCallBackFunc.insert(make_pair(szFunName, pContext)).second);
}

bool CClass::IsCallBack()
{
	if( m_bIsCallBack)
		return true;
	size_t stSize = m_vpBaseRegist.size();
	for( size_t i = 0; i < stSize; ++i )
	{
		if( m_vpBaseRegist[i]->IsCallBack() )
		{
			//因为我的父类是callback的，那么我也是callback的
			m_bIsCallBack = true;
			return true;
		}
	}
	return false;
}

void CClass::AddBaseClass( CClass* pRegist )
{
	if( pRegist )
	{
		m_vpBaseRegist.push_back( pRegist );
	}
}


bool CClass::FindBase( CClass* pRegistBase )
{
	if( pRegistBase == this )
		return true;

	size_t stSize = m_vpBaseRegist.size();
	for( size_t i = 0; i < stSize; i++ )
	{
		if( m_vpBaseRegist[i]->FindBase( pRegistBase ) )
		{
			return true;
		}
	}
	return false;
}


void CClass::SetClassRef(int32 iRef)
{
	m_iClassRef = iRef;
}

int32 CClass::GetClassRef()
{
	return m_iClassRef;
}
