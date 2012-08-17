gac_require "relation/cofc/CofCMainInc"
gac_require "relation/cofc/Members"
gac_require "relation/cofc/Technology"
gac_require "relation/cofc/Log"

--返回商会主界面信息
--参数：自己的职位，商会名称，等级，资金，商业指数，在线商会人数，总人数，人气值，资源产量加成，上周商会盈利，本周商会盈利，杂物回收加成，目前正在升级科技名称， 现有科技等级，总科技等级，商会宗旨
function Gas2Gac:RetGetCofcInfo(Conn,selfOccupation, name, level, money, exp, onlinePersonCount, totalPersonCount, populirity, res, lastProfit, thisProfit, recovery, curTechName, techLevel, techTotalLevel, propose)
	if g_GameMain.m_CofCMainWnd == nil then
		g_GameMain.m_CofCMainWnd = CreateChamberOfCommerce(g_GameMain)
	end
	g_GameMain.m_CofCMainWnd.m_CurQuestInfoTbl = {}
	g_GameMain.m_CofCMainWnd.m_selfOccu = selfOccupation	--自己在商会中的职位
	
	g_GameMain.m_CofCMainWnd.m_tblCofCInfo = {name, level, money, exp, onlinePersonCount, totalPersonCount, populirity, res, lastProfit, thisProfit, recovery, curTechName, techLevel, techTotalLevel, propose}
	g_GameMain.m_CofCMainWnd:OpenCofcMainWnd(name, level, money, exp, onlinePersonCount, totalPersonCount, populirity, res, lastProfit, thisProfit, recovery, curTechName, techLevel, techTotalLevel, propose)
	
	local assoListWnd = g_GameMain.m_AssociationWnd.m_AssociationListWnd
	if(assoListWnd) then
		assoListWnd:RetSetCofCList()
	end
end

function Gas2Gac:RetGetCofcQuestInfo(Conn, questName)
	table.insert(g_GameMain.m_CofCMainWnd.m_CurQuestInfoTbl,questName)
end

function Gas2Gac:RetGetCofcQuestInfoEnd(Conn)
	g_GameMain.m_CofCMainWnd:ShowCurQuestInfo()
end


--修改商会宗旨返回函数
function Gas2Gac:RetModifyCofcPropose( Conn )	
	local cofcMainWnd = g_GameMain.m_CofCMainWnd
	local newProposeText = cofcMainWnd.m_CofCModifyCPurposeWnd.m_ProposeText:GetWndText()	
	cofcMainWnd.m_CofCPurposeDesc:SetWndText(newProposeText)
end

---------------------------------------------------------------------------------

--创建商会主界面
function CreateChamberOfCommerce(Parent)
	local wnd = CChamberOfCommerceWnd:new()
	wnd:CreateFromRes("CofCMain",Parent)
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 1)
	wnd:ShowWnd(false)
	wnd:InitMainWndChild()
	return wnd
end

--用成员变量来记录商会主界面面板中的child控件
function CChamberOfCommerceWnd:InitMainWndChild()
	self.m_CofCName					=	self:GetDlgChild("Name")   								--商会名称
	self.m_CofCLevel 				= 	self:GetDlgChild("Level")								--商会等级
	self.m_CofCMoney				=	self:GetDlgChild("Money") 								--商会资金
	self.m_CofCMemberNo				= 	self:GetDlgChild("Exploit") 							--商会人数
	self.m_CofCCommerce 			= 	self:GetDlgChild("Popularity ") 						--商会指数
	self.m_CofCPopulirity			=	self:GetDlgChild("Stable") 								--人气值
	self.m_CofCSourceCount 			= 	self:GetDlgChild("SourceCountText") 					--资源产量加成
	self.m_CofCLastWeekProfit		= 	self:GetDlgChild("LastWeekProfitText") 					--上周商会盈利
	self.m_CofCCurWeekProfit		= 	self:GetDlgChild("CurWeekProfitText") 					--本周商会盈利
	self.m_CofCDebrisRecovery		= 	self:GetDlgChild("DebrisRecoveryText") 					--杂物回收加成
	self.m_CofCFinancialLogBtn 		=	self:GetDlgChild("CheckFinancialLog") 					--查看历史财报
	self.m_CofCCurTechnology		= 	self:GetDlgChild("CurUpdateTechText") 					--目前正在升级科技
	self.m_CofCModifyPurposeBtn		=	self:GetDlgChild("ChangePurposeBtn")					--修改宗旨按钮
	self.m_CofCPurposeDesc			=	self:GetDlgChild("PurposeText")							--显示宗旨窗口
	self.m_CofCCloseBtn				= 	self:GetDlgChild("Close")								--关闭窗口按钮
	self.m_CofCMemberBtn			= 	self:GetDlgChild("CofCMemberBtn")						--商会成员按钮
	self.m_CofCTechBtn				=	self:GetDlgChild("CofCTech")							--商会科技按钮
	self.m_CofCLogBtn				=	self:GetDlgChild("CofCLogBtn")							--商会日志按钮
	self.m_CofCCloseWndBtn			= 	self:GetDlgChild("ShutDownBtn")							--商会关闭按钮
	self.m_ChallengeLable			=	self:GetDlgChild("ChallengeLable")						--目前正在升级**科技
	self.m_OpenCofCStock			=	self:GetDlgChild("RequestOpenStock")
	self.m_CofCModifyPurposeBtn:EnableWnd(false)												--初始设置修改商会宗旨面板按钮为不可点状态
