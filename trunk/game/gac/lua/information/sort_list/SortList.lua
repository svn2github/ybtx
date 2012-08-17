gac_gas_require "framework/common/CMoney"
cfg_load "sort/SortList_Common"
lan_load "sort/Lan_SortList_Common"

CSortList = class( SQRDialog ) --导出类
CSortItem = class( SQRDialog )
------------------------------------------------
--1.佣兵小队排行 (小队等级排行、小队声望排行、小队军资排行、小队资金排行)

--2.军资运输排行
--3.竞技联赛(胜利场次排行、连胜场次排行、杀人数量排行)
--4.围攻异界领主(小队击杀领主次数排行、玩家击杀领主次数排行)
--5.资源点争夺(资源点占领次数排行、杀人数量排行)

local nRowNum = 10

local tblPersonal = {}
	tblPersonal["等级"]					= GetStaticTextClient(1030)
	tblPersonal["财富"]					= GetStaticTextClient(22014)
	tblPersonal["死亡"]					= GetStaticTextClient(22024)
	tblPersonal["累计杀怪"]				= GetStaticTextClient(22025)
	tblPersonal["在线时间"]				= GetStaticTextClient(22026)
	tblPersonal["角斗场总参赛次数"]		= GetStaticTextClient(22027)
	tblPersonal["杀人"]					= GetStaticTextClient(22029)
	tblPersonal["角斗场总胜利次数"]		= GetStaticTextClient(22028)
	tblPersonal["角斗场总积分"]			= GetStaticTextClient(22030)
	tblPersonal["战斗力总评分"]			= GetStaticTextClient(22033)
	
	
local function MoneyTrans(nMoney)
	return g_MoneyMgr:ChangeMoneyToString(nMoney, true)
end

local function TimeTrans(nTime)
	return g_CTimeTransMgr:FormatTimeDH(nTime)
end

local function ClassTrans(nClass)
	return g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(nClass)
end

local function TongTypeTrans(nType)
	return g_GameMain.m_DisplayCommonObj:GetTongTypeName(nType)
end
	
local fl_tblTransFunc = {}
	fl_tblTransFunc["钱"]		= MoneyTrans
	fl_tblTransFunc["时间"]		= TimeTrans
	fl_tblTransFunc["职业"]		= ClassTrans
	fl_tblTransFunc["小队类型"]	= TongTypeTrans
	
function CSortList:Ctor()
	self:CreateFromRes( "SortListWnd", g_GameMain )
	self:Init()
	g_ExcludeWndMgr:InitExcludeWnd(self, "非互斥")
end

function CSortList:OnChildCreated()
	self.m_close			= self:GetDlgChild("close")
	self.m_Personal			= self:GetDlgChild("Personal")
	self.m_YongbingGroup	= self:GetDlgChild("YongbingGroup")
	self.m_PlayFunc			= self:GetDlgChild("PlayFunc")
	self.m_UpPage			= self:GetDlgChild("UpPage")
	self.m_DownPage			= self:GetDlgChild("DownPage")
	self.m_SortTree			= self:GetDlgChild("SortTree")		--排行榜单
	self.m_SortList			= self:GetDlgChild("SortList")		--排行榜列表
	self.m_Page				= self:GetDlgChild("Page")			--页数
	self.m_ShowSort			= self:GetDlgChild("ShowSort")		--具体的显示信息概括
	
	self.m_tblRadio 		= {self:GetDlgChild("AmyEmpery"), self:GetDlgChild("FaNuoSi"), self:GetDlgChild("EMoDao"), self:GetDlgChild("All")}
	self.m_tblRadio[4]:SetCheck(true) --初始选中“全部”单选按钮(要和self.m_nCampType对应起来)
	self.m_Personal:SetCheck(true)
	
	self.m_tblLabel	= {}
	for i = 3, 5 do
		self.m_tblLabel[i] = self:GetDlgChild("Lable" .. i)
	end
end

function CSortList:Init()
	self.m_nCampType		= 4	--4:全部;1:艾米;2法诺斯;3恶魔岛
	self.m_SortListTbl		= {}
	self.m_tblSortListItem	= {}
	self:SetPage(1)
	self.m_ShowSort:SetWndText(GetStaticTextClient(1070) .. ": --")
	self.m_ShowSort:ShowWnd(false)
	self:CreateTree("个人")
end

function CSortList:BeOpenPanel()
	local bFlag = not self:IsShow()
	if( bFlag ) then
		self:GetSortList()
	end
	self:ShowWnd(bFlag)
end

