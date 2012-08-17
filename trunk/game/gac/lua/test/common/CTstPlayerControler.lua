gac_require "test/common/CTstPlayerControlerInc"


--------------------		rpc for test 		-----------------
function Gas2Gac:RetrunNpcIdForTest( Connection, id )
--	print("Gas2Gac:RetrunNpcIdForTest", id)
	--db()
	coroutine.resume( g_TestCo, id )
end


------------------------------------------------------------------

local assert = _G.assert

CTstPlayerControler.s_WaitTime = nil

function CTstPlayerControler:SetWaitTime( time )
	assert( type(time) == "number"  )
	CTstLoginControler.s_WaitTime = time
end

function CTstPlayerControler.__WaitEvent( b, time, queue )
	local t = time or CTstLoginControler.s_WaitTime
	local r = WaitEvent( true, t, queue )
	SetEvent(queue,false)
	return r
end

---------------------------------------------------------------
function CTstPlayerControler:GetGridPos()
	local pos = CPos:new()
	g_MainPlayer:GetGridPos( pos )
	return pos.x, pos.y
end


function CTstPlayerControler:Sleep( milisec )
	g_TestCo = coroutine.running()
	
	local function wake_up()
		if( self.m_SleepTick == nil ) then
			return
		end
		
		UnRegisterTick( self.m_SleepTick )
		self.m_SleepTick = nil
		coroutine.resume( g_TestCo )
	end
	
	if( self.m_SleepTick ) then
		UnRegisterTick( self.m_SleepTick )
	end
	
	self.m_SleepTick = RegisterTick( "wake_up", wake_up, milisec)
	coroutine.yield()
end


function CTstPlayerControler:MoveToPixel( x, y )
	assert( g_MainPlayer ~= nil )
	local pos = {}
	pos.x = x
	pos.y = y
	
	SetEvent( Event.Test.PlayerMoveEnded, false )
	g_MainPlayer:BeforeMoveToSelectPos( pos )
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerMoveEnded )
	assert( r_tbl[1] == EEventState.Success )
end

function CTstPlayerControler:MoveToGrid( x, y )
	self:MoveToPixel( x*EUnits.eGridSpanForObj, y*EUnits.eGridSpanForObj )
end

--function CTstPlayerControler:FlyToPixel( x, y )
--	assert( g_MainPlayer ~= nil )
--	local pos = {}
--	pos.x = x
--	pos.y = y
--	
--	SetEvent( Event.Test.PlayerMoveEnded, false )
--	g_MainPlayer:JumpToSelectPos( pos )
--	
--	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerMoveEnded )
--	assert( r_tbl[1] == EEventState.Success )
--end

--function CTstPlayerControler:FlyToGrid( x, y )
--	self:FlyToPixel( x*EUnits.eGridSpanForObj, y*EUnits.eGridSpanForObj )
--end

function CTstPlayerControler:AddFightSkill( id, level )
--	local level = level or 1
--	
--	Gac2Gas:AddFightSkill( g_Conn, id, level )
--	
--	local i = 1
--	while g_MainPlayer:GetSkillLevel( id ) == 0 do
--		self:Sleep( 300 )
--		i = i + 1
--		assert( i < 100 )
--	end
end

local function GetCoolDownInfo( skill_name,skill_level )
	local sid = MakeFightSkillID( skill_name, g_MainPlayer:GetSkillLevel( skill_name ) )
	local skill = GetSkillConfigMgrGac():GetElement( sid )
	assert( skill ~= nil )
	return skill:GetCoolDownType() , skill:GetCoolDownTime()
end

function CTstPlayerControler:LockObj( target_id )
	local target = CCoreObjectFollower_GetObjectFollower( target_id )
	if( target == nil ) then
		return false
	end
	target = target:GetHandler()
	if( target == nil ) then
		return false
	end
	g_MainPlayer:LockObj( target )
	return true
end

function CTstPlayerControler:SelectCharacter( target_id )
	assert( type(target_id) == "number" )
	local target = CCoreObjectFollower_GetObjectFollower( target_id ):GetHandler()
	
	g_MainPlayer:LockObj( target )
end

