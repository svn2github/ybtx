gac_require "information/tooltips/ToolTipsInc"
gac_gas_require "relation/tong/TongTechnologyMgr"
gac_gas_require "framework/common/CMoney"
lan_load "tong/Lan_TongFightTech_Common"

------佣兵团战斗科技Tooltips------
function CTooltips:GetTongFightTechTips(sName, nLevel)
	local nNextLevel	= nLevel + 1
	local sCurEffect	= ""
	if(nLevel > 0) then
		sCurEffect = ToolTipsText( 802, Lan_TongFightTech_Common(MemH64(sName .. nLevel), "Description") ) --#r科技效果:%s
	end
	
	local strParameter, Color, Tips = "","",{}
	if( g_TongTechMgr:HasFightTechInfo(sName, nLevel + 1) ) then
		local displayName	= Lan_TongFightTech_Common(MemH64(sName .. nNextLevel), "DisplayName")
		local needTonglevel	= g_TongTechMgr:GetFightTechNeedTongLevel(sName, nNextLevel)
		local stationLine	= g_TongTechMgr:GetFightTechStationLine(sName, nNextLevel)
		local nextEffect	= Lan_TongFightTech_Common(MemH64(sName .. nLevel + 1), "Description")
		--%s#r等级:%d%s#r升级到下一级需要:#r佣兵团等级:%d#r佣兵团战线:%d#r下一级科技效果:%s
		strParameter = ToolTipsText(801, displayName, nLevel, sCurEffect, needTonglevel, stationLine, nextEffect)
	else
		local displayName	= Lan_TongFightTech_Common(MemH64(sName .. nLevel), "DisplayName")
		strParameter = displayName .. sCurEffect
	end
	
	self:SetFirstTipsInfo( Tips, Color, strParameter, 0 )
	return self:Completed( Tips )
end

function CTooltips:GetTongProdItemTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
	self:GetMatrialCommonTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
end

function CTooltips:GetTongChallengeItemTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
	self:GetMatrialCommonTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
end

function CTooltips:GetTongItemTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
	self:GetTongItemCommonTips(Tips, nBigID, nIndex, nItemID, foldCount, payType,GoldType)
end