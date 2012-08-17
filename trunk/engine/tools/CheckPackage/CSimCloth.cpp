#include "stdafx.h"
#include "CSimCloth.h"

namespace Cloth
{
    namespace ClothMath
    {
        void ClosestPointOnLine(const CVector3f &vA, const CVector3f &vB, const CVector3f &vPoint,CVector3f &vClosestPoint)
        {
            CVector3f vVector2 = vB - vA;
            float d = vVector2.Mag();
            
            vVector2.Normalize();
            float t = vVector2.Dot((vPoint - vA));

            if (t <= 0) { vClosestPoint = vA; return; }
            if (t >= d) { vClosestPoint = vB; return; }
            vClosestPoint = vA + vVector2 * t;
        }
        void MultPosMat( CVector3f & Out, CVector3f & In, const CMatrix & mat )
        {
            Out.x = In.x*mat._11 + In.y*mat._21 + In.z*mat._31 + mat._41;
            Out.y = In.x*mat._12 + In.y*mat._22 + In.z*mat._32 + mat._42;
            Out.z = In.x*mat._13 + In.y*mat._23 + In.z*mat._33 + mat._43;
        }

        void MultNorMat( CVector3f & Out, CVector3f & In, const CMatrix & mat )
        {
            Out.x = In.x*mat._11 + In.y*mat._21 + In.z*mat._31;
            Out.y = In.x*mat._12 + In.y*mat._22 + In.z*mat._32;
            Out.z = In.x*mat._13 + In.y*mat._23 + In.z*mat._33;
        }

    };

    int CSoftVerNode::Calculate( float dt )
    {	
        if( !m_IsFix )
        {
            m_Accel     = m_Force / m_MASS;
            m_Speed    += m_Accel * dt;
            m_Vertex->p  = m_Vertex->p + m_Speed * dt;
        }
        return 0;
    }

    CCloth::CCloth( )
    {
        m_GRAVITY       = 300.0f;
        m_GASSFORCE     = 30.0f;
        m_WindPowerMin  = 0.0f;
        m_WindPowerMax  = 0.0f;
        m_TimeBase      = 0.0f;
    }

    CCloth::~CCloth()
    {
    }

    uint16 CCloth::AddNode( const CSoftVerNode& Node )
    {
        m_Node.push_back( Node );
        return m_Node.size() - 1;
    }

    uint16 CCloth::AddSpring( uint16 Index1, uint16 Index2, float k, float StaticLen, float fLengthMax, float fLengthMin )
    {
        m_Spring.push_back( CSpring( &m_Node[Index1], &m_Node[Index2], k, StaticLen, fLengthMax, fLengthMin ) );
        return m_Spring.size() - 1;
    }

    void CCloth::SetFixNode( uint16 Index )
    {
        m_Node[Index].SetFix( true );
    }

    void CCloth::Calculate( const CMatrix& worldMat )
    {
        m_TimeBase += m_TimePass * rand() / (float)RAND_MAX;

        CVector3f t;
        ClothMath::MultNorMat( t, CVector3f(0, 0, -1), worldMat );

        for( int i = 0; i < (int)m_Node.size(); ++i) 
        {
            m_Node[i].Calculate( 0.03f );
        }

        m_Wind.Init( t.x, t.y, t.z );

        for( int i = 0; i < (int)m_Node.size();   ++i ) 
        {

            m_Node[i].SetWind( m_Wind, (m_WindPowerMin + (m_WindPowerMax * rand() / (float)RAND_MAX * ( sin( m_TimeBase ) ))));
            m_Node[i].ResetForce();
        }
        for( int i = 0; i < (int)m_Spring.size(); ++i)
        {
            if( m_Spring[i].CalculateForce() ){}
        }
    }

    void CCloth::SetWind( float WindPowerMin, float WindPowerMax, float TimePass )
    {
        m_WindPowerMin = WindPowerMin;
        m_WindPowerMax = WindPowerMax;
        m_TimePass     = TimePass;
    }

};
