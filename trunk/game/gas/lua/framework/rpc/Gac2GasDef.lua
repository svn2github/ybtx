gas_require "framework/rpc/Gac2GasInc"
gac_gas_require "framework/rpc/RemoteInfo"
gas_require "framework/gm_command/GMCommand"
gas_require "entrance/login/LoginServer"
gas_require "entrance/login/GasCreateRole"
gas_require "entrance/login/GasLoginCheck" --网易通行证
gas_require "commerce/npc_shop/NpcShop"
gas_require "commerce/players_trade/CPlayerTran"
gas_require "item/equip/EquipMgr"
gas_require "commerce/consignment/Market"
gas_require "commerce/PurchasingAgent/PurchasingAgent"
--gas_require "relation/tong/GasTongMarket"
gas_require "item/boxitem/BoxItemSever"
gas_require "activity/FbActionPanel"
--gas_require "world/tong_area/FetchRes"
gas_require "smallgames/HeadGameMgr"
gas_require "world/facial_action/FaceAction"
gas_require "fight/pet/GasPetInfo"
gas_require "commerce/ContractManufacture/GasContractManufacture"
gas_require "item/equip/GasEquipRefine"
gas_require "item/equip/GasEquipSuperadd"

--压测的时候,才会把这些消息通知客户端
function StressRpcToClient(ClassName, func, ...)
	if GasConfig.StressCheck == 1 then
		ClassName[func](ClassName,...)
	end
end

local tblErrMap = {}
tblErrMap[-1] = "rpc函数id错误"
tblErrMap[-2] = "验证码校验出错，可能出现网络消息错误"
tblErrMap[-3] = "数据格式错误"
tblErrMap[-4] = "没有找到该rpc处理函数"
tblErrMap[-5] = "没有Gac2Gas"

function Gac2Gas:OnError(num, fun)
--[[
		-1 ：表示id错误
		-2 ：表示验证码错误
		-3 ：表示数据格式错误
		-4 ：表示没有找到处理函数
		-5 ：表示返回的错误
		>0 ：表示用户自己返回的参数
--]]
	if num > 0 then
		error(fun .. " rpc call error: " .. "rpc函数不允许有返回值" )
		return false
	end
			
	error(fun .. " rpc call error: " .. tblErrMap[num] )
	return false
end

function Gac2Gas:ByteCall(Pipe,x,y )
	--print("recv data" .. x .. y)
end

-- 远程变量读取: 
-- GetValue  获取单个变量	  MS MakeSure 方式
-- GetValues 获取多个变量     CB CallBack 方式
-- GetType   获取单个变量类型

function Gac2Gas:Remote_MS_GetValue( Pipe, strVar )
	local retStr = Remote._GetValue( strVar )
	local ret = Gas2Gac:Remote_ResumeResult( Pipe, retStr )
end

function Gac2Gas:Remote_MS_GetValues( Pipe, strVar )
	local retStr = Remote._GetValues( strVar )
	local ret = Gas2Gac:Remote_ResumeResult( Pipe, retStr )
end

function Gac2Gas:Remote_MS_GetType( Pipe, strVar )
	local retStr = Remote._GetType( strVar )
	local ret = Gas2Gac:Remote_ResumeResult( Pipe, retStr )
end

function Gac2Gas:Remote_CB_GetValue( Pipe, strVar )
	local retStr = Remote._GetValue( strVar )
	local ret = Gas2Gac:Remote_ReceiveResult( Pipe, retStr )
end

function Gac2Gas:Remote_CB_GetType( Pipe, strVar )
	local retStr = Remote._GetType( strVar )
	local ret = Gas2Gac:Remote_ReceiveResult( Pipe, retStr )
end

function Gac2Gas:Remote_ResumeResult( Pipe, strStr )
	coroutine.resume( Remote.co, strStr )
end


-- GM Command : 相应Gac提交GM指令
function Gac2Gas:GM_Execute( Pipe, strCmd )
	local strAvailable, strErrorMsg, strCommand, tVarTable, funcExecutor = GMCommand:_Available( strCmd )
	if ( not strAvailable ) then
		Gas2Gac:Remote_ReceiveResult( Pipe, "e^^Invalid GM Command -> "..(strErrorMsg or "") )
		return
	end
	local function CallBack(nLevel)
		if ( GMCommand:_Validate( Pipe, strCommand,nLevel ) ) then
			local strResult = GMCommand:_Execute( Pipe, funcExecutor, tVarTable )
			Gas2Gac:Remote_ReceiveResult( Pipe, strResult )
		else
			Gas2Gac:Remote_ReceiveResult( Pipe, "e^^You\'re not allowed to use this GM Command!" )
		end
	end
	if not (Pipe and IsCppBound(Pipe) and Pipe.m_Player) then return end
	local data ={
		['ip'] = Pipe.m_strIP,
		['name'] = Pipe.m_UserName,
		['content'] = strCmd,
		['game_id'] = Pipe.m_nGameID,
	}
	local gmlogger = "GMCmdLogger"
	CallDbTrans(gmlogger, "InsertGmCommandLog", CallBack, data,Pipe.m_Player.m_AccountID)
end

function Gac2Gas:GM_RunScript( Pipe, strScript )
	if ( GMCommand:_Validate( Pipe, "runscript" ) ) then
		local strResult = GMCommand:_RunScript( Pipe, strScript )
		Gas2Gac:Remote_ReceiveResult( Pipe, strResult )
	else
		Gas2Gac:Remote_ReceiveResult( Pipe, "e^^You\'re not allowed to use this GM Command!" )
	end
end

function Gac2Gas:GM_OpenConsole(Pipe)
	local function CallBack(nGMLevel)
		local bFlag = GMCommand:_Validate( Pipe, "", nGMLevel) 
		Gas2Gac:RetOpenGMConsole(Pipe, bFlag)
	end
	local data = {["user_name"] = Pipe.m_UserName,["game_id"] = Pipe.m_nGameID}
	CallDbTrans("GMDB", "GetGmLevel", CallBack, data,Pipe.m_Player.m_AccountID)
end

--for stress test only
function Gac2Gas:TestRpc( Pipe, num1, num2, str1, str2, num3, num4, 
					str3, str4, str5, str6, str7, str8, str9 )
	--print(num1)
end


function Gac2Gas:DeleteHandForage(Conn)
	local Player = Conn.m_Player
	if not Player or not IsCppBound(Player) then
		return 
	end
	CFetchRes.DoForageNum(nil, Player)
end


function Gac2Gas:GetTongFethResInfo(Conn)
	CFetchRes.GetTongFethResInfo(Conn)
end


function Gac2Gas:FetchResource(Conn)
	local Player = Conn.m_Player
	CFetchRes.GetHandAndResForageNum(nil, Player)
end
	
--宝石相关AAA
function Gac2Gas:RequestOpenHole(Conn,Panel,Depart,Slot,ItemID)
	CStoneMgr.GetStoneMgr():RequestOpenHole(Conn,Panel,Depart,Slot,ItemID)
end

function Gac2Gas:RequestInlayStone(Conn,Panel,Depart,Slot,StoneId)
	CStoneMgr.GetStoneMgr():RequestInlayStone(Conn,Panel,Depart,Slot,StoneId)
end	

function Gac2Gas:RequestRemovalStone(Conn,Panel,Depart,Slot)
	CStoneMgr.GetStoneMgr():RequestRemovalStone(Conn,Panel,Depart,Slot)
end	

function Gac2Gas:ChangeFrame(Conn,oldFrameID,FrameID)
	CStoneMgr.GetStoneMgr():ChangeFrame(Conn,oldFrameID,FrameID)
end

function Gac2Gas:SendSynthesisItemBegin(Conn)
	CStoneMgr.GetStoneMgr():SendSynthesisItemBegin(Conn)
end

function Gac2Gas:SendSynthesisItem(Conn,ItemId)
	CStoneMgr.GetStoneMgr():SendSynthesisItem(Conn,ItemId)
end

function Gac2Gas:SendSynthesisItemEnd(Conn)
	CStoneMgr.GetStoneMgr():SendSynthesisItemEnd(Conn)
end

function Gac2Gas:RequestStoneAppraiseAgain(Conn)
	CStoneMgr.GetStoneMgr():RequestStoneAppraiseAgain(Conn)
end

function Gac2Gas:TakeAppraisedStone(Conn)
	CStoneMgr.GetStoneMgr():TakeAppraisedStone(Conn)
end
--宝石相关EEE

--进领域副本
function Gac2Gas:EnterScopesFb(Conn, SceneName)
	local ScopesExploration = CScopesExploration:new()
	ScopesExploration:EnterScopesFb(Conn, SceneName)
