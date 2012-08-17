#pragma once


namespace sqr
{
	class CGUIRenderer;

	class CEngineImpl
	{
		CGUIRenderer*		m_guiRenderer;
	public:
		bool				m_bWndActive;
		bool				m_bMultipleView;
		bool				m_bInitialized;
	
		CEngineImpl();
		~CEngineImpl();

		bool Init(bool bMyGUI,bool bMultipleView);
	};
}