--function CTstPlayerControler:__Fight( skill_name, target_id )
--	assert( g_MainPlayer ~= nil )
--
--	local SkillConfigMgr = GetSkillConfigMgrGac()
--	local level = g_MainPlayer:GetSkillLevel( skill_name )
--	assert( level ~= 0 )
--	local FightSkill = SkillConfigMgr:GetElement( MakeFightSkillID( skill_name, level ) )
--	assert( FightSkill ~= nil )
--	
--	local SkillType		= FightSkill:GetPoperty()
--	local TargetType		= FightSkill:GetTargetType()
--	
--	-- 光环技能
--	if( SkillType == EFSProperty.eFSP_Aurae ) then
--	
--	else
--		-- 其他技能
--		-- 需要 攻击目标 或 自身释放
--		if( TargetType == EFSTargetType.eFSTT_Object or  TargetType == EFSTargetType.eFSTT_SelfOrSelfPosition ) then		
--			self:SelectCharacter( target_id )
--			
--			assert( self:CanFightSkill(skill_name) == 0 )
--			
--			local uCoolDownType,uCoolDownTime = GetCoolDownInfo( skill_name )
--			assert( g_CoolDownMgr:NewCoolDown( uCoolDownType , uCoolDownTime )	)
--			g_MainPlayer:FightSkill( skill_name )
--			
--		elseif( TargetType == EFSTargetType.eFSTT_Position ) then	
--		
--		end
--	end
--end

function CTstPlayerControler:CanFightSkill( skill_name,skill_level )
	return g_MainPlayer:CanFightSkill( skill_name,skill_level )
end

function CTstPlayerControler:Fight( skill_name,skill_level, target_id )
	print("CTstPlayerControler:Fight", target_id)
	local target = CCoreObjectFollower_GetObjectFollower( target_id ):GetHandler()
	assert( target ~= nil )

	g_MainPlayer:LockObj( target )
	g_MainPlayer:FightSkill( skill_name,skill_level)
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
	assert( r_tbl[1] == EEventState.Success )
end

function CTstPlayerControler:FightSelf( skill_name,skill_level )
--	print("CTstPlayerControler:FightSelf")
	--db()
	g_MainPlayer:LockObj( g_MainPlayer )
	g_MainPlayer:FightSkill( skill_name,skill_level )
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
	assert( r_tbl[1] == EEventState.Success )
end

-------------------测普通攻击暂时先加的------------------------------------------
function CTstPlayerControler:FightWithNomalAttack( target_id )
--	print("CTstPlayerControler:FightWithNomalAttack", target_id)
--	local target = CCoreObjectFollower_GetObjectFollower( target_id ):GetHandler()
--	assert( target ~= nil )

	--g_MainPlayer:LockObj( target )
	g_MainPlayer:DoNormalAttack(target_id, true)
	
	--local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
	--assert( r_tbl[1] == EEventState.Success )
end
-------------------------------------------------------------
-------------------------------------------------------------
function CTstPlayerControler:NomalAttack( target_id )
--	print("CTstPlayerControler:Fight", target_id)
	local target = CCoreObjectFollower_GetObjectFollower( target_id ):GetHandler()
	assert( target ~= nil )

	g_MainPlayer:LockObj( target )
	g_MainPlayer:FightSkill( skill_name,skill_level )
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
	assert( r_tbl[1] == EEventState.Success )
end
-------------------------------------------------------------

function CTstPlayerControler:Shut( skill_name,skill_level, target_id )
	self:Fight( skill_name,skill_level, target_id )
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.BulletHit )
	assert( r_tbl[1] == EEventState.Success )
end

function CTstPlayerControler:FightNpcUntilDie( skill_name,skill_level, target_id )
	SetEvent( Event.Test.NpcDead, false )
	self:Fight( skill_name,skill_level, target_id )
	local r_tbl = CTstPlayerControler.__WaitEvent( true, 30000, Event.Test.NpcDead )
	
--	local uCoolDownType,uCoolDownTime = GetCoolDownInfo( skill_name )
--	uCoolDownTime = uCoolDownTime - 300
--	if uCoolDownTime < 0 then
--		uCoolDownTime = 0
--	end
	
	while( r_tbl[1] ~= EEventState.Success ) do 
--		self:Sleep(uCoolDownTime)	
		self:Fight( skill_name,skill_level, target_id )
		r_tbl = CTstPlayerControler.__WaitEvent( true, 30000, Event.Test.NpcDead )
	end
end

function CTstPlayerControler:WaitNpcDie( target_id , time)
	local target_core		= CCoreObjectFollower_GetObjectFollower( target_id )
	if( target_core == nil ) then
		return true
	end
	local target_handler	= target_core:GetHandler()
	if( target_handler == nil ) then
		return true
	end
	local time_ac = os.clock() + time / 1000
	while( target_handler:GetCurHP() > 0 ) do 
			local target_core		= CCoreObjectFollower_GetObjectFollower( target_id )
			if( target_core == nil ) then
				return true
			end
			local target_handler	= target_core:GetHandler()
			if( target_handler == nil ) then
				return true
			end
			if( os.clock() > time_ac ) then
				return false
			end
			self:Sleep( 50 )
	end	
	return true
end

