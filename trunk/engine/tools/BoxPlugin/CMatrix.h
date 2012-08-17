#pragma once
/******************************************************************************
/*! \file       CMatrix.h
*   \brief      矩阵类的定义
*
*				定义各种可能的构造矩阵的方法,和相关的矩阵操作
*   \version	1.0
*   \date		01-2-10 9:40:12
*   \sa			CCoord, CVector3f
*   \todo		还没有做完整的测试及代码的优化
*******************************************************************************/


#include <cmath>
#include <cstring>
#include "CVector3.h"


namespace sqr
{
class CMatrix
{
public:

	union
	{
		struct
		{
			float	_11, _12, _13, _14;
			float	_21, _22, _23, _24;
			float	_31, _32, _33, _34;
			float	_41, _42, _43, _44;
		};
		float	m[4][4];
	};
	/**
	* we're using m[y][x] as our notation.
	*/

	//! 缺省构造函数
	CMatrix();
	//! 拷贝构造函数
	CMatrix(const CMatrix&	mat);
	//! 用Euler角构造矩阵
	CMatrix(float fPitch, float fYaw, float fRoll);
	//! 构造绕某个轴旋转某个角度的变换矩阵
	CMatrix(const CVector3f& vDir, float fRads);
	//! 根据三个矢量和原点(坐标轴)(都是相对于世界坐标系)，构造转换矩阵
	CMatrix(const CVector3f& vX, const CVector3f& vY,
	        const CVector3f& vZ, const CVector3f& vOrg);
	//! 根据起始，终止矢量和向上的矢量，构造视矩阵
	CMatrix(const CVector3f& vFrom, const CVector3f& vAt,
	        const CVector3f& vUp);

	~CMatrix();


	//!按行设置矩阵
	void SetWithRow(const CVector3f& r0,const CVector3f& r1,const CVector3f& r2);
	//!按列设置矩阵
	void SetWithColumn(const CVector3f& c0,const CVector3f& c1,const CVector3f& c2);
	//!  求同一矩阵
	inline void Identity();
	//! 求逆矩阵
	bool		Invert();
	//! 求逆矩阵
	void		InvertFast();
	//! 求转置矩阵
	void		Transpose();
	//! 矩阵相乘
	CMatrix	operator * (const CMatrix& mat) const;
	//! 矩阵自乘
	CMatrix&	operator *= (const CMatrix& mat);
	// 矩阵和矢量相乘
	const CVector3f operator*( const CVector3f &b);
	//! 设置偏移矩阵
	void SetTranslate( float tx, float ty, float tz );
	//! 设置偏移矩阵
	void SetTranslate( const CVector3f& v );
	//! 设置缩放矩阵
	void SetScale( float sx, float sy, float sz );
	//! 设置缩放矩阵
	void SetScale( const CVector3f& v );

	//! 根据一个传入的坐标系(相对于世界坐标系),构造一个转换矩阵
	//void SetTransform(const CCoord& crd, bool IsWorldToObj = true);

	//! 根据起始，终止矢量和向上的矢量(都是相对于世界坐标系)，构造转换矩阵
	// 因该去掉，只使用第一个
	bool SetTransform( const CVector3f& vFrom,
	                   const CVector3f& vAt,
	                   const CVector3f& vUp );

	//! 根据三个矢量(坐标轴)和原点(都是相对于世界坐标系)，构造转换矩阵
	void SetTransform( const CVector3f& vX,
	                   const CVector3f& vY,
	                   const CVector3f& vZ,
	                   const CVector3f& vOrg);

	//! 构造从一个坐标系到另一个坐标系的转换矩阵
	//void SetTransform(const CCoord& crdSrc, const CCoord& crdDes);

	//! 根据传入的参数构造投影矩阵
	bool SetProjection( float fFOV = 1.570795f,
	                    float fAspect = 1.0f,
	                    float fNearPlane = 1.0f,
	                    float fFarPlane = 1000.0f );

	//! 设置绕X轴旋转fRads弧度的变换矩阵
	void    SetRotateX( float fRads );
	//! 设置绕Y轴旋转fRads弧度的变换矩阵
	void    SetRotateY( float fRads );
	//! 设置绕Z轴旋转fRads弧度的变换矩阵
	void    SetRotateZ( float fRads );
	//! 构造Euler旋转矩阵
	void	SetRotation(float fPitch, float fYaw, float fRoll);
	//! 设置绕某个轴旋转某个弧度的变换矩阵
	void    SetRotation( const CVector3f& vDir, float fRads );

