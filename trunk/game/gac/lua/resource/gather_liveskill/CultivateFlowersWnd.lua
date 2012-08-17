gac_gas_require "activity/item/FlowerInfoMgr"
CFlowerWnd = class(SQRDialog)

local function AddImage(Wnd, StateStr)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE	
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(Wnd:GetGraphic(), -1,  StateStr, nil, CFPos:new(), "0xffffffff", 15 ) 	
	Wnd:SetWndBkImage( Flag, DefaultImage )
end

local function ShowTooltips(Wnd, str)
	if Wnd then
		Wnd:SetMouseOverDescAfter(str)
	end
end

function CFlowerWnd:Ctor()
	self:Init(g_GameMain)
	self:ShowWnd(false)
end

function CFlowerWnd:Init(Parent)
	self:CreateFromRes("FlowerWnd", Parent)
end

--获取tooltip上显示的文字描述和冷却时间的倒计时
local function GetToolTipStr(MsgId, Time)
	local str = GetStaticTextClient(MsgId)
	str = str..GetStaticTextClient(6019, Time)
	return str
end

--显示带倒计时的tooltip
local function SkillCooldownTooltips(Time)
	if Time > 0 then
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_WaterBtn, GetToolTipStr(6004,Time))
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_WormBtn, GetToolTipStr(6005,Time))
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_SunShineBtn, GetToolTipStr(6006,Time))
	else
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_WaterBtn, GetStaticTextClient(6004))
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_WormBtn, GetStaticTextClient(6005))
		ShowTooltips(g_GameMain.m_LiveSkillFlower.m_SunShineBtn, GetStaticTextClient(6006))
	end
end

--绘制OBJ
function CFlowerWnd:DrawObj()
	self.m_Flower.m_WndBody.RenderObj:SetDirection(CDir:new(self.m_nDir))
end

function CFlowerWnd:SetAni()
	self.m_Flower.m_WndBody.RenderObj:DoAni( "stand01", true, -1.0 )
end

function CFlowerWnd:AddEffect(wnd, StateStr, isCircle)
	if StateStr == nil then
		return
	end
	local n = 0
	local ModelString,StateString = StateStr[1],StateStr[2]
	if isCircle then
		n = -1
	end
	return wnd.m_WndBody.RenderObj:AddEffect( ModelString, StateString, n, nil )
end

function CFlowerWnd:GrowthEffect()
	if self.m_FlowerGrowthEfx then
		self.m_Flower.m_WndBody.RenderObj:DelEffect(self.m_FlowerGrowthEfx)
	end
	local str = g_FlowerInfoMgr:GetFlowerInfo(self.m_FlowerName, "生长特效", self.m_Rank)
	if IsTable(str) then
		self.m_FlowerGrowthEfx = self:AddEffect(self.m_Flower, str, true)
	end
end

--更新花卉状态(健康,长虫等)
function CFlowerWnd:UpdateFlowerState(State)
	self.m_State = State
	if State == 0 then
		g_GameMain.m_LiveSkillFlower:SkillBtnIsShow(false)
		--ShowTooltips(self.m_FlowerStateWnd, "")
		self.m_Flower.m_WndBody.RenderObj:RemoveAllPiece()
		self.m_Flower.m_WndBody.RenderObj:Delframework()
		g_GameMain.m_LiveSkillFlower:ShowWnd(false)
		g_GameMain.m_FlowerMiniWnd:ShowWnd(false)
	else
		if State ~= 1 then
			self:RegisterEventTick()
		end
		--ShowTooltips(self.m_FlowerStateWnd, GetStaticTextClient(6000 + State))
		self.m_Flower.m_WndBody.RenderObj:DelEffect(self.m_FlowerStateEfx)
		self.m_FlowerStateEfx = self:AddEffect(self.m_Flower, CultivateFlowerStateEfx[State], true)
	end
end

