gac_gas_require "item/store_room_cfg/StoreRoomCfg"
local g_TongDepotMain = g_TongDepotMain 				--包裹个子范围
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_CDepotSQLMgr = RequireDbBox("GasCDepotSQLMgrDB")
local CDepotMgrBox = CreateDbBox(...)


--【仓库内移动物品】
function CDepotMgrBox.GetNcountItemFromA2B(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos,nCount)
	local nFromType,nFromName,nFromCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nFromDepotID,nFromPage,nFromPos)
	local nToType,nToName,nToCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nToDepotID,nToPage,nToPos)
	if nCount <= 0 then
		return 	0
	end
	if (nFromDepotID == nToDepotID and nFromPage == nToPage and nFromPos == nToPos) then
		--起始位置和目标位置相同，不移动，返回
		return 0
	end
	
	if (nFromCount == nil or nFromCount == 0) then
		--起始物品没有物品，返回
		return 0
	end
	if nCount > nFromCount then
		--取走的数目大于物品的实际数目，返回
		return 0
	end
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( nFromType,nFromName,"FoldLimit" ) or 1

	if (nToCount == nil or nToCount == 0) then  
		--目标格子没有物品
		--超过叠放上限
		if nCount > FoldLimit then
			return -1
		end
	else 
		--第二个格子有物品，判断两格子物品名称类型是否相同
		--判断是否超过叠放上限
		if (nFromType ~= nToType or nFromName ~= nToName) then
			return 1
		else
			if nToCount + nCount > FoldLimit then
				return 2
			end
		end
	end
	--改变该物品位置
	g_CDepotSQLMgr.UpdateNumCDepotItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos,nCount)
	return 3
end

--【物品从起始位置移动到目标位置，其实相当于起始和目标物品交换位置，这里用的是这个思路】
function CDepotMgrBox.ChangeItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
	local nFromType,nFromName,nFromCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nFromDepotID,nFromPage,nFromPos)
	local nToType,nToName,nToCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nToDepotID,nToPage,nToPos)
	if ((nFromCount == nil or nFromCount == 0) and (nToCount == nil or nToCount == 0)) then
		--如果目标位置和起始位置都为空，则返回一个空的table
		return 0
	elseif (nToCount == nil or nToCount == 0) then
		--如果目标格子没有物品，则把起始格子所有物品都移动到目标格子
		g_CDepotSQLMgr.UpdateCDepotItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
	elseif (nFromCount == nil or nFromCount == 0) then
		--如果起始格子没有物品，则把目标格子的移动到起始格子
		g_CDepotSQLMgr.UpdateCDepotItemPos(nToDepotID,nToPage,nToPos,nFromDepotID,nFromPage,nFromPos)
	else
		--目标和起始格子都有物品，则两格子物品交换位置
		--获得目标位置的所有物品id
		local tblToItem = g_CDepotSQLMgr.GetDepotItemIDByPos(nToDepotID,nToPage,nToPos)
		g_CDepotSQLMgr.UpdateCDepotItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
		for i=1,tblToItem:GetRowNum() do
			g_CDepotSQLMgr.UpdateCDepotItemPosByID(nFromDepotID,nFromPage,nFromPos,tblToItem(i,1))
		end
	end
end


