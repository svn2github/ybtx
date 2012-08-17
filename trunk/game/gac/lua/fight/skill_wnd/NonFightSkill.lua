cfg_load "skill/NonFightSkill_Common"
lan_load "skill/Lan_NonFightSkill_Common"

CNonFightSkill = class(SQRDialog)

--创建非战斗技能面板
function CNonFightSkill:Ctor()
	self:CreateFromRes("NonFightSkillWnd",g_GameMain.m_CommonSkillWnd)
end

function CNonFightSkill:OnChildCreated()
	self.m_SkillBtnTbl = {}
    local Keys = NonFightSkill_Common:GetKeys()
	for i, v in pairs(Keys) do 
		local RowInfo = NonFightSkill_Common(v)
		if tonumber(RowInfo("ViewPosition")) ~= 0 then
    		local BtnWnd		= self:GetDlgChild("SkillBtn" .. RowInfo("ViewPosition"))
    		BtnWnd.m_SkillName	= RowInfo("NonFightSkillName")
    		BtnWnd.m_Icon		= RowInfo("SkillSmallIcon")
    		BtnWnd.m_IsLearn 	= false
    		g_LoadGrayIconFromRes(RowInfo("SkillSmallIcon"), BtnWnd, -2, IP_ENABLE, IP_CLICKDOWN)
    		BtnWnd:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(RowInfo("NonFightSkillName"),0))
			table.insert(self.m_SkillBtnTbl, BtnWnd)
		end
	end
	self.m_FlashBtnNumber = 0
end

function CNonFightSkill:SetBtnFlash(Btn)
	Btn:AddFlashInfoByName("TalentItemFlash")
	if self.m_FlashBtnNumber <= 0 then
		self.m_FlashBtnNumber = 0
	end 
	self.m_FlashBtnNumber = self.m_FlashBtnNumber  + 1
	Btn.m_IsFlashBtn = true
	CFunctionBar.SetToolBarSkillBtnFlash()	
end

function CNonFightSkill:ClearAllBtnFlash()
	for i = 1, #(self.m_SkillBtnTbl) do 
		self.m_SkillBtnTbl[i]:DelFlashAttention()
		self.m_SkillBtnTbl[i].m_IsFlashBtn = nil
	end
	self.m_FlashBtnNumber = 0
end

function CNonFightSkill:ActiveNonFightSkill(SkillName,IsShowFlash)
	if NonFightSkill_Common(SkillName) then
		for i = 1,#(self.m_SkillBtnTbl) do
			if self.m_SkillBtnTbl[i].m_SkillName == SkillName then
				self.m_SkillBtnTbl[i].m_IsLearn = true
				g_LoadIconFromRes(self.m_SkillBtnTbl[i].m_Icon, self.m_SkillBtnTbl[i], -2, IP_ENABLE, IP_CLICKDOWN)
				if IsShowFlash then
					self:SetBtnFlash(self.m_SkillBtnTbl[i])
				end
				if not g_GameMain.m_LearnedSkillShowWnd then
					g_GameMain.m_LearnedSkillShowWnd = CreateLearnedSkillShowWnd(g_GameMain)
				end	
			end
		end
	end
end

function CNonFightSkill:IsShowBurstSoulWnd(SkillName)
	if("攒怒能力提升" == SkillName) then
		g_GameMain.m_BurstSoulWnd:ShowWnd(true)
	end
end

function CNonFightSkill:DelBtnFlash(Btn)
	if Btn.m_IsFlashBtn then
		Btn:DelFlashAttention()
		Btn.m_IsFlashBtn = nil
		self.m_FlashBtnNumber = self.m_FlashBtnNumber - 1
		CFunctionBar.DelToolBarSkillBtnFlash()
	end	
end

function CNonFightSkill:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_DRAG or uMsgID == BUTTON_LCLICK  then
		local state = g_WndMouse:GetCursorState()
		if(state == ECursorState.eCS_Normal) then
			for i = 1,#(self.m_SkillBtnTbl) do
				if Child == self.m_SkillBtnTbl[i] 
					and Child.m_IsLearn then
						local tbl = {}
						tbl.SkillName = Child.m_SkillName
						tbl.Level = 1
						g_WndMouse:SetPickupSkill( tbl, Child )
						self:DelBtnFlash(Child)
				end
			end
		end
	end
