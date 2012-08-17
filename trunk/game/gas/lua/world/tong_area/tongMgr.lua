gas_require "world/tong_area/tongMgrInc"


function AddBuildID(EntityID, uID)
--	if not g_Build_uID2EntityID[uID] then 
		g_Build_uID2EntityID[uID] = EntityID
--		db()
--	end	
end

function GetBuidlEntityIDByuID( uID)
	if not g_Build_uID2EntityID[uID] then
		return nil
	end
	return g_Build_uID2EntityID[uID]
end


function DelBuildEntityIDByuID( uID)
	if g_Build_uID2EntityID[uID] then
		g_Build_uID2EntityID[uID] = nil
	end 
end

function DelEntityID( EntityID)
	for uID, nEntityID in pairs(g_Build_uID2EntityID) do 
		if nEntityID == EntityID then
			g_Build_uID2EntityID[uID] = nil 
			return
		end		
	end
end
