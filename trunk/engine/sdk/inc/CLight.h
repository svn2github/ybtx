#pragma once
#include "GraphicBase.h"
#include "CCtrlNode.h"

namespace sqr
{
	//这是一个临时的类型
	//以后会逐步完善衍生成各种光源
	//非常期待！

	struct LightStruct //这个结构不能随意更改，需要与ＡＰＩ同步
	{
		LIGHTTYPE		Type;
		CColor4			Diffuse;			/* Diffuse color of light */
		CColor4			Specular;			/* Specular color of light */
		CColor4			Ambient;			/* Ambient color of light */
		CVector3f       Position;			/* Position in world space */
		CVector3f       Direction;			/* Direction in world space */
		float           Range;				/* Cutoff range */
		float           Falloff;			/* Falloff */
		
		float           Attenuation0;		/* Constant attenuation */
		float           Attenuation1;		/* Linear attenuation */
		float           Attenuation2;		/* Quadratic attenuation */
		
		float           Theta;				/* Inner angle of spotlight cone */
		float           Phi;				/* Outer angle of spotlight cone */
	};

	class CLight : public CCtrlNode
	{
	public:
		CLight();
		~CLight();

		void			SetType(LIGHTTYPE type);
		void			SetColor(const CColor4& color);
		void			SetRange(float range);
		void			SetAttenuation(float A0, float A1, float A2);
		
		LightStruct*	GetLightStruct();
		LIGHTTYPE		GetType(void)		const;
		const CColor4&	GetColor(void)		const;
		float			GetRange(void)		const;
		const CLight&	operator = (const CLight& light);
	protected:
				void			_UpdateEx(void)		const;
		mutable	LightStruct		m_ListStruct;
	};

	inline void		CLight::SetType(LIGHTTYPE type)
	{
		m_ListStruct.Type = type;
	}

	inline void		CLight::SetColor(const CColor4& color)
	{
		m_ListStruct.Diffuse = m_ListStruct.Specular = color;
	}

	inline void		CLight::SetRange(float range)
	{
		m_ListStruct.Range = range;
	}

	inline LIGHTTYPE		CLight::GetType(void) const
	{
		return m_ListStruct.Type;
	}

	inline const CColor4&	CLight::GetColor(void) const
	{
		return m_ListStruct.Diffuse;
	}

	inline float	CLight::GetRange(void) const
	{
		return m_ListStruct.Range;
	}

	inline void	CLight::SetAttenuation(float A0, float A1, float A2)
	{
		m_ListStruct.Attenuation0	= A0;
		m_ListStruct.Attenuation1	= A1;
		m_ListStruct.Attenuation2	= A2;
	}

	inline LightStruct*	CLight::GetLightStruct()
	{
		Update();
		return &m_ListStruct;
	}

}