CTempBagWnd = class(SQRDialog)
local CTempBagMiniWnd = class(SQRDialog)
cfg_load "int_obj/TempBagObj_Common"
lan_load "int_obj/Lan_TempBagObj_Common"

local GridNum = 9

function CTempBagWnd:GetTooltipStr(name)
	assert(not IsNil(Lan_TempBagObj_Common(MemH64(name))),name .. "µÄTempBagObjÃ»ÓÐTooltips")
	return Lan_TempBagObj_Common(MemH64(name), "Tooltips")
end

function CTempBagWnd:SetTooltips(Wnd)
	local str = ""
	if Wnd.m_Name then
		str = self:GetTooltipStr(Wnd.m_Name)
	end
	Wnd.m_WndBody:SetMouseOverDescAfter(str)
end

function CTempBagWnd:LoadPiece(Index)
	local Wnd = self.m_GridTbl[Index]
	local RenderObj = Wnd.m_WndBody.RenderObj
	RenderObj:RemoveAllPiece()
	RenderObj:Delframework()
	if Wnd.m_Effect then
		RenderObj:DelEffect(Wnd.m_Effect)
		Wnd.m_Effect = nil
	end
	if Wnd.m_BirthEffect then
		RenderObj:DelEffect( Wnd.m_BirthEffect )
		Wnd.m_BirthEffect = nil
	end
	self:SetTooltips(Wnd)
	if not Wnd.m_Name then
		return
	end
	local name = IntObj_Common(Wnd.m_Name, "ModelName")
	local Info = IntObjRes_Client(name)
	RenderObj:AddPiece( RES_TYPE.RFT_ARA, Info("AniFile") , "", 0 )
	RenderObj:AddPiece( RES_TYPE.RFT_ARP, Info("ModelFile") , "",  Info("RenderStyle") )
	RenderObj:SetDirection(CDir:new(90))
	RenderObj:SetScale(Info("Scaling")*2)
	RenderObj:SetPosition(CVector3f:new( 0, -100, -100 ) )
	RenderObj:DoAni( "stand01", true, -1.0 )
	RenderObj:DoAni( "birth", false, -1.0 )
	
	--local config = Info("AniFile")
	--local fileName = string.gsub(config, "object/ani/", "fx/setting/obj/")
	--fileName = string.gsub(fileName, ".ske", ".efx")
	
	--Wnd.m_BirthEffect = self:AddEffect(Index, {fileName,"stand01/create"}, true)
	--Wnd.m_BirthEffect = RenderObj:AddEffect( fileName, "stand01/create", -1, nil)
	--print(fileName)
	local tbl = GetCfgTransformValue(false,"TempBagObj_Common", Wnd.m_Name, "Effect")
	Wnd.m_Effect = self:AddEffect(Index, tbl, true)
end

local function CreateTempBagMiniWnd(Parent)
	local Wnd = CTempBagMiniWnd:new()
	Wnd:CreateFromRes("TempBagMiniWnd", Parent)
	Wnd.m_Btn = Wnd:GetDlgChild("Btn")
	Wnd:ShowWnd(false)
	return Wnd
end

function CTempBagWnd:Ctor(parent)
	self:CreateFromRes("TempBagWnd", parent)
	
	self.m_TempBagMiniWnd = CreateTempBagMiniWnd(parent)
	
	self:ShowWnd(false)
end

function CTempBagWnd:GetWnd()
	local Wnd = g_GameMain.m_TempBagWnd
	if not Wnd then
		Wnd = CTempBagWnd:new(g_GameMain)
		g_GameMain.m_TempBagWnd = Wnd
	end
	return Wnd
end

function CTempBagWnd:OnChildCreated()
	self.m_GridTbl = {}
	for i = 1, GridNum do
		self.m_GridTbl[i] = {}
		self.m_GridTbl[i].m_WndBody = self:GetDlgChild("Wnd"..i)
		OnCreateRoleModel(self.m_GridTbl[i])
	end
	self.m_Close = self:GetDlgChild("Close")
end

function CTempBagWnd:OnDestroy()
	for i = 1, GridNum do
		OnDestroy(self.m_GridTbl[i])
		self.m_GridTbl[i] = nil
	end
	self.m_GridTbl = nil
	self = nil
end

function CTempBagWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if uMsgID == RICHWND_LCLICKUP  then
		--print("RICHWND_LCLICKUP")
		for i =1, GridNum do
			if Child == self.m_GridTbl[i].m_WndBody then
				--print(self.m_GridTbl[i].m_Name)
				if self.m_GridTbl[i].m_Name then
					Gac2Gas:ClearTempBagObj(g_Conn, i)
				end
				break
			end
		end
	elseif uMsgID == RICHWND_RCLICKUP  then
		--print("RICHWND_RCLICKUP")
		for i =1, GridNum do
			if Child == self.m_GridTbl[i].m_WndBody then
				--print(self.m_GridTbl[i].m_Name)
				if self.m_GridTbl[i].m_Name then
					Gac2Gas:UseTempBagObj(g_Conn, i)
				end
				break
			end
		end
	elseif uMsgID == BUTTON_LCLICK then
		if Child == self.m_Close then
			self:ShowWnd(false)
			self.m_TempBagMiniWnd:ShowWnd(true)
		end
	end
end

function CTempBagMiniWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if uMsgID == BUTTON_LCLICK then
		if Child == self.m_Btn then
			local Wnd = CTempBagWnd.GetWnd()
			Wnd:ShowWnd(true)
			self:ShowWnd(false)
		end
	end
end

function CTempBagWnd:AddEffect(Index, StateStr, isCircle)
	if StateStr == nil or not IsTable(StateStr) then
		return
	end
	local n = 0
	local ModelString,StateString = unpack(StateStr)
	--ModelString,StateString = "fx/setting/obj/shjn_shaitaiyang.efx","shaitaiyang/create"
	if isCircle then
		n = -1
	end
	return self.m_GridTbl[Index].m_WndBody.RenderObj:AddEffect( ModelString,StateString, n, nil)
end

function CTempBagWnd:SetGrid(Index, Name)
	self.m_GridTbl[Index].m_Name = Name
	--self.m_GridTbl[Index].m_Num = num
	self:LoadPiece(Index)
end

function CTempBagWnd:OpenTempBag()
	for i = 1, GridNum do
		self:SetGrid(i)
	end
	self.m_TempBagMiniWnd:ShowWnd(false)
	self:ShowWnd(true)
end

function CTempBagWnd:CloseTempBag()
	self.m_TempBagMiniWnd:ShowWnd(false)
	self:ShowWnd(false)
	for i = 1, GridNum do
		self:SetGrid(i)
	end
end
