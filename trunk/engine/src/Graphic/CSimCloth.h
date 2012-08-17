#pragma once
#include "CVector3.h"
#include "CGraphicMallocObject.h"

namespace sqr
{
namespace Cloth
{
///柔体质点.
class CSoftVerNode : public CGraphicMallocObject
{
public:
	CVector3f*		m_Vertex;	///< 顶点(位置)

	CVector3f		m_Force;	///< 力
	CVector3f		m_Accel;	///< 加速度
	CVector3f		m_Speed;	///< 速度
	CVector3f		m_Wind;		///< 风向
	float           m_MASS;			///< 质量
	bool            m_IsFix;		///< 是否为固定点
	float           m_GRAVITY;		///< 重力加速度
	float           m_GASSFORCE;
	float           m_WindPower;	///< 风力
	float           m_WindPowerH;

	CSoftVerNode() {};
	CSoftVerNode( CVector3f* pVertex, float mass, float GRAVITY, float GASSFORCE);

	void ResetForce();

	/// 更新结点位置.
	/// @param	float deltatime	距离上次计算的时间
	int32	Calculate( float deltatime );
	void	SetFix( bool Fix )
	{
		m_IsFix  = Fix;
	}
	void SetWind( CVector3f& W, float windpower, float windpowerH = 0 )
	{
		m_Wind   = W;
		m_WindPower   = windpower;
		m_WindPowerH = windpowerH;
	}
	/// 施加一个新的力
	void	AddForce( CVector3f& F )
	{
		m_Force += F;
	}
	bool	IsFixed()
	{
		return m_IsFix;
	}

	CVector3f& GetVertex()
	{
		return *m_Vertex;
	}
};

/// 弹簧.
/// 胡克定律: F(力)=k(倔强系数) * d(弹性形变)
class CSpring : public CGraphicMallocObject
{

public:
	CSoftVerNode*   m_Node1;	///< 质点1
	CSoftVerNode*   m_Node2;	///< 质点2
	float   m_StaticLen;		///< 静态长度
	float   m_K;				///< 倔强系数
	float   m_LengthMax;		///< 最小长度
	float   m_LengthMin;		///< 最大长度

	CSpring( CSoftVerNode* Node1, CSoftVerNode* Node2 ,  float k, float StaticLen, float fLengthMax, float fLengthMin);

	/// 计算施加在质点上的力
	bool CalculateForce();
};

/// 布料.
/// 采用“质点-弹簧”模型进行模拟
class CCloth : public CGraphicMallocObject
{
public:

	GVector<CSpring>			m_Spring;	///< 弹簧集合
	GVector<CSoftVerNode>	m_Node;		///< 质点集合

	CVector3f			m_Wind;			///< 风向
	int16               m_WindPowerMin;	///< 最小风力
	int16               m_WindPowerMax;	///< 最大风力
	int16               m_WindPowerMaxH;
	float				m_GRAVITY;      //< 重力加速度
	float				m_GASSFORCE;	//< 空气阻力（牛顿/（米/秒） ）
	float               m_TimeBase;		///< 开始时间
	float               m_TimePass;		///< 逝去时间


	DWORD				m_LastUpdateTime;		///< 上次更新时间
	DWORD				m_LastUpdateWindTime;	///< 上次风力更新时间

	CCloth();
	~CCloth(void);

	void    Calculate( const CVector3f& vDir );
	void    SetFixNode( uint16 Index );
	void    SetWind( uint16 WindPowerMin, uint16 WindPowerMax, uint16 m_WindPowerMaxH, float TimePass );
	uint16  AddSpring( uint16 Index1, uint16 Index2, float k, float StaticLen, float fLengthMax, float fLengthMin);
	uint16  AddNode( const CSoftVerNode& Node );

	GVector<CSoftVerNode>&   GetNode()
	{
		return m_Node;
	}
	GVector<CSpring>&        GetSpring()
	{
		return m_Spring;
	}
};

};
};
