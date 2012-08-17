-------------------------------------------
local StmtOperater = {}		--用来挂sql语句的table


local GasAppTeamListBox = CreateDbBox(...)


------------------------------------------相关sql---------------------------------------------------

--查询地下城    申请人的信息
local StmtDef ={
			"_selectAppTeamList",
		  [[
		 select 
		  	cs.cs_uId,c.c_sName,cs.cs_uCamp,cs.cs_uClass,cb.cb_uLevel,tl.tl_nType,tl.tl_sMsg,unix_timestamp(now())-unix_timestamp(tl.tl_dtTime)
		  from 
		  	tbl_char_static as cs,
		  	tbl_char_basic as cb,
		  	tbl_char as c,
		  	tbl_char_online co,
		  	tbl_team_list as tl
		  where 
		  	cs.cs_uId = tl.cs_uId and cb.cs_uId = tl.cs_uId and c.cs_uId = tl.cs_uId and co.cs_uId = tl.cs_uId
		  	and tl.cs_uId not in(select cs_uId from tbl_member_team) and unix_timestamp(now())-unix_timestamp(tl.tl_dtTime) < 14400 and tl.tl_nType =? 
		  ]]
}
DefineSql ( StmtDef, StmtOperater )

--插入地下城    申请人信息
local StmtDef = {
    	"_insertAppTeamList",
    	[[ 
    						insert into tbl_team_list(cs_uId,tl_nType,tl_sMsg,tl_dtTime) value(?,?,?,now())
    	]]
}    
DefineSql ( StmtDef, StmtOperater )

--删除申请人信息（”申请人取消排队“）
local StmtDef = {
    	"_deleteAppTeamList",
    	[[ 
    						delete from tbl_team_list where cs_uId = ? and  tl_nType = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--更新排队信息
local StmtDef = {
    	"_updateAppTeamList",
    	[[ 
    						update tbl_team_list set tl_dtTime = now(),tl_sMsg = ? where cs_uId = ? and  tl_nType = ?
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--用于判断 玩家是否在 数据库中
local StmtDef = {
    	"_judgeAppTeamList",
    	[[ 
    						select tl_nType,tl_dtTime tl_ from tbl_team_list where cs_uId = ? and  tl_nType = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


--用于判断 玩家是否在 数据库中  上线时候刷新
local StmtDef = {
    	"_judgeFreshAppTeamList",
    	[[ 
    						select tl_nType,unix_timestamp(now())-unix_timestamp(tl_dtTime) tl_ from tbl_team_list where cs_uId = ? 
    	]]
}    
DefineSql ( StmtDef, StmtOperater )


-------------------------------------------数据库操作相关部分------------------------------------------

--查询地下城     申请组队列表 
function  GasAppTeamListBox.inquireAppTeamList(parameters)
	local result = StmtOperater._selectAppTeamList:ExecStat(parameters.nType)
	local row = result:GetRowNum()
	local AppTeamList = {}
		for i=1,row do							
			table.insert(AppTeamList,{result(i,1),result(i,2),result(i,3),result(i,4),result(i,5),result(i,6),result(i,7),result(i,8)})
		end
	return AppTeamList
end

--地下城 插入报名组队玩家信息   cs_uId,tl_type,tl_state
function GasAppTeamListBox.addAppTeamList(parameters)
			local result = StmtOperater._judgeAppTeamList:ExecStat(parameters.uCharID,parameters.nType)
			local row = result:GetRowNum()
		if 0 == row then
			StmtOperater._insertAppTeamList:ExecStat(parameters.uCharID,parameters.nType,parameters.sMsg)
			if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
	  		error ("插入玩家信息失败")
	  	else
  			return true
			end
		else
			StmtOperater._updateAppTeamList:ExecStat(parameters.sMsg,parameters.uCharID,parameters.nType)
			if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
	  		error ("插入玩家信息失败")
	  	else
  			return true
			end
		end
end

--删除取消排队玩家信息
function GasAppTeamListBox.removeAppTeamList(parameters)
		 	 StmtOperater._deleteAppTeamList:ExecStat(parameters.uCharID,parameters.nType)
		  if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
 		 	 error ("删除排队玩家失败")
 	    else
  	return true 
  end
end


function GasAppTeamListBox.judgeAppTeamList(uCharID)
				local result = StmtOperater._judgeFreshAppTeamList:ExecStat(uCharID)
				local row = result:GetRowNum()
				local onList = {}
				for i=1 ,row do 
					if result(i,2) < 14400 then
						table.insert(onList,{result(i,1),result(i,2)})
					end
				end
				return onList
end
------------------闭包结束
return GasAppTeamListBox