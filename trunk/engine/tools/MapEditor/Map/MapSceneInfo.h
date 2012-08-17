#pragma once
#include "../IO/VersionMapChunk.h"

namespace bly
{
class COperator;
}

namespace Map
{

/// 场景信息.
class MapSceneInfo : public IO::DataChunk
{
	DeclareChunk(MapSceneInfo);
public:
	MapSceneInfo();
	virtual ~MapSceneInfo();

	/// 写入流
	void Write();
	/// 从流中读取
	void Read();

	/// 加载
	void LoadFromOperator(COperator* o);
public:
	struct SceneInfo
	{
		uint		ambientColor;			///< 环境光颜色
		uint		playerColor;			///< 主角自身光颜色
		uint		sunColor;				///< 太阳光颜色
		D3DXVECTOR3	sunDir;					///< 太阳光方向
		uint		playerSunColor;			///< 主角太阳光
		uint		playerAmbientColor;		///< 主角环境光颜色
		uint		playerLightColor;		///< 主角灯颜色
		float		attenuation;			///< 主角灯强度
		float		rangeOfLight;			///< 主角灯范围
		uint		fogColor;				///< 雾的颜色
		float		fogStart;				///< 雾的近点
		float		fogEnd;					///< 雾的远点
		char		skyTexture[64];			///< 天空贴图(懒得一个个写, 就弄个固定大小-_-)
	};
	enum
	{
		DAYTIME_NUM = 4
	};
private:
	SceneInfo sceneInfo[DAYTIME_NUM];
};

}// namespace Map
