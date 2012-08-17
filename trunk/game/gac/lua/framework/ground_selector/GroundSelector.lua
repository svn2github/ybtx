gac_require "framework/ground_selector/GroundSelectorInc"
gac_require "smallgames/HeadGames/HeadHunt"

--点选某些特殊的物品
function Gas2Gac:CreatItemSelector(Conn,nSize,sName,nRange)
	GroundSelector:CancelSelectGround()
	GroundSelector:BeginItemSelectGround( nSize,sName,nRange)
end

function CGroundSelector:OnTick()
	if( GroundSelector.m_Actived ) then
		if( not ( g_App and g_CoreScene and g_MainPlayer) ) then
			UnRegisterTick( GroundSelector.m_Ticker )
		else
			if( g_MainPlayer ~= nil ) then
				g_MainPlayer:UpdateSelectPos()
			end
			local pos = CVector3f:new()
			g_CoreScene:GetRenderScene():GetSelectedPos (pos)
			
			--以上算法跟服务器不一致，以下才是跟服务器一致的算法
			local playerPos = CFPos:new()
			g_MainPlayer:GetPixelPos(playerPos)
			
			local a = playerPos.x - GetPixelOfGrid(math.floor(pos.x / EUnits.eGridSpanForObj), EUnits.eGridSpanForObj / 2)
			local b = playerPos.y - GetPixelOfGrid(math.floor(pos.z / EUnits.eGridSpanForObj), EUnits.eGridSpanForObj / 2)
			local dis = math.sqrt(a * a + b * b) / EUnits.eGridSpanForObj
						
			local ratio = EUnits.eGridSpanForObj / EUnits.eGridSpan
			pos.x = pos.x * ratio
			pos.y = pos.y * ratio
			pos.z = pos.z * ratio
			GroundSelector.m_vecPos = pos
			if GroundSelector.m_Range == 0 or GroundSelector.m_Range >= dis then
				GroundSelector.m_IsUsed = true
				self:PlayFx(true);
			else
				GroundSelector.m_IsUsed = false
				self:PlayFx(false);
			end
			
		end
	end
end

function CGroundSelector:IsActive()
	return GroundSelector.m_Actived
end

function CGroundSelector:BeginSelectGround( nSize, nRange )
	GroundSelector.m_Size = nSize
	g_MainPlayer.m_UsingSkill = true
	
	if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= "" then
		nRange = 10
	end
	GroundSelector.m_Range = nRange
	self:DoState( 1 )
end

function CGroundSelector:BeginSelectMouseSelector()
	if GroundSelector.m_SkillName == "射箭" then
		BeginHeadHunt()
	end
	g_MainPlayer.m_UsingSkill 	= true
	GroundSelector.m_Actived	= true
	GroundSelector.m_Attack		= false
	GroundSelector.m_IsUsed = true
	--开始冲击波选择地点
	g_WndMouse:LockCursorToState(ECursorSkillState.eCSS_UseSkill)
end

function CGroundSelector:BeginSelectTargetSelector(nRange)
	g_MainPlayer.m_UsingSkill 	= true
	GroundSelector.m_Actived	= true
	GroundSelector.m_Attack		= false
	GroundSelector.m_IsUsed = true
	g_WndMouse:LockCursorToState(ECursorSkillState.eCSS_BreakItem)
	g_MainPlayer:SetDeadFriendCanBeSelected(true)
end

function CGroundSelector:BeginOrderServantMove()
	GroundSelector.m_Actived	= true
	GroundSelector.m_IsUsed = true
	GroundSelector.m_SkillName = "召唤兽移动"
	g_WndMouse:LockCursorToState(ECursorSkillState.eCSS_UseSkill)
end

--为了改变光圈的大小(蓄力时)
function CGroundSelector:SetGroundSelectorSize( nSize )
	GroundSelector.m_Size = nSize
end

function CGroundSelector:BeginItemSelectGround( nSize,sName,nRange)
	
	GroundSelector.m_Size = nSize
	g_MainPlayer.m_SpecialItemName = sName
	g_MainPlayer.m_Range = nRange
	self:DoState(1)
end

