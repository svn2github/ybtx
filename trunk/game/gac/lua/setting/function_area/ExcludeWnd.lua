CShowOpendWnd = class()
------窗体互斥------

--玩家打开窗口面板的初始化分类函数。需要加入互斥系统的窗体，要在初始化时调用此函数；
--参数：窗口对象，所属类别，是否停靠，父窗体表
--wndObject：要注册的窗口对象
--typeNo：数字类型，相同类型互斥，意思就是不会同时出现。不同类型的面板按照其类型数值的绝对值大小从左到右依次依靠排列
--bDepend：布尔类型，是否停靠。
--TblFatherWnd：父窗口对象表(一定要传表结构！)，父窗口如果关闭，子窗口将随之关闭。如果没有父窗体，就不加此参数。
--functionalGroup：功能系统（如：帮会系统面板为1，，这个目前是用来同时关闭某个系统打开的所有面板的，不需要这个参数的就不要传了）
--ps：typeNo为0的窗体与除其子窗体之外的其它任何类型窗体互斥，其子窗体的typeNo约定为负值

local function SetXPos(wnd, x, y)
	local GameMainRect = CFRect:new()
	g_GameMain:GetLogicRect(GameMainRect)
	
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	
	local uWidth  = Rect.right - Rect.left
	Rect.left	= x
	Rect.right	= x+uWidth
	
	wnd:SetLogicRect(Rect)
	
	if(Rect.right > GameMainRect.right) then
		SetXPos(wnd, GameMainRect.right - uWidth)
	end
end

function CShowOpendWnd:Ctor()
	self:Clear()
end

function CShowOpendWnd:Clear()
	self.m_OpenWndRecordTbl = {}
	self.m_OpenWndRecordTblUnExclude = {}
end

function CShowOpendWnd:InitExcludeWnd(wndObject, typeNo, bDepend, tblFatherWnd, functionalGroup)
	if(tblFatherWnd) then
		assert(IsTable(tblFatherWnd), "此参数应该传table")
	end
	
	local bFlag = false
	if( wndObject:IsShow() ) then
		wndObject:ShowWnd(false)
		bFlag = true
	end
	
	wndObject.TYPE		= typeNo
	wndObject.BDEPEND	= bDepend
	wndObject.TBLFATHER	= tblFatherWnd
	wndObject.FUNCGROUP	= functionalGroup
	
	if(bFlag) then
		wndObject:ShowWnd(true)
	end
end

function CShowOpendWnd:InitExcludeWndEx(wndObject, typeNo, bDepend, tblFatherWnd, functionalGroup)
	wndObject:ShowWndIgnTex( wndObject:IsShow() )
	self:InitExcludeWnd(wndObject, typeNo, bDepend, tblFatherWnd, functionalGroup)
end

function CShowOpendWnd:ShowWnd(func, wndObject, bShow, bFlag)
	if(bShow) then
		self:OpenExcludeWnd(func, wndObject)
	else
		self:CloseExcludeWnd(func, wndObject, bFlag)
	end
end

--改变记录现在打开的窗口对象的tbl，在打开窗体时替代ShowWnd(true)。参数：要打开的窗口对象
function CShowOpendWnd:OpenExcludeWnd(func, wndObject)
	assert(wndObject.TYPE, "未加入面板互斥系统就调用互斥系统的方法！")
	assert(func, "禁止直接调用OpenExcludeWnd") 
------------------------------------------------------------------------------------------------------------
	if("非互斥" == wndObject.TYPE) then --非互斥面板
		if( wndObject:IsShow() and self.m_OpenWndRecordTblUnExclude[wndObject] ) then
			return
		end
		func(wndObject, true) --显示此面板
		self.m_OpenWndRecordTblUnExclude[wndObject] = true
		
------------------------------------------------------------------------------------------------------------
	else
		if( wndObject:IsShow() and self.m_OpenWndRecordTbl[wndObject.TYPE] == wndObject) then
			return
		end
		func(wndObject, true) --显示此面板
		if(not next(self.m_OpenWndRecordTbl)) then --如果互斥面板表为空
			self.m_OpenWndRecordTbl[wndObject.TYPE] = wndObject
		else
			self:BeAddWnd(wndObject) --关闭其互斥面板，并添加此子窗体到互斥面板表
		end
		--self:ShowWndCorrectly() --让面板按照类型大小排序
	end
end

--如果窗口关闭，则清除打开窗口记录表里的对象，在关闭窗体时替代ShowWnd(false)。
function CShowOpendWnd:CloseExcludeWnd(func, wndObject, bFlag)
	if(not wndObject) then return end
	assert(func, "禁止直接调用CloseExcludeWnd") 
	assert(wndObject.TYPE, "未加入面板互斥系统就调用互斥系统的方法！")
	
	if(wndObject.VirtualSpecialWndOnClose) then
		wndObject:VirtualSpecialWndOnClose()
		wndObject.VirtualSpecialWndOnClose = nil
		return
	end