	const CVector3f GetLoc() const;

	/**
	* GetAxis only returns the correct axis if we're talking about a matrix that
	* transforms 'up' towards worldspace.  For example, this will return the
	* correct axis if called on a object->world matrix, but incorrect one if
	* called on the world->camera matrix.  (If one needs the axes for such a
	* matrix, invert the world->camera transform to get a camera->world
	* transform)
	*/
	CVector3f GetAxis( int axis ) const;

protected:


};

//! 缺省构造函数
inline CMatrix::CMatrix()
		: _11(1.0f), _12(0.0f), _13(0.0f), _14(0.0f)
		, _21(0.0f), _22(1.0f), _23(0.0f), _24(0.0f)
		, _31(0.0f), _32(0.0f), _33(1.0f), _34(0.0f)
		, _41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f)
{
	//Identity();
}

//! 析购函数
inline CMatrix::~CMatrix()
{

}

//! 拷贝构造函数
inline CMatrix::CMatrix(const CMatrix&	mat)
{
	memcpy(this, &mat, sizeof(CMatrix));
}

//******************************************************************************
/*! \fn     CMatrix::CMatrix(float fPitch, float fYaw, float fRoll)
*   \brief  用Euler角构造矩阵，参考SetRotation
*   \param  float fPitch	相对于x轴旋转的弧度
*   \param  float fYaw		相对于y轴旋转的弧度
*   \param  float fRoll		相对于z轴旋转的弧度
*   \sa     SetRotation(float fPitch, float fYaw, float fRoll)
*******************************************************************************/
inline CMatrix::CMatrix(float fPitch, float fYaw, float fRoll)
{
	SetRotation(fPitch, fYaw, fRoll);
}

//******************************************************************************
/*! \fn     CMatrix::CMatrix(const CVector3f& vDir, float fRads)
*   \brief   构造绕某个轴旋转某个角度的变换矩阵,参考SetRotation
*
*   \param  const CVector3f&	vDir	表示空间中的一个轴
*   \param  float				fRads	旋转的弧度值
*   \sa     SetRotion(const CVector3f& vDir, float fRads)
*******************************************************************************/
inline CMatrix::CMatrix(const CVector3f& vDir, float fRads)
{
	SetRotation(vDir, fRads);
}

//******************************************************************************
/*! \fn     CMatrix::CMatrix(const CVector3f& vX, const CVector3f& vY,
*					const CVector3f& vZ, const CVector3f& vOrg)
*   \brief  根据三个矢量和原点(坐标轴)构造视矩阵, 参考SetView
*
*			根据视空间的三个坐标轴和原点形成世界空间到此视空间的4*4变换矩阵
*   \param  const CVector3f& vX	视空间的x轴
*   \param  const CVector3f& vY	视空间的y轴
*   \param  const CVector3f& vZ	视空间的z轴
*   \param  const CVector3f& vOrg	视空间的原点
*   \sa     SetView
*******************************************************************************/
inline CMatrix::CMatrix(const CVector3f& vX, const CVector3f& vY,
                        const CVector3f& vZ, const CVector3f& vOrg)
{
	SetTransform(vX, vY, vZ, vOrg);
}

//******************************************************************************
/*! \fn     CMatrix::CMatrix(const CVector3f& vFrom, const CVector3f& vAt,
*									const CVector3f& vUp)
*   \brief  根据起始，终止矢量和向上的矢量，构造视矩阵, 参考SetView
*
*   \param  const CVector3f& vFrom		起始矢量
*   \param  const CVector3f& vAt		终止矢量
*   \param  const CVector3f& vUp		向上的矢量
*   \sa     SetView
*******************************************************************************/
inline CMatrix::CMatrix(const CVector3f& vFrom, const CVector3f& vAt,
                        const CVector3f& vUp)
{
	SetTransform(vFrom, vAt, vUp);
}


