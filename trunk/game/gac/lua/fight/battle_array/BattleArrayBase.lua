gac_require "fight/battle_array/BattleArrayBaseInc"

function CreateBattleArrayBase(Parent)
	local BattleArrayBase = CBattleArrayBase:new()
	BattleArrayBase:Init(Parent)
	return BattleArrayBase
end

function CBattleArrayBase:Init(Parent)
	local tbl = g_ItemInfoMgr:GetTableByBigID(26)
	self.m_AllBattleArrayName = {}
	local tmpTblForSort = {}
	local tblNames = tbl:GetKeys()
	for i,key in pairs(tblNames) do
		local v = tbl(key)
		table.insert( tmpTblForSort, {key, v("type")} )
	end
	table.sort(tmpTblForSort, function (a, b) return a[2] < b[2] end)
	for i = 1, #tmpTblForSort do
		table.insert(self.m_AllBattleArrayName, tmpTblForSort[i][1])
	end
	self.m_tblBattleArrayPos = {} --{ { {id, arrayName, arrayType}, {pos1, pos2, pos3, pos4, pos5} }, {{}, {}}, {{}, {}}, {} }
	self.m_tblBattleArrayMem = {}
end

function CBattleArrayBase:GetArrayNameIndex(arrayName)
	local tblAllArray = self.m_AllBattleArrayName
	for i, p in pairs(tblAllArray) do
		if(arrayName == p) then return i end
	end
	return nil
end

--得到阵法临时技能名称字符串
function CBattleArrayBase:GetArraySkillDes(arrayName)
	local item_type = 26
	local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(item_type, arrayName)
	local arrayDes	= ItemInfo("Skill1")
					 .. "#r" .. ItemInfo("Skill2")
					 .. "#r" .. ItemInfo("Skill3")
					 .. "#r" .. ItemInfo("Skill4")
					 .. "#r" .. ItemInfo("Skill5")
	return arrayDes
end

--RPC-------------------------------------------------------------------------------

--返回保存阵型
function Gas2Gac:ReturnSaveBattleArrayPos(Conn, id, itemId)
	local tblArrayTmp		= g_GameMain.m_CreateBattleArrayWnd.m_tblArrayPosTemp--{unpack(g_GameMain.m_CreateBattleArrayWnd.m_tblArrayPosTemp)}
	local tblArrayPos		= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local arrayName			= g_GameMain.m_CreateBattleArrayWnd.m_sArrayName
	local nameIndex			= g_GameMain.m_BattleArrayBase:GetArrayNameIndex(arrayName)
	local page				= math.floor(nameIndex/12)
	tblArrayPos[nameIndex]	= {{id, arrayName, 1}, tblArrayTmp}
	tblArrayTmp = {}
	if(0 ~= itemId) then g_DynItemInfoMgr:DelItemInfo(itemId) end
	g_GameMain.m_CreateBattleArrayWnd:ShowWnd(false)
	g_GameMain.m_BattleArrayStoreWnd.m_Page = page
	g_GameMain.m_BattleArrayStoreWnd:SetCheckBtn(page+1)
	g_GameMain.m_BattleArrayStoreWnd:OpenWnd(nameIndex, g_GameMain.m_CreateBattleArrayWnd.m_openPlace)
end

--返回阵型信息
function Gas2Gac:ReturnGetAllBattleArrayPosInfo(Conn, id, arrayName, pos1, pos2, pos3, pos4, pos5, arrayType)
	local tbl		= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local nameIndex	= g_GameMain.m_BattleArrayBase:GetArrayNameIndex(arrayName)
	tbl[nameIndex]	= {{id, arrayName, arrayType}, {pos1, pos2, pos3, pos4, pos5}}
end

--返回阵型信息结束
function Gas2Gac:ReturnGetAllBattleArrayPosInfoEnd(Conn)
	--print("------阵型信息加载完毕！------")
end

--返回列阵信息
function Gas2Gac:ReturnGetAllBattleArrayMemPosInfo(Conn, arrayId, id1, id2, id3, id4, id5)
	local tblMem	= g_GameMain.m_BattleArrayBase.m_tblBattleArrayMem
	local tblPos	= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
	local arrayName = ""
	for i, p in pairs(tblPos) do
		if(arrayId == p[1][1]) then
			arrayName = p[1][2]
			break
		end
	end
	local nameIndex = g_GameMain.m_BattleArrayBase:GetArrayNameIndex(arrayName)
	tblMem[nameIndex] = {{arrayId, arrayName}, {id1, id2, id3, id4, id5}}
