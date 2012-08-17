local GetBuyingCouponsListFlag  = true --表明允许查询当前可购买点券列表
local BuyCouponsWebIP = ""
local BuyCouponsWebPort = ""

function Gac2Gas:GetCapableOfBuyingCouponsList(Conn)
    if GetBuyingCouponsListFlag == false then
        MsgToConn(Conn, 430001)
    end
    local player = Conn.m_Player
    if not IsCppBound(player)  then
        return
    end
    
    local tblLen = # g_CapableOfBuyingCouponsList
    Gas2Gac:RetCapableOfBuyingCouponsInfoBegin(Conn)
    for i=1, tblLen do
        local ID        = g_CapableOfBuyingCouponsList[i]["ID"]
        local name      = g_CapableOfBuyingCouponsList[i]["Name"]
        local price     = g_CapableOfBuyingCouponsList[i]["Price"]
        local smallIcon = g_CapableOfBuyingCouponsList[i]["SmallIcon"]
        local desc      = g_CapableOfBuyingCouponsList[i]["Desc"]
        local url       = g_CapableOfBuyingCouponsList[i]["Url"]
        Gas2Gac:RetCapableOfBuyingCouponsInfo(Conn, ID, name, price, smallIcon, desc, url) 
    end
    Gas2Gac:RetCapableOfBuyingCouponsInfoEnd(Conn)
end

function Gac2Gas:BuyCouponsUsingJinBi(Conn, ID, name)
    local player = Conn.m_Player
    if not IsCppBound(player)  then
        return
    end
    MsgToConn(Conn, 430006)
	--获得序列号
	local http = HttpClient:new()
	http.url = "/cgi-bin/gm_tools/Sell_GetSN.php"
	http.params = {["id"] = ID, ["server"] = ERatingConfig["AGIP_Gateway_id"], ["role"] = player.m_Properties:GetCharName(), ["user"] = player.m_Conn.m_UserName}
	local function call()
		http:_Connect(BuyCouponsWebIP, BuyCouponsWebPort)
		local flag, msg = coroutine.yield()
  
		local ret = http.m_ret_msg
		if ret == nil then
		    MsgToConn(Conn, 430005)
			return
		end
		if string.find(ret, "YBTX-BuyCoupons",1, true) == nil then
			MsgToConn(Conn, 430005)
			return
		end
		--print("ret:" .. ret)
		local i,_ = string.find(ret, "<error")
		if i ~= nil then
		    MsgToConn(Conn, 430003)
			--print("序列号：已售完")
		else
			local b1,_ = string.find(ret, "<code")
			local _,e1 = string.find(ret, ">", b1)
			local b2,_ = string.find(ret, "</code>", e1)
			ret = string.sub(ret, e1+1, b2-1)
			--print("序列号：" .. ret)
			local charID = player.m_uID
						local sequenceID = ret
            local data = {["CharID"]=charID, ["ID"] =ID, ["Name"] =name ,["SequenceID"] = sequenceID}
            local function CallBack(sucFlag, ret)
                if sucFlag == false then
                    MsgToConn(Conn, ret)
                    http.url = "/cgi-bin/gm_tools/Sell_BuyError.php"
                    http.params = {["id"] = ID, ["code"] = sequenceID}
                    http:_Connect(BuyCouponsWebIP, BuyCouponsWebPort)
                    --print("购买失败")
                else
                    --print("购买成功")
                    MsgToConn(Conn, 430004)
                    local couponsRet = ret["BoughtCoupons"]
                    Gas2Gac:RetBoughtCouponsInfoBegin(Conn)
                    local row = couponsRet:GetRowNum()
                    for i=1, row do
                        Gas2Gac:RetBoughtCouponsInfo(Conn, couponsRet(i,1), couponsRet(i,2), couponsRet(i,3), couponsRet(i,4), couponsRet(i,5), couponsRet(i,6), couponsRet(i,7), couponsRet(i,8) )
                    end
                    Gas2Gac:RetBoughtCouponsInfoEnd(Conn)
                end
            end
            CallAccountManualTrans(Conn.m_Account, "BuyCouponsDB", "BuyCouponsUsingJinBi", CallBack, data)	
		end
	end
	http.m_co = coroutine.create(call)
	coroutine.resume(http.m_co)
end

