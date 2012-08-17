gas_require "world/tong_area/TongChallengeFbMgrInc"
g_TongChallengeFbMgr = CTongChallengeFbMgr:new()
g_TongChallengeFbMgr.m_TongChallengeSceneName = "∞Ôª·ÃÙ’Ω±æ"

g_TongChallengeFbMgr.m_TongChallengeSceneTbl = {}

local ChallengerEnterPos = {70, 74}
local ChallengerCenterPos = {90, 87}

local TargetEnterPos = {202, 296}
local TargetCenterPos = {202, 274}

function CTongChallengeFbMgr:GetChallengerEnterPos()
	return ChallengerEnterPos
end

function CTongChallengeFbMgr:GetChallengerCenterPos()
	return ChallengerCenterPos
end

function CTongChallengeFbMgr:GetTargetEnterPos()
	return TargetEnterPos
end

function CTongChallengeFbMgr:GetTargetCenterPos()
	return TargetCenterPos
end