gac_require "message/message_box/MsgBoxMsg"

CStoneCompound = class ( SQRDialog )
function CStoneCompound:Ctor( Parent )
	self:CreateFromRes( "Stonecompound" , Parent )
	self:GetDlgChild("tishi1"):SetTransparent( 0 )--控件透明
	self:GetDlgChild("tishi2"):SetTransparent( 0 )
	self.FoldLimit 			= nil
	self.m_material 		= {}  --保存放在材料槽中的物品名字 和 id
	self.MaterialID 		= {}  --点击包裹中的物品时保存ID
	self.m_highStoneID 	= {}
	self.m_Nowmaterial 	=nil   --nIndex
	self.MaterialBigID 		= nil     --BigID
	self.m_Compound 		= self:GetDlgChild("compound")
	self.m_Batchcompound 	= self:GetDlgChild("batchcompound")
	self.m_Lowstone 	= self:GetDlgChild("lowstone")
	self.m_Highstone 	= self:GetDlgChild("highstone")
	self.m_Close 			= self:GetDlgChild("close")
	self.m_Money_Gold		= self:GetDlgChild("money1")
	self.m_Money_Argent	= self:GetDlgChild("money2")
	self.m_Money_Copper	= self:GetDlgChild("money3")
	self.m_Compound:EnableWnd(false)
	self.m_Batchcompound:EnableWnd(false)

	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CStoneCompound.GetWnd()
	local Wnd = g_GameMain.m_stonecompound
	if not Wnd then
		Wnd = CStoneCompound:new(g_GameMain)
		g_GameMain.m_stonecompound = Wnd
	end
	return Wnd
end

