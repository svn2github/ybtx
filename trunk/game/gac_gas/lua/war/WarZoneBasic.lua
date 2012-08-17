gac_gas_require "war/WarZoneBasicInc"

function CWarZoneBasic:GetStationDefaultCamp(stationId)
	return math.floor(stationId / 100)
end

function CWarZoneBasic:GetStationWarLine(stationId)
	return math.floor((stationId % 100)/10)
end