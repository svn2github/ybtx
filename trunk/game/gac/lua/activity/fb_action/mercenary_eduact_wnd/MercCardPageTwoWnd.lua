lan_load "fb_game/Lan_MercCardTooltip_Common"
cfg_load "fb_game/MercCardTooltip_Common"

CMercCardPageTwoWnd = class(SQRDialog)
function CMercCardPageTwoWnd:Ctor(parent)
	self:CreateFromRes("Fb_MercCardPageWnd",parent) 
	self:ShowWnd(false)	
end