function CDepotMgrBox.MoveItem(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
	if (nFromDepotID == nToDepotID and nFromPage == nToPage and nFromPos == nToPos) then
		return 
	end
	local FromSoulNum = g_CDepotSQLMgr.GetDepotSoulNumByPos(nFromDepotID,nFromPage,nFromPos)
	local ToSoulNum = g_CDepotSQLMgr.GetDepotSoulNumByPos(nToDepotID,nToPage,nToPos)
	
	if ToSoulNum > 0 and FromSoulNum ~= ToSoulNum then
		return 63
	end
	
	local nFromType,nFromName,nFromCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nFromDepotID,nFromPage,nFromPos)
	if (nFromCount == nil or nFromCount <= 0) then
		return 
	end
	local nToType,nToName,nToCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nToDepotID,nToPage,nToPos)
	
	if (nToCount == nil or nToCount == 0) then
		--目标位置没有物品，直接把物品从起始位置移动到目标位置
		CDepotMgrBox.ChangeItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
		return {},"Replace"
	else
		local FoldLimit = g_ItemInfoMgr:GetItemInfo( nFromType,nFromName,"FoldLimit" ) or 1
		if (nFromType == nToType and nFromName == nToName) then
			--目标位置和起始位置是相同物品
			local nMoveCount = 0
			if nToCount == FoldLimit then
				--目标格子物品到达叠加上限，则起始位置和目标位置物品直接交换
				CDepotMgrBox.ChangeItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
				return {},"Replace"
			elseif nFromCount + nToCount >  FoldLimit then
				--起始位置和目标位置物品数量和大于此物品叠加上限，则目标位置还能叠加多少个就移动多少个到目标位置，剩下的仍然放在原来的位置；
				nMoveCount = FoldLimit - nToCount
			else
				--起始位置和目标位置的物品数量和没有达到叠加上限，则把起始位置物品都移动到目标位置
				nMoveCount = nFromCount
			end
			CDepotMgrBox.GetNcountItemFromA2B(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos,nMoveCount)
			return {},"Move"
		else
			--不同类，则两位置物品交换
			CDepotMgrBox.ChangeItemPos(nFromDepotID,nFromPage,nFromPos,nToDepotID,nToPage,nToPos)
			return {},"Replace"
		end
	end
end

--【删除某仓库某位置上的所有物品】
function CDepotMgrBox.DeleteItemByPos(nDepotID,nPage,nPos)
	--得到该位置所有物品id
	local tblItemInfo = g_CDepotSQLMgr.GetDepotItemIDByPos(nDepotID,nPage,nPos)
	
	--删除该位置上的所有物品
	--为了避免在删除的过程中有物品已被删除，就根据id来删除而不是根据位置来删除
	local nCount = tblItemInfo:GetRowNum()
	local tblItem = {}
	for i=1, nCount do
		g_CDepotSQLMgr.DelCDepotItemByID(tblItemInfo(i,1))
		if (g_DbChannelMgr:LastAffectedRowNum() > 0)  then
			--如果g_DbChannelMgr:LastAffectedRowNum() == 0，说明已经被他人删除
  			table.insert(tblItem, tblItemInfo(i,1))
  		end
	end
	--返回所有被删除物品id
	return tblItem
end

function CDepotMgrBox.DelItem(nDepotID,nBigID,nIndex,nCount)	
	if nCount <= 0 then
		return 	1
	end

	if not g_ItemInfoMgr:CheckType( nBigID,nIndex ) then
		return 2
	end
	
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( nBigID,nIndex,"FoldLimit" ) or 1
	local RoomPosCount = g_CDepotSQLMgr.GetPosCountByType(nDepotID,nBigID,nIndex)
	local nSum = 0
	local AllItem,DelRet = {},{}
	
	--计算有多少个该物品
	if RoomPosCount:GetRowNum() > 0 then
		for n=1, RoomPosCount:GetRowNum() do
			nSum = RoomPosCount(n,3) + nSum
		end
	end
	
	if nSum < nCount then
		return 3
	end
	
  nSum = nCount
	for n=1, RoomPosCount:GetRowNum() do
		local nRoom,nPos,nFoldCount = RoomPosCount(n,1),RoomPosCount(n,2),RoomPosCount(n,3)
		if nSum <= 0 then
			break
		elseif nSum - nFoldCount >= 0 then
			nSum = nSum - nFoldCount
			DelRet = CDepotMgrBox.DeleteItemByPos(nDepotID,nRoom,nPos,nFoldCount)
		else
			DelRet = CDepotMgrBox.DeleteItemByPos(nDepotID,nRoom,nPos,nSum)
			nFoldCount = nSum
			nSum = 0
		end
		if IsNumber(DelRet) then
			return DelRet
		end
	end
	
	--返回被删除的物品信息
	return AllItem
