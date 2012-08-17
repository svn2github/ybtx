gas_require "multiserver/TransferDataInc"

--[[   字符	说明	   
i	表示有符号整型
I	表示无符号整型
h	表示有符号短整型
H	表示无符号短整型
c	表示有符号字符型型
C	表示无符号字符型
f	表示单精度浮点
d	表示双精度浮点
s	表示长度小于256的字符串
S	表示长度小于65536的字符串
b	表示布尔类型
--]]

local ProList = 
{ 
	{"CheckConnect","sIi"},
	{"RetCheckConnect","sIIi"},
	
  {"SendGlobalAnousment", "S"},
  {"SendSuccConn","I"},--某个服务器成功连上另外一个服务器时，把自己的serverID发过去
  {"ShutDownNotifyOtherServer","I"},
  {"NotifyDestServerChangeScene","IIIIbff"},
  {"RetSrcServerChangeScene","IIII"},
  {"SetUserName","s"},--要新创建角色,通知要去的那个服务器(创建钥匙)
  {"RetUserKey","ss"},--创建角色的那个服务器,返回钥匙
  
  {"GetDbCall","I"},
  {"LogoutForAdultCheck","I"},
  {"SendDataBegin","Is"},
  {"SendDataEnd","I"},
  {"SendTableEnd","I"},
  {"CancelDelaySaveTick","Id"},
  
  {"NotifyPlayerGetMsg","bCI"},
  {"GetCacheMsg","CId"},
  {"ClearCacheMsg","I"},
  {"UserTipSigal","si"},--去其它的服务器顶号
  {"UserAutoLoginGame","I"}, 
  {"SendNpcChannelMsg", "sIsI"},
  {"SendPlayerChannelMsg", "IsISI"}, 
  {"InitTeamInfo", "III"},
  {"SendTeamMemberHPMP", "IIIIII"},
  {"ResetTeamMemberLevel", "I"},
  {"SendIBPayInfo", ""},
  {"SendMoneyExchangeInfo", ""},
  {"SendEratingAuditTick", ""},
  
  {"SetCharTongInfo", "III"},
  {"AddTongMember","IIIsIs"},
  {"DelTongMember","III"},
  {"SendTeachPropertyInfo", "I"},
  {"SendTongChannelMsg", "sIsIIS"},
  {"SendArmyChannelMsg","sIsIIS"},
--  {"CreateActionRoom","Is"},
  {"PlayerEnterActionScene","IIs"},
--  {"PlayerCancelAction","IIs"},
--  {"AddActionTeam", "sIII"},
  {"GetPkState", "II"},
  {"CheckAreaFbState", "IIIs"},
  {"SendResistanceValueToGac", "sII"},
  {"RetAreaFbState", "IsIIb"},
  {"KickOutOfCave", "I"},
  {"LeaveOutOfCave", "I"},
  {"DestroyDragon", "II"},
  {"DoSkillInPlayerSet", "IIsIs"},
    
  {"CreateRobScene", "Is"},
  {"UpdateRobResWin", ""},
  {"RetSendWarZoneState", "II"},
  
  {"GetRobResWinInfo", "IIssI"},
  {"ChangeByCancel", "II"},
  
  {"CheckJoinOreScene", "IIsI"},
  {"RetCheckJoinOreScene", "IIsI"},
  
  {"PlayerRetreatStation", "II"},
  {"SendTongMessage", "II"},
  {"UpdateCampExploit",""},
  {"CreateOreBossInScene", "IIsI"},
  {"SendOreBossMsg", "Isss"},
  {"PublicChatSendMsgDef","IsS"},
  {"ReturnSetCtrlType","III"},
  {"ResponseRequestJoinGroup","I"},
  {"ExchangeStation", "IIII"},
  {"CloseTongScene", "II"},
  {"NotifyPlayerLeaveFbScene","III"},
  
  {"RetSetMulExpActivity","s"},
  {"UpdataClosedAction","sbss"},
  {"DestroyNpcAppointByGM","s"},
  {"QuestNpcPosition","Is"},
  {"ReceiveQuestNpcPosition","CIS"},
  
  {"CreateScopesExplorationRoom","II"},
  {"RetCanTransFromTrap","I"},
  {"KickOffRole","s"},
  {"KickRole","I"},
  {"ShutUpRole","II"},
  {"CancelShutUpRole","IsI"},
  {"GMMoveRole","ssII"},
  {"GMSendWarning","sS"},
  {"GMSendMail","s"},
  {"SetPlayerExp","Is"},
  {"KickOffRoleForDelEquip","sIs"},
  {"SetServerType","I"},
  {"SetBuyCouponsAddress",""},
  {"UpdateJointSalesList",""},
  
	{"AgreeInviteJoinScene", "II"},
	{"InviteChangeScene", "IIIII"},
	
	{"FreeActionTime", "I"},
	{"SetAreaPlayerNum", "II"},
  {"RevertAreaPlayerNum",""},
  {"GMSetWarner","IIII"},
  {"SetJfsTeamNum", "I"},
  
  {"SendActionRollMsg","ss"},
  {"NotifyGasUpdate",""},
  
  {"SelectNeedAssign","IHIIs"},
  {"SelectAcutionAssign","IIbIs"},
	
  {"UpdataBossBattlePlayerTbl","sIIi"},
  {"BossBattleSendKillBossMsg","ssss"},
  {"GMBeginBossBattle",""},
  {"GMEndBossBattle",""},
  {"GMSetBossBattleMaxPlayer","I"},
  {"SendPkValueRollMsg","sI"},
  {"AddVarNumForTeamQuest", "IsI"},
  {"SendUpTongTypeMsg",  "s"},
  {"SetArmyCorpsInfoOnLeave", "IIs"},
  {"ReturnChangeGroupDeclare", "IS"},
  {"ReturnAddGroupMem","IsCIC"}, --群成员信息 参数：--玩家ID,玩家name,玩家职位标识(1:群主，2:管理员(最多只有三个)，3:普通成员),--玩家所在群ID,玩家是不是在线(1--在线，2--离线)
	{"ReturnDisbandGroup","I"},   --解散群返回 参数：群ID
	{"ReceiveMemberLeaveGroup","II"}, --返回剔除人 参数：玩家ID，群ID
	{"LogCreatedPipe",""},
	
	{"SetRecruitMethod", "IIII"},

	{"SetAllServerWarTime", "III"},
	{"StartAllServerWar", "I"},
	{"StopAllServerWar", ""},
	{"SysPlayerBuffTbl","Is"},
	{"ClearPlayerBuffTbl","I"},
	{"InitNewPanel","I"},
}




