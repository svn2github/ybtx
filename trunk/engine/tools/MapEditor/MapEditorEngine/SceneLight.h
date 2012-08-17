#pragma once
#include "CLight.h"

namespace sqr
{
	#define SCENE_LIGHT_INFO_SIZE	28

	struct ScenePointLight_Gac
	{
		float           m_fAttenuation0;
		float           m_fAttenuation1;
		float           m_fAttenuation2;
		uint16          m_fRang;
		uint8           m_rColor;
		uint8           m_gColor;
		uint8           m_bColor;
		uint8           m_rDyn_Color;
		uint8           m_gDyn_Color;
		uint8           m_bDyn_Color;
		uint8           m_Dyn_ChangeProbability;
		uint8           m_Dyn_BrightTime;
		uint8           m_Dyn_DarkTime;
		uint8           m_Dyn_ChangeTime;
	};

	struct SceneSpotLight_Gac
	{
		float				m_fAttenuation0;
		float				m_fAttenuation1;
		float				m_fAttenuation2;
		float				m_DirectionX;	 ///聚光灯方向
		float				m_DirectionY;
		float				m_DirectionZ;
		float				m_TargetPosX;	///聚光灯方向终点位置，也即目标位置
		float				m_TargetPosY;
		float				m_TargetPosZ;
		float				m_fTheta;		 ///指定内圆锥角度，单位是弧度
		float				m_fPhi;			 ///指定外圆锥角度，单位是弧度
		uint16				m_fRang;
		uint16				m_fFallOff;      ///从内圆锥到外圆锥之间的强度衰减
		uint8				m_rColor;
		uint8				m_gColor;
		uint8				m_bColor;
	};

	struct SceneLight_OLD
	{
		SceneLight_OLD();
		bool				m_bIAmbScenePointLight;
		bool				m_ShowMe;
		TCHAR				m_Info[SCENE_LIGHT_INFO_SIZE];
		float				m_Attenuation0;
		float				m_Attenuation1;
		float				m_Attenuation2;
		float				m_Rang;
		uint32				m_Color;

		bool                m_bDynamic;
		uint32				m_Dyn_Color;
		uint8				m_Dyn_BrightTime;
		uint8				m_Dyn_DarkTime;
		uint8				m_Dyn_ChangeTime;
		uint8				m_Dyn_ChangeProbability;

		uint8				m_SHOW_CurState;
		DWORD               m_SHOW_LastRefreshTime;
		bool                m_SHOW_FirstTime;
	};

	// 场景点光源数据结构，包括动态的属性
	struct SceneLight
	{
		SceneLight();
		bool				m_bIAmbScenePointLight;
		bool				m_ShowMe;
		TCHAR				m_Info[SCENE_LIGHT_INFO_SIZE];
		float				m_Attenuation0;
		float				m_Attenuation1;
		float				m_Attenuation2;
		float				m_Rang;
		uint32				m_Color;

		bool                m_bDynamic;
		uint32				m_Dyn_Color;
		uint8				m_Dyn_BrightTime;
		uint8				m_Dyn_DarkTime;
		uint8				m_Dyn_ChangeTime;
		uint8				m_Dyn_ChangeProbability;

		uint8				m_SHOW_CurState;
		DWORD               m_SHOW_LastRefreshTime;
		bool                m_SHOW_FirstTime;
		bool				m_bEffectTerrain;///是否影响地表

		SceneLight& operator= ( const ScenePointLight_Gac& gacLight );
	};

	// 场景聚光灯数据结构
	// 美术核对之后，不需要动态的聚光灯
	struct SceneSpotLight
	{
		SceneSpotLight();
		bool				m_bIAmbSceneSpotLight;
		TCHAR				m_Info[SCENE_LIGHT_INFO_SIZE];
		float				m_fAttenuation0;
		float				m_fAttenuation1;
		float				m_fAttenuation2;
		float				m_fRang;
		float				m_fFallOff;      ///从内圆锥到外圆锥之间的强度衰减
		float				m_fTheta;		 ///指定内圆锥角度，单位是弧度
		float				m_fPhi;			 ///指定外圆锥角度，单位是弧度
		uint32				m_Color;
		CVector3f			m_Direction;	 ///聚光灯方向
		CVector3f			m_TargetPos;	 ///聚光灯方向终点位置，也即目标位置
		bool				m_bEffectTerrain;///是否影响地表

		SceneSpotLight& operator= ( const SceneSpotLight_Gac& gacLight );
	};

	uint32 RefreshDynamicSceneLight( SceneLight &DSL, DWORD nRefreshTime);
	const CLight& SceneLightToCLight(CLight& light, const SceneLight& point);
	const CLight& SceneLightToCLight(CLight& light, const SceneSpotLight& spot);
}