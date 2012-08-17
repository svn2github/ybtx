gas_require "relation/RevertValidateMgrInc"
function RevertValidateMgr:Ctor()
	self.m_tblRevertFun = 
		{
			["MakeTeam"] = 1, --组队
			["RecommendJoinTong"] = 2, --引荐加入帮会
			["InviteJoinTong"] = 3, --邀请加入帮会
			["AddFriend"] = 4, --申请加为好友
		}
end

function RevertValidateMgr:GetFunIndexByStr(sFunStr)
	return self.m_tblRevertFun[sFunStr]
end