local function FlowerAddPiece(RenderObj, Info, Dir)
	RenderObj:RemoveAllPiece()
	RenderObj:Delframework()
	RenderObj:AddPiece( RES_TYPE.RFT_ARA, Info("AniFile") , "", 0 )
	RenderObj:AddPiece( RES_TYPE.RFT_ARP, Info("ModelFile") , "",  Info("RenderStyle") )
	RenderObj:SetDirection(CDir:new(Dir))
	RenderObj:SetScale( Info("Scaling") )
	RenderObj:SetPosition(CVector3f:new( 0, -100, -100 ) )
	RenderObj:DoAni( "stand01", true, -1.0 )
end

--花卉成长(下一阶段贴图)
function CFlowerWnd:FlowerGrowth(Rank)
	local str = g_FlowerInfoMgr:GetFlowerInfo(self.m_FlowerName,"显示图像",Rank)
	if not self:IsShow() and
		 not g_GameMain.m_FlowerMiniWnd:IsShow() then
		g_GameMain.m_FlowerMiniWnd:ShowWnd(true)
	end
	self:SkillBtnIsShow(true)
	local RenderObj = self.m_Flower.m_WndBody.RenderObj
	local Info = IntObjRes_Client(str)
	if Info == nil then
		return
	end
	FlowerAddPiece(RenderObj, Info, self.m_nDir)
end

local function GetRankByGrowthTime(FlowerInfo, GrowthTime)
	local rank = 1
	local Arg = FlowerInfo[rank]
	while IsNumber(Arg) do
		if (GrowthTime - Arg) <= 0 then
			return rank, Arg - GrowthTime
		else
			GrowthTime = GrowthTime - Arg
		end
		rank = rank + 1
		Arg = FlowerInfo[rank]
	end
	Gac2Gas:FlowerPerishGiveItem(g_Conn, true)
end

function CFlowerWnd:StartGrowth(FlowerName, leftTime)
	local GrowthTime
	self.m_FlowerName = FlowerName
	self.m_FlowerGrowthTime = g_FlowerInfoMgr:GetFlowerInfo(FlowerName,"生长时间")
	self.m_Rank, GrowthTime = GetRankByGrowthTime(self.m_FlowerGrowthTime, leftTime)
	self:UpdateFlowerState(1)
	self:ShowWnd(true)
	self:SetFocus()
	self:RegisterGrowthTimeTick(GrowthTime)
end

function CFlowerWnd:UpdateGrowthTime(FlowerName, leftTime)
	local FlowerGrowthTimeTbl = g_FlowerInfoMgr:GetFlowerInfo(FlowerName,"生长时间")
	self.m_Rank, self.m_GrowthTime = GetRankByGrowthTime(FlowerGrowthTimeTbl, leftTime)
end

--花卉技能图标显示控制(没有种花的时候不显示)
function CFlowerWnd:SkillBtnIsShow(bShow)
	self.m_WaterBtn:ShowWnd(bShow)
	self.m_WormBtn:ShowWnd(bShow)
	self.m_SunShineBtn:ShowWnd(bShow)
	self.m_GetItemBtn:ShowWnd(bShow)
end

--花卉技能使用控制(冷却中不能使用)
function CFlowerWnd:SkillBtnEnable(bEnable)
	if bEnable then
		SkillCooldownTooltips(0)
	end
	self.m_WaterBtn:EnableWnd(bEnable)
	self.m_WormBtn:EnableWnd(bEnable)
	self.m_SunShineBtn:EnableWnd(bEnable)
	
	self.m_WaterCD:ShowWnd(not bEnable)
	self.m_WormCD:ShowWnd(not bEnable)
	self.m_SunShineCD:ShowWnd(not bEnable)
end

function CFlowerWnd:SetSkillCDPos(num)
	if num <= 0 then
		self.m_WaterCD:ShowWnd(false)
		self.m_WormCD:ShowWnd(false)
		self.m_SunShineCD:ShowWnd(false)
	else
		self.m_WaterCD:SetPos(num)
		self.m_WormCD:SetPos(num)
		self.m_SunShineCD:SetPos(num)
		self.m_WaterCD:ShowWnd(true)
		self.m_WormCD:ShowWnd(true)
		self.m_SunShineCD:ShowWnd(true)
	end
