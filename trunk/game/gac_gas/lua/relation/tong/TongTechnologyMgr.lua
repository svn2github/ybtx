gac_gas_require "relation/tong/TongTechnologyMgrInc"
cfg_load "tong/TongFightTech_Common"
lan_load "tong/Lan_TongFightTech_Common"
function CTongTechnologyMgr:Ctor()
	--等待队列中的科技状态
	self.m_tblTechStat = {
		["Normal"]		= 0,	--正常
		["Waiting"]		= 1,	--排队中
		["Researching"]	= 2,	--研发中
		["Stay"]		= 3		--暂停
	}
	
	self.m_nMaxTeachLevel = 160		--科技最高等级
end

function CTongTechnologyMgr:GetStateIndex(sState)
	return self.m_tblTechStat[sState]
end

--等待序列的最大数量
function CTongTechnologyMgr:GetMaxOrder(nTongLevel)
	return nTongLevel
end

--读取配置表中数据----------------------------------------------------------------
function CTongTechnologyMgr:HasFightTechInfo(sName, nLevel)
	return TongFightTech_Common:HasNode( sName, tostring(nLevel) )
end

--技能名字列表
function CTongTechnologyMgr:GetFightTechNames()
	return TongFightTech_Common:GetKeys()
end

--战斗科技图标
function CTongTechnologyMgr:GetFightTechSmallIcon(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "SmallIcon")
end

--需要的佣兵团等级
function CTongTechnologyMgr:GetFightTechNeedTongLevel(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "NeedTongLevel")
end

--需要的佣兵团驻地所在战线
function CTongTechnologyMgr:GetFightTechStationLine(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "StationLine")
end

--需要的佣兵团资金
function CTongTechnologyMgr:GetFightTechNeedTongMoney(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "NeedTongMoney")
end

--需要的佣兵团资源
function CTongTechnologyMgr:GetFightTechNeedRes(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "NeedRes")
end

--需要花费的时间
function CTongTechnologyMgr:GetFightTechNeedTime(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "NeedTime")
end

--减少升级需要的消耗
function CTongTechnologyMgr:GetFightTechConsumeDepress(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "ConsumeDepress")
end

--减少升级需要的时间
function CTongTechnologyMgr:GetFightTechTimeDepress(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "TimeDepress")
end

--增加的天赋名字
function CTongTechnologyMgr:GetFightTechTalentName(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "TalentName")
end

--增加的天赋点
function CTongTechnologyMgr:GetFightTechTanlentPoint(sName, nLevel)
	assert( TongFightTech_Common:HasNode(sName, tostring(nLevel)), "TongFightTech_Common中没有Name:" .. sName .. " Level:" .. nLevel)
	return TongFightTech_Common(sName, tostring(nLevel), "TanlentPoint")
end

function CTongTechnologyMgr:GetTechMH64ByDisplayName(sDisplayName)
	local tblMH64 = {}
	local keys = TongFightTech_Common:GetKeys()
	for i, v in pairs(keys) do
		local real_name = v
		local levels = TongFightTech_Common:GetKeys(real_name)
		for j,k in pairs(levels) do
			local level = k
			local lanIndex = MemH64(string.format("%s%s", real_name, level))
			local sMH64Name = MemH64(real_name)
			local displaySuitName = tostring(Lan_TongFightTech_Common(lanIndex, "DisplayName"))
			if displaySuitName == sDisplayName then
				if( next(tblMH64) ) then
					local bFlag = true
					for i, w in ipairs(tblMH64) do
						if(w == sMH64Name) then bFlag = false break end
					end
					if(bFlag) then
						table.insert(tblMH64, sMH64Name)
					end
				else
					table.insert(tblMH64, sMH64Name)
				end
			end
		end
	end
	return tblMH64
end
--------------------------------------------------------------------------------

g_TongTechMgr = g_TongTechMgr or CTongTechnologyMgr:new()