--gac_require "relation/cofc/CofCStockMain"

local function OpenGuideWnd()
	g_GameMain.m_GuideWnd:OpenPanel( not g_GameMain.m_GuideWnd:IsShow() )
end

function InitShowWndFun()
	--初始化回调函数  (这里所有的东西 都和CNpcDialogBox 中的消息对应)
	linkfun = {}
	linkfun[1] = WhiteStoneWnd				--白宝石鉴定      		$openpanel(1)
	linkfun[3] = NPCShopSellWnd				--NPC购买商店			$openpanel(3)
	linkfun[4] = NPCShopSoldWnd				--NPC购回商店
	linkfun[5] = EmailWnd					--邮件箱主界面
	linkfun[6] = ReceiveWnd					--收件箱窗口
	linkfun[7] = EquipUpWnd					--装备升级强化 
	linkfun[9] = EquipToSoulWnd  			--装备炼化魂
	linkfun[10]= AgiotageWnd				--货币兑换
	linkfun[11]= ConsignmentWnd         	--寄售交易所窗口
	--linkfun[13]= CofCNpcDialogWnd			--商会npc对话框
	--linkfun[14]= OpenCofCWnd				--打开商会主界面
	--linkfun[15]= OpenCofCStockWnd			--打开商会股票主界面
	linkfun[16]= OpenContractManufactureWnd --打开代工所面板
	linkfun[17] = OpenSuperaddWnd
	linkfun[18] = OpenItemBagLockWnd --打开背包锁面板
	linkfun[19] = OpenGuideWnd			--攻略面板
	linkfun[20] = OpenBuyCouponsWnd         --打开点券购买商店
end

function WhiteStoneWnd()
	local Whitestone = CWhiteStone.GetWnd()
	Whitestone:ShowWnd(not Whitestone:IsShow())
end

function NPCShopSellWnd()
	SetEvent( Event.Test.OpenNpcSellWnd, true, true)
	if (g_GameMain.m_NPCShopSell:IsShow() == false) then
		g_GameMain.m_NPCShopSell:ShowWnd(true)
		g_GameMain.m_WndMainBag:ShowWnd(true)
		if g_GameMain.m_NPCShopSell.m_buyBatchWnd ~= nil and 
			g_GameMain.m_NPCShopSell.m_buyBatchWnd:IsShow() then
			g_GameMain.m_NPCShopSell.m_buyBatchWnd:ShowWnd(false)
		end
		g_GameMain.m_NPCShopSell:InitNPCShop("儿童") --NpcShop_Common[npcName].ShopName
		g_GameMain.m_NPCShopSell.npcName = "儿童"
		g_GameMain.m_WndMainBag.m_ctItemRoom:UpdateBagTips(false, nil)
	else
		g_GameMain.m_NPCShopSell:ShowWnd(false)
	end
end

function NPCShopSoldWnd()
	if g_GameMain.m_NPCShopPlayerSold:IsShow() == false then
		g_GameMain.m_NPCShopPlayerSold:ShowWnd( true )
		g_GameMain.m_NPCShopPlayerSold:ShowWnd(true)
		g_GameMain.m_WndMainBag:ShowWnd(true)
		g_GameMain.m_NPCShopSell:InitNPCShop("儿童") --NpcShop_Common[npcName].ShopName
		g_GameMain.m_NPCShopSell.npcName = "儿童"
	else
		g_GameMain.m_NPCShopPlayerSold:ShowWnd( false )
	end
end

function EmailWnd()
	SetEvent( Event.Test.OpenEmailWnd, true, true) 
	if(g_GameMain.m_EmailBox:IsShow() == false) then
		g_GameMain.m_EmailBox.m_MailInfo = {}
		g_GameMain.m_EmailBox:EnableBtnFalse()
		g_GameMain.m_EmailBox:SetFocus()
		g_GameMain.m_EmailBox.m_SysItemCount = -1
		g_GameMain.m_EmailBox.m_CommItemCount = -1
		g_GameMain.m_EmailBox.EmailWndTbl = {}
		g_GameMain.m_EmailBox.NowChooseEmailTbl = {}
		g_GameMain.m_EmailBox.NowEmail = {}
		g_GameMain.m_EmailBox:GetMailsInfoFromGas()
		g_GameMain.m_EmailBox:ShowWnd(true)
	else
		g_ExcludeWndMgr:CloseAllActiveWndExclude()
	end
	local AutoCloseWnd = CAutoCloseWnd:new()
	AutoCloseWnd:AutoCloseWnd(g_GameMain.m_EmailBox)
