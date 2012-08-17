#pragma once 

struct lua_State;

namespace sqr
{
	class CValueTable;

	class CScriptUnPackSerial
	{
	public:
		static const char* UnPackString(lua_State* pState, 
			const char* strType, const char* vecBuf, uint32& uTypePos, uint32& uBufPos);
		static double UnPackNumber(lua_State* pState, const char* vecBuf, 
			uint32& uTypePos, uint32& uBufPos);
		static bool UnPackBoolean(lua_State* pState, const char* vecBuf, 
			uint32& uTypePos, uint32& uBufPos);
		static void UnPackNil(lua_State* pState, uint32& uTypePos);
		static void* UnPackFunctionContext(lua_State* pState, const char* vecBuf, 
			uint32& uTypePos, uint32& uBufPos);
		static void UnPackTable(lua_State* pState, const char* strType, 
			const char* vecBuf, uint32& uTypePos, uint32& uBufPos);
		static void UnPackValueTable(lua_State* pState, const char* strType,
			const char* vecBuf, uint32& uTypePos, uint32& uBufPos);

		static void UnPackData(void* pData, const char* vecBuf, 
			size_t stDataSize, uint32& uBufPos);
	};

}