local function IsBarrier(vecPos)
	local CurrPos = CPos:new()
	CurrPos.x = vecPos.x / 64
	CurrPos.y = vecPos.z / 64

	if g_CoreScene:GetBarrier(CurrPos) ==  EBarrierType.eBT_NoBarrier 
		or g_CoreScene:GetBarrier(CurrPos) ==  EBarrierType.eBT_LowBarrier then
		local PlayerPos = CFPos:new()
		g_MainPlayer:GetPixelPos(PlayerPos)
		
		local PosTo = CFPos:new()
		PosTo.x = vecPos.x
		PosTo.y = vecPos.z
		local path = g_CoreScene:CreatePath(PlayerPos, PosTo, EFindPathType.eFPT_Line, EBarrierType.eBT_MidBarrier)
		if not path then
			MsgClient(1812)
			return true
		else
			local Dir = CDir:new()
			local PosDir = CVector2f:new()
			PosDir.x = PosTo.x - PlayerPos.x
			PosDir.y = PosTo.y - PlayerPos.y
			if (PosDir.x <= -0.001 or PosDir.x >= 0.001)
				or (PosDir.y <= -0.001 or PosDir.y >= 0.001) then
				Dir:Set(PosDir)
				g_MainPlayer.m_uDir = Dir.uDir;
				g_MainPlayer:TurnAndSyncDir(Dir.uDir)
			end
			return false
		end
	else
		return true
	end
end

function CGroundSelector:OnLButtonUp()
	--喝酒射箭
	--if g_GameMain.m_ShootProgressWnd.m_Tick then
	--	g_GameMain.m_ShootProgressWnd:DoShoot()
	--	return
	--end
	
	local SecectPos = CVector3f:new()
	g_CoreScene:GetRenderScene():GetSelectedPos (SecectPos)
	
	if self.m_SkillName == "射箭" and g_GameMain.HeadHuntTick then
		SecectPos = GetHeadHuntTargetPos()
		EndHeadHunt()
	end
	
	local ratio = EUnits.eGridSpanForObj / EUnits.eGridSpan
	local pos = CVector3f:new()
	pos.x = SecectPos.x * ratio
	pos.y = SecectPos.y * ratio
	pos.z = SecectPos.z * ratio
	self.m_vecPos = pos
	
	if GroundSelector.m_SkillName == "召唤兽移动" then
		Gac2Gas:MasterOrderServantMoveCommand(g_Conn, g_MainPlayer:GetEntityID(), ENpcType.ENpcType_Summon, self.m_vecPos.x, self.m_vecPos.z)
		GroundSelector:CancelSelectGround()
		return
	end
	
	--如果没有在有效的范围不释放技能
	if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= "" then
		if not GroundSelector.m_IsUsed then
			return
		end
		--如果释放点有障碍，不让释放
		if IsBarrier(pos) then
			return
		end
		local SkillName = g_MainPlayer.m_ActionSkillName
		if SkillName then
			local PixelPos = CFPos:new()
			PixelPos.x = self.m_vecPos.x
			PixelPos.y = self.m_vecPos.z
			g_MainPlayer:SetTargetPos(PixelPos)
			Gac2Gas:ActiveBehaviorDoSkill(g_Conn, 0 ,SkillName,self.m_vecPos.x,self.m_vecPos.z)--改在服务端调用技能
		end
		return
	end

	if g_MainPlayer.m_UsingSkill then
		GroundSelector.m_Attack = true
		g_MainPlayer.m_UsingSkill = nil
	
		local posSelf = CFPos:new()
		g_MainPlayer:GetPixelPos(posSelf)
		local posDis = CVector2f:new()
		posDis.x = pos.x - posSelf.x
		posDis.y = pos.z - posSelf.y
		local dirDis = CDir:new()
		dirDis:Set(posDis)
		GroundSelector.m_Direction = dirDis.uDir
	end
	self:DoEffect()
	self:DoState( 0 )
end

function CGroundSelector:CancelSelectGround()
	g_MainPlayer.m_UsingSkill = nil
	g_WndMouse:ReleaseCursorLockAndUpate( ECursorSkillState.eCSS_Normal )

	if self:IsActive() then
		if g_MainPlayer.m_Properties:GetHandsBehaviorName() ~= "" then
			if g_GameMain.m_HoldThingType == "抱" then
				g_MainPlayer:StopMoving()
				Gac2Gas:ActiveBehavior(g_Conn)
			else
				Gac2Gas:QuitFitoutState(g_Conn)
			end
		end
	end
	self:DoState( 0 )
	if g_MainPlayer.m_SpecialItemName then
		Gac2Gas:UseItemEnd(g_Conn, g_MainPlayer.m_SpecialItemName)
		g_MainPlayer.m_SpecialItemName = nil
	end
	self.m_SkillName = ""
	if g_GameMain.HeadHuntTick then
		EndHeadHunt()
	end
end

