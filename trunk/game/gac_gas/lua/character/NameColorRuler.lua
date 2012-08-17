gac_gas_require "character/NameColorDef"
gac_gas_require "character/NameColorRulerInc"

--[[
	ECharNameColor.eCNC_Blue			= 1
	ECharNameColor.eCNC_Red			= 2
	ECharNameColor.eCNC_Purple		= 3
	ECharNameColor.eCNC_Green		= 4
]]

-- AChar 被 SeeByThisChar 看起来是什么颜色
function CNameColorRuler:GetColorSeeBy( AChar, SeeByThisChar )

-- PK 相关颜色
	if( AChar.IsRedName and AChar:IsRedName()  ) then
		return ECharNameColor.eCNC_Red 
	end
	
-- PVP 相关颜色
--[[
	if ( g_PVPMgr:IsInPVPCase( AChar ) ) then
		-- 是否同阵营
		if( g_PVPMgr:SameRegime( AChar,  SeeByThisChar ) ) then
			return ECharNameColor.eCNC_Green
		else
			return ECharNameColor.eCNC_Purple
		end	
	end
]]	

-- Default: 正常情况下: 默认值 蓝色
	return ECharNameColor.eCNC_Blue
end