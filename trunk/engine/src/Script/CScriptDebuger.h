#pragma once

#include "ScriptDef.h"

extern "C"
{
#include "lua/lua.h"
}

#include <set>
#include <string>


namespace sqr
{
	class CScriptDebugerMgr;
	class CScriptDbContainer;

	class CScriptDebuger
	{
		friend class CScriptDebugerMgr;

	private:
		//下列四个变量只有在进行StepIn,StepOut,StepOver的过程中才被用到，平时不会被访问。
		//可以考虑将他们放到一个结构体里面
		char*			m_pBuf;
		int32				m_nBreakStackLevel;		//断点的堆栈深度
		lua_State*			m_pPrevState;			//上一次hook调进来的state，用于判断是否进入了新的coroutine
		lua_State*			m_pBaseState;			//调试开始时的state。
		bool(CScriptDebuger::* m_pCurHook)(lua_State*,lua_Debug*);

		CScriptDbContainer*	m_pDbContainer;

		std::string			m_sLastVarName;


		enum{EInputLineLength=1024};

		char			m_szLineBuffer[EInputLineLength];
		char			m_szWordBuffer[EInputLineLength];
		
		bool PrintLine( lua_State *pState, uint32 uLevel, int32 nLine, bool bIsCurLine );
		
		const char* ReadWord(bool bNewLine=false);
		//-----------------------------------------------------
		// 当脚本执行中行跳转时将会调用此函数
		//-----------------------------------------------------
		static void HookProc( lua_State *pState, lua_Debug* pDebug );

		bool StepInHook( lua_State *pState,lua_Debug* pDebug );
		bool StepOutHook( lua_State *pState,lua_Debug* pDebug );
		bool StepOverHook( lua_State *pState,lua_Debug* pDebug );
		bool StepSwitchHook(lua_State *pState, lua_Debug* pDebug);

		void SetHook( lua_State* pState,bool bSetHook);
		void SetBaseState( lua_State* pState);

		bool m_bAtFileEnd;
		int m_nCurLevel;
		int m_nCurLine;

		void HandleHelp();
		void HandleLoad(lua_State* pState);
		void HandleList(lua_State* pState);
		void HandlePrint(lua_State* pState);
		void HandleFrame(lua_State* pState);
		void HandleShowCo(lua_State* pState);
		void HandleThread(lua_State* pState);
		void HandleInfo(lua_State* pState);
		void HandleDelBreak(lua_State* pState);
		void HandleAddBreak(lua_State* pState);

	public:	

		CScriptDebuger(lua_State* pState);
		void Release(lua_State* pState);

		void SetStepOver(lua_State* pState);
		void SetStepIn(lua_State* pState);
		void SetStepOut(lua_State* pState);
		void SetStepSwitch(lua_State* pState);
		
		void Debug(lua_State* pState);
	};
}
