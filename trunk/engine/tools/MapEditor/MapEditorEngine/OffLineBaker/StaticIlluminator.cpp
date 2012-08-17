//------------------------------------------------------------------------------
//  StaticIlluminator.cpp
//  (C) 2009 ThreeOGCMan
//------------------------------------------------------------------------------
#include "StdAfx.h"
#include "StaticIlluminator.h"

namespace sqr
{
	//------------------------------------------------------------------------------
	CStaticIlluminator::CStaticIlluminator()
	{
		this->ClearAllLights();
	}

	//------------------------------------------------------------------------------
	void
	CStaticIlluminator::ClearAllLights()
	{
		this->pointLights.clear();
		this->spotLights.clear();
	}

	//------------------------------------------------------------------------------
	CColor4
	CStaticIlluminator::Illuminate( const sMaterial& material, const CVector3f& position, const CVector3f& normal )
	{
		return this->DirectLighting(material, position, normal) +
			   this->PointLighting(material, position, normal) +
			   this->SpotLighting(material, position, normal);
	}

	//------------------------------------------------------------------------------
	CColor4
	CStaticIlluminator::DirectLighting( const sMaterial& material, const CVector3f& position, const CVector3f& normal )
	{
		float NdotL = normal.Dot(-this->globalLight.getDIRECTION());
		CColor4 diffuse = this->ambient * material.Ambient +
						  this->globalLight.GetColor() * max(NdotL, 0.0f) * material.Diffuse;

		return diffuse;
	}

	//------------------------------------------------------------------------------
	CColor4
	CStaticIlluminator::PointLighting( const sMaterial& material, const CVector3f& position, const CVector3f& normal )
	{
		CColor4 color = CColor4::Black;
		for (vector<CLight>::iterator it = this->pointLights.begin();
			it != this->pointLights.end(); ++it)
		{
			CLight& light = *it;
			CVector3f direction = light.GetPosition() - position;
			float distance = direction.Mag();
			if (light.GetRange() < distance)
				continue;
			direction.Normalize();
			float NdotL = direction.Dot(normal);
			NdotL = max(NdotL, 0.0f);

			LightStruct &tLight = *(light.GetLightStruct());
		
			float attenuation = 1.0f / (tLight.Attenuation0 +
				tLight.Attenuation1 * distance +
				tLight.Attenuation2 * distance * distance);
			
			color += tLight.Diffuse * material.Diffuse * NdotL * attenuation;
		}

		return color;
	}

	//------------------------------------------------------------------------------
	CColor4
	CStaticIlluminator::SpotLighting( const sMaterial& material, const CVector3f& position, const CVector3f& normal )
	{
		CColor4 color = CColor4::Black;
		for (vector<CLight>::iterator it = this->spotLights.begin();
			it != this->spotLights.end(); ++it)
		{
			CLight& light = *it;
			LightStruct &tLight = *(light.GetLightStruct());

			CVector3f direction = tLight.Position - position;
			float distance = direction.Mag();
			if (tLight.Range < distance)
				continue;
			direction.Normalize();
			float NdotL = direction.Dot(normal);
			NdotL = max(NdotL, 0.0f);
			float attenuation = 1.0f / (tLight.Attenuation0 +
				tLight.Attenuation1 * distance +
				tLight.Attenuation2 * distance * distance);
			// spot tLight factor
			float cosAlpha = (-direction).Dot(tLight.Direction);
			float cosHalfPhi = cos(tLight.Phi * 0.5f);
			if (cosAlpha <= cosHalfPhi)
				continue;
			float cosHalfTheta = cos(tLight.Theta * 0.5f);
			float intensity = (cosAlpha - cosHalfPhi) / (cosHalfTheta - cosHalfPhi);
			intensity = pow(intensity, tLight.Falloff);

			color += tLight.Diffuse * material.Diffuse * NdotL * attenuation * intensity;
		}

		return color;
	}
}