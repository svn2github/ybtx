gac_require "message/message_box/MsgBoxMsg"

CWhiteStone = class ( SQRDialog )
local uAcceleration = -0.0005
--白宝石鉴定
function CWhiteStone:Ctor( Parent )
	self:CreateFromRes( "WhiteStoneTurnTable", Parent )
	self.tbl_New_StoneName = {}
	self.whitestoneBigID = nil
	self.ToStoneID ={} --鉴定后的高级宝石的ID
	self.m_JiandingBtn = self:GetDlgChild("jianding")
	self.pointer       = self:GetDlgChild( "pointer" )
	self.ToStone	  = self:GetDlgChild("Tostone")
	self.OK		  = self:GetDlgChild( "ok" )
	self.Colse	  = self:GetDlgChild("close")
	self.HighStoneName = self:GetDlgChild("StoneName")
	self.RotationAngle = 0
	self.RotationAcceleration = uAcceleration
	self.NeedRotationCircleNum  = 1
	self.position = 0 
	self.m_JiandingBtn:EnableWnd(false)
	self.ClickNumber = 1
	g_ExcludeWndMgr:InitExcludeWnd(self, 1)
end

function CWhiteStone.GetWnd()
	local Wnd = g_GameMain.m_whitestone
	if not Wnd then
		Wnd = CWhiteStone:new(g_GameMain)
		g_GameMain.m_whitestone = Wnd
	end
	return Wnd
end

--当用户不单击时10后自动停止
function Rotation()
	local Wnd = CWhiteStone.GetWnd()
	if( Wnd.RotationAngle >=  10008) then		
		Wnd.m_JiandingBtn:EnableWnd(false)
		Wnd:ClickStopButton(Wnd.position)
		Wnd.RotationAngle = 0			
		return 
	end
	Wnd.RotationAngle = Wnd.RotationAngle + 1
	Wnd.pointer:SetRotation( Wnd.RotationAngle*3.1415926/180 )
end

function CWhiteStone:OnCtrlmsg( Child, uMsgID, uParam1, uParam2 )
	if (uMsgID == BUTTON_LCLICK) then
		if( Child == self.m_JiandingBtn) then
			--鉴定
			if self.ClickNumber == 1 then
				if self.ToStoneID == nil or #(self.ToStoneID) == 0 then
					return
				end
				Gac2Gas:RequestStoneAppraiseAgain(g_Conn)
				self.m_JiandingBtn:EnableWnd(false)
				if self.ontimer ~= nil then 
					UnRegisterTick(self.ontimer)
					self.ontimer = nil 
				end
				self.OK:EnableWnd(false)
				self.ontimer = RegisterTick("Rotation", Rotation, 1 )
			elseif self.ClickNumber == 2 then
				self.m_JiandingBtn:EnableWnd(false)
				self.ClickNumber = 1
				if self.AnthorTimer == nil then
					self:ClickStopButton(self.position)	
				end
				self.position = 0
			end
		elseif( Child == self.OK ) then
			--提取产生的宝石
			Gac2Gas:TakeAppraisedStone(g_Conn)
			if #(self.ToStoneID) == 0 then
				SetEvent(Event.Test.RetTakeAppraisedStone,true)
			end
			UnRegisterTick( self.ontimer )
			UnRegisterTick( self.AnthorTimer )
		elseif( Child == self.Colse) then
			--关闭或取消自动提取
			self:OnClose()
			self:ShowWnd(false)
		end
	end
end

function CWhiteStone:OnClose()
	Gac2Gas:TakeAppraisedStone(g_Conn)
	UnRegisterTick( self.ontimer )
	UnRegisterTick( self.AnthorTimer )
	self.AnthorTimer = nil
	self.RotationAcceleration = uAcceleration
	self.NeedRotationCircleNum = 1
	self.RotationAngle = 0
	self.OK:EnableWnd(true)
end

function CWhiteStone:VirtualExcludeWndClosed()
	self:OnClose()
end

