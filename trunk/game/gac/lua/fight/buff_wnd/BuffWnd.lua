gac_require "framework/texture_mgr/TextureMgr"

CBuffWnd								= class ( SQRDialog )
CBuffItem 							= class ( SQRDialog )
function CBuffWnd:Init( Parent, ParentRectWnd )
	self:InitData()
	self.m_Tick		= {}
	self:CreateFromRes( "BuffDebuff", Parent )
	self.m_BuffItemList = self:GetDlgChild("BuffItemList")
	self:SetIsBottom(true)
	self:SetTransparent( 0 )
	self:InitList()
	self:InitPos(ParentRectWnd)
end

-----------------------------------------------------------------------
function CBuffWnd:InitData()
	self.m_RowNum			= 4
	self.m_ColNum			= 6
	self.m_IconSize			= 22
	self.m_OneTypeLineNum	= 2
	self.m_tblBuffs = {{}, {}}
	self.m_tblBuffs[1].n = 0
	self.m_tblBuffs[2].n = 0
end

function CBuffWnd:GetBuffList(BuffType)
	return BuffType and self.m_tblBuffs[2] or self.m_tblBuffs[1]
end

function CBuffWnd:SetListItemIndex(BuffList, ID, BuffType)
	if g_MainPlayer and BuffList[ID].installerID == g_MainPlayer:GetEntityID() then
		self:ClearBuffItemWnd()
		self:InsertBuffSortRect(BuffList, ID, BuffType)
		self:ReSetRect()
	else
--		if(not BuffType) then --Buff
--			BuffY = 0
--			if(self.m_tblBuffs[2].n > 0 and 1 == BuffX) then
--				self:ClearBuffItemWnd()
--				self:InsertBuffSortRect(BuffList, ID, BuffType)
--				self:ReSetRect()
--				return
--			end
--		end
		if (not BuffType and BuffList[ID].bFriend) or (BuffType and not BuffList[ID].bFriend) then
			BuffList[ID].listItemIndex = BuffList.n
			local TypeIndex = BuffList[ID].bFriend and 1 or 2
			local AddCount = self.m_ColNum*math.ceil(self.m_tblBuffs[TypeIndex].n / self.m_ColNum)
			local DeBuffIndex = BuffList[ID].bFriend and 2 or 1
			local tblBuffList = self:GetBuffList(not BuffType)
			local Last = nil
			local Index = 0
			local nowLastIndex = 0
			for i, p in pairs(tblBuffList) do
				if( IsNumber(i) ) then
					Index = Index + 1
					for j, w in pairs(tblBuffList) do
						if( IsNumber(j) ) then
							if w.listItemIndex > nowLastIndex and (not Last or w.listItemIndex < Last.listItemIndex) then
								Last = w
							end
						end
					end
					if Last then
						nowLastIndex = Index
						Last.listItemIndex = Index
					end
				end
			end
			for i, p in pairs(tblBuffList) do
				if( IsNumber(i) ) then
					p.listItemIndex = p.listItemIndex + AddCount
				end
			end
			self:ClearBuffItemWnd()
			self:ReSortBuffRect()
			self:ReSetRect()
		else
			local TypeIndex = BuffList[ID].bFriend and 1 or 2
			local BuffX, BuffY = self:GetBuffShowPosByIndex(self.m_tblBuffs[TypeIndex].n)
			BuffList[ID].listItemIndex = BuffList.n + self.m_ColNum*BuffY
		end
	end
end

function CBuffWnd:InitPos(ParentRectWnd)
	local Rect	= CFRect:new()
	ParentRectWnd:GetLogicRect(Rect)
	self:SetPos(self, Rect.left, Rect.bottom + 2)
end
----------------------------------------------------------------------

function CBuffWnd:SetPos(wnd, x, y)
	local Rect=CFRect:new()
	wnd:GetLogicRect(Rect)
	if(x) then
		local uWidth  = Rect.right - Rect.left
		Rect.left	= x
		Rect.right	= x + uWidth
	end
	if(y) then
		local uHeight = Rect.bottom - Rect.top
		Rect.top	= y
		Rect.bottom	= y + uHeight
	end
	wnd:SetLogicRect(Rect)
end

