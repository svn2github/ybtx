
CStonePart = class ( SQRDialog )
CStoneMaterialListWnd = class( SQRDialog )
CStoneMaterialItemWnd = class( SQRDialog )
local PartName = {18001,18002,18003,18004,18005,18006,18007,18008,18009,18010,18011,18012}--确定下来后,从配置表里读

function CStonePart:InitPart()
	self.stoneMaterialListWnd = CStoneMaterialListWnd:new()
	self.stoneMaterialListWnd:CreateFromRes("StoneMaterialListWnd", self)
	self.stoneMaterialListWnd.materialList = self.stoneMaterialListWnd:GetDlgChild("MaterialList")
	self.NowSelectHoleID = nil       --目前选中的孔
	self.MaterialID = {} --目前选中的材料ID
	self.MaterialBigID = nil
	self.m_stoneID = nil
	self.m_NowINmaterial = nil
	self.m_Nowmaterial = nil    --目前选中的材料
	self:GetDlgChild("one"):ShowWnd(false)
	self:GetDlgChild("two"):ShowWnd(false)
	self:GetDlgChild("three"):ShowWnd(false)
	self:GetDlgChild("four"):ShowWnd(false)
	self:GetDlgChild("five"):ShowWnd(false)
	self:GetDlgChild("six"):ShowWnd(false)
	self:GetDlgChild("seven"):ShowWnd(false)
	self:GetDlgChild("eight"):ShowWnd(false)
	self.m_ShuXing		= self:GetDlgChild("shuxing")
	self.m_PartAdd		= self:GetDlgChild("partAdd")
	
	self.m_tblPart = {}
	for i = 1, 8 do
		self.m_tblPart[i] = self:GetDlgChild("Part" .. i)
	end
	
	self.m_Close = self:GetDlgChild("close")
	self.m_DaKong		= self:GetDlgChild("dakong")
	self.m_XiangQian	= self:GetDlgChild("xiangqian")
	self.m_ZhaiChu	= self:GetDlgChild("zhaichu")
	self:ShowWnd(false)
end

function CStonePart:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_DaKong) then
			local StoneWnd = CStone.GetWnd()
			self.m_DaKong:EnableWnd(false)
			Gac2Gas:RequestOpenHole(g_Conn,StoneWnd.FrameID,StoneWnd.PartID,self.NowSelectHoleID,self.MaterialID[1])
			--面板,部位,孔,材料id
		elseif(Child == self.m_XiangQian) then
			local StoneWnd = CStone.GetWnd()
			self.m_XiangQian:EnableWnd(false)
			Gac2Gas:RequestInlayStone(g_Conn,StoneWnd.FrameID,StoneWnd.PartID,self.NowSelectHoleID,self.MaterialID[1])
		elseif(Child == self.m_ZhaiChu) then
			local StoneWnd = CStone.GetWnd()
			local msg = g_MoneyMgr:ChangeMoneyToString(1000,EGoldType.GoldCoin)
			StoneWnd.m_MSG:GetDlgChild("WndInfo"):SetWndText(GetStaticTextClient(1071,msg))
			StoneWnd.m_MSG:ShowWnd(true)
		elseif(Child == self.m_Close) then
			self:Close()
		else
			for i = 1, 8 do
				if(Child == self.m_tblPart[i]) then
					self:DownButton(tostring(i))
				end
			end
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
		for i = 1, 8 do
			if(Child == self.m_tblPart[i]) then
				self:RDownButton(tostring(i))
			end
		end
	end
end

function CStonePart:Close()
	self:VirtualExcludeWndClosed()
	for i =1, 8 do
		g_DelWndImage(self.m_tblPart[i], 1, IP_ENABLE, IP_CLICKDOWN)   --取消添加物品
		self.m_tblPart[i]:SetMouseOverDescAfter("")
	end
	self.m_NowINmaterial = nil
	self.MaterialBigID = nil
	self.m_Nowmaterial = nil
	self.MaterialID = {}
	local StoneWnd = CStone.GetWnd()
	StoneWnd.PartID = nil
	StoneWnd:DrawStoneFrame(StoneWnd.FrameID)
	self:ShowWnd(false)
	local StoneWnd = CStone.GetWnd()
	StoneWnd.m_MSG:ShowWnd(false)
	StoneWnd.m_MSG:GetDlgChild("WndInfo"):SetWndText("")
end

function CStonePart:GetPosition()
	return self.m_Position
end

function CStonePart:VirtualExcludeWndClosed()
	local StoneWnd = CStone.GetWnd()
	if self.SelectGridTable == nil then
		return
	end
	for i=1, #( self.SelectGridTable ) do
		if self.SelectGridTable[i] ~= nil then
			local x, y, roomIndex = unpack(self.SelectGridTable[i],1,3)
			local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
			SrcCt:SetClickedWndState(x, y, roomIndex, true)
		end
	end
	self.SelectGridTable = {}
end

