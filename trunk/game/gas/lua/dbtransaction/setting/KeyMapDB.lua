cfg_load "key_map/KeyMap_Common"

local StmtContainer = class()

local StmtDef = {
		"GetMouseKeyStmt",
		[[ select 
				ckm_lockTarget, ckm_movekey, ckm_selectLeft, ckm_selectRight, ckm_attrackLeft, ckm_attrackRight, ckm_tabDistance
			from
				tbl_keymap_mouse
			where
				cs_uId = ?
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"GetCharAllKeyMap",
		[[ select 
				ckm_sFunctionName, ckm_sFirstKey, ckm_uKeyPostion
			from
				tbl_keymap
			where
				cs_uId = ?
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"SetKeyMapStmt",
		[[ replace into tbl_keymap values (?, ?, ?, ?) ]]
}
DefineSql( StmtDef, StmtContainer )	

local StmtDef = {
		"DeleteKeyMapStmt",
		[[
			delete from tbl_keymap
			where
			 cs_uId = ? and ckm_sFunctionName = ? and ckm_uKeyPostion = ?
		]]
}
DefineSql( StmtDef, StmtContainer )	

local StmtDef = {
		"HasMouseKeyRecord",
		[[ select count(*) from tbl_keymap_mouse where cs_uId = ? ]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"InsertMouseKeyWithoutDis",
		[[	insert into 
				tbl_keymap_mouse
				(	cs_uId,
					ckm_lockTarget,
					ckm_movekey,
					ckm_selectLeft,
					ckm_selectRight,
					ckm_attrackLeft,
					ckm_attrackRight,
					ckm_bShowMsgBox
				)
			values
				(?, ?, ?, ?, ?, ?, ?, ?)
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"InsertMouseKeyWithoutShow",
		[[	insert into 
				tbl_keymap_mouse
				(	cs_uId,
					ckm_lockTarget,
					ckm_movekey,
					ckm_selectLeft,
					ckm_selectRight,
					ckm_attrackLeft,
					ckm_attrackRight,
					ckm_tabDistance
				)
			values
				(?, ?, ?, ?, ?, ?, ?, ?)
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"UpdateMouseKeyWithoutDis",
		[[	update
				tbl_keymap_mouse
			set
				ckm_lockTarget		= ?,
				ckm_movekey			= ?,
				ckm_selectLeft		= ?,
				ckm_selectRight		= ?,
				ckm_attrackLeft		= ?,
				ckm_attrackRight	= ?,
				ckm_bShowMsgBox		= ?
			where
				cs_uId = ?
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"UpdateMouseKeyWithoutShow",
		[[	update
				tbl_keymap_mouse
			set
				ckm_lockTarget		= ?,
				ckm_movekey			= ?,
				ckm_selectLeft		= ?,
				ckm_selectRight		= ?,
				ckm_attrackLeft		= ?,
				ckm_attrackRight	= ?,
				ckm_tabDistance		= ?
			where
				cs_uId = ?
		]]
}
DefineSql( StmtDef, StmtContainer )

local StmtDef = {
		"BeShowMouseSettingMsgBox",
		[[	select
				ckm_bShowMsgBox
			from
				tbl_keymap_mouse
			where
				cs_uId = ?
		]]
}
DefineSql( StmtDef, StmtContainer )


--local KeyMap_Common = KeyMap_Common
local KeyMapDB = CreateDbBox(...)

local function GetMouseMap(data)
	local query_result = StmtContainer.GetMouseKeyStmt:ExecSql("nnnnnnn", data["char_id"])
	return query_result
end

local function GetAllKeyMap(data)
	local query_result = StmtContainer.GetCharAllKeyMap:ExecSql("s[32]s[16]n", data["char_id"])
	return query_result
end


function KeyMapDB.GetAllKeyMap(data)
	return {GetAllKeyMap(data), GetMouseMap(data)}
end

function KeyMapDB.SaveMouseKey(data)
	local lockTarget	= data["lockTarget"]
	local movekey		= data["movekey"]
	local selectLeft	= data["selectLeft"]
	local selectRight	= data["selectRight"]
	local attrackLeft	= data["attrackLeft"]
	local attrackRight	= data["attrackRight"]
	local distance		= data["distance"]
	local charID		= data["char_id"]
	
	local ret = StmtContainer.HasMouseKeyRecord:ExecSql("", charID)
	local num = ret:GetData(0, 0)
	if(num > 0) then
		StmtContainer.UpdateMouseKeyWithoutShow:ExecSql("", lockTarget, movekey, selectLeft,
				selectRight, attrackLeft, attrackRight, distance, charID)
	else
		StmtContainer.InsertMouseKeyWithoutShow:ExecSql("",charID, lockTarget, movekey, selectLeft,
				selectRight, attrackLeft, attrackRight, distance)
	end
	
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function KeyMapDB.SaveMouseKeyFromMsgBox(data)
	local lockTarget	= data["lockTarget"]
	local movekey		= data["movekey"]
	local selectLeft	= data["selectLeft"]
	local selectRight	= data["selectRight"]
	local attrackLeft	= data["attrackLeft"]
	local attrackRight	= data["attrackRight"]
	local bShowMsgBox	= data["bShowMsgBox"]
	local charID		= data["char_id"]
	
	local ret = StmtContainer.HasMouseKeyRecord:ExecSql("", charID)
	local num = ret:GetData(0, 0)
	if(num > 0) then
		StmtContainer.UpdateMouseKeyWithoutDis:ExecSql("", lockTarget, movekey,
				selectLeft, selectRight, attrackLeft, attrackRight, bShowMsgBox, charID)
	else
		StmtContainer.InsertMouseKeyWithoutDis:ExecSql("",charID, lockTarget, movekey,
				selectLeft, selectRight, attrackLeft, attrackRight, bShowMsgBox)
	end
	
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

function KeyMapDB.BeShowMouseSettingMsgBox(nCharID)
	local ret = StmtContainer.BeShowMouseSettingMsgBox:ExecSql("", nCharID)
	if ret:GetRowNum() > 0 then
		return ret:GetData(0, 0)
	else
		return 1
	end
end

function KeyMapDB.SetKeyMap(data)
--	for i=1,#(data) do
--		local NewValue = data[i]["Key"]
--		local updateflag = true
--		local uCount = KeyMap_Common:GetHeight()
--		for k = 1,uCount do
--			if(KeyMap_Common(k,"Name") == data[i]["FuncName"]) then
--				if(data[i]["Position"]==1 and tostring(KeyMap_Common(k,"DefaultKey1")) == NewValue ) or 
--					(data[i]["Position"]==2 and tostring(KeyMap_Common(k,"DefaultKey2")) == NewValue ) then
--					--判断新设置的值是否和静态文件的值一样,一样时通知服务端重数据库删除这项设置
--					if data[i]["Style"] then
--						data[i]["Style"] = false
--					end
--					updateflag = false
--				end
--				break
--			end
--		end
--		if updateflag and not data[i]["Style"] then
--			data[i]["Style"] = true
--		end
--	end
	for i=1, #(data) do
		if(data[i]["Style"])then
			StmtContainer.SetKeyMapStmt:ExecSql("", data[i]["char_id"], data[i]["FuncName"], data[i]["Key"], data[i]["Position"])
		else
			StmtContainer.DeleteKeyMapStmt:ExecSql("", data[i]["char_id"], data[i]["FuncName"], data[i]["Position"])
		end
	end
	return g_DbChannelMgr:LastAffectedRowNum() > 0
end

SetDbLocalFuncType(KeyMapDB.GetAllKeyMap)
SetDbLocalFuncType(KeyMapDB.SetKeyMap)
SetDbLocalFuncType(KeyMapDB.SaveMouseKey)
SetDbLocalFuncType(KeyMapDB.SaveMouseKeyFromMsgBox)
SetDbLocalFuncType(KeyMapDB.BeShowMouseSettingMsgBox)
return KeyMapDB