end
function Gac2Gas:AgreedJoinScopesFb(Conn, SceneName, PosX, PosY)
	local ScopesExploration = CScopesExploration:new()
	ScopesExploration:AgreedJoinScopesFb(Conn, SceneName, PosX, PosY)
end

--进剧情副本
function Gac2Gas:ShowJuQingWnd(Conn, EntityID)
	CScenarioExploration.ShowJuQingWnd(Conn, EntityID)
end
function Gac2Gas:EnterSenarioFb(Conn, SceneName, IsAutoTeam)
	CScenarioExploration.EnterScene(Conn, SceneName, IsAutoTeam)
end
function Gac2Gas:AgreedJoinSenarioFb(Conn, SceneName, PosX, PosY)
	CScenarioExploration.AgreedJoinScene(Conn, SceneName, PosX, PosY)
end

function Gac2Gas:GetForageAutoInfo(Conn)
	if not IsCppBound(Conn) then
		return
	end
	GetTongPos(Conn)
end

function Gac2Gas:OpenAutoSeekWnd(Conn)
	if not IsCppBound(Conn) then
		return
	end
	GetAutoSeekInfo(Conn)
end

function Gac2Gas:FbStationAutoSeekInfo(Conn)
	if not IsCppBound(Conn) then
		return
	end
	GetFbStationAutoSeekInfo(Conn)
end

--发布征粮令
function Gac2Gas:IssueTongInfo(Conn)
	if not IsCppBound(Conn) then
		return
	end
	GetIssueFetchInfo(Conn)
end

--function Gac2Gas:IssueFetchQueryFun(Conn)
--	if (not g_GameMain.m_IssueFetchWnd) then
--		g_GameMain.m_IssueFetchWnd = CTongIssueFetchWnd:new(g_GameMain)
--	end
--	g_GameMain.m_IssueFetchWnd:ShowWnd(true)
--end

--------------------------------任务相关-------------------------------
function Gac2Gas:AcceptQuest(Conn, QuestName, ObjEntityID)
	CRoleQuest.AcceptQuest(Conn, QuestName, ObjEntityID)
end
function Gac2Gas:AcceptQuestJoinFb(Conn, QuestName, SceneName)
	CRoleQuest.AcceptQuestJoinFb(Conn, QuestName, SceneName)
end
function Gac2Gas:FinishQuest(Conn, sQuestName, SelectIndex, ObjEntityID)
	CRoleQuest.FinishQuest(Conn, sQuestName, SelectIndex, ObjEntityID)
end
function Gac2Gas:GiveUpQuest(Conn,QuestName)
	CRoleQuest.GiveUpQuest(Conn,QuestName)
end
function Gac2Gas:RequestQuestByItem(Conn,QuestName)
	CRoleQuest.RequestQuestByItem(Conn,QuestName)
end
function Gac2Gas:ResponseShareQuest(Conn, QuestTeamID, QuestName)
	CRoleQuest.ResponseShareQuest(Conn, QuestTeamID, QuestName)
end
function Gac2Gas:SetCheckInMercenaryInfo(Conn)
	CRoleQuest.SetCheckInMercenaryInfo(Conn)
end
function Gac2Gas:SetLockingNPCID(Conn,NpcID)
	CRoleQuest.SetLockingNPCID(Conn,NpcID)
end
function Gac2Gas:SetQuestFailure(Conn, QuestName)
	CRoleQuest.SetQuestFailure(Conn, QuestName)
end
function Gac2Gas:SetQuestIsLimitTime(Conn, QuestName)
	CRoleQuest.SetQuestIsLimitTime(Conn, QuestName)
end
function Gac2Gas:SetQuestLevelVarNum(Conn, QuestName, varName)
	CRoleQuest.SetQuestLevelVarNum(Conn, QuestName, varName)
end
function Gac2Gas:ShareQuest(Conn, QuestName)
	CRoleQuest.ShareQuest(Conn, QuestName)
end
function Gac2Gas:ShowFinishQuestWnd(Conn,sQuestName, EntityID)
	CRoleQuest.ShowFinishQuestWnd(Conn,sQuestName, EntityID)
end
function Gac2Gas:ShowTakeQuestWnd(Conn,QuestName, EntityID)
	CRoleQuest.ShowTakeQuestWnd(Conn,QuestName, EntityID)
end

function Gac2Gas:CreateMercenaryLevel(Conn)
	CMercenaryLevel.CreateMercenaryLevel(Conn)
end
function Gac2Gas:MercenaryLevelUp(Conn)
	CMercenaryLevel.MercenaryLevelUp(Conn)
end
function Gac2Gas:DoMercenaryLevelAppraise(Conn)
	CMercenaryLevel.DoMercenaryLevelAppraise(Conn)
end
function Gac2Gas:GetMercenaryLevelAward(Conn, Type)
	CMercenaryLevel.GetMercenaryLevelAward(Conn, Type)
end
function Gac2Gas:SetMercenaryLevelAward(Conn, Award)
	CMercenaryLevel.SetMercenaryLevelAward(Conn, Award)
end
------------------------------充值兑换-------------------------------------------------
function Gac2Gas:ChangeTicketToMoney(Conn,nTicket)
	CExchangeMoney.ChangeTicketToMoney(Conn,nTicket)
end
function Gac2Gas:GetTotalTicket(Conn)
	CExchangeMoney.GetTotalTicket(Conn)
end
function Gac2Gas:GetCompensateItemInfo(Conn,ActivityID)
	CGasCompenate.GetCompensateItemInfo(Conn,ActivityID)
end
function Gac2Gas:GetActivationCodeCompenateItem(Conn)
	CGasCompenate.GetActivationCodeCompenateItem(Conn)
end
function Gac2Gas:TakeCompensateItem(Conn,ActivityID,ItemDisName,ItemNum)
	CGasCompenate.TakeCompensateItem(Conn,ActivityID,ItemDisName,ItemNum)
end
function Gac2Gas:TakeActivationCodeCompensate(Conn,CompenateID)
	CGasCompenate.TakeActivationCodeCompensate(Conn,CompenateID)
end
------------------------------生活技能相关---------------------------------------------
function Gac2Gas:ProduceByProductName(Conn,sSkillName, sDirectType, sProductName, nNum)
	CDirectionsMake.ProduceByProductName(Conn,sSkillName, sDirectType, sProductName, nNum)
end
function Gac2Gas:ProduceByProductNameAll(Conn,sSkillName, sDirectType, sProductName)
	CDirectionsMake.ProduceByProductNameAll(Conn,sSkillName, sDirectType, sProductName)
end
function Gac2Gas:LiveSkillBreakProducing(Conn,sSkillName)
	CDirectionsMake.LiveSkillBreakProducing(Conn,sSkillName)
end
function Gac2Gas:GetProductsCanMake(Conn,sSkillName)
	CDirectionsMake.GetProductsCanMake(Conn,sSkillName)
end

function Gac2Gas:GetLiveSkillHasLearned(Conn)
	CLiveskillBase.GetLiveSkillHasLearned(Conn)
end
function Gac2Gas:GetPracticedInfo(Conn,nSkillType)
	CExpertAndPractice.GetPracticedInfo(Conn,nSkillType)
end
function Gac2Gas:GetPracticedInfoForNpcPanel(Conn,nSkillType)
	CExpertAndPractice.GetPracticedInfoForNpcPanel(Conn,nSkillType)
end
function Gac2Gas:LearnLiveSkill(Conn, skillName)
	CLiveskillBase.LearnLiveSkill(Conn, skillName)
end
function Gac2Gas:LearnLiveSkillExpert(Conn,sSkillName,sEquipType)
	CExpertAndPractice.LearnLiveSkillExpert(Conn,sSkillName,sEquipType)
end
function Gac2Gas:GetLiveSkillExpert(Conn,sSkillName)
	CExpertAndPractice.GetLiveSkillExpert(Conn,sSkillName)
end

function Gac2Gas:WashLiveSkill(Conn,sSkillName)
	CLiveskillBase.WashLiveSkill(Conn,sSkillName)
end

function Gac2Gas:ForgetLiveSkillExpert(Conn)
	CExpertAndPractice.ForgetLiveSkillExpert(Conn)
end

function Gac2Gas:GetLiveSkillProdRemainCDTime(Conn,sSkillName,DirectType,DirectionName)
	 CDirectionsMake.GetLiveSkillProdRemainCDTime(Conn,sSkillName,DirectType,DirectionName)
end

