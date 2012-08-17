gac_require "fight/buff_wnd/BaseBuff"

CPlayerBuffWnd 					= class ( CBuffWnd )
CPlayerBuffItem		 			= class ( CBuffItem )
------- 创建 CPlayerBuffWnd
function CPlayerBuffWnd:Ctor( Parent )
	self:Init( Parent, g_GameMain.m_PlayerInfo.m_ProgessOne )
end

function CPlayerBuffWnd.GetWnd()
	local Wnd = g_GameMain.m_Buff
	if not Wnd then
		Wnd = CPlayerBuffWnd:new(g_GameMain)
		g_GameMain.m_Buff = Wnd
	end
	return Wnd
end

function CPlayerBuffWnd:GetRemainTime(ID, buff_name)
	local RemainTime = g_MainPlayer:GetRemainTime(buff_name, ID)
	return RemainTime or 0
end

function CPlayerBuffWnd:GetBuffItem()
	return CPlayerBuffItem:new()
end

function CPlayerBuffItem:WndProc(uMsgID, uParam1, uParam2)
	if( uMsgID == WM_RBUTTONUP )then
		local buff_name = nil
		if(g_GameMain.m_Buff.m_tblBuffs[1][self.ID]) then
			buff_name = g_GameMain.m_Buff.m_tblBuffs[1][self.ID].realName
			if( buff_name and g_MainPlayer:IsCanCancelState(buff_name) ) then
				g_MainPlayer:CancelState(self.ID)
			end
		end
	end
end

------- 更新Buff Debuff  参数：Buff或Debuff的ID，名称，层数，时间, buff剩余时间
function CPlayerBuffWnd:UpdatePlayerBuffDebuff(ID, description, DivNum, Time, RemainTime, BuffType, SmallIcon, sName, uEntityID, InstallerID)
	--BuffType : 是否是负面效果类型, true表示是负面效果DeBuff,false表示是Buff
--	print("buff  ID = "..ID.."  sName ="..sName.."  DivNum ="..DivNum.."  Time ="..Time.."  RemainTime ="..RemainTime)
	g_GameMain.m_CRideCamelWnd:SendToGas(sName,RemainTime)
	if sName == "600秒buff" then
		g_GameMain.m_DownTimeWnd:ShowWindow(RemainTime)
	end
	if sName == "疯狂杀戮状态" then
		if Time == 0 then
			CComboHitsWnd.HideComboHitsWnd()
			g_GameMain.ComboHitsInfo.m_IsCrazyState = false
		end
	end
	self:UpdateBuffDebuff(ID, description, DivNum, Time, RemainTime, BuffType, SmallIcon, sName, uEntityID, InstallerID, true)
end


