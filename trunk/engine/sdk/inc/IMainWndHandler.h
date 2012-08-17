#pragma once


namespace sqr
{
class IMainWndHandler
{
public:
	virtual uint32 OnInputMessage(uint32 uMsg,uint32 wParam,uint32 lParam)
	{
		return 0;
	}
	virtual uint32 OnUserMessage(uint32 uMsg,uint32 wParam,uint32 lParam)
	{
		return 0;
	}
	virtual void OnMinimize() {};
	virtual void OnRestore() {};
	virtual void OnActive(bool bActive) {};
	virtual void OnClose() {};
};
}



