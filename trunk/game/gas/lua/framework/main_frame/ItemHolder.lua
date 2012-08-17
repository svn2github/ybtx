--拾取容器，npc尸体，采集型交互对象，掉落的箱子等所有可以拾取物品的功能
gas_require "framework/main_frame/ItemHolderInc"


--[[
	Owner: 可能是Player对象,也可能是Team对象
]]
function CItemHolder:Create(Owner)

	self._m_uCurItemIndex = 0
	self.m_Owner=Owner
	self.m_tbl_Items={}
	self.m_tbl_Opener={}		--所有打开了该容器的玩家对象链表
	g_App:RegisterTick(self,30*1000)
	self:SetTickName("CItemHolder")

end


function CItemHolder:Destroy()

	g_App:UnRegisterTick(self)
	
end

--uQuality DynAttrib仅仅对动态装备有用
function CItemHolder:AddItem( ItemPlayerOwner,uItemBigId,uItemSmallId, DynAttrib )

	local ItemInfo = 
	{
		m_Owner=ItemPlayerOwner,
		m_uItemBigId=uItemBigId,
		m_uItemSmallId=uItemSmallId,
		m_DynAttrib=DynAttrib
	}
	self._m_uCurItemIndex = self._m_uCurItemIndex - 1
	self.m_tbl_Items[self._m_uCurItemIndex]=ItemInfo
	
	local uFakeItemId = self._m_CurItemIndex
	local nBindingType = 0
	for k,v in pairs(self.m_tbl_Opener) do

		local Conn = k.m_Conn
				
		Gas2Gac:RetAddItemToPickWnd( Conn,uFakeItemId )		
		
	end
end


function CItemHolder:DelItem( uFakeItemId )

	self.m_tbl_Items[uFakeItemId]=nil
	
	
	for k,v in pairs(self.m_tbl_Opener) do
		
		Gas2Gac:RetDelItemFrPickWnd( k.m_Conn,uFakeItemId )
		
	end

end


function CItemHolder:OnTick()

	--print("拾取等待超时，所有玩家都可以拾取该物品")
	self.m_Owner = nil
	
end


--玩家打开这个物品容器,此时将物品列表发送给这个玩家，玩家指针被记录
--到列表里，容器里的物品的变动都会发送给列表中的所有玩家。
function CItemHolder:Open( Player )
	self.m_tbl_Openner[Player]=1
end


function CItemHolder:Close( Player )
	self.m_tbl_Openner[Player]=nil
end