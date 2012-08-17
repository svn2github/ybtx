cfg_load "tong/TongUpLevelNeedDevelopDegree"
gac_gas_require "relation/tong/TongPurviewMgr"
gac_gas_require "relation/tong/TongMgr"
gac_gas_require "framework/common/CMoney"
cfg_load "tong/TongAutoSeekStation_Common"
cfg_load "tong/TongResPos_Common"

CTongMainWnd = class(SQRDialog)	--导出类

function CTongMainWnd.CreateTongMainWnd(parent)
	local wnd = CTongMainWnd:new()
	wnd:Init(parent)
	return wnd
end

function CTongMainWnd:Init(parent)
	self:CreateFromRes("TongMain", parent)
end

function CTongMainWnd:OnChildCreated()
	self.m_Name				= self:GetDlgChild("Name")
	self.m_LeaderName		= self:GetDlgChild("LeaderName")
	self.m_Level			= self:GetDlgChild("Level")
	self.m_Exploit			= self:GetDlgChild("Exploit")
	self.m_MemberNum		= self:GetDlgChild("MemberNum")
	self.m_Honor			= self:GetDlgChild("Honor")
	self.m_Money			= self:GetDlgChild("Money")
	self.m_Type				= self:GetDlgChild("Type")
	self.m_PurposeEdit		= self:GetDlgChild("PurposeEdit")
	self.m_PurposeRichText	= self:GetDlgChild("PurposeRichText")
	self.m_ChangePurpBtn	= self:GetDlgChild("ChangePurposeBtn")
	self.m_ChangePurpOk		= self:GetDlgChild("ChangePurposeOk")
	self.m_Res				= self:GetDlgChild("Res")
	self.m_ChallengeInfo	= self:GetDlgChild("ChallengeInfo")
	self.m_StationInfo		= self:GetDlgChild("StationInfo")
	self.m_ResPostionInfo		= self:GetDlgChild("ResPostionInfo")
	self.m_ChangeToArmyCorpsBtn	= self:GetDlgChild("ChangeToArmyCorps")
	self.m_DevelopDegreeWnd = self:GetDlgChild("DevelopDegree")
	self.m_DevelopWnd = self:GetDlgChild("Develop")
	self.m_PurposeEdit:ShowWnd(false)
	self.m_ChangePurpOk:ShowWnd(false)
end

--消息
function CTongMainWnd:OnCtrlmsg( Child, uMsgID, uParam1, uParam2)
	if (uMsgID == BUTTON_LCLICK) then
		if(Child == self.m_ChangePurpBtn) then
			self:OnChangePurpose()
		elseif(Child == self.m_ChangePurpOk) then
			self:OnChangePurposeOk()
		elseif(Child == self.m_ChangeToArmyCorpsBtn) then
			CArmyCorpsPanel.GetWnd():ShowArmyCorpsInfo()
		end
	elseif (uMsgID == RICH_CLICK) then
		if(Child == self.m_StationInfo) then
			local value = self.m_StationInfo:GetChooseStrAttr()
			if (self.m_StationId ==0 )then
				Gac2Gas:FbStationAutoSeekInfo(g_Conn)
			else					
				local linktbl = {}
				for _,indexId in pairs(TongAutoSeekStation_Common:GetKeys()) do
					for _,warZoneId in pairs(TongAutoSeekStation_Common(indexId):GetKeys()) do
						local index = tonumber(indexId)
						local nWarZoneId = tonumber(warZoneId)
						if (index == self.m_StationId )then	
							if (nWarZoneId == self.m_WarZoneId )then
								local scenename = TongAutoSeekStation_Common(indexId,warZoneId,"SceneName")
								local posx      = TongAutoSeekStation_Common(indexId,warZoneId,"posx")
								local posy      = TongAutoSeekStation_Common(indexId,warZoneId,"posy")
								linktbl[1] = scenename
								linktbl[2] = posx
								linktbl[3] = posy
								if linktbl then
									PlayerAutoTrack(value,linktbl[1],linktbl[2],linktbl[3])	
								end
							end
							return
						end
					end
				end
			end
		elseif (Child == self.m_ResPostionInfo) then
			local value = self.m_ResPostionInfo:GetChooseStrAttr()		
			local index = TongResPos_Common:GetKeys()
			if not self.m_resName then
				return
			else
				for _,v in pairs (index) do
					if self.m_resName == v then
						local sceneName = TongResPos_Common(v,"sceneName")
						local posx      = TongResPos_Common(v,"posx")
						local posy      = TongResPos_Common(v,"posy")
						PlayerAutoTrack(value,sceneName,posx,posy)
						return
					end
				end
			end			
		end	
	end
