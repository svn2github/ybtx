gac_require "framework/rpc/Gas2GacInc"
gac_gas_require "framework/rpc/RemoteInfo"
gac_gas_require "reload/Update"
gac_require "toolbar/short_cut/GacShortcut"
gac_require "relation/team/TeamClient"
--gac_require "relation/cofc/CofCNpcDialog"
gac_require "relation/tong/TongRecommend"
gac_require "smallgames/HeadGames/HeadFishing"

local tblErrMap = {}
tblErrMap[-1] = "rpc函数id错误"
tblErrMap[-2] = "验证码校验出错，可能出现网络消息错误"
tblErrMap[-3] = "数据格式错误"
tblErrMap[-4] = "没有找到该rpc处理函数"
tblErrMap[-5] = "没有Gas2Gac"

g_AutoSeekTbl = {}

function Gas2Gac:OnError(num, fun)
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

function Gas2Gac:ByteCall(Pipe,x,y )
	--print("recv data" .. x .. y)
end

function Gas2Gac:GMPrint(Pipe,retStr )
	g_GameMain.m_GMConsole:SetResult(retStr)
end

function Gas2Gac:Reload(Pipe,szFile )
	local Reload = CUpdate:new()
	local RetFlag,RetMessage = Reload:Reload(szFile)
	g_GameMain.m_GMConsole:SetResult(RetMessage)
end

-- 远程变量读取: 
-- GetValue  获取单个变量	  MS MakeSure 方式
-- GetValues 获取多个变量     CB CallBack 方式
-- GetType   获取单个变量类型

function Gas2Gac:Remote_MS_GetValue( Pipe, strVar )
	local retStr = Remote._GetValue( strVar )
	local ret = Gac2Gas:Remote_ResumeResult( Pipe, retStr )
end

function Gas2Gac:Remote_MS_GetValues( Pipe, strVar )
	local retStr = Remote._GetValues( strVar )
	local ret = Gac2Gas:Remote_ResumeResult( Pipe, retStr )
end

function Gas2Gac:Remote_MS_GetType( Pipe, strVar )
	local retStr = Remote._GetType( strVar )
	local ret = Gac2Gas:Remote_ResumeResult( Pipe, retStr )
end

function Gas2Gac:Remote_ResumeResult( Pipe, strStr )
	coroutine.resume( Remote.co, strStr )
end

function Gas2Gac:SendUserGameID(Conn,nGameID)
	 g_GameID = nGameID
end

function Gas2Gac:Remote_ReceiveResult( Pipe, strStr )
	 g_GameMain.m_GMConsole:OnReceiveResult( strStr )
end

function Gas2Gac:RetGetTotalTicket(Conn,nTotalBalance)
	 g_GameMain.m_Agiotage:RetGetTotalTicket(Conn,nTotalBalance)
end

function Gas2Gac:RetDelTicket(Conn,nTotalBalance)
	 g_GameMain.m_Agiotage:RetDelTicket(Conn,nTotalBalance)
end
--------------------------------------【gm补偿】--------------------------
function Gas2Gac:ReturnGetCompensateItemInfoBegin(Conn,ActivityID) 
	g_GameMain.m_GmActivity:ReturnGetCompensateItemInfoBegin(Conn,ActivityID) 
end

function Gas2Gac:ReturnGetCompensateItemInfo(Conn,ActivityID,ItemDisName,item_left_num,begin_time,end_time) 
	g_GameMain.m_GmActivity:ReturnGetCompensateItemInfo(Conn,ActivityID,ItemDisName,item_left_num,begin_time,end_time) 
end

function Gas2Gac:ReturnActivationCodeCompensateItem(Conn,CompensateID,ItemType,ItemName,ItemCount) 
	g_GameMain.m_GmActivity:ReturnActivationCodeCompensateItem(Conn,CompensateID,ItemType,ItemName,ItemCount) 
end

function Gas2Gac:ReturnGetCompensateItemInfoEnd(Conn,ActivityID) 
	g_GameMain.m_GmActivity:ReturnGetCompensateItemInfoEnd(Conn,ActivityID)
end

-------------------------------------【邮件】------------------------------
function Gas2Gac:RetMailListBegin(Conn)
	CMailRpc.RetMailListBegin(Conn)
end
function Gas2Gac:RetMailList(Conn,mailID,senderID,mailTitle,mailState,senderName,MailType,mail_LeftSeconds)
  CMailRpc.RetMailList(Conn,mailID,senderID,mailTitle,mailState,senderName,MailType,mail_LeftSeconds)
end
function Gas2Gac:RetMailListEnd(Conn)
  CMailRpc.RetMailListEnd(Conn)
end

function Gas2Gac:RetGetSendMoneyCess(Conn,SendMoney,cess)
    g_GameMain.m_SendBox:RetGetSendMoneyCess(SendMoney,cess)
end

function Gas2Gac:RetDeleteMail(Conn,IsSuccessful)
	CMailRpc.RetDeleteMail(Conn,IsSuccessful)
end
function Gas2Gac:OpenMailWnd(Conn)
	CMailRpc.OpenMailWnd(Conn)
end
--右键点击包裹里的邮件文本附件，查看文本信息返回函数
--参数：时间、发送者、标题、内容、类型（0为普通用户邮件，1为系统邮件）
function Gas2Gac:RetCheckMailTextAttachment(Conn, sendMailTime, sender, mailTitle, mailTextAttach, senderType,strType)
	CMailRpc.RetCheckMailTextAttachment(Conn, sendMailTime, sender, mailTitle, mailTextAttach, senderType,strType)
end
--从服务器返回邮件的详细信息
function Gas2Gac:RetGetMailInfo(Conn,mailID,senderID,receiverID,EmailTitle,EmailText,EmailState,MoneyAtta,SenderName,MailType,strType)
   CMailRpc.RetGetMailInfo(Conn,mailID,senderID,receiverID,EmailTitle,EmailText,EmailState,MoneyAtta,SenderName,MailType,strType)
end
function Gas2Gac:RetGetMailGoods(Conn,mailid,goods_id,emailIndex,BigID,Index,count)--EmailID, 物品id,第几个格子,物品类型名,物品名 及 物品数量
	CMailRpc.RetGetMailGoods(Conn,mailid,goods_id,emailIndex,BigID,Index,count)
end
function Gas2Gac:RetGetMailGoodsEnd(Conn,mailID)  --物品接收完成,显示
   CMailRpc.RetGetMailGoodsEnd(Conn,mailID)
end
function Gas2Gac:RetTakeAtachment(Conn,succeed, mailID)--提取全部附件
	CMailRpc.RetTakeAtachment(Conn,succeed, mailID)
end
function Gas2Gac:RetTakeAtachmentByIndex(Conn,index,succeed, mailID)
	CMailRpc.RetTakeAtachmentByIndex(Conn,index,succeed, mailID)
end
--返回发送邮件后的信息,成功或失败
function Gas2Gac:RetSendEmail(Conn,IsSuccessful)
	CMailRpc.RetSendEmail(Conn,IsSuccessful)
end
--发送物品函数
function Gas2Gac:SendMailGoodsBegin(Conn)
  CMailRpc.SendMailGoodsBegin(Conn)
end

function Gas2Gac:RetSendUserAdviceInfo(Conn,MsgId)
		local str = GetStaticTextClient(MsgId)
	g_GameMain.m_MsgBox = MessageBox(g_GameMain,str , MB_BtnOK, CallBack, g_GameMain)
end

function Gas2Gac:RetQuestionsList(Conn, Content)
	if g_GameMain then
		g_GameMain.m_UserAdviceWnd:InitQuestionList(Content)
	end
end

-----------------------------------【物品分解】-----------------------------
function Gas2Gac:ReturnBreakItemBegin(Conn)
	CBreakItemRpc.ReturnBreakItemBegin(Conn)
end
function Gas2Gac:ReturnBreakProducts(Conn,item_type,item_name,item_num)
	CBreakItemRpc.ReturnBreakProducts(Conn,item_type,item_name,item_num)
end
function Gas2Gac:ReturnBreakItemEnd(Conn,bFlag,nRoomIndex,nPos)
	CBreakItemRpc.ReturnBreakItemEnd(Conn,bFlag,nRoomIndex,nPos)
end
function Gas2Gac:AddBreakItemExp(Conn,nAddExp)
	CBreakItemRpc.AddBreakItemExp(Conn,nAddExp)
end
function Gas2Gac:InitBreakItemExp(Conn,nAddExp)
	CBreakItemRpc.InitBreakItemExp(Conn,nAddExp)
end

-------------------------------------【金钱】----------------------------------
function Gas2Gac:SetMoneyTypeBtnCheck(Conn, moneyType)
	CMoneyRpc.SetMoneyTypeBtnCheck(Conn, moneyType)
end
function Gas2Gac:ReturnAddMoney( Connection,nMoney)
	CMoneyRpc.ReturnAddMoney( Connection,nMoney)
end
function Gas2Gac:ReturnAddMoneyError( Connection,nMoney)
	CMoneyRpc.ReturnAddMoneyError( Connection,nMoney)
end
function Gas2Gac:ReturnAddTicket( Connection,nTicket)
	CMoneyRpc.ReturnAddTicket( Connection,nTicket)
end	
function Gas2Gac:ReturnAddTicketError( Connection,nTicket)
	CMoneyRpc.ReturnAddTicketError( Connection,nTicket)
end
function Gas2Gac:ReturnAddBindingMoney( Connection,nMoney)
	CMoneyRpc.ReturnAddBindingMoney( Connection,nMoney)
end
function Gas2Gac:ReturnAddBindingMoneyError( Connection,nMoney)
	CMoneyRpc.ReturnAddBindingMoneyError( Connection,nMoney)
end
function Gas2Gac:ReturnAddBindingTicket( Connection,nTicket)
	CMoneyRpc.ReturnAddBindingTicket( Connection,nTicket)
end	
function Gas2Gac:ReturnAddBindingTicketError( Connection,nTicket)
	CMoneyRpc.ReturnAddBindingTicketError( Connection,nTicket)
end

-----------------------------------【物品使用】-------------------------
--删除装备时回调的函数
function Gas2Gac:RetFetchEquipByPart(Conn, eEquipPart, bDel)
	CUseItemRpcMgr.RetFetchEquipByPart(eEquipPart, bDel)
end
--戒指交换函数
function Gas2Gac:RetSwitchTwoRing(Connection,nRBigID,nRIndex,nLBigID,nLIndex)
	CUseItemRpcMgr.RetSwitchTwoRing(Connection,nRBigID,nRIndex,nLBigID,nLIndex)
end
--武器交换函数
function Gas2Gac:RetSwitchWeapon(Connection,nWeaponBigID,nWeaponIndex,nAssociateWeaponBigID,nAssociateWeaponIndex)
	CUseItemRpcMgr.RetSwitchWeapon(Connection,nWeaponBigID,nWeaponIndex,nAssociateWeaponBigID,nAssociateWeaponIndex)
end
--使用物品
function Gas2Gac:RetUseItem( Connection,nRoomIndex,nPos,nItemID,eEquipPart)
	CUseItemRpcMgr.RetUseItem( Connection,nRoomIndex,nPos,nItemID,eEquipPart)
end												
function Gas2Gac:AoiAddEquip(Connection,uEntityID,EquipPart,nBigID,nIndex)
	CUseItemRpcMgr.AoiAddEquip(Connection,uEntityID,EquipPart,nBigID,nIndex)
end
function Gas2Gac:SetItemGridWndState(Conn, Room, Pos, State)
	CStoreRoomWndMgr.GetMgr():SetItemGridWndState(Room,Pos,State)
end
function Gas2Gac:SpecailItemCoolDown(Conn,ItemName,time,coolDownType,nRoomIndex,nPos)
	CUseItemRpcMgr.SpecailItemCoolDown(Conn,ItemName,time,coolDownType,nRoomIndex,nPos)
end
function Gas2Gac:SetItemCoolDownTime(Conn, itemName, CoolDownTime, processTime)
	CUseItemRpcMgr.SetItemCoolDownTime(Conn, itemName, CoolDownTime, processTime)
end

------------------------------------------【背包\物品】-----------------------------
--交换两个包
function Gas2Gac:ReturnChange2Bag(Connection,nASlot,nBSlot)	
	CRoomItemRpc.ReturnChange2Bag(Connection,nASlot,nBSlot)	
end
function Gas2Gac:ReturnChange2BagError(Connection)
	CRoomItemRpc.ReturnChange2BagError(Connection)
end
--取下包
function Gas2Gac:ReturnFetchBag(Connection, nSlot, nRoom, nPos, nGridID)
	CRoomItemRpc.ReturnFetchBag(Connection, nSlot, nRoom, nPos, nGridID)
end
function Gas2Gac:ReturnFetchBagError(Connection)
	CRoomItemRpc.ReturnFetchBagError(Connection)
end
function Gas2Gac:ReturnSetBagState(Connection)
	CRoomItemRpc.ReturnSetBagState(Connection)
end
--放置背包
function Gas2Gac:ReturnPlaceBag(Connection,nRoom,nPos,nSlot,nUseRoomIndex)
	CRoomItemRpc.ReturnPlaceBag(Connection,nRoom,nPos,nSlot,nUseRoomIndex)
end
function Gas2Gac:RetSoulPearlInfo( Connection, uItemId, uSoulNum )
	CRoomItemRpc.RetSoulPearlInfo( Connection, uItemId, uSoulNum )
end
function Gas2Gac:ReturnPlaceBagError(Connection)
	CRoomItemRpc.ReturnPlaceBagError(Connection)
end
-- 删除背包
function Gas2Gac:ReturnDelBag(Connection,nSlot)
	CRoomItemRpc.ReturnDelBag(Connection,nSlot)
end
function Gas2Gac:ReturnDelBagError(Connection,nError)
	CRoomItemRpc.ReturnDelBagError(Connection,nError)
end
--鼠标右键，交换物品
function Gas2Gac:RetQuickMoveEnd(Connection)
	CRoomItemRpc.RetQuickMoveEnd(Connection)
end
function Gas2Gac:RetQuickMoveError(Connection,nError)
	CRoomItemRpc.RetQuickMoveError(Connection,nError)
end
function Gas2Gac:RetCleanBag(Connection)
	CRoomItemRpc.RetCleanBag(Connection)
end
function Gas2Gac:RetDelItem(Connectionn,ItemID)
	CRoomItemRpc.RetDelItem(nItemID)
end
--设置事件，为gm命令删除
function Gas2Gac:RetDelItemAllGMEnd( Connection,nBigID,nIndex,nCount)
	CRoomItemRpc.RetDelItemAllGMEnd( Connection,nBigID,nIndex,nCount)
end
function Gas2Gac:RetItemTypeInfo(Connection,nItemID,nBigID,nIndex,nBindingType)
	CRoomItemRpc.RetItemTypeInfo(Connection,nItemID,nBigID,nIndex,nBindingType)
end

function Gas2Gac:SetItemBindingTypeByID(Connection,nItemID,nBindingType)
	CRoomItemRpc.SetItemBindingTypeByID(Connection,nItemID,nBindingType)
end
function Gas2Gac:RetItemLeftTime(Connection,nItemID,nLeftTime)
	CRoomItemRpc.RetItemLeftTime(Connection,nItemID,nLeftTime)