function CBuffWnd:InitList()
	local Rect=CFRect:new()
	Rect.left	= 0
	Rect.top	= 0
	Rect.right	= self.m_IconSize * self.m_ColNum
	Rect.bottom	= self.m_IconSize * self.m_RowNum
	self:SetLogicRect(Rect)
	self.m_BuffItemList:SetLogicRect(Rect)
	
	for i = 1, self.m_ColNum do
		self.m_BuffItemList:InsertColumn(i - 1, self.m_IconSize)
	end
	for i = 1, self.m_RowNum do
		self.m_BuffItemList:InsertItem(i - 1, self.m_IconSize)
	end
	self.m_tblListItemWnd = {}
	for i = 1, self.m_RowNum do
		for j = 1, self.m_ColNum do
			local Item		= self.m_BuffItemList:GetSubItem(i - 1, j - 1)
			local ItemWnd	= self:CreateBuffItem(Item)
			table.insert(self.m_tblListItemWnd, ItemWnd)
		end
	end
end

function CBuffWnd:GetBuffItem()
	return CBuffItem:new()
end

function CBuffWnd:CreateBuffItem(parent)
	local wnd = self:GetBuffItem()
	wnd:CreateFromRes("BuffItem", parent)
	wnd.m_BuffItemDivWnd	= wnd:GetDlgChild("BuffDivWnd")
	wnd.m_BuffItemClock		= wnd:GetDlgChild("BuffItemClock")
	wnd.m_BuffItemClock:SetBackImageColor(0xaa000000)  --设置透明度及颜色
	wnd:SetTransparent(0)
	wnd.ID = 0
	return wnd
end

function CBuffWnd:ResetBuff(BuffList, tblInfo, ID, RemainTime)
	BuffList[ID].name		= tblInfo.name
	BuffList[ID].divNum	= tblInfo.divNum
	BuffList[ID].time		= tblInfo.time
	BuffList[ID].realName	= tblInfo.realName
	if(RemainTime >= 0) then --RemainTime~=-2时 则更新时间
		BuffList[ID].runedTime = tblInfo.runedTime
	end
	local ItemWnd = self.m_tblListItemWnd[BuffList[ID].listItemIndex]
	ItemWnd.m_BuffItemDivWnd:SetWndText( (1 == tblInfo.divNum) and "" or tblInfo.divNum)
	if(tblInfo.time ~= -1000 and RemainTime ~= -2)then
		local Character = CCharacterFollower_GetCharacterByID(tblInfo.entityID)
		ItemWnd.m_BuffItemClock:setTimeEx(tblInfo.time, GetRenderTime(Character))
		ItemWnd.m_BuffItemClock:setRunedTime(BuffList[ID].runedTime)
	end
	self:TooltipsSetTime(BuffList[ID], ID)
end

function CBuffWnd:DeleteFirstBuffDebuff(BuffList)
	if(BuffList.n >= self.m_OneTypeLineNum * self.m_ColNum) then
		for m, n in pairs(BuffList) do
			if( IsNumber(m) ) then
				self:DeleteBuffDebuff( m )
				break
			end
		end
	end
end

function CBuffWnd:ArriveMaxBuffNum(BuffList)
	if(BuffList.n >= self.m_OneTypeLineNum * self.m_ColNum) then
		return true
	end
	return false
end

function CBuffWnd:SetBuff(BuffList, tblInfo, ID, RemainTime, BuffType)
	local BuffCount = BuffList.n
	if(BuffList[ID]) then
		self:ResetBuff(BuffList, tblInfo, ID, RemainTime)--可叠加buff ,Tick只需要注册一次,如果原来有,不需在注册
		return
	else
		if self:ArriveMaxBuffNum(BuffList) then
			return
		end
		--查下目标是否存在相同ID的Buff
		for i, v in ipairs(self.m_tblBuffs) do
			if(v[ID]) then
				local id1 = (not v[ID].entityID) and 0 or v[ID].entityID
				local id2 = (not tblInfo.entityID) and 0 or tblInfo.entityID
				local str = string.format("%s,%s,%d,%d,%d",v[ID].realName,tblInfo.realName,id1,id2,ID)
				LogErr("SetBuff时已经存在相同ID的Buff", str)
				self:DeleteBuffDebuff(ID)
			end
		end
		
		BuffList[ID] = tblInfo
		BuffList.n = BuffList.n + 1
		self:ShowWnd( true )
		self:SetTransparent(0)
		self:SetListItemIndex(BuffList, ID, BuffType)
		self:ShowBuff(BuffList[ID], ID)
	end
