#include "stdafx.h"
#include "CSimCloth.h"
#include "TimeHelper.h"

namespace sqr
{
namespace Cloth
{
int CSoftVerNode::Calculate( float dt )
{
	if ( !m_IsFix )
	{
		m_Accel     = m_Force / m_MASS;
		m_Speed    += m_Accel * dt;
		*m_Vertex  = *m_Vertex + m_Speed * dt;
	}
	return 0;
}

CSoftVerNode::CSoftVerNode( CVector3f* pVertex, float mass, float GRAVITY, float GASSFORCE )
{
	m_Vertex     = pVertex;

	m_Force.Init( 0, 0, 0 );
	m_Accel.Init( 0, 0, 0 );
	m_Speed.Init( 0, 0, 0 );
	m_Wind.Init( 0, 0, 0 );
	m_WindPower = 0;
	m_MASS  = mass;
	m_IsFix = false;
	m_GRAVITY   = GRAVITY;
	m_GASSFORCE = GASSFORCE;
}

void CSoftVerNode::ResetForce()
{
	CVector3f WindForce( m_Wind * m_WindPower );
	CVector3f WindForceH( m_Wind.Cross( CVector3f(0,1,0) ) );
	WindForceH.Normalize();
	WindForceH = WindForceH * m_WindPowerH;

	m_Force.Init(  0, -m_GRAVITY * m_MASS , 0 );
	m_Force += -m_GASSFORCE * m_Speed + WindForce + WindForceH;
}

CCloth::CCloth( ):m_WindPowerMin(0),m_WindPowerMax(0),m_GRAVITY(0.0f),m_GASSFORCE(0.0f),m_TimeBase(0.0f),m_TimePass(0.0f),m_LastUpdateWindTime(0)
{
	m_LastUpdateTime = DWORD(GetProcessTime());
}

CCloth::~CCloth()
{
}

uint16 CCloth::AddNode( const CSoftVerNode& Node )
{
	m_Node.push_back( Node );
	return static_cast<uint16>(m_Node.size() - 1);
}

uint16 CCloth::AddSpring( uint16 Index1, uint16 Index2, float k, float StaticLen, float fLengthMax, float fLengthMin )
{
	m_Spring.push_back( CSpring( &m_Node[Index1], &m_Node[Index2], k, StaticLen, fLengthMax, fLengthMin ) );
	return static_cast<uint16>(m_Spring.size() - 1);
}

void CCloth::SetFixNode( uint16 Index )
{
	m_Node[Index].SetFix( true );
}

void CCloth::Calculate( const CVector3f& vDir )
{
	DWORD TickNow  = DWORD(GetProcessTime());
	DWORD TickPass = TickNow - m_LastUpdateTime;
	TickPass = TickPass > 333 ? 333: TickPass;
	for ( int i = 0; i < (int)m_Node.size(); ++i)
	{
		m_Node[i].Calculate( TickPass / 1000.0f );
	}
	m_LastUpdateTime = TickNow;

	m_Wind = vDir;
	if ( TickNow - m_LastUpdateWindTime > 33 )
	{
		m_LastUpdateWindTime = TickNow;
		m_TimeBase += m_TimePass * rand() / (float)RAND_MAX;
	}

	for ( int i = 0; i < (int)m_Node.size();   ++i )
	{
		float r = rand() / (float)RAND_MAX * ( sin( m_TimeBase ) );
		m_Node[i].SetWind( m_Wind, (m_WindPowerMin + (m_WindPowerMax * r )),(m_WindPowerMaxH * r )   );
		m_Node[i].ResetForce();
	}
	for ( int i = 0; i < (int)m_Spring.size(); ++i)
	{
		if ( m_Spring[i].CalculateForce() ) {}
	}

}

void CCloth::SetWind( uint16 WindPowerMin, uint16 WindPowerMax, uint16 WindPowerMaxH, float TimePass )
{
	m_WindPowerMin = WindPowerMin;
	m_WindPowerMax = WindPowerMax;
	m_TimePass     = TimePass;
	m_WindPowerMaxH = WindPowerMaxH;
}


CSpring::CSpring( CSoftVerNode* Node1, CSoftVerNode* Node2 , float k, float StaticLen, float fLengthMax, float fLengthMin )
{
	m_Node1     = Node1;
	m_Node2     = Node2;
	m_K         = k;
	m_StaticLen = (StaticLen>=0)? StaticLen : (Node1->GetVertex() - Node2->GetVertex()).Mag();
	m_LengthMax = m_StaticLen * fLengthMax;
	m_LengthMin = m_StaticLen * fLengthMin;
}

bool CSpring::CalculateForce()
{
	CVector3f   d      = m_Node2->GetVertex() - m_Node1->GetVertex();
	float       Length = d.Mag();
	d.Normalize();

	if ( Length > m_LengthMax )			// 限制过渡拉伸
	{
		m_Node2->GetVertex() = m_Node1->GetVertex() + d * m_LengthMax;
	}
	else if ( Length < m_LengthMin )		// 限制过渡压缩
	{
		m_Node2->GetVertex() = m_Node1->GetVertex() + d * m_LengthMin;
	}
	else if ( Length > m_StaticLen )		// 正常变形范围，计算弹簧力，并施加在两端的质点上
	{
		d = d * m_K * ( Length - m_StaticLen );
		m_Node1->AddForce( d );
		d *= -1.0f;
		m_Node2->AddForce( d );
	}
	return true;
}
};
};