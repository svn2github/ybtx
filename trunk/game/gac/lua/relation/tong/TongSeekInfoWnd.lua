CSeekInfoWnd = class(SQRDialog)
cfg_load "tong/TongAutoSeek_Common"
local AutoTrackColorStr = "#c"..g_ColorMgr:GetColor("×Ô¶¯Ñ°Â·")

function CSeekInfoWnd:Ctor(parent)
	self:CreateFromRes("SeekInfoWnd", parent)
	self.m_Info = self:GetDlgChild("Info")
	self:GetWnd()
end

function CSeekInfoWnd:GetWnd()
	self:ShowWnd(true)
	return self 
end

function CSeekInfoWnd:OnCtrlmsg(Child, uMsgID, uParam1, uParam2 )
	if(uMsgID == RICH_CLICK )	then
		local Index = Child:GetChooseStrAttr()
		local sceneName, posx, posy = nil,nil,nil
		local s1 = string.find( Index, ",", 0)
		if not s1 then
			sceneName = g_AutoSeekTbl[tonumber(Index)][1]
			posx = g_AutoSeekTbl[tonumber(Index)][2]
			posy = g_AutoSeekTbl[tonumber(Index)][3]
			PlayerAutoTrack("",sceneName,posx,posy)
			return
		end
		local index = string.sub(Index, 0, s1 - 1)
		local strLength = string.len(Index)
		local warZoneId = string.sub(Index, s1 + 1, strLength)
		local sceneName = nil
		local TongAutoSeekKey = TongAutoSeek_Common:GetKeys()
		
		for _, indexId in pairs(TongAutoSeekKey) do
			if index == indexId then
				local CommonKeys = TongAutoSeek_Common(index)
				local Indexkeys = CommonKeys:GetKeys()
				for i, _ in pairs(Indexkeys) do
					local str = i..""
					local wzId = CommonKeys(str, "warZoneId")
					if tonumber(warZoneId) == wzId then
						sceneName = CommonKeys(str, "SceneName")
						posx = CommonKeys(str, "posx")
						posy = CommonKeys(str, "posy")
						break
					end
				end
			end
		end
		PlayerAutoTrack("",sceneName,posx,posy)
	end
end



