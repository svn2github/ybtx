local StmtOperater = {}
local CTongOtherBox = CreateDbBox(...)

----------------------------------------sql相关---------------------------------------------------------------------
--【增加军资,资源点玩家相关信息】
local StmtDef = 
{
	"_addCharData",
	"insert into tbl_tong_char_other(cs_uId,tco_uTypeId,tco_uNum) values(?,?,?)"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"_getCharData",
	"select tco_uNum from tbl_tong_char_other where tco_uTypeId = ? and cs_uId = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"_getAllCharData",
	"select cs_uId, tco_uNum from tbl_tong_char_other where tco_uTypeId = ?"
}
DefineSql(StmtDef, StmtOperater)

--【更新军资,资源点玩家相关信息】
local StmtDef = 
{
	"_updateCharData",
	"update tbl_tong_char_other set tco_uNum = tco_uNum + ? where cs_uId = ? and tco_uTypeId = ?"
}
DefineSql(StmtDef, StmtOperater)

--【增加资源点小队相关信息】
local StmtDef = 
{
	"_addTongData",
	"replace into tbl_tong_other(t_uId,to_uNum) values(?,?)"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"_getTongData",
	"select to_uNum from tbl_tong_other where t_uId = ?"
}
DefineSql(StmtDef, StmtOperater)

local StmtDef = 
{
	"_getAllTongData",
	"select t_uId, to_uNum from tbl_tong_other "
}
DefineSql(StmtDef, StmtOperater)

--【更新资源点小队相关信息】
local StmtDef = 
{
	"_updateTongData",
	"update tbl_tong_other set to_uNum = to_uNum + 1 where t_uId = ?"
}
DefineSql(StmtDef, StmtOperater)


--【增加军资,资源点玩家相关信息】
function CTongOtherBox.AddCharData(data)   --1为军资,2为击杀
	local type = data["type"]
	local charId = data["charId"]
	local nAddData = data["nAddResource"]
	local res = StmtOperater._getCharData:ExecStat(type, charId)
	if type == 2 then
		nAddData = 1
	end
	if res:GetRowNum() <= 0 then
		StmtOperater._addCharData:ExecStat(charId, type, nAddData)
		return 
	end
	StmtOperater._updateCharData:ExecStat(nAddData, charId, type)
	
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran()
		return
	end
end

function CTongOtherBox.GetAllCharData(data)
	local type = data["type"]
	local resTbl = {}
	local res = StmtOperater._getAllCharData:ExecStat(type)
	if res:GetRowNum() > 0 then
		for i = 0, res:GetRowNum() - 1 do
			local charId = res:GetData(i,0)
			local num = res:GetData(i,1)
			table.insert(resTbl,{charId, num})
		end
	end
	return resTbl
end

--【增加资源点小队相关信息】
function CTongOtherBox.AddTongData(data)
	local tongId = data["tongId"]
	local res = StmtOperater._getTongData:ExecStat(tongId)
	if res:GetRowNum() <= 0 then
		StmtOperater._addTongData:ExecStat(tongId, 1)
		return 
	end
	StmtOperater._updateTongData:ExecStat(tongId)
	if not (g_DbChannelMgr:LastAffectedRowNum()>0)  then
		CancelTran()
		return
	end
end

function CTongOtherBox.GetAllTongData(data)
	local res = StmtOperater._getAllTongData:ExecStat(type)
	if res:GetRowNum() > 0 then
		for i = 0, res:GetRowNum() - 1 do
			local tongId = res:GetData(i,0)
			local num = res:GetData(i,1)
			table.insert(resTbl,{tongId, num})
		end
	end
	return resTbl
end

return CTongOtherBox