end

function CBuffWnd:TooltipsSetTime(tblInfo, ID)
	local second	= (tblInfo.time == -1000) and "" or SetTime(tblInfo.time - tblInfo.runedTime)
	local Index		= tblInfo.listItemIndex
	self.m_tblListItemWnd[Index]:SetMouseOverDescAfter("#cffffff" .. tblInfo.name .."#r".. second)
end

function CBuffWnd:ShowBuff(buffInfo, ID)
	local Character = CCharacterFollower_GetCharacterByID(buffInfo.entityID)
	local Index = buffInfo.listItemIndex
	g_LoadBufIconFromRes(buffInfo.smallIcon, self.m_tblListItemWnd[Index], -1, IP_ENABLE, IP_ENABLE)
	self.m_tblListItemWnd[Index].ID = ID
	self:TooltipsSetTime(buffInfo, ID)
	for i, v in ipairs(self.m_tblBuffs) do
		for j, w in pairs(v) do
			if( IsNumber(j) ) then
				self.m_tblListItemWnd[w.listItemIndex]:SetTransparent(100)
			end
		end
	end

	if(buffInfo.time ~= -1000)then
		local Tick = RegisterDistortedTickByCoreObj( Character, "BuffWndTick", self.OnTick, 1000, self,self,ID )
		if(self.m_Tick[ID])then
			LogErr("注册Buff的Tick时存在相同ID的Tick了", buffInfo.realName)
			self:DeleteBuffDebuff(ID)
		end
		self.m_Tick[ID] = Tick
		self.m_tblListItemWnd[Index].m_BuffItemClock:SetBeFrontImageColor(true)
		self.m_tblListItemWnd[Index].m_BuffItemClock:setTimeEx(buffInfo.time,GetRenderTime(Character))
		self.m_tblListItemWnd[Index].m_BuffItemClock:setRunedTime(buffInfo.runedTime)
	else
		self.m_tblListItemWnd[Index].m_BuffItemClock:SetBeFrontImageColor(false)
		self.m_tblListItemWnd[Index].m_BuffItemClock:setTimeEx(0,GetRenderTime(Character))
		self.m_tblListItemWnd[Index].m_BuffItemClock:setRunedTime(0)
	end
	self.m_tblListItemWnd[Index]:ShowWnd(true)
	self.m_tblListItemWnd[Index].m_BuffItemDivWnd:SetWndText( (1 == buffInfo.divNum) and "" or buffInfo.divNum )
	
	if g_MainPlayer and buffInfo.installerID == g_MainPlayer:GetEntityID() then
		self.m_tblListItemWnd[Index]:AddFlashInfoByName("buffsquare")
	end
end

function CBuffWnd:GetBuffShowPosByIndex(nIndex)
	if(nIndex <= 0) then return 0, 0 end
	local y = math.ceil( nIndex / self.m_ColNum )
	local x = (nIndex - 1) % self.m_ColNum + 1
	return x, y
end

function CBuffWnd:UpdateBuffDebuff(ID, sName, DivNum, Time, RemainTime, BuffType, SmallIcon, sRealName, uEntityID, InstallerID, bFriend)
	if (DivNum == 0) then
		self:DeleteBuffDebuff( ID )
		return
	end
	local Runedtime = (RemainTime >= 0) and (Time - RemainTime) or 0
	Time = Time*1000
	Runedtime = Runedtime*1000
	
	local tblInfo		= {}
	tblInfo.name		= sName
	tblInfo.divNum		= DivNum
	tblInfo.time		= Time
	tblInfo.runedTime	= Runedtime
	tblInfo.realName	= sRealName
	tblInfo.entityID	= uEntityID
	tblInfo.smallIcon	= SmallIcon
	tblInfo.installerID	= InstallerID
	tblInfo.bFriend		= bFriend
	
	local tblBuffList = self:GetBuffList(BuffType)
	self:SetBuff(tblBuffList, tblInfo, ID, RemainTime, BuffType)
end

