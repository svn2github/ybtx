CUseVIPItem = class()

function CUseVIPItem.UseVIPItem(Conn, ItemName, RoomIndex, Pos)
	local Player = Conn.m_Player
	if Player:ExistState("Ë«±¶ÀëÏß×´Ì¬") then
		return
	end
	local function CallBack(result,sMsg)
		if result then
			Player:PlayerDoItemSkill("Ë«±¶ÀëÏß",1)
		else
			if sMsg then
				MsgToConn(Conn,sMsg)
			end
		end
	end
	
	local data = {}
	data["PlayerID"]	= Player.m_uID
	data["ItemName"]	= ItemName
	data["RoomIndex"] = RoomIndex
	data["Pos"]				= Pos
	data["uLevel"] = Player:CppGetLevel()
	
	CallAccountManualTrans(Conn.m_Account, "UseVIPItemDB", "UseVIPItem", CallBack, data)
end

function CUseVIPItem.SendMailItemToVip()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun()
			
			end
			CallDbTrans("UseVIPItemDB","SendMailItemToVip", CallBackFun,{})
		end
	end
	g_AlarmClock:AddTask("SendMailItemToVipTick", {wday = {1,2,3,4,5,6,7},time = {"05:00:00"}}, CallBack)
end


function CUseVIPItem.SendWelfareLevel1MailItem()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun()
			
			end
			local data = {["WelfareLevel"] = 1}
			CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,data)
		end
	end
	g_AlarmClock:AddTask("SendWelfareLevel1MailItemTick", {wday = {1,2,3,4,5,6,7},time = {"05:02:00"}}, CallBack)
end



function CUseVIPItem.SendWelfareLevel2MailItem()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun()
			
			end
			local data = {["WelfareLevel"] = 2}
			CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,data)
		end
	end
	g_AlarmClock:AddTask("SendWelfareLevel2MailItemTick", {wday = {1,2,3,4,5,6,7},time = {"05:04:00"}}, CallBack)
end


function CUseVIPItem.SendWelfareLevel3MailItem()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun()
			
			end
			local data = {["WelfareLevel"] = 3}
			CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,data)
		end
	end
	g_AlarmClock:AddTask("SendWelfareLevel3MailItemTick", {wday = {1,2,3,4,5,6,7},time = {"05:06:00"}}, CallBack)
end

function CUseVIPItem.SendWelfareLevel4MailItem()
	local function CallBack()
		if not g_App:CallDbHalted() then
			local function CallBackFun()
			
			end
			local data = {["WelfareLevel"] = 4}
			CallDbTrans("UseVIPItemDB","SendWelfareMailItem", CallBackFun,data)
		end
	end
	g_AlarmClock:AddTask("SendWelfareLevel4MailItemTick", {wday = {1,2,3,4,5,6,7},time = {"05:08:00"}}, CallBack)
end
