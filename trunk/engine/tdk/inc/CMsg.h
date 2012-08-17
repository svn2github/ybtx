#pragma once
#include "CMsgDefine.h"

namespace sqr
{

	typedef unsigned int MSG_CUSTOM_TYPE;

	union MSG_TYPE
	{
		MSG_TYPE():sys_type(MGT_SYS_IDLE){};
		MSG_TYPE(const MSG_SYSTEM_TYPE& type):sys_type(type){};
		MSG_TYPE(const MSG_KEY_TYPE& type):key_type(type){};
		MSG_TYPE(const MSG_MOUSE_TYPE& type):mouse_type(type){};
		MSG_TYPE(const MSG_UI_TYPE& type):ui_type(type){};
		MSG_TYPE(const MSG_CUSTOM_TYPE& type):custom_type(type){};

		MSG_SYSTEM_TYPE sys_type;
		MSG_KEY_TYPE	key_type;
		MSG_MOUSE_TYPE	mouse_type;
		MSG_UI_TYPE     ui_type;
		MSG_CUSTOM_TYPE	custom_type;
	};

	union MSG_INFO
	{
		MSG_INFO():_uinfo(0){};
		MSG_INFO(const MSG_MOUSE_INF& info):mouse_info(info){};
		MSG_INFO(const MSG_KEY_INF& info):key_info(info){};
		MSG_INFO(const unsigned& info):_uinfo(info){};
		MSG_INFO(const int& info):_iinfo(info){};
		MSG_INFO(const float& info):_finfo(info){};
		MSG_INFO(void* ptr):_handle(ptr){};

		MSG_MOUSE_INF	mouse_info;
		MSG_KEY_INF		key_info;
		unsigned int	_uinfo;
		int				_iinfo;
		float			_finfo;
		void*			_handle;
	};

	class CMsg
	{
	public:
		CMsg( const MSG_SOURCE& fSource,const MSG_TYPE& fType,const MSG_INFO& fInfo );

		const MSG_SOURCE	msgSource;
		const MSG_TYPE		msgType;
		const MSG_INFO		msgInfo;
	};

}
