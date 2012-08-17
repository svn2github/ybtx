
-- ºÏ²¢×Ö´®
function StrJoin(delimiter, list)
	local MyLen = table.getn(list)
	if MyLen == 0 then
		return ""
	end
	local string = list[1]
	for i = 2, MyLen do
		string = string .. delimiter .. list[i]
	end
	return string
end


-- ·Ö¸î×Ö´®
function StrSplit(pattern, str)
	if str=="" then
		return nil
	elseif string.match(str, pattern)==nil then
		return {str}
	end

	local outResults = {}
	local theStart = 1
	local Start, End = string.find(str, pattern, 1)

	while Start do
		table.insert(outResults, string.sub(str, theStart, Start-1))
		theStart = End + 1
		Start, End = string.find(str, pattern, theStart)
	end

	table.insert(outResults, string.sub(str, theStart))
	return outResults
end


