CUISetting = class()

function CUISetting.SaveUISetting(Conn,uPlayerNameShow,uPlayerHonourShow,uPlayerTongNameShow,uPlayerArmyCorpsNameShow,uOtherNameShow,uOtherHonourShow,uOtherTongNameShow,uOtherArmyCorpsNameShow,uNpcNameShow,uMonsterNameShow,uHeadPieceShow,uLockShortcut,uMonsterNameShowMO,uCanNotSelectFriend)
	local Player = Conn.m_Player
	if Player == nil then
		return
	end
	Player.tblSysSettingInfo = Player.tblSysSettingInfo or {}
	
	local parameters = {}
	parameters["uUserID"]					= Conn.m_Account:GetID()
	parameters["uPlayerNameShow"]			= uPlayerNameShow
	parameters["uPlayerHonourShow"]			= uPlayerHonourShow
	parameters["uPlayerTongNameShow"]		= uPlayerTongNameShow
	parameters["uPlayerArmyCorpsNameShow"]	= uPlayerArmyCorpsNameShow
	parameters["uOtherNameShow"]			= uOtherNameShow
	parameters["uOtherHonourShow"]			= uOtherHonourShow
	parameters["uOtherTongNameShow"]		= uOtherTongNameShow
	parameters["uOtherArmyCorpsNameShow"]	= uOtherArmyCorpsNameShow
	parameters["uNpcNameShow"]				= uNpcNameShow
	parameters["uMonsterNameShow"]			= uMonsterNameShow
	parameters["uHeadPieceShow"]			= uHeadPieceShow
	parameters["uLockShortcut"]				= uLockShortcut
	parameters["uMonsterNameShowMO"]		= uMonsterNameShowMO
	parameters["uCanNotSelectFriend"]		= uCanNotSelectFriend
	
	Player.tblSysSettingInfo["UISetting"]	= parameters
end