end

--激活非战斗技能
function CNonFightSkill.ReturnNonFightSkill(name)
	if g_GameMain.m_NonFightSkillWnd then
		g_GameMain.m_NonFightSkillWnd:ActiveNonFightSkill(name,true)
		if g_GameMain.m_LearnedSkillShowWnd then
			g_GameMain.m_LearnedSkillShowWnd:SetSkillBtnPicture(NonFightSkill_Common(name,"SkillSmallIcon"),name,1,0)
		end
	end
end


local function SetCameraPos()
	if not IsCppBound(g_MainPlayer) then
		return
	end
	
	local RenderScene=g_CoreScene:GetRenderScene()
	
	local RunTime = 10000--转一圈所用的时间
	
	local function Tick_CallBack(Tick)
		if RenderScene.SetCameraXRotate then
			RenderScene:SetCameraXRotate( RenderScene:GetCameraXRotate() + 2*3.1415926/(RunTime/20) )
			RenderScene:SetCameraYRotate( RenderScene:GetCameraYRotate() + 2*3.1415926/(RunTime/20) )
		end
	end
	
	--镜头拉远
	RenderScene:SetCameraOffset( 2000 )
	--开始转镜头
	if g_GameMain.bIsForbidFreedomCamera then
		CRenderSystemClient_Inst():SetFreeCameraEnabled(true)
	end
	if g_GameMain.m_CameraMoveTick then
		UnRegisterTick(g_GameMain.m_CameraMoveTick)
		g_GameMain.m_CameraMoveTick = nil
	end
	g_GameMain.m_CameraMoveTick = RegisterTick("CameraMoveTick",Tick_CallBack,20)
end

--open
function CNonFightSkill.LearnNoneFightSkill(TargetEntityID, SkillName)
	if IsCppBound(g_MainPlayer) then
		--1.目标加特效
		local IntObj = CIntObjClient_GetIntObjByID(TargetEntityID)
		if IntObj then
			IntObj.m_EffectId = IntObj:GetRenderObject():AddEffect( "fx/setting/obj/obj_0261.efx", "stand01/create", -1 ,nil)
		end
		
		--2.玩家播放动作
		g_MainPlayer:GetRenderObject():DoAni("renbuff",false, -1.0)
		
		--3.玩家加特效
		g_MainPlayer.m_FightSkillFx = g_MainPlayer:GetRenderObject():AddEffect("fx/setting/other/other/renbuff/create.efx", "renbuff/create", -1 ,nil)
		
		--移动镜头
		--if not string.find(g_GameMain.m_SceneName,"树屋酒吧") then
			SetCameraPos()
		--end
	end
end

--close
function CNonFightSkill.CancelLearnNoneFightSkill(TargetEntityID)
	if g_GameMain.m_CameraMoveTick then
		UnRegisterTick(g_GameMain.m_CameraMoveTick)
		g_GameMain.m_CameraMoveTick = nil
	end
	if IsCppBound(g_MainPlayer) then
		--1.玩家去掉特效
		if g_MainPlayer.m_FightSkillFx then
			g_MainPlayer:GetRenderObject():DelEffect(g_MainPlayer.m_FightSkillFx)
			g_MainPlayer.m_FightSkillFx = nil
		end		
		
		--2.目标去掉特效
		local IntObj = CIntObjClient_GetIntObjByID(TargetEntityID)
		if IntObj and IntObj.m_EffectId then
			IntObj:GetRenderObject():DelEffect(IntObj.m_EffectId)
			IntObj.m_EffectId = nil
		end
		
		--停止移动镜头,回到原始位置
		if g_GameMain.bIsForbidFreedomCamera then
			CRenderSystemClient_Inst():SetFreeCameraEnabled(false)
			--local RenderScene=g_CoreScene:GetRenderScene()
			--RenderScene:SetCameraYRotate(0)
		end
	end
end
