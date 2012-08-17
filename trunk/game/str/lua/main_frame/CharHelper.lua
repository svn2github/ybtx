local LoginSucMsg = MsgTbl.LoginSucMsg
local LoginWaitingMsg = MsgTbl.LoginWaitingMsg
local RepeatLoginMsg = MsgTbl.RepeatLoginMsg
local CreateRoleSucMsg = MsgTbl.CreateRoleSucMsg
local GetCharListMsg = MsgTbl.GetCharListMsg


--[[
	sync check user可能会排队和顶号，所以我们需要根据其返回值来进行相应处理
	如果返回loginsucmsg，则继续，
	凡或loginwatingmsg，则看是否继续等待
--]]
function SyncCheckUser(Conn, UserName)
	CheckUser(Conn, UserName)
	local waited_msg = WaitForMsg(LoginSucMsg, LoginWaitingMsg, RepeatLoginMsg)
	return waited_msg
end

function SyncCreateRole(Conn)
	CreateRole(Conn)
	WaitForMsg(CreateRoleSucMsg)	
end

function SyncGetCharList(Conn)
	GetCharList(Conn)
	WaitForMsg(GetCharListMsg)
end


--[[

	创建角色，为了方便，我们就已帐户名1的形式表示角色姓名
	角色需要初始属性
	Hair	范围 1--4
	HairColor	范围 1--4
	Face	范围 1--4 
	Sex		范围 1--2
	Class	范围 1--7
	Inherence 范围 1--12
	这些属性我们随机的设置给角色
--]]	
local RoleNum = 0
function CreateRole(Conn)
	assert(IsCppBound(Conn))
	RoleNum = RoleNum + 1
	local strUserName = Conn.m_UserName
	local strName = strUserName .. "v" .. RoleNum
		
	local nHair = math.random(1, 4)
	local nHairColor = math.random(1, 4)
	local nFace = math.random(1, 4)
	local nSex = math.random(1, 2)
	local nClass = math.random(1, 6)
	if nClass == 3 then
		nClass = 9
	end
	local PosX = math.random(300,309)
	local PosY = math.random(280,289)
	
	--选择类型
--	local aryClass = EClass
--	aryClass.eCL_AllClass = nil 
--	aryClass.eCL_Npc = nil
--	aryClass.eCL_Special = nil
--	aryClass.eCL_Count = nil
--	aryClass.eCL_OrcWarrior = nil
--	aryClass.eCL_ElfHunter = nil
--		
--	local nClassList = {}		
--	for k, v in pairs(aryClass) do
--		table.insert(nClassList, v)
--	end
--	
--	local key = math.random(1, #nClassList)	
--	local nClass = nClassList[key]	
	
	Gac2Gas:CreateRole(Conn, strName, 
						53, 1, nFace, 
						100, nSex, nClass, 1, 
						PosX, PosY)
	
end


function GetCharList(Conn)
	assert(IsCppBound(Conn))
	Conn.m_RoleList = {}
	Gac2Gas:Get_Char_List(Conn)
end

--选择任意一个角色
function SelectChar(Conn)
	assert(IsCppBound(Conn))
	local key = math.random(1, #Conn.m_RoleList)

	local role = Conn.m_RoleList[key]
	
	Conn.m_CurRole = role
	
end

--进入游戏
function EnterGame(Conn, LoginType)
	assert(IsCppBound(Conn))
	local id = Conn.m_CurRole.m_id
	Gac2Gas:CharEnterGame(Conn, LoginType, id)
end
