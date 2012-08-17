local GlobalDbTransRegister = 
{
["DoItem"] = true,
["GetBuChangRange"] = true,
["DoItemByID"] = true,
["GetBuChangItemIDRange"] = true,

["CheckUserTrust"] = true,
["UserTrust"] = true,
["CancelUserTrust"] = true,
["CheckTrusteeLogin"] = true,

["TestDb2Gac"] = true,
["GetServerCanLoginNum"] = true,
["ForceLogout"] = true,
["AddQuestTheater"]=true, 
["KillNpcAddQuestDropObj"]=true, 
["OnSaveFightPropertyTick"]=true, 
["AllPlayerQuestFail"]=true,
["OnSaveSceneToDB"]=true, 
["OnSaveFBToDB"]=true, 
["EnterScene"]=true, 
["OrderTimeTest"]=true, 
["ExcuteTrade"]=true, 
["GetMoneyExchangeCess"]=true,
["GetAccountInfoFromDB"]=true, 
["GetAllActionMembers"]=true, 
["DelAllOvertimeMail"]=true, 
["GetCampById"]=true, 
["CheckCMOrderTimeOut"] = true,
["SelectPlayerInfoFromLocalDB"]=true, 
["UpdatePasswd"]=true, 
["SaveLogoutInfo"]=true, 
["SendQuitRecord"]=true, 
["SaveAndUpTime"]=true, 
["CopyUnSentRecord"]=true, 
["updateDaTaoShaPoint"]=true, 
["SavePlayerDeadInfo"]=true, 
["UpdateDataInfo"]=true,
["_DeleteScene"]=true,
["_CreateScene"]=true,
["SelectAll"]=true, 
["SelectByName"]=true, 
["SelectByIP"]=true, 
["ServerTimeUpdateTick"]=true, 
["ResetSoulPearlTime"]=true,
["ClearItemAmountInfo"]=true,
["GasStartUp"]=true, 
["GasStartUpFinalize"]=true, 
["ClientLogout"]=true, 
["GetLogoutData"] = true,
["LogoutSave"] = true,
["LogoutDelaySaveData"] = true,
["LogoutImmediatelySaveData"] = true,
["InsertGmCommandLog"]=true, 
["GetGmLevel"]=true, 
["CreateSortTbl1"]=true, 
["CreateSortTbl2"]=true, 
["CreateSortTbl3"]=true, 
["CreateSortCharInfoFunc1"]=true, 
["CreateSortCharInfoFunc2"]=true, 
["CreateSortCharInfoFunc3"]=true, 
["CreateSortCharInfoFunc4"]=true, 
["CreateSortCharInfoFunc5"]=true, 
["CreateSortCharInfoFunc6"]=true, 
--["GetAllRobTong"]=true,
["DeleteTongAllInfo"]=true, 
["GetOnlineInfo"]=true,
["GetOnlinePlayerInfo"]=true,
["GetSceneServerId"] = true,
["GetCampExploitSumDB"] = true,
["UpdateCampExploitDB"] = true,
["GetAllMemAndCeByTongIDRpc"] = true,
["GetOnlinePlayerUsedAct"]=true,

["GetStationTimeInfo"]=true,
["GetTongStationOnly"]=true,
["DeleteIssueInfo"]=true,

["GetUserComenpate"]=true,
["GetCDepotItemByPage"]=true, 
["GetTeamMembersByChar"]=true, 
["GetTeamStaticID"]=true,
["AddTempTeamlog"]=true,
["GetTeamCapAndMem"]=true, 
["GetPlayerLevelAndTeamID"]=true,
["GetTongBuildingInfo"]=true,
["DestroyBuilding"]=true,
["BuildingIsDestroy"]=true, 
["FbActionAward"] = true,
["MonsAttackTongAward"] = true,
["GetPartyOnlinePlayer"] = true,
["GetTongMemberLevelInfo"] = true,
["GetTongMaxLevelRpc"] = true,
["DelActionItem"] = true,
["SendMailGiveGatherItem"] = true,
["GetTongMember"] = true,
["GetTongRelativeLine"] = true,
["GetLogsInfoRPC"] = true,
["GetRequestInfoRPC"] = true,
["GetTongInfoDB"] = true,
["GetTongInfoForInit"] = true,
["GetTongTeachInfoDB"] = true,
["GetTongTeachInfoByCharID"] = true,
["AutoChangeCaptain"] = true,
["CheckAndChangeTongCaptain"] = true,
["AutoRepairBuilding"] = true,
["CalculateTongDevelopDegree"] = true,
["SendMailToTongCaptainDB"]  = true,
["SendWarAwardDB"] = true,
["ClearDataInDataBaseDB"] = true,

["AuthSuccessAndInsertLocalDB"] = true,
["GetAccountInfoFromID"] = true,

["GetAccountInfoFromName"] = true,
["GetOneCharLeftTimeInfo"] = true,
["GetAllCharLeftTimeInfo"] = true,
["IsSceneExistent"] = true,
["NeedAssignGiveItem"] = true,
["AcutionAssignGiveItem"] = true,
--["SelectTong"] = true,
["GetAllMemByTongIDRpc"] = true,
["GetTongId"] = true,
["GetHotSaleItemInfo"] = true,
["ClearBuyedInfoByTime"] = true,
["ClearContributeMoneyInfo"] = true,
["GetAllMoneyExchangeInfo"] = true,
["GetAllIBPayData"] = true,
["UpdateIBPayState"] = true,
["UpdateExchangeState"] = true,
["UserChargeAuditDB"] = true,
["UserChargeDB"] = true,
["PayListQueryDB"] = true,
["PayDetailDemandDB"] = true,
["AddUserDrawBalanceInfo"] = true,
["DelUserDrawBalanceInfo"] = true,
["GetUserDrawBalanceInfo"] = true,
["ClearDrawBalanceMemory"] = true,
["AddAuditInfo"] = true,
["ClearWMXInfo"] = true,
["AssignUserSoulAndExp"] = true,
["ClearWMXAndCessGM"] = true,
["GetAuditInfo"] = true,
["DelAuditInfo"] = true,
["DelCompenateItem"] = true,
["CreateRole"] = true,
["GetBackRole"] = true,
["TextExecute"] = true,
["ShutUp"] = true,
["ChannelShutUp"] = true,
["FreezeAccountDB"] = true,
["CancelFreezeAccountDB"] = true,
["FreezeAccountById"] = true,
["CancelFreezeAccountById"] = true,
["CheckUserLoginGame"] = true,
["UserAutoLoginGame"] = true,
["CanceLoginGame"] = true,
["DelLogoutUserInfo"] = true,
["AddMoney"] = true,
["AddBindingMoney"] = true,
["AddTicket"] = true,
["AddExp"] = true,
["AddQuest"] = true,
["FinishQuest"] = true,
["DelQuest"] = true,
["AddItem"] = true,
["AddSoul"] = true,
["AddBindingSoul"] = true,
["AddGMMgr"] = true,
["DelGMMmr"] = true,
["DelCurrencyLimit"] = true,
["DelJifenUseLimit"] = true,
["DelItemUseLimit"] = true,
["AddCurrencyLimit"] = true,
["AddJifenUseLimit"] = true,
["AddItemUseLimit"] = true,
["DelItemInBag"] = true,
["DelItemOnBody"] = true,
["GetEquipItemOnBody"] = true,
["DelItemInDepot"] = true,
["DelItemInTongDepot"] = true,
["SearchClosedAction"] = true,
["InsertClosedAction"] = true,
["DeleteClosedAction"] = true,
["CheckClosedAction"] = true,
["InitGasConf"] = true,
["GetRoomInfoForCreateScene"] = true,
["CancelAllAction"] = true,
["AddPointForPlayerTbl"] = true,
["GetAllActionName"] = true,

["CancelActionByRoom"] = true,
["CancelAction"] = true,
["UpdateWarnValue"] = true,
["ClearAllWarnValue"] = true,
["SaveMatchGameServerLog"] = true,
["SaveXianxieLog"] = true,
["SavePVPXianxieRes"] = true,
--["GetObjName"] = true,
["DeleteActionTeam"] = true,
["WaitPlayerEnd"] = true,
["RetreatStationRpc"] = true,
["PlayerRetreatStation"] = true,
["ExchangeStation"] = true,
["GetAllLineChallengeInfo"] = true,
["GetTongChallengeBuildingInfo"] = true,
["TongBuildingDeadSubResource"] = true,
["UpdateHonor"]									 = true,
["GetPlayerLevel"]							 = true,
["DelFbPosition"] = true,
["LeaveSceneSetQuestFailure"] = true,
["SelectAllTong"] = true,
["DeleteTempRobResData"] = true,
["GetRobResState"] = true,
["GetRobResDataTong"] = true,
["SelectAllTongId"] = true,
["UpdateRobResState"] = true,
["AddRobResState"] = true,
["SelectObjNameByCharId"] = true,
["FormitLeaveFb"] = true,
["StartClearAwardInfo"] = true,
["DelHeaderAwardInfo"] = true,

["SelectAllInfo"] = true,
["EnterAreaFbScene"] = true,
["AddAreaFbCount"] = true,
["AddTeamAreaFbCount"] = true,
["UpdateOnlineStat"] = true,
["GetGlobalUserNum"] = true,
["SetSceneServerRpc"] = true,
["UpdateHonorAndBattleState"] = true,
["GetTongChallengeBattleInfo"] = true,
["GetTongChallengeBattleCount"] = true,
["GetTongStationInfo"] = true,
["GetDbCall"] = true,
["ClearArg"] = true,
["ReturnMoneyAndClearChallengeInfo"] = true,
["UpdateSelfServerBattleStateAndSendMsg"] = true,
["ClearSelfServerChallengeBattleInfo"] = true,
["ChallengeEndUpdateHonor"] = true,
["GetChlgerNameAndTgtName"] = true,
["CanEnterWarZone"] = true,
["AddWinLogAndSendMsg"] = true,
["CheckInFbScene"] = true,
["ExitAndGoBack"] = true,
["StationMoveGetTongStationInfo"] = true,
["OccupyStationRpc"] = true,
["SetMaxOnLineUserNum"] = true,
--["GetUserWaitQueueSequence"] = true,
["CheckPlayerInFbBySceneName"] = true,
["GetPlayerPanelInfo"] = true,
["GutQuestFailure"] = true,
["SetLatencyToDB"] = true,
["ReadCfgToCSMDBTable"] = true,
["DeleteObjInfo"] = true,
["DeleteResourceInfo"] = true,
["AddRobResTong"] = true,
["SaveServantInfo"] = true,
["GetItemLeftTimeByPlayer"] = true,
["ResumeServantByTransfered"] = true,
["SaveTruckInfo"] = true,
["OnSaveAllPlayerJobEnd"] = true,
["GM_SetActionTeamNum"] = true,
["ModifyPlayerSoul"] = true,
["OnSaveBurstSoulTime"]	= true,
["TempMemberExpAdd"] = true,
["DelNoUseItemId"] = true,
["AddQuestFinishLimit"] = true,
["DelQuestFinishLimit"] = true,
["AddObjDropLimit"] = true,
["DelObjDropLimit"] = true,
["SendMail"] = true,
["SendMailNotice"] = true,
["GetAllForageInfo"] = true,
["ClearPkInfo"] = true,
["ClearPkFlagInfo"] = true,
["GetDrawForagTime"] = true,
["UpdateForageInfoWeek"] = true,
["GetStatTongInfo"] = true,
["PrivateChatRequest"] = true,
["SendGMMsg"] = true,
["AddDragonCaveTong"] = true,
["DeleteDragonCave"] = true,
["DeleteDragonBuilding"] = true,
["CheckDragonCaveList"] = true,
["GetSceneTbl"] = true,
["CheckSceneState"] = true,
["SendAwardToTong"] = true,
["BossBattleAward"] = true,
["CreateBossBattleScene"] = true,
["GetCharId"] = true,
["RoleRename"] = true,
["TongRename"] = true,
["RemoveTongMember"] = true,
["GetQueryInfo"] = true,

["ChangeTongMember"] = true,
["ChangeTongLevel"] = true,
["AddItemToTong"] =  true,
["AddMoneyToTong"] =  true,
["AddResourceToTong"] =  true,
["AddHonorToTong"] =  true,
["GetTongId"] = true,
["GetPlayerIdByName"] = true,

["GetPatchNum"] = true,
["AddHotUpdateCode"] = true,
["AddHotUpdateResult"] = true,
["GetHotUpdateDb"] = true,
["GetHotUpdateGasGac"] = true,
["DelChar"] = true,
["GetBackRole"] = true,
["ClearActionJoinCount"] = true,
["ClearCurCycAndHistoryCount"] = true,
["AddSuccessRecordCount"] = true,
["AddXiuXingTaCount"] = true,
["RecordActionTimes"] = true,
["GetFightingEvaluationDB"] = true,
["ClearItemInBag"] = true,
["ClearItemInDepot"] = true,
["ClearChannelChat"] = true,
["SetConfServer"] = true,
["BuyItemFromWebShop"] = true,
["AddDrawBalanceInfo"] = true,
["IsDrawBalanceSucc"] = true,
["SaveFightingEvaluation"] = true,
["GetBackWeapon"] = true,
["GetBackArmor"] = true,
["GetBackShield"] = true,
["GetBackRing"] = true,
["GetBackArmor"] = true,
["ReturnAuctionMoney"] = true,
["ValidateAuctionMoney"] = true,
["GetGMSLangs"] = true,
["UpdateRecruitInfo"] = true,
["TempBackData"] = true,

["AddTongToArmyCorpsDB"] = true,
["GetArmyCorpsId"] = true,
["StorageSoulValue"] = true,
["SetAllServerMaxUserNum"] = true,
["GetDbTransInfo"] = true,
["EnableSqlProfiler"] = true,
["GetDbSqlProfilerData"] = true,
["SaveServiceOnlineUserInfo"] = true,

["UpdateCupStatus"] = true,
["GetRecruitInfo"] = true,
["SetGmRecruitInfo"] = true,
["IsAutoRecruit"] = true,
["AddGmRecruitInfo"] = true,
["GetRecruitCampInfo"] = true,
["UpdateEquipEffectDB"] = true,
["TongMemberAddExp"] = true,
["CleanTongNeedFire"] = true,
["GetRecruitSetting"] = true,

["UpdateCouponsInfo"] =true,
["GetBuyCouponsWebInfo"] = true,
["SaveWaitQueueCountDB"] = true,
["GetVIPEffect"] = true,
["SendMailItemToVip"] = true,
["Recover"] = true,
["GetTongLevelBackToHighSpeed"] = true,
["SendWelfareMailItem"] = true,
}

