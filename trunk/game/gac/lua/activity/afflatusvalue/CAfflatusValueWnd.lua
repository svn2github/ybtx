cfg_load "player/LevelExp_Common"
CAfflatusValueWnd = class(SQRDialog)

function CAfflatusValueWnd:Ctor(parent)
	self:CreateFromRes("offline_exp_wnd",parent) 
	self.m_CurOffLineExp 		= self:GetDlgChild("CurOffLineExp")
	self.m_CurAfflatus 			= self:GetDlgChild("CurAfflatus")
	self.m_ExChangeAfflatus 	= self:GetDlgChild("ExChangeAfflatus")
	self.m_AfflatusOutPut 		= self:GetDlgChild("AfflatusOutPut")
	self.m_GoldDemand 			= self:GetDlgChild("GoldDemand")
	self.m_SilverDemand			= self:GetDlgChild("SilverDemand")
	self.m_CopperDemand			= self:GetDlgChild("CopperDemand")
	self.m_CirGold				= self:GetDlgChild("CirGold")
	self.m_CirSilve  			= self:GetDlgChild("CirSilve")
	self.m_CirCopper 			= self:GetDlgChild("CirCopper")
	self.m_Confirm 				= self:GetDlgChild("Confirm")
	self.m_CloseBtm				= self:GetDlgChild("Close")
	self.m_uCurMaxLevelLimit	= 0
	self.m_CurAfflatusValue 	= 0
	
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
	self:ShowWnd(false)
end

function CAfflatusValueWnd.OpenAfflatusValueWnd()
	local AfflatusValueWnd = CAfflatusValueWnd.GetWnd()
	if not AfflatusValueWnd:IsShow() then
		Gac2Gas:GetAfflatus_OffLineExp_Info(g_Conn)
	else
		AfflatusValueWnd:ShowWnd(false)
	end
end

function CAfflatusValueWnd.GetWnd()
	local Wnd = g_GameMain.m_AfflatusValue
	if not Wnd then
		Wnd = CAfflatusValueWnd:new(g_GameMain)
		g_GameMain.m_AfflatusValue = Wnd
	end
	return Wnd
end

local function TransformMoney(Money)
	local uGold = uint32(Money/10000)							--金币需求
	local uLeftCopper = Money - uGold*10000
	local uSilve = uint32(uLeftCopper/100)						--银币需求
	local uCopper = uLeftCopper - uSilve*100					--铜币需求
	return uGold, uSilve, uCopper
end

--计算灵感值和经验与金钱的兑换关系
function CAfflatusValueWnd:GetMoneyAndOffLineExpCost(uAfflatusValue)
	local uTotalCopper = 100*uAfflatusValue						--总共需要的铜币
	local uGold, uSilve, uCopper = TransformMoney(uTotalCopper)
	local uOffLineExp = 5000*uAfflatusValue					--离线经验
	return uGold, uSilve, uCopper, uOffLineExp
end

function CAfflatusValueWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_CloseBtm then
			self:ShowWnd(false)
		elseif Child == self.m_Confirm then
			local strText = self.m_ExChangeAfflatus:GetWndText()
			if strText ~= "" then
				local uExChange = uint32(tonumber(strText))
				Gac2Gas:ExChangeAfflatus(g_Conn, uExChange)
			else
				MsgClient(350006)
			end
		end
	elseif Child == self.m_ExChangeAfflatus and (uMsgID == EDIT_ONCHAR or uMsgID == EDIT_DELETE) then
		local strText = self.m_ExChangeAfflatus:GetWndText()
		local uCurAfflatusValue = 0
		if strText ~= "" then
			uCurAfflatusValue = tonumber(strText)
		end
		if uCurAfflatusValue == nil then
			MsgClient(350005)
			self.m_ExChangeAfflatus:SetWndText("")
			uCurAfflatusValue = 0
		elseif (uCurAfflatusValue - uint32(uCurAfflatusValue)) ~= 0  then
			MsgClient(350005)
			self.m_ExChangeAfflatus:SetWndText("")
			uCurAfflatusValue = 0
		elseif uCurAfflatusValue < 0 or uCurAfflatusValue == 0 then
			MsgClient(350006)
			self.m_ExChangeAfflatus:SetWndText("")
			uCurAfflatusValue = 0
		elseif uCurAfflatusValue > self.m_CurAfflatusValue then
			MsgClient(350003)
			self.m_ExChangeAfflatus:SetWndText("")
			uCurAfflatusValue = 0
		elseif g_MainPlayer:CppGetLevel() >= self.m_uCurMaxLevelLimit then
			MsgClient(350007)
			self.m_ExChangeAfflatus:SetWndText("")
			uCurAfflatusValue = 0
		end
		
		local GoldDemand, SilverDemand, CopperDemand, OffLineExpDemand = self:GetMoneyAndOffLineExpCost(uCurAfflatusValue)
		self.m_AfflatusOutPut:SetWndText(uCurAfflatusValue .. GetStaticTextClient(1292) .. OffLineExpDemand)
		self.m_GoldDemand:SetWndText(GoldDemand)
		self.m_SilverDemand:SetWndText(SilverDemand)
		self.m_CopperDemand:SetWndText(CopperDemand)
	end
end

function CAfflatusValueWnd.InitPlayerAfflatusValue(Conn, CurOffLineExp, CurAfflatusValue, Money, MaxLevelLimit)
	local Wnd = g_GameMain.m_AfflatusValue
	
	local uLevel = g_MainPlayer:CppGetLevel()
	local uMaxOffLineExp		= LevelExp_Common(uLevel, "OffLineExpLimit")
	local uMaxValueLimit = LevelExp_Common(uLevel, "AfflatusValue")
	Wnd.m_CurOffLineExp:SetWndText(CurOffLineExp.."/"..uMaxOffLineExp)
	Wnd.m_CurAfflatus:SetWndText(CurAfflatusValue.."/"..uMaxValueLimit)
	Wnd.m_CurAfflatusValue = CurAfflatusValue
	Wnd.m_ExChangeAfflatus:SetWndText("")
	Wnd.m_AfflatusOutPut:SetWndText(0 .. GetStaticTextClient(1292) .. 0)
	Wnd.m_GoldDemand:SetWndText(0)
	Wnd.m_SilverDemand:SetWndText(0)
	Wnd.m_CopperDemand:SetWndText(0)
	
	local uGold, uSilve, uCopper = TransformMoney(Money)
	Wnd.m_CirGold:SetWndText(uGold)
	Wnd.m_CirSilve:SetWndText(uSilve)
	Wnd.m_CirCopper:SetWndText(uCopper)
	
	Wnd.m_uCurMaxLevelLimit = MaxLevelLimit
	
	Wnd:ShowWnd(true)
end



