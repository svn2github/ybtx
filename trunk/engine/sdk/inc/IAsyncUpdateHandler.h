#pragma once


#define  UPDATED_FLAG   L"updated"
#define  UPDATING_FLAG  L"updating"


namespace sqr
{

	class IAsyncUpdateHandler
	{
	public:
		virtual void SetDownState(int percent) = 0;     // 设置下载progress控件进度
		virtual void SetSetupState(int percent) = 0;    // 设置安装progress控件进度
		virtual void ShowMessage(const wchar_t* Msg) = 0; // 打印当前信息
		virtual void CallBackFinishPatch(const wchar_t* szCmd) = 0; // 下载完成回调函数
		virtual void AlertExit(const wchar_t* Msg)=0;     // 警告并退出
		virtual void PatchContinue()=0;     // 不重启直接继续上Patch
		virtual void UpdateVersion()=0;                 // 更新当前版本号
		virtual void VerifyFilesFinish(int bVerifySuccess)=0;                 // 校验客户端文件已完成

	};

}