end
--**********************************************************************
--增删物品
--**********************************************************************
function Gas2Gac:RetItemRoom(Conn, nRoomIndex)
	CRoomItemRpc.RetItemRoom(nRoomIndex)
end
function Gas2Gac:RetAddItemToGrid(Conn, nItemID)
	CRoomItemRpc.RetAddItemToGrid(nItemID)
end
function Gas2Gac:RetAddItemToGridEnd(Conn, nGridID, nPos, nType, sName, nBind)
	CRoomItemRpc.RetAddItemToGridEnd(nGridID, nPos, nType, sName, nBind)
end
function Gas2Gac:RetAddItemToGridEndEx(Conn, nGride, nPos)
	CRoomItemRpc.RetAddItemToGridEndEx(nGride, nPos)
end
function Gas2Gac:RetRefreshBag(Conn)
	CRoomItemRpc.RetRefreshBag()
end
function Gas2Gac:RetDelItemFromGrid(Connection, nItemID, nRoomIndex, nPos, bDelDynInfo)
	CRoomItemRpc.RetDelItemFromGrid(nItemID, nRoomIndex, nPos, bDelDynInfo)
end
function Gas2Gac:RetDelItemError(Connection,nError)
	CRoomItemRpc.RetDelItemError(Connection,nError)
end
--删除物品的GM命令错误返回
function Gas2Gac:RetDelItemGMError( Connection,nBigID,nIndex,sError)
	CRoomItemRpc.RetDelItemGMError( Connection,nBigID,nIndex,sError)
end
--暂时只用在佣兵小队仓库
function Gas2Gac:RetAddItem(Connection,nItemID,nRoomIndex,nPos)
	CRoomItemRpc.RetAddItem(nItemID,nRoomIndex,nPos)
end
--*********************************************************
--物品制作者
function Gas2Gac:RetItemMakerName(Connection,maker_name,item_id)
	CRoomItemRpc.RetItemMakerName(Connection,maker_name,item_id)
end
--设置事件部分
function Gas2Gac:RetAddItemAllGMEnd( Connection,nBigID,nIndex,nCount)
	CRoomItemRpc.RetAddItemAllGMEnd( Connection,nBigID,nIndex,nCount)
end
function Gas2Gac:RetAddItemGMError( Connection,nBigID,nIndex,sError)
	CRoomItemRpc.RetAddItemGMError( Connection,nBigID,nIndex,sError)
end
--拆分物品
function Gas2Gac:RetSplitItem(Conn, nSrcRoom, nSrcPos, nItemID)
	CRoomItemRpc.RetSplitItem(nSrcRoom, nSrcPos, nItemID)
end
function Gas2Gac:RetSplitItemEnd(Conn, nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	CRoomItemRpc.RetSplitItemEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
end
function Gas2Gac:RetSplitItemError( Connection,nError,nARoom,nAPos,nBRoom,nBPos)
	CRoomItemRpc.RetSplitItemError( Connection,nError,nARoom,nAPos,nBRoom,nBPos)
end
--移动物品
function Gas2Gac:RetMoveItem( Connection,nARoom,nAPos,nItemID,nBRoom,nBPos)
	CRoomItemRpc.RetMoveItem( Connection,nARoom,nAPos,nItemID,nBRoom,nBPos)
end
function Gas2Gac:RetMoveItemEnd( Connection,nARoom,nAPos,nBRoom,nBPos)
	CRoomItemRpc.RetMoveItemEnd( Connection,nARoom,nAPos,nBRoom,nBPos)
end
function Gas2Gac:RetMoveItemByGridID(Conn, nGridID, nRoomIndex, nPos)
	CRoomItemRpc.RetMoveItemByGridID(nGridID, nRoomIndex, nPos)
end
function Gas2Gac:RetReplaceItemByGridIDEnd(Conn, nSrcRoom, nSrcPos, nDescRoom, nDescPos)
	CRoomItemRpc.RetReplaceItemByGridIDEnd(nSrcRoom, nSrcPos, nDescRoom, nDescPos)
end
function Gas2Gac:RetPlayerItemSound(Conn, nItemType, sItemName)
	CRoomItemRpc.RetPlayerItemSound(Conn, nItemType, sItemName)
end
--移动物品的服务端回调
function Gas2Gac:RetMoveReplaceItem( Connection,nARoom,nAPos,nBRoom,nBPos )
	CRoomItemRpc.RetMoveReplaceItem( Connection,nARoom,nAPos,nBRoom,nBPos )
end
function Gas2Gac:RetMoveItemError( Connection,nARoom, nAPos, nBRoom, nBPos)
	CRoomItemRpc.RetMoveItemError( Connection,nARoom, nAPos, nBRoom, nBPos)
end
--初始化玩家的附背包信息
function Gas2Gac:RetCharBagInfo(Connection,nItemID,uBagSlotIndex,nRoomIndex,nBigID,nIndex,nBindingType)
	CRoomItemRpc.RetCharBagInfo(Connection,nItemID,uBagSlotIndex,nRoomIndex,nBigID,nIndex,nBindingType)
end
function Gas2Gac:RetCharBagInfoEnd(Connection)
	CRoomItemRpc.RetCharBagInfoEnd(Connection)
end

---------------------------------------------------------
--龙穴活动
function Gas2Gac:RetShowDragonCaveWnd(Conn, NpcID)
	local Wnd = g_GameMain.m_DragonCaveWnd
	Wnd.m_TargetNpcId = NpcID
  Wnd:ShowWnd(true)
end

function Gas2Gac:RetEnterDragonCave(Conn, SceneName)
	CDragonCaveWnd:RetEnterDragonCave(Conn, SceneName)
end

function Gas2Gac:SendMsgToLocalSence(Conn)
	CDragonCaveWnd:SendMsgToLocalSence(Conn)
end

function Gas2Gac:SendMsgToOtherSence(Conn)
	CDragonCaveWnd:SendMsgToOtherSence(Conn)
end

function Gas2Gac:SendMsgToEverySence(Conn)
	CDragonCaveWnd:SendMsgToEverySence(Conn)
end

--区域副本中的rpc
function Gas2Gac:OpenAllAreaFb(Conn)	
	CAreaFbSelWndNew.OpenAllAreaFb()
end

function Gas2Gac:RetShowAreaFbSelWnd(Conn, NpcID)
	local Wnd = CAreaFbInfoWnd.GetWnd()
	Wnd:InitAreaFbInfoWnd(NpcID)
end

function Gas2Gac:RetAreaFbBossNum(Conn, SceneName, sceneId, serverId, LeftNum, BossNum)
	CAreaFbSelWndNew.RetAreaFbBossNum(SceneName, sceneId, serverId, LeftNum, BossNum)
end

function Gas2Gac:RetIsJoinAreaFb(Connection, SceneName, FbDifficulty, FbLev, SceneID, ServerId)
	CAreaFbSelWndNew.RetIsJoinAreaFb(SceneName, FbDifficulty, FbLev, SceneID, ServerId)
end

function Gas2Gac:RetDelAllAreaFb(Connection)
	CAreaFbSelWndNew.RetDelAllAreaFb()
end

function Gas2Gac:RetIsChangeOut(Connection, time, msgID)
	CAreaFbSelWndNew.RetIsChangeOut(time, msgID)
end

--点挑战任务传送OBJ时,显示列表窗口的rpc
function Gas2Gac:RetAreaFbListBegin(Connection, SceneName, QuestName)
	local Wnd = CAreaFbListWnd.GetWnd()
	Wnd:RetAreaFbListBegin(SceneName, QuestName)
end
function Gas2Gac:RetAreaFbListEnd(Connection)
	local Wnd = CAreaFbListWnd.GetWnd()
	Wnd:RetAreaFbListEnd()
end

--
function Gas2Gac:RetObjShowContentWnd(Connection,GlobalID)
	local Wnd = CObjShowContent.GetWnd()
	Wnd:RetObjShowContentWnd(GlobalID)
end

--任务相关
function Gas2Gac:RetSendMercTempQuestPool(Conn, sQuestName, iQuestType)
	local Wnd = CQuestPoolWnd.GetWnd()
	Wnd:RetSendMercTempQuestPool(sQuestName, iQuestType)
end
function Gas2Gac:RetSendMercTempQuestPoolEnd(Conn, IsServer)
	local Wnd = CQuestPoolWnd.GetWnd()
	Wnd:RetSendMercTempQuestPoolEnd(IsServer)
end
function Gas2Gac:RetSendMercQuestPool(Conn,sQuestName,iQuestType)
	local Wnd = CQuestPoolWnd.GetWnd()
	Wnd:RetSendMercQuestPool(sQuestName,iQuestType)
end
function Gas2Gac:RetShowMercQuestPoolWndEnd(Conn, ObjEntityID, XYQuestName)
	local Wnd = CQuestPoolWnd.GetWnd()
	Wnd:RetShowMercQuestPoolWndEnd(ObjEntityID, XYQuestName)
end
function Gas2Gac:OnRClickQuestItem(Conn,uBigID,uIndex)
	CQuest.OnRClickQuestItem(Conn,uBigID,uIndex)
end
function Gas2Gac:NotifyTeammateShareQuest(Conn, teamID, PlayerName, quest_name)
	CShareQuestWnd.NotifyTeammateShareQuest(teamID, PlayerName, quest_name)
end
function Gas2Gac:RetShowFinishQuest(Conn, QuestName, ItemIndex)
	g_GameMain.m_finishTask:RetShowFinishQuest(QuestName, ItemIndex)
end
function Gas2Gac:RetShowQihunjiZhiyinWnd(Conn, IsShow)
	g_GameMain.m_QihunjiZhiyinWnd:RetShowQihunjiZhiyinWnd(IsShow)
end
function Gas2Gac:RetShowTakeQuest(Conn, QuestName)
	g_GameMain.m_takeTask:RetShowTakeQuest(QuestName)
end
function Gas2Gac:FarFromNPC(Conn)
	g_GameMain.m_takeTask:FarFromNPC()
end
function Gas2Gac:RetShowTakeDareQuest(Conn, QuestName, SceneName)
	g_GameMain.m_takeTask:RetShowTakeDareQuest(QuestName, SceneName)
end

--佣兵指引系统
function Gas2Gac:RetUpdateMercenaryLevelTrace(Conn,MercenaryLevel,Status,Subject1,Subject2,Subject3,Count1,Count2,Count3,Challenge,Integral,Level)
	g_GameMain.m_MercenaryLevelTrace:RetUpdateMercenaryLevelTrace(MercenaryLevel,Status,Subject1,Subject2,Subject3,Count1,Count2,Count3,Challenge,Integral,Level)
end
function Gas2Gac:ShowMercenaryLevelMessage(Conn, Num)
	g_GameMain.m_MercenaryLevelTrace:ShowMercenaryLevelMessage(Num)
end
function Gas2Gac:RetCheckInMercenaryInfo(Conn)
	g_GameMain.m_MercenaryLevelTrace:RetCheckInMercenaryInfo()
end
function Gas2Gac:SetMercenaryLevelFinishAwards(Conn, Award)
	g_GameMain.m_MercenaryLevelAward:SetMercenaryLevelFinishAwards(Award)
end
function Gas2Gac:ClearMercenaryLevelFinishAwards(Conn)
	g_GameMain.m_MercenaryLevelAward:ClearMercenaryLevelFinishAwards()
end
function Gas2Gac:RetShowMercenaryLevelAward(Conn)
	g_GameMain.m_MercenaryLevelAward:RetShowMercenaryLevelAward()
end
function Gas2Gac:RetUpdateMercenaryLevelAwardItem(Conn, Award)
	g_GameMain.m_MercenaryLevelAward:RetUpdateMercenaryLevelAwardItem(Award)
end
function Gas2Gac:RetShowOpenYbAssessWnd(Conn)
	g_GameMain.m_MercenaryAssess:RetShowOpenYbAssessWnd()
end
function Gas2Gac:ShowNpcTalkWnd(Conn, TalkName, uEntityID, uBuildingTongID, IsUseTrigger)
	g_GameMain.m_NpcTalkWnd:ShowNpcTalkWnd(TalkName, uEntityID, uBuildingTongID, IsUseTrigger)
end
function Gas2Gac:RetShowFuncNpcOrObjTalkWnd(Conn)
	g_GameMain.m_NpcTalkWnd:RetShowFuncNpcOrObjTalkWnd()
end

--召唤队友
function Gas2Gac:NotifyTeammateTransport(Conn,CallerName)
	local Wnd = CTransportWnd.GetWnd()
	Wnd:InitTransportHintWnd(CallerName)
end

function Gas2Gac:RetShowJiFenSaiInfoWnd(Conn, WinNum, LoseNum, RunNum, WinPro, KillNum, DeadNum, Integral)
	local Wnd = CJiFenSaiInfoWnd.GetWnd()
	Wnd:RetShowJiFenSaiInfoWnd(WinNum, LoseNum, RunNum, WinPro, KillNum, DeadNum, Integral)
end

--加入 Type{{1,个人},{2,队伍},{3,帮会}}
function Gas2Gac:RetInsertFbActionToQueue(Conn,FbName,IsTeam,PeopleNum)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:RetInsertFbActionToQueue(FbName,IsTeam,PeopleNum)
end

--删除
function Gas2Gac:RetDelQueueFbAction(Conn,FbName,DoDeleteList)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:RetDelQueueFbAction(FbName,DoDeleteList)
end

function Gas2Gac:RetSetJoinBtnEnable(Conn,FbName)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:RetSetJoinBtnEnable(FbName)
end

--在副本中的话，就直接在排队面板上加一个排队的信息
function Gas2Gac:RetInitFbActionQueueWnd(Conn,FbName)
	Gas2Gac:RetInsertFbActionToQueue(Conn,FbName,false,1)
	Gas2Gac:RetSetJoinBtnEnable(Conn,FbName)
end
function Gas2Gac:ShowMatchGameStatisticsWnd(Conn, gameName, dataList, exitTime, selIndex)
	local FbGameStatisticsWnd = CFbGameStatisticsWnd.GetWnd()
	FbGameStatisticsWnd:ShowMatchGameStatisticsWnd(gameName, dataList, exitTime, selIndex)
end

--排队面板确认消息
function Gas2Gac:RetFbActionOverTime(Conn,FbName)
	if g_GameMain.m_FbActionMsgWnd[FbName] then
		g_GameMain.m_FbActionMsgWnd[FbName]:SetShowWnd(false)
	end
end
function Gas2Gac:RetIsJoinFbActionAffirm(Conn,FbName,FbTime,NpcLevel)
	CFbActionMsgWnd.ShowFbActionMsgWnd(FbName,FbTime,1,NpcLevel)--现在,怪物难度只有1
end

--大逃杀中等待的人数
function Gas2Gac:RetInFbActionPeopleNum(Conn,FbName,PeopleNum)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:RetInFbActionPeopleNum(FbName,PeopleNum)
end

function Gas2Gac:WaitOtherTeammate(Connection, actionName)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:WaitOtherTeammate(actionName)
end

function Gas2Gac:ChangeActionWarModel(Connection)
	local FbActionQueueWnd = CFbActionQueueWnd.GetWnd()
	FbActionQueueWnd:ChangeActionWarModel()
end

--Boss争夺战的消息
function Gas2Gac:RetShowBossBattleWnd(Conn)
	g_GameMain.m_BossBattleWnd:ShowWnd(true)
end
function Gas2Gac:RetShowBossBattleInfoWnd(Conn, SceneName, IsShow)
	g_GameMain.m_BossBattleWnd.m_InfoWnd.m_Count:SetWndText(0)
	g_GameMain.m_BossBattleWnd.m_InfoWnd:ShowWnd(IsShow)
end
function Gas2Gac:RetUpdateBossBattleInfoWnd(Conn, Count)
	g_GameMain.m_BossBattleWnd.m_InfoWnd.m_Count:SetWndText(Count)
end
function Gas2Gac:BossBattleKillBossMsg(Conn, SceneName, BossName, TongName, PlayerName)
	g_GameMain.m_BossBattleWnd:BossBattleKillBossMsg(SceneName, BossName, TongName, PlayerName)
end
function Gas2Gac:BossBattleJoinFailMsg(Conn, SceneName)
	g_GameMain.m_BossBattleWnd:BossBattleJoinFailMsg(SceneName)
end
function Gas2Gac:RetEnterBossBattle(Conn, SceneName)
	g_GameMain.m_BossBattleWnd:RetEnterBossBattle(SceneName)
end
function Gas2Gac:RetLeaveBossBattle(Conn, NotKillBoss)
	g_GameMain.m_BossBattleWnd:RetLeaveBossBattle(NotKillBoss)
end
function Gas2Gac:RetWaitBossBattle(Conn)
	g_GameMain.m_BossBattleWnd:RetWaitBossBattle()
end

--喝酒射箭副本
function Gas2Gac:BeginDrinkShoot(Conn)
	g_GameMain.m_ShootProgressWnd:BeginDrinkShoot()
end
function Gas2Gac:UpdateDrinkShootTurnArrow(Conn, PosX, PosY)
	g_GameMain.m_ShootProgressWnd:UpdateTurnArrow(PosX, PosY)
end

--
function Gas2Gac:SendScopesExplorationDPS(Connection,Name,Hurt,Heal,Coef,Point,Rate)
	local Wnd = CDpsOutPutWnd.GetWnd()
	Wnd:AddItem(Name,Hurt,Heal,Coef/10,Point,Rate)
end
function Gas2Gac:SendScopesExplorationDPSEnd(Connection)
	local Wnd = CDpsOutPutWnd.GetWnd()
	Wnd:ShowDPSWnd()
end

function Gas2Gac:OpenFbDpsInfoWnd(Connection)
	local Wnd = CFbDPSWnd.GetWnd()
	Wnd:OpenWnd()
end
function Gas2Gac:CloseFbDpsInfoWnd(Connection)
	local Wnd = CFbDPSWnd.GetWnd()
	Wnd:CloseWnd()
end
function Gas2Gac:UpdateMemberDpsInfo(Connection, Name, Data1, Data2)
	local Wnd = CFbDPSWnd.GetWnd()
	Wnd:UpdateMemberDpsInfo(Name, Data1, Data2)
end
function Gas2Gac:DeleteMemberDpsInfo(Connection, Name)
	local Wnd = CFbDPSWnd.GetWnd()
	Wnd:DeleteMemberDpsInfo(Name)
end

--确定进副本的消息(领域副本)
function Gas2Gac:RetIsJoinScopesFb(Connection, SceneName, PosX, PosY)
	CLingYuTransmitWnd.RetIsJoinScopesFb(SceneName, PosX, PosY)
end

function Gas2Gac:SendScenarioFinishInfo(Connection, ActionName)
	local Wnd = CJuQingTransmitWnd.GetWnd()
	Wnd:InitActionInfo(ActionName)
end
--确定进副本的消息(剧情副本)
function Gas2Gac:RetShowJuQingWnd(Connection, EntityID)
	local Wnd = CJuQingTransmitWnd.GetWnd()
	Wnd:InitWnd(EntityID)
end
function Gas2Gac:RetIsJoinSenarioFb(Connection, SceneName, PosX, PosY)
	CJuQingTransmitWnd.JoinScene(SceneName, PosX, PosY)
end
function Gas2Gac:RetChangeCameraPos(Conn, ActionName, IsOpen)
	CJuQingTransmitWnd.RetChangeCameraPos(ActionName, IsOpen)
end

function Gas2Gac:ActionAllReady(Connection, serverId, roomId, actionName)
	Gac2Gas:AutoJoinActio(g_Conn, serverId, roomId, actionName)
end

function Gas2Gac:RetShowDaDuoBaoWnd(Conn)
	local Wnd = CDaDuoBaoWnd.GetWnd()
	Wnd:InitWindow()
end
--接收服务端发来的显示倒计时的窗口
--参数2:表示秒
function Gas2Gac:RetShowDownTimeWnd(Conn, DownTime)
	g_GameMain.m_DownTimeWnd:ShowWindow(DownTime)
end

function Gas2Gac:RetCloseDownTimeWnd(Conn)
	g_GameMain.m_DownTimeWnd:ShowWindow(0)
end

function Gas2Gac:OpenGameCountdownWnd(Conn, Time, Index)
	g_GameMain.m_GameDownTimeWnd:OpenGameCountdownWnd(Time, Index)
end

function Gas2Gac:CloseGameCountdownWnd(Conn)
	g_GameMain.m_GameDownTimeWnd:CloseGameCountdownWnd()
end

function Gas2Gac:SetCountdownTime(Conn, SecondNum)
	g_GameMain.m_GameDownTimeWnd:SetCountdownTime(SecondNum)
end

function Gas2Gac:RetShowDaTaoShaWnd(Conn,EntityID)
	local Wnd = CFbActionQueueWnd.GetWnd()
	local IsInWait = Wnd:FindAciont("角斗场")
	local DaTaoShaWnd = CDaTaoShaWnd.GetWnd()
	DaTaoShaWnd:RetShowDaTaoShaWnd(IsInWait, EntityID)
end

function Gas2Gac:RetSetSceneStateForClient(Conn, SceneState)
	local DaTaoShaWnd = CDaTaoShaWnd.GetWnd()
	DaTaoShaWnd:RetSetSceneStateForClient(SceneState)
end

function Gas2Gac:RetShowRollMsg(Conn, sName, sAction)
	local message = GetStaticTextClient(8004, sName, g_GetFbActionNameLanStr(sAction))
	DisplayMsg(2,message)
	SysRollAreaMsg(message)
end


function Gas2Gac:RetShowJiFenSaiWnd(Conn, EntityID)
	local Wnd = CFbActionQueueWnd.GetWnd()
	local IsInWait = Wnd:FindAciont("竞技场")
	local JiFenSaiWnd = CJiFenSaiWnd.GetWnd()
	JiFenSaiWnd:RetShowJiFenSaiWnd(IsInWait, EntityID)
end

function Gas2Gac:RetSetJFSPlayerCamp(Conn)
	MsgClient(3412)--,"进入两队PK的战斗状态了!!")
	--更新周围的NPC和OBJ
	g_GameMain.m_TeamMarkSignMgr:UpdateCharacterInView()
	--更新周围的玩家
	g_GameMain.m_CharacterInSyncMgr:UpdateCharacterInSync()