function CBuffWnd:DeleteBuffDebuff(ID)
	self:ClearBuffItemWnd()
	local uEntityID = 0
	for i, v in ipairs(self.m_tblBuffs) do
		if(v[ID]) then
			uEntityID	= v[ID].entityID
			v[ID]		= nil
			v.n			= v.n - 1
		end
	end
	self:DeleteTickAndShow(ID, uEntityID)
	self:ReSortBuffRect()
	self:ReSetRect()
end

function CBuffWnd:InsertBuffSortRect(BuffList, ID, BuffType)
	local selfBuffCount = 0
	local TypeIndex = BuffList[ID].bFriend and 1 or 2
	if (BuffType and BuffList[ID].bFriend) or (not BuffType and not BuffList[ID].bFriend)then
		local value = math.ceil(self.m_tblBuffs[TypeIndex].n / self.m_ColNum)
		selfBuffCount = self.m_ColNum*value
	end
	local tempTblOtherBuff = {}
	for k, p in pairs(BuffList) do
		if( IsNumber(k) ) then
			if BuffList[ID].installerID == p.installerID then
				selfBuffCount = selfBuffCount + 1
			else
				tempTblOtherBuff[k] = p
			end
		end
	end
	BuffList[ID].listItemIndex = selfBuffCount ~= 0 and selfBuffCount or 1
	local function BuffTblEmpty(tbl)
		for k, p in pairs(tbl) do
			if( IsNumber(k) ) then
				return true
			end
		end
		return false
	end
	while(BuffTblEmpty(tempTblOtherBuff)) do
		local Index = 0
		local tempBuffID = 0
		for m, n in pairs(tempTblOtherBuff) do
			if( IsNumber(m) ) then
				if n.listItemIndex < Index or Index == 0 then
					Index = n.listItemIndex
					tempBuffID = m
				end
			end
		end
		selfBuffCount = selfBuffCount + 1
		BuffList[tempBuffID].listItemIndex = selfBuffCount
		tempTblOtherBuff[tempBuffID] = nil
	end
	if (not BuffType and BuffList[ID].bFriend) or (BuffType and not BuffList[ID].bFriend)then
		local value = math.ceil(self.m_tblBuffs[TypeIndex].n / self.m_ColNum)
		if (self.m_tblBuffs[TypeIndex].n-1) == self.m_ColNum*(value-1) then
			local TempBuffList = self:GetBuffList(not BuffType)
			local Index = self.m_ColNum*value
			for k, p in pairs(TempBuffList) do
				if( IsNumber(k) ) then
					Index = Index + 1
					TempBuffList[k].listItemIndex = Index
				end
			end
		end
	end
end

function CBuffWnd:ReSortBuffRect()
	for i, v in ipairs(self.m_tblBuffs) do
		local value = 0
		for m, n in pairs(v) do
			if( IsNumber(m) ) then
				if n.bFriend then
					value = (2 == i) and math.ceil(self.m_tblBuffs[1].n / self.m_ColNum) or 0
				else
					value = (1 == i) and math.ceil(self.m_tblBuffs[2].n / self.m_ColNum) or 0
				end
				break
			end
		end
		local Index = 0
		local count = 0
		for m, n in pairs(v) do
			if( IsNumber(m) ) then
				count = count + 1
				Index = count + self.m_ColNum*value
				local buffID = 0
				local buffInfo = nil
				for k, p in pairs(v) do
					if( IsNumber(k) ) then
						if p.listItemIndex == Index then
							buffInfo = p
							buffID = k
							break
						elseif p.listItemIndex > Index and ( not buffInfo or 
							p.listItemIndex < buffInfo.listItemIndex or uSelfEntityID == p.installerID) then
							buffInfo = p
							buffID = k
						end
					end
				end
				if buffID == 0 then
					buffID = m
					buffInfo = n
				end
				buffInfo.listItemIndex = Index
			end
		end
	end
end

