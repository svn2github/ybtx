
function Gac2Gas:AddNeedContractManufactureOrder(Conn, skillType, direction, timeLimit, money, prescripName)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
    if Conn.m_Player.m_ItemBagLock then
        MsgToConn(Conn, 160043)
        return
    end
    local moneyObj = CMoney:new()
    local systemFee = moneyObj:CalculateTotalFee(money , 1, 0.01, timeLimit/24)
    local data = {["CharID"] = charID, ["PrescripName"] = prescripName,
    ["SkillType"]=skillType, ["Direction"] = direction,  ["TimeLimit"]= timeLimit, ["Money"] = money, ["SystemFee"] = systemFee}
    
    local function CallBack(sucFlag, result)
        if sucFlag == false then
            local errorMsgId = result
            MsgToConn(Conn, errorMsgId)
        else
            MsgToConn(Conn, 340009)
            local ItemData = result["ItemInfo"]
            local money = result["Money"]
			Gas2Gac:RetAddCMOrderSuc(Conn)         
        end
    end
    
    CallAccountManualTrans(Conn.m_Account, "ContractManufactureDB", "AddContractManufactureOrder", CallBack, data, "ContractManufacture")        
end


function Gac2Gas:SearchCharContractManufactureOrder(Conn, curPage)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    local charID = Conn.m_Player.m_uID
    local data  =  {["CharID"] = charID, ["CurPage"] = curPage} 
    local function CallBack(result)
        local orderNum = result["OrderNum"]
        local orderRet = result["OrderInfoRet"]
        local orderItemRet = result["ItemInfoRet"]
        local totalPage = result["TotalPage"]
        Gas2Gac:RetCharCMOrderBegin(Conn)
        for i=1, orderNum do
            local endTime = orderRet(i,5) 
            if endTime < 0 then
                endTime  = 0
            end
            Gas2Gac:RetCharContractManufactureOrder(Conn,orderRet(i,1) ,orderRet(i,2),orderRet(i,3) ,orderRet(i,4),endTime ,orderRet(i,6))
            local materialKinds = orderItemRet[i]:GetRowNum()
            for j=1, materialKinds do
                local materialType = orderItemRet[i](j,1)
                local materialName = orderItemRet[i](j,2)
                local materialCount= orderItemRet[i](j,3)
                local materialIndex = j
                Gas2Gac:RetCharCMOrderMaterialInfo(Conn, orderRet(i,1), materialIndex, materialType, materialName, materialCount)
            end
        end
        Gas2Gac:RetCharCMOrderEnd(Conn, totalPage)
    end
    CallAccountManualTrans(Conn.m_Account, "ContractManufactureDB", "SearchCharContractManufactureOrder", CallBack, data, "ContractManufacture")        
end                               


function Gac2Gas:CancelContractManufactureOrder(Conn, cmOrderID)  
    if not IsCppBound(Conn.m_Player) then
        return 
    end      
    local data = {["CharID"] = Conn.m_Player.m_uID, ["CMOrderID"] =cmOrderID }
    
    local function CallBack(sucFlag, msgID)
        if sucFlag == false then
            MsgToConn(Conn, msgID)
        else
            MsgToConn(Conn, 340008)
        end
        Gas2Gac:RetCancelContractManufactureOrder(Conn)
    end
    
    CallAccountManualTrans(Conn.m_Account, "ContractManufactureDB", "CancelContractManufactureOrder", CallBack, data, "ContractManufacture")        
end

function CheckContractManufactureOrderTimeOut()
   local data = {}
    local function CallBack(sucFlag, result)
        if sucFlag == true then
            local sell_playerID_tbl 		=	result["MakeOrderPlayerIDTbl"]
            if sell_playerID_tbl ~= nil then
        		for i=1, # sell_playerID_tbl do
        			local player=g_GetPlayerInfo(sell_playerID_tbl[i])
        			if player~=nil then
        				local Conn=player.m_Conn
        				SystemFriendMsgToClient(Conn, 5006)
        			end
        		end
	        end 
        end
    end
    CallDbTrans("ContractManufactureDB", "CheckCMOrderTimeOut", CallBack, data,"ContractManufacture" )
end

function Gac2Gas:SearchCommonContractManuOrder(Conn, productName, skillName, skillLevel, productLowLevel, productHighLevel, curPage)
    local data = {["ProductName"] =productName, ["SkillName"]=skillName, ["SkillLevel"]=skillLevel,  
    ["LowLevel"] =productLowLevel,  ["HighLevel"]=productHighLevel, ["CurPage"]=curPage}
    local function CallBack(result)
        local totalOrderCount   = result["TotalOrderCount"]
        local OrderCountRet        = result["OrderRet"]
        local materialInfoTbl   = result["MaterialInfoTbl"]
        local totalPage         = result["TotalPage"]
        local OrderCount = OrderCountRet:GetRowNum()
        
        Gas2Gac:RetCommonCMOrderBegin(Conn)
        for i=1, OrderCount do
            local materialNodeCount = # materialInfoTbl[i]
            if materialNodeCount > 0 then
                Gas2Gac:RetCommonContractManufactureOrder(Conn, OrderCountRet(i, 1), OrderCountRet(i, 2), OrderCountRet(i, 3), OrderCountRet(i, 4), OrderCountRet(i, 5), OrderCountRet(i, 6), OrderCountRet(i, 7))
                for j =1, materialNodeCount do
                    local materialType = materialInfoTbl[i][j][1]
                    local materialName = materialInfoTbl[i][j][2]
                    local materialNum  = materialInfoTbl[i][j][3]
                    Gas2Gac:RetCommonCMOrderMaterialInfo(Conn, OrderCountRet(i, 1), j, materialType, materialName, materialNum)
                end
            end
        end
        Gas2Gac:RetCommonCMOrderEnd(Conn)
    end
    
    CallAccountManualTrans(Conn.m_Account, "ContractManufactureDB", "SearchCommonContractManuOrder", CallBack, data, "ContractManufacture")        
end

function Gac2Gas:TakeContractManufactureOrder(Conn, cmOrderID)
    if not IsCppBound(Conn.m_Player) then
        return 
    end
    if Conn.m_Player.m_ItemBagLock then
        MsgToConn(Conn, 160044)
        return
    end
    local charID = Conn.m_Player.m_uID
    local data = {["CharID"] = charID, ["CMOrderID"] =cmOrderID }
    local function CallBack(bFlag,result)
        
    	if not bFlag then
    		if IsNumber(result) then
    			MsgToConn(Conn,result)
    		end
    		return 
    	end
     	MsgToConn(Conn, 340010)
     	Gas2Gac:RetTakeContractManufactureOrder(Conn)
    end
    CallAccountManualTrans(Conn.m_Account, "ContractManufactureDB", "TakeContractManufactureOrder", CallBack, data, "ContractManufacture")        
end