end

--发一个奖励的消息
function Gas2Gac:WeekAwardMsgFromJfs(Conn)
	local str = GetStaticTextClient(15300)--"每周竞技场排名荣誉值及经验奖励已发放，请注意查收"
	DisplayMsg(2,str)
	SysRollAreaMsg(str)
end

--黑夜本用的相关rpc
function Gas2Gac:TempBagSetGrid(Conn, Index, Name)
	local TempBagWnd = CTempBagWnd.GetWnd()
	TempBagWnd:SetGrid(Index, Name)
end
function Gas2Gac:TempBagClearGrid(Conn, Index)
	local TempBagWnd = CTempBagWnd.GetWnd()
	TempBagWnd:SetGrid(Index)
end
function Gas2Gac:OpenTempBag(Conn)
	local TempBagWnd = CTempBagWnd.GetWnd()
	TempBagWnd:OpenTempBag()
end
function Gas2Gac:CloseTempBag(Conn)
	local TempBagWnd = CTempBagWnd.GetWnd()
	TempBagWnd:CloseTempBag()
end
-------------------MatchGame副本------------------
function _GTN(index)
	local gameName = g_GameMain.m_MatchGameWnd.m_GameName
	local teamName = Lan_MatchGame_Common(MemH64(gameName), "TeamShowName")
	local teamNameTbl = loadstring(" return { " .. teamName .. " }")()
	return teamNameTbl[index] or index
end

--首次进入 小副本初始化记分界面
function Gas2Gac:InitMatchGameCountWnd(Conn, gameName, timeType, ResidualTime, gameState)
	g_GameMain.m_MatchGameWnd:InitMatchGameCountWnd(Conn, gameName, timeType, ResidualTime, gameState)
end

--其他人进入小副本初始化记分界面
function Gas2Gac:MatchGameAddTeam(Conn, isShow, teamId, dataList)
	g_GameMain.m_MatchGameWnd:MatchGameAddTeam(Conn, isShow, teamId, dataList)
end

function Gas2Gac:MatchGameAddTeamMember(Conn, isShow, teamId, playerId, dataList)
	g_GameMain.m_MatchGameWnd:MatchGameAddTeamMember(Conn, isShow, teamId, playerId, dataList)
end

function Gas2Gac:MatchGameRemoveTeam(Conn, teamId)
	g_GameMain.m_MatchGameWnd:MatchGameRemoveTeam(Conn, teamId)
end

function Gas2Gac:MatchGameRemoveTeamMember(Conn, teamId, playerId)
	g_GameMain.m_MatchGameWnd:MatchGameRemoveTeamMember(Conn, teamId, playerId)
end

function Gas2Gac:MatchGameShowTeam(Conn, teamId)
	g_GameMain.m_MatchGameWnd:MatchGameShowTeam(Conn, teamId)
end

function Gas2Gac:MatchGameShowTeamMember(Conn, teamId, playerId, playerName)
	g_GameMain.m_MatchGameWnd:MatchGameShowTeamMember(Conn, teamId, playerId, playerName)
end

function Gas2Gac:MatchGameUpdateTeamScore(Conn, teamId, dataIndex, value, isUpdate)
	g_GameMain.m_MatchGameWnd:MatchGameUpdateTeamScore(Conn, teamId, dataIndex, value, isUpdate)
end

function Gas2Gac:MatchGameUpdatePlayerScore(Conn, teamId, playerId, dataIndex, value, isUpdate)
	g_GameMain.m_MatchGameWnd:MatchGameUpdatePlayerScore(Conn, teamId, playerId, dataIndex, value, isUpdate)
end

function Gas2Gac:MatchGameSetWaitTime(Conn, Time)
	g_GameMain.m_MatchGameWnd:MatchGameSetWaitTime(Time)
end

--离开游戏的玩家
function Gas2Gac:CloseList(Conn)
	g_GameMain.m_MatchGameWnd:CloseList(Conn)
end

--游戏开始  开始计时   如果游戏类型为 3 则 time = 0
function Gas2Gac:BeginGame(Conn, time)
	g_GameMain.m_MatchGameWnd:BeginGame(Conn, time)
end

--设置面板显示文字
function Gas2Gac:SetMatchGameWndTitleText(Conn, index)
	g_GameMain.m_MatchGameWnd.m_TitleText = GetStaticTextClient(index)
end

function Gas2Gac:RetShowMatchGameSelWnd(Conn, gameNames, npcName)
	if IsCppBound(g_MainPlayer) then
		g_GameMain.m_MatchGameSelWnd:Init(gameNames, npcName)
	end
end

function Gas2Gac:SetDungeonCount(Conn, ActionName, Counts)
	CAreaFbSelWndNew.SetDungeonCount(Conn, ActionName, Counts)
end

function Gas2Gac:RetShowDungeonInfoWnd(Conn, ActionName, Counts)
	CAreaFbSelWndNew.RetShowDungeonInfoWnd(Conn, ActionName, Counts)
end

----------- 宝石用RPC  ---------------
function Gas2Gac:ReturnGetOpenedHole(Conn, frameID, position, holeID)--已打孔的
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnReturnGetOpenedHole(Conn, frameID, position, holeID)
end
function Gas2Gac:ReturnGetInlayedHole(Conn, frameID, position, holePos ,stoneBigID, stonename)--已镶嵌的孔
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnReturnGetInlayedHole(Conn, frameID, position, holePos ,stoneBigID, stonename)
end
function Gas2Gac:ReturnOpenHole(Conn, flag)--打孔的返回
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnReturnOpenHole(Conn, flag)
end
function Gas2Gac:ReturnInlayStone(Conn,flag)--镶嵌的返回
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnReturnInlayStone(Conn, flag)
end
function Gas2Gac:ReturnRemovalStone(Conn,flag)--宝石摘除返回
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnReturnRemovalStone(Conn,flag)
end
--打孔,镶嵌,摘除End ,需要更新 表HoleTbl,StoneTbl
function Gas2Gac:UpdateAllHoleInfoBegin(Conn)  --考虑异步问题
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnUpdateAllHoleInfoBegin(Conn)
end
function Gas2Gac:SendAllHoleInfoEnd(Conn)--初始化结束
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnSendAllHoleInfoEnd(Conn)
end
function Gas2Gac:NotifyAddAttr(Conn, frameid, position, holePos)--镶嵌满4, 8 个时 第4, 第8个孔的宝石获得附加属性 增加m%,n%
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnNotifyAddAttr(Conn, frameid, position, holePos)
end
function Gas2Gac:NotifyOpenPanel(Conn,num)-- 等级满足条件 开启后续面板
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnNotifyOpenPanel(Conn,num)
end
function Gas2Gac:UsePanel(Conn,PanelID)-- 通知正在使用哪个面板
	local StoneWnd = CStone.GetWnd()
	StoneWnd:OnUsePanel(Conn,PanelID)
end

function Gas2Gac:ReturnSynthesisItem(Conn,ItemId,Type,Name)
	local Wnd = CStoneCompound.GetWnd()
	Wnd:OnReturnSynthesisItem(Conn,ItemId,Type,Name)
end
function Gas2Gac:ReturnSynthesisItemEnd(Conn, flag)
	local Wnd = CStoneCompound.GetWnd()
	Wnd:OnReturnSynthesisItemEnd(Conn, flag)
end

--------------白宝石鉴定Rpc -----------------
function Gas2Gac:RetWhiteStoneAppraise(Conn, stoneBigID, stoneName, stoneID, nTimes)
	--鉴定有一定的几率失败
	local WhiteStone = CWhiteStone.GetWnd()
	WhiteStone:RetWhiteStoneAppraise(stoneBigID,stoneName,stoneID,nTimes)
	--SetEvent( Event.Test.RetTakeAppraisedStone,true,flag)
end
function Gas2Gac:RetWhiteStoneAppraiseFirstTime(Conn,stoneBigID,stoneName)
	local WhiteStone = CWhiteStone.GetWnd()
	WhiteStone:RetWhiteStoneAppraiseFirstTime(stoneBigID,stoneName)
end
function Gas2Gac:RetTakeAppraisedStone(Conn,flag)
	local WhiteStone = CWhiteStone.GetWnd()
	WhiteStone:RetTakeAppraisedStone(flag)