--当用户单击停止按钮时
local function OtherRotation()
	local Wnd = CWhiteStone.GetWnd()
	local Acceleration = 1
	
	if Wnd.NeedRotationCircleNum <= 0 then
		Wnd.RotationAcceleration = Wnd.RotationAcceleration + uAcceleration
		Acceleration = Acceleration + Wnd.RotationAcceleration --指针每次移动的距离
	end
	if(Wnd.RotationAngle >= Wnd.StopRotation) then
		Wnd.NeedRotationCircleNum = Wnd.NeedRotationCircleNum - 1
		if Acceleration > 0.2 then
			Wnd.RotationAngle = Wnd.RotationAngle - 360
		else
			Wnd.RotationAcceleration = uAcceleration
			Wnd.NeedRotationCircleNum = 1
			UnRegisterTick( Wnd.AnthorTimer )
			Wnd.AnthorTimer = nil
			if Wnd.stoneName~= "失败" then
				Wnd:RetWhiteStoneAppraise(Wnd.stoneBigID,Wnd.stoneName,Wnd.stoneID)
			else
				Wnd.ClickNumber = 1
				--Wnd.m_MsgBox  = MessageBox( Wnd, MsgBoxMsg(3014), MB_BtnOK )
			end
			Wnd.OK:EnableWnd(true)
			Wnd.RotationAngle = 0
			SetEvent( Event.Test.RetTakeAppraisedStone,true,flag)
			return
		end
	end
	
	if Acceleration <= 0.2 then
		Acceleration = 0.2
	end
	Wnd.RotationAngle = Wnd.RotationAngle + Acceleration
	Wnd.pointer:SetRotation( Wnd.RotationAngle*3.1415926/180 )
end

--计算出表针停止的位置
function CWhiteStone:ClickStopButton( ID )
	self.ClickNumber = 1
	local stop_index = ID 
	UnRegisterTick( self.ontimer )
	self.ontimer = nil
	---ID是从0 开始到 15 结束
	local index
	--得到转过圆的圈数
	local CircleNumber = math.floor(self.RotationAngle/360)
	--得到表针的现在弧度
	local CurrNumber  = self.RotationAngle - CircleNumber*360
	--得到当前的index
	if ( CurrNumber >345 or CurrNumber < 15 ) then
		index = 0
	else
		index = math.floor((CurrNumber + 15 )/30)						
	end
	self.OK:EnableWnd(false)
	--得到需要转动的刻度
	if index <= stop_index then
		local RotationAngle = (stop_index - index)*30
		self.StopRotation = self.RotationAngle + RotationAngle
		self.StopRotation = (math.floor((self.StopRotation + 15) / 30) )* 30
		self.AnthorTimer = RegisterTick("OtherRotation", OtherRotation, 2)
	elseif index > stop_index then			
		local RotationAngle = 360 - (index - stop_index)*30
		self.StopRotation = self.RotationAngle + RotationAngle
		self.StopRotation = (math.floor((self.StopRotation + 15) / 30) )* 30
		self.AnthorTimer = RegisterTick("OtherRotation", OtherRotation, 2)
	end

end

