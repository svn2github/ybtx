character_require "loader"

cfg_load_single "model_res/NpcRes_Client"
cfg_load_single "npc/Npc_Common"
cfg_load_single "model_res/IntObjRes_Client"
cfg_load_single "int_obj/IntObj_Common"
cfg_load_single "model_res/PropRes_Common"
cfg_require "model_res/NpcRes_Common"

print"从策划文件加载角色模型配置表"
local tblRes = NpcRes_Client
local tblCharacter = NpcRes_Common
local tblPropRes = PropRes_Common
print"两个表local完了"

function CreatePiece(IndexName)

	if IndexName ~= "" and  IndexName ~= nil then
		local PieceTbl =  tblRes[IndexName]
		--Ast(PieceTbl,"CharacterRes_Client.xml没有 "..IndexName.." 这一项")
		if PieceTbl == nil then
			return nil
		end
		local piece  = CModelPiece:new()
				
		piece:SetIndexName(PieceTbl.PartName)
		piece:SetModelFileName(PieceTbl.BoyModelFile)
		piece:SetPieceName(PieceTbl.PieceName)
		piece:SetRenderStyle(PieceTbl.RenderStyle)
		--db()
--		if PieceTbl.HidePiece ~= "" then
--		local HidePiece_Str="{"..PieceTbl.HidePiece.."}"
--		local HideTbl = loadstring("return " .. HidePiece_Str)()
--			for i,v in pairs(HideTbl) do
--				piece:AddHidePiece(HideTbl[i])
--			end
--		end
		return piece
	end
end
function GetCharacterPiece(tblModel)
		local resPiece = {}
		resPiece.bodyPiece = CreatePiece(tblModel.Body)
		resPiece.headPiece = CreatePiece(tblModel.Face)
		resPiece.armPiece = CreatePiece(tblModel.Arm)	
		resPiece.shoePiece = CreatePiece(tblModel.Shoe)	
		resPiece.weaponPiece = CreatePiece(tblModel.Weapon)
		resPiece.caestusPiece = CreatePiece(tblModel.Caestus)	
		resPiece.shoulderPiece = CreatePiece(tblModel.Shoulder)	
		resPiece.hairPiece = CreatePiece(tblModel.Hair)		
		resPiece.hatPiece = CreatePiece(tblModel.Armet)
		
		return resPiece
end

for i, v in pairs(IntObj_Common) do
	local character = CCharacter:new()
	local modeName = v.ModelName
	local modeTbl = IntObjRes_Client[modeName]
	character:SetName(i)
	character:SetScaling(modeTbl.Scaling)
	character:SetAniFileName(modeTbl.AniFile)
	local piece  = CModelPiece:new()
	
	piece:SetIndexName(modeName)
	piece:SetModelFileName(modeTbl.ModelFile)
	piece:SetPieceName(modeTbl.PieceName)
	piece:SetRenderStyle(modeTbl.RenderStyle)
		
	character:AddModelPiece(piece)
	character:SetModPiece(piece)
	local Cache = GetCCache()
	Cache:AddCharacter(character,1)
end

for i,v in pairs(tblCharacter) do
		local character = CCharacter:new()
		character:SetName(tblCharacter[i].NpcName)
		character:SetScaling(tblCharacter[i].Scaling)
		character:SetAniFileName("character/ani/npc/" .. tblCharacter[i].AniFileName .. ".ske")
		local resTbl = GetCharacterPiece(tblCharacter[i])
		--Ast(resTbl)
		--db()
		for i,v in pairs(resTbl) do
			character:AddModelPiece(resTbl[i])
		end
		character:SetModPiece(resTbl.bodyPiece)
		local Cache = GetCCache()
		Cache:AddCharacter(character,0)
end

for i,v in pairs(Npc_Common) do
	local Cache = GetCCache()
	Cache:AddNPC(i,v.AIType);
end
	
for i,v in pairs(tblPropRes) do
		local playerModel = CPlayerModelPart:new()
		playerModel:SetName(tblPropRes[i].Name)
		modeFileName = tblPropRes[i].HumanMaleModelFile .. "," .. tblPropRes[i].HumanFeMaleModelFile .. "," .. tblPropRes[i].OrcModelFile
		playerModel:SetModelFileName(modeFileName)
		playerModel:SetAniFileName(tblPropRes[i].AniFile)
		playerModel:SetEffectFileName(tblPropRes[i].EffectFile)
		playerModel:SetPieceName(tblPropRes[i].PieceName)
		playerModel:SetRenderStyle(tblPropRes[i].RenderStyle)
		playerModel:SetHumanMaleHidePiece(tblPropRes[i].HumanMaleHidePiece)
		playerModel:SetHumanFeMaleHidePiece(tblPropRes[i].HumanFeMaleHidePiece)
		playerModel:SetDwarfHidePiece(tblPropRes[i].DwarfHidePiece)
		playerModel:SetElfHidePiece(tblPropRes[i].ElfHidePiece)
		playerModel:SetOrcHidePiece(tblPropRes[i].OrcHidePiece)
		--playerModel:SetDynamic(tblPropRes[i].Dynamic)
		playerModel:SetTransform(tblPropRes[i].Transform)
		local Cache = GetCCache()
		Cache:AddPlayerModelPart(playerModel)
end

