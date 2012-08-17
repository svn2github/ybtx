#pragma once 
#include "CPos.h"
#include "CBaseAoiObj.h"
#include "CQuadMallocObject.h"
#include "TQuadAllocator.h"

namespace sqr
{

	class CAsyAoiViewObj;
	class CAsyAoiScene;
	class CAsyAoiDimScene;

	class CAsyAoiDimObj : public CBaseAoiObj
						, public CQuadMallocObject
	{
	public:
		CAsyAoiDimObj(CAsyAoiDimScene* pScene, CAsyAoiViewObj* pViewObj, const CFPos& pos);
		~CAsyAoiDimObj();
	
		void Release();

		CAsyAoiDimScene* GetScene() const
		{
			return m_pScene;
		}

		void SetSize( float fSize );
		void SetEyeSight( float fEyeSight );
		void SetStealth( float fStealth );
		void SetKeenness(float Keenness);
		void SetViewGroup(uint32 uGroupID);
		float GetSize() const; 
		float GetEyeSight() const; 
		float GetStealth() const; 
		float GetKeenness() const; 
		uint32 GetViewGroup() const; 

	protected:
		CAsyAoiDimScene*	m_pScene;
		CAsyAoiViewObj*	m_pViewObj;

		//由于每一个aoi object对应一个对象，所以对象的一些属性我们这里需要用到以作aoi 处理 
		float			m_fSize;		//体积
		float			m_fEyeSight;	//视力(视野范围)
		float			m_fStealth;		//隐形度
		float			m_fKeenness;	//敏锐度
		uint32			m_uViewGroup;	//观察组	观察组相同的两个Object忽略Stealth和Keenness属性
	};
}
