gac_require "framework/color_mgr/ColorMgrInc"
cfg_load "color/Color_Common"

function CColorMgr:GetColor(ColorIndex)
	assert(Color_Common(ColorIndex),ColorIndex .. "字段在Color_Common配置表不存在")
	local colorValue = Color_Common(ColorIndex,"ColorValue")
	
	colorValue = string.sub(colorValue,2,-2)
	colorValue = "#c" .. colorValue 
	return colorValue
end

function CColorMgr:Get16Color(ColorIndex)
	assert(Color_Common(ColorIndex),ColorIndex .. "字段在Color_Common配置表不存在")
	local colorValue = Color_Common(ColorIndex,"ColorValue")
	
	colorValue = string.sub(colorValue,2,-2)
	colorValue = "0xff" .. colorValue 
	return colorValue
end

-- 颜色映射表
g_ColorMgr = CColorMgr:new()