function CBuffWnd:ReSetRect()
	local uSelfEntityID = g_MainPlayer and g_MainPlayer:GetEntityID() or 0
	for i, v in ipairs(self.m_tblBuffs) do
		for m, n in pairs(v) do
			if( IsNumber(m) ) then
				local Index = n.listItemIndex
				local itemWnd = self.m_tblListItemWnd[Index]
				g_LoadBufIconFromRes(n.smallIcon, itemWnd, -1, IP_ENABLE, IP_ENABLE)
				itemWnd.m_BuffItemDivWnd:SetWndText( (1 == n.divNum) and "" or n.divNum )
				self:TooltipsSetTime(n,m)
				itemWnd.ID = m
				itemWnd:SetTransparent(100)
				itemWnd:ShowWnd(true)
				local Character = CCharacterFollower_GetCharacterByID(n.entityID)
				if n.time ~= -1000 then
					local RemainTime = self:GetRemainTime(m, n.realName)
					RemainTime = string.format("%.3f",RemainTime)
					RemainTime = tonumber(RemainTime)*1000
					local Runedtime = n.time - RemainTime
					itemWnd.m_BuffItemClock:setTimeEx(n.time,GetRenderTime(Character))
					itemWnd.m_BuffItemClock:setRunedTime(Runedtime)
					itemWnd.m_BuffItemClock:SetBeFrontImageColor(true)
				else
					itemWnd.m_BuffItemClock:setTimeEx(0,GetRenderTime(Character))
					itemWnd.m_BuffItemClock:setRunedTime(0)
					itemWnd.m_BuffItemClock:SetBeFrontImageColor(false)
				end
				if n.installerID == uSelfEntityID then
					itemWnd:AddFlashInfoByName("buffsquare")
				end
			end
		end
	end
end

function CBuffWnd:ClearBuffItemWnd() --重新设置Debuff的位置,可能上移OR下移
--	for i, v in ipairs(self.m_tblBuffs) do
--		for j, w in pairs(v) do
--			if( IsNumber(j) and w.listItemIndex ~= nil) then
--				local ItemWnd = self.m_tblListItemWnd[w.listItemIndex]
--				g_DelWndImage(ItemWnd, 1, IP_ENABLE, IP_ENABLE)
--				ItemWnd:SetTransparent(0)
--				ItemWnd:ShowWnd(false)
--				ItemWnd:DelFlashAttention()
--			end
--		end
--	end
	for i = 1, #(self.m_tblListItemWnd) do --发现下面几个很耗
		g_DelWndImage(self.m_tblListItemWnd[i], 1, IP_ENABLE, IP_ENABLE)
		self.m_tblListItemWnd[i]:SetMouseOverDescAfter("")
		self.m_tblListItemWnd[i]:SetTransparent(0)
		self.m_tblListItemWnd[i]:ShowWnd(false)
		self.m_tblListItemWnd[i]:DelFlashAttention()
	end
end

function CBuffWnd:DeleteBuffDebuffIfCharacterDestroyed(BuffID)
	--实现空函数,不能删除
	return false
end

function CBuffWnd:OnTick(Tick, ID)
	local uEntityID	= 0
	for i, v in ipairs(self.m_tblBuffs) do
		if(v[ID]) then
			uEntityID = v[ID].entityID
			local leftTime = v[ID].time - v[ID].runedTime
			if( leftTime >= 0 ) then
				v[ID].runedTime	= v[ID].runedTime + 1000
				self:TooltipsSetTime(v[ID], ID)
				return
			else
				uEntityID = v[ID].entityID
				self:DeleteTickByID(ID, uEntityID)
				if self:DeleteBuffDebuffIfCharacterDestroyed(ID) then
					return
				end
			end
		end
	end
end

function CBuffWnd:DeleteTickByID(ID,uEntityID)
	if(self.m_Tick[ID])then
		local Character = CCharacterFollower_GetCharacterByID(uEntityID)
		if Character and IsCppBound(Character) then
			UnRegisterDistortedTickByCoreObj(Character, self.m_Tick[ID])
		else
			UnRegisterDistortedTick(self.m_Tick[ID])
		end
		self.m_Tick[ID] = nil
	end
end

function CBuffWnd:DeleteTickAndShow(ID,uEntityID)
	self:DeleteTickByID(ID,uEntityID)
	local nBuffNum = 0
	for i, v in ipairs(self.m_tblBuffs) do
		nBuffNum = nBuffNum + v.n
	end
	if(0 == nBuffNum) then self:ShowWnd(false) end
end

function CBuffWnd:ClearAllBuffState()
	for i, v in ipairs(self.m_tblBuffs) do
		for j, w in pairs(v) do
			if( IsNumber(j) ) then
				self:DeleteBuffDebuff(j)
			end
		end
	end
end
