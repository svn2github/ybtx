
----------------------------------------好友群-------------------------------------------
--@brief 给群里添加成员
function Db2Gas:AddMemberToFriendGroup(data)
	CGasFriendGroupBasic.AddMemberToFriendGroup(data)
end


--@brief 解散群
function Db2Gas:DismissalGroup(data)
	CGasFriendGroupBasic.DismissalGroup(data)
end

--@brief 踢人
function Db2Gas:DelMember(data)
	CGasFriendGroupBasic.DelMember(data)
end