end

function Gas2Gac:InitPkSwitch(Conn, PkSwitchValue)
	if IsCppBound(g_MainPlayer) then
		g_MainPlayer.m_SwitchState = PkSwitchValue
	end	
	g_GameMain.m_PkSwitchWnd:ShowPkWnd()
end

function Gas2Gac:TransferSwitchState(Conn, PkSwitchValue)
	if IsCppBound(g_MainPlayer) then
		g_MainPlayer.m_SwitchState = PkSwitchValue
	end	
	g_GameMain.m_PkSwitchWnd:IsShowWnd()
end


--更新头顶信息
function Gas2Gac:UpdatePkSwitchState(Conn)
	if IsCppBound(g_MainPlayer) then
		g_GameMain.m_PkSwitchWnd:IsShowWnd()
		g_GameMain.m_CharacterInSyncMgr:PlayerHeadInfoInit() --主角头顶信息
	end
end

function Gas2Gac:CreateWarTransprot(Conn)
	if not g_GameMain.m_WarTransportWnd then
		g_GameMain.m_WarTransportWnd = CWarTransportWnd:new(g_GameMain)
	end
	g_GameMain.m_WarTransportWnd:Open()
end

--返回包裹中装备耐久数值
function Gas2Gac:RetEquipDuraValueInBag(Conn, equipID, roomIndex, pos, curDuraValue, MaxDuraValue)
    g_GameMain.m_EquipDuraWnd.RetEquipDuraValueInBag(equipID, roomIndex, pos, curDuraValue, MaxDuraValue)
end
function Gas2Gac:RetGetForageAutoInfo(Conn,sceneName,posx,posy)
	PlayerAutoTrack("",sceneName,posx,posy)
end

--是否显示小地图边上的耐久小人
function Gas2Gac:RetShowEquipDuraWnd(Conn, bShowFlag)
    g_GameMain.m_EquipDuraWnd.RetShowEquipDuraWnd(bShowFlag)
end

--刷新装备修理列表(角色)
function Gas2Gac:RefreshListCtrlInRole(Conn)
	g_GameMain.m_CEquipInRoleWnd.RefreshListCtrlInRole()
end

function Gas2Gac:RefreshListCtrlInBag(Conn)
	g_GameMain.m_CEquipInBagWnd.RefreshListCtrlInBag()
end

--返回装备耐久值
function Gas2Gac:RetEquipDuraValue(Conn, equipID, equipPart, duraState, curDuraValue, maxDuraValue)
    g_GameMain.m_EquipDuraWnd.RetEquipDuraValue(equipID, equipPart, duraState, curDuraValue, maxDuraValue)
end


--修复所有装备
function Gas2Gac:RenewAllEquipEnd(Conn)
    g_GameMain.m_CEquipInBagWnd:RenewAllEquipEnd()
end

function Gas2Gac:RetRenewEquipSuc(Conn, equipPart, equipType, equipName, equipID)
    g_GameMain.m_CEquipInRoleWnd:RetRenewEquipSuc(equipPart, equipType, equipName, equipID)
end

------------------------------------------------------------------------------------
--需求分配模式
function Gas2Gac:ShowNeedAssignWnd(Conn, BigId, ItemName, ItemCount, ServerId, IntObjID, Class, Time)
	g_GameMain.m_NeedAssignTbls[IntObjID] = CNeedAssignWnd.CreateNeedAssignWnd(g_GameMain, BigId, ItemName, ItemCount, ServerId, IntObjID, Class, Time)
end

function Gas2Gac:CloseNeedAssignWnd(Conn, IntObjID)
	if g_GameMain.m_NeedAssignTbls[IntObjID] then
		g_GameMain.m_NeedAssignTbls[IntObjID]:Destroy()
	end
end

--拍卖分配模式
function Gas2Gac:ShowAcutionAssignWnd(Conn, BigId, ItemName, ItemCount, ServerId, IntObjID, Time)
	g_GameMain.m_AcutionAssignTbls[IntObjID] = CAuctionAssignWnd.CreateAuctionPickUpWnd(g_GameMain, BigId, ItemName, ItemCount, ServerId, IntObjID, Time)
end

function Gas2Gac:FinishAuction(Conn, IntObjID)
	if g_GameMain.m_AcutionAssignTbls[IntObjID] then
		g_GameMain.m_AcutionAssignTbls[IntObjID]:Destroy()
	end
end

function Gas2Gac:NotifyChangeAuctionPrice(Conn, NewPrice, IntObjID)
	if g_GameMain.m_AcutionAssignTbls[IntObjID] then
		g_GameMain.m_AcutionAssignTbls[IntObjID]:NotifyChangeAuctionPrice(NewPrice)
	end
end

--插旗挑战rpc
--确认挑战
function Gas2Gac:PropChallengeMessage(Conn,EntityID,TargetName)
	CShowMessageWnd.CancelChallengeWnd(Conn, true, 2)
	g_GameMain.m_SureMessage = CShowMessageWnd.CreateShowMessageWnd(g_GameMain,EntityID,TargetName)
end

function Gas2Gac:PropChallengeAdvertMessage(Conn,EntityID,TargetName,PlayerLevel,minLevel,maxLevel)
	CShowMessageWnd.CancelChallengeWnd(Conn, true, 2)
	g_GameMain.m_SureMessage = CShowMessageWnd.CreateShowMessageWnd(g_GameMain,EntityID,TargetName,PlayerLevel,minLevel,maxLevel)
end

function Gas2Gac:SetChallengeFlag(Conn, bValue)
	g_MainPlayer.m_ChallengeFlag = bValue
	CShowMessageWnd.CancelChallengeWnd(Conn, bValue, 1)
end

function Gas2Gac:CancelChallengeWnd(Conn, bValue)
	CShowMessageWnd.CancelChallengeWnd(Conn, bValue, 2)
end

--发起挑战提示
function Gas2Gac:ShowChallengeMessage(Conn,EntityID,name)
	CShowMessageWnd.ShowChallengeMessage(Conn,EntityID,name)
end

function Gas2Gac:ShowChallengeAdvertMessage(Conn,EntityID,name,Level,MinLevel,MaxLevel)
	CShowMessageWnd.ShowChallengeMessage(Conn,EntityID,name,Level,MinLevel,MaxLevel)
end

function Gas2Gac:SetTickCompassHeadDir(Conn,Type,PosX,PosY,ShowTime)
	g_GameMain.m_ChallengeCompass:SetTickCompassHeadDir(Type,PosX,PosY,ShowTime)
end

function Gas2Gac:ClearCompassHeadDir(Conn)
	g_GameMain.m_ChallengeCompass:CancelCompassState()
end

function Gas2Gac:SetCompassHeadDir(Conn,Type,PosX,PosY)
	g_GameMain.m_ChallengeCompass:CancelCompassState()
	g_GameMain.m_ChallengeCompass:SetCompassHeadDir(Type,PosX,PosY)
end

----------------------连续击杀界面显示函数-----------------------
function Gas2Gac:UpdateComboHitsTimes(Conn, ComboHitsTime)
	CComboHitsWnd.ShowComboHitsWnd(ComboHitsTime)
end
------------------------------------------------------------------

--读条窗口
function Gas2Gac:NotifyLoadProgressBar(Conn, time, IsPerfectColl, ActionName)
	CProgressBarWnd.NotifyLoadProgressBar(Conn, time, IsPerfectColl, ActionName)
end

function Gas2Gac:NotifyStopProgressBar(Conn)
	CProgressBarWnd.NotifyStopProgressBar(Conn)
end

function Gas2Gac:ResourceLoadProgress(Conn, time, ActionName)
	g_GameMain.m_ResourceProgressWnd:LoadProgressByTime(time, ActionName)
end

function Gas2Gac:NotifyUseItemProgressLoad(Conn, time, ActionName)
	CUseItemProgressWnd.NotifyUseItemProgressLoad(Conn, time, ActionName)
end

function Gas2Gac:NotifyUseItemProgressStop(Conn)
	CUseItemProgressWnd.NotifyUseItemProgressStop(Conn)
end

--沙漏计时
function Gas2Gac:NotifySandGlassLoad(Conn, time)
	g_GameMain.m_SandGlassWnd:LoadProgressByTime(time)
end

function Gas2Gac:NotifySandGlassClose(Conn)
	g_GameMain.m_SandGlassWnd:ShowWnd(false)
end

--非战斗行为蓄力
function Gas2Gac:OpenXLProgressWnd(Conn,IntObjID)
	CXLProgressWnd.OpenXLProgressWnd(Conn,IntObjID)
end

function Gas2Gac:CloseXLProgressWnd(Conn)
	CXLProgressWnd.CloseXLProgressWnd(Conn)
end

--------------------------------------------------------------------------
--复活
function Gas2Gac:CloseRebornMsgWnd(Conn)
	CWndDeadMsg.CloseRebornMsgWnd(Conn)
end

function Gas2Gac:HideFBDeadMsgWnd(Conn)
	g_GameMain.m_FBDeadMsgBox:HideWnd()
end
--------------------------------矿洞-------------------------------------------
--矿藏图使用提示
function Gas2Gac:OreMapInfoShow(Conn, SceneName, x, y)
	COreSceneMSG.OreMapInfoShow(SceneName, x, y)
end

function Gas2Gac:JoinAreaSceneMsgBox(Conn, SceneId, serverId, SceneName, SceneLevel)
	COreSceneMSG.JoinAreaSceneMsgBox(SceneId, serverId, SceneName, SceneLevel)
end

--是否进入副本提示(主)
function Gas2Gac:CreateAreaSceneMsgBox(Conn, SceneName)
	COreSceneMSG.CreateAreaSceneMsgBox(SceneName)
end

function Gas2Gac:TeamJoinSceneMsgBox(Conn, SceneId, serverId, SceneName, SceneLevel)
	COreSceneMSG.TeamJoinSceneMsgBox(SceneId, serverId, SceneName, SceneLevel)
end

function Gas2Gac:IsExitSceneMsgBox(Conn, IsShow)
	COreSceneMSG.IsExitSceneMsgBox(IsShow)
end

--是否接受邀请进入副本提示
function Gas2Gac:InviteJoinSceneMsgBox(Conn, SceneId, ServerId, SceneName, PlayerName, SceneType, OtherParam)
	COreSceneMSG.InviteJoinSceneMsgBox(SceneId, ServerId, SceneName, PlayerName, SceneType, OtherParam)
end

--------------------------------花卉----------------------------------------
--更新花卉状态(健康,长虫等)
function Gas2Gac:UpdateFlowerState(Conn, State)
	g_GameMain.m_LiveSkillFlower:UpdateFlowerState(State)
end

--更新花卉健康值
function Gas2Gac:UpdateFlowerHealthPoint(Conn, HealthPoint)
	g_GameMain.m_LiveSkillFlower:UpdateFlowerHealthPoint(HealthPoint, false)
end

--增加花卉健康值
function Gas2Gac:AddFlowerHealthPoint(Conn, HealthPoint)
	g_GameMain.m_LiveSkillFlower:UpdateFlowerHealthPoint(HealthPoint, true)
end

--更新花卉收获次数
function Gas2Gac:UpdateFlowerGetCount(Conn, GetCount)
	g_GameMain.m_LiveSkillFlower:UpdateFlowerGetCount(GetCount)
end

--花卉开始成长或继续成长
function Gas2Gac:CultivateFlowerStart(Conn, FlowerName, leftTime)
	g_GameMain.m_LiveSkillFlower:StartGrowth(FlowerName, leftTime)
end

function Gas2Gac:FlowerEnd(Conn)
	g_GameMain.m_LiveSkillFlower:FlowerOnDead()
end

--花卉技能使用控制(冷却中不能使用)
function Gas2Gac:CultivateFlowerSkillBtnEnable(Conn, time)
	g_GameMain.m_LiveSkillFlower:RegisterCooldown(time)
end

function Gas2Gac:PlayFlowerSkillEffect(Conn, index)
	g_GameMain.m_LiveSkillFlower:AddEffect(g_GameMain.m_LiveSkillFlower.m_Flower, CultivateFlowerCleanStateEfx[index])
end

function Gas2Gac:FlowerGiveItemCoolDownCD(Conn)
	g_GameMain.m_LiveSkillFlower:FlowerGiveItemCoolDownCD()
end

function Gas2Gac:ShowGatherItemsWnd(Conn, BigId, ItemName, ItemCount, MsgId, Arg)
	CShowGatherItemsWnd.ShowGatherItemsWnd(Conn, BigId, ItemName, ItemCount, MsgId, Arg)
end
---------------------------------------------------------------------------
--OBJ采集面板
function Gas2Gac:RetCanCollItem(Conn,ItemType,ItemName,ItemNum)
	CCollObjShowWnd.RetCanCollItem(Conn,ItemType,ItemName,ItemNum)
end

function Gas2Gac:RetCanCollItemEnd(Conn,GlobalID,PerfectType)
	CCollObjShowWnd.RetCanCollItemEnd(Conn,GlobalID,PerfectType)
end

function Gas2Gac:RetCollAllItemGrid(Conn,GetIndex)
	CCollObjShowWnd.RetCollAllItemGrid(Conn,GetIndex)
end

function Gas2Gac:NotifyCloseCollWnd(Conn)
	CCollObjShowWnd.NotifyCloseCollWnd(Conn)
end

function Gas2Gac:RetCollGetGridItem(Conn,GetIndex,Succ)
	CCollObjShowWnd.RetCollGetGridItem(Conn,GetIndex,Succ)
end

function Gas2Gac:RetSelectAllGridItems(Conn,Succ)
	CCollObjShowWnd.RetSelectAllGridItems(Conn,Succ)
end
---------------------------------------------------------------------------
--**************************************************************************************
--佣兵小队
--**************************************************************************************
function Gas2Gac:OnRequestJoinInTong(Conn)
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateTongReqMsgMinimum(5020)
end
	
function Gas2Gac:ReturnPreChangeTongName(Conn)
	local TongChangeName = CTongChangeNameWnd:GetWnd(g_GameMain)
	TongChangeName:OpenPanel()
end

function Gas2Gac:ShowFoundTong(Conn,sName)
	local tbl = g_GameMain.m_TongRequest.m_AllTongInfoTbl
	local uListCountOnePage = 14
	local nowPage = 0
	for i=1, #tbl do
		if tbl[i].sName == sName then
			if i%uListCountOnePage >0 then
				nowPage = i/uListCountOnePage 
			elseif i%uListCountOnePage == 0 then
				nowPage = i/uListCountOnePage - 1
			end
			g_GameMain.m_TongRequest:TurntoPageBySearch(math.floor(nowPage), i)
		end
	end	
end

function Gas2Gac:ShowFoundPlayer(Conn,sName)
	local tbl = g_GameMain.m_TongRecruit.m_AllPlayerInfoTbl
	local uListCountOnePage = 10
	local nowPage = 0
	for i=1, #tbl do
		if tbl[i].sName == sName then
			if i%uListCountOnePage >0 then
				nowPage = i/uListCountOnePage
			elseif i%uListCountOnePage == 0 then
				nowPage = i/uListCountOnePage - 1
			end
			g_GameMain.m_TongRecruit:TurntoPageBySearch(math.floor(nowPage), i)
		end
	end		
end

