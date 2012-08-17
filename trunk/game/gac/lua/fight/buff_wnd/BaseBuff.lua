cfg_load "skill/BuffAndDebuff_Common"
lan_load "skill/Lan_BuffName"

function SetTime(s)
	local time
	if(s < 0) then
		return "0"
	elseif(s < 60*1000) then
		time = math.floor(s/1000)
		return ""..time..GetStaticTextClient(1119)
	elseif(s < 60*60*1000) then
		time = math.floor(s/1000/60)
		return ""..time..GetStaticTextClient(1120)
	else
		time = math.floor(s/1000/60/60)
		return ""..time..GetStaticTextClient(1121)
	end
end

local s_format = string.format
local SkillLevelFormulaTbl = {}

function GetBuffTips(sBuffName, SkillLevel)
	local buff = BuffAndDebuff_Common(sBuffName)
	if buff ~= nil then
		local displayNameIndex = MemH64(sBuffName)
		local SmallIcon = buff("SmallIcon")
		if Lan_BuffName(displayNameIndex) then
			local description = Lan_BuffName(displayNameIndex,"Description")
			local BuffDesArg = buff("BuffDesArg")
			local arg1 = nil
			if ( BuffDesArg ~= "NULL" and BuffDesArg ~= nil ) then
				local bSingleNumber = true
				local pos = 0
				local BuffDesTbl = {}
				local parVar = ""
				while true do
					local j = pos + 1
					pos = string.find(BuffDesArg, ",", j)
					if pos == nil then
						parVar = string.sub(BuffDesArg,j,string.len(BuffDesArg))
						table.insert(BuffDesTbl, parVar)
						break
					end
					bSingleNumber = false
					parVar = string.sub(BuffDesArg,j,pos-1)
					table.insert(BuffDesTbl, parVar)
				end

				if bSingleNumber then
					local str1 = CCfgGlobal_GetCalcChars("人物属性.公式")
					str1 = string.gsub(str1, "X", "SkillLevel")
					local str = s_format("SkillLevel = %d \n return { %s } ", SkillLevel, str1)
					local argTbl = SkillLevelFormulaTbl[str]
					if not argTbl then
						argTbl = loadstring(str)()
						SkillLevelFormulaTbl[str] = argTbl
					end
					
					if SkillLevel <= 20 then
						arg1 = argTbl[SkillLevel]
					else
						arg1 = argTbl[21]
					end
					arg1 = arg1 * BuffDesArg
				else
					if #(BuffDesTbl)>SkillLevel then
						arg1 = BuffDesTbl[SkillLevel]
					else
						arg1 = BuffDesTbl[#(BuffDesTbl)]
					end
				end
			end
			
			if arg1 ~= nil then 
				description = GetStrAccordWildcard(description, arg1)
			end
			return description,SmallIcon
		end
	end
	return nil,nil
end

function GetBuffDisplayName(sBuffName)
	if BuffAndDebuff_Common(sBuffName) ~= nil then
		local displayNameIndex = MemH64(sBuffName)
		local FightInfoType = BuffAndDebuff_Common(sBuffName,"FightInfoType")
		if Lan_BuffName(displayNameIndex) then
			local displayName = Lan_BuffName(displayNameIndex,"DisplayName")
			return displayName,FightInfoType
		end
	end
	return nil,nil
end

function GetRenderTime(pCharacter)
	if pCharacter then
		local RenderObj = pCharacter:GetRenderObject()
		if RenderObj then
			return RenderObj:GetRenderTime()
		end
	end
	return nil
end

