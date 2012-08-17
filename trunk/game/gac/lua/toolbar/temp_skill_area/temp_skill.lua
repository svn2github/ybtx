gac_require "toolbar/temp_skill_area/temp_skillInc"
cfg_load "skill/SkillPart_Common"

local MaxNumber = 6 
local CDColor = 0xaa0d000e
function CreateTempSkill(Parent)
	local Wnd = CTempSkill:new()
	Wnd:CreateFromRes("TempSkillWnd",Parent)
	
	Wnd.m_BtnTBl = {}
	Wnd.m_CdAnimationTbl = {}
	Wnd.m_SkillDuraTimeTbl = {}
	Wnd.m_ShortCutsIndexTbl = {}
	for i = 1,6 do 
		local Btn = Wnd:GetDlgChild("SkillBtn" .. i)
		Btn.m_State = EShortcutPieceState.eSPS_None
		Btn.Pos = i
		table.insert(Wnd.m_BtnTBl,Btn)
		
		local CdBtn = Wnd:GetDlgChild("CdAnimation" .. i)
		table.insert(Wnd.m_CdAnimationTbl,CdBtn)
		CdBtn:SetBackImageColor(CDColor)
		CdBtn:SetParent(Btn)
		CdBtn:ShowWnd(false)
		
		local DuraTimeStatic = Wnd:GetDlgChild("SkillDuraTime" .. i)
		DuraTimeStatic.Pos = i
		table.insert(Wnd.m_SkillDuraTimeTbl,DuraTimeStatic)
		
		local ShortCut = Wnd:GetDlgChild("ShortCutsIndex" .. i)
		ShortCut.Pos = i
		table.insert(Wnd.m_ShortCutsIndexTbl,ShortCut)
	end
		
	return Wnd
end

--临时技能栏事件响应
function CTempSkill:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child.m_State == EShortcutPieceState.eSPS_Skill then
			local Btn=self.m_BtnTBl[Child.Pos]
			local tbl = Btn.m_SkillContext
			if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= "" then
				local SkillName = g_MainPlayer.m_ActionSkillName
				if SkillName and GroundSelector:IsActive() then
					GroundSelector:OnLButtonUp()
					return
				else
					if tbl.SkillName == SkillName then
						Gac2Gas:ActiveBehavior(g_Conn)
						g_MainPlayer:UseTempSkill(tbl.SkillName,tbl.SkillLevel,Child.Pos)
					else
						return
					end
				end
			else
				g_MainPlayer:UseTempSkill(tbl.SkillName,tbl.SkillLevel,Child.Pos)
			end
		end
	elseif uMsgID == BUTTON_CLICKDOWN then
		if Child.m_State == EShortcutPieceState.eSPS_Skill then
			local Btn=self.m_BtnTBl[Child.Pos]
			Btn:AddFlashInfoByName("skillclickdown1")
		end		
	end
end

--添加技能
function CTempSkill:AddTempSkill(SkillName,SkillLevel,time,pos)
	self:DeleteTempSkill(pos)
	if not self:IsShow() then
		self:ShowWnd(true)
	end
	local tbl = {}
	tbl.SkillName = SkillName
	tbl.SkillLevel = SkillLevel
	tbl.Time = time 
	
	if tbl.SkillName ~= nil then
		self.m_BtnTBl[pos].m_State = EShortcutPieceState.eSPS_Skill
		self.m_BtnTBl[pos].m_SkillContext = tbl
		self:DrawWnd(self.m_BtnTBl[pos])
		if tbl.Time ~= 0 then
			self.m_SkillDuraTimeTbl[pos]:SetWndText(tbl.Time)
			self.m_BtnTBl[pos].IsShowTick = RegClassTick("IsShowTick", self.WndNoShow,1000,self,self.m_SkillDuraTimeTbl[pos])
		end
		local skillCd = g_MainPlayer:GetCoolDownTime(SkillName,SkillLevel) * 1000
		local lefttime = g_MainPlayer:GetSkillLeftCoolDownTime(SkillName)
		if lefttime ~= 0 then
			self:SetCdFun(self.m_BtnTBl[pos],skillCd,lefttime)
		end
	else
		return
	end	
end

--加载图片和设置ToolTips
function CTempSkill:DrawWnd(Btn)
	local tbl = Btn.m_SkillContext
	local ItemInfo = SkillPart_Common(tbl.SkillName)
	g_LoadIconFromRes(ItemInfo("SkillSmallIcon"), Btn, -1, IP_ENABLE, IP_CLICKDOWN)
	Btn:SetMouseOverDescAfter(g_Tooltips:GetWndSkillTips(tbl.SkillName,0))
end

--显示技能时间的Tick回调函数
function CTempSkill:WndNoShow(Tick, Btn)
	if not IsCppBound(Btn) and Btn.IsShowTick then
		UnRegisterTick(Btn.IsShowTick)
	end
	
	local number = Btn:GetWndText()
	if number == "" then
		number = 1
	end
	number = tonumber(number) - 1
	Btn:SetWndText(number)
	if number <= 0 then
		self:DeleteTempSkill(Btn.Pos)
	end
end 

--显示CD的tick回调函数
function CTempSkill:WndShowCd(Tick, Btn)
	local totalTime = Btn.leftCdTime
	totalTime = totalTime - 200
	local displayCdNum = math.ceil(totalTime/1000)
	Btn.leftCdTime = totalTime 	
	Btn:SetWndText(displayCdNum)
	if displayCdNum <= 0 then
		UnRegisterTick(Btn.CdAnimationTick)
		Btn.CdAnimationTick = nil
		Btn:SetWndText("")
	end	
