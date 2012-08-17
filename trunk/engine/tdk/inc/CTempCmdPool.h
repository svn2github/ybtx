#pragma once

namespace sqr_tools
{
	class CTemporaryCmd;
	class CTempCmdPool
	{
		friend class CTemporaryCmd;
	public:
		CTempCmdPool(void);
		~CTempCmdPool(void);

	protected:
		virtual void PushCmd(CTemporaryCmd* cmd);
		virtual void Destrory(CTemporaryCmd* cmd);
		virtual void Clear(void);
	protected:
		typedef set<CTemporaryCmd*> TempCmdPool;
		TempCmdPool	m_cmdPool;
	};
}