end

--商会面板事件响应函数
function CChamberOfCommerceWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  	if(Child == self.m_CofCCloseWndBtn or Child == self.m_CofCCloseBtn) then 			--关闭按钮
	  		self:ShowWnd(false)
	  	elseif(Child == self.m_CofCMemberBtn)    then 							--打开商会成员面板按钮
	  		self:OpenCofCMemberWnd()
	  	elseif(Child == self.m_CofCTechBtn)      then								--打开商会科技面板按钮										
	  		self:OpenCofCTechWnd()
	  	elseif(Child == self.m_CofCLogBtn)       then								--打开商会日志面板按钮	
	  		self:OpenCofCLogWnd()
	  	elseif(Child == self.m_CofCFinancialLogBtn) then							--查看历史财报
	  		self:CheckFinancialLog()	
	  	elseif(Child == self.m_CofCModifyPurposeBtn) then	 						--修改商会宗旨
	  		self:ModifyCofCPurpose()
	  	elseif(Child == self.m_OpenCofCStock) then
	  		Gac2Gas:RequestOpenCofCStock(g_Conn)
		end
	end
end

--打开商会主界面
function CChamberOfCommerceWnd:OpenCofcMainWnd(name, level, money, exp, onlinePersonCount, totalPersonCount, populirity, res, lastProfit, thisProfit, recovery, curTechName, techLevel, techTotalLevel, propose)
	self.m_CofCName:SetWndText( name )
	self.m_CofCLevel:SetWndText( level )
	self.m_CofCMoney:SetWndText( money )
	self.m_CofCMemberNo:SetWndText( onlinePersonCount .. "/" .. totalPersonCount )
	self.m_CofCCommerce:SetWndText( exp .. "%")
	self.m_CofCPopulirity:SetWndText( populirity )
	self.m_CofCSourceCount:SetWndText( res )
	self.m_CofCLastWeekProfit:SetWndText( lastProfit )
	self.m_CofCCurWeekProfit:SetWndText( thisProfit )
	self.m_CofCDebrisRecovery:SetWndText( recovery .. "%" )
	self.m_CofCCurTechnology:SetWndText( techLevel .. "/" .. techTotalLevel )
	self.m_CofCPurposeDesc:SetWndText( propose )
	self.m_ChallengeLable:SetWndText(GetStaticTextClient(1085, curTechName))
	if self.m_selfOccu	==  "会长" or self.m_selfOccu == "副会长" then
		self.m_CofCModifyPurposeBtn:EnableWnd(true)
	end
end

--显示商会主界面中的任务信息
function CChamberOfCommerceWnd:ShowCurQuestInfo()
	local questTbl = g_GameMain.m_CofCMainWnd.m_CurQuestInfoTbl
	for i =1, table.getn(questTbl) do
		local questNameWnd = Quest ..i
		local questWnd = self:GetDlgChild(questNameWnd)
		local questName = questTbl[i]
		questWnd:SetWndText(questName)
	end
end

