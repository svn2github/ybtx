using namespace System;

namespace sqr_tools {

	class CCallBackCmd;
	public ref class ClrCmd
	{
	public:
		ClrCmd(void);
		virtual ~ClrCmd();
		void	Execute(void);

		virtual void _Execute(void);
		virtual void _UnExecute(void);
	protected:
		CCallBackCmd*	m_pCallBackCmd;
	};

	public ref class ClrState : public ClrCmd
	{
	public:
		virtual void ActiveState(void);

	//protected:
		virtual void _Execute(void)		override;
		virtual void _UnExecute(void)	override;
	};
}