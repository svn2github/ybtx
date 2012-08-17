gac_require "message/bag_shapeeffect_wnd/BagShapeEffectWndInc"

function CreateBagShapeEffectWnd(Parent)
	local Wnd = CBagShapeEffectWnd:new()
	Wnd:CreateFromRes("BagShapeEffectWnd",Parent)
	Wnd.m_Static = Wnd:GetDlgChild("Static")
	return Wnd
end

function CBagShapeEffectWnd:ShapeEffectFun(uBigID,uIndex)
	if(not (uBigID >= 5 and uBigID <= 9) ) then return end --只显示装备

	local function StartShape(Tick)
		if self.m_StartbagShapeTick then
			UnRegisterTick(self.m_StartbagShapeTick)
			self.m_StartbagShapeTick = nil
		end
		local SrcRt = CFRect:new()
		self:GetWndRectBak(SrcRt)
		local DesRt = CFRect:new()
		g_GameMain.m_tblFunctionBar[1].m_tblFunctionBarBtn[2]:GetWndRect(DesRt)
		DesRt.right = DesRt.left + 32
		DesRt.bottom = DesRt.top + 32
		self:SetShapeEffect(250, SrcRt, DesRt);
		self:SetTransparentObj(250, true)
	end
	if self.m_StartbagShapeTick then
		UnRegisterTick(self.m_StartbagShapeTick)
		self.m_StartbagShapeTick = nil
	end
	if uBigID and uIndex then
		local SmallIcon = g_ItemInfoMgr:GetItemInfo(uBigID,uIndex,"SmallIcon")
		g_DelWndImage(self.m_Static, -1, IP_ENABLE, IP_ENABLE)
		g_LoadIconFromRes(SmallIcon, self.m_Static, -1, IP_ENABLE, IP_ENABLE)	
		local DesRt = CFRect:new()
		self:GetWndRectBak(DesRt)
		local SrcRt		= CFRect:new()
		SrcRt.left		= DesRt.left - 10
		SrcRt.top		= DesRt.top - 10 
		SrcRt.right		= DesRt.right + 10
		SrcRt.bottom	= DesRt.bottom + 10
		self:EndTransparent(false)
		self:SetTransparent(1)
		self:ShowWnd(true)
		self:SetShapeEffect(500, SrcRt, DesRt);	
		self.m_StartbagShapeTick = RegisterTick( "StartbagShapeTick", StartShape, 1500 )
	end
end