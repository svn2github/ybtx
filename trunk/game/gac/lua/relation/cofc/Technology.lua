gac_require "relation/cofc/TechnologyInc"
gac_gas_require "relation/cofc/CofcTechnologyMgr"

--服务端传相关科技数据
function Gas2Gac:ReturnGetCofcTechnologyInfo(Conn, techIndex, nowLevel, nowValue)
	g_GameMain.m_CofCTechWnd.m_TechTbl[techIndex] = {nowLevel, nowValue}
end

--服务端传数据结束，在客户端显示
function Gas2Gac:ReturnGetCofcTechnologyInfoEnd(Conn, curTechIndex)
	g_GameMain.m_CofCTechWnd.CurTechId = curTechIndex
	g_GameMain.m_CofCTechWnd:ShowTechInfo()
end



-----------------------------------------------------------------------------------

--创建商会科技窗口
function CreateCofCTechWnd(Parent)
	local wnd	=	CCofCTechWnd:new()
	wnd:CreateFromRes("CofCTech",Parent)
	wnd:ShowWnd( true )
	wnd:Init()
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 3, false, {g_GameMain.m_CofCMainWnd})
	return wnd
end

--初始化商会科技窗口子控件
function CCofCTechWnd:OnChildCreated()
	self.m_CloseBtn			=	self:GetDlgChild("Close")
	self.m_ShutDownBtn		=	self:GetDlgChild("ShutDownBtn")
	self.m_SetCurTechBtn	=	self:GetDlgChild("SetCurTechBtn")
	self.m_TechList			=	self:GetDlgChild("TechList")
end

function CCofCTechWnd:Init()
	self.m_techMgr = CCofcTechnologyMgr:new()
	self:ClearTabel()
end

--商会科技窗口事件响应函数
function CCofCTechWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ShutDownBtn or Child == self.m_CloseBtn) then 		
			self:ShowWnd(false)
		elseif(Child == self.m_SetCurTechBtn) then
			local itemIndex	= self.m_TechList:GetSelectItem(-1) + 1
			if(not itemIndex or itemIndex < 1) then
				MsgClient(9010, "请选择要升级的科技")
			else
				Gac2Gas:SetCofcCurrentTechnology(g_Conn, itemIndex)
			end
		end
	elseif ( uMsgID == ITEM_LBUTTONCLICK ) then
		
	end
end

function CCofCTechWnd:ClearTabel()
	self.m_TechTbl		= {}
	self.m_TechListTbl	= {}
	local tblMgrInfoAll = self.m_techMgr:GetTechnologyInfoAll()
	for i = 1, #tblMgrInfoAll do
		self.m_TechTbl[i] = {0, 0}
	end
end

--显示商会科技相关信息
function CCofCTechWnd:ShowTechInfo()
	local mgrInfoAll = self.m_techMgr:GetTechnologyInfoAll()
	local techTbl = self.m_TechTbl
	self.m_TechList:DeleteAllItem()
	self.m_TechList:InsertColumn( 0, self.m_TechList:GetWndWidth())
	for i = 1, #mgrInfoAll do
		self.m_TechList:InsertItem( i-1, 25 )
		local item			= self.m_TechList:GetSubItem( i-1, 0)
		local techItemWnd	= self:CreateTechListItem(item)
		local nowLevel	= techTbl[i][1]
		local nowValue	= techTbl[i][2]
		local mgrInfo	= self.m_techMgr:GetTechnologyInfo(i)
		local name		= mgrInfo.TechName
		local maxValue	= mgrInfo.NeedPoint
		local isCur		= self.CurTechId == i
		techItemWnd:SetInfo(name, nowLevel, nowValue .. "/" .. maxValue, isCur)
		table.insert(self.m_TechListTbl, techItemWnd)
	end
end

----------------------------------------------
function CCofCTechWnd:CreateTechListItem(parent)
	local wnd = CCofCTechListItem:new()
	wnd:CreateFromRes("CofCTechItem", parent)
	wnd:ShowWnd( true )
	return wnd
end

function CCofCTechListItem:OnChildCreated()
	self.m_TechName		= self:GetDlgChild("TechName")
	self.m_Level		= self:GetDlgChild("Level")
	self.m_HaveLevel	= self:GetDlgChild("HaveLevel")
end

function CCofCTechListItem:SetInfo(name, level, haveLevel, isCur)
	self.m_TechName:SetWndText(isCur and name .. "(当前)" or name)
	self.m_Level:SetWndText(level)
	self.m_HaveLevel:SetWndText(haveLevel)
end