end
--【添加某数量的某物品到某仓库的某位置,主要用于从个人包裹托物品到仓库某位置】
function CDepotMgrBox.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nPage,nPos,tbl_ItemIDs)
	if g_ItemInfoMgr:IsSoulPearl(ItemType) then
		ItemName = g_ItemInfoMgr:GetSoulpearlInfo(ItemName)
	end
	local nCount=table.getn(tbl_ItemIDs)
	if nCount <= 0 then
		return 40
	end
	if not g_ItemInfoMgr:CheckType( ItemType,ItemName ) then
		return 44
	end
	
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"FoldLimit" ) or 1
	local nToType, nToName, nToCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nDepotID,nPage,nPos)
	
	--判断会不会超过叠加上限
	if nToCount + nCount > FoldLimit then
		return 42
	end
	
	-- 如果目标位置有物品，要判断类型是否相同
	if nToCount ~= 0 then
		if (nToName ~= ItemName or tonumber(nToType) ~= tonumber(ItemType)) then
			return 60
		end
	end
	
	local AllItem = {}
	local g_RoomMgr = RequireDbBox("GasRoomMgrDB")
	local SoulNum = g_CDepotSQLMgr.GetDepotSoulNumByPos(nDepotID,nPage, nPos)
	for n=1, nCount do
		local item_id = tbl_ItemIDs[n]
		if SoulNum > 0 and SoulNum ~= g_RoomMgr.GetSoulPearlInfoByID(item_id) then
			return 63
		end
		if not g_CDepotSQLMgr.AddCDepotItem(nDepotID, item_id, nPage, nPos) then
			CancelTran()
			return 57
		end
		table.insert(AllItem,tbl_ItemIDs[n])
	end
	return AllItem
end

