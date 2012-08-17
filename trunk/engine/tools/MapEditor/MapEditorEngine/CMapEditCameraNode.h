#pragma once
#include "Scene.h"


namespace sqr
{
	class CMapEditCamera;

	class CMapEditCameraNode
	{
		friend class CMapEditCamera;

		CMapEditCamera	*m_pCamera;

	private:
		CMapEditCameraNode(CMapEditCamera* cam);
		uint16  m_iX;
		uint16  m_iY;

	public:
		void OnEvent(const CMsg& msg);
		void Render();
	};
}