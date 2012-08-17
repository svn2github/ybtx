#pragma once
//对shell暴露的头文件，不要放置引擎专用的代码

namespace sqr
{
	enum EServerObjectType
	{
		eSOT_Dictator,
		eSOT_Mediator,
		eSOT_Calculator
	};

	enum EServerObjectFlag
	{
		eSOF_None,
		eSOF_GridPosChangedNotifycation	=	1
	};
}

