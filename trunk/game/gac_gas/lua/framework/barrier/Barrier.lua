--获得 GridPos 附近一个非障碍的格子坐标
--返回值为CFPos类型
--采用螺旋方式遍历周围格子
function g_GetNearNoBarrierPos(Scene, srcPos)
	assert(srcPos,"坐标不能为空！！！！")
	local Pos = CFPos:new()
	Pos.x = srcPos.x
	Pos.y = srcPos.y
	local  CoreScene = Scene
	local GridPos = CPos:new()
	GridPos.x = Pos.x / EUnits.eGridSpanForObj
	GridPos.y = Pos.y / EUnits.eGridSpanForObj
	local barrierType = CoreScene:GetBarrier(GridPos)
	if barrierType ==  EBarrierType.eBT_NoBarrier then
		return Pos
	end
	
	local Length = 1  --边长,每转两次增加1
	local Dir = 1 --方向, 1:上(y++)  2:右(x++) 3:下(y--) 4:左(x--)
	local OffsetX = 0
	local OffsetY = 0
	local SearchCount =  0
	while SearchCount < 20 do  --最多20次再大的话没多大意思()
		SearchCount = SearchCount + 1
		if Dir == 1 then
			OffsetX = 0
			OffsetY = 1
		elseif Dir == 2 then
			OffsetX = 1
			OffsetY = 0
		elseif Dir == 3 then
			OffsetX = 0
			OffsetY = -1
		elseif Dir == 4 then
			OffsetX = -1
			OffsetY = 0
		end
		for i = 1, Length do
			GridPos.x = GridPos.x + OffsetX
			GridPos.y = GridPos.y + OffsetY
			barrierType = CoreScene:GetBarrier(GridPos)
			if barrierType ==  EBarrierType.eBT_NoBarrier then
				Pos.x = (GridPos.x + 0.5 ) * EUnits.eGridSpanForObj
				Pos.y = (GridPos.y + 0.5 ) * EUnits.eGridSpanForObj
				return Pos
			end
		end
		Length = Length + 0.5
		Dir = Dir == 4 and 1 or Dir + 1
	end
	return Pos
end
