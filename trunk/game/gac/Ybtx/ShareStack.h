
#include <list>


class ShareStack
{
public:
	static ShareStack* GetInst();

	void PushMsg(const wchar_t* msg);
	wstring PopMsg();
	size_t GetMsgSize();

	void PushCmd(const wchar_t* cmd);
	wstring PopCmd();

private:
	ShareStack();
	~ShareStack();

	list<wstring>            m_MsgQueue;
	list<wstring>            m_CmdQueue;
	CRITICAL_SECTION        m_cs;
};