end

--返回列阵信息结束
function Gas2Gac:ReturnGetAllBattleArrayMemPosInfoEnd(Conn)
	--print("------列阵信息加载完毕！------")
end

--返回保存列阵
function Gas2Gac:ReturnSaveBattleArrayMemPos(Conn, bFlag)
	if(bFlag) then
		local tblPos			= g_GameMain.m_BattleArrayBase.m_tblBattleArrayPos
		local tblMemPosInfo		= g_GameMain.m_BattleArrayMemPosWnd.m_tblMemPosInfoTemp
		local tblBattleArrayMem	= g_GameMain.m_BattleArrayBase.m_tblBattleArrayMem
		local arrayName			= g_GameMain.m_BattleArrayMemPosWnd.m_sArrayName
		local nameIndex			= g_GameMain.m_BattleArrayBase:GetArrayNameIndex(arrayName)
		local arrayId			= tblPos[nameIndex][1][1]
		tblBattleArrayMem[nameIndex] = {{arrayId, arrayName}, {tblMemPosInfo[1][1], tblMemPosInfo[2][1], tblMemPosInfo[3][1], tblMemPosInfo[4][1], tblMemPosInfo[5][1]}}
		tblMemPosInfo = {}
		g_GameMain.m_BattleArrayMemPosWnd.m_bMemChanged = false
		g_GameMain.m_BattleArrayMemPosWnd:ShowWnd(false)
		g_GameMain.m_BattleArrayStoreWnd.m_Page = math.floor(nameIndex/12)
		g_GameMain.m_BattleArrayStoreWnd:OpenWnd(nameIndex)
		MsgClient(12001)
	else
		MsgClient(12002)
	end
end

--返回还原成阵法书
function Gas2Gac:ReturnRevertToBattleArrayBook(Conn, bFlag)
	if(bFlag) then
		local storeWnd	= g_GameMain.m_BattleArrayStoreWnd
		local base		= g_GameMain.m_BattleArrayBase
		local name		= storeWnd.m_TmpArrayName
		local item		= storeWnd.m_TmpItem
		local nameIndex	= base:GetArrayNameIndex(name)
		local tblPos	= base.m_tblBattleArrayPos
		
		tblPos[nameIndex][1][3] = 0
		g_WndMouse:ClearIconWnd(item)
		item:EnableWnd(false)
		name = ""
		item = {}
		base.m_tblBattleArrayMem[nameIndex] = nil
		
		if(storeWnd.FunCallBackDeleteMainShortCut) then
			storeWnd:FunCallBackDeleteMainShortCut()
		end
		if(storeWnd.FunCallBackSaveShortCut) then
			storeWnd:FunCallBackSaveShortCut()
		end
		storeWnd:OpenWnd(0)
	else
		MsgClient(12003)
	end
end

function Gas2Gac:ReturnSaveBattleArrayShortCutIndex(Conn, bFlag)
	if(bFlag) then
		local wnd = g_GameMain.m_BattleArrayStoreWnd
		if(wnd.FunCallBackSaveShortCut) then
			wnd:FunCallBackSaveShortCut()
		end
	else
		MsgClient(12004)
	end
end

function Gas2Gac:ReturnGetAllBattleSkill(Conn, Index, arrayName)
	g_GameMain.m_BattleArrayStoreWnd.m_tblShortCutInfo[Index] = arrayName
	local SmallIcon	= g_ItemInfoMgr:GetItemInfo(26, arrayName,"SmallIcon")
	local item		= g_GameMain.m_BattleArrayStoreWnd.m_tblShortCutWnd[Index].m_Btn
	g_LoadIconFromRes(SmallIcon, item, -2, IP_ENABLE, IP_CLICKDOWN)
end

--按U键打开阵法面板
function OpenBattleArrayStoreWnd()
	g_GameMain.m_BattleArrayStoreWnd:ShowWnd(not g_GameMain.m_BattleArrayStoreWnd:IsShow())
end