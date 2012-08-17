gas_require "world/tong_area/truck_Inc"
gac_gas_require "relation/tong/TongMgr"

--初始化运输车信息
function InitTruckInfo(Npc)
	if Npc.m_TruckInfo then
		return
	end
	CTruck:Create(Npc)
end

--创建运输车	
function CTruck:Create(TruckEntity)
	local TruckName = TruckEntity.m_Properties:GetCharName()
	local MaxLoad = g_TongMgr.m_tblTruckMaxLoad[TruckName]
	if MaxLoad == nil then
		LogErr(TruckName.."不是运输车npc")
		MaxLoad = g_TongMgr.m_tblTruckMaxLoad["小型运输车"]
	end
	TruckEntity.m_TruckInfo = CTruck:new()
	TruckEntity.m_TruckInfo:Init(TruckEntity, MaxLoad)
--	local Master = TruckEntity:GetMaster()
--	if Master then
--		Master.m_Truck = TruckEntity
--	end
	return TruckEntity
end

function CTruck:Init(TruckEntity, MaxLoad)
	self.m_Entity = TruckEntity
	self.m_MaxLoad = MaxLoad
	self:SetResource(0)
end

--车上已装载资源数量
function CTruck:GetResource()
	return self.m_Resource
end

--最大装载数量
function CTruck:GetMaxResource()
	return self.m_MaxLoad
end

--还能装载的资源数量
function CTruck:GetOddResource()
	return self.m_MaxLoad - self.m_Resource
end

--设置运输车资源数
function CTruck:SetResource(ResNum)
--	if ResNum > self.m_MaxLoad then
--		self.m_Resource = self.m_MaxLoad
--		self:UpDataTurckState()
--		return ResNum - self.m_MaxLoad
--	else
		self.m_Resource = ResNum
		self:UpDataTurckState()
--	end
end

--运输车装载资源
function CTruck:LoadResource(ResNum)
	local OddRes = self:GetOddResource()
	if ResNum > OddRes then
		self.m_Resource = self.m_MaxLoad
		self:UpDataTurckState()
		return ResNum - OddRes
	else
		self.m_Resource = self.m_Resource + ResNum
		self:UpDataTurckState()
	end
end

function CTruck:UpDataTurckState()
	--print("当前资源数目：" .. self.m_Resource)
	if self.m_Resource ~= 0 then
		self.m_Entity:ServerDoNoRuleSkill("运输车满载状态", self.m_Entity)
	else
		self.m_Entity:ServerDoNoRuleSkill("清除运输车满载状态", self.m_Entity)	
	end
	self.m_Entity.m_Properties:SetMaterialNum(self.m_Resource)
end

--运输车卸载及销毁
function UnloadResource(TruckEntity)
	if not TruckEntity.m_TruckInfo then
		return 1	--不是运输车
	end
	if not TruckEntity:CppIsLive() then
		return 2	--运输车已经被破坏了
	end
--	local Player = CEntityServerManager_GetEntityByID(TruckEntity.m_CreatorEntityID)
--	if Player ~= nil then
--		Player.m_Truck = nil
--	end
	TruckEntity.m_TruckInfo:Destroy()
	TruckEntity.m_TruckInfo = nil
	TruckEntity:SetOnDisappear(true)
end

function CTruck:Destroy()
	self.m_Resource = nil
	self.m_MaxLoad = nil
	self = nil
end

--运输车销毁 资源掉落在地上
function TruckDestroy(TruckEntity)
	if not TruckEntity.m_TruckInfo then
		return
	end
--	local Player = CEntityServerManager_GetEntityByID(TruckEntity.m_CreatorEntityID)
--	if Player ~= nil then
--		Player.m_Truck = nil
--	end
	local Pos = CFPos:new()
	TruckEntity:GetPixelPos(Pos)
--	local ResFoldLimit = g_ItemInfoMgr:GetItemInfo(g_ItemInfoMgr:GetTongItemBigID(), TruckEntity.m_Properties:GetCharName(), "ResFoldLimit")
	DropResource(TruckEntity.m_Scene, Pos.x, Pos.y, TruckEntity.m_TruckInfo.m_Resource)
end

function PlayerSaveTruckInfo(Player, truck)
	if not truck.m_TruckInfo then
		return
	end
	local Name = truck.m_Properties:GetCharName()
	local Resource = truck.m_TruckInfo:GetResource()
	local data = {}
	data["char_id"] = Player.m_uID
	data["TruckName"] = Name
	data["Resource"] = Resource
	CallDbTrans("CTongTruckBox", "SaveTruckInfo", nil, data, Player.m_AccountID)
end

function PlayerLoadTruckInfo(Player, truck)
	if not IsCppBound(Player) or not IsCppBound(Player.m_Conn) then
		return
	end
	local function CallBack(TruckName, Resource)
		if TruckName == nil then
			return
		end
		if not IsCppBound(Player) then
			return
		end
		if not IsCppBound(truck) then
			return
		end
		truck.m_TruckInfo:SetResource(Resource)
	end
	local data = {}
	data["char_id"] = Player.m_uID
	CallAccountLoginTrans(Player.m_Conn.m_Account, "CTongTruckBox", "GetTruckInfo", CallBack, data)
end