end

------显示帮会信息面板------
function CTongMainWnd:OpenPanel(bFlag)
	if(bFlag) then
		Gac2Gas:GetTongInfo(g_Conn)
	end
	self:ShowWnd(bFlag)
end

------刷新帮会主面板信息------
--[[
团id
团名称
团主名称
团当前人数
团人数上限
团等级
团荣誉
团功勋
团资金
团当前粮草
团粮草上限
团宗旨
战争状态
战区
驻地位置
--]]

function CTongMainWnd:UpdateTongInfo(id, name, leaderName, curMemNum, maxMemNum,
									level, honor, exploit, money, res, maxRes, purpose,
									challengeState, warZoneId, stationId, type, developdegree, permitUpLevel,resName)
	local sLevel		= g_TongMgr:LevelN2S(level)
	local nUpperHonor	= g_TongMgr:GetUpperHonor(level)
	local sType			= g_GameMain.m_DisplayCommonObj:GetTongTypeName(type)
	self.m_Name:SetWndText(name)
	self.m_LeaderName:SetWndText(leaderName)
	self.m_Level:SetWndText(sLevel)
	self.m_MemberNum:SetWndText( string.format("%d/%d", curMemNum, maxMemNum ) )
	self.m_Exploit:SetWndText(exploit)
	self.m_Honor:SetWndText( string.format("%d/%d", honor, nUpperHonor ) )
	self.m_PurposeEdit:SetWndText(purpose)
	self.m_PurposeRichText:SetWndText(purpose)
	self.m_Res:SetWndText( string.format("%d/%d", res, maxRes ) )
	self.m_Money:SetWndText(g_MoneyMgr:ChangeMoneyToString(money,EGoldType.GoldCoin))
	self.m_Type:SetWndText(sType)
	local uUpLevelNeedDevelopDegree = TongUpLevelNeedDevelopDegree(level, "DevelopDegree")
	self.m_DevelopDegreeWnd:SetWndText(developdegree.."/"..uUpLevelNeedDevelopDegree)
	if permitUpLevel ==1 then
		self.m_DevelopWnd:SetWndText(GetStaticTextClient(9129))
	else
		self.m_DevelopWnd:SetWndText(GetStaticTextClient(9130))
	end
	if resName ~= "" then
		displayName = GetIntObjDispalyName(resName)
		local linkstr = AutoTrackColorStr.."#u#l"..displayName.."#i[".. MemH64(resName) .."]#l#u#W"
		self.m_ResPostionInfo:SetWndText(linkstr)
	else
		self.m_ResPostionInfo:SetWndText(resName)
	end		
	------------------------------------------------
	local tblState = {}
	tblState[ g_TongMgr.m_tblChallengeState["无战"] ] = GetStaticTextClient(10001)
	tblState[ g_TongMgr.m_tblChallengeState["挑战"] ] = GetStaticTextClient(10002)
	tblState[ g_TongMgr.m_tblChallengeState["战争"] ] = GetStaticTextClient(10003)
	local strStation = g_GameMain.m_TongBase:GetStationDes(warZoneId,stationId)
	local linkstr = AutoTrackColorStr.."#u#l"..strStation.."#i[".. MemH64(strStation) .."]#l#u#W"
	self.m_StationInfo:SetWndText(linkstr)

	--self.m_ChallengeInfo:SetWndText(tblState[challengeState])
	
	------------------------------------------------
	
	local bShowChangePurp = ( 1 == g_TongPurviewMgr:GetUpdatePurposeValue(g_GameMain.m_TongBase.m_TongPos) )
	self.m_ChangePurpBtn:ShowWnd(bShowChangePurp)
	self.m_StationId = stationId
	self.m_WarZoneId = warZoneId
	self.m_resName   = resName
end

------允许修改帮会宗旨------
function CTongMainWnd:OnChangePurpose()
	self.m_PurposeRichText:ShowWnd(false)
	self.m_PurposeEdit:ShowWnd(true)
	self.m_ChangePurpBtn:ShowWnd(false)
	self.m_ChangePurpOk:ShowWnd(true)
end

------确认修改帮会宗旨------
function CTongMainWnd:OnChangePurposeOk()
	self.m_PurposeEdit:ShowWnd(false)
	self.m_PurposeRichText:ShowWnd(true)
	self.m_ChangePurpBtn:ShowWnd(true)
	self.m_ChangePurpOk:ShowWnd(false)
	local sText = self.m_PurposeEdit:GetWndText()
	if string.len(sText) > 1024 then
		MsgClient(9178)
		return
	end
	Gac2Gas:ChangeTongPurpose(g_Conn, sText)
end