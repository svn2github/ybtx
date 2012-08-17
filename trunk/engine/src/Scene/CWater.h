#pragma once
#include "TimeHelper.h"
#include "GraphicBase.h"
#include "CSceneBase.h"
#include "CVector3.h"

#define WATER_NULL			NULL
#define WATER_CENTER		((CWaterWave*)1)


namespace sqr
{

struct CWaterWaveInfo;
class CWater;
class CGraphic;
class IEffect;

/// 水波浪.
class CWaterWave : public CSceneMallocObject
{
	struct WAVE
	{
		BOOL		bInUsed;
		VerColor3D	verPos[4];
		uint32		nStartTime;
		uint32		nLifeTime;
		float		fSpeed;
		float		xDir;
		float		zDir;
		CVector3f	vecOff;
	};

	float			m_xDir;
	float			m_zDir;
	CVector3f		m_vecOff;
	float			m_RemainTime;
	WAVE*			m_pWaves;

	const CWaterWaveInfo* m_pWaveInfo;

public:
	CWaterWave( const CWaterWaveInfo* pWaveInfo );
	~CWaterWave();
	void	Init( float xDir, float zDir, const CVector3f& vecOff );
	int		Update( uint32 nCurTime, float DeltaTime );
	void	Render( VerNCT*& pVB, uint16*& pIB, uint16& Base );
};

/// 涟漪.
struct CDimple : public CSceneMallocObject
{
	uint32			m_uStartTime;
	CWater*			m_pWater;
	IEffect*		m_pEffectObj;
	CMatrix			m_matWorld;

	CDimple( CWater* pWater, IEffect* pDimpleFx, const CMatrix& matWorld );
	~CDimple();
	void Render( uint32 uPreTime );
};

class CWater : public CSceneMallocObject
{
	uint32		m_nTexBegin;					///< 纹理索引起始点
	uint32		m_nTexCount;					///< 纹理数量
	CWaterWave*	m_pWaterWave[eGridNumPerRegion];///< 每个格子的波浪单独处理的吗?
	SVector<uint8> m_WaterGrid;					///< 有水的格子的索引

	uint8		m_MinWater;
	uint8		m_MaxWater;

	uint8		m_MinWave;
	uint8		m_MaxWave;

	uint32		m_nPreTime;						///< 当前时间
	uint32		m_OrgID;

	//int32		m_nDimplesNum;
	typedef		SVector<CDimple*>	DimplePool;
	DimplePool	m_pPoolDimples;

	/// 更新波浪
	uint32 UpdateWave( uint32 nCurTime );
public:

	CWater(uint32 orgID);
	~CWater(void);

	/// 增加涟漪
	void AddDimple( CDimple* pObj );
	/// 设置纹理索引范围
	void SetTextureRange( uint32 nTextBegin, uint32 nTextCount );
	void GetTextureRange(uint& begin, uint& count);
	/// 设置格子是否合法
	void SetGridValid( uint8 nGrid );
	/// 设置格子的水波浪信息
	void SetGridDir( const CWaterWaveInfo* pWaveInfo,
	                 uint8 nGrid, float xDir, float yDir, const CVector3f& vecGridOffset );
	/// 取得当前水的纹理索引
	uint32 GetCurWaterTextureIndex( uint32 nCurTime );

	/// 判断一个格子是不是水
	bool IsWater( int32 nGrid );

	/// 返回水的格子索引
	uint8 GetWaterGrid( uint32 index );
	/// 返回水的格子数
	uint GetWaterGridNum() const;
	/// 绘制波浪
	void RenderWave( ITexture* texture, uint32 nCurTime );
	/// 绘制涟漪
	void RenderDimple();

	uint32 GetOrgID(void);
};

inline uint32 CWater::GetOrgID(void)
{
	return m_OrgID;
}

//------------------------------------------------------------------------------
inline uint8 CWater::GetWaterGrid( uint32 index )
{
	return m_WaterGrid[index];
}

//------------------------------------------------------------------------------
inline uint CWater::GetWaterGridNum() const
{
	return m_WaterGrid.size();
}

//------------------------------------------------------------------------------
inline uint32 CWater::GetCurWaterTextureIndex( uint32 nCurTime )
{
	return ( nCurTime/33 )%m_nTexCount + m_nTexBegin;
}

//------------------------------------------------------------------------------
inline bool CWater::IsWater( int32 nGrid )
{
	return m_pWaterWave[nGrid] != WATER_NULL;
}

//------------------------------------------------------------------------------
inline void
CWater::GetTextureRange( uint& begin, uint& count )
{
	begin = this->m_nTexBegin;
	count = this->m_nTexCount;
}

//------------------------------------------------------------------------------
inline void
CWater::SetTextureRange( uint32 nTextBegin, uint32 nTextCount )
{
	m_nTexBegin = nTextBegin;
	m_nTexCount = nTextCount;
}

}


