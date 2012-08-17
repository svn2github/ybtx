--PS：10级或以下的角色删除后在界面中保留7天；10级以上30级以下保留20天；31级以上保留60天。
function GetDelCharSavedTimeByLevel(level)
	if level < 11 then
		return 7
	elseif level < 31 then
		return 20
	else
		return 60
	end
end