end

function ReceiveWnd()
	g_GameMain.m_ReceiveBox:ShowWnd(not g_GameMain.m_ReceiveBox:IsShow())
end

function EquipUpWnd()
	if g_GameMain.m_EquipUpIntensifyWnd:IsShow() == false then
		g_GameMain.m_EquipUpIntensifyWnd:ShowWnd(true)
	else
		g_GameMain.m_EquipUpIntensifyWnd:ShowWnd(false)
	end
end

function EquipToSoulWnd()
	if g_GameMain.m_EquipToSoul:IsShow() == false then
		g_GameMain.m_EquipToSoul:ShowWnd(true)
	else
		g_GameMain.m_EquipToSoul:ShowWnd(false)
	end
	g_GameMain.m_EquipToSoul:ClearEquipBtn()
end

function AgiotageWnd()
	g_GameMain.m_Agiotage:ShowWnd(not g_GameMain.m_Agiotage:IsShow())
end


--收到服务器发来的打开或关闭某个窗口的消息
function Gas2Gac:ReturnOpenPanel(Connetion, quickKey)
	local function Callback(g_GameMain,uButton)
		g_GameMain.m_MsgBox = nil
		return true
	end
	if linkfun[quickKey] == nil then
		g_GameMain.m_MsgBox = MessageBox(g_GameMain, MsgBoxMsg(402), MB_BtnOK,Callback,g_GameMain)
		return
	end
	linkfun[quickKey]()
end

--寄售交易所窗口
function ConsignmentWnd()
	if g_GameMain.m_CSMSellOrderWnd:IsShow() == false then
		g_GameMain.m_CSMSellOrderWnd:ShowWnd(true)
	else
		g_GameMain.m_CSMSellOrderWnd:ShowWnd(false)
	end
	g_GameMain.m_CSMSellOrderWnd:SendSearchInfo()
end	

function CofCNpcDialogWnd()
	Gac2Gas:OpenCofCNpcDialog(g_Conn,0)
end

function OpenCofCWnd()
	if g_GameMain.m_CofCMainWnd == nil or g_GameMain.m_CofCMainWnd:IsShow()  == false then
		if g_GameMain.m_CofCMainWnd == nil then
			g_GameMain.m_CofCMainWnd = CreateChamberOfCommerce(g_GameMain)
			g_GameMain.m_CofCMainWnd:ShowWnd(true)
		elseif g_GameMain.m_CofCMainWnd:IsShow() == false then
			g_GameMain.m_CofCMainWnd:ShowWnd(true)
		end
		local AutoCloseWnd = CAutoCloseWnd:new()
		AutoCloseWnd:AutoCloseWnd(g_GameMain.m_CofCMainWnd)
		
		local cofc_id	=	g_MainPlayer.m_Properties:GetCofcID()
		Gac2Gas:GetCofcInfo(g_Conn, cofc_id)
	end
end

function OpenCofCStockWnd()
	if g_GameMain.m_CofCMainStockWnd == nil then
		g_GameMain.m_CofCMainStockWnd = CreateStockMainWnd(g_GameMain)
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
	elseif g_GameMain.m_CofCMainStockWnd:IsShow() == false then
		g_GameMain.m_CofCMainStockWnd:ShowWnd(true)
	end
	Gac2Gas:GetCofCStockInfo(g_Conn)
end

function OpenContractManufactureWnd()
    CContractManufactureMarketWnd.OpenContractManufMarketWnd()
end

function OpenSuperaddWnd()
    CEquipSuperaddWnd.OpenSuperaddWnd()
end

function OpenItemBagLockWnd()
	if g_MainPlayer.m_ItemBagLock then
		g_GameMain.m_ItemBagTimeLockWnd:ShowWnd(true)
	else
		Gac2Gas:OpenItemBagLockWnd(g_Conn)
	end
end

function OpenBuyCouponsWnd()
    CBuyCouponsWnd.OpenBuyCouponsWnd()
end