function CDepotMgrBox.HaveEnoughRoomPos(nDepotID,ItemType,ItemName,nPage,nCount)
	if g_ItemInfoMgr:IsSoulPearl(ItemType) then
		ItemName = g_ItemInfoMgr:GetSoulpearlInfo(ItemName)
	end
	local FoldLimit = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"FoldLimit" ) or 1
	
	--查询该仓库所有有该物品且没有达到叠加上限的物品所在页数、所在格子、此位置上的数量
	local HaveItemPosInfo
	if nPage then
		HaveItemPosInfo = g_CDepotSQLMgr.GetPosCountByPage(nDepotID, ItemType, ItemName, FoldLimit,nPage)
	else
	 HaveItemPosInfo = g_CDepotSQLMgr.GetPosCountByNameAndType(nDepotID, ItemType, ItemName, FoldLimit)
	end
	
	local nSum,OtherNeedGrid= 0,0
	
	--计算需要空的格子数
	if HaveItemPosInfo:GetRowNum() == 0 then
		--说明该空间还没有未达到叠加上限的物品
		OtherNeedGrid = math.ceil(nCount / FoldLimit)
	else
		for n=1, HaveItemPosInfo:GetRowNum() do
			--计算已经有该物品的位置还能叠放多少该物品
			nSum = FoldLimit - HaveItemPosInfo(n,3) + nSum
		end
		if nSum - nCount < 0 then
			--还能叠放的物品数量小于要叠放的物品数量
			OtherNeedGrid = math.ceil((nCount - nSum) / FoldLimit)
		else
			OtherNeedGrid = 0
		end
	end
	
	nSum = nCount
	local tblRoomPos,tblBagGridNum,UseEmptyRoom = {},{},{}

	--OtherNeedGrid 不为0说明还需要空的格子来放物品
	if OtherNeedGrid ~= 0 then
		--根据仓库id查询该仓库已经有物品的格子信息
		local HaveItemRoomCount
		if nPage then
			HaveItemRoomCount = g_CDepotSQLMgr.GetPosWhichHaveItemByPage(nDepotID,nPage)
		else
			HaveItemRoomCount = g_CDepotSQLMgr.GetPosWhichHaveItem(nDepotID)
		end
		--把所有已经有物品的的格子信息放到一个三维表里
		--第一维萨页数，
		--第二维是位置，
		--第三维应该是该位置上的所有物品信息；
		for n=1, HaveItemRoomCount:GetRowNum() do
			if tblRoomPos[HaveItemRoomCount(n,1)] == nil then
				tblRoomPos[HaveItemRoomCount(n,1)] = {}
			end	
			tblRoomPos[HaveItemRoomCount(n,1)][HaveItemRoomCount(n,2)] = {}
		end
		
		--获得该仓库的所有页数和格子数
		-- 数量暂定
		local nPageIndexNum = 5
		local nGridIndexNum = g_TongDepotMain.size
		
		--从该仓库的第一页的第一个格子开始查找，如果为nil，说明该格子没有被占用过，
		--如果没有被占用过，则把该格子信息放到UseEmptyRoom这张表里；
		--当UseEmptyRoom这张表长度大于需要的空格子数时，说明空格子已经找够，用break退出循环；
		if nPage then
			for j=1, nGridIndexNum do
				--如果在有物品的结果集中找不到，说明这个格子没有使用过
				if tblRoomPos[nPage] == nil or tblRoomPos[nPage][j] == nil then 
					--插入可用的空的格子位置
					table.insert(UseEmptyRoom,{nPage,j}) 
					--找到足够的空格子，返回
					if #UseEmptyRoom >= OtherNeedGrid then 
						break
					end
				end
			end
		else
			for i=1, nPageIndexNum do
				if #UseEmptyRoom >= OtherNeedGrid then 
					break
				end
				for j=1, nGridIndexNum do
					--如果在有物品的结果集中找不到，说明这个格子没有使用过
					if tblRoomPos[i] == nil or tblRoomPos[i][j] == nil then 
						--插入可用的空的格子位置
						table.insert(UseEmptyRoom,{i,j}) 
						--找到足够的空格子，返回
						if #UseEmptyRoom >= OtherNeedGrid then 
							break
						end
					end
				end
			end
		end
	end
	
	return #UseEmptyRoom == OtherNeedGrid
end

