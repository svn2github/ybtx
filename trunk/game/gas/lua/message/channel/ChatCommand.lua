
function ChatCommandGoHome(Player)
	if not IsCppBound(Player) then
		return
	end
	local NameTbl = {
		["黑夜本"] = true,
		["守护迷你村庄"] = true,
		["海岸防守"] = true,
		["异界守护塔队伍"] = true,
		["异界守护塔单人"] = true,
	}
	local Scene = Player.m_Scene
	local SceneName = Scene.m_SceneName
	if NameTbl[SceneName] then
		Player:CancelCastingProcess()
		local ModelString = "fx/setting/skill/rlm_fs_ds_skill/sf_13/create.efx"
		local StateString = "sf_13/create"
		Gas2Gac:UseItemPlayerEffect(Player.m_Conn,ModelString,StateString)
		Gas2Gac:UseItemTargetEffect(Player:GetIS(0),ModelString,StateString,Player:GetEntityID())

		local PixelPos = CFPos:new()
		local PosX = Scene.m_SceneAttr.InitPosX
		local PosY = Scene.m_SceneAttr.InitPosY
		PixelPos.x = PosX * EUnits.eGridSpanForObj
		PixelPos.y = PosY * EUnits.eGridSpanForObj
		Player:SetPixelPos(PixelPos)
		return true
	end
end

ChatCommandList = {
	["hg"] = ChatCommandGoHome,
}