g_CallDbTransReg = GlobalDbTransRegister
AddCheckLeakFilterObj(g_CallDbTransReg)

local ProcessFunc =
{
	["FlushDbCall"] = FlushDbCall,
	["Db2CallBack"] = Db2CallBack,
	["Db2GacById"] = FlushDb2Gac,
}

local function doDbProcessMsg(processResult)
	for name, v in pairs(processResult) do  --目前只有 FlushDbCall 要处理
		ProcessFunc[name](v)
	end
end

--在处理所有callback时, 
local function GetProcessCallBack(callback)
	local function ProcessCallBack(...)
		local arg = {...}
		local processResult, funResult = arg[1], arg[2]
		if type(processResult) == "table" then
			doDbProcessMsg(processResult)
		end
		if callback then
			callback(unpack(funResult, 1, funResult.n))
		end
	end
	return ProcessCallBack
end

function CallDbTrans(filename, funcname, callback, data, ...)
	if not GlobalDbTransRegister[funcname] then
		error("全局数据库操作未被注册")
	end
	MultiDbTrans(filename, funcname, GetProcessCallBack(callback), data, nil, ...)
end

function PCallDbTrans(filename, funcname, callback, errfunc, data, ...)
	if not GlobalDbTransRegister[funcname] then
		error("全局数据库操作未被注册")
	end
	MultiDbTrans(filename, funcname, GetProcessCallBack(callback), data, errfunc, ...)
