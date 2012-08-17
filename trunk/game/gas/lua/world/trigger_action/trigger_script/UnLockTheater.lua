local function Script(Arg, Trigger, Player)
	
	local Scene = Trigger.m_Scene
	
	for i=1,#(Arg) do
		Scene.m_TheaterMgr:UnLockTheater(Arg[i])
	end
end

return Script
