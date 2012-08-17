gas_require "framework/main_frame/SandBoxDef"

local add_item_DB_Ret_By_Id = CRoleQuest.add_item_DB_Ret_By_Id
local CallAccountManualTrans = CallAccountManualTrans
local IsCppBound = IsCppBound
local IsServerObjValid = IsServerObjValid
local g_IntObjServerMgr = g_IntObjServerMgr
local g_ItemInfoMgr = g_ItemInfoMgr
local g_ObjActionArgTbl = g_ObjActionArgTbl
local MsgToConn = MsgToConn
local g_GetPlayerInfo = g_GetPlayerInfo
local CLiveskillBase = CLiveskillBase
local SendPlayerFinishDirect = SendPlayerFinishDirect
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, IntObj, ObjName, GlobalID)

	local Player = Conn.m_Player
	if not IsCppBound(Player) then 
		return
	end
	local data = {}
	data["char_id"] = Player.m_uID
	data["nType"] = 28
	data["sName"] = ObjName
	data["nCount"] = 1
	data["AddOreExp"] = g_ObjActionArgTbl[ObjName][1]
	data["sceneName"] = Player.m_Scene.m_SceneName
	
	local function CallBack(result)

		if result[1] then
			if IsServerObjValid(IntObj) then
				g_IntObjServerMgr:Destroy(IntObj,false)
			end
			add_item_DB_Ret_By_Id(data["char_id"], data["nType"], data["sName"], 1, result[2])
			if IsCppBound(Player) then 
				if result[2][1][1] then
					Gas2Gac:AddItemMsgToConn(Player.m_Conn,result[2][1][1])
				end
				if result[5] then
					SendPlayerFinishDirect(Player, result[5])
				end
			end
			if result[3] and result[3] ~= 0 then
				MsgToConn(Conn, 9624, result[3])
				local skill_info = result[4]
				if skill_info then
					CLiveskillBase.RetLiveskillInfo(Conn,skill_info)
				end
			end
		else
			if IntObj then
				IntObj.m_IsClick = nil
			end
			if IsCppBound(Player) then
				if result[2] == 3 then
					MsgToConn(Player.m_Conn, 3038)
				end
			end
		end
	end
	IntObj.m_IsClick = true
	CallAccountManualTrans(Conn.m_Account, "ItemObjDB", "CollectOreObj", CallBack, data)
end

return Entry
 