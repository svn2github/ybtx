#pragma once
//$id$
//对象系统gac到gas的协议定义头文件

#include "PrtlGas2GacSegment.h"

namespace sqr
{
	enum EGas2GacObjectCmdId
	{
		eGas2GacOCI_Create_Still_Object=eGas2GacPS_Object_Begin,
		eGas2GacOCI_Create_Moving_Object,
		eGas2GacOC_Check_Object_ViewVariant,
		eGas2GacOCI_Sync_View_Variant_End,
		eGas2GacOC_Init_View_Variant,
		eGas2GacOCI_Director_Create_End,
		
		eGas2GacOC_Check_Object_SyncVariant,
		eGas2GacOC_Init_Sync_Variant,
		eGas2GacOCI_Enter_Sync_Aoi,
		eGas2GacOCI_Leave_Sync_Aoi,
		
		eGas2GacOCI_Destroy_Object,
		eGas2GacOC_Set_Time_Distorted_Ratio,
		eGas2GacOCI_Set_Object_Pos,
		eGas2GacOCI_Sync_Object_Pos,
		
		eGas2GacOCI_Move_Follower_To,
		eGas2GacOCI_Change_Follower_Move_Target,
		eGas2GacOCI_Change_Follower_Move_Speed,
		eGas2GacOCI_Set_Main_Scene,
		
		eGas2GacOCI_Validate_Follower_Pos,
		eGas2GacOCI_Tell_Follower_Stepped_SmallDist,
		eGas2GacOCI_Tell_Follower_Stepped_MiddleDist,
		eGas2GacOCI_Tell_Follower_Stepped_BigDist,
		eGas2GacOCI_Tell_Follower_Stopped,
		eGas2GacOCI_Tell_Follower_Reached,
		eGas2GacOCI_Enable_Follower_Delay,
		eGas2GacOCI_Suspend_Follower_Moving,
		eGas2GacOCI_Resume_Follower_Moving,
		
		eGas2GacOCI_Set_Director_Active,
		eGas2GacOCI_Set_Director_Active_And_Move_To,
		eGas2GacOCI_Set_Director_Max_Speed,
		eGas2GacOCI_Set_Director_Passive,
		
		eGas2GacOCI_Static_Sync_Variant_Small_Data,
		eGas2GacOCI_Static_Sync_Variant_Middle_Data,
		eGas2GacOCI_Static_Sync_Variant_Big_Data,
		eGas2GacOCI_Agile_Sync_Variant_Small_Data,
		eGas2GacOCI_Agile_Sync_Variant_Middle_Data,
		eGas2GacOCI_Agile_Sync_Variant_Big_Data,

		eGas2GacOCI_Static_View_Variant_Small_Data,
		eGas2GacOCI_Static_View_Variant_Middle_Data,
		eGas2GacOCI_Static_View_Variant_Big_Data,
		eGas2GacOCI_Agile_View_Variant_Small_Data,
		eGas2GacOCI_Agile_View_Variant_Middle_Data,
		eGas2GacOCI_Agile_View_Variant_Big_Data,

		eGas2GacOCI_Sync_Dynamic_Barrier,
		
		eGas2GacOCI_Bind_To,
		eGas2GacOCI_Disbind_From_Parent,
		eGas2GacOCI_Disbind_All_Children,
		
		eGas2GacOCI_Count
	};
}