--------------------------------------队伍相关---------------------------------
function Gac2Gas:InviteMakeTeam(Conn, uTargetID)
	CGasTeam.InviteMakeTeam(Conn, uTargetID)
end
function Gac2Gas:InviteMakeTeamByName(Conn, uTargetName)
	CGasTeam.InviteMakeTeamByName(Conn, uTargetName)
end
function Gac2Gas:RespondInvite(Conn, InviterID, InviterName, bAccept)
	CGasTeam.RespondInvite(Conn, InviterID, InviterName, bAccept)
end
function Gac2Gas:RespondApp(Conn, InviterID, InviterName, bAccept)
	CGasTeam.RespondApp(Conn, InviterID, InviterName, bAccept)
end
function Gac2Gas:BreakTeam(Conn)
	CGasTeam.BreakTeam(Conn)
end
function Gac2Gas:LeaveTeam(Conn)
	CGasTeam.LeaveTeam(Conn)
end
function Gac2Gas:RemoveTeamMember(Conn, LeaverID)
	CGasTeam.RemoveTeamMember(Conn, LeaverID)
end
function Gac2Gas:SetCaptain(Conn, uTargetID)
	CGasTeam.SetCaptain(Conn, uTargetID)
end
function Gac2Gas:SetAssignMode(Conn, eAM)
	CGasTeam.SetAssignMode(Conn, eAM)
end
function Gac2Gas:GetTeamMembers(Conn)
	CGasTeam.GetTeamMembersByConn(Conn)
end
function Gac2Gas:SetAuctionStandard(Conn, AuctionStandard)
	CGasTeam.SetAuctionStandard(Conn, AuctionStandard)
end
function Gac2Gas:SetAuctionBasePrice(Conn, AuctionBasePrice)
	CGasTeam.SetAuctionBasePrice(Conn, AuctionBasePrice)
end
function Gac2Gas:UpdateTeamMark(Conn, uMarkType, uTargetType, uTargetID)
	CTeamMark.UpdateTeamMark(Conn, uMarkType, uTargetType, uTargetID)
end
function Gac2Gas:getAppTeamList(Conn,nType)
	CTeamAppl.getAppTeamList(Conn,nType)
end
function Gac2Gas:leaveList(Conn,nType)
	CTeamAppl.leaveList(Conn,nType)
end
function Gac2Gas:joinTeamList(Conn,nType,sMsg)
	CTeamAppl.joinTeamList(Conn,nType,sMsg)
end
function Gac2Gas:upFreshTeamList(Conn,nType)
	CTeamAppl.upFreshTeamList(Conn,nType)
end
------------------------------------佣兵团---------------------------------------
function Gac2Gas:PreChangeTongName(Conn)
	CTongBasic.PreChangeTongName(Conn)
end
function Gac2Gas:ChangeTongName(Conn, name)
	CTongBasic.ChangeTongName(Conn, name)
end
function Gac2Gas:RequestCreateTong(Conn)
	CTongBasic.RequestCreateTong(Conn)
end
function Gac2Gas:CreateTong(Conn, name,purpose)
	CTongBasic.CreateTong(Conn, name,purpose)
end
function Gac2Gas:GetTongInfo(Conn)
	CTongBasic.GetTongInfo(Conn)
end
function Gac2Gas:GetTongRequestInfo(Conn)
	CTongBasic.GetTongRequestInfo(Conn)
end
function Gac2Gas:GetTongMemberInfo(Conn)
	CTongBasic.GetTongMemberInfo(Conn)
end
function Gac2Gas:GetTongLog(Conn, type)
	CTongBasic.GetTongLog(Conn, type)
end
function Gac2Gas:ChangeTongPurpose(Conn, sTongPorpose)
	CTongBasic.ChangeTongPurpose(Conn, sTongPorpose)
end
function Gac2Gas:ChangePos(Conn, target_id, old_position, new_position)
	CTongBasic.ChangePos(Conn, target_id, old_position, new_position)
end
function Gac2Gas:TongResign(Conn)
	CTongBasic.TongResign(Conn)
end
function Gac2Gas:KickOutOfTong(Conn, target_id)
	CTongBasic.KickOutOfTong(Conn, target_id)
end
function Gac2Gas:LeaveTong(Conn)
	CTongBasic.LeaveTong(Conn)
end
function Gac2Gas:BeJoinTong(Conn, target_id, bBlooen)
	CTongBasic.BeJoinTong(Conn, target_id, bBlooen)
end
function Gac2Gas:ChangeTongType(Conn, TongType)
	CTongBasic.ChangeTongType(Conn, TongType)
end
function Gac2Gas:UpTongType(Conn,uUpToType)
	CTongBasic.UpTongType(Conn,uUpToType)
end
function Gac2Gas:ResetTongType(Conn)
	CTongBasic.ResetTongType(Conn)
end
function Gac2Gas:GetAllTongSomeInfo(Conn)
	CTongBasic.GetAllTongSomeInfo(Conn)
end
function Gac2Gas:SearchTong(Conn,sName)
	CTongBasic.SearchTong(Conn,sName)
end
function Gac2Gas:GetAllPlayerSomeInfo(Conn)
	CTongBasic.GetAllPlayerSomeInfo(Conn)
end
function Gac2Gas:SearchPlayer(Conn,sName)
	CTongBasic.SearchPlayer(Conn,sName)
end
function Gac2Gas:RequestJoinTong(Conn,tong_id)
	CTongBasic.RequestJoinTong(Conn,tong_id)
end
function Gac2Gas:RequestJoinTongByName(Conn,target_name)
	CTongBasic.RequestJoinTongByName(Conn,target_name)
end
function Gac2Gas:RecommendJoinTong(Conn, target_name, sInfor)
	CTongBasic.RecommendJoinTong(Conn, target_name, sInfor)
end
function Gac2Gas:InviteJoinTong(Conn,target_id)
	CTongBasic.InviteJoinTong(Conn,target_id)
end
function Gac2Gas:ResponseBeRecommendJoinTong(Conn, inviterId, bFlag)
	CTongBasic.ResponseBeRecommendJoinTong(Conn, inviterId, bFlag)
end

function Gac2Gas:InviteJoinTongByName(Conn, target_name)
	CTongBasic.InviteJoinTongByName(Conn, target_name)
end
function Gac2Gas:ResponseBeInviteToTong(Conn, inviter_id,bBlooean)
	CTongBasic.ResponseBeInviteToTong(Conn, inviter_id,bBlooean)
end
function Gac2Gas:GetTongLevel(Conn)
	CTongBasic.GetTongLevel(Conn)
end
function Gac2Gas:GetTongProfferCount(Conn)
	CTongBasic.GetTongProfferCount(Conn)
end
function Gac2Gas:ContributeMoney(Conn,nNum)
	CTongContriMoney.ContributeMoney(Conn,nNum)
end
function Gac2Gas:GetMoneyCanContribute(Conn)
	CTongContriMoney.GetMoneyCanContribute(Conn)
end
function Gac2Gas:GetCollectiveBagOnePageItemList(Conn, nType,nPage)
	CTongDepot.GetCollectiveBagOnePageItemList(Conn, nType,nPage)
end
function Gac2Gas:CollectiveBagMoveItemSelf(Conn, nType, nFromPage, nFromPos, nToPage, nToPos)
	CTongDepot.CollectiveBagMoveItemSelf(Conn, nType, nFromPage, nFromPos, nToPage, nToPos)
end
function Gac2Gas:BagAddItemFromCollectiveBag(Conn,nFromRoom, nFromPos,nFromPage,nToRoom,nToPos)
	CTongDepot.BagAddItemFromCollectiveBag(Conn,nFromRoom, nFromPos,nFromPage,nToRoom,nToPos)
end
function Gac2Gas:QuikMoveItemFromCDepotToBag(Conn,nFromRoom, nFromPos,nFromPage,nToStoreRoom)
	CTongDepot.QuikMoveItemFromCDepotToBag(Conn,nFromRoom, nFromPos,nFromPage,nToStoreRoom)
end
function Gac2Gas:CollectiveBagAddItemFromOther(Conn, nFromRoom, nFromPos, nToRoom, nToPage, nToPos)
	CTongDepot.CollectiveBagAddItemFromOther(Conn, nFromRoom, nFromPos, nToRoom, nToPage, nToPos)
end
function Gac2Gas:CreateTongProdItem(Conn, sItemName,nItemType,nDepot)
	CTongItemCreate.CreateTongProdItem(Conn, sItemName,nItemType,nDepot)
end
function Gac2Gas:GetCreatingProdItemInfo(Conn)
	CTongItemCreate.GetCreatingProdItemInfo(Conn)
