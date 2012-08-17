#pragma once

#include <vector>

#include "Base.h"
#include "GraphicBase.h"

namespace Cloth
{
    namespace ClothMath
    {
        void ClosestPointOnLine(const CVector3f &vA, const CVector3f &vB, const CVector3f &vPoint,CVector3f &vClosestPoint);
        void MultNorMat( CVector3f & Out, CVector3f & In, const CMatrix & mat );
        void MultPosMat( CVector3f & Out, CVector3f & In, const CMatrix & mat );
    };

    class CSoftVerNode
    {
    public:
        VerNorTex*		m_Vertex;

        CVector3f		m_Force;
        CVector3f		m_Accel;
        CVector3f		m_Speed;
        CVector3f		m_Wind;
        float           m_MASS;
        bool            m_IsFix;
        float           m_GRAVITY;
        float           m_GASSFORCE;
        float           m_WindPower;

        CSoftVerNode(){};
        CSoftVerNode( VerNorTex* pVertex, CVector3f& p, CVector3f& np, float stu, float stv, float mass, float GRAVITY, float GASSFORCE)
        {
            m_Vertex     = pVertex;
            m_Vertex->p  = p;
            m_Vertex->np = np;
            m_Vertex->tu = stu;
            m_Vertex->tv = stv;

            m_Force.Init( 0, 0, 0 );
            m_Accel.Init( 0, 0, 0 );
            m_Speed.Init( 0, 0, 0 );
            m_Wind.Init( 0, 0, 0 );
            m_MASS  = mass;
            m_IsFix = false;
            m_GRAVITY   = GRAVITY;
            m_GASSFORCE = GASSFORCE;
            m_WindPower = 0.0f;
        }

        void ResetForce()
        {	
            CVector3f WindForce( m_Wind * m_WindPower );
            m_Force.Init(  0, -m_GRAVITY * m_MASS , 0 );
            m_Force += -m_GASSFORCE * m_Speed + WindForce;
        }

        int  Calculate( float dt );
        void SetFix( bool Fix )			{ m_IsFix  = Fix; }
        void SetWind( CVector3f& W, float windpower )	{ m_Wind   = W; m_WindPower   = windpower;  }
        void AddForce( CVector3f& F )	{ m_Force += F;  }
        BOOL IsFixed()                  { return m_IsFix; }

        VerNorTex* GetVertex()			{ return m_Vertex; }
    };

    class CSpring
    {

    public:
        CSoftVerNode*   m_Node1;
        CSoftVerNode*   m_Node2;
        float   m_StaticLen;    
        float   m_K;
        float   m_LengthMax;
        float   m_LengthMin;
        
        CSpring( CSoftVerNode* Node1, CSoftVerNode* Node2 ,  float k, float StaticLen, float fLengthMax, float fLengthMin)
        {
            m_Node1     = Node1;
            m_Node2     = Node2;
            m_K         = k;
            m_StaticLen = (StaticLen>=0)? StaticLen : (Node1->GetVertex()->p - Node2->GetVertex()->p).Mag();
            m_LengthMax = m_StaticLen * fLengthMax;
            m_LengthMin = m_StaticLen * fLengthMin;
        }

        bool CalculateForce()
        {
            CVector3f   d      = m_Node2->GetVertex()->p - m_Node1->GetVertex()->p;
            float       Length = d.Mag();
            d.Normalize();

            // 限制过渡拉扯
            if( Length > m_LengthMax ) { 
                m_Node2->GetVertex()->p = m_Node1->GetVertex()->p + d * m_LengthMax; 
            } 
            else if( Length < m_LengthMin ) { 
                m_Node2->GetVertex()->p = m_Node1->GetVertex()->p + d * m_LengthMin; 
            }
            else 
            if( Length > m_StaticLen ) 
            {
                d = d * m_K * ( Length - m_StaticLen );
                m_Node1->AddForce( d );
                d *= -1.0f;
                m_Node2->AddForce( d );
            }
            return true;
        }
    };

    class CCloth
    {
        public:

        vector<CSpring>		    m_Spring;
        vector<CSoftVerNode>	m_Node;

        CVector3f			m_Wind;
        float               m_WindPowerMin;
        float               m_WindPowerMax;
        float               m_TimeBase;
        float               m_TimePass;

        float m_GRAVITY;            //重力加速度
        float m_GASSFORCE;			//空气阻力（牛顿/（米/秒） ）

        CCloth();
        ~CCloth(void);

        void    Calculate( const CMatrix& worldMat );
        void    SetFixNode( uint16 Index );
        void    SetWind( float WindPowerMin, float WindPowerMax, float TimePass );
        uint16  AddSpring( uint16 Index1, uint16 Index2, float k, float StaticLen, float fLengthMax, float fLengthMin);
        uint16  AddNode( const CSoftVerNode& Node );

        vector<CSoftVerNode>&   GetNode()   { return m_Node; }
        vector<CSpring>&        GetSpring() { return m_Spring; }
    };

};
