cfg_load "image_res/Image_Client"

CImageMgr = class()

function CImageMgr:GetImagePath(ImageID)
	ImageID = tonumber(ImageID)
	assert(Image_Client(ImageID),ImageID .. "字段在Image_Client配置表不存在")
	local ImagePath = Image_Client(ImageID,"ImagePath")
	return ImagePath
end