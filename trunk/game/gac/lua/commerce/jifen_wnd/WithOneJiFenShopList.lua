lan_load "npc/Lan_WithOneNpcShopDisplay_Common"

CWithOneJiFenShopListWnd = class(SQRDialog)

function CWithOneJiFenShopListWnd:Ctor(parent)
    self:CreateFromRes("JiFenWithOneShopList", parent)
    self.m_List = self:GetDlgChild("List")
end

function CWithOneJiFenShopListWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2)
    if uMsgID ==  ITEM_LBUTTONCLICK then
        local choosedItem = Child:GetSelectItem(-1) + 1
        local NPCName =  self.m_ListItemTbl[choosedItem].NPCName
        g_GameMain.m_NPCShopSell:NPCShopSellWnd(NPCName)
        self:ShowWnd(false)
    end
end

--初始随身积分商店列表
function CWithOneJiFenShopListWnd:InitWithOneJiFenShopList(jiFenShopList)
    self.m_ListItemTbl = {}
    self.m_ListItemWndTbl = {}
    self.m_List:DeleteAllItem()
    self.m_List:InsertColumn(0, 170)
    for i, v in pairs (jiFenShopList) do
        local index = i
        local itemWnd = SQRDialog:new()
        self.m_List:InsertItem(index - 1, 30)
        local item = self.m_List:GetSubItem(index-1, 0)
        itemWnd:CreateFromRes("JiFenWithOneShopItem", item)
        local showName= Lan_WithOneNpcShopDisplay_Common(MemH64(v),"DisplayName")
        itemWnd:SetWndText(showName)
        item.NPCName = v
        table.insert(self.m_ListItemTbl, item)
        table.insert(self.m_ListItemWndTbl, itemWnd)
        itemWnd:ShowWnd(true)
    end
end