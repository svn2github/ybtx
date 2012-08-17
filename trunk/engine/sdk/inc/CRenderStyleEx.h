#pragma once
#include "CResourceContainer.h"
#include "CGpuProgram.h"

namespace sqr
{
	class CFixStage;

	struct ColorParams
	{
		GMap<float,CColor4>		m_Ambient;
		GMap<float,CColor4>		m_Diffuse;
		GMap<float,CColor4>		m_Specular;
		GMap<float,CColor4>		m_Emissive;
		GMap<float,float>		m_Specularpower;
		GMap<float,uint32>		m_Alpharef;
		GMap<float,CColor>		m_Texturefactor;
	};

	struct BlendParams
	{
		uint32		m_AlphaBlendEnable;
		uint32		m_SrcBlend;
		uint32		m_DestBlend;

		uint32		m_AlphaTestEnable;
		uint32		m_AlphaTestFun;

		uint32		m_LightEnable;
		uint32		m_SpecularEnable;

		uint32		m_ZTestEnable;
		uint32		m_ZTestFun;
		uint32		m_ZWrite;

		uint32		m_Cull;
		uint32		m_FillMode;
	};

	typedef GVector<ITexture*>		TexturesPool;
	typedef TexturesPool::iterator	TexturesPoolIter;

	class CRenderStyleEx : public CResUnit
	{
		friend class CRenderStyleExInst;
		friend class CRenderStyleCoder;

	public:
		CRenderStyleEx();
		~CRenderStyleEx();

	public:
		float	GetTimeRange() const;
		bool	GetIsSync()	const;

	protected:
		TexturesPool	m_DefaultTexture;
		ColorParams		m_ColorParams;
		BlendParams		m_BlendParams;
		float			m_fTimeRange;
		bool			m_bSync;

	protected:
		GVector<CFixStage*>	m_Stages;
	};
}