end

function CFlowerWnd:OnChildCreated()
	self.m_Flower = {}
	self.m_Flower.m_WndBody		= self:GetDlgChild("Flower")
	self.m_CloseWnd				= self:GetDlgChild("Close")
	self.m_WaterBtn				= self:GetDlgChild("Water")
	self.m_WormBtn				= self:GetDlgChild("Worm")
	self.m_SunShineBtn		= self:GetDlgChild("SunShine")
	self.m_GetItemBtn			= self:GetDlgChild("GetItem")
	
	self.m_GetItemCD			= self:GetDlgChild("GetItemCD")
	self.m_WaterCD				= self:GetDlgChild("WaterCD")
	self.m_WormCD					= self:GetDlgChild("WormCD")
	self.m_SunShineCD			= self:GetDlgChild("SunShineCD")
	
	self.m_GrowthTimeWnd	= self:GetDlgChild("GrowthTime")
	
	self.m_GetItemCD:SetProgressMode(3)
	self.m_WaterCD:SetProgressMode(3)
	self.m_WormCD:SetProgressMode(3)
	self.m_SunShineCD:SetProgressMode(3)
	
	local MaxRange = CultivateFlowerParams["SkillCooldownTime"]
	local GetItemCooldownTime = CultivateFlowerParams["GetItemCooldownTime"]
	self.m_GetItemCD:SetRange(GetItemCooldownTime)
	self.m_WaterCD:SetRange(MaxRange)
	self.m_WormCD:SetRange(MaxRange)
	self.m_SunShineCD:SetRange(MaxRange)
	
	self.m_GetItemCD:ShowWnd(false)
	self.m_WaterCD:ShowWnd(false)
	self.m_WormCD:ShowWnd(false)
	self.m_SunShineCD:ShowWnd(false)
	
	ShowTooltips(self.m_WaterBtn, GetStaticTextClient(6004))
	ShowTooltips(self.m_WormBtn, GetStaticTextClient(6005))
	ShowTooltips(self.m_SunShineBtn, GetStaticTextClient(6006))
	
	OnCreateRoleModel(self.m_Flower)
	self.m_nDir				 = 90
	self.m_State			 = 0
	self.m_Rank				 = 1
	self.m_HealthPoint = 0
	self.m_GetCount		 = 0
	self.m_GiveItemTime = GetItemCooldownTime
	self.m_FlowerGrowthTime = nil
	self.m_FlowerName = ""
	self:SkillBtnIsShow(false)
end

function CFlowerWnd:UpdateGrowthTimeWnd()
	if self.m_GrowthTime and self.m_GrowthTime >= 0 then
		ShowTooltips(self.m_GrowthTimeWnd, GetStaticTextClient(6021, self.m_GrowthTime))
	end
end

function CFlowerWnd:OnDestroy()
	self:FlowerOnDead()
	OnDestroy(self.m_Flower)
	self.m_Flower = nil
	g_GameMain.m_LiveSkillFlower = nil
	self = nil
end

