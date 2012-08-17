gac_require "message/message_box/MsgBoxMsg"
gac_gas_require "event/Event"
cfg_load "item/HoleMaterial_Common"

CStone = class ( SQRDialog )
CMSG  = class ( SQRDialog )
CStoneAttrWnd = class( SQRDialog )

function CStone:Ctor( Parent )
	self:CreateFromRes( "StoneMain" , Parent )
	self.wholeAttr = {}		--保存总体属性和 1<攻击>2<暴击>3<法伤>
	self.stonetbl = {}		--保存宝石,及对应的frameID, partID , holeID
	self.holetbl   = {}		--保存打孔的孔位和frameID, partID , holeID
	self.FrameID = 1		--目前选中的面板  默认为 1
	self.PartID = nil		--目前选中的部位
	self:InitMSG()
	self:AllInitStonePart(self)
	self:InitAttrWnd(self)
	self.partwholeAttr ={}
	self.PartStoneType = {{18013,18014},{18013,18014},{18013,18014},{18013,18014},
						{18015,18016,18017},{18015,18016,18017},{18015,18016,18017},{18015,18016,18017},
						{18014,18016,18018},{18014,18016,18018},{18014,18016,18018},{18014,18016,18018}}

	g_ExcludeWndMgr:InitExcludeWnd(self, 2)
end

function CStone.GetWnd()
	local Wnd = g_GameMain.m_stone
	if not Wnd then
		Wnd = CStone:new(g_GameMain)
		g_GameMain.m_stone = Wnd
	end
	return Wnd
end

function CStone:OnChildCreated()
	self.m_XBtn		= self:GetDlgChild("XBtn")
	self.m_Panel1	= self:GetDlgChild("one")
	self.m_Panel2	= self:GetDlgChild("two")
	self.m_Panel3	= self:GetDlgChild("three")
	
	self.m_tblPart = {}
	for i = 1, 12 do
		self.m_tblPart[i] = self:GetDlgChild("Part" .. i)
		if i~=1 and i~=5 and i~= 9 then
			self.m_tblPart[i]:EnableWnd(false)
			self.m_tblPart[i]:ShowWnd(false)
		end
	end

	self.m_Check	= self:GetDlgChild("checkShow")
	self:GetDlgChild("shuoming"):SetTransparent( 0.5 )--控件透明
	self:GetDlgChild("two"):EnableWnd(false)
	self:GetDlgChild("three"):EnableWnd(false)
end

function CStone:InitMSG()
	self.m_MSG = CMSG:new()
	self.m_MSG:CreateFromRes("WndMsg", g_GameMain)

end

function CStone:InitAttrWnd(parent)
	self.attrWnd = CStoneAttrWnd:new()
	self.attrWnd:CreateFromRes("StoneAttribute", parent)
	self.attrWnd.AttrText = self.attrWnd:GetDlgChild("AttrText")

end

function CStone:AllInitStonePart(Parent)
	self.stonepart = {}
	for i =1, 12 do
		self.stonepart[i] = CStonePart:new()
		self.stonepart[i]:CreateFromRes( "StonePart"..i, Parent )
		self.stonepart[i]:ShowWnd( true )
		self.stonepart[i]:InitPart()
		self.stonepart[i].m_Position = i
	end
	self.StonePartUsing = self.stonepart[1]		--当前打开的StonePart
end

function CStone:VirtualExcludeWndClosed()
	self.StonePartUsing:Close()
end

function CStone:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_XBtn) then
			self:ShowWnd(false)
		elseif(Child == self.m_Panel1) then
			--通知Server更换面板, 属性更换
			if self.FrameID ~= 1 then
				Gac2Gas:ChangeFrame(g_Conn,self.FrameID,1)
				self:DrawStoneFrame(1)
				self.StonePartUsing:Close()
			end
		elseif(Child == self.m_Panel2) then
			if self.FrameID ~= 2 then
				Gac2Gas:ChangeFrame(g_Conn, self.FrameID, 2)
				self:DrawStoneFrame(2)
				self.StonePartUsing:Close()
			end
		elseif(Child == self.m_Panel3) then
			if self.FrameID ~= 3 then
				Gac2Gas:ChangeFrame(g_Conn, self.FrameID, 3)
				self:DrawStoneFrame(3)
				self.StonePartUsing:Close()
			end
		elseif(Child == self.m_Check) then
			self.attrWnd:ShowWnd( not self.attrWnd:IsShow() )
		else
			for i = 1, 12 do
				if(Child == self.m_tblPart[i]) then
					DrawpartstoneFrame(i)
				end
			end
		end
	end
end

