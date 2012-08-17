#pragma once
#include "CTexture.h"
#include "CResourceContainer.h"
#include "CRenderStyleEx.h"

namespace sqr
{
	struct STATIC_RS : public RenderState
	{
		ITexture*		m_Texture_S0;
		ITexture*		m_Texture_S1;
		ITexture*		m_Texture_S2;
		ITexture*		m_Texture_S3;

		CPlane			m_ClipPlane;	///< 裁剪面
		float			m_fRefractIndex;///< 折射率
		bool			m_bPreLight;
		float			m_fZBias;		///< 深度偏移
		//float			m_fSlopeScaleZBias;

		STATIC_RS();

		void	TexAddRef();
		void	TexRelease();
	};

	class CRenderStyle 
		: public CResUnit
		, public ColorParams
		, public BlendParams
	{
	protected:
		friend class CDataSources;
		CRenderStyle( const string& szName, const wstring& szAlias );
	public:
		~CRenderStyle(void);
	
		int32					m_Ref;

		float					m_TimeRang;
		bool					m_bSyn;

		uint32					m_Uv_S0;
		uint32					m_Uv_S1;
		uint32					m_Uv_S2;

		SHADER_HANDLE			m_ShaderType;

		GMap<float,uint32>		m_Texture_S0;
		GMap<float,uint32>		m_Texture_S1;
		GMap<float,uint32>		m_Texture_S2;
		GMap<float,uint32>		m_Texture_S3;

		uint32		m_Sample_S0;
		uint32		m_Sample_S1;
		uint32		m_Sample_S2;
		uint32		m_Sample_S3;
		uint32		m_TexOP_S0;
		uint32		m_TexOP_S1;
		uint32		m_TexOP_S2;
		uint32		m_TexOP_S3;
		uint32		m_TexArg1_S0;
		uint32		m_TexArg1_S1;
		uint32		m_TexArg1_S2;
		uint32		m_TexArg1_S3;
		uint32		m_TexArg2_S0;
		uint32		m_TexArg2_S1;
		uint32		m_TexArg2_S2;
		uint32		m_TexArg2_S3;
		uint32		m_AlpOP_S0;
		uint32		m_AlpOP_S1;
		uint32		m_AlpOP_S2;
		uint32		m_AlpOP_S3;
		uint32		m_AlpArg1_S0;
		uint32		m_AlpArg1_S1;
		uint32		m_AlpArg1_S2;
		uint32		m_AlpArg1_S3;
		uint32		m_AlpArg2_S0;
		uint32		m_AlpArg2_S1;
		uint32		m_AlpArg2_S2;
		uint32		m_AlpArg2_S3;
		uint32		m_UVSrc_S0;
		uint32		m_UVSrc_S1;
		uint32		m_UVSrc_S2;
		uint32		m_UVSrc_S3;
		uint32		m_UAddr_S0;
		uint32		m_UAddr_S1;
		uint32		m_UAddr_S2;
		uint32		m_UAddr_S3;
		uint32		m_VAddr_S0;
		uint32		m_VAddr_S1;
		uint32		m_VAddr_S2;
		uint32		m_VAddr_S3;
		uint32		m_MipMapSample_S0;
		uint32		m_MipMapSample_S1;
		uint32		m_MipMapSample_S2;
		uint32		m_MipMapSample_S3;
		uint32		m_MipMapBias_S0;
		uint32		m_MipMapBias_S1;
		uint32		m_MipMapBias_S2;
		uint32		m_MipMapBias_S3;
		void	OnRead( CRefBufFile& GraphicFile );
		bool	m_bOnRead;
	public:

		bool			IsValid(){ return CResUnit::IsValid(); }
		int				BinLoad( void );
		void			GetCurRenderStyle( STATIC_RS& RenderStyle, ITexture** pTextureList, int nListNum, uint32 FrameTime ,uint32 CurTime, void* pUser) const;

		bool			IsAlphaBlend( float Time );

		const char*		GetName() const
		{
			return m_szName.c_str();
		}
		int32			AddRef()
		{
			return ++m_Ref;
		};
		virtual void	Release()
		{
			if ( --m_Ref == 0 )FreeRes();
		};

	protected:
		
	};


	//渲染参数
	struct RenderParam
	{
		STATIC_RS		m_RS;
		CMatrix			m_Mat;
	};

};
