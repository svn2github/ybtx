local StmtOperater = {}
local UISettingBox = CreateDbBox(...)

-------------------------------------------------------
--添加设置信息
local StmtDef = {
	"_AddUISettingInfo",
	[[
		replace into tbl_ui_setting
			(	us_uId,
				uis_uPlayerNameShow,
				uis_uPlayerHonourShow,
				uis_uPlayerTongNameShow,
				uis_uPlayerArmyCorpsNameShow,
				uis_uOtherNameShow,
				uis_uOtherHonourShow,
				uis_uOtherTongNameShow,
				uis_uOtherArmyCorpsNameShow,
				uis_uNpcNameShow,
				uis_uMonsterNameShow,
				uis_uHeadPieceShow,
				uis_uLockShortcut,
				uis_uMonsterNameShowMO,
				uis_uCanNotSelectFriend	)
		values(?,?,?,?,?,?,?,?,?,?,?,?,?,?,?)
	]]
}
DefineSql( StmtDef, StmtOperater )

--查询某玩家的设置信息
local StmtDef = {
	"_GetUISettingInfo",
	[[
		select
			uis_uPlayerNameShow,
			uis_uPlayerHonourShow,
			uis_uPlayerTongNameShow,
			uis_uPlayerArmyCorpsNameShow,
			uis_uOtherNameShow,
			uis_uOtherHonourShow,
			uis_uOtherTongNameShow,
			uis_uOtherArmyCorpsNameShow,
			uis_uNpcNameShow,
			uis_uMonsterNameShow,
			uis_uHeadPieceShow,
			uis_uLockShortcut,
			uis_uMonsterNameShowMO,
			uis_uCanNotSelectFriend
		from
			tbl_ui_setting
		where
			us_uId = ?
	]]
}
DefineSql( StmtDef, StmtOperater )

--游戏属性设置
function UISettingBox.SaveUISettingDB(parameters)
	local us_uId					= parameters["uUserID"]
	local uPlayerNameShow			= parameters["uPlayerNameShow"]
	local uPlayerHonourShow			= parameters["uPlayerHonourShow"]
	local uPlayerTongNameShow		= parameters["uPlayerTongNameShow"]
	local uPlayerArmyCorpsNameShow	= parameters["uPlayerArmyCorpsNameShow"]
	local uOtherNameShow			= parameters["uOtherNameShow"]
	local uOtherHonourShow			= parameters["uOtherHonourShow"]
	local uOtherTongNameShow		= parameters["uOtherTongNameShow"]
	local uOtherArmyCorpsNameShow	= parameters["uOtherArmyCorpsNameShow"]
	local uNpcNameShow				= parameters["uNpcNameShow"]
	local uMonsterNameShow			= parameters["uMonsterNameShow"]
	local uHeadPieceShow			= parameters["uHeadPieceShow"]
	local uLockShortcut				= parameters["uLockShortcut"]
	local uMonsterNameShowMO		= parameters["uMonsterNameShowMO"]
	local uCanNotSelectFriend		= parameters["uCanNotSelectFriend"]
	
	StmtOperater._AddUISettingInfo:ExecStat(us_uId,uPlayerNameShow,uPlayerHonourShow,uPlayerTongNameShow,uPlayerArmyCorpsNameShow,uOtherNameShow,uOtherHonourShow,uOtherTongNameShow,uOtherArmyCorpsNameShow,uNpcNameShow,uMonsterNameShow,uHeadPieceShow,uLockShortcut,uMonsterNameShowMO,uCanNotSelectFriend)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran()
		return false
	end
	return true
end

--得到某玩家的游戏设置信息
function UISettingBox._GetUISettingInfoDB(uUserID)
	local query_result	= StmtOperater._GetUISettingInfo:ExecStat(uUserID)
	if query_result:GetRowNum() == 0 then
		return {true,false,false,false,true,false,false,false,true,true,true,false,true,false}
	end
	local tblTemp = {[0] = false, true}
	local tblInfo = {}
	for i = 1, 14 do
		table.insert(tblInfo, tblTemp[query_result(1, i)] )
	end
	return tblInfo
end


--得到某玩家的游戏设置信息
function UISettingBox.GetUISettingInfoDB(parameters)
	return UISettingBox._GetUISettingInfoDB(parameters["uUserID"])
end

-------------------------------------------------------
SetDbLocalFuncType(UISettingBox.SaveUISettingDB)
SetDbLocalFuncType(UISettingBox.GetUISettingInfoDB)
return UISettingBox