function CWhiteStone:UseWhiteStone(nBigID, nSmallID, nRoomIndex, nPos, eEquipPart )
	--放置白宝石
	if( #(self.ToStoneID) ~= 0 )then
		MessageBox( g_GameMain, MsgBoxMsg(3007), MB_BtnOK )
		return
	end
	self.ClickNumber = 1
	Gac2Gas:UseItem(g_Conn,nRoomIndex,nPos,nBigID, nSmallID,eEquipPart)
	g_WndMouse:ClearCursorAll()
end

--根据宝石的名称得到宝石的位置
local function GetPositionByName(stoneName)
	local WhiteStone = CWhiteStone.GetWnd()
	for i=1,# WhiteStone.tbl_New_StoneName do
		if WhiteStone.tbl_New_StoneName[i][2] == stoneName then
			return i - 1
		end
	end
	return 0
end
-- RCP 实现功能  --
function CWhiteStone:RetWhiteStoneAppraise(stoneBigID, stoneName, stoneID, nTimes)
	if nTimes == 1 then--第一次鉴定马上停止 
		self.position = GetPositionByName(stoneName)
		if stoneBigID == 0 then
			return
		end
		table.insert(self.ToStoneID,stoneID)
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo( tonumber(stoneBigID) , stoneName,"SmallIcon" ), self.ToStone, -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self.ToStone,stoneBigID, stoneName, 0)
		self.HighStoneName:SetWndText(stoneName)
		self.stoneBigID = stoneBigID
		self.stoneName = stoneName
		self.stoneID = stoneID
		local RotationAngle = (self.position - 0)*30
		self.StopRotation = self.RotationAngle + RotationAngle
		self.StopRotation = (math.floor((self.StopRotation + 15) / 30) )* 30
		self.pointer:SetRotation( self.StopRotation*3.1415926/180 )
		self.m_JiandingBtn:EnableWnd(true)
		--self:ClickStopButton(self.position)	
	elseif nTimes == 2 then --第二次鉴定,等待10s,或等待玩家按下停止按钮
		self.ClickNumber = 2 
		self.m_JiandingBtn:EnableWnd(true)
		--self.position = math.random(0,15)
		self.stoneBigID = stoneBigID
		self.stoneName = stoneName
		self.stoneID = stoneID
		self.position = GetPositionByName(stoneName)
		SetEvent( Event.Test.RetTakeAppraisedStone,true,true)
	else -- 按下停止后,很快就会停
		--self.position = GetPositionByName(stoneName)
		if stoneBigID == 0 then
			return
		end
		self.ToStoneID = {} --第二次鉴定后, 第一次的结果应该清空
		table.insert(self.ToStoneID,stoneID)
		g_DelWndImage(self.ToStone, 1, IP_ENABLE, IP_ENABLE)
		self.ToStone:SetMouseOverDescAfter("")
		g_LoadIconFromRes(g_ItemInfoMgr:GetItemInfo( tonumber(stoneBigID) , stoneName,"SmallIcon" ), self.ToStone, -1, IP_ENABLE, IP_ENABLE)
		self.HighStoneName:SetWndText(stoneName)
		g_SetItemRichToolTips(self.ToStone,stoneBigID, stoneName, 0)
	end
	if not self:IsShow() then
		self:ShowWnd(true)
	end
end

function CWhiteStone:DrawAllStoneItem()
	local tbl_new = self.tbl_New_StoneName
	self.tbl_New_StoneName = {}
	for n = 1, 12 do
		local r = math.random(#(tbl_new))
		table.insert(self.tbl_New_StoneName, tbl_new[r])
		table.remove(tbl_new,r)
	end

	local posString = {"1","2","3","4","5","6","7","8","9","10","11","12"}
	for i=1,# (posString) do
		local SmallIcon = g_ItemInfoMgr:GetItemInfo( tonumber(self.tbl_New_StoneName[i][1]) , self.tbl_New_StoneName[i][2],"SmallIcon" )
		g_LoadIconFromRes(SmallIcon, self:GetDlgChild("Child" .. posString[i]), -1, IP_ENABLE, IP_ENABLE)
		g_SetItemRichToolTips(self,tonumber(self.tbl_New_StoneName[i][1]) , self.tbl_New_StoneName[i][2], 0)
	end
end

function CWhiteStone:RetWhiteStoneAppraiseFirstTime(stoneBigID,stoneName)
	table.insert(self.tbl_New_StoneName, {stoneBigID,stoneName})
	if #(self.tbl_New_StoneName) >12 then
--		print("白宝石鉴定结果出错, tbl_New_StoneName在某处没有被清空!")
		return
	elseif #(self.tbl_New_StoneName) == 12 then
		self:DrawAllStoneItem()
	end
end

function CWhiteStone:RetTakeAppraisedStone(flag)
	if(flag ==true) then
		self.tbl_New_StoneName = {}
		self.ToStoneID ={}
		g_DelWndImage(self.ToStone, 1, IP_ENABLE, IP_ENABLE)
		self.HighStoneName:SetWndText("")
		self.ToStone:SetMouseOverDescAfter("")
		self.pointer:SetRotation(0)
		local posString = {"1","2","3","4","5","6","7","8","9","10","11","12"}
		for i=1,# (posString) do
			g_DelWndImage(self:GetDlgChild("Child" .. posString[i]), 1, IP_ENABLE, IP_ENABLE)
			self:GetDlgChild("Child" .. posString[i]):SetMouseOverDescAfter("")
		end
		self:ShowWnd(false)
	end
	SetEvent( Event.Test.RetTakeAppraisedStone,true,flag)
end