end
function Gac2Gas:GetTongRelativeLine(Conn)
	CTongItemCreate.GetTongRelativeLine(Conn)
end
function Gac2Gas:CancelCreateProdItem(Conn,nOrderID)
	CTongItemCreate.CancelCreateProdItem(Conn,nOrderID)
end
function Gac2Gas:GetTongFightScienceInfo(Conn)
	CTongTech.GetTongFightScienceInfo(Conn)
end

function Gac2Gas:LearnTongTech(Conn, techName)
    CTongTech.LearnTongTech(Conn, techName)
end

function Gac2Gas:UpdateTongScience(Conn, TechName,TechType)
	CTongTech.UpdateTongScience(Conn, TechName,TechType)
end
function Gac2Gas:CancelUpdateScience(Conn, name,Type)
	CTongTech.CancelUpdateScience(Conn, name,Type)
end
function Gac2Gas:SendChallenge(Conn, nWarZoneId, nWarZoneStationId, nMoney)
	CTongWar.SendChallenge(Conn, nWarZoneId, nWarZoneStationId, nMoney)
end
function Gac2Gas:GetTongChallengeInfoList(Conn)
	CTongWar.GetTongChallengeInfoList(Conn)
end
function Gac2Gas:GetTongBattleListInfo(Conn)
	CTongWar.GetTongBattleListInfo(Conn)
end
function Gac2Gas:GetTongBuildingList(Conn)
	CTongBuilding.GetTongBuildingList(Conn)
end
function Gac2Gas:RemoveBuilding(Conn, NpcID)
	CTongBuilding.RemoveBuilding(Conn, NpcID)
end
function Gac2Gas:CreateBuildingItem(Conn, sBuidingName,nDepot)
	CTongBuilding.CreateBuildingItem(Conn, sBuidingName,nDepot)
end
function Gac2Gas:GetTongLevelToFilterAvailableBuildingItems(Conn)
	CTongBuilding.GetTongLevelToFilterAvailableBuildingItems(Conn)
end
function Gac2Gas:CancelCreateBuildingItem(Conn,nBuildID)
	CTongBuilding.CancelCreateBuildingItem(Conn,nBuildID)
end
function Gac2Gas:GetCreatingBuildingItemInfo(Conn)
	CTongBuilding.GetCreatingBuildingItemInfo(Conn)
end
function Gac2Gas:UpdateBuildingLevel(Conn, NpcID)
	CTongBuilding.UpdateBuildingLevel(Conn, NpcID)
end

function Gac2Gas:ContributeMoneyChangeExp(Conn, nMoney)
	CTongContriMoney.ContributeMoneyChangeExp(Conn, nMoney)
end
function Gac2Gas:ContributeMoneyChangeSoul(Conn, nMoney)
	CTongContriMoney.ContributeMoneyChangeSoul(Conn, nMoney)
end
function Gac2Gas:ContributeMoneyChangeResource(Conn, nMoney)
	CTongContriMoney.ContributeMoneyChangeResource(Conn, nMoney)
end

function Gac2Gas:GetTongContributeMoneyInfo(Conn, eIndex)
	CTongContriMoney.GetTongContributeMoneyInfo(Conn, eIndex)
end

-----------------------------------邮件-----------------------------
function Gac2Gas:GetMail(Conn, MailId)
		MailMgr.GetMail(Conn, MailId)
end
function Gac2Gas:GetMailList(Conn)
	MailMgr.GetMailList(Conn)
end
function Gac2Gas:SendMail(Conn, sRecieverName, sMailTitle, sMailContent, uMoney,nMoneyType)
	MailMgr.SendMail(Conn, sRecieverName, sMailTitle, sMailContent, uMoney,nMoneyType)
end

function Gac2Gas:GetSendMoneyCess(Conn, SendMoney)
	MailMgr.GetSendMoneyCess(Conn, SendMoney)
end

function Gac2Gas:SendMailGoods(Conn,goods_id,mail_grid)
	MailMgr.SendMailGoods(Conn,goods_id,mail_grid)
end
function Gac2Gas:SendMailGoodsEnd(Conn)
	MailMgr.SendMailGoodsEnd(Conn)
end
function Gac2Gas:TakeAttackmentByIndex(Conn, uMailId, SlotIndex)
	MailMgr.TakeAttackmentByIndex(Conn, uMailId, SlotIndex)
end
function Gac2Gas:TakeAttachment(Conn, uMailId)
	MailMgr.TakeAttachment(Conn, uMailId)
end

function Gac2Gas:TakeContextAtachment(Conn,mail_id)
	MailMgr.TakeContextAtachment(Conn,mail_id)
end
function Gac2Gas:DeleteMail(Conn, MailId)
	MailMgr.DeleteMail(Conn, MailId)
end
function Gac2Gas:GetMailMoney(Conn,MailId)
	MailMgr.GetMailMoney(Conn,MailId)
end
function Gac2Gas:DeleteBatchMailBegin(Conn)
	MailMgr.DeleteBatchMailBegin(Conn)
end
function Gac2Gas:DeleteBatchMail(Conn, MailID)
	MailMgr.DeleteBatchMail(Conn, MailID)
end
function Gac2Gas:DeleteBatchMailEnd(Conn)
	MailMgr.DeleteBatchMailEnd(Conn)
end
function Gac2Gas:CheckMailTextAttachment(Conn, nRoomIndex, nPos)
	MailMgr.CheckMailTextAttachment(Conn, nRoomIndex, nPos)
end

function Gac2Gas:SendUserAdviceInfo(Conn,questionText,questionTypeId, typeOptionId)
	UserAdvice.SendUserAdviceInfo(Conn,questionText,questionTypeId, typeOptionId)
end

function Gac2Gas:QueryQuestionList(Conn)
	UserAdvice.QueryQuestionList(Conn)
end


---------------------------------物品-----------------------------
--物品相关
function Gac2Gas:DelWithTimeOutItem(Connection, nItemID,nItemType,sItemName)
	CRoomRpcMgr.DelWithTimeOutItem(Connection, nItemID,nItemType,sItemName)
end
function Gac2Gas:SplitItem( Connection, nARoom,nAPos,nBRoom,nBPos,nCount)
	CRoomRpcMgr.SplitItem( Connection, nARoom,nAPos,nBRoom,nBPos,nCount)
end
function Gac2Gas:MoveItem( Connection, nARoom, nAPos, nBRoom, nBPos)
	CRoomRpcMgr.MoveItem( Connection, nARoom, nAPos, nBRoom, nBPos)
end
function Gac2Gas:PlaceBag(Connection,nRoom,nPos,nSlot)
	CRoomRpcMgr.PlaceBag(Connection,nRoom,nPos,nSlot)
end
function Gac2Gas:FetchBag(Connection,nSlot,nRoom,nPos)
	CRoomRpcMgr.FetchBag(Connection,nSlot,nRoom,nPos)
end
function Gac2Gas:DelBag(Connection,nSlot)
	CRoomRpcMgr.DelBag(Connection,nSlot)
end
function Gac2Gas:Change2Bag(Connection,nASlot,nBSlot)
	CRoomRpcMgr.Change2Bag(Connection,nASlot,nBSlot)
end
function Gac2Gas:QuickMove(Connection,nARoom,nAPos,nStoreMain)
	CRoomRpcMgr.QuickMove(Connection,nARoom,nAPos,nStoreMain)
end
function Gac2Gas:CleanBag(Connection,nStoreMain)
	CRoomRpcMgr.CleanBag(Connection,nStoreMain)
end
function Gac2Gas:OnUseDelItemByPos(Connection, nRoomIndex,nPos,nCount)
	CRoomRpcMgr.OnUseDelItemByPos(Connection, nRoomIndex,nPos,nCount)
end
function Gac2Gas:OnLButtonUpDelItemByPos(Connection, nRoomIndex,nPos,nCount)
	CRoomRpcMgr.OnLButtonUpDelItemByPos(Connection, nRoomIndex,nPos,nCount)
end
function Gac2Gas:BreakItem(Connection,nRoomIndex,nPos)
	CBreakItem.BreakItem(Connection,nRoomIndex,nPos)
end
function Gac2Gas:GetDepotInfo( Connection)
	CDepotMgr.GetDepotInfo( Connection)
end
function Gac2Gas:GetMoneyFromDepot( Connection,nMoney)
	CDepotMgr.GetMoneyFromDepot( Connection,nMoney)
end
function Gac2Gas:SaveMoneyToDepot( Connection,nMoney)
	CDepotMgr.SaveMoneyToDepot( Connection,nMoney)
