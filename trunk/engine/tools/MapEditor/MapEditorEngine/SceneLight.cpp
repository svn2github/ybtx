#include "stdafx.h"
#include "SceneLight.h"

namespace sqr
{
	unsigned __int32 RefreshDynamicSceneLight( SceneLight &DSL, DWORD nRefreshTime)
	{
		if( DSL.m_SHOW_FirstTime )
		{   
			DSL.m_SHOW_FirstTime        = false;
			//DSL.m_SHOW_LastTimeBase     = nRefreshTime;
			DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
			DSL.m_SHOW_CurState         = 0;
			return DSL.m_Color;
		}

	DOSWITCH:
		switch( DSL.m_SHOW_CurState)
		{
		case 0: 
			{               
				if ( (float)(nRefreshTime - DSL.m_SHOW_LastRefreshTime) / 1000.0f > (float)DSL.m_Dyn_BrightTime/128.0f )
				{
					if( rand() < (int)((float)DSL.m_Dyn_ChangeProbability / 255.0f * (float)RAND_MAX))
					{
						DSL.m_SHOW_CurState = 1;
					}
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					goto DOSWITCH;
				}
				return DSL.m_Color;                
				break;
			}
		case 1: 
			{

				float P = (float)(nRefreshTime - DSL.m_SHOW_LastRefreshTime) / 1000.0f;
				float T = (float)DSL.m_Dyn_ChangeTime /128.0f;

				if ( DSL.m_Dyn_ChangeTime < 5){
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					DSL.m_SHOW_CurState = 2;
					goto DOSWITCH;
				}

				if ( P > T )
				{
					DSL.m_SHOW_CurState = 2;
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					//return DSL.m_Dyn_Color;
					goto DOSWITCH;
				}

				int B = GetBValue(DSL.m_Color);
				int G = GetGValue(DSL.m_Color);
				int R = GetRValue(DSL.m_Color);
				int dB = GetBValue(DSL.m_Dyn_Color);
				int dG = GetGValue(DSL.m_Dyn_Color);
				int dR = GetRValue(DSL.m_Dyn_Color);
				float BL = P / T;
				int nB = (int)(B + ( dB - B ) * BL);
				int nG = (int)(G + ( dG - G ) * BL);
				int nR = (int)(R + ( dR - R ) * BL);

				return RGB(nR,nG,nB);
				break;
			}

		case 2: 
			{

				if ( DSL.m_Dyn_DarkTime < 5){
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					DSL.m_SHOW_CurState = 3;
					goto DOSWITCH;
				}

				if ( (float)(nRefreshTime - DSL.m_SHOW_LastRefreshTime) / 1000.0f > (float)DSL.m_Dyn_DarkTime /128.0f )
				{
					DSL.m_SHOW_CurState = 3;
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					goto DOSWITCH;
				}
				return DSL.m_Dyn_Color;                
				break;
			}

		case 3: 
			{

				float P = (float)(nRefreshTime - DSL.m_SHOW_LastRefreshTime) / 1000.0f;
				float T = (float)DSL.m_Dyn_ChangeTime /128.0f;


				if ( DSL.m_Dyn_ChangeTime < 5){
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					DSL.m_SHOW_CurState = 0;
					goto DOSWITCH;
				}

				if ( P > T )
				{
					DSL.m_SHOW_CurState = 0;
					DSL.m_SHOW_LastRefreshTime  = nRefreshTime;
					goto DOSWITCH;
					//return DSL.m_Color;
				}

				int B = GetBValue(DSL.m_Color);
				int G = GetGValue(DSL.m_Color);
				int R = GetRValue(DSL.m_Color);
				int dB = GetBValue(DSL.m_Dyn_Color);
				int dG = GetGValue(DSL.m_Dyn_Color);
				int dR = GetRValue(DSL.m_Dyn_Color);
				float BL = P / T;
				int nB = (int)(dB + ( B - dB ) * BL); 
				int nG = (int)(dG + ( G - dG ) * BL); 
				int nR = (int)(dR + ( R - dR ) * BL); 

				return RGB(nR,nG,nB);

			   break;
			}
		default:
			return 0;
			break;

		}
	}

	// ----------------------------------------------------------------------------
	SceneLight_OLD::SceneLight_OLD()
	{
		m_Attenuation0	= 1.0f;
		m_Attenuation1	= 0.0f;
		m_Attenuation2	= 0.0f;
		m_Color			= 0xff646464;
		m_Rang			= 400.0f;
		ZeroMemory(m_Info, SCENE_LIGHT_INFO_SIZE);
		m_bIAmbScenePointLight	= false;
		m_ShowMe				= true;

		m_bDynamic              = false;
		m_Dyn_Color             = 0x0;
		m_Dyn_BrightTime        = 0;
		m_Dyn_DarkTime          = 0;
		m_Dyn_ChangeTime        = 0;
		m_Dyn_ChangeProbability = 0;

		m_SHOW_CurState         = 0;
		m_SHOW_LastRefreshTime  = 0;
		m_SHOW_FirstTime        = true;
	}

	SceneLight::SceneLight()
	{
		m_Attenuation0	= 1.0f;
		m_Attenuation1	= 0.0f;
		m_Attenuation2	= 0.0f;
		m_Color			= 0xff646464;
		m_Rang			= 400.0f;
		ZeroMemory(m_Info, SCENE_LIGHT_INFO_SIZE);
		m_bIAmbScenePointLight	= false;
		m_ShowMe				= true;

		m_bDynamic              = false;
		m_Dyn_Color             = 0x0;
		m_Dyn_BrightTime        = 0;
		m_Dyn_DarkTime          = 0;
		m_Dyn_ChangeTime        = 0;
		m_Dyn_ChangeProbability = 0;

		m_SHOW_CurState         = 0;
		m_SHOW_LastRefreshTime  = 0;
		m_SHOW_FirstTime        = true;

#ifdef USE_OLD_LIGHT_MODEL
		m_bEffectTerrain		= false;
#else
		m_bEffectTerrain		= true;
#endif
		
	}

