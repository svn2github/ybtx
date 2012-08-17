cfg_load "npc/NpcShowTask_Common"
CCreateListCtrlItemWnd = class(SQRDialog)

local function Transfer(Name,EntityID)
	Gac2Gas:RemitSpecScene(g_Conn,EntityID, Name)
end

local function CreateNpc(Name,EntityID)
	Gac2Gas:RenoSpecNpc(g_Conn, EntityID, Name)
end

--local function TransCostMoney(Name, EntityID, id)
--	Gac2Gas:RetTransInfo(g_Conn, Name, EntityID, id)
--end


function CCreateListCtrlItemWnd:Ctor(parent, WndName, FunType,TriggerType,TargetEntityID,TexPath)
	self:CreateFromRes("SpecialNpcDialogBoxListCtrlItemListItem", parent)
	self:ShowWnd(true)
	self.m_Text = self:GetDlgChild("Text")
	self.m_Icon = self:GetDlgChild("Icon")
	self:SetStyle(0x64040000)
	if TexPath ~= "" and type(TexPath) == "string" then
		local Flag = IMAGE_PARAM:new()
		Flag.CtrlMask = SM_BS_BK
		Flag.StateMask = IP_ENABLE
		local DefaultImage = WND_IMAGE_LIST:new()
		DefaultImage:AddImage(self.m_Icon:GetGraphic(), -1, TexPath, nil, CFPos:new(), "0xffffffff", 15 ) 	
		self.m_Icon:SetWndBkImage( Flag, DefaultImage )
		self.m_Icon:ShowWnd(true)
	else
		self.m_Icon:ShowWnd(false)
	end
	self.m_Text:SetWndText(WndName)
	self.m_Type = FunType
	self.m_Name = TriggerType
	self.m_EntityID = TargetEntityID
	parent.m_CreateList = self
	self:GetWnd()
end

function CCreateListCtrlItemWnd:GetWnd()
	self:ShowWnd(true)
	return self
end

function CCreateListCtrlItemWnd:CanTransport()
	local Npc = CCharacterFollower_GetCharacterByID(self.m_EntityID)
	if not IsCppBound(Npc) then
		return
	end
	local npcName = Npc.m_Properties:GetCharName()
	local NpcShowTaskCommon = NpcShowTask_Common(npcName)
	local NpcShowTaskIndex = NpcShowTaskCommon:GetKeys()
	for i, _ in pairs(NpcShowTaskIndex) do
		local str = i..""
		local TaskType = NpcShowTaskCommon(str, "TaskType")
		local TriggerType = NpcShowTaskCommon(str, "TriggerType")
		local CostMoney = NpcShowTaskCommon(str, "CostMoney")
		if TriggerType == self.m_Name then
			local CostMoney = NpcShowTaskCommon(str, "CostMoney")
			local TriggerTypeShowMessage = NpcShowTaskCommon(str, "TriggerTypeShowName")
			local index = npcName .. TriggerType
			local TriggerTypeShowMessage = Lan_NpcShowTask_Common(MemH64(index), "TriggerTypeShowMessage")
			if CostMoney > 0 then
				if g_MainPlayer.m_nBindMoney >=  CostMoney then	
					CCreateListCtrlItemWnd.ShowTransWndInfo(Conn, TriggerTypeShowMessage, CostMoney, TriggerType, self.m_EntityID, true)
					return
				elseif g_MainPlayer.m_nBindMoney < CostMoney and g_MainPlayer.m_nMoney >=  CostMoney then
					CCreateListCtrlItemWnd.ShowTransWndInfo(Conn, TriggerTypeShowMessage, CostMoney, TriggerType, self.m_EntityID, false)
					return
				else
					MsgClient(380003)
					return
				end
				--TransCostMoney(self.m_Name, self.m_EntityID,g_MainPlayer.m_Properties:GetCharID())
			else 
				Transfer(self.m_Name,self.m_EntityID)
			end
		end
	end
end

function CCreateListCtrlItemWnd:RunSpecialFun()
	if self.m_Type == "Transmit" then
		self:CanTransport()
	elseif self.m_Type == "Renovate" then
		CreateNpc(self.m_Name,self.m_EntityID)
	end
end

function CCreateListCtrlItemWnd.ShowTransWndInfo(Conn, TriggerType, CostMoney, Name, EntityID, isBind)
	local money = nil
	local str = nil
	if isBind then
		money = g_MoneyMgr:ChangeMoneyToString(CostMoney, EGoldType.GoldBar)
		str = MsgBoxMsg(200001,TriggerType,money)
	else
		money = g_MoneyMgr:ChangeMoneyToString(CostMoney, EGoldType.GoldCoin)
		str = MsgBoxMsg(200002,TriggerType,money)
	end
	local function IsCanTransport(g_GameMain,uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:IsCanTransport(g_Conn, CostMoney, Name, EntityID, isBind)--弹出另一个窗口
		end
		return true
	end
	g_GameMain.m_SpecialInfo = MessageBox( g_GameMain, str, BitOr( MB_BtnOK, MB_BtnCancel),IsCanTransport,g_GameMain)
end