function CStoneCompound:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if( Child == self.m_Compound ) then
			--只发送self..m_material 的前4个 即可
			if(#(self.m_material) < 4 ) then
				MessageBox( g_GameMain, MsgBoxMsg(3008), MB_BtnOK )
				self.m_Compound:EnableWnd(false)
				return
			end
			Gac2Gas:SendSynthesisItemBegin(g_Conn)
			for i=1,4 do
				Gac2Gas:SendSynthesisItem(g_Conn,self.m_material[i][2])
			end
			Gac2Gas:SendSynthesisItemEnd(g_Conn)
		elseif( Child == self.m_Batchcompound ) then --批量合成
			if(#(self.m_material) < 4 ) then
				MessageBox( g_GameMain, MsgBoxMsg(3008), MB_BtnOK )
				self.m_Batchcompound:EnableWnd(false)
				return
			end
			Gac2Gas:SendSynthesisItemBegin(g_Conn)
			for i=1,#( self.m_material ) - #( self.m_material )%4 do
				Gac2Gas:SendSynthesisItem(g_Conn,self.m_material[i][2])
			end
			Gac2Gas:SendSynthesisItemEnd(g_Conn)
			self.m_Batchcompound:EnableWnd(false)
		elseif( Child == self.m_Close ) then
			--关闭窗口自动提取所有合成的宝石
			self:VirtualExcludeWndClosed()
			self:ShowWnd(false)
		elseif( Child == self.m_Lowstone ) then
			self:ClickLowStoneButton()
		end
	elseif uMsgID == BUTTON_RIGHTCLICKDOWN then
		if( Child == self.m_Lowstone ) then
			self:RClickLowStoneButton()
		end
	end
end

function CStoneCompound:FlushWnd()
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

function CStoneCompound:VirtualExcludeWndClosed()
	--self.m_Lowstone:SetWndText(GetStaticTextClient(1073))
	self.m_Lowstone:SetWndText("")
	g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_Lowstone:SetMouseOverDescAfter("")
	--self.m_Highstone:SetWndText(GetStaticTextClient(1074))
	self.m_Highstone:SetWndText("")
	g_DelWndImage(self.m_Highstone, 1, IP_ENABLE, IP_ENABLE)
	self.m_Highstone:SetMouseOverDescAfter("")
	self:FlushWnd()
	self:ClearShowExpendMoney()
end

function CStoneCompound:UseMaterial(nRoomIndex, nPos )
	if self:IsShow() == false or nRoomIndex==nil or nPos==nil then
		return
	end
	self:ClickLowStoneButton(nRoomIndex, nPos)
end

--判断放进去的材料是否符合要求 , 并暂时保存相关数据
function CStoneCompound:ClickLowStoneButton(nRoomIndex, nPos)
	g_DelWndImage(self.m_Highstone, 1, IP_ENABLE, IP_ENABLE)
	self.m_Highstone:SetMouseOverDescAfter("")
	--self.m_Highstone:SetWndText(GetStaticTextClient(1074))
	self.m_Highstone:SetWndText("")
	local state, context = g_WndMouse:GetCursorState()
	g_WndMouse:ClearCursorAll()
	local fromRoom, fromRow, fromCol, SrcCt, srcPos
	if state == ECursorState.eCS_PickupItem then --手上有物品
		fromRoom, fromRow, fromCol = context[1], context[2], context[3]
		SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
		srcPos = SrcCt:GetPosByIndex( fromRow, fromCol, fromRoom )
	end
	
	local Grid = nil
	if nRoomIndex and nPos then
		Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(nRoomIndex, nPos)
	elseif fromRoom and srcPos then
		Grid = g_GameMain.m_BagSpaceMgr:GetGridByRoomPos(fromRoom, srcPos)
	end
	if(Grid == nil) then
		return
	end
	self:MaterialTbl(Grid)
	if( self.MaterialBigID == g_ItemInfoMgr:GetHoleMaterialBigID() and #(self.MaterialID) ~= 0 ) then--打孔材料
		self.m_material = {}
		g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
		self:FlushWnd()
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(self.MaterialBigID,self.m_Nowmaterial,"SmallIcon"), self.m_Lowstone, -1, IP_ENABLE, IP_CLICKDOWN)
		g_SetItemRichToolTips(self.m_Lowstone,self.MaterialBigID,self.m_Nowmaterial, 0)
		for i=1,#(self.MaterialID) do
			table.insert(self.m_material, {self.m_Nowmaterial, self.MaterialID[i]})
		end
		self.m_Lowstone:SetWndText(#(self.m_material))
		
		if( #(self.m_material) >= 8 ) then
			self.m_Batchcompound:EnableWnd(true)
			self.m_Compound:EnableWnd(true)
		elseif( #(self.m_material) >= 4 ) then
			self.m_Batchcompound:EnableWnd(false)
			self.m_Compound:EnableWnd(true)
		else
			self.m_Batchcompound:EnableWnd(false)
			self.m_Compound:EnableWnd(false)
		end
	elseif( self.MaterialBigID == g_ItemInfoMgr:GetStoneBigID() and #(self.MaterialID) ~= 0 ) then--宝石
		self.m_material = {}
		g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
		self:FlushWnd()
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(self.MaterialBigID,self.m_Nowmaterial,"SmallIcon"), self.m_Lowstone, -1, IP_ENABLE, IP_CLICKDOWN)
		g_SetItemRichToolTips(self.m_Lowstone,self.MaterialBigID,self.m_Nowmaterial, 0)
		for i=1,#(self.MaterialID) do
			table.insert(self.m_material, {self.m_Nowmaterial, self.MaterialID[i]})
		end
		self.m_Lowstone:SetWndText(#(self.m_material))
			
		if( #(self.m_material) >= 8 ) then
			self.m_Batchcompound:EnableWnd(true)
			self.m_Compound:EnableWnd(true)
		elseif( #(self.m_material) >= 4 ) then
			self.m_Batchcompound:EnableWnd(false)
			self.m_Compound:EnableWnd(true)
		else
			self.m_Batchcompound:EnableWnd(false)
			self.m_Compound:EnableWnd(false)
		end
	elseif self.MaterialBigID ~= nil then
		MessageBox( g_GameMain, MsgBoxMsg(3011), MB_BtnOK )
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
	elseif nRoomIndex ~= nil and nPos ~= nil then
		local SrcCt = g_GameMain.m_BagSpaceMgr:GetSpaceRelateLc( g_StoreRoomIndex.PlayerBag )
		local srcPos = nPos
		local cols = g_PlayerMainBag.columns
		if(self.SelectGridTable == nil)then
			self.SelectGridTable = {}
		end
		local y, x = SrcCt:GetIndexByPos(srcPos, nRoomIndex)
		table.insert(self.SelectGridTable, {x, y, nRoomIndex})
		SrcCt:SetClickedWndState(x, y, nRoomIndex, false)
	end
	self:ShowExpendMoney()
	self.m_Nowmaterial =nil
	self.MaterialBigID = nil
end

function CStoneCompound:ClearShowExpendMoney()
	self.m_Money_Gold:SetWndText("")
	self.m_Money_Argent:SetWndText("")
	self.m_Money_Copper:SetWndText("")
end

function CStoneCompound:ShowExpendMoney()
	self:ClearShowExpendMoney()
	local item_num = #(self.m_material)
	local new_item_num=math.floor(item_num/4)
	if not self.MaterialBigID or not self.m_Nowmaterial then
		return
	end
	local sNameLevelName = g_ItemInfoMgr:GetItemInfo(self.MaterialBigID,self.m_Nowmaterial,"NextLevelName")
	if sNameLevelName == nil or sNameLevelName == "" then
		return
	end
	local ExpendMoney = g_ItemInfoMgr:GetItemInfo(self.MaterialBigID,self.m_Nowmaterial,"ExpendMoney")
	ExpendMoney = ExpendMoney * new_item_num
	
	local uGold 	= math.floor (ExpendMoney / 10000)
	local uArgent = math.floor ( (ExpendMoney - uGold * 10000) / 100)
	local uCopper = math.floor( (ExpendMoney - uGold * 10000 - uArgent * 100))
	
	self.m_Money_Gold:SetWndText(uGold)
	self.m_Money_Argent:SetWndText(uArgent)
	self.m_Money_Copper:SetWndText(uCopper)
end

function CStoneCompound:RClickLowStoneButton()
	self:ClearShowExpendMoney()
	self:FlushWnd()
	self.MaterialID = {}
	g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
	self.m_Lowstone:SetMouseOverDescAfter("")
	self.m_material = {}
	--self.m_Lowstone:SetWndText(GetStaticTextClient(1073))
	self.m_Lowstone:SetWndText("")
	self.m_Batchcompound:EnableWnd(false)
	self.m_Compound:EnableWnd(false)
end

function CStoneCompound:MaterialTbl(Grid)
	local num = Grid:Size()
	self.MaterialID = Grid:GetItemID(num)
	local BigId, Index = Grid:GetType()
	--local GlobalID = Grid:GetItem():GetItemID()
	if not Grid:Empty() then
		self.FoldLimit = Grid:GetFoldLimit()
		self.m_Nowmaterial =Index
		self.MaterialBigID = BigId
	else
		self.m_Nowmaterial = nil
	end
end
-- RCP 实现功能 --
function CStoneCompound:ReturnSynthesisItemEnd(flag)
	--在高级宝石处显示图片信息
	self:ClearShowExpendMoney()
	if(flag == false) then
		self.MaterialID = {}
		self.m_material = {}
		self.m_Nowmaterial =nil
		self.MaterialBigID = nil
		--self.m_Lowstone:SetWndText(GetStaticTextClient(1073))
		self.m_Lowstone:SetWndText("")
		g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
		self.m_Lowstone:SetMouseOverDescAfter("")
		return
	end
	if(#(self.m_highStoneID) > 0) then
		self.MaterialID = {}
		self.m_material = {}
		self.m_Nowmaterial =nil
		self.MaterialBigID = nil
		--self.m_Lowstone:SetWndText(GetStaticTextClient(1073))
		self.m_Lowstone:SetWndText("")
		g_DelWndImage(self.m_Lowstone, 1, IP_ENABLE, IP_CLICKDOWN)
		self.m_Lowstone:SetMouseOverDescAfter("")
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo(self.m_highStoneID[1][2],self.m_highStoneID[1][3],"SmallIcon"), self.m_Highstone, -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.m_Highstone,self.m_highStoneID[1][2],self.m_highStoneID[1][3], 0)
		self.m_Highstone:SetWndText(#(self.m_highStoneID))
	end
	self.m_highStoneID = {}
	SetEvent( Event.Test.ReturnSynthesisItemEnd,true,flag)
end

function CStoneCompound:OnReturnSynthesisItem(Conn,ItemId,Type,Name)
	table.insert(self.m_highStoneID,{ItemId,Type,Name})
	self.m_Batchcompound:EnableWnd(false)
	self.m_Compound:EnableWnd(false)
end
function CStoneCompound:OnReturnSynthesisItemEnd(Conn, flag)
	self:FlushWnd()
	self:ReturnSynthesisItemEnd(flag)
end