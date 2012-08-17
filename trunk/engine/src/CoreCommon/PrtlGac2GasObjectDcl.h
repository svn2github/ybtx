#pragma once
//$id$
//对象系统gac到gas的协议定义头文件

#include "PrtlGac2GasSegment.h"

namespace sqr
{
	enum EGac2GasObjectCmdId
	{
		eGac2GasOC_Tell_Mediator_Sync_Target_SyncVariant		=eGac2GasPS_Object_Begin,
		eGac2GasOCI_Tell_Mediator_Max_Speed_Set,
		eGac2GasOCI_Tell_Mediator_Stepped,
		eGac2GasOCI_Tell_Mediator_SetPosition,
		eGac2GasOCI_Report_Invalid_Step,		
		eGac2GasOC_Tell_Mediator_Sync_Target_ViewVariant,
		eGac2GasOCI_Tell_Mediator_Stopped,
		eGac2GasOCI_Report_Invalid_Move,					//报告对象的移动目标是无效的		
		eGac2GasOCI_Change_Mediator_Move_Target,
		eGac2GasOCI_Notify_Mediator_Run_Mode_Switched,
		eGac2GasOCI_Move_Mediator_To,			//报告对象的step路径于实际路径不符合
		eGac2GasOCI_Tell_Mediator_Reached,
		eGac2GasOCI_Change_Mediator_Move_Speed,

		eGac2GasOCI_Count
	};
}


