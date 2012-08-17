#pragma once
#include "TSingleton.h"
#include "CCameraBase.h"
#include "CEventOwner.h"

namespace sqr
{
	class CCameraBase;
	class CMapEditCamera;
	class CRecordCamera;
	class CGameGacCamera;

	class CCameraManager : public Singleton<CCameraManager>
	{
		friend class Singleton<CCameraManager>;
		friend class CCameraBase;

		CCameraManager();
		CCameraManager( const CCameraManager& );
		CCameraManager& operator = ( const CCameraManager& );

		void				_Init();
		
		//相机对象
		CCameraBase*	m_CameraList[eCT_Count];
		CCameraBase*	m_pActiveCamera;
	
	public:
		~CCameraManager();
		CCameraBase*		GetActiveCamera() const;
		CMapEditCamera*		GetMapEditCamera();
		CRecordCamera*		GetRecordCamera();
		CGameGacCamera*		GetGameGacCamera();
		void				SetActiveCamera( CCameraBase *pCamera );

		void				Update();
		void                EventProcess(const CMsg& msg);
	};
}