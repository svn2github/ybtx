/*
 *	代表engine中的每个的场景
 */
#pragma once

class CRegion;
class IGameObject;
class CObjectCreateInfo;

struct CWeatherInfo
{
	uint32		nAmbientColor;			// 环境光的颜色
	uint32		nWeatherParam;			// 天气参数，比如下雨的几率，下雪的几率
	uint32		nWindParam;				// 风力,会影响程序实现动画
};

struct CBkMusicInfo
{
	uint8		nCount;		// 背景音乐的个数
	uint8		nFlag;		// 播放的标志，循环还是随机
	char**		szName;		// nCount 个背景音乐的名字，每个64个字节
};

struct CCameraSetting
{
	float	fFov;		// 视角
	float	fBack;		// 后退多少, 相对于主角的中心
	float	fUp;		// 抬高多少, 相对于主角的中心
};

struct CSceneFileHeader
{
	int32				nMagicID;		// "AMAP"
	int32				nVersion;		// 版本
	char				szDesc[64];		// 地图的描述
	uint16				nWidth;			// 地图的宽度，以区域为单位
	uint16				nHeight;		// 地图的高度，以区域为单位
	uint16				dwRegionSpace;
	uint16				dwGridSpace;
	char				szTileSet[64];	// 地图相关的图素的名字
	CWeatherInfo		WeatherInfo;	// 天气相关的信息	
	CBkMusicInfo		BkMusicInfo;	// 背景音乐的信息
	CCameraSetting		CameraInfo;		// 摄像机的信息
};

//////////////////////////////////////////////////////////////////////////

struct RegionChunkHdr
{
	uint32			nMagicID;		// 标识数据块的类型
	uint32			nOffset;		// 数据块的起始位置相对于文件头的偏移
	uint32			nSize;			// 数据块的大小
};

struct RegionHeader
{
	uint32				dwMagicID      // Region类型标识
	int					nVersion		// Region version
	uint32				nCount;			// 数据块的个数
	RegionChunkHdr		ChunkHdr[1];	// nCount个ChunkHdr
};

enum enumRegionChunkType
{
	Barrier		= 0,	// 障碍信息，按照格子存储
	Trap,				// 触发点信息，按照格子存储
	Object,				// 对象列表，包含了所有的对象，内部按照类别再分为小的快
	Vertex,				// 地表的信息，几何信息，光照信息，
	Grid,				// 贴图信息,建筑信息,

	ChunkTypeCount
};

struct BarrierInfo
{
	uint8	nFlag;		// 障碍的类型
	float	fHeight;	// 高度
};

struct TrapInfo
{
	uint32	nTrapID;	//	脚本文件的数字标识，由脚本文件名计算得到的
};


*	对象数据块写入的顺序
对象的个数
对于每个对象：
{
	对象相关的类的名字
		属性的个数
		对于每个属性：
	{
		属性的名字
			属性的类型
			属性的大小
			属性的值
	}
}

