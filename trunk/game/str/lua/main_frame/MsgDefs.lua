--压力测试处理消息

MsgTbl = 
{
	--连接相关消息
	ConnSucMsg = "ConnSucMsg", 
	ConnFailedMsg = "ConnFailedMsg", 
	DisConnMsg = "DisConnMsg", 
	ConnErrorMsg = "ConnErrorMsg",
	
	--登陆相关消息
	LoginSucMsg = "LoginSucMsg", 
	LoginFailedMsg = "LoginFailedMsg", 
	RepeatLoginMsg = "RepeatLoginMsg",
	
	--排队相关消息
	LoginWaitingMsg = "LoginWaitingMsg",
	DelWaitingMsg = "DelWaitingMsg",
	UpdateWaitingMsg = "UpdateWaitingMsg",
	
	--角色相关消息
	CreateRoleSucMsg = "CreateRoleSucMsg",
	CreateRoleFailedMsg = "CreateRoleFailedMsg",
	GetCharListMsg = "GetCharListMsg",
	CreatePlayerMsg = "CreatePlayerMsg",
	
	--切场景
	ChangeSceneSucMsg = "ChangeSceneSucMsg",
	ChangeSceneFailedMsg = "ChangeSceneFailedMsg",
	RetEnterOtherServerMsg = "RetEnterOtherServerMsg",
	
	--进比赛本相关
	SetLevelMsg = "SetLevelMsg",
	ReadyGoFbMsg = "ReadyGoFbMsg",
	ActionAllReadyMsg = "ActionAllReadyMsg",
	ClearActionTimesMsg = "ClearActionTimesMsg",
	ExitMatchGameMsg = "ExitMatchGameMsg",
}