function CGroundSelector:DoState( TargetState )
	if( TargetState == 1 ) then
		if GroundSelector.m_Ticker == nil and (GroundSelector.m_SelectorType == ESelectGroundType.eSGT_GroundSelector or g_MainPlayer.m_SpecialItemName) then
			GroundSelector.m_Ticker	= RegClassTick( "CGroundSelector", CGroundSelector.OnTick, 30, self )
			self:PlayFx(true);
			GroundSelector.m_Actived	= true
			GroundSelector.m_Attack	= false
			g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_SelectGround, nil )
		end
		GroundSelector.m_Actived	= true
	elseif( TargetState == 0 ) then
		GroundSelector.m_Actived = false
		if GroundSelector.m_SelectorType == ESelectGroundType.eSGT_GroundSelector or g_MainPlayer.m_SpecialItemName  then
			UnRegisterTick( GroundSelector.m_Ticker )
			GroundSelector.m_Ticker = nil
			g_CoreScene:DestroyObjectPerformer(GroundSelector.m_MouseFx)
			GroundSelector.m_MouseFx = nil
		else
			g_WndMouse:SetCursorSkillState( ECursorSkillState.eCSS_Normal, nil )
			g_WndMouse:UpdateCursor()
		end
		if ESelectGroundType.eSGT_TargetSelector == GroundSelector.m_SelectorType then
			g_MainPlayer:SetDeadFriendCanBeSelected(false)
		end
		GroundSelector.m_SelectorType = nil
	end
end

function CGroundSelector:DoEffect()
	if self.m_SelectorType == ESelectGroundType.eSGT_TargetSelector then
		local Target = CEntityClient_GetSelectedEntity(g_CoreScene)
		if Target ~= nil and g_MainPlayer then
			g_MainPlayer:OnSelectObj(Target, false)
		end
	end
	if( GroundSelector.m_Attack ) then
		g_MainPlayer:DoAttack( self.m_SkillName, self.m_SkillLevel, self.m_Pos, self.m_Direction, self.m_uMagicDir)
	end
	
	g_WndMouse:ReleaseCursorLockAndUpate( ECursorSkillState.eCSS_Normal )
	if g_MainPlayer.m_SpecialItemName ~= nil then
		local pos = GroundSelector.m_vecPos
		local targetPos = CFPos:new()
		targetPos.x = pos.x
		targetPos.y = pos.z
		g_MainPlayer:SetTargetPos(targetPos)
		local name = g_MainPlayer.m_SpecialItemName	
		Gac2Gas:SelectArea(g_Conn,pos.x,pos.y,pos.z,name)
	end
end

function CGroundSelector:SetSkill( SkillName,SkillLevel, Pos, SelectorType)
	self.m_SkillName = SkillName
	self.m_SkillLevel=SkillLevel
	self.m_Pos=Pos
	self.m_SelectorType = SelectorType
	self.m_Direction = 0	
	self.m_uMagicDir = 0 --地点类魔法施放的八个方向,从0-7
end

function CGroundSelector:ChagneDirection()
	if GroundSelector.m_MouseFx then 
		GroundSelector.m_uMagicDir = (GroundSelector.m_uMagicDir + 1) % 8
		local RenderObj =GroundSelector.m_MouseFx:GetRenderObject()
		local Dir = CDir:new()
		Dir.uDir = GroundSelector.m_uMagicDir * 32
		RenderObj:SetDirection(Dir)
		--print("Cur dir = ", GroundSelector.m_uMagicDir ,Dir.uDir )
	end
end

function CGroundSelector:PlayFx(InRange)
	local SizeString ,SelectString
	if( GroundSelector.m_Size >= 1 ) and ( GroundSelector.m_Size <= 11 )then
		SelectString = GroundSelector.m_Size.."x"..GroundSelector.m_Size
	elseif( GroundSelector.m_Size == 101 ) then
		SelectString = "zhangaifanwei01"
	end
	if InRange then
		SizeString = "selectground/"..SelectString
	else
		SizeString = "selectground_red/"..SelectString
	end
	
	local PlayerPos = CVector3f:new()
	g_CoreScene:GetRenderScene():GetSelectedPos (PlayerPos)
	local pos = CFPos:new()
	pos.x=PlayerPos.x
	pos.y=PlayerPos.z
	
	if GroundSelector.m_MouseFx == nil then 
		GroundSelector.m_MouseFx  =  g_CoreScene:CreateObjectPerformer(pos, nil)
	end
	GroundSelector.m_MouseFx:SetPixelPos(pos)
	GroundSelector.m_MouseFx:SetLieOnTheTerrain(false)
	local RenderObj =GroundSelector.m_MouseFx:GetRenderObject()
	GroundSelector.m_MouseFx:SetHeight(PlayerPos.y)
	if(GroundSelector.m_MouseFx.SizeString ~= SizeString) then
		RenderObj:RemovePiece(nil)
		RenderObj:AddEffect( "fx/setting/selectground.efx", SizeString, -1, NULL )
		GroundSelector.m_MouseFx.SizeString  = SizeString
	end
end

function Gas2Gac:ChangeSceneCancelSelectGround(Conn)
	if GroundSelector:IsActive() then
		GroundSelector:CancelSelectGround()
	end
end
