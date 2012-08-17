
local function Script(Arg, Npc)
	local scene = Npc.m_Scene
	local musicName = Arg[1]
	for id, plyer in pairs(scene.m_tbl_Player) do
		 Gas2Gac:RevertBackgroundMusic(plyer.m_Conn)
	end
end

return Script