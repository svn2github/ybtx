#pragma once
#include "CMsgDefine.h"
#include "TSingleton.h"

namespace sqr
{
	class CMsg;
	class CEventCollector;
	class CEventState : public Singleton<CEventState>
	{
		friend class CEventCollector;
	public:
		CEventState();
		~CEventState();

		MSG_KEY_TYPE	GetState(MSG_KEY_INF key_inf);
		MSG_MOUSE_TYPE	GetState(MSG_MOUSE_TYPE mouse_inf);
		MSG_MOUSE_INF	GetMousePosition(void);
	public:
		bool	UpdateState(const CMsg& msg);
		
	private:

		typedef vector<MSG_KEY_TYPE>	KeyStatePool;
		typedef	vector<MSG_MOUSE_TYPE>	MouseStatePool;

		KeyStatePool	m_KeyState;
		MouseStatePool	m_MouseState;
		MSG_MOUSE_INF	m_MousePos;
	};
}