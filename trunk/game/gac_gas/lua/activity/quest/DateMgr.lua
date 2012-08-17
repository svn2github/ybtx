

function TwoTimesIsSameDate(DateTime1,DateTime2)
	if DateTime1==nil or DateTime2==nil then
		return false
	end
	local date1 = os.date("*t",DateTime1)
	local date2 = os.date("*t",DateTime2)
	if date1.year == date2.year and date1.month == date2.month and date1.day == date2.day then
		return true
	end
	return false
end