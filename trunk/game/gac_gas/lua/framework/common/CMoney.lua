gac_gas_require "framework/common/CMoneyInc"


--加钱的地方都必须在此表填写相应功能
--["FunName"]和["Description"] 是每个功能都必须填的
--索引是代表哪个系统，["FunName"]也是代表哪个系统
--其他的参数代表哪个系统的哪个操作
--["Description"]项必须是中文描述，用来给erating审计用的（此项只有扣钱的情况用到，只增加钱的功能可以不加描述项）
function CMoney:Ctor()
	self.m_tblMoneyFun =
		{	
			["NpcShop"] 		= {["FunName"]= "NpcShop",["Description"]= "Npc普通商店",["Sell"] = "Sell",["Buy"] = "Buy",["BuyBack"] = "BuyBack"},
			["CSM"] 				= {["FunName"]= "CSM",["Description"]= "拍卖行管理费和税收",["AddSellOrder"]= "AddSellOrder",["AddBuyOrder"]= "AddBuyOrder",["CancelBuyOrder"]= "CancelBuyOrder",["BuyOrder"]= "BuyOrder"},
			["Quest"] 			= {["FunName"]= "Quest",["Description"]= "喝酒消费",["MoneyAward"]= "MoneyAward",["Drink"] = "Drink"},
			["GMcmd"] 			= {["FunName"]= "GMcmd",["Description"]= "gm指令",["AddMoney"] = "AddMoney",["AddBindingTicket"] = "AddBindingTicket",["AddBindingMoney"] = "AddBindingMoney",
										["AddTicket"] = "AddTicket"},
			["GmTools"] 		= {["FunName"]= "GmTools",["Description"]= "gm工具",["AddMoney"] = "AddMoney",["AddTicket"] = "AddTicket"},
			["Depot"] 			= {["FunName"]= "Depot",["Description"]= "仓库存取钱",["SaveMoney"] = "SaveMoney",["GetMoney"] = "GetMoney",},
			["Mail"] 				= {["FunName"]= "Mail",["Description"]= "邮件管理费和税收",["GetMoney"] = "GetMoney",["SendMail"] = "SendMail"},
			["Tong"] 				= {["FunName"]= "Tong",["Description"]= "佣兵小队捐钱",["Contribut"] = "Contribut",["RegistTong"] = "RegistTong"},
			["ArmyCorps"] 	= {["FunName"]= "ArmyCorps",["Description"]= "佣兵团创建消耗",["RegistArmyCorps"] = "RegistArmyCorps"},
			["fuben"] 			= {["FunName"]= "fuben",["Description"]= "大夺宝副本",["Daduobao"] = "Daduobao"},
			["PlayerTran"] 	= {["FunName"]= "PlayerTran",["Description"]= "玩家交易税",["PlayerTran"] = "PlayerTran"},
			["Trap"] 				= {["FunName"]= "Trap",["Description"]= "踩trap加钱",["TrapAddMoney"] = "TrapAddMoney"},
			["LearnSkill"] 	= {["FunName"]= "LearnSkill",["Description"]= "技能学习",["LearnSkill"] = "LearnSkill"},
			["EquipDurability"] = {["FunName"] = "EquipDurability",["Description"]= "装备修理", ["RenewEquip"] = "RenewEquip"},
			["ToolMall"] = {["FunName"]= "ToolMall" ,["Description"]= "道具商城", ["BuyItemForSelf"] = "BuyItemForSelf", ["BuyItemForOther"] = "BuyItemForOther", ["DrawBalance"] = "DrawBalance"},
			["LiveSkill"] = {["FunName"]= "LiveSkill" ,["Description"]= "生活技能和专精学习", ["LearnSkill"] = "LearnSkill"},
			["Afflatus"] = {["FunName"] = "Afflatus",["Description"]= "买离线经验", ["ExChgMoney"] = "ExChgMoney"},
			["Stone"] = {["FunName"] = "Stone",["Description"]= "宝石系统", ["SynthesisExpend"] = "SynthesisExpend", ["RemoveStone"] = "RemoveStone"},
			["Chat"] = {["FunName"] = "Chat",["Description"]= "频道聊天收费", ["WorldChat"] = "WorldChat",["ChuanSheng"] = "ChuanSheng"},
			["ContractManufacture"] = {["FunName"] = "ContractManufacture",["Description"]= "代工厂管理费和税收", ["AddContractOrder"] = "AddContractOrder", ["CancelContractOrder"] = "CancelContractOrder"},
			["RobResourceAward"] = {["FunName"] = "RobResourceAward",["Description"]= "占领资源点佣兵小队长领取非绑定金", ["HeaderAward"] = "HeaderAward"},
			["SpecialTransport"] = {["FunName"] = "SpecialTransport",["Description"]= "特殊NPC传送消耗", ["SpecialNpc"] = "SpecialNpc"},
			["ClearTalent"] = {["FunName"] = "ClearTalent",["Description"]= "玩家转职", ["ClearTalent"] = "ClearTalent"},
			["ArmorPieceEnactment"] = {["FunName"] = "ArmorPieceEnactment",["Description"]= "装备镶嵌护甲片扣钱", ["ArmorPieceEnactment"] = "ArmorPieceEnactment"},
			["SpecialTransportUnBind"] = {["FunName"] = "SpecialTransportUnBind",["Description"]= "特殊NPC传送非绑定金币", ["SpecialTransportUnBind"] = "SpecialTransportUnBind"},
			["EquipSuperadd"] = {["FunName"] = "EquipSuperadd",["Description"]= "装备追加消耗金钱", ["EquipSuperadd"] = "EquipSuperadd"},
			["WebShop"] = {["FunName"] = "WebShop",["Description"]= "WebShop购买", ["buy"] = "buy"},
			["PurchasingAgent"] = {["FunName"] = "PurchasingAgent",["Description"]= "求购行管理费",["CancelBuyOrder"] = "CancelBuyOrder",["AddBuyOrder"] = "AddBuyOrder"},
			["GMCompenate"] = {["FunName"] = "GMCompenate",["AddMoney"] = "AddMoney"},
			["BuyCoupons"] = {["FunName"] = "BuyCoupons",["Description"]= "购买点券",["BuyCouponsUsingJinBi"] = "BuyCouponsUsingJinBi"},
			["LearnTongTech"] ={["FunName"]="LearnTongTech", ["Description"]="学习佣兵小队科技",["LearnTongTech"] ="LearnTongTech"},
	}
