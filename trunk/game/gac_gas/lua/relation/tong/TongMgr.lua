gac_gas_require "relation/tong/TongMgrInc"
cfg_load "tong/TongPurview_Common"
cfg_load "tong/TongLevelAndHonor_Common"
cfg_load "tong/TongFightTechToPlayer_Common"

function CTongMgr:Ctor()
	self:Init()
end

function CTongMgr:Init()
	self.m_nRegistLevel = 20		--注册帮会需要等级
	self.m_nJoinInLevel = 10		--加入帮会需要的等级
	self.m_nRegistMoney = 10000		--帮会创建需要资金，20金
	self.m_nDefaultResource = 0		--初始资源
	self.m_nUpperResource = 18000	--资源上限
	self.m_nDepotContent = 20000	--每个粮草仓资源容量上限
	self.m_nDefaultMoney = 0
	self.m_nDefaultHonor = 0		--初始荣誉
	self.m_nBuildWaitNum = 6		--建造建筑模型时最大允许的等待数
	self.m_nAddProfferByContributeMoney = 10		--捐献金钱奖励的帮贡
	self.m_nInitMember  = 40 --佣兵团初始人数上限
	self.m_nEveryLevelMember = 8 --每级提升的人数
	self.m_nInitBuildNum = 8 --初始建筑数量
	self.m_nEveryLevelBuild = 1 --每级提升建筑数量
	self.m_nRequstTimeLimit = 86400  --加入团时间限制 24*60*60
	self.m_nMaxRequestNum = 50 --申请人数限制	
	self.m_nUpTongMoney = 10000	--提升小队类型需要的金币
	self.m_nUpTongExploit = 10000 --提升为精英战斗小队需要的声誉
	
	self.m_tblBuildingState = 
		{	
			["等待建造"] 	= 1,
			["建造"] 			= 2,
 			["模型"] 			= 3,
 			["正常"] 	= 4,
		}
		
	self.m_tblLogType =
		{	
			["普通"] 	= 0,
			["人员"] 	= 1,
 			["建设"] 	= 2,
 			["战争"] 	= 3,
 			["科技"] 	= 4,
 			["内政"] 	= 5,
 			["仓库"] 	= 6,
		}
		--仓库使用等级
		--字符串对应TongPurview_Common表的字段名称
		--数字对应第几个仓库
	self.m_tblDepotType =
		{	
			[5] 	= "UseTZDepot",		--团长
 			[4] 	= "UseFTZdepot",	--副团长
 			[3] 	= "UseYZdepot",		--营长
 			[2] 	= "UseDZdepot",		--队长
 			[1] 	= "UseTYdepot",		--团员
		}
		
		--职位多语言对应的messageID
		self.m_tblPosLevel2MsgID =
		{	
			[5] 	= 2001,		--团长
 			[4] 	= 2002,		--副团长
 			[3] 	= 2003,		--营长
 			[2] 	= 2004,		--队长
 			[1] 	= 2005,		--团员
		}
	self.m_tblChallengeState =
		{
			["无战"]	= 0,
			["挑战"]	= 1,
			["战争"]	= 2,
		}
	self.m_tblPosInfo =
		{
			["团长"]	= 5,
			["副团长"]	= 4,
			["营长"]	= 3,
			["队长"]	= 2,
			["团员"]	= 1,
		}
	self.m_tblTruckMaxLoad =
	{
		["小型运输车"] = 5,
		["中型运输车"] = 10,
		["大型运输车"] = 50,
	}
	self.m_tblTongType = 
	{
		["普通"] = 0,
		["战斗"] = 1,
		["生产"] = 2,
		["精英战斗"] = 3,
		["精英生产"] = 4,
	}
end

function CTongMgr:GetDepotContent()
	return self.m_nDepotContent
end

function CTongMgr:GetTongTotalMem(nLevel)
	return self.m_nInitMember + self.m_nEveryLevelMember *(nLevel-1)
end

function CTongMgr:GetDepotStrByPage(nPage)
	return self.m_tblDepotType[nPage]
end

function CTongMgr:GetProfferByContribtMoney()
	return self.m_nAddProfferByContributeMoney
end

