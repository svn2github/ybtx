gac_gas_require "fight/target_info/SmashMgrInc"
cfg_load "image_res/SmashImage_Common"

function CSmashInfoMgr:Ctor()
	self.m_tblAttackTypeString = {
		[EAttackType.eATT_Puncture]		= "护甲",
		[EAttackType.eATT_Chop]			= "护甲",
		[EAttackType.eATT_BluntTrauma]	= "护甲",
		[EAttackType.eATT_Nature]		= "自然抗",
		[EAttackType.eATT_Destroy]		= "破坏抗",
		[EAttackType.eATT_Evil]			= "暗黑抗"	} --这里的字符串要和SmashImage_Common表中相同
end

function CSmashInfoMgr:GetSmashImage(uAttackType, fSmashRate)
	local sAttackType = self.m_tblAttackTypeString[uAttackType]
	local tblAttackType = SmashImage_Common(sAttackType)
	for i, v in ipairs( tblAttackType:GetKeys() ) do
		if(fSmashRate > tblAttackType(v, "LowerLimit") and fSmashRate <= tblAttackType(v, "UpperLimit")) then
			return tblAttackType(v, "ImagePathEnable"), v
		end
	end
end

