gas_require "fb_game/random_game/RandomGameMgr"
local function Script(Arg, Trigger)

	local GameName = Arg[1]
	Trigger.m_RandomGame = CRandomGameMgr:new(GameName)
	Trigger.m_RandomGame:StartGame()
end

return Script
