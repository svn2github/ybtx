#pragma once
#include "TClientMsgRegister.h"
#include "PrtlGas2GacObjectDef.h"
#include "PrtlGac2GasGenericDef.h"
#include "PrtlGas2GacGenericDef.h"


//namespace sqr{
template<typename TraitsConcreateConn>
void TClientMsgRegister<TraitsConcreateConn>::RegisterMsgHandler()
{

	TClientMsgRegister<TraitsConcreateConn>::SetHandlerNumber(eGas2GacOCI_Count + eGas2GacGenericCmd_Count);

	//connection
	IntRegisterServerCommand				<CGas2GacGC_Ping_Client>								();
	IntRegisterServerCommand				<CGas2GacGC_Small_Shell_Message>						();
	IntRegisterServerCommand				<CGas2GacGC_Middle_Shell_Message>						();
	IntRegisterServerCommand				<CGas2GacGC_Big_Shell_Message>							();
	IntRegisterServerCommand				<CGas2GacGC_Tell_Server_Time>							();
	IntRegisterServerCommand				<CGas2GacGC_Tell_Error>									();
	IntRegisterServerCommand				<CGas2GacGC_Tell_Current_Version>						();
	IntRegisterServerCommand				<CGas2GacGC_Tell_OwnerInfo>								();
	IntRegisterServerCommand				<CGas2GacGC_Tell_Guids>									();
	//scene
	IntRegisterSceneCommand					<CGas2GacOC_Create_Still_Object>						();
	IntRegisterSceneCommand					<CGas2GacOC_Create_Moving_Object>						();
	IntRegisterSceneCommand					<CGas2GacOC_Destroy_Object>								();
	IntRegisterSceneCommand					<CGas2GacOC_Set_Time_Distorted_Ratio>					();

	IntRegisterSceneMgrCommand				<CGas2GacOC_Set_Main_Scene>								();

	//director
	IntRegisterDirectorCommand				<CGas2GacOC_Set_Dir_Max_Speed_And_Move_To>			();
	IntRegisterDirectorCommand				<CGas2GacOC_Set_Director_Active>						();
	IntRegisterDirectorCommand				<CGas2GacOC_Set_Dir_Max_Speed_Zero>						();
	IntRegisterDirectorCommand				<CGas2GacOCI_Director_Create_End>						();
	IntRegisterDirectorCommand				<CGas2GacOC_Set_Dir_Max_Speed>						();


	//follower
	IntRegisterFollowerCommand				<CGas2GacOC_Set_Object_Pos>								();
	IntRegisterFollowerCommand				<CGas2GacOC_Sync_Object_Pos>							();
	IntRegisterFollowerCommand				<CGas2GacOC_Enable_Follower_Delay>						();
	IntRegisterFollowerCommand				<CGas2GacOC_Check_Object_ViewVariant>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Sync_View_Variant_End>						();
	IntRegisterFollowerCommand				<CGas2GacOC_Check_Object_SyncVariant>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Entered_Sync_Aoi>							();
	IntRegisterFollowerCommand				<CGas2GacOC_Left_Sync_Aoi>								();
	IntRegisterFollowerCommand				<CGas2GacOC_Init_View_Variant>								();
	IntRegisterFollowerCommand				<CGas2GacOC_Init_Sync_Variant>								();

	IntRegisterFollowerCommand				<CGas2GacOC_Static_Sync_Variant_Small_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Static_Sync_Variant_Middle_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Static_Sync_Variant_Big_Data>						();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_Sync_Variant_Small_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_Sync_Variant_Middle_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_Sync_Variant_Big_Data>						();

	IntRegisterFollowerCommand				<CGas2GacOC_Static_View_Variant_Small_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Static_View_Variant_Middle_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Static_View_Variant_Big_Data>						();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_View_Variant_Small_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_View_Variant_Middle_Data>					();
	IntRegisterFollowerCommand				<CGas2GacOC_Agile_View_Variant_Big_Data>						();

	IntRegisterFollowerCommand				<CGas2GacOC_Sync_Dynamic_Barrier>						();

	IntRegisterFollowerCommand				<CGas2GacOC_Bind_To>									();
	IntRegisterFollowerCommand				<CGas2GacOC_Disbind_From_Parent>							();
	IntRegisterFollowerCommand				<CGas2GacOC_Disbind_All_Children>						();

	IntRegisterFollowerSlowDelayCommand		<CGas2GacOC_Move_Follower_To>							();
	IntRegisterFollowerSlowDelayCommand		<CGas2GacOC_Change_Follower_Move_Speed>					();

	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Change_Follower_Move_Target>				();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Tell_Follower_Stepped_SmallDist>			();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Tell_Follower_Stepped_MiddleDist>			();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Tell_Follower_Stepped_BigDist>						();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Tell_Follower_Stopped>						();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Tell_Follower_Reached>						();
	IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Validate_Follower_Pos>						();
	//IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Suspend_Follower_Moving>					();
	//IntRegisterFollowerFastDelayCommand		<CGas2GacOC_Resume_Follower_Moving>						();
}

//连接自己处理的消息
template <typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterServerCommand(){
	ImpConn_t::template RegisterServerCommand< CmdClass>();
}

template <typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterServerCommand(){
	ImpConn_t::template RegisterServerCommand< CmdClass>();
}


template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterSceneCommand(){
	ImpConn_t::template RegisterSceneCommand< CmdClass>();
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterSceneCommand(){
	RegisterServerCommand< CmdClass>();
}


//场景管理器处理的消息
template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterSceneMgrCommand(){
	ImpConn_t::template RegisterSceneMgrCommand< CmdClass>();
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterSceneMgrCommand(){
	RegisterServerCommand< CmdClass>();
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterFollowerFastDelayCommand(){
	ImpConn_t::template RegisterFollowerFastDelayCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerFastDelayCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterFollowerFastDelayCommand(){
	RegisterServerCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerFastDelayCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterFollowerSlowDelayCommand(){
	ImpConn_t::template RegisterFollowerSlowDelayCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerSlowDelayCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterFollowerSlowDelayCommand(){
	RegisterServerCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerSlowDelayCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterFollowerCommand(){
	ImpConn_t::template RegisterFollowerCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterFollowerCommand(){
	RegisterServerCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallFollowerCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::IntRegisterDirectorCommand(){
	ImpConn_t::template RegisterDirectorCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallDirectorCommand<CmdClass>);
}

template<typename TraitsConcreateConn>
template<typename CmdClass>
void TClientMsgRegister<TraitsConcreateConn>::RegisterDirectorCommand(){
	RegisterServerCommand< CmdClass>();
	//TClientMsgRegister::SetHandler<CmdClass>(&TClientMsgRegister::CallDirectorCommand<CmdClass>);
}

//}
