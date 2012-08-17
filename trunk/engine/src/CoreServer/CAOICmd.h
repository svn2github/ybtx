#pragma once 

namespace sqr
{
	class CSyncCoreObjectServer;
	class CAoiCmdBase
	{
	public:
		virtual ~CAoiCmdBase(){}
		virtual void HandleCmd(CSyncCoreObjectServer* pObj)=0;
	};

	class CAoiDisbindConnCmd : public CAoiCmdBase
	{
	public:
		void HandleCmd(CSyncCoreObjectServer* pObj);
	};

	class CAoiPostShellMsgCmd : public CAoiCmdBase
	{
	public:
		CAoiPostShellMsgCmd(const void* pContext);
		void HandleCmd(CSyncCoreObjectServer* pObj);
	private:
		const void* m_pContext;
	};


	class CAoiDecBlockCountCmd : public CAoiCmdBase
	{
	public:
		void HandleCmd(CSyncCoreObjectServer* pObj);
	};

	class CAoiBindToCmd : public CAoiCmdBase
	{
	public:
		CAoiBindToCmd(uint32 uParentID, bool bRet);
		void HandleCmd(CSyncCoreObjectServer* pObj);
	private:
		uint32 m_uParentID;
		bool m_bRet;
	};
}
