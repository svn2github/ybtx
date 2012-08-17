CScreenMask = class( SQRDialog )

function CScreenMask:Ctor()
	self:CreateFromRes("ScreenMask", g_SelectChar)
	self:SetIsBottom(true)
end