//******************************************************************************
/*! \fn     void CMatrix::SetWithRow(const CVector3f& r0,const CVector3f& r1,const CVector3f& r2)
*   \brief  按行设置矩阵
*
*   \param  const CVector3f& r0 第一行
*   \param  const CVector3f& r1 第二行
*   \param  const CVector3f& r2 第三行
*   \return 无
*******************************************************************************/
inline void CMatrix::SetWithRow(const CVector3f& r0,const CVector3f& r1,const CVector3f& r2)
{
	_11 = r0.x,_12 = r0.y,_13 = r0.z,_14 = 0;
	_21 = r1.x,_22 = r1.y,_23 = r1.z,_24 = 0;
	_31 = r2.x,_32 = r2.y,_33 = r2.z,_34 = 0;
	_41 = 0,   _42 = 0,   _43 = 0,   _44 = 1;
}

//******************************************************************************
/*! \fn     void CMatrix::SetWithColumn(const CVector3f& c0,const CVector3f& c1,const CVector3f& c2);
*   \brief  按行设置矩阵
*
*   \param  const CVector3f& c0 第一列
*   \param  const CVector3f& c1 第二列
*   \param  const CVector3f& c2 第三列
*   \return 无
*******************************************************************************/
inline void CMatrix::SetWithColumn(const CVector3f& c0,const CVector3f& c1,const CVector3f& c2)
{
	_11 = c0.x,_12 = c1.x,_13 = c2.x,_14 = 0;
	_21 = c0.y,_22 = c1.y,_23 = c2.y,_24 = 0;
	_31 = c0.z,_32 = c1.z,_33 = c2.z,_34 = 0;
	_41 = 0,   _42 = 0,   _43 = 0,   _44 = 1;
}

//******************************************************************************
/*! \fn     void CMatrix::Identity()
*   \brief  设置同一矩阵
*
*           同一矩阵对角为一,矩阵与同一矩阵相乘后不会改变
*   \return void
*******************************************************************************/
inline void CMatrix::Identity()
{
	_12 = _13 = _14 = _21 = _23 = _24 = 0.0f;
	_31 = _32 = _34 = _41 = _42 = _43 = 0.0f;
	_11 = _22 = _33 = _44 = 1.0f;
}


//******************************************************************************
/*! \fn     void CMatrix::SetTranslate( float tx, float ty, float tz )
*   \brief  设置偏移矩阵
*   \param		float tx x方向上的偏移
*   \param		float ty y方向上的偏移
*   \param		float tz z方向上的偏移
*   \return void
*******************************************************************************/
inline void CMatrix::SetTranslate( float tx, float ty, float tz )
{
	Identity();
	_41 = tx;
	_42 = ty;
	_43 = tz;
}

//******************************************************************************
/*! \fn     void CMatrix::SetTranslate( const CVector3f& v )
*   \brief  设置偏移矩阵
*   \param  const CVector3f& v 代表3个方向上的偏移
*   \return void
*******************************************************************************/
inline void CMatrix::SetTranslate( const CVector3f& v )
{
	SetTranslate( v.x, v.y, v.z );
}

//******************************************************************************
/*! \fn     void CMatrix::SetScale( float sx, float sy, float sz )
*   \brief  设置缩放矩阵
*   \param   float sx	x方向上的缩放比例
*   \param   float sy	y方向上的缩放比例
*   \param   float sz	z方向上的缩放比例
*   \return void
*******************************************************************************/
inline void CMatrix::SetScale( float sx, float sy, float sz )
{
	Identity( );
	_11 = sx;
	_22 = sy;
	_33 = sz;
}

//******************************************************************************
/*! \fn     void CMatrix::SetScale( const CVector3f& v )
*   \brief  设置缩放矩阵
*           详细描述
*   \param  const CVector3f& v 代表3个方向上的缩放比例
*   \return void
*******************************************************************************/
inline void CMatrix::SetScale( const CVector3f& v )
{
	SetScale( v.x, v.y, v.z );
}


