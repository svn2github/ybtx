
require "libsrc/StrOP"


function Main(fileName)
	local fh = assert(io.open(fileName, "r"))
	local fw = assert(io.open("result.out", "w"))

	local List = {}
	local str;
	local i=0;
	for line in fh:lines() do
		local temp = StrSplit("\t", line)

		str = temp[3]..temp[2]
		if temp[7]~="0" then	
			if List[str] == nil then
				List[str] = {temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7], 1}
			else
				List[str][6] = List[str][6] + temp[6]
				List[str][7] = List[str][7] + temp[7]
				List[str][8] = List[str][8] + 1
			end
		end
	end

	fw:write("Stack Level\tFile\tFunction\tDefined Line\tCurrent Line\tLocal Time\tTotal Time\tCall Count\n")
	for key,value in pairs(List) do
		fw:write(List[key][1],"\t",List[key][2],"\t",List[key][3],"\t",List[key][4],"\t",List[key][5],"\t",List[key][6],"\t",List[key][7],"\t",List[key][8],"\n")
	end


	fw:close()
	fh:close()
end


if arg[1]==nil then
	print("Please provide a file name in command line parameter 1")
else
	Main(arg[1])
end

