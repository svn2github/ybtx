#include "stdafx.h"
#include "CCtrlNode.h"
#include "CNodeController.h"
namespace sqr
{
	CCtrlNode::CCtrlNode()
		:m_bVisible(true)
		,m_bNeedUpdate(false)
		,m_vScale(1.0f,1.0f,1.0f)
		,m_v3Position(CVector3f::ZERO)
		,m_pController(NULL)
	{
	}

	CCtrlNode::~CCtrlNode()
	{
		if(m_pController)
			m_pController->UnBind();
	}


	//平移
	void CCtrlNode::move(float x,float y, float z)
	{
		m_v3Position.x += x;
		m_v3Position.y += y;
		m_v3Position.z += z;
		m_bNeedUpdate = true;
		return;
	}

	void CCtrlNode::move(const CVector3f& v3 )
	{
		m_v3Position += v3;
		m_bNeedUpdate = true;
		return;
	}

	void CCtrlNode::moveRelative(float x,float y,float z )
	{
		return moveRelative(CVector3f(x,y,z));
	}

	void CCtrlNode::moveRelative(const CVector3f& v3)
	{
		m_v3Position += m_Quaternion*v3;
		m_bNeedUpdate =  true;
		return;
	}

	//旋转
	void CCtrlNode::Rotate(float alpha,float beta,float gamma)
	{
		//下面写得比较随意，等待更新，暂时这样 
		CVector3f axis = m_Quaternion * CVector3f::UNIT_X;
		CQuaternion	tempQ;
		tempQ.FromAngleAxis( alpha,axis );
		m_Quaternion =tempQ * m_Quaternion;

		axis = m_Quaternion * CVector3f::UNIT_Y;
		tempQ.FromAngleAxis( beta,axis );
		m_Quaternion = m_Quaternion * tempQ;

		axis = m_Quaternion * CVector3f::UNIT_Z;
		tempQ.FromAngleAxis( gamma,axis );

		return Rotate(tempQ);
	}

	void CCtrlNode::Rotate(const CVector3f& axis,float angle)
	{
		CQuaternion	tempQ;
		tempQ.FromAngleAxis(angle,axis);
		return Rotate(tempQ);
	}

	void CCtrlNode::Rotate(const CQuaternion& q)
	{
		//ie:u1*u2*p*u2'*u1'
		m_Quaternion = m_Quaternion * q;
		m_bNeedUpdate = true;
		return;
	}

	void CCtrlNode::Pitch(float alpha )
	{return Rotate(m_Quaternion * CVector3f::UNIT_X,alpha );}

	void CCtrlNode::Yaw(float beta )
	{return Rotate(m_Quaternion * CVector3f::UNIT_Y,beta);}

	void CCtrlNode::Roll(float gamma )
	{return Rotate(m_Quaternion * CVector3f::UNIT_Z,gamma);}

	void CCtrlNode::RotateX(float alpha)
	{ return Rotate(CVector3f::UNIT_X,alpha); }

	void CCtrlNode::RotateY(float beta)
	{ return Rotate(CVector3f::UNIT_Y,beta);	}

	void CCtrlNode::RotateZ(float gamma)
	{ return Rotate(CVector3f::UNIT_Z,gamma);	}

	void CCtrlNode::SetRotate(const CVector3f& axis,float angle)
	{
		m_Quaternion.FromAngleAxis(angle,axis);
		m_bNeedUpdate = true;
		return;
	}
	//坐标相关
	void CCtrlNode::SetPosition( float x, float y, float z )
	{
		m_v3Position.x = x;
		m_v3Position.y = y;
		m_v3Position.z = z;
		m_bNeedUpdate =  true;
		return; 
	}
	void  CCtrlNode::SetPosition(const CVector3f& v3 )
	{
		m_v3Position = v3;
		m_bNeedUpdate =  true;
		return; 
	}

	const CVector3f& CCtrlNode::GetPosition() const
	{ return m_v3Position; }

	void CCtrlNode::setScale( float x,float y,float z)
	{
		m_vScale.x = x;
		m_vScale.y = y;
		m_vScale.z = z;
		m_bNeedUpdate =  true;
	}

	void CCtrlNode::setScale( const CVector3f& v3 )
	{
		m_vScale = v3;
		m_bNeedUpdate =  true;
	}

	void CCtrlNode::setScale( float Scale) //保持同一种比例缩放
	{ 
		m_vScale.x = m_vScale.y = m_vScale.z = Scale; 
		m_bNeedUpdate =  true;
	}

	void CCtrlNode::Scale( float x,float y,float z)
	{
		m_vScale.x *= x;
		m_vScale.y *= y;
		m_vScale.z *= z;
		m_bNeedUpdate =  true;
	}

	void CCtrlNode::Scale( const CVector3f& v3 )
	{
		m_vScale *= v3;
		m_bNeedUpdate =  true;
	}

	void CCtrlNode::Scale( float Scale) // 保持同一种比例缩放
	{ 
		m_vScale *= Scale; 
		m_bNeedUpdate =  true;
	}


