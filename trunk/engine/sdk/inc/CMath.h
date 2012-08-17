#ifndef __GMath_H__
#define __GMath_H__

//#include "GMacro.h"
//#include "GCommon.h"
#include <math.h>
#include "TSingleton.h"
#include "CVector3.h"

/*----------------------------------------
使用Single Instruction Multiple Data
使用Intel的汇编指令集以优化
需要运行于Pentium 3或更好的CPU
如果需要 可取消针对SIMD的优化 取消以下定义
不定义SIMD 则只能使用速度较慢的单数据流浮点运算
----------------------------------------*/
#if defined(_WIN32) || defined(__WIN32__)
#define SIMD
//定义SIMD对齐方式
#define _SIMD_ALIGNMENT __declspec(align(16)) 
#define _MATH_INLINE_ __forceinline
#else
#define _MATH_INLINE_ inline
#endif


//定义弧度
#define TWO_PI	6.2831853071795864769252867665590
#define PI		3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916397
#define QTR_PI	0.7853981633974483096156608458198
#define SQRT_2  1.4142135623730950488016887242096
#define SQRT_3	1.7320508075688772935274463415058

namespace sqr
{

class CMath : public Singleton<CMath>
{
public:

	CMath();
	~CMath();

	//在2*PI范围内创建SinTable和TanTable,用于快速查询三角运算值
	//uint32 trigTableSize用于设定创建表的大小，数值越大则，表容量越大，数值越精确
	//bool mp用于设定是否要提升精度
	//如果设置成true，则只有trigTableSize高于当前容量时才更新三角函数表
	//如果设置成false，则无论trigTableSize多大都会重新更新三角函数表
	static void buildTrigTables(uint32 trigTableSize = 4096, bool mp = false);

	static float SinTable( float fRadian );
	static float CosTable( float fRadian );
	static float TanTable( float fRadian );

	static float Sin( float fRadian);
	static float Cos( float fRadian );
	static float Tan( float fRadian);

	static float ACos( float fValue );
	static float ASin( float fValue );
	static inline float ATan2(float fY, float fX) { return float(atan2(fY,fX)); }

	static float ToRadian( float fDegree );
	static float ToDegree( float fRadin );

	//数学函数
	static float Abs( float fValue );
	static float Sqr( float fValue );
	static float Sqrt( float fValue );
	static float InvSqrt( float fValue );

	static double Logbase( double a, double base );

	// 新增函数
	static bool IntersectTri( const CVector3f& v0, const CVector3f& v1, const CVector3f& v2, const CVector3f& vRayOrg, const CVector3f& vRayDir, float& u, float& v, float& d );

protected:
	static uint32 m_stTrigTableSize;
	static float m_stTrigTableFactor;

	static float* m_apSinTable;
	static float* m_apTanTable;

public:
	static const float pi;
	static const float half_pi;
	static const float two_pi;
	static const float qtr_pi;
public:
	static const float sqrt_2;
	static const float sqrt_3;
};

}
#endif //__GMath_H__
