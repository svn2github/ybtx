gac_gas_require "item/item_info_mgr/ItemInfoMgr"
gac_gas_require "liveskill/LiveSkillMgr"
gac_gas_require "framework/common/CMoney"
gac_gas_require "relation/tong/TongPurviewMgr"
gac_require "information/tooltips/ToolTips"
gac_require "setting/system_setting/UISettingMgr"
gac_require "framework/display_common/TimeTrans"
gac_require "framework/texture_mgr/DynImageMgr"
gac_require "framework/display_common/DisplayCommon"
gac_require "setting/function_area/ExcludeWnd"
gac_require "framework/url_mgr/UrlMgr"
--这个文件里放置所有全局变量

g_ItemInfoMgr = CItemInfoMgr:new()

-- 初始化依赖 g_ItemInfoMgr 之后
g_Tooltips = CTooltips:new()
	
-- 放置有动态的物品信息的表，依据数据库的id为索引
g_DynItemInfoMgr = CDynItemInfoMgr.Init()

-- Image映射表
g_ImageMgr = CImageMgr:new()

g_LiveSkillMgr = g_LiveSkillMgr or CLiveSkillMgr:new()

g_UISettingMgr		= CUISettingMgr:new()
g_CTimeTransMgr		= CTimeTrans:new()
g_DynImageMgr		= CDynImageMgr:new()
g_MoneyMgr			= CMoney:new()
g_UISettingMgr		= CUISettingMgr:new()
g_CTimeTransMgr		= CTimeTrans:new()
g_DisplayCommon		= CDisplayCommon:new()
g_TongPurviewMgr	= CTongPurviewInfoMgr:new()
g_ExcludeWndMgr		= CShowOpendWnd:new()
g_UrlMgr			= CUrlMgr:new()