------返回佣兵小队日志------
function Gas2Gac:ReturnGetTongLog(Conn, iData, iType, sDes)
	g_GameMain.m_TongMainPan.m_tblPanel[5]:ReturnGetTongLog(iData, iType, sDes)
end

function Gas2Gac:ReturnGetTongLogEnd(Conn)
	g_GameMain.m_TongMainPan.m_tblPanel[5]:ChangePage(0)
end

------返回佣兵小队战斗科技------
function Gas2Gac:ReturnGetTongFightScienceInfoBegin(Conn, nTongLevel, nTongLine)
	g_GameMain.m_TongScience:ReturnGetTongFightScienceInfoBegin(nTongLevel, nTongLine)
end

function Gas2Gac:ReturnGetTongFightScienceInfo(Conn, sName, nLevel, nMark, nPassedTime)
	g_GameMain.m_TongScience:InsertInfo(sName, nLevel, nMark, nPassedTime)
	g_GameMain.m_TongLearnScience:UpdateTongScienceInfo(sName,nLevel)
end

function Gas2Gac:ReturnGetTongFightScienceInfoEnd(Conn)
	g_GameMain.m_TongScience:UpdateInfo()
end

function Gas2Gac:ReturnTongContributeMoneyInfo(Conn, eIndex,max_money,sum_money)
	CTongContributeMoneyWnd.GetWnd():OpenPanel(eIndex,max_money,sum_money)
end

function Gas2Gac:ReturnContributeMoney(Conn)
	CTongContributeMoneyWnd.GetWnd():ShowWnd(false)
end

function Gas2Gac:RetCharLearnedTech(Conn,TechName,TechLevel)
	g_GameMain.m_TongLearnScience:UpdatePlayerScienceInfo(TechName,TechLevel)
end

function  Gas2Gac:ShowNoTongNoneAwardWnd(Conn)
	local function WarBattleOver(GameWnd, uButton)
		return true
	end
	MessageBox(g_GameMain, GetStaticTextClient(9133), MB_BtnOK, WarBattleOver, g_GameMain)
end

function Gas2Gac:ShowNoneAwardWnd(Conn)
	local function WarBattleOver(GameWnd, uButton)
		return true
	end
	MessageBox(g_GameMain, GetStaticTextClient(9132), MB_BtnOK, WarBattleOver, g_GameMain)	
end

function  Gas2Gac:ShowNoPlayerGetAwardWnd(Conn)
	local function WarBattleOver(GameWnd, uButton)
		return true
	end
	MessageBox(g_GameMain, GetStaticTextClient(9134), MB_BtnOK, WarBattleOver, g_GameMain)
end

--上面是佣兵小队的函数
--**************************************************************************************

function Gas2Gac:ExposePlayer(Conn, EntityID)
	g_GameMain.m_HideFollowerMgr:ExposePlayer(EntityID)
end

--**************************************************************************************
--修行塔
--**************************************************************************************
function Gas2Gac:ShowMercCard(Conn, CardId)
	g_GameMain.m_YbMercCardBagWnd:ShowMercCard(CardId)
end

function Gas2Gac:InitMercCard(Conn)
	g_GameMain.m_YbMercCardBagWnd:InitMercCard(Conn)
end

function Gas2Gac:RecordMercCard(Conn,i)
	g_GameMain.m_YbMercCardBagWnd:RecordMercCard(Conn,i)
end

function Gas2Gac:RetShowYbEducateActInfoWnd(Conn, RulesText, DownTime, NowGameID)
	CYbEducateActionWnd:RetShowYbEducateActInfoWnd(Conn, RulesText, DownTime, NowGameID)
end

function Gas2Gac:RetRecordGameInfo(Conn, RulesText, NowGameID)
	CYbEducateActionWnd:RetRecordGameInfo(Conn, RulesText, NowGameID)
end

function Gas2Gac:RetCloseYbEducateActInfoWnd(Conn)
	CYbEducateActionWnd:RetCloseYbEducateActInfoWnd(Conn)
end

-- MusicType 为1是奖励关音乐，2是普通关音乐，3是等待时候音乐
function Gas2Gac:SendPlayXiuXingTaBgm(Conn, MusicType)
	CYbEducateActionWnd:SendPlayXiuXingTaBgm(Conn, MusicType)
end

function Gas2Gac:RetStopXiuXingTaBgm(Conn)
	CYbEducateActionWnd:RetStopXiuXingTaBgm(Conn)
end

--右边活动说明
function Gas2Gac:RetShowSmallActInfoWnd(Conn, NowGameID)
	CYbEducateActionWnd:RetShowSmallActInfoWnd(Conn, NowGameID)
end

-- 上线后恢复数字
function Gas2Gac:SendResumeNum(Conn, IsOver, NameLocal, Num, GameID)
 CYbEducateActionWnd:SendResumeNum(Conn, IsOver, NameLocal, Num, GameID)
end

function Gas2Gac:RetUpDateSmallInfo(Conn)
	CYbEducateActionWnd:RetUpDateSmallInfo(Conn)
end

--系统区的消息提示内容
function Gas2Gac:RetYbEducateActShowMsg(Connection, GameID, NameLocal, Num, CountNum)
	CYbEducateActionWnd:RetYbEducateActShowMsg(Connection, GameID, NameLocal, Num, CountNum)
end

function Gas2Gac:RetYbEducateActShowLoseGameMsg(Connection, GameID, NameLocal, Num, CountNum)
	CYbEducateActionWnd:RetYbEducateActShowLoseGameMsg(Connection, GameID, NameLocal, Num, CountNum)
end

function Gas2Gac:ReturnAddMoneySlowly(Connection, AddMoney, DividNum)
	CYbEducateActionWnd:ReturnAddMoneySlowly(Connection, AddMoney, DividNum)
end

function Gas2Gac:ReturnModifyPlayerSoulNumSlowly(Connection, SoulNum, AddSoulNum, DividNum)	
	CYbEducateActionWnd:ReturnModifyPlayerSoulNumSlowly(Connection, SoulNum, AddSoulNum, DividNum)	
end

function Gas2Gac:RetShowYbEducateActionWnd(Conn)
	CYbEducateActionWnd:RetShowYbEducateActionWnd(Conn)
end

-- 修行塔结束后显示给奖励的窗口
function Gas2Gac:RetOverYbXlAction(Conn, Type, ItemName, ItemNum)
	CYbEducateActionWnd:RetOverYbXlAction(Conn, Type, ItemName, ItemNum)
end

function Gas2Gac:RetIsContinueYbXlAction(Conn, LifeNum, Is15Floor)
	CYbEducateActionWnd:RetIsContinueYbXlAction(Conn, LifeNum, Is15Floor)
end

--**************************************************************************************
--活动提示面板
--**************************************************************************************
function Gas2Gac:SetActionCount(Conn, ActionName, Count1, Count2)
	-- 记录活动次数
	CFbActionPanelWnd.SetActionCount(Conn, ActionName, Count1, Count2)
end

function Gas2Gac:RetShowActionInfoWnd(Conn, Hour, Min)
	-- 打开窗口
	CFbActionPanelWnd.RetShowActionInfoWnd(Conn, Hour, Min)
end

function Gas2Gac:AddActionNum(Conn, activityName, AddNum)
	CFbActionPanelWnd.AddActionNum(Conn, activityName, AddNum)
end

function Gas2Gac:ReturnGetActionCountExBegin(Conn)
	g_GameMain.m_GuideData:ReturnGetActionCountExBegin()
end

function Gas2Gac:ReturnGetActionCountEx(Conn, ActionName, Count1, Count2)
	g_GameMain.m_GuideData:ReturnGetActionCountEx(ActionName, Count1, Count2)
end

function Gas2Gac:ReturnGetActionCountExEnd(Conn)
	g_GameMain.m_GuideData:ReturnGetActionCountExEnd()
end

function Gas2Gac:ReturnGetActionAllTimes(Conn, sName, nTime)
	g_GameMain.m_GuideData:ReturnGetActionAllTimes(sName, nTime)
end

function Gas2Gac:SetActionPanelWarnValue(Conn, WarnValue)
	g_GameMain.m_FbActionPanel:SetWarnValue(WarnValue)
end
--**************************************************************************************
--中地图
--**************************************************************************************
function Gas2Gac:AreaPkOnMiddleMap(Conn, SceneName, AreaName, AreaState, Time, EternalState)
	g_GameMain.m_AreaInfoWnd:AreaPkOnMiddleMap(Conn, SceneName, AreaName, AreaState, Time, EternalState)
end


function Gas2Gac:RobResWinOnMiddleMap(Conn, sceneName, objName, tongName, tongCamp)
	g_GameMain.m_AreaInfoWnd:RobResWinOnMiddleMap(Conn, sceneName, objName, tongName, tongCamp)
end

function Gas2Gac:AreaPkOnMiddleMap(Conn, SceneName, AreaName, AreaState, Time, EternalState)
	g_GameMain.m_AreaInfoWnd:AreaPkOnMiddleMap(Conn, SceneName, AreaName, AreaState, Time, EternalState)
end

function Gas2Gac:CheckIsNeedSendTeammatePos(Conn)
	g_GameMain.m_AreaInfoWnd:CheckIsNeedSendTeammatePos(Conn)
end

function Gas2Gac:CheckIsNeedSendTongmatePos(Conn, TongID)
	g_GameMain.m_AreaInfoWnd:CheckIsNeedSendTongmatePos(Conn, TongID)
end

function Gas2Gac:StopSendTeammatePos(Conn)
	g_GameMain.m_AreaInfoWnd:DeleteTeammatePosWnd()
end

function Gas2Gac:StopSendTongmatePos(Conn)
	g_GameMain.m_AreaInfoWnd:DeleteTongmatePosWnd()
end

function Gas2Gac:StopSendFbPlayerPos(Conn)
	g_GameMain.m_AreaInfoWnd:DeleteFbPlayerPos()
end

function Gas2Gac:HideLeavedTongmatePos(Conn, PlayerID)
	g_GameMain.m_AreaInfoWnd:HideLeavedTongmatePos(Conn, PlayerID)
end

function Gas2Gac:HideLeavedTeammatePos(Conn, PlayerID)
	g_GameMain.m_AreaInfoWnd:HideLeavedTeammatePos(Conn, PlayerID)
end

function Gas2Gac:HideLeftFbPlayerPos(Conn, PlayerID)
	g_GameMain.m_AreaInfoWnd:HideLeftFbPlayerPos(Conn, PlayerID)
end

function Gas2Gac:SetMatchGameNpcTips(Conn, PointName, GameNames)
	g_GameMain.m_AreaInfoWnd:SetMatchGameNpcTips(Conn, PointName, GameNames)
end

function Gas2Gac:RetPlayerAreaInfo(Conn, AreaName, IsDiscovery, QuestCount)
	g_GameMain.m_AreaInfoWnd:RetPlayerAreaInfo(Conn, AreaName, IsDiscovery, QuestCount)
end

function Gas2Gac:RetFirstTimeIntoArea(Conn, AreaName, OldState, QuestCount)
	g_GameMain.m_AreaInfoWnd:RetFirstTimeIntoArea(Conn, AreaName, OldState, QuestCount)
end

function Gas2Gac:RetPlayerSceneAreaInfo(Conn,SceneName)
	g_GameMain.m_AreaInfoWnd:RetPlayerSceneAreaInfo(Conn,SceneName)
end

function Gas2Gac:SendTeammatePos(Conn,MemberId,SceneName,Gridx,Gridy)
	g_GameMain.m_AreaInfoWnd:SendTeammatePos(Conn,MemberId,SceneName,Gridx,Gridy)
end

function Gas2Gac:SendTongmatePos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName)
	g_GameMain.m_AreaInfoWnd:SendTongmatePos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName)
end

function Gas2Gac:SendFbPlayerPos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName, TeamID, isLive)
	g_GameMain.m_AreaInfoWnd:SendFbPlayerPos(Conn, MemberId, SceneName, Gridx, Gridy, MemberName, TeamID, isLive)
end

function Gas2Gac:RetShowWarZoneState(Conn, stationID, campID, tongName)
	g_GameMain.m_AreaInfoWnd:SaveWarZoneState(stationID, campID, tongName)
end

function Gas2Gac:EndSendWarZoneState(Conn)
	g_GameMain.m_AreaInfoWnd:EndSendWarZoneState()
end

function Gas2Gac:UpdateWarZoneState(Conn, stationID, campID, tongName)
	g_GameMain.m_AreaInfoWnd:UpdateWarZoneState(Conn, stationID, campID, tongName)
end

function Gas2Gac:BornNpcOnMiddleMap(Conn, SceneName, NpcName, PosX, PosY)
	g_GameMain.m_AreaInfoWnd:BornNpcOnMiddleMap(Conn, SceneName, NpcName, PosX, PosY)
end

function Gas2Gac:DeadNpcOnMiddleMap(Conn, SceneName, NpcName)
	g_GameMain.m_AreaInfoWnd:DeadNpcOnMiddleMap(Conn, SceneName, NpcName)
end
--
--**************************************************************************************
function Gas2Gac:ExposePlayer(Conn, EntityID)
	g_GameMain.m_HideFollowerMgr:ExposePlayer(EntityID)
end

---------------------------------------------------------------------------------
--装备精灵重生返回
function Gas2Gac:AdvancedEquipReborn(Conn,ItemID)
	CEquipUpIntensify.AdvancedEquipReborn(ItemID)
end

--注强化魂失败返回
function Gas2Gac:NoticePourIntensifyError(Conn, uItemId, uSoulNum)
	CEquipUpIntensify.NoticePourIntensifyError(uItemId, uSoulNum)	
end

--注强化魂成功返回
function Gas2Gac:NoticePourIntensifySuccess(Conn,uItemId)
	CEquipUpIntensify.NoticePourIntensifySuccess(uItemId)	
end

--服务器进阶成功返回
function Gas2Gac:NoticeAdvanceSuccess(Conn,uItemId)
	CEquipUpIntensify.NoticeAdvanceSuccess(uItemId)
end

--服务器进阶出错返回
function Gas2Gac:NoticeAdvanceError(Conn, uItemId)
	CEquipUpIntensify.NoticeAdvanceError(uItemId)
end

--服务器端强化成功返回
function Gas2Gac:NoticeIntensifySuccess(Conn,uItemId, equipPart)
	CEquipUpIntensify.NoticeIntensifySuccess(uItemId, equipPart)
end

function Gas2Gac:RetEquipIntenBack(Conn, equipRoom, equipPos, equipPart, equipID)
	CEquipUpIntensify.RetEquipIntenBack(equipRoom, equipPos, equipPart, equipID)
end

--更新强化面板信息。
function Gas2Gac:UpdateEquipIntensifyInfo(Conn,uItemId)
	CEquipUpIntensify.UpdateEquipIntensifyInfo(uItemId)	
end

--将魂值添加到装备上成功后，server的返回接口
function Gas2Gac:NoticeAbsorbSoul(Conn, uNum, nItemId)
	CEquipUpIntensify.NoticeAbsorbSoul(uNum, nItemId)
end

function Gas2Gac:ReturnModifyPlayerSoulNum(Conn,total_soul)
	CEquipUpIntensify.ReturnModifyPlayerSoulNum(total_soul)
end