function CStonePart:DownButton( holename, nRoomIndex, nPos )--处理 点击孔事件, 后两个参数仅为右键点击包裹内物品时使用的参数
	local holeID = tonumber(holename)
	local pos = self:GetPosition()
	local state, context = g_WndMouse:GetCursorState()
	local fromRoom, fromRow, fromCol, srcPos
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	if state == ECursorState.eCS_PickupItem then --手上有物品
		fromRoom, fromRow, fromCol = context[1], context[2], context[3]
		srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
	end
	local Grid = nil
	if nRoomIndex and nPos then
		Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nRoomIndex, nPos)
	elseif fromRoom and srcPos then
		Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	end
	if Grid then
		self:VirtualExcludeWndClosed()
		DrawpartstoneFrame(pos)
		self:PartstoneTbl(Grid)
	end
	--任何一个button上都没有材料时才能放置
	local res=0
	local Attr_tbl = { "one","two","three","four","five","six","seven","eight" }
	
	g_WndMouse:ClearCursorAll()
	local StoneWnd = CStone.GetWnd()
	for i =1,#(StoneWnd.stonetbl) do
		if( StoneWnd.stonetbl[i][1] == StoneWnd.FrameID and pos == StoneWnd.stonetbl[i][2] and holeID == StoneWnd.stonetbl[i][3] ) then
			if( Grid ~= nil) then
				DrawpartstoneFrame(pos)
				MessageBox( g_GameMain, MsgBoxMsg(3000), MB_BtnOK )
				return
			end
			self.NowSelectHoleID = holeID
			DrawpartstoneFrame(pos)
			StoneWnd:DrawStoneFrame(StoneWnd.FrameID)
			self.m_XiangQian:EnableWnd(false)
			self.m_DaKong:EnableWnd(false)
			self.m_ZhaiChu:EnableWnd(true)
			return
		end
	end 
	if( Grid ~= nil and self.MaterialBigID and self.m_Nowmaterial ) then
		local res = JudgeButtonActive( pos, holeID )
		--res    1 <可以打孔>   2 <已经打孔>,<可以镶嵌> , 3 <已经镶嵌> 4 <前面的还没有打孔> 5 <前面的还没有镶嵌> 
		local tab = {}
		local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(self.MaterialBigID,self.m_Nowmaterial)--BigID,m_Nowmaterial 在包裹中点击宝石时得到的
		local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(self.MaterialBigID,self.m_Nowmaterial)
		if( self.MaterialBigID ==g_ItemInfoMgr:GetStoneBigID() ) then  --是宝石
			tab ={ ItemInfo("Depart1"), ItemInfo("Depart2"),ItemInfo("Depart3"),ItemInfo("Depart4"),ItemInfo("Depart5"),ItemInfo("Depart6"),ItemInfo("Depart7"),ItemInfo("Depart8"),ItemInfo("Depart9"),ItemInfo("Depart10"),ItemInfo("Depart11"),ItemInfo("Depart12") }
		end
		if( res == 1 ) then--可以打孔
			local success = false
			if HoleMaterial_Common(self.m_Nowmaterial) ~= nil and HoleMaterial_Common(self.m_Nowmaterial,"Level") == tonumber(holename) then
				--有等级限制
				local playerLevel = g_MainPlayer:CppGetLevel()  --角色等级
				local Level_Limit_flag = true
				if( playerLevel < 20 and tonumber(holename) >= 2) then
					Level_Limit_flag = false
				end
				if( playerLevel < 40 and tonumber(holename) >= 4) then
					Level_Limit_flag = false
				end
				if( playerLevel < 60 and tonumber(holename) >= 6) then
					Level_Limit_flag = false
				end
				if( playerLevel < 70 and tonumber(holename) == 8) then
					Level_Limit_flag = false
				end
				if not Level_Limit_flag then
					self.m_NowINmaterial = nil
					self.MaterialBigID = nil
					self.m_Nowmaterial = nil
					self.MaterialID = {}
					DrawpartstoneFrame(pos)
					MessageBox( g_GameMain, MsgBoxMsg(3001), MB_BtnOK )
					return
				end
				self.m_NowINmaterial = self.m_Nowmaterial
				success = true
			end
			if not success then
				self.MaterialID = {}

				DrawpartstoneFrame(pos)
				if g_ItemInfoMgr:IsStone(self.MaterialBigID) then
					MessageBox( g_GameMain, MsgBoxMsg(3013), MB_BtnOK )
				else
					MessageBox( g_GameMain, MsgBoxMsg(3002), MB_BtnOK )
				end
				self.m_NowINmaterial = nil
				self.MaterialBigID = nil
				self.m_Nowmaterial = nil
				return
			end
			
			if state == ECursorState.eCS_PickupItem then --手上有物品
				local cols = g_PlayerMainBag.columns
				if(self.SelectGridTable == nil)then
					self.SelectGridTable = {}
				end 
				local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
				table.insert(self.SelectGridTable, {x, y, fromRoom})
				SrcCt:SetClickedWndState(x, y, fromRoom, false)
			else
				local srcPos = nPos
				local cols = g_PlayerMainBag.columns
				if(self.SelectGridTable == nil)then
					self.SelectGridTable = {}
				end
				local y, x = SrcCt:GetIndexByPos(srcPos, nRoomIndex)
				table.insert(self.SelectGridTable, {x, y, nRoomIndex})
				SrcCt:SetClickedWndState(x, y, nRoomIndex, false)
			end
			self.NowSelectHoleID = holeID
			g_LoadIconFromRes(ItemInfo("SmallIcon"), self.m_tblPart[tonumber(holename)], -1, IP_ENABLE, IP_CLICKDOWN) --显示打孔材料图片
			g_SetItemRichToolTips(self.m_tblPart[tonumber(holename)],self.MaterialBigID,self.m_Nowmaterial, 0)
			--材料是否为空, 空<提示需要材料>,判断材料是否是打孔材料, 并且等级符合
			self.m_DaKong:EnableWnd(true)
			self.m_XiangQian:EnableWnd(false)
			self.m_ZhaiChu:EnableWnd(false)
		elseif( res == 2 ) then --可以镶嵌
			--材料是否为空, 空<提示需要材料> ,判断材料是否是宝石
			self.m_NowINmaterial = self.m_Nowmaterial
			if(#(tab) ~= 0) then
				if( tab[pos] == 1) then   --宝石属性符合,可以放置
					if state == ECursorState.eCS_PickupItem then --手上有物品
						local cols = g_PlayerMainBag.columns
						if(self.SelectGridTable == nil)then
							self.SelectGridTable = {}
						end 
						local y, x = SrcCt:GetIndexByPos(srcPos, fromRoom)
						table.insert(self.SelectGridTable, {x, y, fromRoom})
						SrcCt:SetClickedWndState(x, y, fromRoom, false)
					else
						local srcPos = nPos
						local cols = g_PlayerMainBag.columns
						if(self.SelectGridTable == nil)then
							self.SelectGridTable = {}
						end
						local y, x = SrcCt:GetIndexByPos(srcPos, nRoomIndex)
						table.insert(self.SelectGridTable, {x, y, nRoomIndex})
						SrcCt:SetClickedWndState(x, y, nRoomIndex, false)
					end
					self.NowSelectHoleID = holeID
					g_LoadIconFromRes(ItemInfo("SmallIcon"), self.m_tblPart[tonumber(holename)], -1, IP_ENABLE, IP_CLICKDOWN) --显示宝石图片
					g_SetItemRichToolTips(self.m_tblPart[tonumber(holename)],self.MaterialBigID,self.m_Nowmaterial, 0)
					self:GetDlgChild(Attr_tbl[self.NowSelectHoleID]):SetWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("AttrValue"))
					--宝石面板 总属性预览
					local flag = false
					local Add4ValueStr = ""
					local Add8ValueStr = ""
					local Addr4,Addr8,Addr4Value,Addr8Value,Addr4Type,Addr8Type,ShowAddr4Type,ShowAddr8Type = self:CheckAddr( pos, self.NowSelectHoleID )
					for j =1, #(StoneWnd.wholeAttr) do
						--Adder4,Adder8 是否可附加属性加成
						--Addr4Value 当这个值不为0时表示这次镶嵌式1-3孔,Addr4Value表示以前镶嵌的第4个空的属性附加值
						--	为0则这次镶嵌的是第4个空,直接去ItemInfo上的值即可,Addr8Value同样
						if(StoneWnd.wholeAttr[j][1] == ItemInfo("AttrType")) then
							flag = true
							StoneWnd.wholeAttr[j][2] = StoneWnd.wholeAttr[j][2] + ItemInfo("AttrValue")
							if Addr4 then
								if Addr4Value == 0 then
									StoneWnd.wholeAttr[j][2] = StoneWnd.wholeAttr[j][2] + ItemInfo("Award4")
									Add4ValueStr = ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award4")
								elseif Addr4Type == ItemInfo("AttrType") then
									StoneWnd.wholeAttr[j][2] = StoneWnd.wholeAttr[j][2] + Addr4Value
									Add4ValueStr = ItemLanInfo("ShowAttrType").."+"..Addr4Value
								else
									for l =1, #(StoneWnd.wholeAttr) do
										if(StoneWnd.wholeAttr[l][1] == Addr4Type) then
											StoneWnd.wholeAttr[l][2] = StoneWnd.wholeAttr[l][2] + Addr4Value
											Add4ValueStr = ShowAddr4Type.."+"..Addr4Value
										end
									end
								end
							end
							if Addr8 then
								if Addr8Value == 0 then
									StoneWnd.wholeAttr[j][2] = StoneWnd.wholeAttr[j][2] + ItemInfo("Award8")
									Add8ValueStr = ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award8")
								elseif Addr8Type == ItemInfo("AttrType") then
									StoneWnd.wholeAttr[j][2] = StoneWnd.wholeAttr[j][2] + Addr8Value
									Add8ValueStr = ItemLanInfo("ShowAttrType").."+"..Addr8Value
								else
									for l =1, #(StoneWnd.wholeAttr) do
										if(StoneWnd.wholeAttr[l][1] == Addr8Type) then
											StoneWnd.wholeAttr[l][2] = StoneWnd.wholeAttr[l][2] + Addr8Value
											Add8ValueStr = ShowAddr8Type.."+"..Addr8Value
										end
									end
								end
							end
							break
						end
					end
					if(not flag) then
						table.insert(StoneWnd.wholeAttr,{ItemLanInfo("ShowAttrType"),ItemInfo("AttrValue")})
						if Addr4 then
							if Addr4Value == 0 then
								StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] = StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] + ItemInfo("Award4")
								Add4ValueStr = ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award4")
							elseif Addr4Type == ItemInfo("AttrType") then
								StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] = StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] + Addr4Value
								Add4ValueStr = ItemLanInfo("ShowAttrType").."+"..Addr4Value
							else
								for l =1, #(StoneWnd.wholeAttr) do
									if(StoneWnd.wholeAttr[l][1] == Addr4Type) then
										StoneWnd.wholeAttr[l][2] = StoneWnd.wholeAttr[l][2] + Addr4Value
										Add4ValueStr = ShowAddr4Type.."+"..Addr4Value
									end
								end
							end
						end
						if Addr8 then
							if Addr8Value == 0 then
								StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] = StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] + ItemInfo("Award8")
								Add8ValueStr = ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award8")
							elseif Addr8Type == ItemInfo("AttrType") then
								StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] = StoneWnd.wholeAttr[#(StoneWnd.wholeAttr)][2] + Addr8Value
								Add8ValueStr = ItemLanInfo("ShowAttrType").."+"..Addr8Value
							else
								for l =1, #(StoneWnd.wholeAttr) do
									if(StoneWnd.wholeAttr[l][1] == Addr8Type) then
										StoneWnd.wholeAttr[l][2] = StoneWnd.wholeAttr[l][2] + Addr8Value
										Add8ValueStr = ShowAddr8Type.."+"..Addr8Value
									end
								end
							end
						end
					end
					StoneWnd.attrWnd.AttrText:SetWndText(GetStaticTextClient(1072))
					for i =1, #(StoneWnd.wholeAttr) do
						StoneWnd.attrWnd.AttrText:AddWndText("#r"..ItemLanInfo("ShowAttrType").."+"..StoneWnd.wholeAttr[i][2])
					end
					--在部位面板上显示部位总体属性值
					local flag = false
					for j =1, #(self.partwholeAttr) do
						if(self.partwholeAttr[j][1] == ItemInfo("AttrType")) then
							flag = true
							self.partwholeAttr[j][2] = self.partwholeAttr[j][2] + ItemInfo("AttrValue")
							break
						end
					end
					if(not flag) then
						table.insert(self.partwholeAttr,{ItemInfo("AttrType"),ItemInfo("AttrValue"),ItemLanInfo("ShowAttrType")})
					end
					self.m_ShuXing:SetWndText("")
					for j =1, #(self.partwholeAttr) do
						self.m_ShuXing:AddWndText(self.partwholeAttr[j][3].."+"..self.partwholeAttr[j][2].."#r")
					end
					if( Add4ValueStr ~= "") then
						self.m_ShuXing:AddWndText(GetStaticTextClient(18022) .. "#r")
						self.m_ShuXing:AddWndText(Add4ValueStr.."#r")
					end
					if(  Add8ValueStr ~= "" ) then
						self.m_ShuXing:AddWndText(GetStaticTextClient(18023) .. "#r")
						self.m_ShuXing:AddWndText(Add8ValueStr.."#r")
					end
					self.m_XiangQian:EnableWnd(true)
					self.m_DaKong:EnableWnd(false)
					self.m_ZhaiChu:EnableWnd(false)
				else
					self.MaterialID = {}
					MessageBox( g_GameMain, MsgBoxMsg(3003), MB_BtnOK )
					self.m_XiangQian:EnableWnd(false)
					self.m_DaKong:EnableWnd(false)
					self.m_ZhaiChu:EnableWnd(false)
				end
			else
				self.MaterialID = {}
				MessageBox( g_GameMain, MsgBoxMsg(3004), MB_BtnOK )
				self.m_XiangQian:EnableWnd(false)
				self.m_DaKong:EnableWnd(false)
				self.m_ZhaiChu:EnableWnd(false)
			end
		elseif( res == 3) then --已经镶嵌, 摘除按钮激活
			self.MaterialID = {}
			self.NowSelectHoleID = holeID
			self.m_XiangQian:EnableWnd(false)
			self.m_DaKong:EnableWnd(false)
			self.m_ZhaiChu:EnableWnd(true)
		elseif( res == 4) then--前面有未开启的孔
			self.MaterialID = {}
			DrawpartstoneFrame(pos)
			MessageBox( g_GameMain, MsgBoxMsg(3005), MB_BtnOK )
		elseif( res == 5) then --前面有未镶嵌的凹槽
			self.MaterialID = {}
			DrawpartstoneFrame(pos)
			if(#(tab) == 0) then
				MessageBox( g_GameMain, MsgBoxMsg(3004), MB_BtnOK )
			else
				MessageBox( g_GameMain, MsgBoxMsg(3006), MB_BtnOK )
			end
		end
	else
		local res = JudgeButtonActive( pos, holeID )
		if( res == 3) then --已经镶嵌, 摘除按钮激活
			self.MaterialID = {}
			self.NowSelectHoleID = holeID
			self.m_XiangQian:EnableWnd(false)
			self.m_DaKong:EnableWnd(false)
			self.m_ZhaiChu:EnableWnd(true)
		elseif ( res == 2) then --已经打孔, 弹出可镶嵌的宝石列表
			self:DrawMaterialList(holename, res)
		elseif ( res == 1) then --可以打孔
			self:DrawMaterialList(holename, res)
		end		
	end
	self.MaterialBigID = nil
	self.m_Nowmaterial = nil
end

function CStonePart:RDownButton( holename )--处理 右键点击孔事件
	local pos = self:GetPosition()
	if #(self.MaterialID) ~= 0 and tonumber(holename) == self.NowSelectHoleID then
		self:VirtualExcludeWndClosed()
		DrawpartstoneFrame(pos)
		local StoneWnd = CStone.GetWnd()
		StoneWnd:DrawStoneFrame(StoneWnd.FrameID)
		self.m_XiangQian:EnableWnd(false)
		self.m_DaKong:EnableWnd(false)
		self.m_ZhaiChu:EnableWnd(false)
	end
end

--包裹内直接右键使用宝石
function CStonePart:UseStone(nBigID, nSmallID, nRoomIndex, nPos )
	if self:IsShow() == false or nRoomIndex==nil or nPos==nil then
		return
	end
	for i=1, 8 do
		local pos = self:GetPosition()
		local res = JudgeButtonActive( pos, i )
		if res == 2 then
			self:DownButton( tostring(i), nRoomIndex, nPos)
		end
	end
end

function CStonePart:OnLButtonDown( wParam, x, y )
	local state, context = g_WndMouse:GetCursorState()
	if state ~= ECursorState.eCS_PickupItem then --手上有物品
		return
	end
	local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
	local fromRoom, fromRow, fromCol = context[1], context[2], context[3]
	local srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
	local Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	local BigId, Index = Grid:GetType()
	for i=1, 8 do
		local pos = self:GetPosition()
		local res = JudgeButtonActive( pos, i )
		if res == 1 and BigId == g_ItemInfoMgr:GetHoleMaterialBigID() then
			self:DownButton( tostring(i))
			break
		end
		if res == 2 and BigId == g_ItemInfoMgr:GetStoneBigID() then
			self:DownButton( tostring(i))
			break
		end
	end
end

--包裹内直接右键使用打孔材料
function CStonePart:UseHoleMaterial(nBigID, nSmallID, nRoomIndex, nPos )
	if self:IsShow() == false or nRoomIndex==nil or nPos==nil then
		return
	end
	for i=1, 8 do
		local pos = self:GetPosition()
		local res = JudgeButtonActive( pos, i )
		if res == 1 then
			self:DownButton( tostring(i), nRoomIndex, nPos )
		end
	end
end

function CStonePart:DrawMaterialList(holename, op)
	self.stoneMaterialListWnd.materialList:DeleteAllItem()
	self.stoneMaterialListWnd.tblMaterialWnd = {}
	self.stoneMaterialListWnd.ExisttblMaterial = {}		--存在并且可以在此孔防止的材料(打孔材料OR宝石)tbl
	self.stoneMaterialListWnd.NotExisttblMaterial = {}	--不存在但可以在此孔防止的材料(打孔材料OR宝石)tbl
	self.stoneMaterialListWnd.materialList:InsertColumn( 0, self.stoneMaterialListWnd.materialList:GetWndOrgWidth() )
	if op == 1 then
		local MaterialIndex = g_ItemInfoMgr:GetHoleMaterialBigID()
		local keys = HoleMaterial_Common:GetKeys()
		for j, key in pairs(keys) do
			local p = HoleMaterial_Common(key)
			if p("Level") == tonumber(holename) then
				local count = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,MaterialIndex, p("Name") )
				local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(g_ItemInfoMgr:GetHoleMaterialBigID(),p("Name"))
				if count == 0 then
					table.insert(self.stoneMaterialListWnd.NotExisttblMaterial,{p("Name"), count, holename, op, ItemLanInfo("DisplayName")})
				else
					table.insert(self.stoneMaterialListWnd.ExisttblMaterial,{p("Name"), count, holename, op, ItemLanInfo("DisplayName")})
				end
			end
		end
	elseif op == 2 then
		local keys = Stone_Common:GetKeys()
		local StoneIndex = g_ItemInfoMgr:GetStoneBigID()
		for j, key in pairs(keys) do
			local p = Stone_Common(key)
			local StoneWnd = CStone.GetWnd()
			if p("Depart"..StoneWnd.PartID) == 1 then
				local count = g_GameMain.m_BagSpaceMgr:GetItemCountBySpace(g_StoreRoomIndex.PlayerBag,StoneIndex, p("Name") )
				local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(g_ItemInfoMgr:GetStoneBigID(),p("Name"))
				if count == 0 then
					table.insert(self.stoneMaterialListWnd.NotExisttblMaterial,{p("Name"), count, holename, op, ItemLanInfo("DisplayName")})
				else
					table.insert(self.stoneMaterialListWnd.ExisttblMaterial,{p("Name"), count, holename, op, ItemLanInfo("DisplayName")})
				end
			end
		end
	end
	self.stoneMaterialListWnd:AutoInsertItem()
	--self.stoneMaterialListWnd.materialList
	local Rect=CFRect:new()
	self.stoneMaterialListWnd:GetLogicRect(Rect)
	local uWidth = Rect.right - Rect.left
	local uHeight = Rect.bottom - Rect.top
	local ParentRect=CFRect:new()
	self.m_tblPart[tonumber(holename)]:GetLogicRect(ParentRect)
	Rect.left 	= ParentRect.right
	Rect.top  	= ParentRect.top
	Rect.right	= Rect.left + uWidth
	Rect.bottom	= ParentRect.top + uHeight
	self.stoneMaterialListWnd:SetLogicRect(Rect)
	self.stoneMaterialListWnd:ShowWnd(true)
	self.stoneMaterialListWnd:SetFocus()
end

--由ItemID得到在包裹中的位置,用于孔的列表功能
function CStonePart:GetSlotIndexByName( ItemName )
	local SpaceMgr = g_GameMain.m_BagSpaceMgr
	local space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
	local BagSize = space:Size()
	for i = 1, BagSize do
		local fromRoom,srcPos 
		if(i > g_PlayerMainBag.size) then  
			fromRoom,srcPos = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(i)
		else
			fromRoom = g_StoreRoomIndex.PlayerBag
			srcPos = i
		end
		local CurGrid = SpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
		local nBigID,nIndex = CurGrid:GetType()
		if (not CurGrid:Empty()) and  (nBigID==g_ItemInfoMgr:GetHoleMaterialBigID() or nBigID==g_ItemInfoMgr:GetStoneBigID()) then
			if ItemName == nIndex then
				return fromRoom, srcPos, CurGrid
			end
--			local vWnd     = self._m_CDialog[i]
--			local num  = CurGrid:Size()
--			local GlobalID = CurGrid:GetItemID(num)
--			for j=1,#(GlobalID) do
--				if GlobalID[j] == ItemID then
--					return fromRoom, srcPos
--				end
--			end
		end
	end
	return nil,nil,nil
end

function CStonePart:CheckAddr( partID, holeID )--处理 点击孔事件
	local Addr4 = false
	local Addr8 = false
	local Addr4Value = 0
	local Addr8Value = 0
	local Addr4Type = 0
	local Addr8Type = 0
	local ShowAddr4Type = ""
	local ShowAddr8Type = ""
	for m = 1, 8 do
		local StoneWnd = CStone.GetWnd()
		for i=1,#(StoneWnd.stonetbl) do
			if( StoneWnd.stonetbl[i][1] ==StoneWnd.FrameID and StoneWnd.stonetbl[i][2] == partID and StoneWnd.stonetbl[i][3] == m ) then
				if m== 4 then
					local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
					local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
					Addr4Value = ItemInfo("Award4")
					Addr4Type  = ItemInfo("AttrType")
					ShowAddr4Type = ItemLanInfo("ShowAttrType")
					for l = 1,4 do
						local res = JudgeButtonActive( partID, l )
						if  res ~= 3 and holeID ~= l then
							break
						elseif l == 4 then
							Addr4 = true
						end
					end
				elseif m==8 and holeID<=8 then
					local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
					local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
					Addr8Value = ItemInfo("Award8")
					Addr8Type  = ItemInfo("AttrType")
					ShowAddr8Type = ItemLanInfo("ShowAttrType")
					for l = 1,8 do
						local res = JudgeButtonActive( partID, l )
						if  res ~= 3 and holeID ~= l then
							break
						elseif l == 8 then
							Addr8 = true
						end
					end
				end
				break
			end
			if i == #(StoneWnd.stonetbl) then
				if m == holeID then
					if m== 4 and holeID<=4 then
						Addr4 = true
					elseif m==8 and holeID<=8 then
						Addr8 = true
					end
				end
			end
		end
	end
	return Addr4,Addr8,Addr4Value,Addr8Value,Addr4Type,Addr8Type,ShowAddr4Type,ShowAddr8Type
end

function CStonePart:PartstoneTbl(Grid)  
	local num = Grid:Size()
	self.MaterialID = Grid:GetItemID(num)
	local BigId, Index = Grid:GetType()
	--local GlobalID = Grid:GetItem():GetItemID()
	if not Grid:Empty() then
		self.m_Nowmaterial =Index
		self.MaterialBigID = BigId
	else
		self.m_Nowmaterial = nil
	end
end


function CStoneMaterialListWnd:AutoInsertItem()
	self.materialList:InsertColumn( 0, self.materialList:GetWndOrgWidth() )
	--根据排序功能  只能简单排序, 如果名字特殊(汉字的)排序就不准确
	--可以用Level列来排序, 但要区分不同的宝石,即每种宝石都要能区分, 可以用AttrType来区分,但每种AttrType只有一种宝石...
	table.sort(self.ExisttblMaterial, function (a, b)
		if(string.lower(a[1]) < string.lower(b[1])) then
			return a[1] < b[1]
		end
	end)
	table.sort(self.NotExisttblMaterial, function (a, b)
		if(string.lower(a[1]) < string.lower(b[1])) then
			return a[1] < b[1]
		end
	end)
	for i=1, # self.ExisttblMaterial do	
		self.materialList:InsertItem(#(self.tblMaterialWnd), 20)
		local embedvalue = CStoneMaterialItemWnd:new()
		local item = self.materialList:GetSubItem( #(self.tblMaterialWnd), 0 )  
		item:SetStyle(0x44440000)
		embedvalue:CreateFromRes( "StoneMaterialItemWnd", item) 
		embedvalue:ShowWnd( true )
		local itemvalue = item:GetDlgChild( "StoneMaterialItemWnd" )
		itemvalue:GetDlgChild("materialName"):SetMouseWheel(true)
		itemvalue:GetDlgChild("materialCount"):SetMouseWheel(true)
		local Flag = IMAGE_PARAM:new(SM_BS_BK, IP_ENABLE)
		itemvalue:GetDlgChild("materialName"):SetWndTextColor(Flag,"0xffffffff" )
		itemvalue:GetDlgChild("materialName"):SetWndText(self.ExisttblMaterial[i][5])
		itemvalue.m_MaterialName = self.ExisttblMaterial[i][1]
		itemvalue:GetDlgChild("materialCount"):SetWndText("#cffffff".."X"..self.ExisttblMaterial[i][2])
		if self.ExisttblMaterial[i][4] == 1 then
			g_SetItemRichToolTips(itemvalue:GetDlgChild("materialName"),g_ItemInfoMgr:GetHoleMaterialBigID(), self.ExisttblMaterial[i][1], 0)
		else
			g_SetItemRichToolTips(itemvalue:GetDlgChild("materialName"),g_ItemInfoMgr:GetStoneBigID(), self.ExisttblMaterial[i][1], 0)
		end
		table.insert(self.tblMaterialWnd, {itemvalue, self.ExisttblMaterial[i][3], self.ExisttblMaterial[i][1]})
	end
	for j =1, # self.NotExisttblMaterial do
		self.materialList:InsertItem(#(self.tblMaterialWnd), 20)
		local embedvalue = CStoneMaterialItemWnd:new()
		local item = self.materialList:GetSubItem( #(self.tblMaterialWnd), 0 )  
		item:SetStyle(0x44440000)
		embedvalue:CreateFromRes( "StoneMaterialItemWnd", item) 
		embedvalue:ShowWnd( true )
		local itemvalue = item:GetDlgChild( "StoneMaterialItemWnd" )
		--itemvalue:SetMouseWheel(true)
		itemvalue:GetDlgChild("materialName"):SetMouseWheel(true)
		itemvalue:GetDlgChild("materialCount"):SetMouseWheel(true)
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE	
		itemvalue:GetDlgChild("materialName"):SetWndTextColor(Flag,"0xff6f858c" )
		itemvalue:GetDlgChild("materialName"):SetWndText(self.NotExisttblMaterial[j][5])
		itemvalue.m_MaterialName = self.NotExisttblMaterial[j][1]
		if self.NotExisttblMaterial[j][4] == 1 then
			g_SetItemRichToolTips(itemvalue:GetDlgChild("materialName"),g_ItemInfoMgr:GetHoleMaterialBigID(), self.NotExisttblMaterial[j][1], 0)
		else
			g_SetItemRichToolTips(itemvalue,g_ItemInfoMgr:GetStoneBigID(), self.NotExisttblMaterial[j][1], 0)
		end
		table.insert(self.tblMaterialWnd, {itemvalue, self.NotExisttblMaterial[j][3], self.NotExisttblMaterial[j][1]})
	end
end

function CStoneMaterialListWnd:OnActive(bActive)
	self:ShowWnd(bActive)
end

function CStoneMaterialItemWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if ( uMsgID == BUTTON_LCLICK ) then
		if Child == self:GetDlgChild("materialName") then
			local fromRoom, srcPos, CurGrid
			local StoneWnd = CStone.GetWnd()
			for i=1, #(StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd) do
				if StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd[i][1].m_MaterialName == self.m_MaterialName then
					fromRoom, srcPos, CurGrid = StoneWnd.StonePartUsing:GetSlotIndexByName(StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd[i][3])
				end
			end
			if fromRoom==nil or srcPos==nil or CurGrid==nil then
				return
			end
			for i=1, #(StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd) do
				local IsGridUnlocked = false
				local SpaceMgr = g_GameMain.m_BagSpaceMgr
				local space = SpaceMgr:GetSpace(g_StoreRoomIndex.PlayerBag)
				local BagSize = space:Size()
				for i = 1, BagSize do
					if(i > g_PlayerMainBag.size) then  
						fromRoom,srcPos = g_GameMain.m_WndBagSlots.m_lcAddBagSlots:GetSlotIndex(i)
					else
						fromRoom = g_StoreRoomIndex.PlayerBag
						srcPos = i
					end
					CurGrid = SpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
					local type, name = CurGrid:GetType()
					local b = CurGrid:GetEnable()
					if CurGrid:GetEnable() and  name == self.m_MaterialName then
						IsGridUnlocked = true
						break
					end
				end
				if IsGridUnlocked == false then 
					MessageBox( g_GameMain, MsgBoxMsg(3015), MB_BtnOK )
					return
				end
				if StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd[i][1].m_MaterialName == self.m_MaterialName and IsGridUnlocked then
					StoneWnd.StonePartUsing:DownButton( StoneWnd.StonePartUsing.stoneMaterialListWnd.tblMaterialWnd[i][2], fromRoom, srcPos)
					break
				end
			end
			StoneWnd.StonePartUsing.stoneMaterialListWnd:ShowWnd(false)
		end
	end
end

function DrawpartstoneFrame( partname ) --初始化部位面板
	local StoneWnd = CStone.GetWnd()
	local LastOpenPartUsing = StoneWnd.StonePartUsing
	StoneWnd.StonePartUsing = StoneWnd.stonepart[partname]		--当前打开的StonePart
	if LastOpenPartUsing ~= StoneWnd.StonePartUsing then
		LastOpenPartUsing:Close()
	end
	StoneWnd.StonePartUsing:ShowWnd(true)
	StoneWnd.StonePartUsing.stoneMaterialListWnd:ShowWnd(false)
	StoneWnd:DrawStoneFrame(StoneWnd.FrameID)
	StoneWnd.StonePartUsing:VirtualExcludeWndClosed()
	for i =1, 8 do
		g_DelWndImage(StoneWnd.StonePartUsing.m_tblPart[i], 1, IP_ENABLE, IP_CLICKDOWN)
		StoneWnd.StonePartUsing.m_tblPart[i]:SetMouseOverDescAfter("")
	end
	local Attr_tbl = { "one","two","three","four","five","six","seven","eight" }
	for i =1,#(Attr_tbl) do
		StoneWnd.StonePartUsing:GetDlgChild(Attr_tbl[i]):SetWndText("")
	end
	StoneWnd.StonePartUsing.m_PartAdd:SetWndText("")
	StoneWnd.StonePartUsing.partwholeAttr ={}
	for i =1, #(StoneWnd.stonetbl) do  --在宝石面板上显示总体属性值
		local flag = false
		local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
		local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
		if(StoneWnd.stonetbl[i][1] == StoneWnd.FrameID and StoneWnd.stonetbl[i][2] == partname) then
			for j =1, #(StoneWnd.StonePartUsing.partwholeAttr) do
				if(StoneWnd.StonePartUsing.partwholeAttr[j][1] == ItemInfo("AttrType")) then
					flag = true
					StoneWnd.StonePartUsing.partwholeAttr[j][2] = StoneWnd.StonePartUsing.partwholeAttr[j][2] + ItemInfo("AttrValue")
					break
				end
			end
			if(not flag) then
				table.insert(StoneWnd.StonePartUsing.partwholeAttr,{ItemInfo("AttrType"),ItemInfo("AttrValue"),ItemLanInfo("ShowAttrType")})
			end
		end
	end
	StoneWnd.StonePartUsing.m_ShuXing:SetWndText("")
	for i =1, #(StoneWnd.StonePartUsing.partwholeAttr) do
		StoneWnd.StonePartUsing.m_ShuXing:AddWndText(StoneWnd.StonePartUsing.partwholeAttr[i][3].."+"..StoneWnd.StonePartUsing.partwholeAttr[i][2].."#r")
	end
	local ItemInfo
	local itemLanInfo
	if(#(StoneWnd.stonetbl) >= 4) then
		local isAdd = false
		local flg = true
		for n =1 ,4 do
			for j =1 ,#(StoneWnd.stonetbl) do
				if(StoneWnd.stonetbl[j][1] == StoneWnd.FrameID and StoneWnd.stonetbl[j][2] == partname and StoneWnd.stonetbl[j][3] == n) then
					isAdd = true
					ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[j][4],StoneWnd.stonetbl[j][5])
					ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[j][4],StoneWnd.stonetbl[j][5])
					break
				elseif( j == #(StoneWnd.stonetbl) ) then
					isAdd = false
					flg = false	
					break
				end
			end
			if(not flg) then
				break
			end
		end
		if( isAdd ) then
			StoneWnd.StonePartUsing.m_ShuXing:AddWndText(GetStaticTextClient(18022) .. "#r")
			StoneWnd.StonePartUsing.m_ShuXing:AddWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award4").."#r")
		end
	end
	if(#(StoneWnd.stonetbl) >= 8) then
		local isAdd = false
		local flg = true
		for n =1 ,8 do
			for j =1 ,#(StoneWnd.stonetbl) do
				if(StoneWnd.stonetbl[j][1] == StoneWnd.FrameID and StoneWnd.stonetbl[j][2] == partname and StoneWnd.stonetbl[j][3] == n) then
					isAdd = true
					ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[j][4],StoneWnd.stonetbl[j][5])
					ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[j][4],StoneWnd.stonetbl[j][5])
					break
				elseif( j == #(StoneWnd.stonetbl) ) then
					isAdd = false
					flg = false	
					break
				end
			end
			if(not flg) then
				break
			end
		end
		if( isAdd ) then
			StoneWnd.StonePartUsing.m_ShuXing:AddWndText(GetStaticTextClient(18023) .. "#r")
			StoneWnd.StonePartUsing.m_ShuXing:AddWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award8").."#r")
		end
	end
	--StoneWnd.StonePartUsing:SetWndText(GetPartDisplayName(partname))
	--StoneWnd.StonePartUsing:GetDlgChild("part"):SetWndText(GetPartDisplayName(partname))
	StoneWnd.StonePartUsing.m_PartAdd:SetWndText(GetPartDisplayName(partname)..GetStaticTextClient(18019))
	
	StoneWnd.PartID = partname
	for i=1, #(StoneWnd.stonetbl) do
		if( StoneWnd.stonetbl[i][1] == StoneWnd.FrameID ) then
			if( StoneWnd.stonetbl[i][2] == partname ) then--判断宝石对应的部位和选中的部位相同, 则显示镶嵌宝石
				ItemInfo = g_ItemInfoMgr:GetItemFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
				ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5])
				g_LoadIconFromRes(ItemInfo("SmallIcon"), StoneWnd.StonePartUsing.m_tblPart[StoneWnd.stonetbl[i][3]], -1, IP_ENABLE, IP_CLICKDOWN) 
				g_SetItemRichToolTips(StoneWnd.StonePartUsing.m_tblPart[StoneWnd.stonetbl[i][3]],StoneWnd.stonetbl[i][4],StoneWnd.stonetbl[i][5], 0)
				StoneWnd.StonePartUsing:GetDlgChild(Attr_tbl[StoneWnd.stonetbl[i][3]]):SetWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("AttrValue"))
			end
		end
	end
	StoneWnd.StonePartUsing.m_DaKong:EnableWnd(false)
	StoneWnd.StonePartUsing.m_XiangQian:EnableWnd(false)
	StoneWnd.StonePartUsing.m_ZhaiChu:EnableWnd(false)
end

function CStonePart:NotifyShowAddAttr(BigId,Index)
	local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(BigId,Index)
	local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(BigId,Index)
	if( holePos == 4 and ItemInfo) then
		self.m_ShuXing:AddWndText(GetStaticTextClient(18022) .. "#r")
		self.m_ShuXing:AddWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award4").."#r")
	elseif( holePos == 8 and ItemInfo) then
		self.m_ShuXing:AddWndText(GetStaticTextClient(18023) .. "#r")
		self.m_ShuXing:AddWndText(ItemLanInfo("ShowAttrType").."+"..ItemInfo("Award8").."#r")
	end
end

function GetPartDisplayName( partIndex )
	local uMsgID = PartName[partIndex]
	if uMsgID then
		return GetStaticTextClient(uMsgID)
	end
end
