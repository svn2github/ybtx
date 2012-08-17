CNpcDialogBoxCopy = class( SQRDialog )

CNpcDialogBoxCopy.DlgTextType =
{
	["锻造"] = {18001, 18007},
	["铸甲"] = {18002, 18008},
	["烹饪"] = {18003, 18009},
	["制药"] = {18004, 18010},
	["采矿"] = {18005, 18011},
	["花卉"] = {18006, 18012},
	["工艺"] = {18013, 18014},
}

CNpcDialogBoxCopy.ENpcShowDlg ={}

CNpcDialogBoxCopy.ENpcShowDlg["锻造"] =
{
	[1] = {GetStaticTextClient(3015), false},	--"学习锻造"
	[2] = {GetStaticTextClient(3016), true},	--"升级锻造"
	[3] = {GetStaticTextClient(3017), true},	--"遗忘锻造"
	[4] = {GetStaticTextClient(3018), true},	--"锻造专精学习"
}

CNpcDialogBoxCopy.ENpcShowDlg["铸甲"] =
{
	[1] = {GetStaticTextClient(3019), false},	--"学习铸甲"
	[2] = {GetStaticTextClient(3020), true},	--"升级铸甲"
	[3] = {GetStaticTextClient(3021), true},	--"遗忘铸甲"
	[4] = {GetStaticTextClient(3022), true},	--"铸甲专精学习"
}

CNpcDialogBoxCopy.ENpcShowDlg["烹饪"] =
{
	[1] = {GetStaticTextClient(3023), false},	--"学习烹饪"
	[2] = {GetStaticTextClient(3024), true},	--"升级烹饪"
	[3] = {GetStaticTextClient(3025), true},	--"遗忘烹饪"
}

CNpcDialogBoxCopy.ENpcShowDlg["制药"] =
{
	[1] = {GetStaticTextClient(3026), false},	--"学习制药"
	[2] = {GetStaticTextClient(3027), true},	--"升级制药"
	[3] = {GetStaticTextClient(3028), true},	--"遗忘制药"
}

CNpcDialogBoxCopy.ENpcShowDlg["采矿"] =
{
	[1] = {GetStaticTextClient(3029), false},	--"学习采矿"
	[2] = {GetStaticTextClient(3030), true},	--"升级采矿"
	[3] = {GetStaticTextClient(3031), true},	--"遗忘采矿"
}

CNpcDialogBoxCopy.ENpcShowDlg["花卉"] =
{
	[1] = {GetStaticTextClient(3032), false},	--"学习花卉"
	[2] = {GetStaticTextClient(3033), true},	--"升级花卉"
	[3] = {GetStaticTextClient(3034), true},	--"遗忘花卉"
}

CNpcDialogBoxCopy.ENpcShowDlg["工艺"] =
{
	[1] = {GetStaticTextClient(3038), false},	--"学习锻造"
	[2] = {GetStaticTextClient(3039), true},	--"升级锻造"
	[3] = {GetStaticTextClient(3040), true},	--"遗忘锻造"
	[4] = {GetStaticTextClient(3041), true},	--"锻造专精学习"
}

function CNpcDialogBoxCopy:InitFunLink()
	self.linkfun		= {}
	self.linkfun[1]	= self.Learn
	self.linkfun[2]	= self.Upgrade
	self.linkfun[3]	= self.Wash
	self.linkfun[4]	= self.OpenExpert
end
