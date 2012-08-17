gac_require "relation/association/AssociationBaseInc"

--[[
好友组ID中约定:"我的好友"的ID为1,"黑名单"的ID为2
--]]

function CreateAssociationBase()
	local base = CAssociationBase:new()
	base:Init()
	return base
end

function CAssociationBase:Init()
	self.m_IsRecChatMsg = false --是否记录聊天信息
--	self.m_sChatMsgPath = string.gsub(g_RootPath, "/", "\\") .. "var\\gac\\cm"
--	os.execute("if not exist " .. self.m_sChatMsgPath .. " mkdir " .. self.m_sChatMsgPath)
	
	self.m_tblTongMember	= {{},{}}
	self.m_tblOpenRecord	= {}
	
	self:InitBlackList()
	self:InitFriend()
	self:InitGroup()
end

function CAssociationBase:InitFriend()
	self.m_tblFriendClassIndex				= {} --用组ID对应索引
	self.m_tblFriendListClass				= {} --分了组的好友列表{name, level, feeling, state, playerId, bOnline}
	self.m_tblFriendListClass[1]			= {}
	self.m_tblFriendListClass[1].className	= GetStaticTextClient(2006) --"我的好友"
	self.m_tblFriendListClass[1].classId	= 1
end

function CAssociationBase:InitBlackList()
	self.m_tblBlackList				= {} --黑名单列表
	self.m_tblBlackList.className	= GetStaticTextClient(2007) --"黑名单" 
	self.m_tblBlackList.classId		= 2
end

function CAssociationBase:InitGroup()
	self.m_tblFriendGroupIndex	= {} --用群ID对应索引
	self.m_tblFriendGroup		= {} --好友群{ { {groupName, groupId, groupType, groupLable, groupDeclare, ctrlType}, {name, id, ctrlType, bOnline} }, ...} --ctrlType:1,创建者;2,管理员;3,普通成员
	self.m_tblCreateGroupIndex	= {} --自己创建的群的索引
	self.m_tblManageGroupIndex	= {} --自己是管理员身份的群索引
	self.m_RefuseGroupMsgGroupId = {} --屏蔽群消息的群id
end

--按照是否在线排序好友
function CAssociationBase:SortFriendListByBeOnline(classId)
	local tblFriendListClass		= self.m_tblFriendListClass
	local tblThisClassInfo			= tblFriendListClass[self.m_tblFriendClassIndex[classId]]
	tblThisClassInfo.nOnlineCount	= #(tblThisClassInfo)
	table.sort(tblThisClassInfo, function (a, b) return a.nOnline < b.nOnline end)
	for i, v in ipairs(tblThisClassInfo) do
		if(2 == v.nOnline) then
			tblThisClassInfo.nOnlineCount = i - 1
			break
		end
	end
end

--按照是否在线排序某群好友
function CAssociationBase:SortFriendGroupByBeOnline(groupId)
	local tblFriendGroup	= self.m_tblFriendGroup
	local tblThisGroupInfo	= tblFriendGroup[self.m_tblFriendGroupIndex[groupId]]
	table.sort(tblThisGroupInfo[2], function (a, b) return a[4] < b[4] end)
end

--重新设定好友组索引表
function CAssociationBase:ResetClassIndex()
	local tblFriendListClass	= self.m_tblFriendListClass
	self.m_tblFriendClassIndex	= {}
	for i = 1, #tblFriendListClass do
		self.m_tblFriendClassIndex[tblFriendListClass[i].classId] = i --一张用组id做索引的对应表
	end
end

--重新设定好友群索引表
function CAssociationBase:ResetGroupIndex()
	local tblFriendGroup	= self.m_tblFriendGroup
	self.m_tblFriendGroupIndex	= {}
	self.m_tblCreateGroupIndex	= {}
	self.m_tblManageGroupIndex	= {}
	for i = 1, #tblFriendGroup do
		self.m_tblFriendGroupIndex[tblFriendGroup[i][1][2]] = i
		if(1 == tblFriendGroup[i][1][6]) then
			table.insert(self.m_tblCreateGroupIndex, i)
		elseif(2 == tblFriendGroup[i][1][6]) then
			table.insert(self.m_tblManageGroupIndex, i)
		end
	end
end

--重新设定好友群自身权限表
function CAssociationBase:ResetCtrlTypeList()
	local tblFriendGroup	= self.m_tblFriendGroup
	self.m_tblCreateGroupIndex	= {}
	self.m_tblManageGroupIndex	= {}
	for i = 1, #tblFriendGroup do
		if(1 == tblFriendGroup[i][1][6]) then
			table.insert(self.m_tblCreateGroupIndex, i)
		elseif(2 == tblFriendGroup[i][1][6]) then
			table.insert(self.m_tblManageGroupIndex, i)
		end
	end
