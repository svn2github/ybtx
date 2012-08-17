
local FightingEvaluationDB = "FightingEvaluationDB"
CGasFightingEvaluation = class()


local suitCountTbl = 
	{
		[2] = 10,
		[3] = 20,
		[4] = 40,
		[6] = 45,
		[8] = 50
	}

--1 大剑士	 522+等级*69
--2 魔剑士	 442+等级*64
--4 法师	 403+等级*59
--5 邪魔	 442+等级*64
--6 牧师	 403+等级*59
--9 兽人战士 431+等级*49
local function GetEquipHealthPoint(player)
	local uClass = player:CppGetClass()
	local uLevel = player:CppGetLevel()
	local uHealthPoint = 0
	local totalHealthPoint = player:GetEquipPropertyValue(GetPropertyName("生命上限"))
	if uClass == 1 then
		uHealthPoint = totalHealthPoint - (522+uLevel*69)
	elseif uClass == 2 then
		uHealthPoint = totalHealthPoint - (442+uLevel*64)
	elseif uClass == 4 then
		uHealthPoint = totalHealthPoint - (403+uLevel*59)
	elseif uClass == 5 then
		uHealthPoint = totalHealthPoint - (442+uLevel*64)
	elseif uClass == 6 then
		uHealthPoint = totalHealthPoint - (403+uLevel*59)
	elseif uClass == 9 then
		uHealthPoint = totalHealthPoint - (431+uLevel*49)
	end
	return uHealthPoint
end


local function GetFightingEvaluationInfo(player,uDPS)
	if not IsCppBound(player) then
		return 
	end
	
	local PhysicalDPS							= player:GetEquipPropertyValue(GetPropertyName("攻击力"))
	local AccuratenessValue						= player:GetEquipPropertyValue(GetPropertyName("精准值"))
	local StrikeValue							= player:GetEquipPropertyValue(GetPropertyName("暴击值"))
	local CRIMax								= player:GetEquipPropertyValue(GetPropertyName("暴击上限"))
	local MagicHitValue							= player:GetEquipPropertyValue(GetPropertyName("法术命中"))
	local ResilienceValue						= player:GetEquipPropertyValue(GetPropertyName("韧性值"))
	local Defence								= player:GetEquipPropertyValue(GetPropertyName("护甲值"))
	local PhysicalDodgeValue					= player:GetEquipPropertyValue(GetPropertyName("物理躲避值"))
	local ParryValue							= player:GetEquipPropertyValue(GetPropertyName("招架值"))
	local MagicDodgeValue						= player:GetEquipPropertyValue(GetPropertyName("法术躲避值"))
	local NatureResistanceValue					= player:GetEquipPropertyValue(GetPropertyName("自然抗值"))
	local DestructionResistanceValue			= player:GetEquipPropertyValue(GetPropertyName("破坏抗值"))
	local EvilResistanceValue					= player:GetEquipPropertyValue(GetPropertyName("暗黑抗值"))
	local MagicDamageValue						= player:GetEquipPropertyValue(GetPropertyName("法伤"))
	local StrikeResistanceValue					= player:GetEquipPropertyValue(GetPropertyName("免爆值"))
	
	local uHealthPoint = GetEquipHealthPoint(player)
	local RatingValue1 = ((PhysicalDPS + uDPS)*10 + AccuratenessValue + StrikeValue + CRIMax*2)/100
	local RatingValue2 = (MagicDamageValue*10 + MagicHitValue + StrikeValue + CRIMax*2)/100
	local RatingValue3 = (uHealthPoint/2 + Defence + PhysicalDodgeValue + ParryValue + ResilienceValue*2+StrikeResistanceValue)/100
	local RatingValue4 = (uHealthPoint/2 + NatureResistanceValue + DestructionResistanceValue + EvilResistanceValue + MagicDodgeValue + ResilienceValue*2+StrikeResistanceValue)/100
	
	local maxValue = 0
	if MagicHitValue >= AccuratenessValue then
		maxValue = MagicHitValue
	else
		maxValue = AccuratenessValue
	end

	local equipTotalPoint = ((PhysicalDPS + uDPS)*10 + StrikeValue 
												+ CRIMax*2 + MagicDamageValue*10 + maxValue + uHealthPoint/2 
												+ Defence + PhysicalDodgeValue + ParryValue + ResilienceValue*2 + NatureResistanceValue 
												+ DestructionResistanceValue + EvilResistanceValue + MagicDodgeValue+StrikeResistanceValue)/100
													
	return RatingValue1,RatingValue2,RatingValue3,RatingValue4,equipTotalPoint
