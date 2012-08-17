#pragma once
#include "CRenderBatch.h"
#include "CRenderGroup.h"
#include "CElementManager.h"

namespace sqr
{
	#define MIN_SHARE_BUF 7

	class ITexture;
	struct TerrainFlag : public CSceneMallocObject
	{
		enum
		{
			Normal			= 0,
			Cliff			= 1<<0,
			LightMap		= 1<<1,
			NormalSpecular	= 1<<2,
			Baking			= 1<<3,
		};
		typedef uint Mask;
	};


	class CTerrainMgr
		: public CRenderBatch
		, public CElementManager
	{
	public:
		static void			RegiserVertexDecl(); //因为地表格式趋于复杂组合,所以将地表的顶点描述单独提炼出来

	protected:
		static const int	VTX_DECL_NUM = 8;
	public:
		static int			GetVtxDeclIndex(TerrainFlag::Mask mask);
	public:
		static bool			st_isEditor;
		static const float	REGION_SIZE;			
		static const uint	MAP_SIZE;			
		static const uint	MAP_SIZE_SQR;		
		static const uint	MAX_TEXTURE;		
		static const uint	MAX_ROW_INDEX;		
		static const uint	TEXTURE_SIZE;		
		static const uint	MIN_TEXTURE_SIZE;	
		static const float	MAGIC_A;			
		static const uint	EXPAND_SIZE;		
		static const uint	LIGHT_MAP;	
		static const uint	SPECULAR;	
	public:

		CTerrainMgr(CRenderGroup* pGroup);
		virtual ~CTerrainMgr();

		CVector2f&		getUV(uint index);
		void			DestroyBuffer();
		uint			maxTextureIndex(void);

		virtual	bool	SetRenderFlag(TerrainFlag::Mask mask);
		TerrainFlag::Mask GetRenderFlag(void);
		bool			HasRenderFlag(TerrainFlag::Mask mask);

		void			TurnOnLightMap(bool isTurnOn);

	public:
		virtual bool    LoadTexture(const char* filename);
		virtual	bool    LoadTexture(ITexture* pText);

		void Render(void);
		void RenderEx(void);
		void Update(void);
		void SetLightIndex(uint index);
		void ClearRSShaderType();
		RenderParam				m_Param;
		ITexture*				m_Texture;
		uint					m_width;
		uint					m_height;

	protected:
		virtual	CElementNode*	_CreateNode( URegionID ID );
	protected:
		bool					m_isInit;

		TerrainFlag::Mask		m_Mask;
		//CCamera				*m_pCamera;
		CRenderGroup*			m_pGroup;
		CShadowFilter*			m_ShadowFilter;

		uint					m_LightIndex;
		uint					m_HotIndex;
		GString					m_szTerrTexName;


	public:
		typedef SVector<CVertexHardwareBuffer*> LightMapPool;
		static LightMapPool				st_LightMapBuffer;

		static GVector<CVertexDeclaration*>		st_TerrainDecl;//[VTX_DECL_NUM];
		static CIndexHardwareBuffer*			st_pIndex;
		static GVector<CVector2f>				st_apTextUV;//[256];
		static DWORD							st_Terrain_VS_Shader_ID;
		static DWORD							st_Terrain_PS_Shader_ID;
		static ITexture*						st_TerrainTex;
	};

	//------------------------------------------------------------------------------
	inline int
		CTerrainMgr::GetVtxDeclIndex( TerrainFlag::Mask mask )
	{
		return (mask & (~TerrainFlag::Baking));
	}
	//------------------------------------------------------------------------------
	inline bool
		CTerrainMgr::HasRenderFlag( TerrainFlag::Mask mask )
	{
		return (m_Mask & mask) != 0;
	}

	//------------------------------------------------------------------------------
	inline TerrainFlag::Mask
		CTerrainMgr::GetRenderFlag( void )
	{
		return m_Mask;
	}

	inline 	void 	
		CTerrainMgr::SetLightIndex(uint index)
	{
		m_LightIndex = index;
	};
}