function CTstPlayerControler:WaitUntilAfterSkill( time )
	local time_ac = os.clock() + time / 1000
	while( 1 ) do 
			if ( not g_MainPlayer:IsMoving() ) then
				return true
			end
			if( os.clock() > time_ac ) then
				return false
			end
			self:Sleep( 50 )
	end	
	return false
end

function CTstPlayerControler:WaitSkillEnd()
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
end

-- 只能在视野范围内加Npc, 因为异步的问题，无法获得视野范围以外的NPC是否已经被创建
--	relative_x, relative_y 是相对于 player 的坐标
function CTstPlayerControler:AddNpc( x, y, HP )
	local _x, _y = self:GetGridPos()
	assert(   math.abs(x-_x)<=6 and math.abs(y-_y)<=6   )
	
	SetEvent( Event.Test.NpcEnterAoi, false )
	
	Gac2Gas:AddNpcForTest( g_Conn, x, y, HP )
	g_TestCo = coroutine.running()
	local npc_id = coroutine.yield()
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, 30000, Event.Test.NpcEnterAoi )
	assert( r_tbl[1] == EEventState.Success )
	
	return npc_id
end

function CTstPlayerControler:DelNpc( uNpcId )
		SetEvent( Event.Test.ObjectDestroied, false )
		Gac2Gas:GM_Execute( g_Conn, string.format("$DelNpc(%d)", uNpcId) )
		WaitEvent( true , nil , Event.Test.ObjectDestroied )
end

function CTstPlayerControler:AddNpc_Relative( relative_x, relative_y, HP )
	local _x, _y = self:GetGridPos()
	local x = _x + relative_x
	local y = _y + relative_y
	
	return self:AddNpc( x, y, HP )
end

function CTstPlayerControler:SetPos( x, y )
	local _x, _y = self:GetGridPos()
	
	if( _x == x and _y == y ) then
		return
	end
	
	local gm_cmd = "$setpos " .. x .. ", " .. y
	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
	
	---------------	直到 位置发生改变 -------------------
	local new_x, new_y = _x, _y
	local i = 1
	while(  (new_x == _x) and (new_y ==_y)  ) do
		new_x, new_y = self:GetGridPos()
		self:Sleep( 300 )
		i = i + 1
		assert( i < 100 )
	end
end

function CTstPlayerControler:AddNpcEx( x, y, HP )
	local _x, _y = self:GetGridPos()
	
	if(   math.abs(x-_x)>3 or math.abs(y-_y)>3   ) then
		self:SetPos( x, y )
	end
	
	return self:AddNpc( x, y, HP )
end

function CTstPlayerControler:AddNpcExRect( x, y, HP, size )
	local _x, _y = self:GetGridPos()
	self:SetPos( x, y )
	
	local npcs={}
	local begin_x = x - math.floor( size / 2 )
	local begin_y = y - math.floor( size / 2 )
	assert( begin_x >= 0 )
	assert( begin_y >= 0 )
	
	
	for i=1,size do
		npcs[i] = {}
		for j=1,size do
			npcs[i][j] = self:AddNpc( begin_x+i, begin_y+j, HP )
		end
	end
end

function CTstPlayerControler:IsNpcRectAllDead( npcRect)
	for i=1,#npcRect do
		for j=1,#npcRect[i] do
			if( CCoreObjectFollower_GetObjectFollower( npcRect[i][j] ) ~= nil ) then
				return false
			end
		end
	end
	
	return true
end

function CTstPlayerControler:ChangeNpcGridPos(npc_id, x, y)
	SetEvent( Event.Test.NpcPosChanged, false )
	Gac2Gas:ChangeGridPos(g_Conn, npc_id, x, y)
	WaitEvent( true, nil, Event.Test.NpcPosChanged )
end

--function CTstPlayerControler:AddMagicEf( buffer_id )
--	local gm_cmd = "$addmagicef " .. buffer_id
--	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
--	
--	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.AddMagicEf )
--	assert( r_tbl[1] == EEventState.Success )
--end
--
--function CTstPlayerControler:CancelMagicEf( buffer_id )
--	local gm_cmd = "$cancelmagicef " .. buffer_id
--	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
--	
--	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.CancelMagicEf )
--	assert( r_tbl[1] == EEventState.Success )
--end
--
--function CTstPlayerControler:ClearAllBuff( buffer_id )
--	local gm_cmd = "$clearbuf "
--	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
--	
--	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.ClearAllBuff )
--	assert( r_tbl[1] == EEventState.Success )
--end
--
--function CTstPlayerControler:HasBuffer( buffer_id )
--	return g_MainPlayer.m_Buffers[buffer_id] ~= nil 
--end
--
--function CTstPlayerControler:GetBufferNum()
--	return #( g_MainPlayer.m_Buffers )
--end