end

function CAssociationBase:ResetFriendGroupList()
	local wnd	= g_GameMain.m_AssociationWnd.m_AssociationListWnd
	if(2 == wnd.m_ListPage) then --好友群列表打开着
		wnd.m_tblList[2]:SetFriendGroupList()
	end
end

--返回加为好友
function CAssociationBase:RetAddFriendToClass(classId, playerId, playerName, level, nCamp, nClass, nTeamSize, sScene, nBeOnline, nTongId)
	local tblFriendListClass	= self.m_tblFriendListClass
	local classIndex			= self.m_tblFriendClassIndex[classId]
	local tblBlackMemList		= self.m_tblBlackList
	
	local tblInfo = {}
	tblInfo.playerId		= playerId
	tblInfo.playerName		= playerName
	tblInfo.playerLevel		= level
	tblInfo.nPlayerCamp		= nCamp
	tblInfo.nPlayerClass	= nClass
	tblInfo.nTeamSize		= nTeamSize
	tblInfo.sScene			= sScene
	tblInfo.nTongId			= nTongId
	tblInfo.nOnline			= nBeOnline
	
	table.insert(tblFriendListClass[classIndex], tblInfo)
	self:SortFriendListByBeOnline(classId)
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd[classIndex]:ResetItem()
	
	for i = 1, #tblBlackMemList do --如果黑名单中有此人,清掉
		if(playerId == tblBlackMemList[i].playerId) then
			table.remove(tblBlackMemList, i)
			local tblItem = g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
			tblItem[#tblItem]:ResetItem()
			break
		end
	end
end

--返回删除好友/被好友删除
function CAssociationBase:DeleteFriendById(playerId, classId)
	local tblFriendListClass	= self.m_tblFriendListClass
	local classIndex			= self.m_tblFriendClassIndex[classId]
	local tbl = tblFriendListClass[classIndex]
	for i, v in ipairs(tbl) do
		if(playerId == v.playerId) then
			table.remove(tbl, i)
			break
		end
	end
	self:SortFriendListByBeOnline(classId)
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd[classIndex]:ResetItem()
end

function CAssociationBase:AddBlackListByName(playerName)
	if(self:IsInBlackList(playerName)) then
		MsgClient(5001)
		return
	end
	local index, classId = self:SearchFriendInListByName(playerName)
	if(index) then
		local tblItem		= g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
		local classIndex	= self.m_tblFriendClassIndex[classId]
		tblItem[classIndex]:PreMoveToBlackList({classId, index})
	else
		Gac2Gas:AddBlackListByName(g_Conn, playerName)
	end
end

function CAssociationBase:ReceivePrivateChatMsg(fromId, fromName, text, time)
	local tbl = g_GameMain.m_tblAssociationPriChat
	for i = 1, #tbl do
		local chatWnd = tbl[i]
		if(chatWnd.m_ChatWithId == fromId) then --和对方聊天的对话框已经打开
			chatWnd:AddNewLineInfoWithTime(fromName, text, time)
			g_GameMain.m_MsgListParentWnd.m_MsgListWnd:FlashOneItemByTypeAndId("私聊", fromId)
			return
		end
	end
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:InsertChatMsg("私聊", fromId, fromName, text, time)
end

--好友上下线处理
function CAssociationBase:NotifyFriendBeOnline(memberId, classId, beOnline)
	local classIndex			= self.m_tblFriendClassIndex[classId]
	local tblThisClassMemInfo	= self.m_tblFriendListClass[classIndex]
	for i = 1, #tblThisClassMemInfo do
		if(memberId == tblThisClassMemInfo[i].playerId) then
			tblThisClassMemInfo[i].nOnline = beOnline
			break
		end
	end
	self:SortFriendListByBeOnline(classId)
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd[classIndex]:ResetItem()
end

--群成员上下线处理
function CAssociationBase:NotifyGroupMemberBeOnline(memberId, groupId, beOnline)
	local groupIndex			= self.m_tblFriendGroupIndex[groupId]
	local tblThisGroupMemInfo	= self.m_tblFriendGroup[groupIndex][2]
	for i = 1, #tblThisGroupMemInfo do
		if(memberId == tblThisGroupMemInfo[i][2]) then
			tblThisGroupMemInfo[i][4] = beOnline
			break
		end
	end
	self:SortFriendGroupByBeOnline(groupId)
	self:FindAndReDrawPubChatWnd(groupId)
end

--离开群
function CAssociationBase:LeaveGroup(groupId)
	local tblFriendGroup	= self.m_tblFriendGroup
	local groupIndex		= self.m_tblFriendGroupIndex[groupId]
	table.remove(tblFriendGroup, groupIndex)
	self:ResetGroupIndex()
	self:ResetFriendGroupList()
	self:FindAndClosePubChatWnd(groupId)
	self:DeleteGroupCallInfo(groupId)
end

--如果groupId此窗体打开,就关闭
function CAssociationBase:FindAndClosePubChatWnd(groupId)
	local tblPubChatWnd = g_GameMain.m_tblAssociationPubChat
	for i = 1, #tblPubChatWnd do
		if(groupId == tblPubChatWnd[i].m_GroupId) then
			tblPubChatWnd[i]:ShowWnd(false)
			break
		end
	end
end

--如果groupId此窗体打开,就刷新成员列表
function CAssociationBase:FindAndReDrawPubChatWnd(groupId)
	local tblPubChatWnd = g_GameMain.m_tblAssociationPubChat
	for i = 1, #tblPubChatWnd do
		if(groupId == tblPubChatWnd[i].m_GroupId) then
			tblPubChatWnd[i]:DrawMemListItem()
			break
		end
	end
end

--打开成员面板并显示成员信息
function CAssociationBase:ShowPersonalInfo(context)
	local memberId, memberName = unpack(context)
	local tbl = g_GameMain.m_tblPerInfoShowWnd
	for i = 1, #tbl do
		if(memberId == tbl[i].m_MemberId) then
			return
		end
	end
	local wnd = CreateAssociationPersonalInfoShowWnd(g_GameMain, memberId, memberName)
	wnd.m_NameLable:SetWndText(memberName)
	wnd.m_IdLable:SetWndText("ID:" .. memberId)
	table.insert(tbl, wnd)
	Gac2Gas:GetAssociationMemberInfo(g_Conn, memberId)
end

--删除该群的群聊信息
function CAssociationBase:DeleteGroupMsgCallInfo(groupId)
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("群聊", groupId)
end

--删除该群的申请信息
function CAssociationBase:DeleteGroupAppCallInfo(groupId)
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("好友群申请", groupId)
end

--删除该群的所有信息
function CAssociationBase:DeleteGroupCallInfo(groupId)
	self:DeleteGroupAppCallInfo(groupId)
	self:DeleteGroupMsgCallInfo(groupId)
end

--删除该id的聊天信息
function CAssociationBase:DeletePriChatCallInfo(chatWithId)
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CloseOne("私聊", groupId)
end

--通过名字在好友表中查找
function CAssociationBase:SearchFriendInListByName(playerName)
	local tbl = self.m_tblFriendListClass
	for i, p in pairs(tbl) do
		for j, v in ipairs(p) do
			if(playerName == v.playerName) then
				return j, p.classId
			end
		end
	end
	return nil
end

--判读是否在黑名单中
function CAssociationBase:IsInBlackList(idOrName)
	if(IsNumber(idOrName)) then
		return self:IsInBlackListById(idOrName)
	else
		return self:IsInBlackListByName(idOrName)
	end
end

--通过对方ID判断对方是否在自己的黑名单中
function CAssociationBase:IsInBlackListById(memberId)
	local blackList = self.m_tblBlackList
	for i = 1, #blackList do
		if(memberId == blackList[i].playerId) then
			return true
		end
	end
	return false
end

--通过对方名字判断对方是否在自己的黑名单中
function CAssociationBase:IsInBlackListByName(memberName)
	local blackList = self.m_tblBlackList
	for i = 1, #blackList do
		if(memberName == blackList[i].playerName) then
			return true
		end
	end
	return false
end

--添加好友群成员
function CAssociationBase:AddGroupMem(memberId, memberName, memberType, groupId, bOnline)
	local tblFriendGroup		= self.m_tblFriendGroup
	local tblGroupIndex			= self.m_tblFriendGroupIndex
	
	if(memberId == g_MainPlayer.m_uID) then
		tblFriendGroup[tblGroupIndex[groupId]][1][6] = memberType
		if(1 == memberType) then
			table.insert(self.m_tblCreateGroupIndex, tblGroupIndex[groupId])
		elseif(2 == memberType) then
			table.insert(self.m_tblManageGroupIndex, tblGroupIndex[groupId])
		end
	end
	table.insert(tblFriendGroup[tblGroupIndex[groupId]][2], {memberName, memberId, memberType, bOnline})
end

function CAssociationBase:AddGroupMemEnd(groupId)
	if(0 == groupId) then
		for k, v in pairs(self.m_tblFriendGroupIndex) do
			self:SortFriendGroupByBeOnline(k)
			self:FindAndReDrawPubChatWnd(k)
		end
	else
		self:SortFriendGroupByBeOnline(groupId)
		self:FindAndReDrawPubChatWnd(groupId)
	end
end

function CAssociationBase:RemoveChatRecord()
	local rootPath = string.gsub(g_RootPath, "/", "\\")
	os.execute("if exist " .. self.m_sChatMsgPath .. " rd /s/q " .. self.m_sChatMsgPath)
end

--从客户端保存文件中读取最近联系人信息
--[[
function CAssociationBase:ImportRecentContactInfo()
	local filePath = g_RootPath .. "var/gac/assolog/recentcontact/RecentContact.asv"
	local f = CLuaIO_Open(filePath, "r")
	f:Close()
end
--]]
----------------------------------------------------------------------------------------------------------

function Gas2Gac:ReturnInitAssociationInfo(Conn)
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:SetFriendList()
end

------好友基本操作------
--返回好友分组开始
function Gas2Gac:ReturnGetAllFriendClassBegin(Conn)
	g_GameMain.m_AssociationBase:InitFriend()
	g_GameMain.m_AssociationBase:InitBlackList()
end

--返回好友分组
function Gas2Gac:ReturnGetAllFriendClass(Conn, classId, className) --不包括“我的好友”, 和“黑名单”
	local base = g_GameMain.m_AssociationBase
	local tblFriendListClass = base.m_tblFriendListClass
	
	local tblInfo		= {}
	tblInfo.className	= className
	tblInfo.classId		= classId
	table.insert(tblFriendListClass, tblInfo)
end

--返回好友分组结束
function Gas2Gac:ReturnGetAllFriendClassEnd(Conn)
	g_GameMain.m_AssociationBase:ResetClassIndex()
end

--返回所有好友信息(包括黑名单)
--好友组ID, 玩家ID, 好友name, 等级, 阵营, 职业, 小队人数, 所在场景, 是否在线
function Gas2Gac:ReturnGetAllFriendInfo(Conn, classId, playerId, playerName, level, nCamp, nClass, nTeamSize, sScene, bOnline, nTongId)
	local base = g_GameMain.m_AssociationBase
	local tblBlackList			= base.m_tblBlackList
	local tblFriendListClass	= base.m_tblFriendListClass
	local tblClassIndex			= base.m_tblFriendClassIndex
	
	local tblInfo = {}
	tblInfo.playerId		= playerId
	tblInfo.playerName		= playerName
	tblInfo.playerLevel		= level
	tblInfo.nPlayerCamp		= nCamp
	tblInfo.nPlayerClass	= nClass
	tblInfo.nTeamSize		= nTeamSize
	tblInfo.sScene			= sScene
	tblInfo.nTongId			= nTongId
	tblInfo.nOnline			= bOnline
	if(2 == classId) then
		table.insert(tblBlackList, tblInfo)
	else
		table.insert(tblFriendListClass[tblClassIndex[classId]], tblInfo)
	end
end

--返回所有好友信息结束(包括黑名单)
function Gas2Gac:ReturnGetAllFriendInfoEnd(Conn)
	local base = g_GameMain.m_AssociationBase
	for k, v in pairs(base.m_tblFriendClassIndex) do
		base:SortFriendListByBeOnline(k)
	end
--	print("----------好友加载完毕----------")
end

--刷新指定好友信息
function Gas2Gac:ReturnUpdateFriendInfo(Conn, classId, level, nTeamSize, sScene, nFriendId, nTongId)
	local base			= g_GameMain.m_AssociationBase
	local classIndex	= base.m_tblFriendClassIndex[classId]
	local tblForThisOne	= base.m_tblFriendListClass[classIndex]
	for i, v in ipairs(tblForThisOne) do
		if( v.playerId == nFriendId ) then
			v.playerLevel	= level
			v.nTeamSize		= nTeamSize
			v.sScene		= sScene
			v.nTongId		= nTongId
			break
		end
	end
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd[classIndex]:ResetItemInfoById(nFriendId)
end

--刷新在线好友信息
function Gas2Gac:ReturnUpdateOnlineFriendInfo(Conn, classId, playerId, level, nTeamSize, sScene, nTongId)
	local base					= g_GameMain.m_AssociationBase
	local tblFriendListClass	= base.m_tblFriendListClass
	local tblClassIndex			= base.m_tblFriendClassIndex
	
	local classIndex			= tblClassIndex[classId]
	local tblForThisOne			= tblFriendListClass[classIndex]
	for i, v in ipairs(tblForThisOne) do
		if( v.playerId == playerId ) then
			v.playerLevel	= level
			v.nTeamSize		= nTeamSize
			v.sScene		= sScene
			v.nTongId		= nTongId
			break
		end
	end
end

--按组返回好友实时信息结束(只刷对应组)
function Gas2Gac:ReturnUpdateOnlineFriendInfoClassEnd(Conn, classId)
	local tblClassIndex	= g_GameMain.m_AssociationBase.m_tblFriendClassIndex
	local classIndex	= tblClassIndex[classId]
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd[classIndex]:ResetItemInfoOnline()
end

--好友上线
function Gas2Gac:NotifyFriendOnline(Conn, classId, memberId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	if(2 == classId) then return end --如果是黑名单成员就返回
	g_GameMain.m_AssociationBase:NotifyFriendBeOnline(memberId, classId, 1)
end

--好友下线
function Gas2Gac:NotifyFriendOffline(Conn, classId, memberId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	if(2 == classId) then return end --如果是黑名单成员就返回
	g_GameMain.m_AssociationBase:NotifyFriendBeOnline(memberId, classId, 2)
end

--返回添加好友组
function Gas2Gac:ReturnAddFriendClass(Conn, bFlag, classId, className)
	if(bFlag) then
		local base = g_GameMain.m_AssociationBase
		local tblInfo		= {}
		tblInfo.className	= className
		tblInfo.classId		= classId
		table.insert(base.m_tblFriendListClass, tblInfo)
		g_GameMain.m_AssociationBase:ResetClassIndex()
	end
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:SetFriendList()
end

------查找玩家------
--返回精确查找玩家
function Gas2Gac:ReturnSearchPlayer(Conn, id, name, level, bOnline)
	local context	= {id, name, level, bOnline}
	local tbl		= g_GameMain.m_AssociationFindWnd.m_AssociationRetFindPlayerWnd.m_tblBeFoundPlayerInfo
	table.insert(tbl, context)
end

--返回精确查找玩家结束
function Gas2Gac:ReturnSearchPlayerEnd(Conn)
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindPlayerWnd:ShowWnd(true)
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindPlayerWnd:DrawPlayerList()
end

------对好友操作------
--返回添加好友
function Gas2Gac:ReturnAddFriendToClass(Conn, classId, playerId, playerName, level, nCamp, nClass, nTeamSize, sScene, nBeOnline,nTongId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:RetAddFriendToClass(classId, playerId, playerName, level, nCamp, nClass, nTeamSize, sScene, nBeOnline,nTongId)
end

--得到加为好友的邀请
function Gas2Gac:ReturnInviteToBeFriend(Conn, sInviterName, nJoinClassId, nInviterId)
	if( g_GameMain.m_AssociationBase:IsInBlackList(nInviterId) ) then
		return
	end
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateFriendInvMsgMinimum(nInviterId, sInviterName, nJoinClassId)
end

--返回删除好友
function Gas2Gac:ReturnDeleteMyFriend(Conn, bFlag, nFriendId, nClassId)
	if(bFlag) then
		g_GameMain.m_AssociationBase:DeleteFriendById(nFriendId, nClassId)
	end
end

--被好友删除
function Gas2Gac:ReturnBeDeleteByFriend(Conn, playerId, classId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:DeleteFriendById(playerId, classId)
end

--返回移动到黑名单
function Gas2Gac:ReturnMoveToBlackList(Conn, bFlag)
	if(bFlag) then
		if(g_GameMain.m_AssociationBase.RetMoveToBlackList) then
			g_GameMain.m_AssociationBase:RetMoveToBlackList()
		end
	end
end

--返回添加到黑名单
function Gas2Gac:ReturnAddToBlackList(Conn, playerId, bFlag, playerName)
	if( not bFlag or g_GameMain.m_AssociationWnd.m_bNotInitBefore ) then
		return
	end
	local base = g_GameMain.m_AssociationBase
	local tblItem = g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
	
	local tblInfo = {}
	tblInfo.playerId		= playerId
	tblInfo.playerName		= playerName
	tblInfo.playerLevel		= 0
	tblInfo.nPlayerCamp		= nil
	tblInfo.nPlayerClass	= nil
	tblInfo.nTeamSize		= 0
	tblInfo.sScene			= ""
	tblInfo.nOnline			= 2
	table.insert(base.m_tblBlackList, tblInfo)
	
	tblItem[#tblItem]:ResetItem()
end

--返回移动到组
function Gas2Gac:ReturnChangeToClass(Conn, bFlag)
	if(bFlag) then
		g_GameMain.m_AssociationBase:RetChangeToClass()
	end
end

--返回删除组
function Gas2Gac:ReturnDeleteFriendClass(Conn, bFlag, nClassId)
	local base		= g_GameMain.m_AssociationBase
	local index		= base.m_tblFriendClassIndex[nClassId]
	local tblFriend	= base.m_tblFriendListClass[index]
	
	for i = 1, #(tblFriend) do
		table.insert(base.m_tblFriendListClass[1], tblFriend[i])
	end
	table.remove(base.m_tblFriendListClass, index)
	base:ResetClassIndex()
	base:SortFriendListByBeOnline(1)
	g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1]:SetFriendList()
end

--返回重命名好友组
function Gas2Gac:ReturnRenameFriendClass(Conn, bFlag, nClassId, sNewName, sOldName)
	local base		= g_GameMain.m_AssociationBase
	local index		= base.m_tblFriendClassIndex[nClassId]
	local tblFriend	= base.m_tblFriendListClass[index]
	local listWnd	= g_GameMain.m_AssociationWnd.m_AssociationListWnd
	
	if bFlag then
		tblFriend.className = sNewName
	end
	if(1 == listWnd.m_ListPage) then
		local titleWnd	= listWnd.m_tblList[1].m_tblItemWnd[index]
		titleWnd:ResetTitle()
	end
end

--返回删除黑名单成员
function Gas2Gac:ReturnDeleteBlackListMem(Conn, bFlag, blackMemID)
	if(bFlag) then
		local tbl = g_GameMain.m_AssociationBase.m_tblBlackList
		local tblItem = g_GameMain.m_AssociationWnd.m_AssociationListWnd.m_tblList[1].m_tblItemWnd
		for i, v in ipairs(tbl) do
			if( v.playerId == blackMemID ) then
				table.remove(tbl, i)
				tblItem[#tblItem]:ResetItem()
			end
		end
	end
end

------聊天------
--私聊消息
function Gas2Gac:PrivateChatReceiveMsg(Conn, fromId, fromName, text)
	g_GameMain.m_AssociationBase:ReceivePrivateChatMsg(fromId, fromName, text)
end

function Gas2Gac:ReceiveSendOfflineMsg(Conn, fromId, fromName, time, text) --玩家id、玩家名称、发消息的时间、内容
	g_GameMain.m_AssociationBase:ReceivePrivateChatMsg(fromId, fromName, text, time)
end

function Gas2Gac:ReturnGetMemberIdbyNameForPrivateChat(Conn, memberId, memberName, str)
	local wnd = g_GameMain.m_AssociationWnd:CreateAssociationPriChatWnd(memberId, memberName)
	if(wnd and "" ~= str) then
		wnd:SendMsg(str)
	end
end

--群聊消息
function Gas2Gac:PublicChatReceiveMsg(Conn, groupId, playerName, text)
	if(g_GameMain.m_AssociationBase.m_RefuseGroupMsgGroupId[groupId] ~= 0) then return end
	local wnd = g_GameMain.m_AssociationWnd:FindAssociationPubChatWndByGroupId(groupId)
	if(wnd) then
		wnd:AddNewLineInfoWithTime(playerName, text)
		g_GameMain.m_MsgListParentWnd.m_MsgListWnd:FlashOneItemByTypeAndId("群聊", groupId)
	else
		g_GameMain.m_MsgListParentWnd.m_MsgListWnd:InsertChatMsg("群聊", groupId, playerName, text)
	end
end

--*************************************************************************************************
--好友群
--*************************************************************************************************
--返回好友群组开始
function Gas2Gac:ReturnGetAllFriendGroupBegin(Conn)
	g_GameMain.m_AssociationBase:InitGroup()
end

--返回好友群组
function Gas2Gac:ReturnGetAllFriendGroup(Conn, groupId, groupName, groupType, groupLable, groupDeclare,uRefuseMsgFlag)
	local base = g_GameMain.m_AssociationBase
	table.insert(base.m_tblFriendGroup, {{groupName, groupId, groupType, groupLable, groupDeclare}, {}})
	base.m_RefuseGroupMsgGroupId[groupId] = uRefuseMsgFlag
end

--返回好友群组结束
function Gas2Gac:ReturnGetAllFriendGroupEnd(Conn)
	g_GameMain.m_AssociationBase:ResetGroupIndex()
end

--返回所有好友群的成员信息
function Gas2Gac:ReturnGetAllGroupMemInfo(Conn, memberId, memberName, memberType, groupId, bOnline)
	g_GameMain.m_AssociationBase:AddGroupMem(memberId, memberName, memberType, groupId, bOnline)
end

--返回所有好友群的成员信息结束
function Gas2Gac:ReturnGetAllGroupMemInfoEnd(Conn, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:AddGroupMemEnd(groupId)
end

function Gas2Gac:ReturnAddGroupMem(Conn, memberId, memberName, memberType, groupId, bOnline)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:AddGroupMem(memberId, memberName, memberType, groupId, bOnline)
	g_GameMain.m_AssociationBase:AddGroupMemEnd(groupId)
end

--收到被同意加入好友群的信息结束
function Gas2Gac:ReturnRequestJoinIntoFriendGroupEnd(Conn)
	local base = g_GameMain.m_AssociationBase
	base:ResetFriendGroupList()
end

--被邀请后加入群
function Gas2Gac:BeInviteToGroupEnd(Conn)
	local base = g_GameMain.m_AssociationBase
	base:ResetFriendGroupList()
end

--返回创建好友群
function Gas2Gac:ReturnCreateFriendGroup(Conn, groupId)
	if(0 == groupId) then
		MsgClient(5002)
	else
		g_GameMain.m_AssociationBase:RetCreateFriendGroup(groupId)
		g_GameMain.m_AssociationBase.m_RefuseGroupMsgGroupId[groupId] = 0
	end
end

--返回修改群公告
function Gas2Gac:ReturnChangeGroupDeclare(Conn, groupId, groupDeclare)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	local base				= g_GameMain.m_AssociationBase
	local tblFriendGroup	= base.m_tblFriendGroup
	local tblGroupIndex		= base.m_tblFriendGroupIndex
	local groupIndex		= tblGroupIndex[groupId]
	tblFriendGroup[groupIndex][1][5] = groupDeclare
	
	local wnd = g_GameMain.m_AssociationWnd:FindAssociationPubChatWndByGroupId(groupId)
	if(wnd) then
		wnd.m_DeclareRichText:SetWndText(groupDeclare)
	end
end

--返回设置开启/关闭群消息
function Gas2Gac:ReturnRefuseGroupMsgOrNot(Conn,groupId,uRefuseOrNot)
	g_GameMain.m_AssociationBase.m_RefuseGroupMsgGroupId[groupId] = uRefuseOrNot
end

--收到入群申请
function Gas2Gac:RecevieRequestJoinIntoFriendGroup(Conn, nInviterId, sInviterName, nGroupId)
	if( g_GameMain.m_AssociationBase:IsInBlackList(nInviterId) ) then
		return
	end
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateFriendGroAppMsgMinimum(nInviterId, sInviterName, nGroupId)
end

--收到入群邀请
function Gas2Gac:ReceiveInviteToGroup(Conn, inviterId, inviterName, groupId, groupName)
	if( g_GameMain.m_AssociationBase:IsInBlackList(inviterId) ) then
		return
	end
	g_GameMain.m_MsgListParentWnd.m_MsgListWnd:CreateFriendGroInvMsgMinimum(inviterId, inviterName, groupId, groupName)
end

--返回离开群
function Gas2Gac:ReturnLeaveGroup(g_Conn, groupId)
	local base = g_GameMain.m_AssociationBase
	base:LeaveGroup(groupId)
	base:DeleteGroupMsgCallInfo(groupId)
end

--收到被踢出群的信息
function Gas2Gac:ReceiveBeKickOutOfGroup(g_Conn, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	local base = g_GameMain.m_AssociationBase
	base:LeaveGroup(groupId)
	base:DeleteGroupMsgCallInfo(groupId)
end

--收到某玩家离开群
function Gas2Gac:ReceiveMemberLeaveGroup(Conn, memberId, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	local base				= g_GameMain.m_AssociationBase
	local tblFriendGroup	= base.m_tblFriendGroup
	local tblGroupIndex		= base.m_tblFriendGroupIndex
	local groupIndex		= tblGroupIndex[groupId]
	local tbl = tblFriendGroup[groupIndex][2]
	for i = 1, #tbl do
		if(tbl[i][2] == memberId) then
			table.remove(tbl, i)
			base:ResetFriendGroupList()
			break
		end
	end
	base:FindAndReDrawPubChatWnd(groupId)
end

--收到解散群的信息
function Gas2Gac:ReturnDisbandGroup(Conn, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	local base				= g_GameMain.m_AssociationBase
	local tblFriendGroup	= base.m_tblFriendGroup
	local tblGroupIndex		= base.m_tblFriendGroupIndex
	local groupIndex		= tblGroupIndex[groupId]
	table.remove(tblFriendGroup, groupIndex)
	base:ResetGroupIndex()
	base:ResetFriendGroupList()
	base:FindAndClosePubChatWnd(groupId)
	base:DeleteGroupCallInfo(groupId)
end

--收到更改某人权限的信息
function Gas2Gac:ReturnSetCtrlType(Conn, memberId, groupId, ctrlType)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	local base				= g_GameMain.m_AssociationBase
	local tblFriendGroup	= base.m_tblFriendGroup
	local tblGroupIndex		= base.m_tblFriendGroupIndex
	local groupIndex		= tblGroupIndex[groupId]
	local tbl = tblFriendGroup[groupIndex][2]
	if(1 == ctrlType) then --如果是转让群主, 就先删除原来的群主记录
		for i = 1, #tbl do
			if(1 == tbl[i][3]) then
				tbl[i][3] = 3
				break
			end
		end
		if(1 == tblFriendGroup[groupIndex][1][6] and memberId ~= g_MainPlayer.m_uID) then
			tblFriendGroup[groupIndex][1][6] = 3
			base:ResetCtrlTypeList()
		end
	end
	for i = 1, #tbl do
		if(memberId == tbl[i][2]) then
			tbl[i][3] = ctrlType
			break
		end
	end
	if(memberId == g_MainPlayer.m_uID) then
		tblFriendGroup[groupIndex][1][6] = ctrlType
		base:ResetCtrlTypeList()
	end
	base:FindAndReDrawPubChatWnd(groupId)
end

--返回上线通知好友群成员
function Gas2Gac:NotifyFriendGroupOnline(Conn, memberId, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:NotifyGroupMemberBeOnline(memberId, groupId, 1)
end

--返回下线通知好友群成员
function Gas2Gac:NotifyFriendGroupOffline(Conn, memberId, groupId)
	if g_GameMain.m_AssociationWnd.m_bNotInitBefore then
		return
	end
	g_GameMain.m_AssociationBase:NotifyGroupMemberBeOnline(memberId, groupId, 2)
end

------查找好友群------
--返回精确查找好友组
function Gas2Gac:ReturnSearchGroup(Conn, id, name, masterName)
	local context	= {id, name, masterName}
	local tbl		= g_GameMain.m_AssociationFindWnd.m_AssociationRetFindGroupWnd.m_tblBeFoundGroupInfo
	table.insert(tbl, context)
end

--返回精确查找好友组结束
function Gas2Gac:ReturnSearchGroupEnd(Conn)
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindGroupWnd:ShowWnd(true)
	g_GameMain.m_AssociationFindWnd.m_AssociationRetFindGroupWnd:DrawGroupList()
end

-----个人信息------
--返回自己的个人信息
function Gas2Gac:ReturnGetAssociationPersonalInfo(Conn, mood, showSen, strInterest, bodyShape, personality, makeFriendsState, style, detail)
	local context = {mood, showSen, strInterest, bodyShape, personality, makeFriendsState, style, detail}
	local personalInfoWnd = g_GameMain.m_AssociationPersonalInfoWnd
	if(personalInfoWnd) then--如果个人信息面板打开了,就刷新个人信息面板
		personalInfoWnd:ShowInfo(context)
	end
	g_GameMain.m_AssociationWnd:ChangePersonalInfo(showSen)
end

--返回保存个人信息成功
function Gas2Gac:ReturnSaveAssociationPersonalInfo(Conn)
	g_GameMain.m_AssociationPersonalInfoWnd:CallBackFuncChangePersonalInfo()
end

--[[
mood              --心情               I
showSen           --寄语               s
class             --职业               I
level             --等级               I
tongName          --佣兵团             s
cofcName          --商会               s
location          --所处的区域（地图） I
strInterest       --兴趣               s
bodyShape         --体型               I
personality       --个性               I
makeFriendsState  --交友状态           I
style             --类型               I
detail			--个人说明				s
--]]

--返回某玩家的个人信息
function Gas2Gac:ReturnGetAssociationMemberInfo(Conn, memberId, mood, showSen, class, level, tongName, cofcName, location, strInterest, bodyShape, personality, makeFriendsState, style, detail)
	local ClassStr = g_GameMain.m_DisplayCommonObj:GetPlayerClassForShow(class)
	local context = {mood, showSen, ClassStr, level, tongName, cofcName, location,
					strInterest, bodyShape, personality, makeFriendsState, style, detail}
	local tbl = g_GameMain.m_tblPerInfoShowWnd
	for i = 1, #tbl do
		if(memberId == tbl[i].m_MemberId) then
			tbl[i]:ShowInfo(context)
			break
		end
	end
end

--返回修改心情寄语
function Gas2Gac:ReturnChangeShowSentence(Conn, bFlag, sNewSen)
	g_GameMain.m_AssociationWnd:RetChangeShowSen(bFlag, sNewSen)
end