--【自动添加物品到某仓库】
--自动添加物品
--nPage可以为nil，为nil说明自动添加到仓库中某一页的某个格子
--先判断有可以叠加的位置
--再计算还需要多少空的格子来放
function CDepotMgrBox.AutoAddItems(nDepotID,ItemType,ItemName,itemids,nPage)
	local nCount=table.getn(itemids)
	if nCount <= 0 then
		return 1
	end
	local soul_count = 0
	if g_ItemInfoMgr:IsSoulPearl(ItemType) then
		ItemName,soul_count = g_ItemInfoMgr:GetSoulpearlInfo(ItemName)
	end
	if not g_ItemInfoMgr:CheckType( ItemType,ItemName ) then
		return 2
	end

	local FoldLimit = g_ItemInfoMgr:GetItemInfo( ItemType,ItemName,"FoldLimit" ) or 1
	
	local HaveItemPosInfo
	if nPage then
		HaveItemPosInfo = g_CDepotSQLMgr.GetPosCountByPage(nDepotID, ItemType, ItemName, FoldLimit,nPage)
	else
	 HaveItemPosInfo = g_CDepotSQLMgr.GetPosCountByNameAndType(nDepotID, ItemType, ItemName, FoldLimit)
	end
	
	local nSum,OtherNeedGrid= 0,0
	--计算需要空的格子数
	if HaveItemPosInfo:GetRowNum() == 0 then
		--说明该空间还没有未达到叠加上限的物品
		OtherNeedGrid = math.ceil(nCount / FoldLimit)
	else
		for n=1, HaveItemPosInfo:GetRowNum() do
			--计算已经有该物品的位置还能叠放多少该物品
			if soul_count == g_CDepotSQLMgr.GetDepotSoulNumByPos(nDepotID,HaveItemPosInfo(n,1), HaveItemPosInfo(n,2)) then
				--不是魂珠，或是魂珠且魂值相同
				nSum = FoldLimit - HaveItemPosInfo(n,3) + nSum
			end
		end
		if nSum - nCount < 0 then
			--还能叠放的物品数量小于要叠放的物品数量
			OtherNeedGrid = math.ceil((nCount - nSum) / FoldLimit)
		else
			OtherNeedGrid = 0
		end
	end
	
	nSum = nCount
	local tblRoomPos,tblBagGridNum,UseEmptyRoom = {},{},{}

	--OtherNeedGrid 不为0说明还需要空的格子来放物品
	if OtherNeedGrid ~= 0 then
		local HaveItemRoomCount
		if nPage then
			HaveItemRoomCount = g_CDepotSQLMgr.GetPosWhichHaveItemByPage(nDepotID,nPage)
		else
			HaveItemRoomCount = g_CDepotSQLMgr.GetPosWhichHaveItem(nDepotID)
		end
		for n=1, HaveItemRoomCount:GetRowNum() do
			if tblRoomPos[HaveItemRoomCount(n,1)] == nil then
				tblRoomPos[HaveItemRoomCount(n,1)] = {}
			end	
			tblRoomPos[HaveItemRoomCount(n,1)][HaveItemRoomCount(n,2)] = {}
		end
		
		local nPageIndexNum = 5
		local nGridIndexNum = g_TongDepotMain.size
		if nPage then
			for j=1, nGridIndexNum do
				if tblRoomPos[nPage] == nil or tblRoomPos[nPage][j] == nil then 
					table.insert(UseEmptyRoom,{nPage,j}) 
					if #UseEmptyRoom >= OtherNeedGrid then 
						break
					end
				end
			end
		else
			for i=1, nPageIndexNum do
				if #UseEmptyRoom >= OtherNeedGrid then 
					break
				end
				for j=1, nGridIndexNum do
					if tblRoomPos[i] == nil or tblRoomPos[i][j] == nil then 
						table.insert(UseEmptyRoom,{i,j}) 
						if #UseEmptyRoom >= OtherNeedGrid then 
							break
						end
					end
				end
			end
		end
	end
	
	if #UseEmptyRoom ~= OtherNeedGrid then
		--没有足够的空间，返回
		return 3 
	end
	local start_index=1
	local AllItem,SetResult={},{}
	--添加物品，先在同类物品的格子上添加，如果添加完了就直接返回
	for n=1, HaveItemPosInfo:GetRowNum() do
		local nPage,nPos,nFoldCount = HaveItemPosInfo(n,1),HaveItemPosInfo(n,2),HaveItemPosInfo(n,3)
		if soul_count == g_CDepotSQLMgr.GetDepotSoulNumByPos(nDepotID,nPage,nPos) then
				if nSum <= 0 then
					return AllItem
				elseif nSum - (FoldLimit - nFoldCount) >= 0 then
					nSum = nSum - (FoldLimit - nFoldCount)
					local tbl_items={}
					for i=start_index,start_index+FoldLimit - nFoldCount-1 do
						table.insert(tbl_items,itemids[i])
					end
					start_index=start_index+FoldLimit - nFoldCount
					if table.getn(tbl_items)>0 then
						SetResult=CDepotMgrBox.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nPage,nPos,tbl_items)
					end
					nFoldCount = FoldLimit - nFoldCount
				else
					local tbl_items={}
					for i=start_index,start_index+nSum-1 do
						table.insert(tbl_items,itemids[i])
					end
					if table.getn(tbl_items)>0 then
						SetResult=CDepotMgrBox.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nPage,nPos,tbl_items)
					end
					nFoldCount = nSum
					nSum = 0
				end
				if IsNumber(SetResult) then
					--SetResult为数字说明添加失败
					CancelTran()
					return SetResult
				end
		end
	end
	--在空的格子上添加
	for n=1, OtherNeedGrid do
		local nPage,nPos,nFoldCount = UseEmptyRoom[n][1],UseEmptyRoom[n][2],0
		if nSum <= 0 then
			return AllItem
		elseif nSum - FoldLimit  >= 0 then
			nSum = nSum - FoldLimit
			local tbl_items={}
			for i=start_index,start_index+FoldLimit-1  do
				table.insert(tbl_items,itemids[i])
			end
			start_index=start_index+FoldLimit - nFoldCount
			SetResult=CDepotMgrBox.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nPage,nPos,tbl_items)
			nFoldCount = FoldLimit
		else
			local tbl_items={}
			for i=start_index,start_index+nSum-1 do
				table.insert(tbl_items,itemids[i])
			end
			if table.getn(tbl_items)>0 then
				SetResult=CDepotMgrBox.AddCDepotItemByPos(nDepotID, ItemType,ItemName,nPage,nPos,tbl_items)
			end
			nFoldCount = nSum
			nSum = 0
		end
		if IsNumber(SetResult) then
			--SetResult为数字说明添加失败
			CancelTran()
			return SetResult
		end
	end
	return SetResult
