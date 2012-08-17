
#include "IAsyncUpdateHandler.h"

#include <string>


class CStarterUpdateHandler
	: public sqr::IAsyncUpdateHandler
{
public:
	CStarterUpdateHandler();
	~CStarterUpdateHandler();

	// 实现抽象类虚函数
	virtual void SetDownState(int percent);
	virtual void SetSetupState(int percent);
	virtual void ShowMessage(const TCHAR* Msg);
	virtual void CallBackFinishPatch(const TCHAR* szCmd);
	virtual void AlertExit(const TCHAR* Msg);
	virtual void UpdateVersion();
	virtual void PatchContinue();
	virtual void VerifyFilesFinish(int bVerifySuccess);

};