end

function CMoney:GetFuncInfo(sFuncName)
	return self.m_tblMoneyFun[sFuncName]
end

--把某数量的铜换算成金、银、铜的数量，然后返回
function CMoney:ChangeMoneyToGoldAndArgent(money)
	local Money		= money or 0
	local nMoney	= tonumber(Money)
	local nGold		= math.floor(nMoney / 10000) 
	local nSilver	= math.floor( (nMoney - nGold * 10000) / 100)
	local nCopper	= math.floor((nMoney - nGold * 10000 - nSilver * 100))
	return nGold, nSilver, nCopper
end

--把一定数量的铜转换为金银铜形式并以字符串方式返回
--金锭 		EGoldType.GoldBar 或者 nil
--金币(RMB) EGoldType.GoldCoin 	
function CMoney:ChangeMoneyToString(nMoney,eGoldType)
	local nGold, nSilver, nCopper = self:ChangeMoneyToGoldAndArgent(nMoney)
	local str = ""
	if eGoldType then 
		str = ( nGold <= 0 and "" or nGold .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1001") )
					.. ( (nSilver <= 0 and nGold <= 0) and "" or nSilver .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1002") )
					.. ( nCopper .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1003") )		
	else
		str = ( nGold <= 0 and "" or nGold .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1106") )
					.. ( (nSilver <= 0 and nGold <= 0) and "" or nSilver .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1107") )
					.. ( nCopper .. CTextureFontMgr_Inst():GetEmtStrByIdx("#1108") )
	end
				
	return str
end

--把金银铜转换为铜
function CMoney:ChangeGoldAndArgentToMoney(Gold, Argent, Copper)
	local nGold	= (tonumber(Gold)) or 0
	local nArgent =  (tonumber(Argent)) or 0
	local nCopper = (tonumber(Copper)) or 0
	
	return nGold * 10000 + nArgent * 100 + nCopper
end

function CMoney:ShowMoneyInfo(nMoney,tblWnd)
	local GoldText,GoldBtn,ArgentText,ArgentBtn,CopperText,CopperdBtn = unpack(tblWnd)
	
	local jinCount, yinCount, tongCount = self:ChangeMoneyToGoldAndArgent(nMoney)
	
	CopperText:SetWndText(tongCount)
	
	if jinCount > 0 then
		GoldBtn:ShowWnd(true)
		GoldText:ShowWnd(true)
		
		ArgentText:ShowWnd(true)
		ArgentBtn:ShowWnd(true)
		
		GoldText:SetWndText(jinCount)
		ArgentText:SetWndText(yinCount)
	else
		
		GoldBtn:ShowWnd(false)
		GoldText:ShowWnd(false)
		GoldText:SetWndText("")
		
		if yinCount > 0 then
			ArgentText:ShowWnd(true)
			ArgentBtn:ShowWnd(true)
			ArgentText:SetWndText(yinCount)
		else
			ArgentText:ShowWnd(false)
			ArgentBtn:ShowWnd(false)
			ArgentText:SetWndText("")
		end
	end
end

--检查输入的金钱是否符合要求，参数：窗口对象，窗口类型（金或者购买数量等：1、银：2、铜：2）
--检查的内容包括：
--@不允许输入非数字的东西；@不允许输入空格；@银和铜为数限制为2位；@不允许输入小数
function CMoney:CheckInputMoneyValidate(wndObj, wndType,funType)
	local inputText = wndObj:GetWndText()
	local findResult = string.find(inputText, "[^0123456789]")
	if  findResult ~= nil  then
		inputText = string.sub(inputText, 1, findResult-1)
		wndObj:SetWndText( inputText )
	end
	local inputCount = tonumber( inputText )
	if inputCount == nil or inputCount <= 0 then 	--输入的是非数字的东西
		wndObj:SetWndText("")
		inputCount = 0
		return
	end
	if wndType == 1 and inputCount > 999999 then
	    local validateMoney = string.sub(inputText, 1, 6 )
    	wndObj:SetWndText(validateMoney)
	    if wndObj.m_MsgBox == nil then
	        local function CallBack(Context,Button)
	            if(Button == MB_BtnOK) then
	                wndObj.m_MsgBox:DestroyWnd()
	                wndObj.m_MsgBox = nil
	            end
	        end
	        	if funType == "SendMail" then
	        		 wndObj.m_MsgBox = MessageBox(wndObj, MsgBoxMsg(255), MB_BtnOK, CallBack)
	        	elseif funType == "ContributeMoney" then
	        		--
	        	else
            	wndObj.m_MsgBox = MessageBox(wndObj, MsgBoxMsg(160), MB_BtnOK, CallBack)
            end
            return 
       end
	end
	if wndType == 2 and inputCount > 99 then --如果输入的是银或铜，则不能超过99	 		
		local validateMoney = string.sub(inputText, 1, 2 )
		wndObj:SetWndText(validateMoney)
		return
	end
end

--用于系统收费的计算；参数：基础价格，商品数目, 折扣，其他折扣
--现用于寄售交易所添加出售和收购订单时，所需要的费用
function CMoney:CalculateTotalFee(price, count, discount, otherDiscount)
    assert(IsNumber(price) and IsNumber(discount) and IsNumber(otherDiscount))
    local totalFee = math.ceil(price * count * discount * otherDiscount)
    if totalFee < 10 then
        return 10
    else
        return totalFee
    end
end

function CMoney:ShowMoneyInfoWithoutShowingWndFalse(nMoney,tblWnd)
	local GoldText,ArgentText,CopperText = unpack(tblWnd)
	
	local jinCount, yinCount, tongCount = self:ChangeMoneyToGoldAndArgent(nMoney)
	CopperText:SetWndText(tongCount)

	if jinCount > 0 then
		GoldText:SetWndText(jinCount)
		ArgentText:SetWndText(yinCount)
	else
	    GoldText:SetWndText("")
		if yinCount > 0 then
			ArgentText:SetWndText(yinCount)
		else
			ArgentText:SetWndText("")
		end
	end
end

function CMoney:GetAftertaxMoney(Money)
	if tonumber(Money) == nil then
		return 0
	end
	return math.ceil(Money*0.95)
end

--判断玩家拥有的金钱，收取系统管理费用是否足够
function CMoney:CheckPayingSysFeeEnough(totalFee)
    if totalFee > g_MainPlayer.m_nBindMoney then 
    	if totalFee > g_MainPlayer.m_nMoney + g_MainPlayer.m_nBindMoney then
    		return false, 156
    	else
            return true, 161
    	end
    end
    return true
end

g_MoneyMgr = g_MoneyMgr or CMoney:new()