end 

--删除图标
function CTempSkill:DeleteTempSkill(pos)
	local Btn = self.m_BtnTBl[pos]
	if Btn.IsShowTick then
		UnRegisterTick(Btn.IsShowTick)
	end
	if Btn.CdAnimationTick then
		UnRegisterTick(Btn.CdAnimationTick)
	end
	Btn.IsShowTick = nil
	Btn.CdAnimationTick = nil
	self.m_SkillDuraTimeTbl[pos]:SetWndText("")
	Btn:SetWndText("")
	self.m_CdAnimationTbl[pos]:ShowWnd(false)
	Btn.m_State = EShortcutPieceState.eSPS_None
	g_DelWndImage(Btn, 1, IP_ENABLE, IP_CLICKDOWN)
	self:ParentIsShow()
	Btn.m_SkillContext = {}
	Btn.leftCdTime = nil	
end

--判断是否有Btn显示
function CTempSkill:ParentIsShow()
	if self.m_BtnTBl[1].m_State == EShortcutPieceState.eSPS_None
		and self.m_BtnTBl[2].m_State == EShortcutPieceState.eSPS_None
		and self.m_BtnTBl[3].m_State == EShortcutPieceState.eSPS_None
		and self.m_BtnTBl[4].m_State == EShortcutPieceState.eSPS_None
		and self.m_BtnTBl[5].m_State == EShortcutPieceState.eSPS_None
		and self.m_BtnTBl[6].m_State == EShortcutPieceState.eSPS_None then
		self:ShowWnd(false)
	end
end

--注销tick
function CTempSkill:UnRegisterTempSkill()
	for i = 1, #(self.m_BtnTBl) do 
		if self.m_BtnTBl[i].IsShowTick ~= nil then
			UnRegisterTick(self.m_BtnTBl[i].IsShowTick)
			self.m_BtnTBl[i].IsShowTick = nil
		end
		if self.m_BtnTBl[i].CdAnimationTick then
			UnRegisterTick(self.m_BtnTBl[i].CdAnimationTick)
			self.m_BtnTBl[i].CdAnimationTick = nil
		end
	end
end

function Gas2Gac:CreateTempSkill(Conn,SkillName,uSkillLevel,uTime,uID)
	g_GameMain.m_TempSkill:AddTempSkill(SkillName,uSkillLevel,uTime,uID)
end

function Gas2Gac:RemoveTempSkill(Conn, uID)
	g_GameMain.m_TempSkill:DeleteTempSkill(uID)
end 

function Gas2Gac:OnCastTempSkill(Conn, uID)
end 

--按下小键盘1-6使用临时技能
function UseTempSkillByKeyBoard(index)
	local Btn = g_GameMain.m_TempSkill.m_BtnTBl[index]
	local tbl = Btn.m_SkillContext
	if not tbl then
		return
	end
	if Btn.m_State == EShortcutPieceState.eSPS_Skill then
		if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= "" then
			local SkillName = g_MainPlayer.m_ActionSkillName
			if SkillName and GroundSelector:IsActive() then
				GroundSelector:OnLButtonUp()
				return
			else
				if tbl.SkillName == SkillName then
					Gac2Gas:ActiveBehavior(g_Conn)
					g_MainPlayer:UseTempSkill(tbl.SkillName,tbl.SkillLevel,index)
				else
					return
				end
			end
		else
			g_MainPlayer:UseTempSkill(tbl.SkillName,tbl.SkillLevel,index)
		end
	end
end

--显示快捷键
function CTempSkill:SetKey(pos,index)
	self.m_ShortCutsIndexTbl[pos]:SetWndText(index)
end

--设置技能CD功能函数
function CTempSkill:SetCdFun(wnd,time,lefttime)
	if wnd.CdAnimationTick then
		UnRegisterTick(wnd.CdAnimationTick)
		wnd.CdAnimationTick = nil
	end
	local pos = wnd.Pos
	self.m_CdAnimationTbl[pos]:ShowWnd(true)
	local runedtime
	if not lefttime or lefttime == 0 then
		lefttime = time
		runedtime = 0
	else
		runedtime = time - lefttime
	end
	self.m_CdAnimationTbl[pos]:setTimeEx(time,GetRenderTime(g_MainPlayer))
	self.m_CdAnimationTbl[pos]:setRunedTime(runedtime)
	wnd:SetWndText(math.ceil(lefttime/1000) == 0 and "" or math.ceil(lefttime/1000))
	wnd.leftCdTime = lefttime	
	wnd.CdAnimationTick = RegClassTick("CdAnimationTick", self.WndShowCd,200,self,wnd)
end

--查找所释放技能在快捷栏上的位置
function CTempSkill:GetSkillPosition(skillname)
	local location = {}
	local k = 1
	for i = 1,#self.m_BtnTBl do
		if  self.m_BtnTBl[i].m_SkillContext and skillname == self.m_BtnTBl[i].m_SkillContext.SkillName then
			 location[k] = self.m_BtnTBl[i]
			 k = k + 1
		end
	end
	return location
end

--接口函数设置技能CD
function CTempSkill:SetSkillCdDisplay(skillname,time)
	local targetWndTbl = self:GetSkillPosition(skillname)
	for i = 1,#targetWndTbl do 
		self:SetCdFun(targetWndTbl[i],time)
	end
end
