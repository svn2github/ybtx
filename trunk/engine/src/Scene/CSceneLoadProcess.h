#pragma once
#include "IProcessCallBack.h"
#include "SyncSystem.h"

namespace sqr
{
	class CSceneLoadProcess : public IProcessCallBack
	{
	public:
		CSceneLoadProcess( IProcessCallBack* pHandler );
		void Clear(void);
		virtual void DoProcess( float process );
	protected:
		IProcessCallBack*	m_pHandler;
		float				m_fProcess;
	};

// 	class CCameraLoadProcess : public IProcessCallBack
// 	{
// 	public:
// 		CCameraLoadProcess( IProcessCallBack* pHandler );
// 		void Clear(void);
// 		void DoProcess( float process );
// 	protected:
// 		IProcessCallBack*	m_pHandler;
// 		float				m_fProcess;
// 	};
}