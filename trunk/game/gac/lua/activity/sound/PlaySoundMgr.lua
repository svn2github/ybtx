cfg_load "sound/LodingSound_Client"
CPlaySoundMgr = class()

function CPlaySoundMgr.GetActionSound(ActionName)
	return LodingSound_Client(ActionName, "SoundName")
end

function CPlaySoundMgr.PlayLodingSound(ActionName)
	local MusicName = CPlaySoundMgr.GetActionSound(ActionName)
	if MusicName and MusicName ~= "" then
		PlayCue(MusicName)
--		print("PlayCue",ActionName,MusicName)
	end
end

function CPlaySoundMgr.StopLodingSound(ActionName)
	local MusicName = CPlaySoundMgr.GetActionSound(ActionName)
	if MusicName and MusicName ~= "" then
		StopCue(MusicName)
--		print("StopCue",ActionName,MusicName)
	end
end