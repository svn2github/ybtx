#pragma once

#include "CMatrix.h"
#include "CVector3.h"
#include "CQuaternion.h"
#include "CGraphicMallocObject.h"
#include "CDynamicObject.h"
#include "CRenderSpaceNode.h"

namespace sqr
{
	class CNodeController;
	class CCtrlNode 
		: public CGraphicMallocObject
		, public CRenderSpaceNode
		, public virtual CDynamicObject //这里出现CBaseObject是合理的，所以说明CCtrlNode应该是Scene的一部分
	{
		friend class CNodeController;
	protected:
		CQuaternion m_Quaternion;
		CVector3f	m_v3Position;
		CVector3f	m_vScale;
		///CVector3f	m_v3TargetPos;//CurCameraDest;
	public:

		CCtrlNode();
		virtual ~CCtrlNode();

		void move( float x, float y, float z ); //相对平移,世界坐标平移
		void move( const CVector3f& v3 );

		void moveRelative( const CVector3f& v3 ); //按自身坐标平移
		void moveRelative( float x,float y,float z );//按自身坐标平移

		//绕自身旋转
		void Rotate(float alpha,float beta,float gamma); //出于性能考虑 不建议使用
		void Rotate(const CVector3f& axis,float angle);
		void Rotate(const CQuaternion& q);
		void Pitch( float alpha );
		void Yaw( float beta );
		void Roll( float gamma );
		void SetRotate(const CVector3f& axis,float angle);

		//绕固定轴旋转
		void RotateX(float alpha);
		void RotateY(float beta);
		void RotateZ(float gamma);

		void SetPosition( float x, float y, float z );//设置绝对坐标
		void SetPosition( const CVector3f& v3 );
		const CVector3f& GetPosition() const; //获取当前坐标

		void setScale( float x,float y,float z);
		void setScale( const CVector3f& v3 );
		void setScale( float Scale); //保持同一种比例缩放

		void Scale( float x,float y,float z);
		void Scale( const CVector3f& v3 );
		void Scale( float Scale); //保持同一种比例缩放

		// 以下绕中心旋转函数由于设计改变
		void centerRotate( const CVector3f& center,float alpha,float beta,float gamma ); //绕中心旋转
		void centerRotate( const CVector3f& center,const CVector3f& axis,float angle); //绕中心旋转
		void centerRotate( const CVector3f& center,const CQuaternion& q);
		void centerPitch( const CVector3f& center,float alpha );
		void centerYaw( const CVector3f& center,float beta );
		void centerRoll( const CVector3f& center,float gamma ); 

		void setTransferMatrix( const CMatrix& mat4  ); //设置变换矩阵
		const CMatrix& GetWorldMatrix();	//获取当前矩阵

		void setVisible( bool IsVisible = true );
		bool getVisible();
		
		const CVector3f& getUP(void)		const;
		const CVector3f& getRIGHT(void)		const;
		const CVector3f& getDIRECTION(void)	const;
		//设置观察方向
		void setDIRECTION( const CVector3f& fVector );
		
		
	protected:			
				void	Update(void)		const;
		virtual void	_UpdateEx(void)		const{};
				bool	m_bVisible;

		mutable CMatrix	m_matCached; //用于储存矩阵
		mutable	bool	m_bNeedUpdate;
		CNodeController*	m_pController;
	private:
	};

	inline const CVector3f& CCtrlNode::getUP(void) const
	{
		Update();
		return *(CVector3f*)&m_matCached.m[1];
	}

	inline const CVector3f& CCtrlNode::getRIGHT(void) const
	{
		Update();
		return *(CVector3f*)&m_matCached.m[0];
	}

	inline const CVector3f& CCtrlNode::getDIRECTION(void) const
	{
		Update();
		return *(CVector3f*)&m_matCached.m[2];
	}
}