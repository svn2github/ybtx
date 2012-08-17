#pragma once
#pragma warning(disable:4251)
#pragma warning(disable:4275)
#pragma warning(disable:4996)

#define		MIN_LEVEL 0.35f

#if defined(_USRDLL)

#ifdef EFFECT_EXPORTS
#define EFFECT_API __declspec(dllexport)
#else
#define EFFECT_API __declspec(dllimport)
#endif

#else
#define EFFECT_API
#endif

#include "CRenderable.h"
#include "BaseHelper.h"
#include "CEffectMallocObject.h"

namespace sqr
{
enum EBaseEffectUnit
{
	eParticleSystem			= 0,
	eSprite,
	ePolyTrail,
	eCameraShake,
	eFlareSprite,
	eCameraFilter,
	eModelConsole,
	eDynamicLight,
	eNull,
	eTDimAudio,
	eModelRenderStyle,
	eLightning,
	eLeading,
	eShake,
	eEfxCombination,

	eBaseUnitCount
};

enum
{
	EFADV_TEXTURE	= 1<<0,
	EFADV_COLOR		= 1<<1,
	EFADV_SCALE		= 1<<2,
	EFADV_OFFSET	= 1<<3,
	EFADV_XYZCALE	= 1<<4,
	EFADV_UVRCENTER	= 1<<5,///< 朝向地表的公告板纹理转动的中心
	EFADV_EMIMINRAD	= 1<<6,///< 粒子发射器最小半径
	EFADV_EMIMAXRAD	= 1<<7,///< 粒子发射器最大半径
};

class CBaseObject;

// m_pAttachObject ： 挂接的逻辑对象，可以为空
// m_pGameData     ： 逻辑自定义数据，可以为空
struct EffectCreateInfo
{
	CBaseObject* m_pAttachObject;
	const char*  m_ClassName;
	void*		 m_pGameData;
};

enum eUpdatePosType
{
	UP_FIXED = 0,//固定
	UP_SOCKETOFFSET,//跟随socket偏移
	UP_NODEOFFSET,//跟随骨骼偏移
	UP_SOCKET,//跟随socket
	UP_NODE,//跟随骨骼
};

}