function Gas2Gac:RetActiveJingLingSkillPiece(Conn,ItemID,ActvieIndex,EquipPart)
	CEquipUpIntensify.RetActiveJingLingSkillPiece(ItemID,ActvieIndex,EquipPart)
end
---------------------------------------------------------------------------------

--拒绝所有交易请求，隐藏交易请求btn
function Gas2Gac:ConcealOrNotPlayersTradeRequest(Conn, showOrNot)
    g_GameMain.m_PlayersTradeRequestWndInGameMain:ShowWnd(showOrNot)
end

--接收到其他玩家发送的交易请求开始，初始化交易请求列表
function Gas2Gac:GetPlayersTradeRequestBegin(Conn)
	g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl = {}
	g_GameMain.m_PlayersTradeWnd.PlayersTradeRequestItem = {}
end

function Gas2Gac:GetPlayersTradeRequest(Conn, invitorID, invitorName)
	local tbl = g_GameMain.m_PlayersTradeWnd.m_TradeRequestTbl
	table.insert(tbl, {invitorID, invitorName})
end

function Gas2Gac:GetPlayersTradeRequestEnd(Conn)
    CPlayersTradeWnd.GetPlayersTradeRequestEnd()
end

function Gas2Gac:RetGetTradeCess(Conn,recieverid,money,cess)
    CPlayersTradeWnd.RetGetTradeCess(Conn,recieverid,money,cess)
end

function Gas2Gac:ReturnAgreeToTrade( Conn, InvitorID ) 
    CPlayersTradeWnd.ReturnAgreeToTrade(InvitorID ) 
end

function Gas2Gac:ReturnDenyedTrade( Conn ) --交易受邀者拒绝了交易请求
    CPlayersTradeWnd.ReturnDenyedTrade() 
end

function Gas2Gac:GotTradeInvitation( Conn, InvitorID, InvitorName )
    CPlayersTradeWnd.GotTradeInvitation( InvitorID, InvitorName )
end

function Gas2Gac:ReturnCanceledTrade( Conn )
    CPlayersTradeWnd.ReturnCanceledTrade( ) 
end

function Gas2Gac:ReturnChoosedTradeMoney( Conn, moneyCount ) --对方输入了交易金币数目
    CPlayersTradeWnd.ReturnChoosedTradeMoney(moneyCount)
end

function Gas2Gac:ReturnChoosedInviteeTradeMoney( Conn, moneyCount )
    CPlayersTradeWnd.ReturnChoosedInviteeTradeMoney(moneyCount)
end

function Gas2Gac:ReturnLockedTrade( Conn )
    CPlayersTradeWnd.ReturnLockedTrade(  )
end

function Gas2Gac:RetSubmitedTrade(Conn)
    CPlayersTradeWnd.RetSubmitedTrade()
end

function Gas2Gac:ReturnChoosedTradeItem( Conn, viewIndex, itemType, itemName, itemCount, itemID )
    CPlayersTradeWnd.ReturnChoosedTradeItem( viewIndex, itemType, itemName, itemCount, itemID )
end

function Gas2Gac:ReturnChoosedTradeItemError(Conn, roomIndex, pos )
    CPlayersTradeWnd.ReturnChoosedTradeItemError( roomIndex, pos )
end

function Gas2Gac:ReturnInviteeChoosedItem( Conn, viewIndex, itemType, itemName, itemCount, itemID )
    CPlayersTradeWnd.ReturnInviteeChoosedItem( viewIndex, itemType, itemName, itemCount, itemID )
end

function Gas2Gac:ReturnCancelChoosedItemSuc(Conn)
    CPlayersTradeWnd.ReturnCancelChoosedItemSuc()
end

function Gas2Gac:ReturnResetItemList(Conn, viewIndex, itemType, itemName, itemCount, itemID)
    CPlayersTradeWnd.ReturnResetItemList(viewIndex, itemType, itemName, itemCount, itemID)
end

function Gas2Gac:ReturnResetItemListEnd( Conn,  itemWndChoosed)
     CPlayersTradeWnd.ReturnResetItemListEnd( itemWndChoosed)
end

function Gas2Gac:ReturnReplaceItem( Conn, viewIndex, itemType, itemName, itemCount, itemID, roomIndex, pos, replacedRoomIndex, replacedPos )
    CPlayersTradeWnd.ReturnReplaceItem( viewIndex, itemType, itemName, itemCount, itemID, roomIndex, pos, replacedRoomIndex, replacedPos )
end

function Gas2Gac:ReturnReplaceItemError(Conn, roomIndex, pos)
    CPlayersTradeWnd.ReturnReplaceItemError( roomIndex, pos)
end

function Gas2Gac:ReturnTongProfferCount(Conn, tongProfferCount)
    CNPCShopSellWnd.ReturnTongProfferCount( tongProfferCount)
end

function Gas2Gac:ReturnTongLevel(Conn, tongLevel, stationLinLevel)
    CNPCShopSellWnd.ReturnTongLevel(tongLevel, stationLinLevel)
end

function Gas2Gac:UpdatePlayerPoint(Conn,jiFenType,modifiedJiFenCount)
    CNPCShopSellWnd.UpdatePlayerPoint(jiFenType,modifiedJiFenCount)
end

function Gas2Gac:ReturnPlayerSoldGoodsListBegin(Conn)
    CNPCShopSellWnd.ReturnPlayerSoldGoodsListBegin()
end 

function Gas2Gac:ReturnPlayerSoldGoodsList(Conn,sGoodsType,sGoodsName,nGoodsCount, nViewIndex, itemId,payType,uPosIndex)
    CNPCShopSellWnd.ReturnPlayerSoldGoodsList(sGoodsType,sGoodsName,nGoodsCount, nViewIndex, itemId,payType,uPosIndex)
end

function Gas2Gac:ReturnPlayerSoldGoodsListEnd(Conn)
    CNPCShopSellWnd.ReturnPlayerSoldGoodsListEnd()
end

function Gas2Gac:RetNpcTradeSucc (Conn)
    CNPCShopSellWnd.RetNpcTradeSucc ()
end

function Gas2Gac:RetCSMAddOrderSuc( Conn, wndNo, IsSuc )
    CConsignmentCommon.RetCSMAddOrderSuc( wndNo, IsSuc )
end

function Gas2Gac:RetCSMCancelOrderSucc( Conn, wndNo, IsSuc )
    CConsignmentCommon.RetCSMCancelOrderSucc( wndNo, IsSuc )
end

function Gas2Gac:RetGetCSMRememberPrice(Conn, pannelNo, price)
    CConsignmentCommon.RetGetCSMRememberPrice(pannelNo, price)
end

function Gas2Gac:RetCloseCSMWnd(Conn)
    CConsignmentCommon.RetCloseCSMWnd()
end

function Gas2Gac:RetCSMBuyOrderError( Conn, IsSuc )
    CConsignmentCommon.RetCSMBuyOrderError( IsSuc )
end

function Gas2Gac:RetCSMOrderListEnd( Conn, wndNo )
    CConsignmentCommon.RetCSMOrderListEnd( wndNo )
end

function Gas2Gac:RetCSMOrderList( Conn, wndNo, orderID, playerName, itemType, itemName, itemPrice, itemCount,  leftTime, itemID )
    CConsignmentCommon.RetCSMOrderList( wndNo, orderID, playerName, itemType, itemName, itemPrice, itemCount,  leftTime, itemID )
end

function Gas2Gac:RetCSMTotalNo( Conn, wndNo, itemTotalNo )
    CConsignmentCommon.RetCSMTotalNo( wndNo, itemTotalNo )
end

function Gas2Gac:RetCancelLeaveGame(Conn)
    CExitGame.RetCancelLeaveGame()
end

function Gas2Gac:ReceiveAntiIndulgenceExitGame(Conn)
	CExitGame.ReceiveAntiIndulgenceExitGame()
end

function Gas2Gac:ReturnNotifyClientLeave(Conn, uToState)--可以下线
    CExitGame.ReturnNotifyClientLeave(uToState)
end

function Gas2Gac:ChangeToStateNow(Conn, eToState)
    CExitGame.ChangeToStateNow( eToState)
end

function Gas2Gac:RetTakeYongBingBi(Conn, yongBingBiCount)
    CToolsMallMainWnd.RetTakeYongBingBi(yongBingBiCount)
end

function Gas2Gac:RetHotSaleItemBegin(Conn)
    g_GameMain.m_ToolsMallWnd.m_HotSaleItemInfoTblFromDB = {}-- 保存从数据库中查询关于热卖物品信息
end

--从服务端返回热卖的物品列表，type表明是礼包、时装、坐骑、药品、装备、宝石、书画、其他中的哪一种，值依次从1递增
--itemType是物品类别，itemName是物品名称
function Gas2Gac:RetHotSaleItem(Conn, type, itemType, itemName, nCount)
   g_GameMain.m_ToolsMallWnd.m_HotSaleItemInfoTblFromDB =  {itemType, nCount}
end

function Gas2Gac:RetHotSaleItemEnd(Conn, type)
    CToolsMallMainWnd.RetHotSaleItemEnd(type)
end


function Gas2Gac:ReturnCheckUser( Pipe,bFlag,uMsgID,userName)
    CLogin.ReturnCheckUser( Pipe,bFlag,uMsgID,userName)
end

function Gas2Gac:ReturnCheckUserByActivationCode( Pipe,randomPubKey1,randomPubKey2,randomPubKey3)
   CLogin.ReturnCheckUserByActivationCode( Pipe,randomPubKey1,randomPubKey2,randomPubKey3)
end

function Gas2Gac:ReturnCheckRCP( Pipe,nRet)
    CLogin.ReturnCheckRCP( nRet)
end

function Gas2Gac:RetReSetLogin(Pipe, IsShowMsg)
    CLogin.RetReSetLogin(IsShowMsg)
end

function Gas2Gac:ReturnPreChangePlayerName(Conn, bFlag)
    CPlayerChangeNameWnd.ReturnPreChangePlayerName(bFlag)
end

function Gas2Gac:ReturnChangePlayerName(Conn, bFlag, nMessageID)
    CPlayerChangeNameWnd.ReturnChangePlayerName(bFlag, nMessageID)
end

function Gas2Gac:ReturnCharListBegin(Connection, rushRegisterAccountFlag)
    CSelectCharState.ReturnCharListBegin(rushRegisterAccountFlag)
end

