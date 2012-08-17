
local function Script(Arg, Npc)
	local scene = Npc.m_Scene
	local musicName = Arg[1]
	for id, plyer in pairs(scene.m_tbl_Player) do
		 Gas2Gac:ChangeBackgroundMusic(plyer.m_Conn, musicName)
	end
end

return Script