function CTstPlayerControler:GetCurHP()
	return g_MainPlayer:CppGetPropertyValueByName("HealthPointAgile")
end

function CTstPlayerControler:GetCurMP()
	return g_MainPlayer:CppGetPropertyValueByName("ManaPointAgile")
end


function CTstPlayerControler:__wait_attr_change( attr )
	assert( type(attr) == "string" )
	
	local func = "GetCur" .. attr
	local pre = g_MainPlayer[func]( g_MainPlayer )
	
	local new = pre
	local i = 1
	while(  new == pre  ) do
		new =  g_MainPlayer[func]( g_MainPlayer )
		self:Sleep( 300 )
		i = i + 1
		assert( i < 100 )
	end

end

function CTstPlayerControler:AddCurHP( val )
	local gm_cmd = "$AddCurHP " .. val 
	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
	
	self:__wait_attr_change( "HP" )
end

function CTstPlayerControler:AddCurMP( val )
	local gm_cmd = "$AddCurMP " .. val 
	Gac2Gas:GM_Execute( g_Conn, gm_cmd )
	
	self:__wait_attr_change( "MP" )
end

function CTstPlayerControler:FightSkill( skill_name,skill_level )
	SetEvent( Event.Test.PlayerSkillEnded, false )
	g_MainPlayer:FightSkill( skill_name,skill_level )
	
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.PlayerSkillEnded )
	assert( r_tbl[1] == EEventState.Success )
end


function CTstPlayerControler:AttackMe( npc_id, SkillName,SkillLevel )
	SetEvent( Event.Test.NpcSkillEnded, false )
	
	local npc = CCoreObjectFollower_GetObjectFollower(npc_id):GetHandler()
	
	if( npc.m_LockCenter.m_LockingObj ) then
		npc:UnLockObj()
	end

	npc.m_LockCenter.m_LockingObj			= g_MainPlayer
	if( SelectObj ) then
		npc:CreateFacingPair( npc, g_MainPlayer )
	end
	
	Gac2Gas:AttackMe( g_Conn, npc_id, SkillName,SkillLevel )
	local r_tbl = CTstPlayerControler.__WaitEvent( true, nil, Event.Test.NpcSkillEnded )
	assert( r_tbl[1] == EEventState.Success )
end

function CTstPlayerControler:__wait_attr_change_ex( get_attr_func )
	assert( type(get_attr_func) == "string" )
	
	local func = get_attr_func
	local pre = g_MainPlayer[func]( g_MainPlayer )
	
	local new = pre
	local i = 1
	while(  new == pre  ) do
		new =  g_MainPlayer[func]( g_MainPlayer )
		self:Sleep( 300 )
		i = i + 1
		assert( i < 100 )
	end

end

function CTstPlayerControler:AddNFAttrServer( attr_name, val )
	local gm_cmd = "Pipe.m_Player:CppAdd" .. attr_name .. " ( " .. tostring(val ) .. " )"
	Gac2Gas:GM_RunScript( g_Conn, gm_cmd )
end

function CTstPlayerControler:ChangePropertyAdderServer( attr_name, val )
	local gm_cmd = "Pipe.m_Player:GM_CppSetPropertyValueByName( \"" .. attr_name .. "Adder\"" .. ", " .. tostring(val) .. " )"
	Gac2Gas:GM_RunScript( g_Conn, gm_cmd )
end

function CTstPlayerControler:SetOtherAttrServer( attr_name, type )
	local gm_cmd = "Pipe.m_Player:CppSet" .. attr_name .. " ( " .. tostring(type ) .. " )"
	Gac2Gas:GM_RunScript( g_Conn, gm_cmd )
end

function CTstPlayerControler:IsNpcDead( npc_id )
	assert( type(npc_id) == "number" )
	
	return ( CCoreObjectFollower_GetObjectFollower( npc_id ) == nil )
end

function CTstPlayerControler:GetNpcCurHP(npc_id)
	local npc = CCoreObjectFollower_GetObjectFollower(npc_id):GetHandler()
	local hp = npc:CppGetPropertyValueByName("HealthPointAgile")
	
	return npc:CppGetPropertyValueByName("HealthPointAgile")
end

function CTstPlayerControler:IsHPMax()
	return g_MainPlayer:GetCurHP() == g_MainPlayer:GetMaxHP()
end

--function CTstPlayerControler:SetAttrServer( attr_name, val )
--	local gm_cmd = "Pipe.m_Player:CppSet" .. attr_name .. " ( " .. tostring(val) .. " )"
--	Gac2Gas:GM_RunScript( g_Conn, gm_cmd )
--end