function Gas2Gac:ReturnCharList( Connection, uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp, sTongName, uTongPos, sArmyName, uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
    CSelectCharState.ReturnCharList( uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp, sTongName, uTongPos, sArmyName, uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
end

function Gas2Gac:ReturnDelCharList( Connection, uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp,uResTime ,uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
    CSelectCharState.ReturnDelCharList( uID, Name,uHair, uHairColor, uFace, fScale, uSex, uClass, uCamp,uResTime ,uLevel, uSceneID,  
								 uArmetResID, uBodyResID, uBackResID, uShoulderResID, uArmResID, uShoeResID, uWeaponID, uOffWeaponID,uArmetPhase,uBodyPhase,uArmPhase,uShoePhase,uShoulderPhase,uWeaponPhaseu,OffWeaponPhase)
end

function Gas2Gac:ReturnCharListEnd( Connection )
    CSelectCharState.ReturnCharListEnd()
end

function Gas2Gac:ReturnDelCharListEnd( Connection )
    CSelectCharState.ReturnDelCharListEnd()
end

function Gas2Gac:ReturnRecruitInfo(Connection, flag1, flag2, flag3)
	CSelectCamp.ReturnRecruitInfo(flag1, flag2, flag3)
end


function Gas2Gac:RetReturnRecruitInfo(Connection, flag1, flag2, flag3,returnFlag)
	CSelectCamp.RetReturnRecruitInfo(flag1, flag2, flag3,returnFlag)
end

function Gas2Gac:DelCharEnd( Connection )
    CSelectCharState.DelCharEnd()
end

function Gas2Gac:CompleteDelCharEnd( Connection )
    CSelectCharState.CompleteDelCharEnd()
end

function Gas2Gac:DelCharErr(Conn, msgID)
    CSelectCharState.DelCharErr(msgID)
end

function Gas2Gac:GetBackRoleEnd( Connection )
    CSelectCharState.GetBackRoleEnd()
end

function Gas2Gac:ReturnNameDiff(Connection,IsOK) --当前角色名与欲恢复的角色名一样，不能恢复
    CSelectCharState.ReturnNameDiff(IsOK)
end

function Gas2Gac:RetServerIpInfo(Connection, sKey, Camp, sIp, iPort)
    CSelectCharState.RetServerIpInfo(Connection, sKey, Camp, sIp, iPort)
end

function Gas2Gac:RetAccountSucc(Connection)
    CSelectCharState.RetAccountSucc()
end

function Gas2Gac:ReturnCreateRole(Pipe,nCode)
    CNewRoleEnterGame.ReturnCreateRole(nCode)
end

function Gas2Gac:RetNpcHeadInfo(Conn, EntityID)
	local Npc = CCharacterFollower_GetCharacterByID(EntityID)
	if not IsCppBound(Npc) then
		return
	end
	g_GameMain.m_NpcHeadSignMgr:ClearNpcHeadInfo(Npc)
end
-------------------------------------------------------------------------------------------
--返回玩家信息：玩家名称、玩家职业、玩家等级
function Gas2Gac:ReturnCharInfo(Conn,PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp)
	CEquipAssessWnd.SavePlayerInfoFun(PlayerID,PlayerName,PlayerClass,PlayerLevel,PlayerCamp)
end

--返回玩家装备信息
function Gas2Gac:ReturnFightingEvaluation(Conn,PlayerID,FightingEvaluation)
	FightingEvaluation	= string.format("%.2f",FightingEvaluation)
	CEquipAssessWnd.SaveFightingEvaluationFun(PlayerID,tonumber(FightingEvaluation))
end
-------------------------------------------------------------------------------------------
function Gas2Gac:ReturnGetTongMemberInfoStart(Conn)
	g_GameMain.m_TongBase:ReturnGetTongMemberInfoStart(Conn)
end
function Gas2Gac:ReturnGetTongMemberInfo(Conn,id,name,position,level,class,proffer,profferExpend,tongProffer,exploit,nOnline,outLineTime)
	g_GameMain.m_TongBase:ReturnGetTongMemberInfo(Conn,id,name,position,level,class,proffer,profferExpend,tongProffer,exploit,nOnline,outLineTime)
end
function Gas2Gac:ReturnGetTongMemberInfoEnd(Conn, maxNum)
	g_GameMain.m_TongBase:ReturnGetTongMemberInfoEnd(Conn, maxNum)
	CArmyCorpsPanel.GetWnd().m_tblPanel[3]:ShowArmyCorpsMemberInfoWnd(Conn, maxNum)
end
--------------------------------------------------------------------------------------------
--佣兵团
function Gas2Gac:OnArmyCorpsPosChanged(Conn,uPos)
	CArmyCorpsPanel.GetWnd():OnArmyCorpsPosChanged(uPos)
end

function Gas2Gac:ReturnCreateArmyCorps(Conn)
	CArmyCorpsPanel.GetWnd():OnCreateArmyCorps()
end

function Gas2Gac:ReturnArmyCorpsInfo( Conn,Name,Level,Money,Purpose,AdminName,MemberCount,Exploit,Honor )
	CArmyCorpsPanel.GetWnd():ReturnArmyCorpsInfo( Name,Level,Money,Purpose,AdminName,MemberCount,Exploit,Honor )
end

function Gas2Gac:OnArmyCorpsPurposeChanged( Conn, sPurpose )
	CArmyCorpsPanel.GetWnd():OnArmyCorpsPurposeChanged(sPurpose)
end

function Gas2Gac:ReturnPreChangeArmyCorpsName(Conn)
	local ArmyCorpsChangeName = CArmyCorpsChangeName:GetWnd(g_GameMain)
	ArmyCorpsChangeName:OpenPanel()
end

function Gas2Gac:OnInviteJoinArmyCorps( Conn, uInviterID, sInviterName, sArmyCorpsName )
	CArmyCorpsPanel.GetWnd():OnInviteJoinArmyCorps(uInviterID, sInviterName, sArmyCorpsName)
end

function Gas2Gac:ReturnArmyCorpsTeamInfoBegin(Conn,MemberCount,MemberCountLimit)
	CArmyCorpsPanel.GetWnd():ReturnArmyCorpsTeamInfoBegin(MemberCount,MemberCountLimit)
end
function Gas2Gac:ReturnArmyCorpsTeamInfoEnd(Conn)
	CArmyCorpsPanel.GetWnd():ReturnArmyCorpsTeamInfoEnd()
end
function Gas2Gac:ReturnArmyCorpsTeamInfo(Conn,tong_type,tong_name,leader_name,tong_level,member_num,tong_exploit,tong_id,warZoneId,stationId)
	CArmyCorpsPanel.GetWnd():ReturnArmyCorpsTeamInfo(tong_type,tong_name,leader_name,tong_level,member_num,tong_exploit,tong_id,warZoneId,stationId)
end
function Gas2Gac:OnLeaveArmyCorps(Conn,bSucc)
	CArmyCorpsPanel.GetWnd():OnLeaveArmyCorps(bSucc)
end
function Gas2Gac:OnKickOutOfArmyCorps(Conn,bSucc)
	CArmyCorpsPanel.GetWnd():OnKickOutOfArmyCorps(bSucc)
end
function Gas2Gac:OnBeKickOutOfArmyCorps(Conn)
	CArmyCorpsPanel.GetWnd():OnBeKickOutOfArmyCorps()
end

-------------------------------------------------------------------------------------------

-------------------------------------------------------------------------------------------

function Gas2Gac:PickOreStart(Conn)
	g_MainPlayer.m_PickOre = true
end

function Gas2Gac:PickOreEnd(Conn)
	g_MainPlayer:SetCurOnceActionLevel(0)
	g_MainPlayer.m_PickOre = false
	if g_MainPlayer.m_uDir then
		g_MainPlayer:TurnAndSyncDir(g_MainPlayer.m_uDir)
		g_MainPlayer.m_uDir = nil
	end
end

function Gas2Gac:PickOreActionEnd(Conn, EntityID)
	local Player = CCharacterFollower_GetCharacterByID(EntityID)
	if not IsCppBound(Player) then
		return
	end
	Player:SetCurOnceActionLevel(0)
end
-------------------------------------------------------------------------------------------


--------------------------------------------------------------------------------------
function Gas2Gac:RetCharCMOrderBegin(Conn)
    CContractManufactureOrderWnd.RetCharCMOrderBegin()
end

function Gas2Gac:RetCharContractManufactureOrder(Conn, cmOrderID, skillName, direction, prescripName, endTime, money)
    CContractManufactureOrderWnd.RetCharContractManufactureOrder(cmOrderID, skillName, direction, prescripName, endTime, money)
end

function Gas2Gac:RetCharCMOrderMaterialInfo(Conn, cmOrderID, index, materialType, materialName, materialCount)
    CContractManufactureOrderWnd.RetCharCMOrderMaterialInfo(cmOrderID, index, materialType, materialName, materialCount)
end

function Gas2Gac:RetCharCMOrderEnd(Conn, totalPage)
    CContractManufactureOrderWnd.RetCharCMOrderEnd(totalPage)
end

function Gas2Gac:RetAddCMOrderSuc(Conn) 
    CContractManufactureOrderWnd.RetAddCMOrderSuc()
end

function Gas2Gac:RetCancelContractManufactureOrder(Conn)
    CContractManufactureOrderWnd.RetCancelContractManufactureOrder()
end

function Gas2Gac:RetCommonCMOrderBegin()
    CContractManufactureMarketWnd.RetCommonCMOrderBegin()
end

function Gas2Gac:RetCommonContractManufactureOrder(Conn, cmOrderID, playerName, skillName, direction, prescripName, leftTime, money)
    CContractManufactureMarketWnd.RetCommonContractManufactureOrder(cmOrderID, playerName, skillName, direction, prescripName, leftTime, money)
end

function Gas2Gac:RetCommonCMOrderMaterialInfo(Conn, cmOrderID, index, materialType, materialName, materialNum)
    CContractManufactureMarketWnd.RetCommonCMOrderMaterialInfo(cmOrderID, index, materialType, materialName, materialNum)
end

function Gas2Gac:RetCommonCMOrderEnd(Conn)
    CContractManufactureMarketWnd.RetCommonCMOrderEnd()
end

function Gas2Gac:RetFightingEvaluationInfoInfo(Conn,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
	CFightingEvaluation.RetFightingEvaluationInfoInfo(uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
end

function Gas2Gac:RetUpdateEquipEffect(Conn,uState)
	CFightingEvaluation.RetUpdateEquipEffect(Conn,uState)
end

function Gas2Gac:RetAimFightingEvaluationInfoInfo(Conn,uCharID,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
	CAimFightingEvaluation.RetAimFightingEvaluationInfoInfo(uCharID,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
end

function Gas2Gac:RetShowIssueWnd(Conn,time)
	if not g_GameMain.m_IssueWnd then
		g_GameMain.m_IssueWnd = CIssueWnd:new(g_GameMain, time)
	end	
	g_GameMain.m_IssueWnd:GetWnd(time)
end

--打开自动寻路面板
function Gas2Gac:RetOpenAutoSeekWndBegin(Conn, stationId)
	if not g_GameMain.m_ForageSeekWnd then
		g_GameMain.m_ForageSeekWnd = CTongForageSeekWnd:new(g_GameMain)
	end
	g_GameMain.m_ForageSeekWnd:DeleteWnd()
end

--打开自动寻路面板
function Gas2Gac:RetOpenAutoSeekWnd(Conn, stationId, warZoneId)
	if not g_GameMain.m_ForageSeekWnd then
		g_GameMain.m_ForageSeekWnd = CTongForageSeekWnd:new(g_GameMain)
	end
	g_GameMain.m_ForageSeekWnd:OpenPanel(stationId, warZoneId)	
end

function Gas2Gac:RetStationSeekWnd(Conn,sceneType, stationId, sceneName, posx, posy, tongId, warZoneId)
	local tbl = {}
	tbl[1] = sceneName
	tbl[2] = posx
	tbl[3] = posy
	g_AutoSeekTbl[tongId] = tbl
	if not g_GameMain.m_ForageSeekWnd then
		g_GameMain.m_ForageSeekWnd = CTongForageSeekWnd:new(g_GameMain)
	end
	g_GameMain.m_ForageSeekWnd:CreatePanel(sceneType,stationId,sceneName, posx, posy, tongId, warZoneId)	
end

function Gas2Gac:RetFbStationAutoSeek(Conn, sceneName, posx, posy)
	PlayerAutoTrack("",sceneName,posx,posy)
end

function Gas2Gac:RetNotForageWnd(Conn)
	if not g_GameMain.m_IssueFetchWnd then
		g_GameMain.m_IssueFetchWnd = CTongForageQueryWnd:new(g_GameMain)
	end
	g_GameMain.m_IssueFetchWnd:InitWnd()
end

function Gas2Gac:RetIssueFetchQueryWnd(Conn, tongName, warzone, stationId, lineId, time)
	if not g_GameMain.m_IssueFetchWnd then
		g_GameMain.m_IssueFetchWnd = CTongForageQueryWnd:new(g_GameMain)
		g_GameMain.m_IssueFetchWnd:InitWnd()
	end
	g_GameMain.m_IssueFetchWnd:OpenPanel(tongName, warzone, stationId, lineId, time)
end


--打开报名查看面板
function Gas2Gac:OpenConfirmWnd(Conn, ActionName, activityName)
	CConfirmWnd:GetWnd(ActionName, activityName)
end

function Gas2Gac:OpenSecConfirmWnd(Conn, activityName,ItemName, count)
	CConfirmSecWnd.GetWnd(nil, activityName,ItemName, count)
end

function Gas2Gac:CreateTbl(Conn)
	CConfirmWnd.CreateTbl()
end

function Gas2Gac:InsertTbl(Conn, activityName,ActionName, ItemName, nHaveCount)
	CConfirmWnd.InsertTbl(activityName,ActionName, ItemName, nHaveCount)
end



function Gas2Gac:RetOpenFetchWnd(Conn)
	CTongIssueFetchWnd.GetWnd()
end

function Gas2Gac:ReturnMouseKey(Conn, lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
	g_GameMain.m_SysSetting.m_MouseSettingWnd:ChangeCtrlKey(lockTarget, movekey, selectLeft, selectRight, attrackLeft, attrackRight, distance)
end



function Gas2Gac:RetTakeContractManufactureOrder(Conn)
   CContractManufactureMarketWnd.RetTakeContractManufactureOrder() 
end

function Gas2Gac:RetShowCancelWnd(Conn)
	CTongCancelSignWnd.GetWnd()
end

function Gas2Gac:CreateRobTransprot(Conn)
	CTongEnterSceneWnd.GetWnd()
end

--打开报名查看面板
function Gas2Gac:RetOpenSignQueryWnd(Conn, tongName, tongCamp, exploitPoint, str)
	CSignQueryWnd.GetWnd(nil,tongName, tongCamp, exploitPoint, str)
end

--打开报名查看面板
function Gas2Gac:RetNoneOpenSignQueryWnd(Conn, str)
	CSignQueryWnd.CreateNoneWnd(nil, str)
end

function Gas2Gac:DelItem(Conn)
	CSignQueryWnd.InitWnd()
end

function Gas2Gac:ShowJudgnJoinWnd(Conn, tongName)
	CConfirmJoinWnd.GetWnd(nil,tongName)
end

function Gas2Gac:ShowCountDown(Conn, tongName, time)
	CRobResCountDown.GetWnd(nil,tongName, time)
end

------------------------------------------------------------------------------------
function Gas2Gac:ReturnAddNewAppellation(Conn,appellationIndex)
	CAppellationAndMoralWnd.ReturnAddNewAppellation(Conn,appellationIndex)
end

function Gas2Gac:ReturnCharAllAppellationInfo(Conn,appellationId)
	g_GameMain.m_AppellationAndMoralWnd:InitAppellationShow(appellationId,2)
end

function Gas2Gac:ReturnDelAppellationBegin(Conn)
	CAppellationAndMoralWnd.ReturnDelAppellationBegin(Conn)
end

function Gas2Gac:ReturnPlayerAppellationInfo(Conn)
	CAppellationAndMoralWnd.ReturnPlayerAppellationInfo(Conn)
end

function Gas2Gac:SendResistanceValue(Conn,CharID,Fb_Name,NatureResistanceValue,EvilResistanceValue,DestructionResistanceValue,Defence)
	if g_GameMain.m_AreaFbMsgWnd[Fb_Name] and g_GameMain.m_AreaFbMsgWnd[Fb_Name].m_EquipAssessWnd then 
		g_GameMain.m_AreaFbMsgWnd[Fb_Name].m_EquipAssessWnd:SetWndResistanceItemInfo(CharID,NatureResistanceValue,EvilResistanceValue,DestructionResistanceValue,Defence)
	else
		CEquipAssessWnd.SaveResistanceValue({CharID,NatureResistanceValue,EvilResistanceValue,DestructionResistanceValue,Defence})	
	end
end


function Gas2Gac:RetRobResourceWnd(Conn,sceneName)
	CRobResourceWnd.GetWnd(nil, sceneName)
end

function Gas2Gac:ShowRobResOverExit(Conn)
	CTongRobOverExit.GetWnd(nil, 10)
end

function Gas2Gac:ShowRobResZeroExitWnd(Conn, tongName)
	CTongRobResExitWnd.GetWnd(nil, 10, tongName)
end

function Gas2Gac:ReturnUpdateMoneyPayType(Conn)
    CNPCShopSellWnd.ReturnUpdateMoneyPayType()
end

function Gas2Gac:ReturnFinishInfo(Conn, QuestName, uTargetID)
 CEssayQuestionWnd.ReturnFinishInfo(QuestName, uTargetID)
end



function Gas2Gac:RetCreateRoleSucAtRushActivity(Conn)
    CNewRoleEnterGame.RetCreateRoleSucAtRushActivity()
end

function Gas2Gac:RetEquipRefine(Conn, sucFlag)
    CEquipRefine.RetEquipRefine(sucFlag)
end


function Gas2Gac:ReturnNpcShopBuyTipInfo(Conn, liuTongShopTipFlag, commonShopTipFlag)
    CNPCShopSellWnd.ReturnNpcShopBuyTipInfo(liuTongShopTipFlag, commonShopTipFlag)
end

function Gas2Gac:GetBackRoleFail(Conn, msgID)
    CSelectCharState.GetBackRoleFail(msgID)
end

-----排行榜------
function Gas2Gac:RetSortListBegin(Conn)
	g_GameMain.m_SortListWnd:RetSortListBegin()
end

function Gas2Gac:RetSortList(Conn, order, up, name, vocation, value)
	g_GameMain.m_SortListWnd:RetSortList(order, up, name, vocation, value)
end

function Gas2Gac:RetSortListEnd(Conn, sType, sName)
	g_GameMain.m_SortListWnd:RetSortListEnd(sType, sName)
end
-----------------

function Gas2Gac:ShowTransWndInfo(Conn, TriggerType, CostMoney, Name, EntityID, level, isBind)
	CCreateListCtrlItemWnd.ShowTransWndInfo(Conn, TriggerType, CostMoney, Name, EntityID, level, isBind)
end

-------------------精灵技能---------------------------------------
function Gas2Gac:RetActiveJingLingSkill(Conn,ActiveSkillID)
	CSkillJingLingWnd.RetActiveJingLingSkill(ActiveSkillID)
end

-----------------------非战斗技能----------------------------------
function Gas2Gac:ReturnNonFightSkill(Conn, name)
	CNonFightSkill.ReturnNonFightSkill(name)
end

function Gas2Gac:LearnNoneFightSkill(Conn, TargetEntityID, SkillName)
	CNonFightSkill.LearnNoneFightSkill(TargetEntityID, SkillName)
end

function Gas2Gac:CancelLearnNoneFightSkill(Conn, TargetEntityID)
	CNonFightSkill.CancelLearnNoneFightSkill(TargetEntityID)
end
-------------------------------公共技能升级学习 天赋技能升级----------------------------------

function Gas2Gac:ReturnLearnSkill(Conn,SkillName,SkillLevel)
	CLearnCommonSkillWnd.ReturnLearnSkill(SkillName,SkillLevel)
end


function Gas2Gac:UpdateFightSkill(Conn,SkillName,SkillLevel,ActionType)
	CLearnCommonSkillWnd.UpdateFightSkill(SkillName,SkillLevel,ActionType)
end

function Gas2Gac:UpdateFightSkillError(Conn,SkillName)
	CLearnCommonSkillWnd.UpdateFightSkillError(SkillName)
end
---------------------------------------------------------------------------
function Gas2Gac:RetLoginConnectStepInfo(Conn, msgID)
    CLoginAccounts.RetLoginConnectStepInfo(msgID)
end

function Gas2Gac:RetLoginCheckUserQueue(Conn, num)
	CLoginAccounts.RetLoginConnectStepInfo(1156, num)
end
-------------------------------天赋转职相关----------------------------------------
function Gas2Gac:AddTalentSkill(g_Conn, name, level )
	CSelectSeriesWnd.AddTalentSkill(name, level )
end

function Gas2Gac:RetAllFightSkill( g_Conn, name, level, kind )
	CSelectSeriesWnd.RetAllFightSkill(name, level, kind )
end

function Gas2Gac:RetAddFightSkillEnd(Conn)
	CSelectSeriesWnd.RetAddFightSkillEnd()
end

function Gas2Gac:ReturnSeries(Conn,SkillSeriesKind)
	CSelectSeriesWnd.ReturnSeries(SkillSeriesKind)
end

function Gas2Gac:ReturnSkillNode(Conn,SkillTreeNode)	
	CSelectSeriesWnd.ReturnSkillNode(SkillTreeNode)	
end

function Gas2Gac:ClearAllGenius(Conn)
	CNpcSkillLearnWnd.ClearAllGenius()
end

--初始化普通攻击技能
function Gas2Gac:ReCommonlySkill(Conn,CommonlySkillName)
	CCommonSkillWnd.ReCommonlySkill(CommonlySkillName)
end

function Gas2Gac:RetAddFightSkillStart(Conn)
	CCommonSkillWnd.RetAddFightSkillStart()
end
-----------------------------------------------------------------------------------

function Gas2Gac:InitPlayerAfflatusValue(Conn, CurOffLineExp, CurAfflatusValue, Money, MaxLevelLimit)
	CAfflatusValueWnd.InitPlayerAfflatusValue(Conn, CurOffLineExp, CurAfflatusValue, Money, MaxLevelLimit)
end

function Gas2Gac:UpdatePlayerCurOffLineExp(Conn, CurOffLineExp)
	local Wnd = g_GameMain.m_AfflatusValue
	if Wnd and Wnd:IsShow() then
		Wnd.m_CurOffLineExp:SetWndText(CurOffLineExp)
	end
end
------------------------------------------------------
function Gas2Gac:ReturnArmorPieceEnactment(Conn,result)
	CArmorPieceEnactment.ReturnArmorPieceEnactment(Conn,result)
end

------------------------------------------------------
function Gas2Gac:ReturnIdentifyResult(Conn,result)
	GacEquipIdentify.ReturnIdentifyResult(Conn,result)
end
-----------------------------------------------------------------------------------------
function Gas2Gac:RetAimEquipInfo( Connection, nBigID, nIndex, nItemID, eEquipPart,nBindingType)
	CAimStatus.RetAimEquipInfo(nBigID, nIndex, nItemID, eEquipPart,nBindingType)
end

function Gas2Gac:RetAimInfoEnd(Conn,CharID)
	CAimStatus.RetAimInfoEnd(CharID)
end

function Gas2Gac:ReturnAimPlayerSoulNum(Conn, soulValue)
	CAimStatus.ReturnAimPlayerSoulNum( soulValue)
end
-------------------------------------------------------------------------------------------

function Gas2Gac:RetEquipInfo( Connection, nBigID, nIndex, nItemID, eEquipPart,nBindingType )
	CPlayerStatus.RetEquipInfo( nBigID, nIndex, nItemID, eEquipPart,nBindingType )
end

function Gas2Gac:ReAssociateAttr(Conn,bool)
	CPlayerStatus.ReAssociateAttr(bool)
end

function Gas2Gac:ReWeaponAttr(Conn,bool,CommonlySkillName)
	CPlayerStatus.ReWeaponAttr(bool,CommonlySkillName)
end
-------------------------------------------------------------------------------------------
function Gas2Gac:AddItemMsgToConn(Conn,ItemId)
	CHortationInfoWnd.AddItemMsgToConn(Conn,ItemId)
end
---------------------------------------------------------------------------------------------
function Gas2Gac:ReturnCharAddSoulMsg(Conn,soulNum)
	CHortationInfoWnd.ReturnCharAddSoulMsg(Conn,soulNum)
end

function Gas2Gac:ShowSoulBottleAddSoulMsg(Conn,soulNum)
	CHortationInfoWnd.ShowSoulBottleAddSoulMsg(Conn,soulNum)
end

--答题游戏接口
function Gas2Gac:RetShowEssayQuestionWnd(Conn, QuestionTypeName, QuestName, EntityID)
	CEssayQuestionWnd.RetShowEssayQuestionWnd(Conn, QuestionTypeName, QuestName, EntityID)
end

function Gas2Gac:RetAnswerResult(Conn, QuestionID, bResult,QuestionStoreName)
	CEssayQuestionWnd.RetAnswerResult(QuestionID, bResult,QuestionStoreName)
end

function Gas2Gac:ClearActionTimesSucMsg(Conn)
end

function Gas2Gac:ExitMatchGameSuccMsg(Conn)
end

function Gas2Gac:ReturnIdentityAffirm(Conn,result)
	CIdentityAffirmWnd.ReturnIdentityAffirm(Conn,result)
end

function Gas2Gac:ReturnLockItemBagByTime(Conn,lock_time)
	CItemBagTimeLockWnd.ReturnLockItemBagByTime(Conn,lock_time)
end

function Gas2Gac:RetOpenIdentityAffirmWnd()
	CIdentityAffirmWnd.RetOpenIdentityAffirmWnd()
end

function Gas2Gac:ReturnInitLockItemBagByTime(Conn,lock_time)
	CItemBagTimeLockWnd.ReturnInitLockItemBagByTime(Conn,lock_time)
end 

function Gas2Gac:RetItemBagLockTimeOut(Conn)
	CItemBagLockWnd.RetItemBagLockTimeOut(Conn)
end


function Gas2Gac:RetEquipSuperaddSuc(Conn)
    CEquipSuperaddWnd.RetEquipSuperaddSuc()
end


function Gas2Gac:ChangeBackgroundMusic(Conn, musicName)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	local cfgTbl = BackgroundMusic_Client(musicName)
	PlayBackgroundMusic(nil, musicName, cfgTbl)
end

function Gas2Gac:RevertBackgroundMusic(Conn)
	if not IsCppBound(g_MainPlayer) then
		return
	end
	if g_GameMain.m_StopBackgroundMusicTick then
		UnRegisterTick(g_GameMain.m_StopBackgroundMusicTick)
		g_GameMain.m_StopBackgroundMusicTick = nil
	end
	if g_GameMain.m_PlayBackgroundMusicTick then
		UnRegisterTick(g_GameMain.m_PlayBackgroundMusicTick)
		g_GameMain.m_PlayBackgroundMusicTick = nil
	end
	StopCue(g_App.m_CurMusicName)
	g_App.m_CurMusicName = nil
	local AreaMusic = g_AreaMgr:GetSceneAreaMusic(g_GameMain.m_SceneName,g_MainPlayer.m_AreaName)
	if AreaMusic then
		local cfgTbl = BackgroundMusic_Client(AreaMusic)
		PlayBackgroundMusic(nil, AreaMusic, cfgTbl)
	end
end

--求购行
function Gas2Gac:ReturnMyPurchasingInfoBegin(Conn)
	CPurchasingWnd.GetWnd():ReturnMyPurchasingInfoBegin()
end
function Gas2Gac:ReturnMyPurchasingInfo(Conn,OrderId,ItemName,SingleMoney,LeftTime,LeftBuyCount)
	CPurchasingWnd.GetWnd():ReturnMyPurchasingInfo(OrderId,ItemName,SingleMoney,LeftTime,LeftBuyCount)
end
function Gas2Gac:ReturnMyPurchasingInfoEnd(Conn)
	CPurchasingWnd.GetWnd():ReturnMyPurchasingInfoEnd()
end
function Gas2Gac:RetAddMyPurchasing(Conn, bSuccess,OrderId)
	CPurchasingWnd.GetWnd():RetAddMyPurchasing(bSuccess,OrderId)
end

function Gas2Gac:RetCancelMyPurchasing(Conn, bSuccess, OrderId)
	CPurchasingWnd.GetWnd():RetCancelBuy(bSuccess,OrderId)
end

function Gas2Gac:ReturnPurchasingInfoBegin(Conn)
	CPurchasingAgentMainWnd.GetWnd():ReturnPurchasingInfoBegin()
end

function Gas2Gac:ReturnPurchasingInfo(Conn,OrderId,ItemName,Level,Count,SingleMoney,LeftTime,BuyPlayerName)
	CPurchasingAgentMainWnd.GetWnd():ReturnPurchasingInfo(OrderId,ItemName,Level,Count,SingleMoney,LeftTime,BuyPlayerName)
end

function Gas2Gac:ReturnPurchasingInfoEnd(Conn,ItemCount)
	CPurchasingAgentMainWnd.GetWnd():ReturnPurchasingInfoEnd(ItemCount)
end

function Gas2Gac:ReturnSellGoods(Conn,Result)
	if Result then
		CPurchasingAgentMainWnd.GetWnd():SearchByLastSearchCondition()
		CPurchasingAgentSellWnd.GetWnd():OpenPanel(false)
		CPurchasingAgentQuickSellWnd.GetWnd():OpenPanel(false)
	end
end

function Gas2Gac:ReturnFastSellItemInfoBegin(Conn)
	CPurchasingAgentMainWnd.GetWnd():RetCanSellPurchasingInfoBegin()
end

function Gas2Gac:ReturnFastSellItemInfo(Conn,ItemName, RoomIndex,Pos)
	CPurchasingAgentMainWnd.GetWnd():RetCanSellPurchasingInfo(ItemName)
end

function Gas2Gac:ReturnFastSellItemInfoEnd(Conn)
	CPurchasingAgentMainWnd.GetWnd():RetCanSellPurchasingInfoEnd()
end

function Gas2Gac:ReturnFastSellItemOrder(Conn,orderId,Count,SingleMoney)
	CPurchasingAgentQuickSellWnd.GetWnd():RetQuickSellItemOrder(orderId,SingleMoney,Count)
end

function Gas2Gac:ReturnFastSellItemOrderFail(Conn)
	CPurchasingAgentQuickSellWnd.GetWnd():RetQuickSellItemOrderFail()
end

function Gas2Gac:GetAveragePriceByItemName(Conn,AveragePrice,Count)
	CPurchasingWnd.GetWnd():RetAveragePriceByItemName(AveragePrice,Count)
end

function Gas2Gac:RetOpenTongNeedFireActivity(Conn,succ,msgId)
	CTongNeedFireActivityWnd.RetOpenTongNeedFireActivity(Conn,succ,msgId)
end

function Gas2Gac:RetCheckNeedFireIsOpen(Conn,succ,uLeftTime)
	CTongNeedFireActivityWnd.RetCheckNeedFireIsOpen(Conn,succ,uLeftTime)
end

function Gas2Gac:RetAddNeedFireItem(Conn,succ) 
	CTongNeedFireActivityItemWnd.RetAddNeedFireItem(Conn,succ) 
end

function Gas2Gac:RetAddFirewoodMsg(Conn,memberName,sItemName,ColorStr)
	CTongNeedFireActivityWnd.RetAddFirewoodMsg(Conn,memberName,sItemName,ColorStr)
end

function Gas2Gac:CreateMatchGameWnd(Conn, gameName)
	CMatchGameSignWnd.GetWnd(gameName)
end

function Gas2Gac:RetNeedFireActivityEnd(Conn,uMsgId)
	CTongNeedFireActivityWnd.RetNeedFireActivityEnd(Conn,uMsgId)
end

function Gas2Gac:RetTongNeedFireActivityMsg(Conn)
	MsgClient(351002)
end

function Gas2Gac:DelNpcFunLinkTbl(Conn, Name)
	g_GameMain.m_AreaInfoWnd:DelNpcFunLinkTbl(Name)
end


function Gas2Gac:DelNpcFunTbl(Conn)
	g_GameMain.m_AreaInfoWnd:DelNpcFunTbl()
end

function Gas2Gac:YYReturn(Conn,key)
	CYYCtrl.SetYYLoginKey(key)
end

function Gas2Gac:YYLoginFail(Conn)
	CYYCtrl.YYLoginFailure("登陆YY校验失败")
	if IsCppBound(g_Conn) then
		g_Conn:ShutDown()
		g_Login.m_nConnStatus = 0
		g_Login.m_IsConnecting = false
	end
end

function Gas2Gac:RetCapableOfBuyingCouponsInfoBegin(Conn)
    CBuyCouponsWnd.RetCapableOfBuyingCouponsInfoBegin()
end

function Gas2Gac:RetCapableOfBuyingCouponsInfo(Conn, sequenceID, name, price, smallIcon, desc, url)
    CBuyCouponsWnd.RetCapableOfBuyingCouponsInfo(sequenceID, name, price, smallIcon, desc, url)
end

function Gas2Gac:RetCapableOfBuyingCouponsInfoEnd(Conn)
    CBuyCouponsWnd.RetCapableOfBuyingCouponsInfoEnd()
end

function Gas2Gac:RetBoughtCouponsInfoBegin(Conn)
    CBuyCouponsWnd.RetBoughtCouponsInfoBegin()
end

--参数：点券购买顺序index，对应的点券号ID，点券序列号
function Gas2Gac:RetBoughtCouponsInfo(Conn, index, ID, sequenceID, name, price, smallIcon, desc, url)
    CBuyCouponsWnd.RetBoughtCouponsInfo(index, ID, sequenceID, name, price, smallIcon, desc, url)
end

function Gas2Gac:RetBoughtCouponsInfoEnd(Conn)
    CBuyCouponsWnd.RetBoughtCouponsInfoEnd()
end

function Gas2Gac:RetCSMAddSellOrderOverAvgPrice(Conn, itemType, itemName, avgPrice)
    CConsignmentCommon.RetCSMAddSellOrderOverAvgPrice(itemType, itemName, avgPrice)
end


function Gas2Gac:ShowPanelByTongItem(Conn, sceneName, posx, posy, name, sceneId, serverId)
	CTongItemUseTrans.GetWnd(sceneName, posx, posy, name, sceneId, serverId)
end


function Gas2Gac:OpenMessageBox(Conn, msgId)
	local str = GetStaticTextClient(msgId)
	g_GameMain.m_MsgBox = MessageBox(g_GameMain,str , MB_BtnOK, CallBack, g_GameMain)
end

function Gas2Gac:OpenWebBrowser(Conn, sToken)
	g_App.m_WeiBo:OpenWebBrowser(sToken)
end

function Gas2Gac:WeiBoLoginFaild(Conn)
	g_App.m_WeiBo:WeiBoLoginFaild()
end

function Gas2Gac:RetChangeInfoWnd(Conn)
	CTongChangeByPos.GetWnd()
end