function CMSG:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self:GetDlgChild("BtnOK")) then
			self:ShowWnd(false)
			local StoneWnd = CStone.GetWnd()
			StoneWnd.StonePartUsing.m_ZhaiChu:EnableWnd(false)
			Gac2Gas:RequestRemovalStone(g_Conn,StoneWnd.FrameID,StoneWnd.PartID,StoneWnd.StonePartUsing.NowSelectHoleID)
		elseif(Child == self:GetDlgChild("BtnCancel")) then
			self:ShowWnd(false)
			self:GetDlgChild("WndInfo"):SetWndText("")
		end
	end
end

function CStone:DrawStoneFrame(name) --初始化面板
	--根据宝石属性 显示攻击,暴击,法伤加成
	--根据角色等级 开启第二,第三个宝石面板 <未定>
	--根据等级判断是否开启第二，第三个面板 ，需要考虑
	self.wholeAttr = {}
	--local color = {"#c000000#eff0000ff#P","#c00ff00","#cff0000"}
	local color = {"#c000000","#c00ff00","#cff0000"}
	self.FrameID = name
	for i =1, 12 do  --十二个部位, 可以镶嵌不同类型的宝石
		local AllHoleState = {1,1,1,1,1,1,1,1}  --1 未开孔, 2 已开孔, 3 已镶嵌
		for j =1, #(self.holetbl) do
			if(self.holetbl[j][1] == self.FrameID and self.holetbl[j][2] == i ) then
				--这个孔已经打开,改变AllHoleState中对应孔的状态
				AllHoleState[self.holetbl[j][3]] = 2
			end
		end
		for j =1, #(self.stonetbl) do
			if(self.stonetbl[j][1] == self.FrameID and self.stonetbl[j][2] == i ) then
				--这个孔已经镶嵌,改变AllHoleState中对应孔的状态
				AllHoleState[self.stonetbl[j][3]] = 3
			end
		end
		local szStoneType = ""
		for j =1, #(self.PartStoneType[i]) do
			if szStoneType == "" then
				szStoneType = szStoneType..GetStaticTextClient(self.PartStoneType[i][j])
			else
				szStoneType = szStoneType..","..GetStaticTextClient(self.PartStoneType[i][j])
			end
		end
		--不同部位 可镶嵌宝石类型的提示信息
		self.m_tblPart[i]:SetMouseOverDescAfter("#cffffff"..GetPartDisplayName(i).."#r"..
		color[AllHoleState[1]]..GetStaticTextClient(18021)..color[AllHoleState[2]]..GetStaticTextClient(18021)
		..color[AllHoleState[3]]..GetStaticTextClient(18021)..color[AllHoleState[4]]..GetStaticTextClient(18021).."#r"
		..color[AllHoleState[5]]..GetStaticTextClient(18021)..color[AllHoleState[6]]..GetStaticTextClient(18021)
		..color[AllHoleState[7]]..GetStaticTextClient(18021)..color[AllHoleState[8]]..GetStaticTextClient(18021).."#r"..
		"#cffffff"..GetStaticTextClient(18020)..szStoneType)
	end
	for i =1, #(self.stonetbl) do  --在宝石面板上显示总体属性值
		local ItemInfo = g_ItemInfoMgr:GetItemFunInfo(self.stonetbl[i][4],self.stonetbl[i][5])
		local ItemLanInfo = g_ItemInfoMgr:GetItemLanFunInfo(self.stonetbl[i][4],self.stonetbl[i][5])
		if(self.stonetbl[i][1] == self.FrameID) then
			self:AccountAttr(ItemInfo,ItemLanInfo,self.stonetbl[i][2],self.stonetbl[i][3])
		end
	end
	self.attrWnd.AttrText:SetWndText("#r" .. GetStaticTextClient(1072))
	for i =1, #(self.wholeAttr) do
		self.attrWnd.AttrText:AddWndText("#r"..self.wholeAttr[i][3].."+"..self.wholeAttr[i][2])
	end
