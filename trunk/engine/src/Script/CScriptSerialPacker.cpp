#include "stdafx.h"
#include "CScriptSerialPacker.h"
#include "CScriptPackSerial.h"
#include "ScriptCommon.h"
#include "CScriptData.h"
#include "TSqrAllocator.inl"

namespace sqr
{

	class CScriptSerialPacker
	{
	public:
		CScriptSerialPacker(const char* szFunName, POST_PACKFUNCTION pPostPackFunc);
		~CScriptSerialPacker();

		static int32 PackSerialFun(lua_State* pState);

		typedef basic_string<char, char_traits<char>, CScriptAllocator<char> > String_t;

		String_t					m_strFunName;
		POST_PACKFUNCTION			m_pPostPackFun;

		String_t					m_strType;
		
		typedef vector<char, CScriptAllocator<char> >	VecBuf_t;		

		VecBuf_t					m_vecBuf;
	};

}

CScriptSerialPacker::CScriptSerialPacker(const char* szFunName, POST_PACKFUNCTION pPostPackFunc)
							 : m_strFunName(szFunName)
							 , m_pPostPackFun(pPostPackFunc)
{
	m_strType.reserve(256);
	m_vecBuf.reserve(1024);		
}

CScriptSerialPacker::~CScriptSerialPacker()
{

}

int32 CScriptSerialPacker::PackSerialFun(lua_State* pState)
{
	CScriptSerialPacker& Packer = *(CScriptSerialPacker*)lua_touserdata(pState, lua_upvalueindex(1));

	String_t& strFunName = Packer.m_strFunName;
	POST_PACKFUNCTION pPostPackFun = Packer.m_pPostPackFun;

	String_t& strType = Packer.m_strType;
	VecBuf_t& vecBuf = Packer.m_vecBuf;

	uint32 uPos = 0;

	CScriptPackSerial::PackString(strFunName.c_str(), strFunName.size(), strType, vecBuf, uPos);

	int32 iTop = lua_gettop(pState);

	uint32 uPostNum = 0;

	if(!lua_isnil(pState, 1))
	{
		uPostNum = (uint32)lua_tonumber(pState, 1);
	}

	for(int32 i = 2; i <= iTop; i++)
	{
		int32 type = lua_type(pState, i);
		switch(type)
		{
		case LUA_TNUMBER:
			{
				double dNum = lua_tonumber(pState, i);
				CScriptPackSerial::PackDouble(dNum, strType, vecBuf, uPos);
				break;
			}
		case LUA_TSTRING:
			{
				size_t stLen = 0;
				const char* str = lua_tolstring(pState, i, &stLen);
				CScriptPackSerial::PackString(str, stLen, strType, vecBuf, uPos);
				break;
			}
		case LUA_TBOOLEAN:
			{
				bool bBoolean = (lua_toboolean(pState, i) == 1);
				CScriptPackSerial::PackBoolean(bBoolean, strType, vecBuf, uPos);
				break;
			}
		case LUA_TFUNCTION:
			{
				CScriptPackSerial::PackFunction(pState, i, strType, vecBuf, uPos);
				break;
			}
		case LUA_TLIGHTUSERDATA:
			{
				void* pContext = lua_touserdata(pState, i);
				CScriptPackSerial::PackFunctionContext(pContext, strType, vecBuf, uPos);
				break;
			}
		case LUA_TTABLE:
			{
				CScriptPackSerial::PackTable(pState, i, strType, vecBuf, uPos);
				break;
			}
		case LUA_TUSERDATA:
			{
				CScriptPackSerial::PackValueTable(pState, strType, vecBuf, uPos);
				break;
			}
		case LUA_TNIL:
			{
				CScriptPackSerial::PackNil(strType);
				break;
			}
		default:
			{
				ostringstream strm;
				strm << type << " 不正确的参数类型" << endl;
				luaL_error(pState, strm.str().c_str());
				break;
			}
		}
	}

	pPostPackFun(uPostNum, strType.c_str(), strType.size(), &vecBuf[0], vecBuf.size());

	strType.clear();
	vecBuf.clear();

	return 0;
}

//------------------------------------------------------------------------

CScriptSerialPackerMgr::CScriptSerialPackerMgr()
{
	
}

CScriptSerialPackerMgr::~CScriptSerialPackerMgr()
{
	SetPack_t::iterator iter = m_setPacker.begin();
	while (iter != m_setPacker.end())
	{
		delete *iter;
		++iter;
	}
}

void CScriptSerialPackerMgr::RegPackFun(CScript& Script,const string& strFunName,
										POST_PACKFUNCTION pPostPackFun)
{
	lua_State* pState = Script.GetData()->m_pState;

	int32 iTop = lua_gettop(pState);

	lua_getglobal(pState, strFunName.c_str());

	if (!lua_isnil(pState, -1))
	{
		lua_settop(pState, iTop);
		return;		
	}

	CScriptSerialPacker* pPacker = new CScriptSerialPacker(strFunName.c_str(), pPostPackFun);

	m_setPacker.insert(pPacker);

	lua_pushlightuserdata(pState, pPacker);
	lua_pushcclosure(pState, reinterpret_cast<lua_CFunction>(CScriptSerialPacker::PackSerialFun), 1);
	lua_setglobal(pState, strFunName.c_str());

	lua_settop(pState, iTop);
}