function CSortList:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_close)then
			self:ShowWnd(false)
		elseif(Child == self.m_Personal) then
			self:CreateTree("个人")
			self:GetSortList()
		elseif(Child == self.m_YongbingGroup) then
			self:CreateTree("佣兵小队")
			self:GetSortList()
		elseif(Child == self.m_PlayFunc) then
			self:CreateTree("玩法")
			self:GetSortList()
		elseif(Child == self.m_UpPage) then--上一页
			if(self.m_PageNumber > 1)then
				self:SetPage(self.m_PageNumber - 1)
				self:DrawSortList()--刷新上一页
			end
		elseif(Child == self.m_DownPage) then--下一页
			if(self.m_PageNumber < #(self.m_SortListTbl)/nRowNum)then
				self:SetPage(self.m_PageNumber + 1)
				self:DrawSortList()--刷新下一页
			end
		else
			for i = 1, #(self.m_tblRadio) do --选择显示哪一个阵营
				if(Child == self.m_tblRadio[i]) then
					self.m_nCampType = i
					self:GetSortList()
					break
				end
			end
		end
	elseif(uMsgID == ITEM_LBUTTONCLICK) then
		if(Child == self.m_SortTree) then
			self:GetSortList()
		end
	end
end

function CSortList:DrawSortList()
	self.m_SortList:DeleteAllItem()
	self.m_tblSortListItem = {}
	self.m_SortList:InsertColumn( 0, self.m_SortList:GetWorkAreaOrgWidth() )
	local beforeIndex = (self.m_PageNumber-1)*nRowNum
	local lastPageNum = #self.m_SortListTbl-beforeIndex
	local iEnd  = nRowNum > lastPageNum and lastPageNum or nRowNum
	for i = 1, iEnd do
		self.m_SortList:InsertItem(i-1, 28)
		local item		= self.m_SortList:GetSubItem(i-1, 0)
		local itemWnd	= self:CreateSortItem(item)
		itemWnd:CopyMsg2Wnd(self.m_SortListTbl[i+beforeIndex])
		self.m_tblSortListItem[i] = itemWnd
	end
end

function CSortList:CreateTree(sType)
	self.m_sType = sType
	self:SetPage(1)
	self.m_tblTreeItem = {}
	self.m_SortTree:DeleteAllItem()
	self.m_SortTree:InsertColumn( 0, self.m_SortTree:GetWorkAreaOrgWidth() )
	local subNode = SortList_Common(sType)
	for i, v in ipairs( subNode:GetKeys() ) do
		self.m_SortTree:InsertItem(i-1, 20)
		local item	= self.m_SortTree:GetSubItem(i-1, 0)
		item.m_Name	= v
		item:SetWndText(Lan_SortList_Common(MemH64(sType .. v), "DisplayName"))
		self.m_tblTreeItem[i] = item
	end
end

function CSortList:SetPage(page)
	self.m_PageNumber = page
	self.m_Page:SetWndText(page)
end

function CSortList:GetSortList()
	if not g_MainPlayer then return end
	local itemIndex = self.m_SortTree:GetSelectItem(-1)+1
	if (not itemIndex or 0 == itemIndex) then return end
	self:SetPage(1)
	
	local sName = self.m_tblTreeItem[itemIndex].m_Name
	local lanIndex	= MemH64(self.m_sType .. sName)
	
	for k, v in pairs(self.m_tblLabel) do
		local sLabel = Lan_SortList_Common(lanIndex, "DisLabel" .. k)
		v:SetWndText(sLabel)
	end
	
	self.m_ShowSort:ShowWnd(sName ~= "个人情况")
	Gac2Gas:GetSortList(g_Conn, self.m_sType, sName, self.m_nCampType)
end

function CSortList:FuncFormat(sType, sName)
	local tbl = self.m_SortListTbl
	if(next(tbl)) then
		if("个人" == sType and "个人情况" == sName) then
			for i, v in ipairs(tbl) do
				if(v[3] == "在线时间") then
					v[5] = TimeTrans(v[5])
				elseif(v[3] == "财富") then
					v[5] = MoneyTrans(v[5])
				end
				v[3] = tblPersonal[v[3]]
				v[4] = ClassTrans(v[4])
			end
		else
			local sFunc4 = SortList_Common(sType, sName, "Func4")
			local sFunc5 = SortList_Common(sType, sName, "Func5")
			for i = 1, #tbl do
				local func = fl_tblTransFunc[sFunc4]
				tbl[i][4] = func(tbl[i][4])
				if(sFunc5 and "" ~= sFunc5) then
					func = fl_tblTransFunc[sFunc5]
					tbl[i][5] = func(tbl[i][5])
				end
			end
		end
	end
end

function CSortList:RetSortListBegin()
	self.m_SortListTbl = {}
	self.m_ShowSort:SetWndText(GetStaticTextClient(1070) .. ": --")
end

function CSortList:RetSortList(order, up, name, vocation, value)
	local sCharName = g_MainPlayer.m_Properties:GetCharName()
	if name == sCharName then
		self.m_ShowSort:SetWndText(GetStaticTextClient(1070) .. ": " .. order)
	end
	table.insert(self.m_SortListTbl, {order, up, name, vocation, value})
end

function CSortList:RetSortListEnd(sType, sName)
	self:FuncFormat(sType, sName)
	self:DrawSortList()
end
-----------------------------------------------------
function CSortList:CreateSortItem(parent)
	local wnd = CSortItem:new()
	wnd:CreateFromRes("SortItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CSortItem:OnChildCreated()
	self.m_tblItem = {}
	for i = 1, 5 do
		self.m_tblItem[i] = self:GetDlgChild("Item" .. i)
	end
end

--设置Item的信息
function CSortItem:CopyMsg2Wnd(tbl)
	for i = 1, #tbl do
		self.m_tblItem[i]:SetWndText(tbl[i])
	end
end