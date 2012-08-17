#pragma once
#include "CXmlConfig.h"
#include "CCameraController.h"
#include "CRenderScene.h"
#include "TSingleton.h"
namespace sqr
{
	class CCameraConfig 
		: public CSceneMallocObject
		, public Singleton<CCameraConfig>
	{
	private:
		float			m_fCamDefDist;
		float			m_fCamMinSpeed;
		ECamMode		m_DefaultMode;
		CXmlConfig*		m_pConfig;

	public:
		CCameraConfig(istream& strmConfig);
		~CCameraConfig(void);

		void	SetCameraInfo(CRenderScene* scene, ECamMode c);

		float	GetCamMaxXRadian(ECamMode e)const;
		float	GetCamMinXRadian(ECamMode e)const;

		float	GetCamYRadian(ECamMode e)const;

		float	GetCamMaxFovRadian(ECamMode e)const;
		float	GetCamMinFovRadian(ECamMode e)const;

		float	GetCamMaxNearPlan(ECamMode e)const;
		float	GetCamMinNearPlan(ECamMode e)const;

		float	GetCamMinYOffset(ECamMode e)const;
		float	GetCamMaxYOffset(ECamMode e)const;

		float	GetCamMinDist(ECamMode e)const;
		float	GetCamMaxDist(ECamMode e)const;

		float	GetCamMaxHOffset(ECamMode e)const;
		float	GetCamSpeed(ECamMode e) const;

		float	GetCamDefDist(ECamMode e)const;
		float	GetCamMinSpeed(void)	const;

	private:
		template<typename ValueType>
		ValueType Get(ValueType DefaultValue,const char* szElemName)const;
		template<typename ValueType>
		ValueType Get(ValueType DefaultValue,const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType Get(ValueType DefaultValue,const char* szElemName1,const char* szElemName2,const char* szElemName3)const;

		template<typename ValueType>
		ValueType Get(const char* szElemName)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2)const;
		template<typename ValueType>
		ValueType Get(const char* szElemName1,const char* szElemName2,const char* szElemName3)const;
	};

	inline float	CCameraConfig::GetCamMinSpeed(void)		const	{ return m_fCamMinSpeed;}
}