end

function CGasFightingEvaluation.GetAimFightingEvaluation(Conn,targetId,suitCountDBTbl,uDPS,uIntensifyPoint)
	local player = g_GetPlayerInfo(targetId)
	local uSuitPoint = 0
	if IsTable(suitCountDBTbl) then
		for i,v in pairs(suitCountDBTbl) do
			uSuitPoint = uSuitPoint + suitCountTbl[i] * v
		end	
	end
	local uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uTotalPoint = GetFightingEvaluationInfo(player,uDPS)
	Gas2Gac:RetAimFightingEvaluationInfoInfo(Conn,targetId,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)
end

--@brief 更新战斗力
function CGasFightingEvaluation.UpdateFightingEvaluationInfo(player)
	if not IsCppBound(player) then
		return 
	end
	local uSuitPoint,uDPS,uIntensifyPoint = player.m_uSuitPoint,player.m_uDPS,player.m_uIntensifyPoint
	if not uDPS then
		CGasFightingEvaluation.SaveFightingEvaluationInfo(player)
		return
	end
	local uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uTotalPoint = GetFightingEvaluationInfo(player,uDPS)
	Gas2Gac:RetFightingEvaluationInfoInfo(player.m_Conn,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)		
	player.m_uFightingTotalPoint = uTotalPoint
	if uTotalPoint > 800 then
		LogErr("有玩家战斗力超过800+了","玩家id：" .. player.m_uID)
	end
end


function CGasFightingEvaluation.SaveFightingEvaluationInfo(player)
	if not IsCppBound(player) then
		return 
	end
	local data = {["uCharID"] = player.m_uID} 
	local function CallBack(suitCountDBTbl,uDPS,uIntensifyPoint)
		local uSuitPoint = 0
		if IsTable(suitCountDBTbl) then
			for i,v in pairs(suitCountDBTbl) do
				uSuitPoint = uSuitPoint + suitCountTbl[i] * v
			end	
		end
		local uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uTotalPoint = GetFightingEvaluationInfo(player,uDPS)
		Gas2Gac:RetFightingEvaluationInfoInfo(player.m_Conn,uRatingValue1,uRatingValue2,uRatingValue3,uRatingValue4,uSuitPoint,uIntensifyPoint,uTotalPoint)		
	
		player.m_uSuitPoint = uSuitPoint
		player.m_uDPS = uDPS
		player.m_uIntensifyPoint = uIntensifyPoint
		player.m_uFightingTotalPoint = uTotalPoint
	end
	CallDbTrans("FightingEvaluationDB", "GetFightingEvaluationDB", CallBack, data, player.m_AccountID)
end

function CGasFightingEvaluation.UpdateEquipEffectDB()
	local function CallBack(old_res,new_res)
		for i = 1, old_res:GetRowNum() do
			Gas2GacById:RetUpdateEquipEffect(old_res(i,1),0)
		end
		for i = 1, new_res:GetRowNum() do
			Gas2GacById:RetUpdateEquipEffect(new_res(i,1),1)
		end
	end
	CallDbTrans("FightingEvaluationDB", "UpdateEquipEffectDB", CallBack, {})
end

--注册时钟,定时刷新装备特效
function CGasFightingEvaluation.UpdateEquipEffect()
	local function CallBack()
		if not g_App:CallDbHalted() then
			CGasFightingEvaluation.UpdateEquipEffectDB()
		end
	end
	g_AlarmClock:AddTask("CreateSortTblTick15", {wday = {1,2,3,4,5,6,7},time = {"05:05"}}, CallBack)
end

function CGasFightingEvaluation.UpdateEquipEffectToGas(Conn,uState)
	if uState == 0 then
		local player = Conn.m_Player
		player:PlayerDoPassiveSkill("撤销装备特效",1)
	else
		local data = {["char_id"] = Conn.m_Player.m_uID}	
		local function CallBack(equipEffectInfo)
			if equipEffectInfo:GetRowNum() > 0 then
				local player = Conn.m_Player
				if equipEffectInfo(1,1) <= 50 then
					player:PlayerDoPassiveSkill("高级装备特效",1)
				else
					player:PlayerDoPassiveSkill("次级装备特效",1)
				end
	  	end
		end
		CallAccountManualTrans(Conn.m_Account, "FightingEvaluationDB", "SelectEquipEffectDB", CallBack, data)
	end
end