end

function PCallAccountLoginTrans(account, filename, funcname, callback, errfunc, data, ...)
	if not account then
		LogErr("PCallAccountLoginTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	account:LoginTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), errfunc, data, ...)
end

function CallAccountLoginTrans(account, filename, funcname, callback, data, ...)
	if not account then
		LogErr("CallAccountLoginTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	account:LoginTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), nil, data, ...)
end

function PCallAccountManualTrans(account, filename, funcname, callback, errfunc, data, ...)
	if not account then
		LogErr("PCallAccountManualTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	return account:ManualGameTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), errfunc, data, ...)
end

function CallAccountManualTrans(account, filename, funcname, callback, data, ...)
	if not account then
		LogErr("CallAccountManualTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	return account:ManualGameTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), nil, data, ...)
end

function PCallAccountAutoTrans(account, filename, funcname, callback, errfunc, data, ...)
	if not account then
		LogErr("PCallAccountAutoTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	account:AutoGameTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), errfunc, data, ...)
end

function CallAccountAutoTrans(account, filename, funcname, callback, data, ...)
	if not account then
		LogErr("CallAccountAutoTrans时account为nil:" .. filename .. "." .. funcname)
		return
	end
	account:AutoGameTrans(MultiDbTrans, filename, funcname, GetProcessCallBack(callback), nil, data, ...)
end

local CheckDbTrans = CheckDbTrans
local g_ChildVMNum = g_ChildVMNum
--在指定的vm上面调用db trans，这种trans必然没有channel
function CallSpecialVMDbTrans(uVM, filename, funcname, callback, errfunc, data)
	if uVM > g_ChildVMNum then
		error("指定的vm的编号不正确")
	end
		
	CheckDbTrans(filename, funcname)
	
	local tickIndex
	if g_bDBCallBackCheck then
		tickIndex = RegisterDBCallbackCheckTick(funcname)
	end
		
	CallDbTransaction(uVM, filename, funcname, GetProcessCallBack(callback), data, errfunc, tickIndex)
end
