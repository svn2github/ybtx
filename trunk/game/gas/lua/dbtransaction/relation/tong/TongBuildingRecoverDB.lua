local mail_event = event_type_tbl["gm²¹³¥"]
local g_ItemInfoMgr = CItemInfoMgr:new()
local g_RoomMgr = RequireDbBox("GasRoomMgrDB")

local CTongBuildingRecover = CreateDbBox(...)
local StmtOperater = {}	


local StmtDef = {
    	"GetTongBuilding",
    	[[ 
				select lts_uId,ltbe_sName,number from dd_test.tong_building
     	]]
}
DefineSql ( StmtDef, StmtOperater )


local StmtDef = {
    	"_GetTongNameById",
    	[[ 
    		select t_sName from tbl_tong  
    		where t_uId = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_GetLeaderByTongID",
    	[[
    		 select  cs_uId  from tbl_member_tong  where mt_sPosition = ? and t_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddTongBuilding",
    	[[
    		 insert into tbl_building_tong(t_uId,bt_sBuildName,bt_uStep,bt_uPosX,bt_uPosY,bt_uState,bt_uLevel)
    		 	values(?,?,1,0,0,3,0)
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

local StmtDef = {
    	"_AddTongBuildingLife",
    	[[
    		 insert into tbl_building_life(bt_uId,bl_uLife,bl_dtLastTime)
    		 	values(?,1,now())
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

function CTongBuildingRecover.Recover()
	local sysMailExecutor = RequireDbBox("SysMailDB")

	local Ret = StmtOperater.GetTongBuilding:ExecStat()
	local nTongItemBigID = g_ItemInfoMgr:GetTongItemBigID()

	for i=1, Ret:GetRowNum() do
		local tongid = tonumber(Ret(i,1))
		local building_name = Ret(i,2)
		local number = tonumber(Ret(i,3))

		local tong_leader_id = StmtOperater._GetLeaderByTongID:ExecStat(5,tongid)

		if (tong_leader_id:GetRowNum() ~= 0) then
			tong_leader_id = tong_leader_id(1,1)
			local params = {}
				params.m_nType = nTongItemBigID
				params.m_sName = building_name
				params.m_nBindingType = g_ItemInfoMgr:GetItemInfo(nTongItemBigID, building_name, "BindingStyle") or 0
				params.m_nCharID = tong_leader_id
				params.m_sCreateType = mail_event
							
			local ItemTbl = {}
			for j=1, number do
				local itemID = g_RoomMgr.CreateItem(params)
				table.insert(ItemTbl, {itemID})
				StmtOperater._AddTongBuilding:ExecStat(tongid,building_name)
				local building_id = g_DbChannelMgr:LastInsertId()
				StmtOperater._AddTongBuildingLife:ExecStat(building_id)
			end

			if ItemTbl then
				sysMailExecutor.SendSysMail("1014", tong_leader_id, "1014", "", ItemTbl, 0, mail_event)
			end
		end
	end
end

return CTongBuildingRecover