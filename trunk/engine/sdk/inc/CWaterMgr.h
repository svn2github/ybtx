#pragma once
/**
	@class CWater

	水.
	由水面, 边缘浪花, 涟漪组成.
	水面质量为低当会用纹理动画代替, 高时用实时反射代替, 默认为美术设置
	@todo 浪花和涟漪
	@todo 实时反射效果
 */
#include "TRefPointer.h"
#include "CRenderBatch.h"
#include "CWaterEffect.h"
#include "CWaterRegion.h"
//#include "CWaterRegionMesh.h"
#include "CWaterSurface.h"
#include "CElementManager.h"

namespace sqr
{
	SmartPointer(CWaterSurface);
	SmartPointer(CWaterEffect);
	class CRenderGroup;

	class CWaterSurTex
	{
		SString		m_TextName;
		ITexture*	m_Texture;

	public:
		CWaterSurTex()
		{
			m_Texture = NULL;
		}
		~CWaterSurTex();
		void		SetTextName( const char* FileName )
		{
			m_TextName = FileName;
		}
		ITexture*	GetTexture(void);
	};

	class CWaterMgr 
		: public CRenderBatch
		, public CElementManager
	{
	public:
		enum WaterEffectType
		{
			TextureAnimation,
			EnvironmentMapping,

			WaterEffectNum
		};
	public:
		CWaterMgr(CRenderGroup* pGroup);
		virtual ~CWaterMgr();
	public:
		// 创建水面
		CWaterSurface* 
			CreateSurface(WaterEffectType type, SVector<ITexture*>& textures);

		// 清空水面
		void ClearSurfaces();
		
		// 绘制
		void Render();

		void SetLightIndex(uint index)
		{
			m_LightIndex = index;
		}

		// 当前水面质量
		EWaterType GetWaterType() const;

		// 设置当前水面质量, 如果对应效果不存在, 返回false
		bool SetWaterType(EWaterType type);

		// 设置最低水面质量使用的纹理序列名称s
		void SetLowestEffectTexNameVec( const SVector<SString>& texNameVec );

	public:
		// 水面效果ID转换
		static SHADER_HANDLE EffectTypeToID(WaterEffectType type);

	protected:
		CElementNode*	_CreateNode( URegionID ID ) { return new CWaterRegion(this,ID); };

	private:
		void	SetLowestEffect();

	private:
		typedef pair<SHADER_HANDLE, SVector<ITexture*> > WaterSurfaceKey;
		typedef SMap<WaterSurfaceKey, CWaterSurfacePtr> WaterSurfaceMap;
		typedef SMap<WaterSurfaceKey, CWaterEffectPtr> WaterEffectMap;
		typedef SVector<SString> LowWaterTexNameVec;

		EWaterType					m_WaterType;	// 水面质量
		CWaterEffectPtr				m_LowestEffect;	// 最低质量
		CWaterEffectPtr				highestEffect;	// 最高质量

		SVector<CWaterSurfacePtr>	m_Surfaces;
		WaterSurfaceMap				surfaceMap;
		WaterEffectMap				effectMap;
		uint						m_LightIndex;
		CRenderGroup*				m_pGroup;
		LowWaterTexNameVec			m_LowTexNameVec;
	
	public: //临时结构
		void	RenderWave(uint32 uCurTime);
		CWater*	GetWater(uint32 OrgId , int32 nGrid);
		const CWaterWaveInfo&		GetWaterWaveInfo(void);
		SVector<CWaterSurTex>		m_TextUnit;		
		CWaterWaveInfo				m_WaveInfo;				// 波浪相关信息
	};

	// ---------------------------------------------------------------------------
	inline			EWaterType		CWaterMgr::GetWaterType() const		{ return m_WaterType; }
	inline const	CWaterWaveInfo&	CWaterMgr::GetWaterWaveInfo(void)	{ return m_WaveInfo;  }

}
