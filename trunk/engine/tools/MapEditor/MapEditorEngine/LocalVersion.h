#pragma once
#include "SceneLight.h"
#include "EditStateOpertiaonMgr.h"

namespace sqr
{
#ifdef USE_OLD_RESOURCEDIR_NOTCODER
	#define LOCAL_VERSION 0x2C
#else
	#define LOCAL_VERSION 0x2D
#endif
	
	const float	WATER_LAYER_BASE = -192.0f;

	const UINT MAX_TEXTURE = 2048;
	const UINT TEXTURE_SIZE = 128;
	const UINT EXPAND_SIZE = 8;
	const UINT ROW_SIZE = MAX_TEXTURE/TEXTURE_SIZE;
	const UINT TEXTURE_CNT = ROW_SIZE*ROW_SIZE;
	const UINT PixelSize = 4;
	const UINT EXPAND_SIZE_TWO = EXPAND_SIZE*2;
	const UINT EXPAND_TEXTURE_SIZE = TEXTURE_SIZE + EXPAND_SIZE_TWO;
	const UINT EXPAND_MAX_SIZE = MAX_TEXTURE + ROW_SIZE*EXPAND_SIZE_TWO;

	enum
	{
		INVALID_VERSION,
		SPOTLIGHT_SAVE,
		WAVE_SIDE_SAVE,
		ETRANS_STATE_SAVE,
		VCEFFECTNPC_MARK_SAVE, // VC = VERTEX COLOR
		MODEL_MIRRORTYPE,
		MODEL_EXACTBLOCKTYPE,
		MODEL_ANIPLAYMARK,
		GRID_MATERIALINDEX,
		SPECULAR_LIGHT_PARA,
		VERTEX_BAKING_COLOR,
	};

	enum
	{
		DEFAULT_MODELPRO      = 0,
		MODELPRO_EYECOLLISION = 1,
		MODELPRO_POSITION	  = 2,
	};

	enum
	{
		DEFAULT_GRIDPRO				= 0,
		GRIDPRO_ENFORCEMAT			= 1,
		GRIDPRO_ENFORCELOWBLOCK		= 2,
	};

	enum
	{
		BLOCK_VERSION			= 1,
		BLOCK_TEX_INFO			= 2,
	};

	enum
	{
		AUDIO_DEFAULT = 1,
		AUDIO_PLAYTIME_INTERVAL,
	};

	struct SLB
	{
		int GridIndex;
		int iter;
		SceneLight m_SL;
	};

	struct SpotLB
	{
		int GridIndex;
		int iter;
		SceneSpotLight m_SpotLight;
	};
}