--------------------------------------------------------------------------------------------------------
	if("非互斥" == wndObject.TYPE) then --非互斥面板
		if(self.m_OpenWndRecordTblUnExclude[wndObject]) then
			self.m_OpenWndRecordTblUnExclude[wndObject] = nil
		end
		self:OnCloseWnd(wndObject, bFlag)
		func(wndObject, false)
		self:ClosedWnd(wndObject, bFlag)
--------------------------------------------------------------------------------------------------------
	else
		self:OnCloseWnd(wndObject, bFlag)
		func(wndObject, false)
		if self.m_OpenWndRecordTbl[wndObject.TYPE] == wndObject then
			self:ClosedWnd(wndObject, bFlag)
			self.m_OpenWndRecordTbl[wndObject.TYPE] = nil
			self:CloseChild(wndObject)
		end
		
		if wndObject.m_CheckIsAutoCloseWndTick then
			UnRegisterTick(wndObject.m_CheckIsAutoCloseWndTick)
			wndObject.m_CheckIsAutoCloseWndTick = nil
		end
		
		--self:ShowWndCorrectly()
	end
end

------不同类型的窗体位置控制------
function CShowOpendWnd:ShowWndCorrectly()
	local tbl = self.m_OpenWndRecordTbl
	local tblOrder = {}
	for i, v in pairs(tbl) do
		table.insert(tblOrder, i)
	end
	table.sort(tblOrder, function(a, b) return math.abs(a) < math.abs(b) end)
	
	local Rect = CFRect:new()
	for i, v in ipairs(tblOrder) do
		if(tbl[v].BDEPEND) then
			local left = 1 
			if(v>1 and tbl[v].BDEPEND) then
				tbl[v-1]:GetLogicRect(Rect)
				left = Rect.right + 1
			end
			SetXPos(tbl[v], left)
		end
	end
end

--关闭所有添加到互斥表里的面板
function CShowOpendWnd:CloseAllActiveWndExclude()
	for i, v in pairs(self.m_OpenWndRecordTbl) do
		v:ShowWnd(false)
	end
end

--关闭所有添加到非互斥表里的面板
function CShowOpendWnd:CloseAllActiveWndUnExclude()
	for k, v in pairs(self.m_OpenWndRecordTblUnExclude) do
		k:ShowWnd(false)
	end
end

function CShowOpendWnd:FindInTbl(tbl, target)
	for i = 1, #tbl do
		if(target == tbl[i]) then return true end
	end
	return false
end

------关闭子窗体------
function CShowOpendWnd:CloseChild(beCloseWnd, beOpenWnd) --参数：即将关闭的面板，即将打开的面板
	for i, v in pairs(self.m_OpenWndRecordTbl) do 
		local tbl = v.TBLFATHER
		if( tbl and next(tbl) ) then --有父面板的面板
			local beOpenWndChild  = self:FindInTbl(tbl, beOpenWnd)
			local beCloseWndChild = self:FindInTbl(tbl, beCloseWnd)
			if((not beOpenWndChild) and beCloseWndChild) then
				v:ShowWnd(false)
			end
		end
	end
end

------是否插入新窗体------
function CShowOpendWnd:BeAddWnd(wndObject)
	if(self.m_OpenWndRecordTbl[wndObject.TYPE]) then --如果有和即将打开的面板互斥的面板
		self.m_OpenWndRecordTbl[wndObject.TYPE]:ShowWnd(false)
	end
	self.m_OpenWndRecordTbl[wndObject.TYPE] = wndObject
end

------关闭某一功能系统的所有打开的面板------
function CShowOpendWnd:RemoveOneFuncGroupPan(functionalGroup)--functionalGroup: 1为帮会系统面板
	for i, v in pairs(self.m_OpenWndRecordTbl) do
		if(functionalGroup == v.FUNCGROUP) then
			v:ShowWnd(false)
		end
	end
end

------各个窗体可以实现的VirtualExcludeWndClosed虚幻数, 在关闭面板的时候调用------
function CShowOpendWnd:ClosedWnd(wnd, bFlag)
	if(wnd.VirtualExcludeWndClosed) then
		wnd:VirtualExcludeWndClosed(bFlag)
	end
end

function CShowOpendWnd:OnCloseWnd(wnd, bFlag)
	if(wnd.VirtualExcludeWndOnClose) then
		wnd:VirtualExcludeWndOnClose(bFlag)
	end
end
