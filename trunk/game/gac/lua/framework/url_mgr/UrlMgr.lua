cfg_load "Url/Url_Common"

CUrlMgr = class()

function CUrlMgr:GetUrl(nUrlIndex)
	assert(Url_Common(nUrlIndex), nUrlIndex .. "字段在Url_Common配置表不存在")
	return Url_Common(nUrlIndex, "UrlValue")
end