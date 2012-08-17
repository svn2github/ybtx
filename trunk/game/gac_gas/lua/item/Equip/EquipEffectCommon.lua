local EffectLevel = {}
EffectLevel[1] = "level1.res"
EffectLevel[2] = "level2.res"
EffectLevel[3] = "level3.res"
EffectLevel[4] = "level4.res"

--获取特效资源的名称
function GetEffectRes(sItemType, sItemName, uLevel)
	assert(IsNumber(uLevel) and uLevel > 0)
	if (uLevel < 5) then
		return EffectLevel[uLevel]
	end
	
	if uLevel == 5 then
		local EffectLevel5 = g_ItemInfoMgr:GetItemInfo(uBigID,uIndex,"EffectLevel5")
		return EffectLevel5
	end
end