end

--【整理仓库物品】
function CDepotMgrBox.ResetCDepotItem(nDepotID)
	local res = g_CDepotSQLMgr.GetAllCDepotItemInfo(nDepotID)
	if res:GetRowNum() == 0 then
		return
	end
	local tbl_res = {}
	for i =1, #res do
		--分别按类型、页数、位置、物品id、名称的顺序插入到tbl_res表中
		table.insert(tbl_res,{res(i,1),res(i,3),res(i,4),res(i,5),res(i,2)}) --把pos设置为1000 ,包裹不可能这么大,只是为了重新设置时比较
	end
	--按类型从小到大排序，如果类型相同，再按名称排
	table.sort(tbl_res, function (a, b)
			if (a[1] < b[1]) then
				return a[1] < b[1]
			elseif(a[1] == b[1]) then
				return string.lower(a[5]) < string.lower(b[5])
			end
		end)
	local pos = 1
	local PageSize  = g_TongDepotMain.size  --格子数，暂定50
	local PageIndex = 1		--从第一页开始
	for j = 1, #(tbl_res) do
		--得到该仓库该位置上物品的数量
		local sType, sName, nHaveCount = g_CDepotSQLMgr.GetDepotItemTypeNameAndCountByPos(nDepotID,PageIndex,pos)
		local FoldLimit = g_ItemInfoMgr:GetItemInfo( sType, sName,"FoldLimit" ) or 1
		if nHaveCount >= FoldLimit then
			--达到叠加上限，从下一个位置开始放
			pos = pos + 1
		elseif (0 < nHaveCount) then
			--有物品且没有达到叠加上限时
			--nHaveCount为0，说明该位置还没有物品，可以放，pos不用加1
			if( sName ~= tbl_res[j][5] or sType ~= tbl_res[j][1])then
				--此格子上的物品和要放的物品不同
				pos = pos + 1
			end
		end
		if(pos > PageSize)then
			--pos已经达到本页格子上限，转到下一页的第一个格子开始添加
			PageIndex = PageIndex + 1
			pos = 1
		end
		--得到该物品应该放的位置后，修改数据库
		g_CDepotSQLMgr.UpdateCDepotItemPosByID(PageIndex,pos,tbl_res[j][4])
	end
end


---------------------------
return CDepotMgrBox