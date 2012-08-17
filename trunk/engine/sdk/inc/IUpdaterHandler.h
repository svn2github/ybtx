#pragma once


namespace sqr
{
class IUpdateObjectBTHandler;

class IBTDownloaderHandler
{
public:
	// 更新下载进度
	virtual void UpdateDownState()=0;
	// 更新安装进度
	virtual void UpdateSetupState()=0;
};
};