--打开商会成员界面
function CChamberOfCommerceWnd:OpenCofCMemberWnd()
	if g_GameMain.m_CofCMemberWnd == nil then
		g_GameMain.m_CofCMemberWnd = CreateCofCMemberWnd(g_GameMain)
		g_GameMain.m_CofCMemberWnd:ShowWnd(true)
		Gac2Gas:GetCofCMembersInfo(g_Conn)
		return
	elseif g_GameMain.m_CofCMemberWnd:IsShow() == false then
		g_GameMain.m_CofCMemberWnd:ShowWnd(true)
		Gac2Gas:GetCofCMembersInfo(g_Conn)
	end
end

--打开商会科技界面
function CChamberOfCommerceWnd:OpenCofCTechWnd()
	if g_GameMain.m_CofCTechWnd == nil then
		g_GameMain.m_CofCTechWnd = CreateCofCTechWnd(g_GameMain)
	end
	g_GameMain.m_CofCTechWnd:ShowWnd(true)
	g_GameMain.m_CofCTechWnd:ClearTabel()
	Gac2Gas:GetCofcTechnologyInfo(g_Conn)
end

--打开商会日志界面
function CChamberOfCommerceWnd:OpenCofCLogWnd()
	if g_GameMain.m_CofCLogWnd == nil then
		g_GameMain.m_CofCLogWnd = CreateCofCLogWnd(g_GameMain)
		g_GameMain.m_CofCLogWnd:ShowWnd(true)
		local logType = 4 		--所有类型的log
		Gac2Gas:GetCofCLogInfo(g_Conn, 4)
		return
	elseif g_GameMain.m_CofCLogWnd:IsShow() == false then
		g_GameMain.m_CofCLogWnd:ShowWnd(true)
		g_GameMain.m_CofCLogWnd:ShowWnd(true)
		Gac2Gas:GetCofCLogInfo(g_Conn, 4)
	end
end

function CChamberOfCommerceWnd:CheckFinancialLog()
	local financialCurWeek = 0
	Gac2Gas:GetCofCFinancialReport(g_Conn,g_MainPlayer.m_Properties:GetCofcID(), financialCurWeek)
end


function CChamberOfCommerceWnd:ModifyCofCPurpose()
	if self.m_CofCModifyPurposeWnd == nil then
		self.m_CofCModifyCPurposeWnd = CreateModifyCofCPurposeWnd(self)
		self.m_CofCModifyCPurposeWnd:ShowWnd(true)
		return
	elseif self.m_CofCModifyPurposeWnd:IsShow() == false then
		self.m_CofCModifyCPurposeWnd:ShowWnd(true)
		self.m_CofCModifyPurposeWnd.m_ProposeText:SetWndText("")
	end
end

--修改商会宗旨窗口--------------------------------------------------------------------------------
function CreateModifyCofCPurposeWnd(Parent)
	local wnd = CModifyCofCPurposeWnd:new()
	wnd:CreateFromRes("CofCModifyPropose",Parent)
	wnd:ShowWnd( true )
	wnd.m_OkBtn 			= wnd:GetDlgChild("ok")		
	wnd.m_CloseBtn 			= wnd:GetDlgChild("close")
	wnd.m_ProposeText		= wnd:GetDlgChild("nameTest")			--商会宗旨内容
	g_ExcludeWndMgr:InitExcludeWnd(wnd, 3, false, {g_GameMain.m_CofCMainWnd})
	return wnd
end

function CModifyCofCPurposeWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
	  if(Child == self.m_CloseBtn) then 		--关闭按钮
	  	self:ShowWnd(false)
	  elseif(Child == self.m_OkBtn) then																				
	  	self:SetNewProposeText()
	  end
	end
end

--设置新的商会宗旨
function CModifyCofCPurposeWnd:SetNewProposeText()
	local proposeText = self.m_ProposeText:GetWndText()		
	local proposeLen = string.len(proposeText)
	if proposeLen > math.pow(2,8) then 				--限定商会宗旨长度
		self.m_MsgBox = MessageBox(self, MsgBoxMsg(16001), MB_BtnOK)
	else
		self:ShowWnd(false)
		Gac2Gas:ModifyCofcPropose( g_Conn, proposeText )	
	end																
end
