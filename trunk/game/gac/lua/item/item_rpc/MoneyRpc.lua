cfg_load "item/MoneySound_Common"
CMoneyRpc = class()

function CMoneyRpc.SetMoneyTypeBtnCheck(Conn, moneyType)
	g_MainPlayer:SetMoneyType(moneyType)
	if moneyType == 1 then  --1为流通的金币，2为绑定的金币
		g_GameMain.m_WndMainBag.m_ChkMoney:SetCheck(true)
		g_GameMain.m_WndMainBag.m_ChkBindingMoney:SetCheck(false)
	elseif moneyType == 2 then
		g_GameMain.m_WndMainBag.m_ChkMoney:SetCheck(false)
		g_GameMain.m_WndMainBag.m_ChkBindingMoney:SetCheck(true)
	end
end

function CMoneyRpc.ReturnAddMoney( Connection,nMoney)
	local SoundName = MoneySound_Common("Money","SoundName")
	PlayCue(SoundName)
	if g_MainPlayer then
		g_MainPlayer:AddMoney(nMoney)
		if g_GameMain.m_WndMainBag ~= nil and g_GameMain.m_WndMainBag:IsCreated() then
			g_GameMain.m_WndMainBag.m_WndGold:SetWndText(tostring(g_MainPlayer:GetGold())) 
			g_GameMain.m_WndMainBag.m_WndArgent:SetWndText(tostring(g_MainPlayer:GetArgent())) 
			g_GameMain.m_WndMainBag.m_WndCopper:SetWndText(tostring(g_MainPlayer:GetCopper())) 
		end
		g_GameMain.m_NPCShopSell:SetCurMoneyStr()
		g_GameMain.m_CSMSellOrderWnd:SetCurMoneyStr()
		if g_GameMain.m_PurchasingAgentMainWnd then
			g_GameMain.m_PurchasingAgentMainWnd:SetCurMoneyStr()
		end
	    g_GameMain.m_ToolsMallWnd:UpdateMoneyCount()
		g_GameMain.m_GMConsole:SetResult("all money: " .. nMoney .. " copper" )
		SetEvent( Event.Test.AddMoneyGM,true, "AddMoneySuccess" )
	end
end

function CMoneyRpc.ReturnAddMoneyError( Connection,nMoney)
	--print("In the ReturnAddMoneyError=",nMoney)
	g_GameMain.m_GMConsole:SetResult("Add money: " .. nMoney .. " copper failed!" )
	SetEvent( Event.Test.AddMoneyGM,true, "AddMoneyError" )
end

function CMoneyRpc.ReturnAddTicket( Connection,nTicket)
	local SoundName = MoneySound_Common("Ticket","SoundName")
	PlayCue(SoundName)
	--print("In the ReturnAddTicket=",nTicket)
	g_MainPlayer:AddTicket(nTicket)
	
--	if g_GameMain.m_WndMainBag ~= nil and g_GameMain.m_WndMainBag:IsCreated() then
--		g_GameMain.m_WndMainBag.m_wndTicket:SetWndText(tostring(g_MainPlayer:GetTicket()))
--	end
	g_GameMain.m_GMConsole:SetResult("添加佣兵币：" .. tostring(nTicket))
	SetEvent( Event.Test.AddTicketGM,true, "AddTicketSuccess" )
end	

function CMoneyRpc.ReturnAddTicketError( Connection,nTicket)
	--print("In the ReturnAddTicketError=",nTicket)
	g_GameMain.m_GMConsole:SetResult("添加佣兵币：" .. tostring(nTicket) .. "失败！" )
	SetEvent( Event.Test.AddTicketGM,true, "AddTicketError" )
end


function CMoneyRpc.ReturnAddBindingMoney( Connection,nMoney)
	local SoundName = MoneySound_Common("BindingMoney","SoundName")
	PlayCue(SoundName)
	g_MainPlayer:AddBindMoney(nMoney)
	
	if g_GameMain.m_WndMainBag ~= nil and g_GameMain.m_WndMainBag:IsCreated() then
		g_GameMain.m_WndMainBag.m_WndBindingGold:SetWndText(tostring(g_MainPlayer:GetBindingGold())) 
		g_GameMain.m_WndMainBag.m_WndBindingArgent:SetWndText(tostring(g_MainPlayer:GetBindingArgent())) 
		g_GameMain.m_WndMainBag.m_WndBindingCopper:SetWndText(tostring(g_MainPlayer:GetBindingCopper())) 
	end
    g_GameMain.m_ToolsMallWnd:UpdateBindingMoneyCount()
	g_GameMain.m_NPCShopSell:SetCurMoneyStr()
	g_GameMain.m_GMConsole:SetResult("Add binding money: " .. nMoney .. " copper" )
	SetEvent( Event.Test.AddBindingMoneyGM,true, "AddBindingMoneySuccess" )
end

function CMoneyRpc.ReturnAddBindingMoneyError( Connection,nMoney)
	--print("In the ReturnAddMoneyError=",nMoney)
	g_GameMain.m_GMConsole:SetResult("Add binding money: " .. nMoney .. " copper failed!" )
	SetEvent( Event.Test.AddBindingMoneyGM,true, "AddBindingMoneyError" )
end

function CMoneyRpc.ReturnAddBindingTicket( Connection,nTicket)
	local SoundName = MoneySound_Common("BindingTicket","SoundName")
	PlayCue(SoundName)
	--print("In the ReturnAddTicket=",nTicket)
	g_MainPlayer:AddBindTicket(nTicket)
	g_GameMain.m_GMConsole:SetResult("添加绑定的佣兵币：" .. tostring(nTicket))
	SetEvent( Event.Test.AddBindingTicketGM,true, "AddBindingTicketSuccess" )
end	

function CMoneyRpc.ReturnAddBindingTicketError( Connection,nTicket)
	--print("In the ReturnAddTicketError=",nTicket)
	g_GameMain.m_GMConsole:SetResult("添加绑定的佣兵币：" .. tostring(nTicket) .. "失败！" )
	SetEvent( Event.Test.AddBindingTicketGM,true, "AddBindingTicketError" )
end