--这里只注册Gas2Gac要用到跨服发消息的函数(Gas2GacProList中需注册)
--即使用到 Gas2GacById的函数要到此来注册
CrossServerToGacList = 
{
	["TestFun"] = true,
	["SendNpcRpcMessage"] = true,
	["ChannelMessage"] = true,
	["PrivateChatReceiveMsg"] = true,
	["AreaPkOnMiddleMap"] = true,
	["RobResWinOnMiddleMap"] = true,
	
	["ReturnInviteMakeTeam"] = true,
	["ReturnAppJoinTeam"] = true,
	["UpdateAssignMode"] = true,
	["UpdateAuctionStandard"] = true,
	["UpdateAuctionBasePrice"] = true,
	["UpdateTeammateIcon"] = true,
	["StopSendTeammatePos"] = true,
	["StopSendTongmatePos"] = true,
	["HideLeavedTongmatePos"] = true,
	["HideLeavedTeammatePos"] = true,
	["ReturnGetTeamMemberBegin"] = true,
	["ReturnGetTeamMember"] = true,
	["ReturnGetTeamMemberEnd"] = true,
	["SendTeamMemberLevel"] = true,
	["NotifyOnline"] = true,
	["ReturnUpdateTeamMark"] = true,
	["ReturnUpdateTeamMarkEnd"] = true,
	["NotifyOffline"] = true,
	["CheckIsNeedSendTeammatePos"] = true,
	["CheckIsNeedSendTongmatePos"] = true,
	["SendTeammatePos"] = true,
	["SendTongmatePos"] = true,
	["RetAreaFbBossNum"] = true,
	["RetIsJoinAreaFb"] = true,
	["RetDelAllAreaFb"] = true,
	["RetIsJoinScopesFb"] = true,
	["RetIsJoinSenarioFb"] = true,
	["RetIsJoinMercenaryMonsterFb"] = true,
	
	["RetInsertFbActionToQueue"] = true,
	["RetIsJoinFbActionAffirm"] = true,
	["RetInFbActionPeopleNum"] = true,
	
	["RetShowTakeDareQuest"] = true,
	["ReturnLearnNewDirection"] = true,
	["ReturnLiveSkillExp"] = true,
	
	["AddBreakItemExp"] = true,
	["ReturnPosChanged"] = true,
	["RetDelItemFromGrid"] = true,
	["RetDelItemAllGMEnd"] = true,
	["ReturnBeOutOfTong"] = true,
	["ReseiveRecommendJoinTong"] = true,
	["ReseiveInviteJoinTong"] = true,
	["OnRequestJoinInTong"] = true,
	["ReseiveResponseBeInviteToTong"] = true,
	["ReturnJoinInTong"] = true,
	["RetDelQueueFbAction"] = true,
	["NotifyTeammateShareQuest"] = true,
	["ReturnProduceByTwoMaterials"] = true,
	["UpdatePlayerPoint"] = true,
	["RetChangeInfoWnd"] = true,
	
	
	["JoinAreaSceneMsgBox"] = true,
	["InviteJoinSceneMsgBox"] = true,
	["ReturnInviteToBeFriend"] = true,
	["ReturnAddFriendToClass"] = true,
	["ReturnBeDeleteByFriend"] = true,
	["ReturnSetCtrlType"] = true,
	["RecevieRequestJoinIntoFriendGroup"] = true,
	["ReturnGetAllFriendGroup"] = true,
	["ReturnGetAllFriendGroupEnd"] = true,
	["ReturnGetAllGroupMemInfo"] = true,
	["ReturnGetAllGroupMemInfoEnd"] = true,
	["ReturnRequestJoinIntoFriendGroupEnd"] = true,
	["PublicChatReceiveMsg"] = true,
	["ReceiveBeKickOutOfGroup"] = true,
	["ReceiveMemberLeaveGroup"] = true,
	["ReceiveInviteToGroup"] = true,
	["ReturnDisbandGroup"] = true,
	["ReturnChangeGroupDeclare"] = true,
	["ReturnLeaveGroup"] = true,
	["NotifyFriendOnline"] = true,
	["NotifyFriendGroupOnline"] = true,
	["NotifyFriendOffline"] = true,
	["NotifyFriendGroupOffline"] = true,
	
	["ReturnGetCompensateItemInfoBegin"] = true,
	["ReturnGetCompensateItemInfo"] = true,
	["ReturnGetCompensateItemInfoEnd"] = true,
	["ReturnActivationCodeCompensateItem"] = true,
	["ReturnTakeCompensateItem"] = true,
	["CreateWarTransprot"] = true,
	["CreateMatchGameWnd"] = true,
	["RetDelTicket"] = true,
	["RetGetTotalTicket"] = true,
	["SendUserGameID"] = true,
	["ReturnAddMoney"] = true,
	["ReturnAddBindingMoney"] = true,
	["RetItemMakerName"] = true,
	["ReturnAddBindingTicket"] = true,
	["ReturnAddTicket"] = true,
	["RetGiveUpQuest"] = true,
	["RetFinishQuest"] = true,
	["RetOreMapItemInfo"] = true,
	["SetItemGridWndState"] = true,
	["RetItemTypeInfo"] = true,
	["SetItemBindingTypeByID"] = true,
	["RetAddItemToGrid"] = true,
	["RetAddItemToGridEnd"] = true,
	["RetItemRoom"] = true,
	["RetAddItem"] = true,
	["RetAddItemToGridEndEx"] = true,
	["RetAddItemAllGMEnd"] = true,
	["RetSoulPearlItemInfo"] = true,
	["RetRefreshBag"] = true,
	["RetMoveItemByGridID"] = true,
	
	["RetDelItemError"] = true,
	["RetSplitItemEnd"] = true,
	["RetSplitItemError"] = true,
	["RetMoveItemEnd"] = true,
	["RetMoveItemError"] = true,
	["ReturnPlaceBag"] = true,
	["ReturnPlaceBagError"] = true,
	["RetReplaceItemByGridIDEnd"] = true,
	
	["ReturnSetBagState"] = true,
	["ReturnFetchBag"] = true,
	["ReturnFetchBagError"] = true,
	["ReturnDelBag"] = true,
	["ReturnDelBagError"] = true,
	["ReturnChange2Bag"] = true,
	["ReturnChange2BagError"] = true,
	["RetQuickMoveEnd"] = true,
	["RetQuickMoveError"] = true,
	["RetCleanBag"] = true,
	["ReturnBreakItemEnd"] = true,
	["ReceiveAntiIndulgenceExitGame"] = true,
	["ReturnBreakItemBegin"] = true,
	["ReturnBreakProducts"] = true,
	["RetItemLeftTime"] = true,
	["RetBoxitemOpened"] = true,
	["RetMailTextAttachmentInfo"] = true,
	["RetCommonBattleArrayBookInfo"] = true,
	["NoticePourIntensifyError"] = true,
	["NoticePourIntensifySuccess"] = true,
	["ReturnModifyPlayerSoulNum"] = true,
	["UpdateEquipIntensifyInfo"] = true,
	["NoticeIntensifySuccess"] = true,
	["NoticeAdvanceError"] = true,
	["RetActiveJingLingSkill"] = true,
	["RetActiveJingLingSkillPiece"] = true,
	["ReturnAddBindingMoney"] = true,
	["RetEquipDuraValueInBag"] = true,
	["RenewAllEquipEnd"] = true,
	["NoticeAdvanceSuccess"] = true,
	["AdvancedEquipReborn"] = true,
	["RetCommonWeaponInfo"] = true,
	["RetCommonArmorInfo"] = true,
	["RetCommonRingInfo"] = true,
	["RetCommonShieldInfo"] = true,
	["RetEquipIntensifyInfo"] = true,
	["ReturnChoosedTradeItem"] = true,
	["ReturnReplaceItem"] = true,
	["ReturnInviteeChoosedItem"] = true,
	["ReturnLockedTrade"] = true,
	["RetEquipAddAttrInfo"] = true,
	["RetEquipAdvanceInfo"] = true,
	["RetRenewEquipSuc"] = true,
	["RetEquipIntenBack"] =true,
	["ActionAllReady"] = true,
	["WaitOtherTeammate"] = true,
	["SetActionPanelWarnValue"] = true,

	["RetCloseCountScoreWnd"]	= true,
	["RetEquipEnactmentInfo"] = true,
	
	["AddItemMsgToConn"] = true,
	["ReturnFightingEvaluation"] = true,
	["ReturnCharInfo"] = true,
	["ShowTongMsg"]		= true,
	["TongTypeChanged"] = true,
	["OnInviteJoinArmyCorps"]	= true,
	["OnBeKickOutOfArmyCorps"] = true,
	["OnArmyCorpsPosChanged"] = true,
	
	["NotifyChangeAuctionPrice"] = true,
	["FinishAuction"] = true,
	["CloseNeedAssignWnd"] = true,
	["SendResistanceValue"] = true,
	["CreateRobTransprot"] = true,
	
	["ShowNewDirect"] = true,
	["RetShowWarZoneState"] = true,
	["EndSendWarZoneState"] = true,
	["UpdateDirectAwardInfo"] = true,
	["AddActionNum"] = true,
	["ReturnAddGroupMem"] = true,
	["ExitMatchGameSuccMsg"] = true,
	["RetEquipSuperaddRate"] = true,
	["RetUpdateEquipEffect"] = true,
	["RetAddFirewoodMsg"] = true,
	["RetNeedFireActivityEnd"] = true,
	["RetTongNeedFireActivityMsg"] = true,
	["ReturnProfferPoint"]	 = true,	
	["ReturnTeamProfferPoint"] = true,
	["RetCharLearnedTech"] = true,
	["ShowPanelByTongItem"] = true,
	
	["ReturnCampGetExploitSum"] = true,
	["ReturnTongWarAwardListInfoBegin"] = true,
	["ReturnTongWarAwardListInfo"]  = true,
	["ReturnTongWarAwardListInfoEnd"] = true
}

AddCheckLeakFilterObj(CrossServerToGacList)

local _, Gas2GacList = AliasDoFile("gac_gas", "framework/rpc/Gas2GacProList")
for _, v in pairs(g_MessageFunTbl) do
	CrossServerToGacList[v[1]] = true
end

for _, v in pairs(g_FriendMsgToClientFunTbl) do
	CrossServerToGacList[v[1]] = true
end

for funName in pairs(Db2GacFunList) do
	CrossServerToGacList["_Db2"..funName] = true
end

for i, v in pairs(Gas2GacList) do
	if CrossServerToGacList[v[1]] then
		table.insert(ProList, {v[1], "ICd" .. v[2]})
	end
end



--定义发送数据对的rpc函数

local TypeSignTbl = {
	["number"] = "d",
	["string"] = "s",
	["lstring"] = "S",
	["boolean"] = "b",
	["table"] = "",
}
for keyType, v in pairs(g_PairRpcDef) do
	for valueType, funName in pairs(v) do
		table.insert(ProList, {funName, "I" .. TypeSignTbl[keyType] ..  TypeSignTbl[valueType]})
	end
end


return ProList