//******************************************************************************
/*! \fn     void SetTransform(const CCoord& crd, bool IsWorldToObj = true)
*   \brief  根据一个传入的坐标系(相对于世界坐标系),构造一个转换矩阵
*
*           根据一个传入的坐标系(相对于世界坐标系),构造一个转换矩阵,第二个参数
*			为true 标明生成由世界坐标系到此坐标系的转换矩阵, 为fasle标明生成由
*			此坐标系到世界坐标系的转换矩阵
*   \param  const CCoord& crd[in]		传入的坐标系
*   \param  bool IsWorldToObj[in]		缺省为true, 标明生成由世界坐标系到此坐标
*										系的转换矩阵,为fasle标明生成由此坐标系到
*										世界坐标系的转换矩阵
*******************************************************************************/
/*
inline void CMatrix::SetTransform(const CCoord& crd, bool IsWorldToObj)
{
// Start building the matrix. The first three rows contains the basis
// vectors used to rotate the view to point at the lookat point
Identity( );
_11 = crd.GetXAxis().x;    _12 = crd.GetYAxis().x;    _13 = crd.GetZAxis().x;
_21 = crd.GetXAxis().y;    _22 = crd.GetYAxis().y;    _23 = crd.GetZAxis().y;
_31 = crd.GetXAxis().z;    _32 = crd.GetYAxis().z;    _33 = crd.GetZAxis().z;

// Do the translation values (rotations are still about the eyepoint)
_41 = - crd.GetOrg().Dot( crd.GetXAxis() );
_42 = - crd.GetOrg().Dot( crd.GetYAxis() );
_43 = - crd.GetOrg().Dot( crd.GetZAxis() );

if (!IsWorldToObj)
{
Invert();
}
}
*/

//******************************************************************************
/*! \fn     void CMatrix::SetView( const CVector3f& vX, const CVector3f& vY,
const CVector3f& vZ, const CVector3f& vOrg)
*   \brief  根据三个矢量(坐标轴)和原点(都是相对于世界坐标系)，构造转换矩阵
*
*			根据三个矢量(坐标轴)和原点(都是相对于世界坐标系)，构造一个由世界坐标
*			系到此坐标系的转换矩阵.一个常用的应用是根据视空间的三个坐标轴和原点
*			形成世界空间到此视空间的4*4变换矩阵,
*   \param   const CVector3f& vX	坐标轴的x轴
*   \param   const CVector3f& vY	坐标轴的y轴
*   \param   const CVector3f& vZ	坐标轴的z轴
*   \param   const CVector3f& vOrg 坐标轴的原点
*   \return void
*	\todo	没有错误检测，这三个轴矢量是否要单位化
*******************************************************************************/
inline void CMatrix::SetTransform( const CVector3f& vX, const CVector3f& vY,
                                   const CVector3f& vZ, const CVector3f& vOrg)
{
	// Start building the matrix. The first three rows contains the basis
	// vectors used to rotate the view to point at the lookat point
	_11 = vX.x;
	_12 = vY.x;
	_13 = vZ.x;
	_21 = vX.y;
	_22 = vY.y;
	_23 = vZ.y;
	_31 = vX.z;
	_32 = vY.z;
	_33 = vZ.z;

	// Do the translation values (rotations are still about the eyepoint)
	_41 = - vOrg.Dot( vX );
	_42 = - vOrg.Dot( vY );
	_43 = - vOrg.Dot( vZ );

	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;
}

//******************************************************************************
/*! \fn     bool CMatrix::SetView( const CVector3f& vFrom, const CVector3f& vAt,
const CVector3f& vUpAssumptive )
*   \brief  根据三个矢量和原点(坐标轴)构造视矩阵
*
*           根据给出的Eye point, LooKAt point, 和一个向上的矢量确定一个4*4的视矩阵
*			一个世界空间的矢量乘以这个矩阵就会变换到这个视空间
*   \param  const CVector3f& vFrom			Camera 的起始点
*   \param  const CVector3f& vAt			Camera 的终止点
*   \param  const CVector3f& vUpAssumptive 向上的坐标
*   \return bool true 成功， false 失败
*   \sa     参考内容
*******************************************************************************/
inline bool CMatrix::SetTransform( const CVector3f& vFrom, const CVector3f& vAt,
                                   const CVector3f& vUpAssumptive )
{
	float	fLen;

	// 得到z轴
	CVector3f vView = vAt - vFrom;
	vView.Normalize();

	// 通过点积计算Up(y)矢量到 View(z)矢量的投影,再计算正真与(View)z相垂直
	// 的Up(y)矢量.画一个图想象点积的几何意义
	float fDotProduct = vUpAssumptive.Dot(vView);
	CVector3f vUp = vUpAssumptive - vView * fDotProduct;
	fLen = vUp.Mag();

	// 如果这个Up(y)矢量太短,则使用缺省矢量尝试
	// 因为 vView, (0.0f, 1.0f, 0.0f)都是单位矢量,可以证明(0.0f, 1.0f, 0.0f)
	// 到 vView的投影就等于vView.y(即 vView到(0.0f, 1.0f, 0.0f)的投影),几何意义
	// 与上面的相同.
	if ( gIsZero(fLen) )
	{
		vUp = CVector3f( 0.0f, 1.0f, 0.0f ) - vView * vView.y;
		fLen = vUp.Mag();
		if ( gIsZero(fLen) )
		{
			vUp = CVector3f( 0.0f, 0.0f, 1.0f ) - vView * vView.z;
			fLen = vUp.Mag();
			if ( gIsZero(fLen) )
			{
				//Ast(false);
				return false;
			}
		}
	}

	// 将Up(y)矢量单位化
	vUp /= fLen;

	// 通过 vUp(Y)矢量 插积 vView(Z)矢量得到 vRight(X)矢量,(注意是Up在前)
	// 两个单位矢量的插积还是单位矢量
	CVector3f vRight = vUp.Cross( vView );

	SetTransform(vRight, vUp, vView, vFrom);

	return true;
}

