gas_require "framework/main_frame/SandBoxDef"

local AddPlayerExp_DB_Ret = CRoleQuest.AddPlayerExp_DB_Ret
local CallAccountManualTrans = CallAccountManualTrans
local IsCppBound = IsCppBound
local g_GetPlayerInfo = g_GetPlayerInfo
local OnSavePlayerExpFunc = OnSavePlayerExpFunc

local Entry = CreateSandBox(...)

function Entry.Exec(Conn,ItemName,ItemInfo, RoomIndex, Pos)

	local Player = Conn.m_Player 
	local AddExp = ItemInfo("ScriptArg")
	OnSavePlayerExpFunc({Player})
	local function CallBack(res,LevelExpTbl)
		if res then
			if IsCppBound(Player) then
				if LevelExpTbl then
					local AddExpTbl = {}
					AddExpTbl["Level"] = LevelExpTbl[1]
					AddExpTbl["Exp"] = LevelExpTbl[2]
					AddExpTbl["AddExp"] = AddExp
					AddExpTbl["uInspirationExp"] = 0
					AddPlayerExp_DB_Ret(Player, AddExpTbl)
				end
			end
--		else
--		 	print("使用经验球失败!")
		end
	end	
	local data = {}
	data["nCharID"] = Player.m_uID
	data["char_id"] = Player.m_uID
	data["nRoom"]  	= RoomIndex               
	data["nPos"]   	= Pos         
	data["ItemName"] = ItemName
	data["nCount"]  = 1
	data["addExp"] = AddExp
	data["sceneName"] = Player.m_Scene.m_SceneName
	CallAccountManualTrans(Conn.m_Account, "RoleQuestDB", "UseItemAddExp", CallBack, data)
end

return Entry
