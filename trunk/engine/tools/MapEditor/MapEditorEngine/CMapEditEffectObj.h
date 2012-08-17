#pragma once
#include "scene.h"
#include "CMapEditObject.h"

namespace sqr_tools
{
	class CEditModel;
}

namespace sqr
{
	class CPieceGroup;
	class IEffect;
	class IEffectGroup;
	class CWireBox;

	class CMapEditEffectObj : public CMapEditObject
	{
		bool m_bIsPointLight;
		bool m_bIsSpotLight;

	public:

		CMapEditEffectObj();

		virtual ~CMapEditEffectObj();
		virtual void Render();

		virtual void SetIsPointLight( const bool b )
		{
			m_bIsPointLight = b;
		}

		virtual void SetIsSpotLight( const bool b )
		{
			m_bIsSpotLight = b;
		}
	};
}