function CFlowerWnd:FlowerOnDead()
	self:ShowWnd(false)
	if self.m_GrowthTimeTick then
		UnRegisterTick(self.m_GrowthTimeTick)
		self.m_GrowthTimeTick = nil
	end
	if self.m_EventTick then
		UnRegisterTick(self.m_EventTick)
		self.m_EventTick = nil
	end
	if self.m_FlowerCooldownTick then
		UnRegisterTick(self.m_FlowerCooldownTick)
		self.m_FlowerCooldownTick = nil
	end
	if self.m_GiveItemTick then
		UnRegisterTick(self.m_GiveItemTick)
		self.m_GiveItemTick = nil
	end
	self.m_Flower.m_WndBody.RenderObj:DelEffect(self.m_FlowerStateEfx)
	if self.m_FlowerGrowthEfx then
		self.m_Flower.m_WndBody.RenderObj:DelEffect(self.m_FlowerGrowthEfx)
	end
	self.m_State			 = 0
	self.m_Rank				 = 0
	self.m_HealthPoint = 0
	self.m_FlowerGrowthTime = nil
	self.m_FlowerName = ""
	self:SkillBtnIsShow(false)
	self:UpdateFlowerState(0)
	self:SkillBtnEnable(true)
	self.m_GetItemBtn:EnableWnd(true)
	self.m_GetItemCD:ShowWnd(false)
	self.m_WaterCD:ShowWnd(false)
	self.m_WormCD:ShowWnd(false)
	self.m_SunShineCD:ShowWnd(false)
end

function CFlowerWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		local param = 0
		if Child == self.m_WaterBtn then
			param = 1
		elseif Child == self.m_WormBtn then
			param = 2
		elseif Child == self.m_SunShineBtn then
			param = 3
		elseif Child == self.m_GetItemBtn then
			if self.m_GiveItemTick then
				return
			end
			Gac2Gas:FlowerPerishGiveItem(g_Conn, false)
		elseif Child == self.m_CloseWnd then
			g_GameMain.m_FlowerMiniWnd:ShowWnd(true)
			self:ShowWnd(false)
		end
		if param == 0 then
			return
		end
		if param ~= 1 and param ~= self.m_State then
			MsgClient(9603 + param, GetFlowerDispalyName(self.m_FlowerName))
			return
		end
		Gac2Gas:CareForFlower(g_Conn, param)
	end
end

function CFlowerWnd:ShowGetItemBtnTooltips()
	if self.m_GiveItemTick and self.m_GiveItemTime > 0 then
		ShowTooltips(self.m_GetItemBtn, GetStaticTextClient(6007)..GetStaticTextClient(6014,self.m_HealthPoint)..GetStaticTextClient(6020,self.m_GetCount)..GetStaticTextClient(6019, self.m_GiveItemTime))
	else
		ShowTooltips(self.m_GetItemBtn, GetStaticTextClient(6007)..GetStaticTextClient(6014,self.m_HealthPoint)..GetStaticTextClient(6020,self.m_GetCount))
	end
end

function CFlowerWnd:UpdateFlowerHealthPoint(HealthPoint, flag)
	if HealthPoint > 0 and self.m_EventTick then
		UnRegisterTick(self.m_EventTick)
		self.m_EventTick = nil
	end
	if flag then
		self.m_HealthPoint = self.m_HealthPoint + HealthPoint
	else
		self.m_HealthPoint = HealthPoint
	end
	self:ShowGetItemBtnTooltips()
end

function CFlowerWnd:UpdateFlowerGetCount(GetCount)
	self.m_GetCount = GetCount
	self:ShowGetItemBtnTooltips()
end

function CFlowerWnd:RegisterCooldown(time)
	SkillCooldownTooltips(time)
	
	if time <= 0 then
		if self.m_FlowerCooldownTick then
			UnRegisterTick(self.m_FlowerCooldownTick)
			self.m_FlowerCooldownTick = nil
		end
		self:SkillBtnEnable(true)
		return
	end
	
	local CooldownTime = time
	local function CooldownFun()
		CooldownTime = CooldownTime - 1
		self:SetSkillCDPos(CooldownTime)
		if CooldownTime > 0 then
			SkillCooldownTooltips(CooldownTime)
		elseif CooldownTime<= 0 then
			UnRegisterTick(self.m_FlowerCooldownTick)
			self.m_FlowerCooldownTick = nil
			self:SkillBtnEnable(true)
		end
	end
	if self.m_FlowerCooldownTick then
		UnRegisterTick(self.m_FlowerCooldownTick)
	end
	self:SetSkillCDPos(CooldownTime)
	self.m_FlowerCooldownTick = RegisterTick("CooldownTick", CooldownFun, 1000)
	self:SkillBtnEnable(false)
