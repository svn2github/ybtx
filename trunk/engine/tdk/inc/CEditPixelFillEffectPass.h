#pragma once

#include "CRenderTexture.h"
#include "CPostEffectPass.h"
#include "CRenderOperation.h"

namespace sqr
{
	struct PFillEffect
	{
		DWORD color;
		int ref;
		DWORD func;
	};

	class CPixelFillEffectPass :public CPostEffectPass
	{
	public:
		CPixelFillEffectPass(void);
		~CPixelFillEffectPass(void);
		void Apply();

		void SetBlueZone(int value);
		void SetGreenZone(int value);
		void SetYellowZone(int value);
		void SetRedZone(int value);
		void SetWhiteZone(int value);
		int GetBlueZone();
		int GetGreenZone();
		int GetYellowZone();
		int GetRedZone();
		int GetWhiteZone();

		void EnableAlpha()
		{
			IsAlpha=true;
		}
		void DisableAlpha()
		{
			IsAlpha=false;
		}
		bool GetIsAlpha()
		{
			return IsAlpha;
		}

	private:
		void UpdateData();
		void RenderWithDif();
		void RenderWithAlpha();

		PFillEffect m_Param[6];
		CRenderOperation	m_Op;
		CIRect m_CurRect;
		int zone_blue;
		int zone_green;
		int zone_yellow;
		int zone_red;
		int zone_white;

		bool flag_blue;
		bool flag_green;
		bool flag_yellow;
		bool flag_red;
		bool flag_white;
		bool IsAlpha;
	};

}
