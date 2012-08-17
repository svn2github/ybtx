#pragma once
#include "GraphicBase.h"

namespace sqr
{
class CRenderTarget;
class CBloomTarget : public CGraphicMallocObject
{
public:
	CBloomTarget(CRenderTarget* pRenderTarget);
	~CBloomTarget();

public:
	bool Create(void);
	bool Release(void);
	bool IsUseful();
	bool IsTurnOn(const string& effect);
	void SetTurnOn(const string& effect, bool b);

	bool DoPostProcess(void);

	//这个类的使用需以下几个参数设定，应交给美工处理比较好
	//关于名字 我理解的可能不是很透彻
	void setBrightPass(float bright);			//光通量 0~1
	void setGaussMultiplier(float multiplier);	//泛光强度 0~2
	void setMean(float mean);					//泛光偏移 -1~+1
	void setStdDev( float sd );					//泛光融和程度  0~1
	void setExposure( float Exp );				//最终亮度0~2
	void setSample(float Sam );					//模糊图比例
	//void setLuminance( D3DXVECTOR4& l );		//混合色彩

	float getBrightPass(void);			//光通量 0~1
	float getGaussMultiplier(void);		//泛光强度 0~2
	float getMean(void);				//泛光偏移 -1~+1
	float getStdDev(void);				//泛光融和程度  0~1
	float getExposure(void);			//最终亮度0~2
	float getSample();					//模糊图比例

	void openHDR(bool bHDR);
	void InitParams(void); //重新计算泛光变量 在各种值设定结束后 需要重新计算...
	void UpdateParmas();
private:
	bool				m_IsInit;

	//-----计算中间量--------------------------
	float		m_HBloomWeights[9];
	float		m_HBloomOffsets[9];
	float		m_VBloomWeights[9];
	float		m_VBloomOffsets[9];
	CRenderTarget*	m_pRenderTarget;
	//D3DXVECTOR4 m_vDsOffsets[16];

protected:
	//这个类的使用需以下几个参数设定，应交给美工处理比较好
	//关于名字 我理解的可能不是很透彻
	float		m_fBrightPass; //光通量 0~1
	float		m_fGaussMultiplier; //泛光强度 0~2
	float		m_fGaussMean; //泛光偏移 -1~+1
	float		m_fGaussStdDev; //泛光融和程度  0~1
	float		m_fExposure;	//最终亮度0~2
	float		m_fSample;			//混合度

	float ComputeGaussianValue( float x, float mean, float std_deviation );
};

//------------------------------------------------------------------------------
inline void
CBloomTarget::UpdateParmas()
{
	m_IsInit = false;
}
//------------------------------------------------------------------------------
// Gassian公式如下^^
/*
-(x - mean)^2
-------------
1.0               2*std_dev^2
f(x,mean,std_dev) = -------------------- * e^
sqrt(2*pi*std_dev^2)

*/
inline float
CBloomTarget::ComputeGaussianValue( float x, float mean, float std_deviation )
{
	return ( 1.0f / sqrt( 2.0f * CMath::pi * std_deviation * std_deviation ) )
	       * expf( (-((x-mean)*(x-mean)))/(2.0f * std_deviation * std_deviation) );
}

//----一些没有必要看的函数--------------
inline void CBloomTarget::setBrightPass(float bright)
{
	m_fBrightPass = bright;
	m_IsInit = false;
}

inline void CBloomTarget::setGaussMultiplier(float multiplier)
{
	m_fGaussMultiplier = multiplier;
	m_IsInit = false;
}

inline void CBloomTarget::setMean(float mean)
{
	m_fGaussMean =mean;
	m_IsInit = false;
}

inline void CBloomTarget::setStdDev( float sd )
{
	m_fGaussStdDev =sd;
	m_IsInit = false;
}

inline void CBloomTarget::setExposure( float Exp )
{
	m_fExposure =Exp;
	m_IsInit = false;
}

inline float CBloomTarget::getBrightPass(void)
{
	return m_fBrightPass;
}

inline float CBloomTarget::getGaussMultiplier(void)
{
	return m_fGaussMultiplier;
}

inline float CBloomTarget::getMean(void)
{
	return m_fGaussMean;
}

inline float CBloomTarget::getStdDev(void)
{
	return m_fGaussStdDev;
}

inline float CBloomTarget::getExposure(void)
{
	return m_fExposure;
}

inline void CBloomTarget::setSample(float Sam )
{
	m_fSample = Sam;
	m_IsInit = false;
}

inline float CBloomTarget::getSample()
{
	return m_fSample;
}

inline void CBloomTarget::openHDR(bool bHDR)
{
}

}