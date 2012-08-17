CCommerceCommon = class ()

function CCommerceCommon:CheckInputLevel(wnd, limitNo)
    local inputStr = wnd:GetWndText()
    local findResult = string.find(inputStr, "[^0123456789]")
	if  findResult ~= nil  then
		inputStr = string.sub(inputStr, 1, findResult-1)
		wnd:SetWndText( inputStr )
	end

	local input = tonumber(inputStr)
	if input == nil then
	   wnd:SetWndText("")
	   return 
	end
	if input == 0 or input > 150 then
		wnd:SetWndText( limitNo )
	end
end