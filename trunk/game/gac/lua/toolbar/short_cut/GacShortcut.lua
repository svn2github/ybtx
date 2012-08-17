gac_gas_require "shortcut/ShortcutCommon"

--[[
快捷栏存储表说明：
	Type		Arg1				Arg2		Arg3
	1(物品)		uBigID				uSmallID	uGlobalID
	2(技能)		TypeID				Level
	3(装备)		EquipPart(装备部位)
	4(阵法)		arrayName
备注：	装备没使用时属于物品(此时快捷栏上的图标索引物品栏)
		装备使用后属于装备(此时快捷栏上的图标索引装备栏)
--]]
function GetPieceInfo(Piece)
	local type = Piece:GetState()
	local arg1, arg2, arg3
	if(type == EShortcutPieceState.eSPS_Item) then
		--该格子上放的是物品
		arg2, arg1 = Piece:GetItemContext()
		arg3 = Piece:GetMainSkill()
	elseif(type == EShortcutPieceState.eSPS_Skill) then
		--该格子上放的是技能
		local fs = Piece:GetSkillContext()
		arg1 = fs.SkillName
		arg2 = fs.Level
		arg3 = Piece:GetMainSkill()
	elseif(type == EShortcutPieceState.eSPS_Equip) then
		--该格子上放的是装备
	elseif(type == EShortcutPieceState.eSPS_Array) then
		--该格子上放的是阵法
		local fs = Piece:GetSkillArrayContext()
		arg1 = fs.SkillName
		arg2 = fs.Level
		arg3 = Piece:GetMainSkill()
	end
	return type, arg1, arg2, arg3
end

--保存快捷栏格子
function SaveShortcut(Conn, Piece)
	local type, arg1, arg2, arg3 = GetPieceInfo(Piece)
	local pos = Piece:GetPos()
	if type == EShortcutPieceState.eSPS_Skill and string.find(arg1, "普通攻击") == nil then
		local eCoolDownType = g_MainPlayer:GetSkillCoolDownType(arg1)
		if eCoolDownType == ESkillCoolDownType.eSCDT_FightSkill or 
			eCoolDownType == ESkillCoolDownType.eSCDT_NoComCDFightSkill or
			eCoolDownType == ESkillCoolDownType.eSCDT_UnrestrainedFightSkill or 
			eCoolDownType == ESkillCoolDownType.eSCDT_NonFightSkill then
			g_MainPlayer:AddLearnSkill(arg1,arg2)
		end
	end
	assert( pos,"参数pos为nil" )
	assert( type,"参数type为nil")
	assert( arg1,"参数arg1为nil")
	assert( arg2,"参数arg2为nil")
	assert( arg3,"参数arg3为nil")	
	if g_GameMain.m_MainSkillsToolBar.m_CurrentPage > 1 then
		pos = (g_GameMain.m_MainSkillsToolBar.m_CurrentPage - 1) * 10 + pos	
	end
	g_GameMain.m_MainSkillsToolBar.m_PieceTbl[arg3][pos] = {type, arg1, arg2}
	Gac2Gas:SaveShortCut(g_Conn, pos, type, arg1, arg2, arg3) --位置、格子的物品属性、技能名称、技能的等级、是否为主菜单
	Gac2Gas:SaveShortCutEnd(g_Conn)
end

--服务器端发回快捷栏数据
function Gas2Gac:ReturnShortcut(Conn, Pos, Type, Arg1, Arg2, Arg3)
	--检查数据是否合法
	if	g_CheckShortcutData(Type, Arg1, Arg2, Arg3) then
		g_GameMain.m_MainSkillsToolBar:ReturnSkill( Pos, Type, Arg1, Arg2, Arg3 )
	end
end

function Gas2Gac:ReturnShortcutEnd(Conn,index)
	g_GameMain.m_MainSkillsToolBar.m_CurrentPage = index
	g_GameMain.m_BeastPoseWnd.m_BgBtnTbl[index]:AddFlashInfoByName("OrangeEquip")
	g_GameMain.m_BeastPoseWnd:UpdateBeastPosToolBar()
end