#pragma once
#include "Scene.h"


namespace sqr
{
	class CRecordCameraNode
	{
		friend class CRecordCamera;
		CRecordCamera	 *m_pCamera;
	private:
		CRecordCameraNode( const CRecordCameraNode & );
		CRecordCameraNode( CRecordCamera *pCam);
		CRecordCameraNode& operator = ( const CRecordCameraNode & );
		uint16  m_iX;
		uint16  m_iY;

	public:
		void OnEvent(const CMsg& msg);
		void Render();
	};
}