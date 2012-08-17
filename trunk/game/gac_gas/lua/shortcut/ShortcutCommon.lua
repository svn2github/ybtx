gac_gas_require "framework/message/message"

EShortcutPieceState =
{
	eSPS_None  = 0,
	eSPS_Item  = 1,
	eSPS_Skill = 2,
	eSPS_Equip = 3,
	eSPS_Array = 4,
}


-----------------------------聊天频道--------------------


ChannelIdTbl = {}
ChannelIdTbl["系统"] = 1 --"系统"
ChannelIdTbl["世界"] = 2 --"世界"
ChannelIdTbl["阵营"] = 3 --"阵营" 
ChannelIdTbl["地图"] = 4 --"地图"
ChannelIdTbl["附近"] = 5 --"附近"
ChannelIdTbl["队伍"] = 6 --"队伍"
ChannelIdTbl["佣兵小队"] = 7 --"佣兵团"
ChannelIdTbl["佣兵团"] = 8 --"团队"
ChannelIdTbl["NPC"] = 9 --"NPC"
ChannelIdTbl["传声"] = 10 --"传声"


ChannelTimeLimitTbl = {}
ChannelTimeLimitTbl[ChannelIdTbl["系统"]]	= 0 
ChannelTimeLimitTbl[ChannelIdTbl["世界"]]	= 5
ChannelTimeLimitTbl[ChannelIdTbl["阵营"]]	= 8
ChannelTimeLimitTbl[ChannelIdTbl["地图"]]	= 3
ChannelTimeLimitTbl[ChannelIdTbl["附近"]]	= 1
ChannelTimeLimitTbl[ChannelIdTbl["队伍"]]	= 0
ChannelTimeLimitTbl[ChannelIdTbl["佣兵团"]]	= 0
ChannelTimeLimitTbl[ChannelIdTbl["佣兵小队"]]	= 0
ChannelTimeLimitTbl[ChannelIdTbl["NPC"]]	= 0
ChannelTimeLimitTbl[ChannelIdTbl["传声"]]	= 0

---------------------------------
function g_CheckShortcutData(Type, Arg1, Arg2, Arg3)
	--[[
	if(Type == EShortcutPieceState.eSPS_Item) then
		local uBigID = Arg1
		assert(g_ItemInfoMgr:IsValid(uBigID) == true)
	elseif(Type == EShortcutPieceState.eSPS_Skill) then
		local SkillConfigMgr = GetSkillConfigMgr()
		local skill_name = Arg1
		local level = Arg2		
		assert(SkillConfigMgr:RawgetElement( MakeFightSkillID( skill_name, level ) ) ~= nil)
	elseif(Type == EShortcutPieceState.eSPS_Equip) then
		if not IsNumber(Arg1) then
			Arg1 = tonumber(Arg1)
		end
		local EquipPart = Arg1
		assert(g_CheckPart(EquipPart) == true)
	end
	--]]
	if(Type == EShortcutPieceState.eSPS_Skill) then
		if not (SkillPart_Common(Arg1) or NonFightSkill_Common(Arg1) or string.find(Arg1, "普通攻击")) then
			return false
		end
	end
	return true
end
-----------------------------------------------
PosSkillNameTbl = {
							["黑豹姿态"] = 1,
							["猛犸姿态"] = 2,
							["雄狮姿态"] = 3,
							["闪电武器"] = 1,
							["火焰武器"] = 2,
							["冰霜武器"] = 3,
							}
							