end
function CStone:AccountAttr(ItemInfo, ItemLanInfo, position, holepos)
	local flag = false
	for j =1, #(self.wholeAttr) do
		if(self.wholeAttr[j][1] == ItemInfo("AttrType")) then
			flag = true
			self.wholeAttr[j][2] = self.wholeAttr[j][2] + ItemInfo("AttrValue")
			for k = 1, 8 do
				local res = JudgeButtonActive( position, k )
				if  res ~= 3 then
					break
				elseif k == 4 and k == holepos then
					self.wholeAttr[j][2] = self.wholeAttr[j][2] + ItemInfo("Award4")
				elseif k == 8 and k == holepos then
					self.wholeAttr[j][2] = self.wholeAttr[j][2] + ItemInfo("Award8")
				end
			end
			break
		end
	end
	if(not flag) then
		table.insert(self.wholeAttr,{ItemInfo("AttrType"),ItemInfo("AttrValue"),ItemLanInfo("ShowAttrType")})
		for k = 1, 8 do
			local res = JudgeButtonActive( position, k )
			if  res ~= 3 then
				break
			elseif k == 4 or k ==8 then
				if k == holepos then
					self.wholeAttr[#(self.wholeAttr)][2] = self.wholeAttr[#(self.wholeAttr)][2] + ItemInfo("Award4")
				end
			end
		end
	end
end

function JudgeButtonActive( partID, holename )  --判断孔的状态
  	--  1 <可以打孔>   2 <已经打孔>,<可以镶嵌>
  	--  3 <已经镶嵌> 4 <前面的还没有打孔> 5 <前面的还没有镶嵌>
	local res = 1
	local StoneWnd = CStone.GetWnd()
	for i=1,#(StoneWnd.holetbl) do
		if( StoneWnd.holetbl[i][1] ==StoneWnd.FrameID and StoneWnd.holetbl[i][2] ==partID ) then --面板 和 部位
			if( StoneWnd.holetbl[i][3] ==holename ) then
				--孔已开启,不能再放置打孔材料
				res = 2
			end
		end
	end
	for i=1,#(StoneWnd.stonetbl) do
		if( StoneWnd.stonetbl[i][1] ==StoneWnd.FrameID and StoneWnd.stonetbl[i][2] == partID and StoneWnd.stonetbl[i][3] == holename ) then
			--已镶嵌,不能再放置宝石   
			res = 3
		end
	end
	if(res ==1) then --判断前面的是否全部打孔
		for i =1,tonumber(holename)-1 do
			local Flag = false
			if( tonumber(holename) > 1) then
				res = 4
				for j=1, #(StoneWnd.stonetbl) do
					if( StoneWnd.stonetbl[j][1] ==StoneWnd.FrameID and StoneWnd.stonetbl[j][2] ==partID and StoneWnd.stonetbl[j][3] ==i) then --面板 和 部位
						res = 1
						Flag = true
						break
					elseif(j == #(StoneWnd.stonetbl)) then
						res = 4
					end
				end
			elseif( tonumber(holename) == 1) then
				break
			end
			if(not Flag) then
				res = 4
				for j = 1, #(StoneWnd.holetbl) do
					if( StoneWnd.holetbl[j][1] ==StoneWnd.FrameID and StoneWnd.holetbl[j][2] ==partID and StoneWnd.holetbl[j][3] ==i) then --面板 和 部位
						res = 1
						break
					elseif(j == #(StoneWnd.holetbl)) then
						res = 4
						return res
					end
				end
			end
		end
	end
	if( res ==2 ) then
		--判断按顺序前面的孔是否都已经镶嵌
		for i=1, tonumber(holename)-1 do
			if( 0== #(StoneWnd.stonetbl) and tonumber(holename) > 1) then
				res = 5
			end
			if( tonumber(holename) == 1 )then
				break
			end
			for j=1, #(StoneWnd.stonetbl) do
				if( StoneWnd.stonetbl[j][1] ==StoneWnd.FrameID and StoneWnd.stonetbl[j][2] ==partID and StoneWnd.stonetbl[j][3] == i ) then
					res = 2
					break
				elseif(j == #(StoneWnd.stonetbl)) then
					res = 5
					return res
				end
			end
		end
	end
	return res
end

--RCP 实现功能--
function CStone:SendAllHoleInfoEnd()
	if(self.PartID ~= nil) then
		DrawpartstoneFrame(self.PartID)
	end
	if(self.FrameID == 1) then
		self:DrawStoneFrame(1)
	elseif(self.FrameID == 2) then
		self:DrawStoneFrame(2)
	elseif(self.FrameID == 3) then
		self:DrawStoneFrame(3)
	else
		self:DrawStoneFrame(1)
	end
	SetEvent( Event.Test.SendAllHoleInfoEnd,true,flag)
end
function CStone:ReturnOpenHole(flag)
	g_DelWndImage(self.StonePartUsing.m_tblPart[self.StonePartUsing.NowSelectHoleID], 1, IP_ENABLE, IP_CLICKDOWN)
	self.StonePartUsing.m_tblPart[self.StonePartUsing.NowSelectHoleID]:SetMouseOverDescAfter("")
	self.StonePartUsing.MaterialID ={}
	if(flag ==true) then
		SetEvent( Event.Test.OpenHole,true,flag)
	else
		SetEvent( Event.Test.OpenHole,false,flag)
	end
end
function CStone:ReturnInlayStone(flag)
	if not flag then
		g_DelWndImage(self.StonePartUsing.m_tblPart[self.StonePartUsing.NowSelectHoleID], 1, IP_ENABLE, IP_CLICKDOWN)
		self.StonePartUsing.m_tblPart[self.StonePartUsing.NowSelectHoleID]:SetMouseOverDescAfter("")
	end
	self.StonePartUsing.MaterialID ={}
	SetEvent( Event.Test.InlayStone,true,flag)
end
function CStone:ReturnRemovalStone(flag)
	if(flag ==true) then
		
	end
	SetEvent( Event.Test.RemovalStone,true,flag)
end
function CStone:NotifyAddAttr(frameid, position, holePos)
	if(self.PartID ~= position) then
		return
	end
	local BigId = 0
	local Index = 0
	for i =1, #(self.stonetbl) do
		if(self.stonetbl[i][1]==frameid and self.stonetbl[i][2] ==position and self.stonetbl[i][3] ==holePos) then
			BigId = self.stonetbl[i][4]
			Index = self.stonetbl[i][5]
		end
	end
	if(BigId == 0 or Index == 0) then
		return
	end
	self.StonePartUsing:NotifyShowAddAttr(BigId,Index)
end

function CStone:OnPlayerLevelUp()
	if g_MainPlayer:CppGetLevel() >= 60 then
		self.m_tblPart[2]:EnableWnd(true)
		self.m_tblPart[6]:EnableWnd(true)
		self.m_tblPart[10]:EnableWnd(true)
		self.m_tblPart[2]:ShowWnd(true)
		self.m_tblPart[6]:ShowWnd(true)
		self.m_tblPart[10]:ShowWnd(true)
	else
		self.m_tblPart[2]:EnableWnd(false)
		self.m_tblPart[6]:EnableWnd(false)
		self.m_tblPart[10]:EnableWnd(false)
		self.m_tblPart[2]:ShowWnd(false)
		self.m_tblPart[6]:ShowWnd(false)
		self.m_tblPart[10]:ShowWnd(false)
	end
end

function CStone:OpenPanel(bShow)
	self:OnPlayerLevelUp()
	self:ShowWnd(bShow)
end

function CStone:NotifyOpenPanel(num)
	if(num == 2) then
		self:GetDlgChild("two"):EnableWnd(true)
	elseif(num == 3) then
		self:GetDlgChild("three"):EnableWnd(true)
	end
end

function CStone:OnReturnGetOpenedHole(Conn,frameID, position, holeID)--已打孔的
	table.insert(self.holetbl, {frameID, position, holeID})
end
function CStone:OnReturnGetInlayedHole(Conn,frameID, position, holePos ,stoneBigID, stonename)--已镶嵌的孔
	table.insert(self.stonetbl, {frameID, position, holePos, stoneBigID, stonename})
end
function CStone:OnReturnOpenHole(Conn,flag)--打孔的返回
	self.StonePartUsing:VirtualExcludeWndClosed()
	self:ReturnOpenHole(flag)
end
function CStone:OnReturnInlayStone(Conn,flag)--镶嵌的返回
	self.StonePartUsing:VirtualExcludeWndClosed()
	self:ReturnInlayStone(flag)
end
function CStone:OnReturnRemovalStone(Conn,flag)--宝石摘除返回
	self:ReturnRemovalStone(flag)
end
--打孔,镶嵌,摘除End ,需要更新 表HoleTbl,StoneTbl
function CStone:OnUpdateAllHoleInfoBegin(Conn)  --考虑异步问题
	self.holetbl = {}
	self.stonetbl = {}
	for i =1, 8 do
		g_DelWndImage(self.StonePartUsing.m_tblPart[i], 1, IP_ENABLE, IP_CLICKDOWN)
		self.StonePartUsing.m_tblPart[i]:SetMouseOverDescAfter("")
	end
end
function CStone:OnSendAllHoleInfoEnd(Conn)--初始化结束
	self:SendAllHoleInfoEnd()
end
function CStone:OnNotifyAddAttr(Conn, frameid, position, holePos)--镶嵌满4, 8 个时 第4, 第8个孔的宝石获得附加属性 增加m%,n%
	self:NotifyAddAttr(frameid, position, holePos)
end
function CStone:OnNotifyOpenPanel(Conn,num)-- 等级满足条件 开启后续面板
	self:NotifyOpenPanel(num)
end
function CStone:OnUsePanel(Conn,PanelID)-- 通知正在使用哪个面板
	self.FrameID = PanelID
end