function CTongMgr:GetLogType(sTypeName)
	return self.m_tblLogType[sTypeName]
end
function CTongMgr:GetBuildingState(sTate)
	return self.m_tblBuildingState[sTate]
end
function CTongMgr:GetBuildWaitNum()
	return self.m_nBuildWaitNum
end

function CTongMgr:GetPosIndexByName(sPosName)
	return self.m_tblPosInfo[sPosName]
end


function CTongMgr:GetPosUpperTotal(nPosIndex)
	local tblInfo = TongPurview_Common("UpperNumber")
	local tblPos = {tblInfo("Level1"), tblInfo("Level2"), tblInfo("Level3"), tblInfo("Level4"), tblInfo("Level5")}
	return tblPos[nPosIndex]
end

function CTongMgr:GetRegistMoney()
	return self.m_nRegistMoney 
end

function CTongMgr:GetRegistLevel()
	return self.m_nRegistLevel 
end

function CTongMgr:GetJoinInLevel()
	return self.m_nJoinInLevel
end

function CTongMgr:GetDefaultResource()
	return self.m_nDefaultResource
end

function CTongMgr:GetUpperResource()
	return self.m_nUpperResource
end

function CTongMgr:GetDefaultMoney()
	return self.m_nDefaultMoney
end

function CTongMgr:GetDefaultHonor()
	return self.m_nDefaultHonor
end

function CTongMgr:GetUpTongMoney()
	return self.m_nUpTongMoney
end

function CTongMgr:GetUpTongExploit()
	return self.m_nUpTongExploit
end

function CTongMgr:GetLevelByHonor(nHonor)
	local tbl = {}
	local keys = TongLevelAndHonor_Common:GetKeys()
	for i,key in pairs(keys) do
		local p = TongLevelAndHonor_Common(key)
		local nIndex = p("nIndex")
		local nHonor = p("nHonor")
		if (not nHonor) or ("" == nHonor) then
			nHonor = 0 
		end
   	table.insert(tbl,{nIndex,tonumber(nHonor)})
  end
  if #tbl == 0 then return 1 end
	table.sort(tbl, function (a, b)
			if (a[1] < b[1]) then
				return a[1] < b[1]
			elseif(a[1] == b[1]) then
				return a[2] < b[2]
			end
		end)
	if nHonor < tbl[1][2] then
		return 1
	end
	for j=2,#tbl do
		if nHonor >= tbl[j-1][2] and nHonor < tbl[j][2] then
			return tbl[j-1][1]
		end
		if j == #tbl then
			return tbl[#tbl][1]
		end
	end
end

function CTongMgr:LevelN2S(nLevel)
	return TongLevelAndHonor_Common(nLevel, "sLevel")
end

function CTongMgr:GetUpperHonor(nCurLevel)
	local nUpperLevel = TongLevelAndHonor_Common(nCurLevel + 1, "nHonor")
	return nUpperLevel or TongLevelAndHonor_Common(nCurLevel, "nHonor")
end

function CTongMgr:GetTongTypeByName(sTypeName)
	return self.m_tblTongType[sTypeName]
end


g_TongMgr = g_TongMgr or CTongMgr:new()


local function GetTongFightTechToPlayerTbl()
	local tbl = {}
	local keys = TongFightTechToPlayer_Common:GetKeys()
	for i,v in pairs (keys) do
		tbl[v] = {}
		local LevelKeys = TongFightTechToPlayer_Common(v):GetKeys()
		for j,m in pairs (LevelKeys) do
			tbl[v][tonumber(m)] = {
													ConsumeSoul 	= TongFightTechToPlayer_Common(v,m,"ConsumeSoul"),
													ConsumeMoney 	= TongFightTechToPlayer_Common(v,m,"ConsumeMoney"),
													Proffer 		= TongFightTechToPlayer_Common(v,m,"Proffer"),
													ConsumeItemTbl 	= loadstring(" return " .. TongFightTechToPlayer_Common(v,m,"ConsumeItem"))(),
													TeamProffer 	= TongFightTechToPlayer_Common(v,m,"TeamProffer"),
												}
		end
	end
	return tbl
end

g_TongFightTechToPlayerTbl = GetTongFightTechToPlayerTbl()