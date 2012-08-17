#include "stdafx.h"
#include "ScriptCoroutine.h"
#include "ScriptCommon.h"
#include "CScriptData.h"
#include "ScriptLog.h"
#include "ScriptDebugHelper.h"
#include "ErrLogHelper.h"

namespace sqr
{
	int BeforeResume(lua_State* pState)
	{
		lua_State* co = lua_tothread(pState, -1);

		CScriptData* pData=CScript::Inst()->GetData();
		//把当前的running state设置成该coroutine
		pData->m_pRunningState = co;

		//得到切换coroutine之前的context stack
		void* pSaveStack = GetCurrentContextStack();

		//保存改context stack
		pData->SetContextStack(pState, pSaveStack);

		//得到切换coroutine处的context stack，并将其设置为当前的
		void* pStack = pData->GetContextStack(co);
		SetCurrentContextStack(pStack);

		lua_pushlightuserdata(pState, pSaveStack);

		return 1;
	}

	int AfterResume(lua_State* pState)
	{
		lua_State* co = lua_tothread(pState, 1);
		void* pSaveStack = lua_touserdata(pState, 2);

		CScriptData* pData=CScript::Inst()->GetData();

		bool bYield = (lua_status(co) == LUA_YIELD);

		if(bYield)
		{
			//如果coroutine是yield状态，那么我们保存该coroutine上面的context stack
			void* pCoStack = GetCurrentContextStack();
			pData->SetContextStack(co, pCoStack);
			SetCurrentContextStack(pSaveStack);
		}
		else //coroutine 正常结束或者失败
		{
			pData->RemoveContextStack(co);
			SetCurrentContextStack(pSaveStack);
		}

		//把当前的running state设置成调用该coroutine的state
		pData->m_pRunningState = pState;

		return 0;
	}


}