	SceneLight& SceneLight::operator=( const ScenePointLight_Gac& gacLight )
	{
		this->m_Attenuation0 = gacLight.m_fAttenuation0;
		this->m_Attenuation1 = gacLight.m_fAttenuation1;
		this->m_Attenuation2 = gacLight.m_fAttenuation2;
		this->m_Rang		 = gacLight.m_fRang;

		DWORD dwColor = CColor(0, gacLight.m_bColor, gacLight.m_gColor, gacLight.m_rColor);
		this->m_Color		 = dwColor;

		dwColor = CColor(0, gacLight.m_rDyn_Color, gacLight.m_gDyn_Color, gacLight.m_bDyn_Color);
		this->m_Dyn_Color	 = dwColor;

		this->m_Dyn_ChangeProbability = gacLight.m_Dyn_ChangeProbability;
		this->m_Dyn_BrightTime = gacLight.m_Dyn_BrightTime;
		this->m_Dyn_DarkTime   = gacLight.m_Dyn_DarkTime;
		this->m_Dyn_ChangeTime = gacLight.m_Dyn_ChangeTime;

		return *this;
	}

	SceneSpotLight::SceneSpotLight()
	{
		m_fAttenuation0	= 1.0f;
		m_fAttenuation1	= 0.0f;
		m_fAttenuation2	= 0.0f;
		m_Color			= 0xffffffff;
		m_fRang			= 800.0f;
		m_fFallOff		= 1.0f;
		m_fTheta		= 1.046667f; ///60¶È
		m_fPhi			= 2.093333f; ///120¶È
		ZeroMemory(m_Info, SCENE_LIGHT_INFO_SIZE);
		m_bIAmbSceneSpotLight = false;
		m_Direction		= CVector3f(-1.0f, 0.0f, 0.0f);
		m_TargetPos		= CVector3f(0.0f,  0.0f, 0.0f);

#ifdef USE_OLD_LIGHT_MODEL
		m_bEffectTerrain		= false;
#else
		m_bEffectTerrain		= true;
#endif

	}

	SceneSpotLight& SceneSpotLight::operator=( const SceneSpotLight_Gac& gacLight )
	{
		this->m_fAttenuation0 = gacLight.m_fAttenuation0;
		this->m_fAttenuation1 = gacLight.m_fAttenuation1;
		this->m_fAttenuation2 = gacLight.m_fAttenuation2;
		this->m_fRang		  = gacLight.m_fRang;
		this->m_fFallOff	  = gacLight.m_fFallOff;
		this->m_fTheta		  = gacLight.m_fTheta;
		this->m_fPhi	      = gacLight.m_fPhi;

		DWORD dwColor = CColor(0, gacLight.m_bColor, gacLight.m_gColor, gacLight.m_rColor);
		this->m_Color		 = dwColor;

 		this->m_Direction.x	  = gacLight.m_DirectionX;//
		this->m_Direction.y	  = gacLight.m_DirectionY;//
		this->m_Direction.y	  = gacLight.m_DirectionZ;//

 		this->m_TargetPos.x	  = gacLight.m_TargetPosX;
		this->m_TargetPos.y	  = gacLight.m_TargetPosY;
		this->m_TargetPos.z	  = gacLight.m_TargetPosZ;

		return *this;
	}
	//------------------------------------------------------------------------------
	/**
	SceneLight to CLight
	*/
	const CLight& SceneLightToCLight(CLight& light, const SceneLight& point)
	{
		LightStruct& tLight = *light.GetLightStruct();
		ZeroMemory(&tLight , sizeof(LightStruct));

		tLight.Type = LIGHT_POINT;

		tLight.Attenuation0 = point.m_Attenuation0;
		tLight.Attenuation1 = point.m_Attenuation1;
		tLight.Attenuation2 = point.m_Attenuation2;

		tLight.Diffuse.r = GetRValue(point.m_Color) / 255.0f;
		tLight.Diffuse.g = GetGValue(point.m_Color) / 255.0f;
		tLight.Diffuse.b = GetBValue(point.m_Color) / 255.0f;
		tLight.Diffuse.a = 1.0f;

		tLight.Range = point.m_Rang;

		return light;
	}

	const CLight& SceneLightToCLight(CLight& light, const SceneSpotLight& spot)
	{
		LightStruct& tLight = *light.GetLightStruct();
		ZeroMemory(&tLight , sizeof(LightStruct));
		
		tLight.Type = LIGHT_SPOT;

		tLight.Attenuation0 = spot.m_fAttenuation0;
		tLight.Attenuation1 = spot.m_fAttenuation1;
		tLight.Attenuation2 = spot.m_fAttenuation2;
		tLight.Range = spot.m_fRang;
		tLight.Falloff = spot.m_fFallOff;
		tLight.Theta = spot.m_fTheta;
		tLight.Phi = spot.m_fPhi;

		tLight.Diffuse.r = GetRValue(spot.m_Color) / 255.0f;
		tLight.Diffuse.g = GetGValue(spot.m_Color) / 255.0f;
		tLight.Diffuse.b = GetBValue(spot.m_Color) / 255.0f;
		tLight.Diffuse.a = 1.0f;
		tLight.Direction = spot.m_Direction;
		return light;
	}
}