function Gac2Gas:GetBoughtCouponsInfo(Conn)
    local player = Conn.m_Player
    if not IsCppBound(player)  then
        return
    end
    local charID = player.m_uID
    local data = {["CharID"]=charID}
    local function CallBack(result)
        local couponsRet = result["BoughtCoupons"]
        Gas2Gac:RetBoughtCouponsInfoBegin(Conn)
        local row = couponsRet:GetRowNum()
        for i=1, row do
            Gas2Gac:RetBoughtCouponsInfo(Conn, couponsRet(i,1), couponsRet(i,2), couponsRet(i,3), couponsRet(i,4), couponsRet(i,5), couponsRet(i,6), couponsRet(i,7), couponsRet(i,8) )
        end
        Gas2Gac:RetBoughtCouponsInfoEnd(Conn)
    end
    CallAccountManualTrans(Conn.m_Account, "BuyCouponsDB", "GetBoughtCouponsInfo", CallBack, data)
end

function UpdateCapableOfBuyingCouponsInGas()
    GetBuyingCouponsListFlag = false
    g_CapableOfBuyingCouponsList = {}
--获得物品列表
	local http = HttpClient:new()
	http.url = "/cgi-bin/gm_tools/Sell_GetList.php"
	http.params = {}
	local function call()
		http:_Connect(BuyCouponsWebIP, BuyCouponsWebPort)
		local flag, msg = coroutine.yield()
		local ret = http.m_ret_msg
		if ret == nil then
			MsgToConn(Conn, 430005)
			return
		end
		if string.find(ret, "YBTX-BuyCoupons", 1 , true) == nil then
			MsgToConn(Conn, 430005)
			return
		end
		local x,y = string.find(ret, "<Item")
		while x ~= nil do
			_,x = string.find(ret, ">", y)
			y,_ = string.find(ret, "</Item>", x)
			local item = string.sub(ret, x+1, y-1)
			x,y = string.find(ret, "<Item", y)
			
			local a0,a1 = string.find(item, "<Id")
			_,a0 = string.find(item, ">", a1)
			a1,_ = string.find(item, "</Id>", a0)
			--print("ID:\t" .. string.sub(item, a0+1, a1-1))
			
			local b0,b1 = string.find(item, "<ItemName")
			_,b0 = string.find(item, ">", b1)
			b1,_ = string.find(item, "</ItemName>", b0)
			--print("ItemName:\t" .. string.sub(item, b0+1, b1-1))
			
			local c0,c1 = string.find(item, "<Tooltips")
			_,c0 = string.find(item, ">", c1)
			c1,_ = string.find(item, "</Tooltips>", c0)
			--print("Tooltips:\t" .. string.sub(item, c0+1, c1-1))
			
			local d0,d1 = string.find(item, "<SmallIcon")
			_,d0 = string.find(item, ">", d1)
			d1,_ = string.find(item, "</SmallIcon>", d0)
			--print("SmallIcon:\t" .. string.sub(item, d0+1, d1-1))
			
			local e0,e1 = string.find(item, "<URL")
			_,e0 = string.find(item, ">", e1)
			e1,_ = string.find(item, "</URL>", e0)
			--print("URL:\t" .. string.sub(item, e0+1, e1-1))
			
			local f0,f1 = string.find(item, "<Price")
			_,f0 = string.find(item, ">", f1)
			f1,_ = string.find(item, "</Price>", f0)
			--print("Price:\t" .. string.sub(item, f0+1, f1-1))
			
            local node = {}
			node["ID"] = tonumber(string.sub(item, a0+1, a1-1))
			node["Name"] = string.sub(item, b0+1, b1-1)
			node["Desc"] = string.sub(item, c0+1, c1-1)
			node["SmallIcon"] = tonumber(string.sub(item, d0+1, d1-1))
			node["Url"] = string.sub(item, e0+1, e1-1)
			node["Price"] = tonumber(string.sub(item, f0+1, f1-1))

			table.insert(g_CapableOfBuyingCouponsList, node)
			i,_ = string.find(ret, "%[", j)
		end
        
        local data = {["CouponsList"] = g_CapableOfBuyingCouponsList}
		local function CallBack()
            GetBuyingCouponsListFlag = true
	    end
		CallDbTrans("BuyCouponsDB", "UpdateCouponsInfo", CallBack, data)	
	end
	http.m_co = coroutine.create(call)
	coroutine.resume(http.m_co)
end

function GetBuyCouponsWebInfo()
    local data = {}
    local function CallBack(ip, port)
        BuyCouponsWebIP = ip(1, 1)
        BuyCouponsWebPort = port(1,1)
        UpdateCapableOfBuyingCouponsInGas()
    end
    CallDbTrans("BuyCouponsDB", "GetBuyCouponsWebInfo", CallBack, data)	
end