//******************************************************************************
/*! \fn     void SetTransform(const CCoord& crdSrc, const CCoord& crdDes)
*   \brief  构造从一个坐标系到另一个坐标系的转换矩阵
*
*           构造从一个坐标系到另一个坐标系的转换矩阵,两个坐标系的都是相对世界
*			坐标系的,构造出的矩阵完成从Src做标系到Des坐标系的转换
*   \param  const CCoord& crdSrc 源坐标系
*   \param  const CCoord& crdDes 目标坐标系
*   \return void
*   \sa     CCoord
*******************************************************************************/
/*
void CMatrix::SetTransform(const CCoord& crdSrc, const CCoord& crdDes)
{
CVector3f xSrc,ySrc,zSrc,oSrc;
CVector3f xDes,yDes,zDes,oDes;
CMatrix mSrc,mDes;

xSrc = crdSrc.GetXAxis();
ySrc = crdSrc.GetYAxis();
zSrc = crdSrc.GetZAxis();
oSrc = crdSrc.GetOrg();
xDes = crdDes.GetXAxis();
yDes = crdDes.GetYAxis();
zDes = crdDes.GetZAxis();
oDes = crdDes.GetOrg();

mSrc.SetTransform(xSrc,ySrc,zSrc,oSrc);
mDes.SetTransform(xDes,yDes,zDes,oDes);

mSrc.Invert();
mSrc *= mDes;

*this = mSrc;
}
*/

//******************************************************************************
/*! \fn     bool CMatrix::SetProjection(float fFOV,float fAspect,
float fNearPlane,float fFarPlane)
*   \brief  设置投影矩阵
*   \param  float fFOV			= 1.570795f		Y轴上的视域 field-of-view,弧度
*   \param  float fAspect		= 1.3333f		x/y 间的比率
*   \param  float fNearPlane	= 1.0f			近平面
*   \param  float fFarPlane		= 1000.0f		远平面
*   \return bool
*******************************************************************************/
inline bool CMatrix::SetProjection(float fFOV, float fAspect, float fNearPlane,
                                   float fFarPlane)
{
	if ( gIsEqual(fNearPlane, fFarPlane, 0.01f) )
		return false;

	float fctg = tanf( fFOV/2 );
	if ( gIsZero( fctg, 0.01f ) )
		return false;

	float h = 1.0f/fctg;
	float w = h/fAspect;
	float Q = fFarPlane / ( fFarPlane - fNearPlane );

	memset( this, 0, sizeof(CMatrix) );
	_11 = w;
	_22 = h;
	_33 = Q;
	_34 = 1.0f;
	_43 = -Q*fNearPlane;

	return true;
}

//******************************************************************************
/*! \fn     void CMatrix::SetRotateX( float fRads )
*   \brief  设置绕X轴旋转fRads弧度的变换矩阵
*   \param  float fRads		旋转的弧度值
*******************************************************************************/
inline void CMatrix::SetRotateX( float fRads )
{
	Identity( );

	float c = cosf(fRads);
	float s = sinf( fRads );

	_22 =  c;
	_23 =  s;
	_32 = -s;
	_33 =  c;
}

