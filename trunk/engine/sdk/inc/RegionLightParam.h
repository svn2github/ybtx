#pragma once
//------------------------------------------------------------------------------
/**
    @struct RegionLightParam

	区域光参.
	把各个参数封装到一起进行统一计算, 减少重复的代码.

    (C) 2009 ThreeOGCMan
*/
#include "CColor.h"
#include "CSceneMallocObject.h"

//------------------------------------------------------------------------------
namespace sqr
{

class RegionLightParam
	: public CSceneMallocObject
{
public:
	RegionLightParam();

	const RegionLightParam operator+(const RegionLightParam& rhs) const;
	const RegionLightParam operator-(const RegionLightParam& rhs) const;
	const RegionLightParam operator*(const RegionLightParam& rhs) const;
	const RegionLightParam operator*(float ratio) const;
	RegionLightParam& operator+=(const RegionLightParam& rhs);
	RegionLightParam& operator-=(const RegionLightParam& rhs);
	RegionLightParam& operator*=(const RegionLightParam& rhs);
	RegionLightParam& operator*=(float ratio);

public:
	static const RegionLightParam Max(const RegionLightParam& param1, const RegionLightParam& param2);
	static const RegionLightParam Min(const RegionLightParam& param1, const RegionLightParam& param2);

public:
	CColor globalAmbient;
	CColor globalDiffuse;
	CColor globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	CColor globalShadow;
#endif

	CColor playerAmbient;
	CColor playerDiffuse;
	CColor playerSpecular;
	CColor fogColor;
	float  fogStart;
	float  fogEnd;
	float  fogDensity;
	float  specularPower;///材质高光反射强度
};

//------------------------------------------------------------------------------
inline
RegionLightParam::RegionLightParam()
{
	ZeroMemory(this, sizeof(RegionLightParam));
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::Max( const RegionLightParam& param1, const RegionLightParam& param2 )
{
	RegionLightParam newParam;
	newParam.globalAmbient	= CColor::Max(param1.globalAmbient, param2.globalAmbient);
	newParam.globalDiffuse	= CColor::Max(param1.globalDiffuse, param2.globalDiffuse);
	newParam.globalSpecular = CColor::Max(param1.globalSpecular, param2.globalSpecular);

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= CColor::Max(param1.globalShadow, param2.globalShadow);
#endif

	newParam.playerAmbient	= CColor::Max(param1.playerAmbient, param2.playerAmbient);
	newParam.playerDiffuse	= CColor::Max(param1.playerDiffuse, param2.playerDiffuse);
	newParam.playerSpecular = CColor::Max(param1.playerSpecular, param2.playerSpecular);
	newParam.fogColor		= CColor::Max(param1.fogColor, param2.fogColor);
	newParam.fogStart		= max(param1.fogStart, param2.fogStart);
	newParam.fogEnd			= max(param1.fogEnd, param2.fogEnd);
	newParam.fogDensity		= max(param1.fogDensity, param2.fogDensity);
	newParam.specularPower	= max(param1.specularPower, param2.specularPower);
	return newParam;
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::Min( const RegionLightParam& param1, const RegionLightParam& param2 )
{
	RegionLightParam newParam;
	newParam.globalAmbient	= CColor::Min(param1.globalAmbient, param2.globalAmbient);
	newParam.globalDiffuse	= CColor::Min(param1.globalDiffuse, param2.globalDiffuse);
	newParam.globalSpecular = CColor::Min(param1.globalSpecular, param2.globalSpecular);

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= CColor::Min(param1.globalShadow, param2.globalShadow);
#endif

	newParam.playerAmbient	= CColor::Min(param1.playerAmbient, param2.playerAmbient);
	newParam.playerDiffuse	= CColor::Min(param1.playerDiffuse, param2.playerDiffuse);
	newParam.playerSpecular = CColor::Min(param1.playerSpecular, param2.playerSpecular);
	newParam.fogColor		= CColor::Min(param1.fogColor, param2.fogColor);
	newParam.fogStart		= min(param1.fogStart, param2.fogStart);
	newParam.fogEnd			= min(param1.fogEnd, param2.fogEnd);
	newParam.fogDensity		= min(param1.fogDensity, param2.fogDensity);
	newParam.specularPower	= min(param1.specularPower, param2.specularPower);
	return newParam;
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::operator+( const RegionLightParam& rhs ) const
{
	RegionLightParam newParam;
	newParam.globalAmbient	= this->globalAmbient + rhs.globalAmbient;

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= this->globalShadow + rhs.globalShadow;
#endif

	newParam.globalDiffuse	= this->globalDiffuse + rhs.globalDiffuse;
	newParam.globalSpecular = this->globalSpecular + rhs.globalSpecular;
	newParam.playerAmbient	= this->playerAmbient + rhs.playerAmbient;
	newParam.playerDiffuse	= this->playerDiffuse + rhs.playerDiffuse;
	newParam.playerSpecular = this->playerSpecular + rhs.playerSpecular;
	newParam.fogColor		= this->fogColor + rhs.fogColor;
	newParam.fogStart		= this->fogStart + rhs.fogStart;
	newParam.fogEnd			= this->fogEnd + rhs.fogEnd;
	newParam.fogDensity		= this->fogDensity + rhs.fogDensity;
	newParam.specularPower	= this->specularPower + rhs.specularPower;
	return newParam;
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::operator-( const RegionLightParam& rhs ) const
{
	RegionLightParam newParam;
	newParam.globalAmbient	= this->globalAmbient - rhs.globalAmbient;
	newParam.globalDiffuse	= this->globalDiffuse - rhs.globalDiffuse;
	newParam.globalSpecular = this->globalSpecular - rhs.globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= this->globalShadow - rhs.globalShadow;
#endif

	newParam.playerAmbient	= this->playerAmbient - rhs.playerAmbient;
	newParam.playerDiffuse	= this->playerDiffuse - rhs.playerDiffuse;
	newParam.playerSpecular = this->playerSpecular - rhs.playerSpecular;
	newParam.fogColor		= this->fogColor - rhs.fogColor;
	newParam.fogStart		= this->fogStart - rhs.fogStart;
	newParam.fogEnd			= this->fogEnd - rhs.fogEnd;
	newParam.fogDensity		= this->fogDensity - rhs.fogDensity;
	newParam.specularPower	= this->specularPower - rhs.specularPower;
	return newParam;
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::operator*( const RegionLightParam& rhs ) const
{
	RegionLightParam newParam;
	newParam.globalAmbient	= this->globalAmbient * rhs.globalAmbient;
	newParam.globalDiffuse	= this->globalDiffuse * rhs.globalDiffuse;
	newParam.globalSpecular = this->globalSpecular * rhs.globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= this->globalShadow * rhs.globalShadow;
#endif

	newParam.playerAmbient	= this->playerAmbient * rhs.playerAmbient;
	newParam.playerDiffuse	= this->playerDiffuse * rhs.playerDiffuse;
	newParam.playerSpecular = this->playerSpecular * rhs.playerSpecular;
	newParam.fogColor		= this->fogColor * rhs.fogColor;
	newParam.fogStart		= this->fogStart * rhs.fogStart;
	newParam.fogEnd			= this->fogEnd * rhs.fogEnd;
	newParam.fogDensity		= this->fogDensity * rhs.fogDensity;
	newParam.specularPower	= this->specularPower * rhs.specularPower;
	return newParam;
}

//------------------------------------------------------------------------------
inline const RegionLightParam
RegionLightParam::operator*( float ratio ) const
{
	RegionLightParam newParam;
	newParam.globalAmbient	= this->globalAmbient * ratio;
	newParam.globalDiffuse	= this->globalDiffuse * ratio;
	newParam.globalSpecular = this->globalSpecular * ratio;

#ifndef AMBIENT_SHADOW_COLOR
	newParam.globalShadow	= this->globalShadow * ratio;
#endif

	newParam.playerAmbient	= this->playerAmbient * ratio;
	newParam.playerDiffuse	= this->playerDiffuse * ratio;
	newParam.playerSpecular = this->playerSpecular * ratio;
	newParam.fogColor		= this->fogColor * ratio;
	newParam.fogStart		= this->fogStart * ratio;
	newParam.fogEnd			= this->fogEnd * ratio;
	newParam.fogDensity		= this->fogDensity * ratio;
	newParam.specularPower	= this->specularPower * ratio;
	return newParam;
}

//------------------------------------------------------------------------------
inline RegionLightParam&
RegionLightParam::operator+=( const RegionLightParam& rhs )
{
	this->globalAmbient	 += rhs.globalAmbient;
	this->globalDiffuse	 += rhs.globalDiffuse;
	this->globalSpecular += rhs.globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	this->globalShadow	 += rhs.globalShadow;
#endif

	this->playerAmbient	 += rhs.playerAmbient;
	this->playerDiffuse	 += rhs.playerDiffuse;
	this->playerSpecular += rhs.playerSpecular;
	this->fogColor		 += rhs.fogColor;
	this->fogStart		 += rhs.fogStart;
	this->fogEnd		 += rhs.fogEnd;
	this->fogDensity	 += rhs.fogDensity;
	this->specularPower	 += rhs.specularPower;
	return *this;
}

//------------------------------------------------------------------------------
inline RegionLightParam&
RegionLightParam::operator-=( const RegionLightParam& rhs )
{
	this->globalAmbient  -= rhs.globalAmbient;
	this->globalDiffuse  -= rhs.globalDiffuse;
	this->globalSpecular -= rhs.globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	this->globalShadow   -= rhs.globalShadow;
#endif

	this->playerAmbient  -= rhs.playerAmbient;
	this->playerDiffuse  -= rhs.playerDiffuse;
	this->globalSpecular -= rhs.globalSpecular;
	this->fogColor		 -= rhs.fogColor;
	this->fogStart		 -= rhs.fogStart;
	this->fogEnd		 -= rhs.fogEnd;
	this->fogDensity	 -= rhs.fogDensity;
	this->specularPower  -= rhs.specularPower;
	return *this;
}

//------------------------------------------------------------------------------
inline RegionLightParam&
RegionLightParam::operator*=( const RegionLightParam& rhs )
{
	this->globalAmbient  *= rhs.globalAmbient;
	this->globalDiffuse  *= rhs.globalDiffuse;
	this->globalSpecular *= rhs.globalSpecular;

#ifndef AMBIENT_SHADOW_COLOR
	this->globalShadow   *= rhs.globalShadow;
#endif

	this->playerAmbient	 *= rhs.playerAmbient;
	this->playerDiffuse  *= rhs.playerDiffuse;
	this->playerSpecular *= rhs.playerSpecular;
	this->fogColor		 *= rhs.fogColor;
	this->fogStart		 *= rhs.fogStart;
	this->fogEnd		 *= rhs.fogEnd;
	this->fogDensity	 *= rhs.fogDensity;
	this->specularPower	 *= rhs.specularPower;
	return *this;
}

//------------------------------------------------------------------------------
inline RegionLightParam&
RegionLightParam::operator*=( float ratio )
{
	this->globalAmbient  *= ratio;
	this->globalDiffuse	 *= ratio;
	this->globalSpecular *= ratio;

#ifndef AMBIENT_SHADOW_COLOR
	this->globalShadow	 *= ratio;
#endif

	this->playerAmbient	 *= ratio;
	this->playerDiffuse  *= ratio;
	this->playerSpecular *= ratio;
	this->fogColor       *= ratio;
	this->fogStart		 *= ratio;
	this->fogEnd	     *= ratio;
	this->fogDensity	 *= ratio;
	this->specularPower  *= ratio;
	return *this;
}

}// namespace sqr
