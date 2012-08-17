#pragma once
#include "TSingleton.h"

namespace sqr
{
	class IProgressCallback
	{
	public:
		virtual void SetProgress(float f) = 0;
		virtual float GetProgress() const = 0;
		virtual void SetDescription(const TCHAR* desc) = 0;
	};

	class CMapEditCamera;
	class CRecordCamera;
	class CEngineImpl;
	class CCameraController;

	class CEngine 
		: public Singleton<CEngine>
	{
		CEngineImpl *m_pImpl;

	private:
		friend class Singleton<CEngine>;
		CEngine();
		virtual ~CEngine();

	public:
		bool Init(bool bMyGUI,bool bMultipleView);
		void Release();
		void Update();
		HWND GetD3DWindow();
		void RenderTopViewMap(uint32 Scale,TCHAR const * const strTitle ,IProgressCallback* pProgress = NULL);
		void RenderGUIMap(IProgressCallback* pProgress = NULL);
		void RefreshShader(void);
	};
}