end

--每阶段生长Tick
function CFlowerWnd:RegisterGrowthTimeTick(GrowthTime)

	if self.m_GrowthTimeTick then
		UnRegisterTick(self.m_GrowthTimeTick)
		self.m_GrowthTimeTick = nil
	end
	if GrowthTime then
		self.m_GrowthTime = GrowthTime
	else
		self.m_Rank = self.m_Rank + 1
		self.m_GrowthTime = self.m_FlowerGrowthTime[self.m_Rank]
	end
	--花的生长周期结束了
	if self.m_GrowthTime == nil or self.m_GrowthTime == "" or type(self.m_GrowthTime) ~= "number" then
		self.m_GrowthTime = 0
		Gac2Gas:FlowerPerishGiveItem(g_Conn, true)
		return
	end
	self:FlowerGrowth(self.m_Rank)
	--if g_GameMain.m_FlowerMiniWnd:IsShow() then
	if self.m_Rank ~= 1 then
		MsgClient(9622, GetFlowerDispalyName(self.m_FlowerName))
	end
	--end
	self:AddEffect(self.m_Flower, CultivateFlowerCleanStateEfx[0])
	self:GrowthEffect()
	local function GrowthFun()
		self.m_GrowthTime = self.m_GrowthTime - 1
		--print("**************************"..(self.m_GrowthTime).."*******************************")
		if self.m_GrowthTime <= 0 then
			self:RegisterGrowthTimeTick()
		end
		self:UpdateGrowthTimeWnd()
	end
	--print("==========="..self.m_Rank.."========"..self.m_GrowthTime.."======="..(os.time()).."===============================")
	self:UpdateGrowthTimeWnd()
	self.m_GrowthTimeTick = RegisterTick("GrowthTimeTick", GrowthFun, 1000)
end

--发生事件Tick(长虫,缺少阳光)
function CFlowerWnd:RegisterEventTick()
	if self.m_State ~= 2 and self.m_State ~= 3 then
		return
	end
	local function EventFun()
		UnRegisterTick(self.m_EventTick)
		self.m_EventTick = nil
		local uMsgId = 9610 + self.m_State
		self:UpdateFlowerState(1)
		Gac2Gas:FlowerStateClean(Conn)
		MsgClient(uMsgId, GetFlowerDispalyName(self.m_FlowerName))
	end
	if self.m_EventTick then
		UnRegisterTick(self.m_EventTick)
	end
	self.m_EventTick = RegisterTick("FlowerEventTick", EventFun, CultivateFlowerParams["EventTime"]*1000)
end

function CFlowerWnd:FlowerGiveItemCoolDownCD()
	local function tickfun()
		self.m_GiveItemTime = self.m_GiveItemTime - 1
		self.m_GetItemCD:SetPos(self.m_GiveItemTime)
		if self.m_GiveItemTime <= 0 then
			self.m_GetItemBtn:EnableWnd(true)
			self.m_GetItemCD:ShowWnd(false)
			UnRegisterTick(self.m_GiveItemTick)
			self.m_GiveItemTick = nil
			self.m_GiveItemTime = CultivateFlowerParams["GetItemCooldownTime"]
		end
		self:ShowGetItemBtnTooltips()
	end
	self.m_GetItemBtn:EnableWnd(false)
	if self.m_GiveItemTick then
		UnRegisterTick(self.m_GiveItemTick)
		self.m_GiveItemTick = nil
		self.m_GiveItemTime = CultivateFlowerParams["GetItemCooldownTime"]
	end
	self.m_GetItemCD:SetPos(self.m_GiveItemTime)
	self.m_GetItemCD:ShowWnd(true)
	self.m_GiveItemTick = RegisterTick("GiveItemTick", tickfun, 1000)
	self:ShowGetItemBtnTooltips()
end
