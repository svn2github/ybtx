gas_require "framework/main_frame/SandBoxDef"
gas_require "world/trigger_action/item/item_trigger/UseItemStateCheck"

local ItemNumCheck = ItemNumCheck
local EDoSkillCtrlState = EDoSkillCtrlState
local MsgToConn = MsgToConn
local g_GetPlayerInfo = g_GetPlayerInfo
local Entry = CreateSandBox(...)

function Entry.Exec(Conn, ItemName, ItemInfo, RoomIndex, Pos)
	local Player = Conn.m_Player
	if Player.m_UseItemLoadingTick or 
		Player.m_ActionLoadingTick or 
		Player.m_CollectResTick then
		return
	end
	if Player:CppGetDoSkillCtrlState(EDoSkillCtrlState.eDSCS_ForbitUseMissionItemSkill) then
		MsgToConn(Conn,829)
		return
	end
	--挂在Player上的东东太多了
	--与物品使用相关都挂到Conn.m_Player.UseItemParam[ItemName]上
	if Conn.m_Player.UseItemParam == nil then
		Conn.m_Player.UseItemParam = {}
	end
	
--	Conn.m_Player.UseItemParam.CreateTbl											= nil		--创建的Npc刷怪器
	if Player.UseItemParam[ItemName] == nil then
		Player.UseItemParam[ItemName] = {}
	else
		return
	end
	Conn.m_Player.UseItemParam[ItemName].RoomIndex = RoomIndex
	Conn.m_Player.UseItemParam[ItemName].RoomPos = Pos
	
--	Conn.m_Player.UseItemParam[ItemName].IsDoAction						= nil		--是否播放动作和特效 调用技能的不播放
--	Conn.m_Player.UseItemParam[ItemName].IsOnTarget						= nil		--对目标使用
--	Conn.m_Player.UseItemParam[ItemName].IsOnPos							= nil		--对地点使用
--	Conn.m_Player.UseItemParam[ItemName].OnPosEffectVector3f  = nil		--对地点使用的坐标x,y,z
	ItemNumCheck(Conn, ItemInfo, RoomIndex, Pos)
--	print("Conn.m_Player.UseItemParam[ItemName] = {}")
end

return Entry