//******************************************************************************
/*! \fn     void CMatrix::SetRotateY( float fRads )
*   \brief  设置绕Y轴旋转fRads弧度的变换矩阵
*   \param  float fRads 旋转的弧度值
*******************************************************************************/
inline void CMatrix::SetRotateY( float fRads )
{
	Identity( );

	float c = cosf(fRads);
	float s = sinf( fRads );

	_11 =  c;
	_13 = -s;
	_31 =  s;
	_33 =  c;
}

//******************************************************************************
/*! \fn     void CMatrix::SetRotateZ( float fRads )
*   \brief  设置绕Z轴旋转fRads弧度的变换矩阵
*   \param  float fRads 旋转的弧度值
*******************************************************************************/
inline void CMatrix::SetRotateZ( float fRads )
{
	Identity( );

	float c = cosf(fRads);
	float s = sinf( fRads );

	_11  =  c;
	_12  =  s;
	_21  = -s;
	_22  =  c;
}

//******************************************************************************
/*! \fn     void CMatrix::SetRotation(float fPitch, float fYaw, float fRoll)
*   \brief  构造Euler旋转矩阵
*           旋转的顺序是 x, y z,应该可以直接构造
*   \param  float fPitch	相对于x轴旋转的弧度
*   \param  float fYaw		相对于y轴旋转的弧度
*   \param  float fRoll		相对于z轴旋转的弧度
*******************************************************************************/
inline void CMatrix::SetRotation(float fPitch, float fYaw, float fRoll)
{
	CMatrix	matTemp;

	Identity();

	matTemp.SetRotateX(fPitch);
	*this *= matTemp;
	matTemp.SetRotateY(fYaw);
	*this *= matTemp;
	matTemp.SetRotateZ(fRoll);
	*this *= matTemp;

	/*
	int i;
	float sinangx,cosangx,sinangy,cosangy,sinangz,cosangz;
	float xold,yold,zold,xnew,ynew,znew;//temporary rotation results to prevent round off error
	sinangx=(float)sin(yaw);
	cosangx=(float)cos(yaw);
	sinangy=(float)sin(pitch);
	cosangy=(float)cos(pitch);
	sinangz=(float)sin(roll);
	cosangz=(float)cos(roll);
	for(i=0; i<3; i++)
	{
	//init points
	xold=m[i][0];
	yold=m[i][1];
	zold=m[i][2];
	//rotate point locally
	//xrot
	xnew=xold;
	ynew=(yold*cosangx)-(zold*sinangx);
	znew=(yold*sinangx)+(zold*cosangx);
	xold=xnew;
	yold=ynew;
	zold=znew;
	//yrot
	xnew=((xold*cosangy)+(zold*sinangy));
	ynew=yold;
	znew=((-1*(xold*sinangy))+(zold*cosangy));
	xold=xnew;
	yold=ynew;
	zold=znew;
	//zrot
	xnew=((xold*cosangz)-(yold*sinangz));
	ynew=((yold*cosangz)+(xold*sinangz));
	znew=zold;
	//change object location
	m[i][0]=xnew;
	m[i][1]=ynew;
	m[i][2]=znew;
	}
	*/
}

//******************************************************************************
/*! \fn     void CMatrix::SetRotation( const CVector3f& vDir, float fRads )
*   \brief  设置绕某个轴旋转某个弧度的变换矩阵
*   \param   const CVector3f& vDir 表示空间中的一个轴
*   \param  float fRads				旋转的弧度值(由轴的正向向原点看去，逆时针为正)
*   \return void
*******************************************************************************/
inline void CMatrix::SetRotation( const CVector3f& vDir, float fRads )
{
	float		fCos	= cosf( fRads );
	float		fSin	= sinf( fRads );
	CVector3f	v		= vDir;
	v.Normalize();

	_11 = ( v.x * v.x ) * ( 1.0f - fCos ) + fCos;
	_12 = ( v.x * v.y ) * ( 1.0f - fCos ) - (v.z * fSin);
	_13 = ( v.x * v.z ) * ( 1.0f - fCos ) + (v.y * fSin);

	_21 = ( v.y * v.x ) * ( 1.0f - fCos ) + (v.z * fSin);
	_22 = ( v.y * v.y ) * ( 1.0f - fCos ) + fCos ;
	_23 = ( v.y * v.z ) * ( 1.0f - fCos ) - (v.x * fSin);

	_31 = ( v.z * v.x ) * ( 1.0f - fCos ) - (v.y * fSin);
	_32 = ( v.z * v.y ) * ( 1.0f - fCos ) + (v.x * fSin);
	_33 = ( v.z * v.z ) * ( 1.0f - fCos ) + fCos;

	_14 = _24 = _34 = 0.0f;
	_41 = _42 = _43 = 0.0f;
	_44 = 1.0f;
}

