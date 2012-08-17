lan_load "npc/Lan_Npc_Common"

function OnRClickNpc(Target)
	local Camp = Target:CppGetCamp()
	local GameCamp = Target:CppGetGameCamp()
	local Name = Target.m_Properties:GetCharName()
	if IsPasserbyCamp(Camp, g_MainPlayer:CppGetCamp(), GameCamp, g_MainPlayer:CppGetGameCamp()) 
		and (Target:CanBeTakeOver() or CQuest.IsObjectContainFun(Target) or GetNpcTriggerArg(Name, "点击")) then 
		Gac2Gas:ClickNpc(g_Conn, Target:GetEntityID())
		return true
	end
	return false
end

function OnRClickDeadNpc(Target)
	local NpcName = Target.m_Properties:GetCharName()
	if g_MainPlayer:IsMoving()  then
		return
	end
	targetID = Target:GetEntityID()
	Gac2Gas:ShowNpcDropItemWnd(g_Conn,targetID)
end

local function SetCanClickDeadNpc(Tick, bCanClick,Npc)
	Npc:SetCanBeSelected(bCanClick)
	if bCanClick then
		if Npc.m_CanClickFxId == nil then
			Npc.m_CanClickFxId = Npc:GetRenderObject():AddEffect( "fx/setting/other/other/shiqu/create.efx", "shiqu/create", -1, nil)
		end
	else
		if Npc.m_CanClickFxId then
			Npc:GetRenderObject():DelEffect( Npc.m_CanClickFxId )
			Npc.m_CanClickFxId = nil
		end
		if g_MainPlayer.m_LockCenter.m_LockingObj == Npc then
			g_MainPlayer:UnLockObj(Npc:CppIsAlive())
		end
	end
	if Npc.m_CanClickDeadNpcTick then
		UnRegisterTick(Npc.m_CanClickDeadNpcTick)
		Npc.m_CanClickDeadNpcTick = nil
	end
end

function CheckCanClickDeadNpc(Npc)
	local OwnerId = Npc.m_Properties:GetDropOwnerId()
	local DropTime = Npc.m_Properties:GetDropTime()
	local BelongToTeam = Npc.m_Properties:GetBelongToTeam()
	if not g_MainPlayer  or not IsCppBound(g_MainPlayer) then
		return
	end
	local TeamID = g_MainPlayer.m_Properties:GetTeamID()
	if OwnerId == 0 then
		SetCanClickDeadNpc(nil,false,Npc)
		return
	end
	if OwnerId == g_MainPlayer.m_uID then
		SetCanClickDeadNpc(nil,true,Npc)
		return
	end
	if BelongToTeam ~= 0 and TeamID == BelongToTeam then
		SetCanClickDeadNpc(nil,true,Npc)
		return
	end
	if os.time() - DropTime > DropItemProtectTime then
		SetCanClickDeadNpc(nil,true,Npc)
		return
	end
	SetCanClickDeadNpc(nil,false,Npc)
end

function Gas2Gac:NotifyNpcDropItem(Conn,NpcId,IsDrop)
	local Npc = CCharacterFollower_GetCharacterByID(NpcId)
	if Npc == nil then
		return
	end
	if not IsDrop then
		SetCanClickDeadNpc(nil,false,Npc)
		return
	end
	local OwnerId = Npc.m_Properties:GetDropOwnerId()
	local DropTime = Npc.m_Properties:GetDropTime()
	local BelongToTeam = Npc.m_Properties:GetBelongToTeam()
	local TeamID = g_MainPlayer.m_Properties:GetTeamID()
	if OwnerId == 0 then
		Npc.m_CanClickFxId = Npc:GetRenderObject():AddEffect( "fx/setting/other/other/shiqu/create.efx", "shiqu/create", -1, nil)
		return
	end
	if OwnerId == g_MainPlayer.m_uID or (BelongToTeam ~= 0 and TeamID == BelongToTeam) then
		Npc.m_CanClickFxId = Npc:GetRenderObject():AddEffect( "fx/setting/other/other/shiqu/create.efx", "shiqu/create", -1 ,nil)
		return
	else
		Npc:SetCanBeSelected(false)
		Npc.m_CanClickDeadNpcTick = RegisterTick("CanClickDeadNpcTick",SetCanClickDeadNpc,40000,true,Npc)
	end
--	print("OwnerId",OwnerId)
end

----军械兽接收面板
function Gas2Gac:NotifyWhetherTakeOverNpc(Conn, EntityID)
	local function IsTakeOverNpc(g_GameMain, uButton)
		if uButton == MB_BtnOK then
			Gac2Gas:AgreeTakeOverNpc(g_Conn, g_MainPlayer:GetEntityID(), EntityID)
		end
		g_GameMain.m_MsgBox = nil
		return true
	end
	g_GameMain.m_MsgBox = MessageBox( g_GameMain, MsgBoxMsg(2003), BitOr( MB_BtnOK, MB_BtnCancel), IsTakeOverNpc, g_GameMain)
end

function GetNpcDisplayName(name)
	--db()
	if name then
		local strname = ""
		if Npc_Common(name) then--传进来的是NPC的真实名
			strname = name
		else--传进来的是NPC是Title名,要先得到真实名
			for _, RealName in pairs(Npc_Common:GetKeys()) do 
			--for RealName, tbl in pairs(Npc_Common) do
				if Npc_Common(RealName,"Title") == name then
					strname = RealName
					break
				end
			end
		end
		if( not Lan_Npc_Common(MemH64(strname)) ) then
			CfgLogErr(name .. "显示名不存在，请检查designer_Npc_Common表是否存在此NPC")
			return ""
		end
		--local aa = Lan_Npc_Common(MemH64(strname),"Title")
		
		return Lan_Npc_Common(MemH64(strname),"Title")
	end
end