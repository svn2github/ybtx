
local function TargetPlayEffect(ModelString,StateString,Npc)
	Npc:GetRenderObject():AddEffect(ModelString,StateString,0,nil) --参数0 特效只播放1次
--	print("Target播放完了")
end

local function PlayEffectOnPos(ModelString,StateString,pos)
	CPositionFX_CppPlayPosFX(ModelString,StateString,pos)
--	print("OnPos播放完了")
end

function Gas2Gac:UseItemPlayerEffect(Conn,ModelString,StateString)
	if IsCppBound(g_MainPlayer) then
		g_MainPlayer:GetRenderObject():AddEffect(ModelString,StateString,0,nil)
	end
--	print("Player Play")
--	PlayerPlayEffect(ModelString,StateString)
end

function Gas2Gac:UseItemTargetEffect(Conn,ModelString,StateString,NpcId)
	local Npc = CCharacterFollower_GetCharacterByID(NpcId)
	if Npc then
		TargetPlayEffect(ModelString,StateString,Npc)
	end
end

function Gas2Gac:UseItemOnPosEffect(Conn,ModelString,StateString,PosX,PosY,PosZ)
--	local pos = CVector3f:new()
--	pos.x = PosX
--	pos.y = PosY
--	pos.z = PosZ
--	if PosX == 0 and PosY == 0 and PosZ == 0 then
--		g_MainPlayer:GetRenderObject():GetRootPosition (pos)
--	end
	local pos = CFPos:new()
	pos.x = PosX
	pos.y = PosZ
	if PosX == 0 and PosZ == 0 then
		g_MainPlayer:GetPixelPos(pos)
	end
	PlayEffectOnPos(ModelString,StateString,pos)
end