//******************************************************************************
/*! \fn     bool CMatrix::Invert()
*   \brief  求逆矩阵
*           只有在矩阵的第四列为[0 0 0 1] 这个函数才能正常工作，自身改变
*   \return bool true 求得逆矩阵， false 求不出逆矩阵
*   \sa     参考内容
*******************************************************************************/
inline bool CMatrix::Invert()
{
	CMatrix	InMat( *this );

	//Verify the matrices with [0 0 0 1] for the 4th column.
	if ( !gIsEqual(_44, 1.0f, .001f) ||
	        !gIsEqual(_14, 0.0f, .001f) ||
	        !gIsEqual(_24, 0.0f, .001f) ||
	        !gIsEqual(_34, 0.0f, .001f) )
		return false;

	float fDetInv = 1.0f / ( InMat._11 * ( InMat._22 * InMat._33 - InMat._23 * InMat._32 ) -
	                         InMat._12 * ( InMat._21 * InMat._33 - InMat._23 * InMat._31 ) +
	                         InMat._13 * ( InMat._21 * InMat._32 - InMat._22 * InMat._31 ) );

	_11 =  fDetInv * ( InMat._22 * InMat._33 - InMat._23 * InMat._32 );
	_12 = -fDetInv * ( InMat._12 * InMat._33 - InMat._13 * InMat._32 );
	_13 =  fDetInv * ( InMat._12 * InMat._23 - InMat._13 * InMat._22 );
	_14 =  0.0f;

	_21 = -fDetInv * ( InMat._21 * InMat._33 - InMat._23 * InMat._31 );
	_22 =  fDetInv * ( InMat._11 * InMat._33 - InMat._13 * InMat._31 );
	_23 = -fDetInv * ( InMat._11 * InMat._23 - InMat._13 * InMat._21 );
	_24 =  0.0f;

	_31 =  fDetInv * ( InMat._21 * InMat._32 - InMat._22 * InMat._31 );
	_32 = -fDetInv * ( InMat._11 * InMat._32 - InMat._12 * InMat._31 );
	_33 =  fDetInv * ( InMat._11 * InMat._22 - InMat._12 * InMat._21 );
	_34 = 0.0f;

	_41 = -( InMat._41 * _11 + InMat._42 * _21 + InMat._43 * _31 );
	_42 = -( InMat._41 * _12 + InMat._42 * _22 + InMat._43 * _32 );
	_43 = -( InMat._41 * _13 + InMat._42 * _23 + InMat._43 * _33 );
	_44 = 1.0f;

	return true;
}

