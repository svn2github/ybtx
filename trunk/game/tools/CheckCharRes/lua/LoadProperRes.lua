require "common/Loader/loader"
load_single "model_res/PropRes_Common"
load_single "item/StaticWeapon_Common"



function g_CheckProperRes()
	print"加载ProperRes配置表"
	local LineNo = 0;
	
	for i, v in pairs(PropRes_Common) do
		local sPropRes = v
		LineNo = LineNo + 1
		PropResChecker_CheckPropResLine(LineNo, sPropRes.Name, sPropRes.ModelFile, sPropRes.AniFile,
				sPropRes.EffectFile, sPropRes.PieceName, sPropRes.RenderStyle,
				sPropRes.HidePiece, sPropRes.Dynamic ~= 0)
		
	end
	
	print "ProperRes配置表检查完毕！"
end

function g_CheckWeapon()
	print"加载StaticWeapon配置表"
	local LineNo = 0;
	
	for i, v in pairs(StaticWeapon_Common) do
		LineNo = LineNo + 1
		PropRes_Line = PropRes_Common[v.ResID]
		if PropRes_Line == nil then
			print("[" .. v.Name .. "] 行的ResID [" .. v.ResID .. "] 在PropRes表中找不到")
		else
			PropResChecker_CheckWeaponLine(LineNo, v.Name, v.ResID, v.AttackEffect, PropRes_Line.AniFile)
			if v.AssociateResID ~= "" then
				PropRes_Line = PropRes_Common[v.AssociateResID]
				if PropRes_Line == nil then
					print("[" .. v.Name .. "] 行的ResID [" .. v.AssociateResID .. "] 在PropRes表中找不到")
				else
					PropResChecker_CheckWeaponLine(LineNo, v.Name, v.AssociateResID, v.AttackEffect, PropRes_Line.AniFile)
				end
			end
		end
	end
	
	print "StaticWeapon配置表检查完毕！"
end	


	
	

PropResChecker_BeginCheck()
--g_CheckProperRes()
--g_CheckWeapon()
CheckAllConfig()
PropResChecker_EndCheck()



