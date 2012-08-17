#pragma once 

#include <vector>
#include <string>
#include "CScriptMallocObject.h"
#include "CScriptAllocator.h"

struct lua_State;

namespace sqr
{
	class CValueTable;

	class CScriptPackSerial : public CScriptMallocObject
	{
	public:	
		typedef basic_string<char, std::char_traits<char>, CScriptAllocator<char> > String_t;
		typedef vector<char, CScriptAllocator<char> >	VecBuf_t;		

		static void PackString(const char* str, size_t stLen, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);
		static void PackBoolean(bool bBoolean, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);
		static void PackDouble(double dNum, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);
		static void PackNil(String_t& strType);
		static void PackFunctionContext(void* pContext, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);
		static void PackFunction(lua_State* pState, int32 index, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);
		static void PackTable(lua_State* pState, int32 index, String_t& strType, 
			VecBuf_t& vecBuf, uint32& uPos);		
		static void PackValueTable(lua_State* pState, String_t& strType,
			VecBuf_t& vecBuf, uint32& uPos);

		static void PackData(VecBuf_t& vecBuf, const void* pData, 
			size_t stDataSize, uint32& uPos);

	};
}

