#pragma once
#include "CMsgDefine.h"
#include "TSingleton.h"

namespace sqr_tools
{
	class CHotKeyMap : public Singleton<CHotKeyMap>
	{
		friend class CEventState;
		friend class Singleton<CHotKeyMap>;
	public:
		typedef void HOT_KEY_CALL(MSG_KEY_INF key);
	
		void	RegAltHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key);
		void	RegCtrlHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key);
		void	RegShiftHotKey(HOT_KEY_CALL* pFun, MSG_KEY_INF key);

		void	DefaultHotKey(void);
	private:	
		CHotKeyMap();
		~CHotKeyMap();
		bool	PressHotKey(MSG_KEY_INF key);
		
	private:
		typedef vector<HOT_KEY_CALL*>	KEY_POOL;
		KEY_POOL	m_CtrlCallBack;
		KEY_POOL	m_ShiftCallBack;
		KEY_POOL	m_AltCallBack;
	};
}