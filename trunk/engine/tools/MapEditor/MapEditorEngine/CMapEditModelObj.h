#pragma once
#include "scene.h"
#include "CMapEditObject.h"

namespace sqr
{
	class CMapEditModelObj : public CMapEditObject
	{
	public:

		CMapEditModelObj();

		virtual ~CMapEditModelObj();
		virtual	void OnPreRender( uint32 uCurTime,CCamera* pCamera );

		virtual void UseNextRenderStyle();
	};
}
