lan_load "fb_game/Lan_MercCardTooltip_Common"
cfg_load "fb_game/MercCardTooltip_Common"

CMercCardWnd = class(SQRDialog)

local MercCardLine 			= 5			-- 5行
local MercCardNum 			= 10 		-- 每行10列
local MercCardPage 			= 2			-- 页数
local MercCardWeightInv = 15 		-- 间距
local MercCardHeightInv = 16
local NowPageWnd 				= nil		-- 当前页面窗口
local NowPageNum				= 1 		-- 当前页数
local RecordMecCardTbl	= {}

function CMercCardWnd:Ctor(Parent, Number)
	self:CreateFromRes("Fb_MercCardWnd",Parent) 
	self.m_ItemIcon = self:GetDlgChild("Image")
	g_LoadIconFromRes(MercCardTooltip_Common(Number, "SmallIcon"), self.m_ItemIcon, -1, IP_ENABLE, IP_ENABLE)
	
	local image = WND_IMAGE_LIST:new()
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP_ENABLE		
	local DesPocStr = MercCardTooltip_Common(Number, "DesPicStr")
	image:AddImageWin( self.m_ItemIcon:GetGraphic(), -1,  DesPocStr, nil, CFPos:new(), "0xffffffff", 0 )

	self.m_ItemIcon:SetMouseOverRichTextImageForAllWnd(815, image)
	local str = "#" .. 815 .. Lan_MercCardTooltip_Common(Number, "Description")
	self.m_ItemIcon:SetMouseOverDescAfter(str)
	self:ShowWnd(false)
end