end
-----------------------------------------道具商城--------------------------
function Gac2Gas:BuyItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType)
		CGasToolsMall.BuyItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType)
end
function Gac2Gas:GiveItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType,sGeterName,leftMsg)
	CGasToolsMall.GiveItemInToolsMall(Conn,nItemType,sItemName,nCount,nPayType,sGeterName,leftMsg)
end
function Gac2Gas:GetHotSaleItemInfo(Conn, type)
    CGasToolsMall.GetHotSaleItemInfo(Conn, type)
end
function Gac2Gas:TakeYongBingBi(Conn)
		CGasToolsMall.TakeYongBingBi(Conn)
end
function Gac2Gas:RequestTakeYongBingBi(Conn,nYuanbao)
		CGasToolsMall.RequestTakeYongBingBi(Conn,nYuanbao)
end

------------------------------------------系统设置---------------------------
function Gac2Gas:SysSettingBegain(Conn)
	CGasSyssetting.SysSettingBegain(Conn)
end

function Gac2Gas:SysSettingEnd(Conn)
	CGasSyssetting.SysSettingEnd(Conn)
end

function Gac2Gas:GameSetting(Conn,team_setting,trans_setting,group_setting,friend_setting,troop_setting,autoMakeTeam_setting,selectQuestSort_setting)
	CGameSetting.GameSetting(Conn,team_setting,trans_setting,group_setting,friend_setting,troop_setting,autoMakeTeam_setting,selectQuestSort_setting)
end

function Gac2Gas:GetGameSettingInfo(Conn)
	CGameSetting.GetGameSettingInfo(Conn)
end

--设置按键映射
function Gac2Gas:SetKeyMap(Conn, sFuncName, sKey, uPosition, Setstyle) --Setstyle 为true时update,为false时delete 此映射恢复到默认,从静态文件读取
	CKeyMapMgr.SetKeyMap(Conn, sFuncName, sKey, uPosition, Setstyle)
end

--获取所有按键映射
function Gac2Gas:GetAllKeyMaps(Conn)
	CKeyMapMgr.GetAllKeyMaps(Conn)
end

