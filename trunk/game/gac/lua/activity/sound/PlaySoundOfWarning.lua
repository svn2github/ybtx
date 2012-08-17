cfg_load "sound/SkillFailSound_Client"

CPlaySoundOfWarning = class()

function CPlaySoundOfWarning.GetVoiceSourceFileSuffix()
	local race = g_GameMain.m_PlayerInfoTbl.m_PlayerClassID
	local sex = g_GameMain.m_PlayerInfoTbl.m_PlayerSex
	local suffix
	if race == EClass.eCL_DwarfPaladin then    --矮人
		suffix = "ar"
	elseif race == EClass.eCL_ElfHunter then   --精灵
		suffix = "jl"
	elseif race == EClass.eCL_OrcWarrior then  --兽人
		suffix = "sr"
	else                                       --人类
		if sex == ECharSex.eCS_Male then --角色男
			suffix = "m"
		else
			suffix = "wm"
		end
	end
	return suffix
end

function CPlaySoundOfWarning.PlaySoundOfNotice(voiceSourceId)
	local prefix = SkillFailSound_Client(voiceSourceId,"Voice")
	local noticeSound
	if prefix and prefix ~= "" then
		local suffix = CPlaySoundOfWarning.GetVoiceSourceFileSuffix()
		noticeSound = prefix .. suffix
		if CueIsStop(noticeSound) then
			PlayCue(noticeSound)
		end
	end
end