//******************************************************************************
/*! \fn     void CMatrix::InvertFast()
*   \brief  求逆矩阵
*           只有在矩阵的第四列为[0 0 0 1]并且三根轴缩放大小一致这个函数才能正常工作，自身改变
*   \return
*   \sa     参考内容
*******************************************************************************/
inline void CMatrix::InvertFast()
{
	/*
	// 摘自Advanced 3D Game Programming with DirectX7
	// first transpose the rotation matrix
	_11 = in._11;
	_12 = in._21;
	_13 = in._31;
	_21 = in._12;
	_22 = in._22;
	_23 = in._32;
	_31 = in._13;
	_32 = in._23;
	_33 = in._33;

	// fix right column
	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;

	// now get the new translation vector
	point3 temp = in.GetLoc();

	_41 = -(temp.x * in._11 + temp.y * in._12 + temp.z * in._13);
	_42 = -(temp.x * in._21 + temp.y * in._22 + temp.z * in._23);
	_43 = -(temp.x * in._31 + temp.y * in._32 + temp.z * in._33);
	*/

	// first transpose the rotation matrix
	CMatrix	InMat( *this );
	CVector3f v( _11, _12, _13 );
	float fScale = _11*_11 + _12*_12 + _13*_13;

	if ( gIsZero( fScale - 1.0f ) )
	{
		_11 = InMat._11;
		_12 = InMat._21;
		_13 = InMat._31;
		_21 = InMat._12;
		_22 = InMat._22;
		_23 = InMat._32;
		_31 = InMat._13;
		_32 = InMat._23;
		_33 = InMat._33;
	}
	else
	{
		fScale = 1.0f/fScale;
		_11 = InMat._11*fScale;
		_12 = InMat._21*fScale;
		_13 = InMat._31*fScale;
		_21 = InMat._12*fScale;
		_22 = InMat._22*fScale;
		_23 = InMat._32*fScale;
		_31 = InMat._13*fScale;
		_32 = InMat._23*fScale;
		_33 = InMat._33*fScale;
	}

	// fix right column
	_14 = 0;
	_24 = 0;
	_34 = 0;
	_44 = 1;

	// now get the new translation vector
	_41 = -( InMat._41 * _11 + InMat._42 * _21 + InMat._43 * _31 );
	_42 = -( InMat._41 * _12 + InMat._42 * _22 + InMat._43 * _32 );
	_43 = -( InMat._41 * _13 + InMat._42 * _23 + InMat._43 * _33 );
}

//******************************************************************************
/*! \fn     void CMatrix::Transpose()
*   \brief  求转置矩阵
*           转置矩阵既是矩阵的行列元素互换，自身改变
*   \return void
*******************************************************************************/
inline void CMatrix::Transpose()
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = i + 1; j < 4; j++)
		{
			float fTemp = m[i][j];
			m[i][j] = m[j][i];
			m[j][i] = fTemp;
		}
	}
}

inline const CVector3f CMatrix::GetLoc() const
{
	return CVector3f( _41, _42, _43 );
}

inline CVector3f CMatrix::GetAxis( int axis ) const
{
	return CVector3f
	       (
	           m[axis][0],
	           m[axis][1],
	           m[axis][2]
	       );
}
//******************************************************************************
/*! \fn     CMatrix CMatrix::operator * (const CMatrix& mat) const
*   \brief  矩阵相乘
*           重载了乘号,完成与传入的矩阵相乘的操作，自身不改变
*   \param  const CMatrix& mat 为第二(右)操作数
*   \return CMatrix CMatrix::operator 返回相乘后的结果矩阵
*******************************************************************************/
inline CMatrix CMatrix::operator * (const CMatrix& mat) const
{
	CMatrix	MatRet;

	float* pA = (float*)this;
	float* pB = (float*)&mat;
	float* pM = (float*)&MatRet;

	memset( pM, 0, sizeof(CMatrix) );

	for ( unsigned int i = 0; i < 4; i++ )
		for ( unsigned int j = 0; j < 4; j++ )
			for ( unsigned int k = 0; k < 4; k++ )
				pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

	return MatRet;
}

//******************************************************************************
/*! \fn     CMatrix& CMatrix::operator *= (const CMatrix& mat)
*   \brief  矩阵自乘
*
*           重载了*=号,完成与传入的矩阵相乘的操作，自身改变
*   \param  const CMatrix& mat 为第二(右)操作数
*   \return CMatrix& CMatrix::operator 返回的是相乘后的结果也是自身的引用
*******************************************************************************/
inline CMatrix& CMatrix::operator *= (const CMatrix& mat)
{
	CMatrix	MatRet;

	float* pA = (float*)this;
	float* pB = (float*)&mat;
	float* pM = (float*)&MatRet;

	memset( pM, 0, sizeof(CMatrix) );

	for ( unsigned int i = 0; i < 4; i++ )
		for ( unsigned int j = 0; j < 4; j++ )
			for ( unsigned int k = 0; k < 4; k++ )
				pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

	memcpy( this, pM, sizeof(CMatrix) );

	return *this;
}

inline const CVector3f CMatrix::operator*( const CVector3f& b)
{
	return CVector3f(
	           b.x*_11 + b.y*_21 + b.z*_31 + _41,
	           b.x*_12 + b.y*_22 + b.z*_32 + _42,
	           b.x*_13 + b.y*_23 + b.z*_33 + _43
	       );
};
}// End of namespace sqr
