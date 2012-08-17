gac_require "framework/texture_mgr/DynImageMgrInc"
cfg_load "image_res/ClassImage_Common"
cfg_load "image_res/FaceImage_Client"
lan_load "player/Lan_NewRoleProperty_Common"
lan_load "player/Lan_Class_Common"

function CDynImageMgr:AddImageByIP(wnd, imagePath, IP)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:AddImage(wnd:GetGraphic(), -1,  imagePath, nil, CFPos:new(), "0xffffffff", 15 )
	wnd:SetWndBkImage( Flag, DefaultImage )
end

function CDynImageMgr:AddImageNeedCut(wnd, imagePath, IP, Rect, Pos)
	local Flag = IMAGE_PARAM:new()
	Flag.CtrlMask = SM_BS_BK
	Flag.StateMask = IP
	local DefaultImage = WND_IMAGE_LIST:new()
	DefaultImage:SetGUIAsynEnable(true)
	DefaultImage:AddImage(wnd:GetGraphic(), -1,  imagePath, Rect, Pos, "0xffffffff", 15 )
	wnd:SetWndBkImage( Flag, DefaultImage )
end

function CDynImageMgr:GetClassPic(nCamp, nClass)
	local tbl				= {"艾米帝国", "神圣教廷", "修斯帝国"}
	local sCamp				= tbl[nCamp]
	local sClass			= g_GetPlayerClassNameByID(nClass)
	
	local sEnableImg		= ClassImage_Common(sCamp, sClass, "ImagePathEnable")
	local sDisableImg		= ClassImage_Common(sCamp, sClass, "ImagePathDisable")
	local sCampDisplayName	= Lan_NewRoleProperty_Common(nCamp,"DisplayName")
	local sClassDisplayName	= Lan_Class_Common(MemH64(sClass), "DisplayName")
	return sEnableImg, sDisableImg, sCampDisplayName, sClassDisplayName
end

function CDynImageMgr:GetFaceImage(nIndex)
	return FaceImage_Client(nIndex, "ImagePath")
end

function CDynImageMgr:GetDisableFaceImage(nIndex)  
	return FaceImage_Client(nIndex, "DisableImagePath")
end

function CDynImageMgr:GetClassMetalPic(nCamp, nClass)
	local tbl		= {"艾米帝国", "神圣教廷", "修斯帝国"}
	local sCamp		= tbl[nCamp]
	local sClass	= g_GetPlayerClassNameByID(nClass)
	return ClassImage_Common(sCamp, sClass, "ImagePathMetal")
end

function CDynImageMgr:GetClassMetalWithoutCampPic(nCamp, nClass)
	local tbl		= {"艾米帝国", "神圣教廷", "修斯帝国"}
	local sCamp		= tbl[nCamp]
	local sClass	= g_GetPlayerClassNameByID(nClass)
	return ClassImage_Common(sCamp, sClass, "ImagePathMetalWithoutCamp")
end

function g_GetLansClass(sClass)
	return Lan_Class_Common(MemH64(sClass),"DisplayName")
end

g_DynImageMgr = g_DynImageMgr or CDynImageMgr:new()
