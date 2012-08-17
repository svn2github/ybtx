#pragma once
//------------------------------------------------------------------------------
/**
	@class CStaticIlluminator

	静态光照模型.

	利用CPU进行顶点光照处理
	@note 所有光照计算一定要用CColor4, byte精度不够...

	(C) 2009 ThreeOGCMan
*/
#include "TSingleton.h"
#include "CLight.h"

//------------------------------------------------------------------------------
namespace sqr
{
	class CStaticIlluminator : public TSingleton<CStaticIlluminator>
	{
	public:
		CStaticIlluminator();

		/// 设置环境光
		void SetAmbient(const CColor4& color);
		/// 设置全局光(方向光)
		void SetGlobalLight(const CLight& global);
		/// 添加一个点光源
		void AddPointLight(const CLight& point);
		/// 添加一个聚光灯
		void AddSpotLight(const CLight& spot);

		/// 消除所有光源
		void ClearAllLights();

		/// 进行光照计算
		CColor4 Illuminate(const sMaterial& materail, const CVector3f& position, const CVector3f& normal);

	private:
		CColor4 DirectLighting(const sMaterial& material, const CVector3f& position, const CVector3f& normal);
		CColor4 PointLighting(const sMaterial& material, const CVector3f& position, const CVector3f& normal);
		CColor4 SpotLighting(const sMaterial& material, const CVector3f& position, const CVector3f& normal);

	private:
		CColor4 ambient;
		CLight globalLight;
		vector<CLight> pointLights;
		vector<CLight> spotLights;
	};

	//------------------------------------------------------------------------------
	inline void
	CStaticIlluminator::SetGlobalLight( const CLight& global )
	{
		Ast(LIGHT_DIRECTIONAL == global.GetType());
		this->globalLight = global;
	}

	//------------------------------------------------------------------------------
	inline void
	CStaticIlluminator::AddPointLight( const CLight& point )
	{
		Ast(LIGHT_POINT == point.GetType());
		this->pointLights.push_back(point);
	}

	//------------------------------------------------------------------------------
	inline void
	CStaticIlluminator::AddSpotLight( const CLight& spot )
	{
		Ast(LIGHT_SPOT == spot.GetType());
		this->spotLights.push_back(spot);
	}

	//------------------------------------------------------------------------------
	inline void
	CStaticIlluminator::SetAmbient( const CColor4& color )
	{
		this->ambient = color;
	}
}