	// 绕中心旋转
	void CCtrlNode::centerRotate( const CVector3f& center,float alpha,float beta,float gamma )
	{
		CQuaternion	tempQ;
		CQuaternion	CachedQ;
		CVector3f axis = m_Quaternion * CVector3f::UNIT_X;
		tempQ.FromAngleAxis(alpha,axis);
		axis = m_Quaternion * CVector3f::UNIT_Y;
		CachedQ.FromAngleAxis(beta,axis);
		tempQ = tempQ * CachedQ;
		axis = m_Quaternion * CVector3f::UNIT_Z;
		CachedQ.FromAngleAxis(gamma,axis);
		tempQ = tempQ * CachedQ;

		centerRotate( center,tempQ );
		return; 
	}

	void CCtrlNode::centerRotate( const CVector3f& center,const CQuaternion& q)
	{
		m_Quaternion = m_Quaternion * q;
		m_v3Position = q*(m_v3Position - center);
		m_v3Position += center;	
		m_bNeedUpdate = true;
	}

	void CCtrlNode::centerRotate(const CVector3f& center,const CVector3f& axis,float angle )
	{
		CQuaternion	tempQ;
		tempQ.FromAngleAxis( angle,axis );
		centerRotate( center,tempQ );
		return;
	}

	void CCtrlNode::centerPitch( const CVector3f& center,float alpha )
	{
		CQuaternion	tempQ;
		tempQ.FromAngleAxis(alpha,m_Quaternion * CVector3f::UNIT_X);
		centerRotate( center,tempQ );
		return; 
	}

	void CCtrlNode::centerYaw( const CVector3f& center,float beta )
	{
		CQuaternion	tempQ;
		tempQ.FromAngleAxis(beta,m_Quaternion * CVector3f::UNIT_Y);
		centerRotate( center,tempQ );
		return; 
	}

	void CCtrlNode::centerRoll( const CVector3f& center,float gamma )
	{
		CQuaternion	tempQ;
		tempQ.FromAngleAxis(gamma,m_Quaternion * CVector3f::UNIT_Z);
		centerRotate( center,tempQ );
		return; 
	}

	void CCtrlNode::setTransferMatrix( const CMatrix& mat4 )
	{
		m_matCached = mat4;
		m_v3Position.Init( mat4.m[3][0],mat4.m[3][1],mat4.m[3][2]);
		m_Quaternion.FromRotationMatrix( mat4 );
		m_bNeedUpdate = false;
		return; 
	}

	const CMatrix& CCtrlNode::GetWorldMatrix()	//获取当前矩阵
	{
		Update();
		return m_matCached; 
	}

	void CCtrlNode::setVisible( bool IsVisible )
	{ m_bVisible = IsVisible; }

	bool CCtrlNode::getVisible()
	{ return m_bVisible; }

	void CCtrlNode::setDIRECTION( const CVector3f& fVector )
	{
		if (CVector3f::ZERO == fVector)
			return;
		//CVector3f dirFixed = fVector;
		//dirFixed.Normalize();	
		//Update();
		//CQuaternion	tempCached;
		//if ( CVector3f::ZERO == dirFixed+getDIRECTION())
		//{
		//	tempCached.FromAngleAxis(CMath::pi,getUP());
		//}
		//else
		//{
		//	CVector3f axis = getDIRECTION().Cross(dirFixed);
		//	float c = getDIRECTION().Dot(dirFixed);

		//	if (c >= 1.0f || c<=-1.0f)
		//	{
		//		tempCached.x=0;
		//		tempCached.y=0;
		//		tempCached.z=0;
		//		tempCached.w=1.0;
		//	}
		//	else
		//	{
		//			//根据2倍角公式，s=2cosa
		//		float s = CMath::Sqrt( (1+c)*2 );
		//		float invs = 1 / s;
		//		tempCached.x = axis.x * invs;
		//		tempCached.y = axis.y * invs;
		//		tempCached.z = axis.z * invs;
		//		tempCached.w = s * 0.5f;
		//	}


		//}
		//m_Quaternion = tempCached * m_Quaternion;
		if( CVector3f::ZERO == fVector + CVector3f::UNIT_Z )
		{
			m_Quaternion.x=0.0f;m_Quaternion.y=1.0f;m_Quaternion.z=0.0f;m_Quaternion.w=0.0f;
		}
		else
			m_Quaternion.FromTwoVector(CVector3f::UNIT_Z,fVector);
		m_bNeedUpdate = true;
	}

	void CCtrlNode::Update(void) const
	{
		if(m_bNeedUpdate)
		{
			//////////////////////////////////////////////////////////////////////////
			//物体的世界矩阵乘法顺序
			//1.缩放
			//2.旋转
			//3.移动
			//PS.N多年后看到这里，要相信自己
			//////////////////////////////////////////////////////////////////////////
			m_Quaternion.ToRotationMatrix( m_matCached );
			m_matCached.m[3][0] = m_v3Position.x;
			m_matCached.m[3][1] = m_v3Position.y;
			m_matCached.m[3][2] = m_v3Position.z;
			m_matCached.getColumn(0)*=m_vScale.x;
			m_matCached.getColumn(1)*=m_vScale.y;
			m_matCached.getColumn(2)*=m_vScale.z;
			m_bNeedUpdate = false;
			_UpdateEx();
		}
		return;
	}
}