function Gac2Gas:ChangeMouseCtrl(Conn, lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
	CKeyMapMgr.ChangeMouseCtrl(Conn, lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
end
function Gac2Gas:ChangeMouseCtrlBeforeGame(Conn, charID, type, bShowMsgBox)
	CKeyMapMgr.ChangeMouseCtrlBeforeGame(Conn, charID, type, bShowMsgBox)
end

function Gac2Gas:SaveUISetting(Conn,uPlayerNameShow,uPlayerHonourShow,uPlayerTongNameShow,uPlayerArmyCorpsNameShow,uOtherNameShow,uOtherHonourShow,uOtherTongNameShow,uOtherArmyCorpsNameShow,uNpcNameShow,uMonsterNameShow,uHeadPieceShow,uLockShortcut,uMonsterNameShowMO,uCanNotSelectFriend)
	CUISetting.SaveUISetting(Conn,uPlayerNameShow,uPlayerHonourShow,uPlayerTongNameShow,uPlayerArmyCorpsNameShow,uOtherNameShow,uOtherHonourShow,uOtherTongNameShow,uOtherArmyCorpsNameShow,uNpcNameShow,uMonsterNameShow,uHeadPieceShow,uLockShortcut,uMonsterNameShowMO,uCanNotSelectFriend)
end

---------------------------------------------------------------------------
--使用物品
function Gac2Gas:ClickItem(Conn, BigID, ItemName, RoomIndex, Pos)
	g_TriggerMgr:ItemTrigger(Conn, BigID, ItemName, RoomIndex, Pos)
end

--花卉
function Gac2Gas:FlowerPerishGiveItem(Conn, IsPerish)
	CFlower.FlowerPerishGiveItem(Conn, IsPerish)
end

function Gac2Gas:CareForFlower(Conn, param)
	CFlower.CareForFlower(Conn, param)
end

function Gac2Gas:FlowerStateClean(Conn)
	CFlower.FlowerStateClean(Conn)
end

---------------------------------------------------------------------------------------
--护甲片镶嵌
function Gac2Gas:ArmorPieceEnactment(Conn,nBigID,nIndex,armorPieceRoomIndex,armorPiecePos,equipRoomIndex,equipPos,itemid,update_index,itemType,ArmorPieceName)
	CGasArmorPieceEnactment.ArmorPieceEnactment(Conn,nBigID,nIndex,armorPieceRoomIndex,armorPiecePos,equipRoomIndex,equipPos,itemid,update_index,itemType,ArmorPieceName)
end

--武器鉴定
function Gac2Gas:WeaponIdentify(Conn,curLevel,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,itemid,equipPart)
	CGasEquipIdentify.WeaponIdentify(Conn,curLevel,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,itemid,equipPart)
end

--防具或者饰品鉴定
function Gac2Gas:ArmorOrJewelryIdentify(Conn,armorBaseValue,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,equipitemid)
	CGasEquipIdentify.ArmorOrJewelryIdentify(Conn,armorBaseValue,nBigID,nIndex,identify_equipTrend,identify_equipTrendValue,scrollRoomIndex,scrollPos,equipRoomIndex,equipPos,equipitemid)
end

--装备炼化
function Gac2Gas:EquipSmeltSoul(Conn,nBigID,nIndex,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,equipRoomIndex,equipPos,itemid,itemType,SmeltSoulScrollName,fromPlaceType,eEquipPart)
	CGasEquipSmeltSoulScroll.EquipSmeltSoul(Conn,nBigID,nIndex,SmeltSoulScrollRoomIndex,SmeltSoulScrollPos,equipRoomIndex,equipPos,itemid,itemType,SmeltSoulScrollName,fromPlaceType,eEquipPart)
end

--保存称谓
function Gac2Gas:SavePlayerAppellation(Conn,appellationId1,appellationId2)
	CGasAppellationAndMoral.SavePlayerAppellation(Conn,appellationId1,appellationId2)
end

--查看排行榜
function Gac2Gas:GetSortList(Conn,sType,sName,campType)
	CSortMgr.GetSortList(Conn,sType,sName,campType)
end

------------------------------------------聊天频道---------------------------------------
--频道聊天
function Gac2Gas:TalkToChannel(Conn, Msg, Channel_id)
	CChatChannelMgr.TalkToChannel(Conn, Msg, Channel_id)
end 

--聊天页移动
function Gac2Gas:MovePosB2PosA( Conn, nPosA, nPosB)
	CChatChannelMgr.MovePosB2PosA( Conn, nPosA, nPosB)
end

--定制聊天频道信息
function Gac2Gas:SetChannelPanel(Conn, Channel_ID, Action, Panel_Position)
	CChatChannelMgr.SetChannelPanel(Conn, Channel_ID, Action, Panel_Position)
end

--定制结束
function Gac2Gas:SetChannelPanelEnd(Conn)
	CChatChannelMgr.SetChannelPanelEnd(Conn)
end 	

--恢复聊天页的默认设置
function Gac2Gas:SetDefaultChannelPanel(Conn)
	CChatChannelMgr.SetDefaultChannelPanel(Conn)
end

--重命名聊天页名字
function Gac2Gas:RenamePanel(Conn, Panel_Name, Panel_Pos)
	CChatChannelMgr.RenamePanel(Conn, Panel_Name, Panel_Pos)
end

--删除聊天页面板
function Gac2Gas:DeletePanel(Conn, Panel_Name, Panel_Pos)
	CChatChannelMgr.DeletePanel(Conn, Panel_Name, Panel_Pos)
end

--添加新的聊天页
function Gac2Gas:AddPanel(Conn, Panel_Name, Panel_Pos)
	CChatChannelMgr.AddPanel(Conn, Panel_Name, Panel_Pos)
end

--把面板信息写入数据库
function Gac2Gas:SetPanel(Conn, Panel_Name, Panel_Pos)
	CChatChannelMgr.SetPanel(Conn, Panel_Name, Panel_Pos)
end

function Gac2Gas:SaveNotShowMsgWnd(Conn,sFirstTimeName)
	CChatChannelMgr.SaveNotShowMsgWnd(Conn,sFirstTimeName)
end
-----------------------------------社群系统-----------------------------------------

function Gac2Gas:UpdateFriendInfo(Conn, nFriendId)
	CGasFriendBasic.UpdateFriendInfo(Conn, nFriendId)
end

function Gac2Gas:UpdateOnlineFriendInfo(Conn)
	CGasFriendBasic.UpdateOnlineFriendInfo(Conn)
end

function Gac2Gas:AddFriendClass(Conn,gf_sName)
	CGasFriendBasic.AddFriendClass(Conn,gf_sName)
end

function Gac2Gas:SearchPlayerAccurately(Conn,f_name,f_id)
	CGasFriendBasic.SearchPlayerAccurately(Conn,f_name,f_id)
end

function Gac2Gas:SearchPlayerCondition(Conn,class,gender,low_level,up_level,sSceneName)
	CGasFriendBasic.SearchPlayerCondition(Conn,class,gender,low_level,up_level,sSceneName)
end

function Gac2Gas:AddFriendToClass(Conn,playerId,fc_id)
	CGasFriendBasic.AddFriendToClass(Conn,playerId,fc_id)
end

function Gac2Gas:RespondAddFriend(Conn, InvitorId,InvitorClassId,InviteeClassId)
	CGasFriendBasic.RespondAddFriend(Conn, InvitorId,InvitorClassId,InviteeClassId)
end

function Gac2Gas:DeleteMyFriend(Conn, Player_ID)
	CGasFriendBasic.DeleteMyFriend(Conn, Player_ID)
end

function Gac2Gas:ChangeToClass(Conn, player_id, newclass_id)
	CGasFriendBasic.ChangeToClass(Conn, player_id, newclass_id)
end

function Gac2Gas:PrivateChatSendMsg(Conn,object_id,content)
	CGasFriendBasic.PrivateChatSendMsg(Conn,object_id,content)
end

function Gac2Gas:MoveToBlackList(Conn, player_id)
	CGasFriendBasic.MoveToBlackList(Conn, player_id)
end

function Gac2Gas:AddBlackListByName(Conn, player_name)
	CGasFriendBasic.AddBlackListByName(Conn, player_name)
end

function Gac2Gas:DeleteBlackListMember(Conn, player_id)
	CGasFriendBasic.DeleteBlackListMember(Conn, player_id)
end

function Gac2Gas:RenameFriendClass(Conn,friend_classId,new_className,old_className)
	CGasFriendBasic.RenameFriendClass(Conn,friend_classId,new_className,old_className)
end

function Gac2Gas:DeleteFriendClass(Conn, class_id)
	CGasFriendBasic.DeleteFriendClass(Conn, class_id)
end

function Gac2Gas:GetMemberIdbyNameForPrivateChat(Conn, memberName, str)
	CGasFriendBasic.GetMemberIdbyNameForPrivateChat(Conn, memberName, str)
end

function Gac2Gas:AddFriendToClassByName(Conn,playerName,fc_id)
	CGasFriendBasic.AddFriendToClassByName(Conn,playerName,fc_id)
end

function Gac2Gas:RefuseBeAddFriend(Conn,inviterId)
	CGasFriendBasic.RefuseBeAddFriend(Conn,inviterId)
end

function Gac2Gas:ChangeShowSentence(Conn, showSen)
	CGasFriendBasic.ChangeShowSentence(Conn, showSen)
end

function Gac2Gas:SaveAssociationPersonalInfo(Conn,fellState,fellStatement,hobby,bodyShape,personality,makeFriendState,style,detail)
	CGasFriendBasic.SaveAssociationPersonalInfo(Conn,fellState,fellStatement,hobby,bodyShape,personality,makeFriendState,style,detail)
end

function Gac2Gas:GetAssociationPersonalInfo(Conn)
	CGasFriendBasic.GetAssociationPersonalInfo(Conn)
end

function Gac2Gas:GetAssociationMemberInfo(Conn,playerId)
	CGasFriendBasic.GetAssociationMemberInfo(Conn,playerId)
end

function Gac2Gas:TongChatSendMsg(Conn,text)
	CGasAssoTongBasic.TongChatSendMsg(Conn,text)
end

function Gac2Gas:CreateFriendGroup(Conn,groupname,groupkind,keyword,groupannoun)
	CGasFriendGroupBasic.CreateFriendGroup(Conn,groupname,groupkind,keyword,groupannoun)
end

function Gac2Gas:SetCtrlType(Conn,Member_Id,Group_Id,ctrlType)
	CGasFriendGroupBasic.SetCtrlType(Conn,Member_Id,Group_Id,ctrlType)
end

function Gac2Gas:SearchGroupAccurately(Conn,friendGroupName,friendGroupId)
	CGasFriendGroupBasic.SearchGroupAccurately(Conn,friendGroupName,friendGroupId)
end

function Gac2Gas:SearchGroupCondition(Conn,keyword,groupkind)
	CGasFriendGroupBasic.SearchGroupCondition(Conn,keyword,groupkind)
end

function Gac2Gas:RequestJoinIntoFriendGroup(Conn,groupId)
	CGasFriendGroupBasic.RequestJoinIntoFriendGroup(Conn,groupId)
end

function Gac2Gas:ResponseRequestJoinGroup(Conn, InvitorId,groupId,index)
	CGasFriendGroupBasic.ResponseRequestJoinGroup(Conn, InvitorId,groupId,index)
end

function Gac2Gas:PublicChatSendMsg(Conn,group_id,text)
	CGasFriendGroupBasic.PublicChatSendMsg(Conn,group_id,text)
end

function Gac2Gas:KickOutOfGroup(Conn,Member_Id,Group_Id)
	CGasFriendGroupBasic.KickOutOfGroup(Conn,Member_Id,Group_Id)
end

function Gac2Gas:InviteToGroup(Conn,inviteeId, groupId)
	CGasFriendGroupBasic.InviteToGroup(Conn,inviteeId, groupId)
end

function Gac2Gas:LeaveGroup(Conn,Group_Id)
	CGasFriendGroupBasic.LeaveGroup(Conn,Group_Id)
end

function Gac2Gas:ResponseInviteToGroup(Conn,Group_Id,inviterId,index)
	CGasFriendGroupBasic.ResponseInviteToGroup(Conn,Group_Id,inviterId,index)
end

function Gac2Gas:DisbandGroup(Conn,Group_Id)
	CGasFriendGroupBasic.DisbandGroup(Conn,Group_Id)
end

function Gac2Gas:ChangeGroupDeclare(Conn, groupid, groupannoun)
	CGasFriendGroupBasic.ChangeGroupDeclare(Conn, groupid, groupannoun)
end

function Gac2Gas:RefuseGroupMsgOrNot(Conn,groupId,uRefuseOrNot)
	CGasFriendGroupBasic.RefuseGroupMsgOrNot(Conn,groupId,uRefuseOrNot)
end

--ClickNpc
function Gac2Gas:ClickNpc(Conn, NpcId)
	CTriggerMgr.ClickNpc(Conn, NpcId)
end

function Gac2Gas:AgreedJoinDareFb(Conn, SceneName, QuestName)
	CDareQuestFb.AgreedJoinDareFb(Conn, SceneName, QuestName)
end

function Gac2Gas:ShowYbEducateActionWnd(Conn, NpcID)
	CMercenaryEducateAct.ShowYbEducateActionWnd(Conn, NpcID)
end

function Gac2Gas:JoinYbEducateAction(Conn, SceneName)
	CMercenaryEducateAct.JoinYbEducateAction(Conn, SceneName)
end

function Gac2Gas:BeginYbEducateAction(Conn)
	CMercenaryRoomCreate.BeginYbEducateAction(Conn)
end

--龙穴活动
function Gac2Gas:ShowDragonCaveWnd(Conn, NpcID)
  CDragonCave:ShowDragonCaveWnd(Conn, NpcID)
end

function Gac2Gas:EnterDragonCave(Conn, NpcName)
	CDragonCave:EnterDragonCave(Conn, NpcName)
end

function Gac2Gas:IntoDragonCave(Conn, SceneName)
	CDragonCave:IntoDragonCave(Conn, SceneName)
end

function Gac2Gas:LeaveDragonCave(Conn)
	CDragonCave:LeaveDragonCave(Conn)
end

-- 地下城
function Gac2Gas:ShowAreaFbSelWnd(Conn, NpcID)
	CAreaFbServer.ShowAreaFbSelWnd(Conn, NpcID)
end

function Gac2Gas:EnterGeneralAreaFb(Conn, NpcID, SceneName)
	CAreaFbServer.EnterGeneralAreaFb(Conn, NpcID, SceneName)
end

function Gac2Gas:AgreedJoinAreaFb(Conn,SceneName, SceneID, ServerID)
	CAreaFbServer.AgreedJoinAreaFb(Conn,SceneName, SceneID, ServerID)
end

function Gac2Gas:DecideJoinAreaFb(Conn, SceneName, SceneId, ServerId)
	CAreaFbServer.DecideJoinAreaFb(Conn, SceneName, SceneId, ServerId)
end
---------------------------------玩法-----------------------------------
function Gac2Gas:JoinDaDuoBao(Conn, LevelType)
	CaptureTreasure.JoinDaDuoBao(Conn, LevelType)
end
function Gac2Gas:ShowDaDuoBaoWnd(Conn, NpcID)
	CaptureTreasure.ShowDaDuoBaoWnd(Conn, NpcID)
end

function Gac2Gas:ShowDrinkWnd(Conn, NpcID)
	Gas2Gac:RetShowDrinkWnd(Conn)
end

function Gac2Gas:DrinkShootGetReady(Conn)
	g_DrinkShootMgr:BeginGame(Conn)
	--Conn.m_Player.m_Scene.m_DrinkShootMgr:BeginGame(Conn)
end
function Gac2Gas:DrinkShootNextPlayer(Conn)
	g_DrinkShootMgr:NextPlayer(Conn)
	--Conn.m_Player.m_Scene.m_DrinkShootMgr:NextPlayer()
end
function Gac2Gas:DrinkShootDoAction(Conn, ActionName)
	g_DrinkShootMgr:DoAction(Conn, ActionName)
	--Conn.m_Player.m_Scene.m_DrinkShootMgr.DoAction(Conn, ActionName)
end
-------------------------------------------------------------------------

--副本中DPS信息积分面板
function Gac2Gas:BeginSendFbDpsInfo(Conn)
	CFbDpsServer.BeginSendFbDpsInfo(Conn)
end
function Gac2Gas:EndSendFbDpsInfo(Conn)
	CFbDpsServer.EndSendFbDpsInfo(Conn)
end
function Gac2Gas:GetBossDpsInfo(Conn)
	CFbDpsServer.GetBossDpsInfo(Conn)
end
function Gac2Gas:ChangeDpsShowType(Conn, ShowTypeName)
	CFbDpsServer.ChangeDpsShowType(Conn, ShowTypeName)
end

function Gac2Gas:SaveShortCut(Conn, pos, type, arg1, arg2, arg3)
	CGasShortcut.SaveShortCut(Conn, pos, type, arg1, arg2, arg3)
end

function Gac2Gas:SaveShortCutEnd(Conn)
	CGasShortcut.SaveShortCutEnd(Conn)
end

function Gac2Gas:SaveBattleArrayPos(Conn,b_sName,b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5,nRoomIndex,nPos)
	CGasBattleArrayBooks.SaveBattleArrayPos(Conn,b_sName,b_uPos1,b_uPos2,b_uPos3,b_uPos4,b_uPos5,nRoomIndex,nPos)
end

function Gac2Gas:SaveBattleArrayMemPos(Conn,b_uId,b_sName,ab_uLoc1,ab_uLoc2,ab_uLoc3,ab_uLoc4,ab_uLoc5)
	CGasBattleArrayBooks.SaveBattleArrayMemPos(Conn,b_uId,b_sName,ab_uLoc1,ab_uLoc2,ab_uLoc3,ab_uLoc4,ab_uLoc5)
end

function Gac2Gas:UseBattleArraySkill(Conn,b_sName)
	CGasBattleArrayBooks.UseBattleArraySkill(Conn,b_sName)
end

function Gac2Gas:RevertToBattleArrayBook(Conn,b_uId,b_sName)
	CGasBattleArrayBooks.RevertToBattleArrayBook(Conn,b_uId,b_sName)
end

function Gac2Gas:SaveBattleArrayShortCutIndex(Conn,b_name,b_pos)
	CGasBattleArrayBooks.SaveBattleArrayShortCutIndex(Conn,b_name,b_pos)
end

--佣兵团
function Gac2Gas:RequestCreateArmyCorps(Conn)
	CArmyCorpsBase.RequestCreateArmyCorps(Conn)
end

function Gac2Gas:CreateArmyCorps(Conn, name,purpose)
	CArmyCorpsBase.CreateArmyCorps(Conn, name,purpose)
end

function Gac2Gas:GetArmyCorpsInfo(Conn)
	CArmyCorpsBase.GetArmyCorpsInfo(Conn)
end

function Gac2Gas:ChangeArmyCorpsPurpose(Conn, sPurpose)
	CArmyCorpsBase.ChangeArmyCorpsPurpose(Conn, sPurpose)
end

function Gac2Gas:PreChangeArmyCorpsName(Conn)
	CArmyCorpsBase.PreChangeArmyCorpsName(Conn)
end

function Gac2Gas:ChangeArmyCorpsName(Conn,sName)
	CArmyCorpsBase.ChangeArmyCorpsName(Conn,sName)
end

function Gac2Gas:InviteJoinArmyCorps(Conn, uTargetId)
	CArmyCorpsBase.InviteJoinArmyCorps(Conn, uTargetId)
end

function Gac2Gas:InviteJoinArmyCorpsByName(Conn, sTargetName)
	CArmyCorpsBase.InviteJoinArmyCorpsByName(Conn, sTargetName)
end

function Gac2Gas:ResponseBeInviteToArmyCorps(Conn, InviteID, bResponse)
	CArmyCorpsBase.ResponseBeInviteToArmyCorps(Conn, InviteID, bResponse)
end

function Gac2Gas:GetArmyCorpsTeamInfo(Conn)
	CArmyCorpsBase.GetArmyCorpsTeamInfo(Conn)
end

function Gac2Gas:LeaveArmyCorps(Conn)
	CArmyCorpsBase.LeaveArmyCorps(Conn)
end

function Gac2Gas:KickOutOfArmyCorps(Conn, tongId)
	CArmyCorpsBase.KickOutOfArmyCorps(Conn, tongId)
end

function Gac2Gas:GetTargetTongMemberInfo(Conn, tongId)
	CTongBasic.GetTargetTongMemberInfo(Conn, tongId)
end

function Gac2Gas:ChangeArmyCorpsPos(Conn,uTargetId,uChangeToPos)
	CArmyCorpsBase.ChangeArmyCorpsPos(Conn,uTargetId,uChangeToPos)
end
--佣兵团end

function Gac2Gas:UseFbItem(Conn,activityName, actionName, count,itemName, times, num)
	CMatchGameRoom.UseFbItem(nil, Conn,activityName, actionName, count, itemName, times, num)
end

function Gac2Gas:UseFbSecItem(Conn,activityName, itemName, num)
	CMatchGameRoom.UseFbItem(nil, Conn,activityName, nil, 0, itemName, 0, num)
end
	
function Gac2Gas:RetIssueFetchWnd(Conn)
	CFetchRes.CanIssueFetch(nil,Conn)
end


function Gac2Gas:OpenExpOrSoulBottle(Conn,nRoomIndex, nPos)
	CExpOrSoulStorage.OpenExpOrSoulBottle(Conn,nRoomIndex, nPos)
end


--创建挑战旗
function Gac2Gas:CreateChallengeFlag(Conn)
	CChanllengeFlagMgr.CreateChallengeFlag(Conn)
end

--销毁挑战旗
function Gac2Gas:DestroyChallengeFlag(Conn)
	CChanllengeFlagMgr.DestroyChallengeFlag(Conn)
end

--判断玩家和Obj之间的距离
function Gac2Gas:JudgeDistance(Conn)
	CChanllengeFlagMgr.JudgeDistance(Conn.m_Player)
end

function Gac2Gas:SendChallengeMessage(Conn, EntityID)
	CChanllengeFlagMgr.SendChallengeMessage(Conn, EntityID)
end

--挑战开始!
function Gac2Gas:StartChallenge(Conn, isAccept)
	CChanllengeFlagMgr.StartChallenge(Conn, isAccept)
end

function Gac2Gas:CancelSign(Conn) 
	CTongRobResMgr.CancelSignResource(nil, Conn)
end

function Gac2Gas:ShowCancelWnd(Conn) 
	CTongRobResMgr.ShowCancelWnd(nil, Conn)
end

function Gac2Gas:EnterRobScene(Conn, flag)
	CTongRobResMgr.EnterRobScene(Conn, flag)
end

function Gac2Gas:OpenSignQueryWnd(Conn)
	CTongRobResMgr.OpenSignQueryWnd(nil, Conn)
end

function Gac2Gas:JoinRobResFb(Conn)   
	CTongRobResMgr.JoinRobResFb(nil, Conn)
end

function Gac2Gas:GetProffer(Conn)
	CTongRobResMgr.GetProffer(nil, Conn)
end

function Gac2Gas:ConfirmJoin(Conn)
	CTongRobResMgr.UpdateJoinInfo(nil, Conn)
end

function Gac2Gas:PlayerExitFb(Conn)
	CTongRobResMgr.LeaveRobResFbScene(Conn)
end

function Gac2Gas:PlayerOutFb(Conn)
	CTongRobResMgr.PlayerOutFb(Conn)
end

function Gac2Gas:GetHeaderAward(Conn) 
	CTongRobResMgr.GetHeaderAward(nil, Conn)
end

-- 中地图
function Gac2Gas:GetFbPlayerPos(Conn)
	CFbPlayerPosMgr.GetFbPlayerPos(Conn)
end

function Gac2Gas:StopGetFbPlayerPos(Conn)
	CFbPlayerPosMgr.StopGetFbPlayerPos(Conn)
end

function Gac2Gas:GetTeammatePos(Conn)
	CGasTeammatePosMgr.GetTeammatePos(Conn)
end

function Gac2Gas:StopGetTeammatePos(Conn)
	CGasTeammatePosMgr.StopGetTeammatePos(Conn)
end

function Gac2Gas:GetTongmatePos(Conn)
	CGasTongmatePosMgr.GetTongmatePos(Conn)
end

function Gac2Gas:StopGetTongmatePos(Conn)
	CGasTongmatePosMgr.StopGetTongmatePos(Conn)
end

--开启PK开关
function Gac2Gas:OpenPkSwitch(Conn, PkSwitchValue)
	CScenePkMgr.OpenPkSwitch(Conn, PkSwitchValue)
end

function Gac2Gas:IsCanTransport(Conn, CostMoney, Name, EntityID, isBind)
	CSpecialNpc.CanTransport(Conn, CostMoney, Name, EntityID,isBind)
end

function Gac2Gas:RetTransInfo(Conn, Name, EntityID, id)
	CSpecialNpc.RetTransInfo(Conn, Name, EntityID, id)
end

------------------------------rpc函数-------------------------------------------
function Gac2Gas:RemitSpecScene(Conn, EntityID, Name)
	CSpecialNpc.RemitSpecScene(Conn, EntityID, Name)
end

--刷怪
function Gac2Gas:RenoSpecNpc(Conn, EntityID, Name)
	CSpecialNpc.RenoSpecNpc(Conn, EntityID, Name)
end

function Gac2Gas:GetPicInfo(Conn,EntityID)
	CSpecialNpc.GetPicInfo(Conn,EntityID)
end


function Gac2Gas:CreateAnswerTbl(Conn, I)
	CEssayQuestion.CreateAnswerTbl(Conn, I)
end


function Gac2Gas:AnswerQuestion(Conn, QuestionStoreName,QuestionId, Answer, QuestName, ID, EntityId)
	CEssayQuestion.AnswerQuestion(Conn, QuestionStoreName,QuestionId, Answer, QuestName, ID, EntityId)
end

function Gac2Gas:AnswerQuestionFinish(Conn, QuestionCount, QuestName, QuestionMode, AnswerRightNum, ID)
	CEssayQuestion.AnswerQuestionFinish(Conn, QuestionCount, QuestName, QuestionMode, AnswerRightNum, ID)
end

function Gac2Gas:ClearTempBagObj(Conn, Index)
	if not IsCppBound(Conn) or not IsCppBound(Conn.m_Player) then
		return
	end
	Conn.m_Player:CancelCastingProcess()
	Conn.m_Player.m_TempBag:ClearTempBagObj(Conn, Index)
end

function Gac2Gas:UseTempBagObj(Conn, Index)
	CTempBag.UseTempBagObj(Conn, Index)
end

function Gac2Gas:IdentityAffirm(Conn, strPassword)
	CItemBagLock.IdentityAffirm(Conn,strPassword)
end

function Gac2Gas:LockItemBagByTime(Conn,lock_time)
	CItemBagLock.LockItemBagByTime(Conn,lock_time)
end

function Gac2Gas:OpenItemBagLockWnd(Conn)
	CItemBagLock.OpenItemBagLockWnd(Conn)
end

function Gac2Gas:ItemBagLockTimeOut(Conn)
	CItemBagLock.ItemBagLockTimeOut(Conn)
end


function Gac2Gas:GetCampInfoByNum(Conn, returnFlag)
	CRecruitMgr.GetCampInfoByNum(Conn, returnFlag)
end

function Gac2Gas:UpdateEquipEffectToGas(Conn,uState)
	CGasFightingEvaluation.UpdateEquipEffectToGas(Conn,uState)
end

function Gac2Gas:OpenTongNeedFireActivity(Conn)
	CTongNeedFireActivity.OpenTongNeedFireActivity(Conn)
end

function Gac2Gas:CheckNeedFireIsOpen(Conn,EntityID)
	CTongNeedFireActivity.CheckNeedFireIsOpen(Conn,EntityID)
end

function Gac2Gas:SendRobResOnMiddleMap(Conn, SceneName)
	CTongRobResMgr.ShowRobResOnMiddleMap(Conn, SceneName)
end

function Gac2Gas:AddNeedFireItem(Conn,uRoomIndex, uPos,sItemName,ColorStr)
	CTongNeedFireActivity.AddNeedFireItem(Conn,uRoomIndex, uPos,sItemName,ColorStr)
end

function Gac2Gas:ActiveBehavior(Conn)
	if IsCppBound(Conn.m_Player) then
		CActiveBehavior.TargetDoBehavior(Conn.m_Player, 0)
	end
end

--释放临时技能
function Gac2Gas:ActiveBehaviorDoSkill(Conn, TargetID ,SkillName,PosX,PosZ)
	if IsCppBound(Conn.m_Player) then
		CActiveBehavior.ActiveBehaviorDoSkill(Conn.m_Player, TargetID ,SkillName,PosX,PosZ)
	end
end

function Gac2Gas:QuitFitoutState(Conn)
	CActiveBehavior.QuitFitoutState(Conn)
end

--左键按下时,接收的包
function Gac2Gas:BeginKickBehavior(Conn)
	CKickBehavior.BeginKickBehavior(Conn)
end

--左键抬起时,接收的包
function Gac2Gas:EndKickBehavior(Conn,IntObjId,XLPos)
	CKickBehavior.EndKickBehavior(Conn,IntObjId,XLPos)
end

--取消时,接收的包
function Gac2Gas:CancelKickBehavior(Conn)
	CKickBehavior.CancelKickBehavior_C(Conn)
end

function Gac2Gas:YYRequest(Conn,sLoginKey,sMacAddress,DistStr)
	CYYLoginServer.YYRequest(Conn,sLoginKey,sMacAddress,DistStr)
end

function Gac2Gas:YYlogin(Conn,key)
	CYYLoginServer.YYlogin(Conn,key)
end

function Gac2Gas:UseVIPItem(Conn, nSmallID, nRoomIndex, nPos)
	CUseVIPItem.UseVIPItem(Conn, nSmallID, nRoomIndex, nPos)
end

function Gac2Gas:ChangeSceneByMsg(Conn, posx, posy, sceneId, serverId)
	CTongItemUseMgr.ChangeSceneByMsg(Conn, posx, posy, sceneId, serverId)
end

function Gac2Gas:CancelChangeCameraPos(Conn)
	CScenarioExploration.CancelChangeCameraPos(Conn)
end

function Gac2Gas:WeiBoLoginCheck(Conn, sCode)
	Conn.m_WeiBo:WeiBoLoginCheck(Conn, sCode)
end

function Gac2Gas:WeiBoRequestStart(Conn, sLoginKey, sMacAddress, DistStr)
	local weibo = CGasWeiBo:new()
	Conn.m_WeiBo		= weibo
	weibo.m_Conn		= Conn
	weibo.m_LoginKey 	= sLoginKey
	weibo.m_MacAddress 	= sMacAddress
	weibo.m_DistStr		= DistStr
	weibo:WeiBoRequestStart()
end


function Gac2Gas:RandomShowTongSellResWnd(Conn, NpcID)
	CGasTongMarket.RandomShowTongSellResWnd(Conn, NpcID)
end

function Gac2Gas:IssueFetchQueryWnd(Conn)
	CGasTongMarket.IssueFetchQueryWnd(Conn)
end

function Gac2Gas:ShowTongResTransWnd(Conn, npcId)
	CGasTongMarket.ShowTongResTransWnd(Conn, npcId)
end

function Gac2Gas:TransToTongArea(Conn, TongName)
	CGasTongMarket.TransToTongArea(Conn, TongName)
end

function Gac2Gas:SellTongResource(Conn,TongID)  --0:自然人;1:驻地Npc
	CGasTongMarket.SellTongResource(Conn,TongID)
end
