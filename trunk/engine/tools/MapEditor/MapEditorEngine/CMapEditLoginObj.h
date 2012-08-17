#pragma once
#include "scene.h"
#include "CMapEditObject.h"

namespace sqr
{
	typedef map<string, vector<int16>>  AniFrameMap;   //一个名字对应多个视角帧
	typedef map<int, AniFrameMap>		CamPathAniMap; ///first 第几个camerapath

	class CMapEditLoginObj : public CMapEditObject
	{
		map<int, map<int16, string>> m_FrameAniLoginList; //first camerpath
		vector<string>				 m_AniNameList;
		string						 m_strLogicName;     //用于逻辑的名字
		void ChangeAnimation();
		bool						 m_bPlayAgainFromStart;
	public:

		CMapEditLoginObj();

		virtual ~CMapEditLoginObj();
		virtual void Render();

		virtual void SetAnimationFrame(const string& str, const int8 nFrame);
		virtual int16 GetAnimationFrame(const string& str);
		virtual vector<int16> GetAnimationFrames(const string& str);
		virtual void SetNextAnimation( const TCHAR* szAniFileName, bool bLoop, int32 DelayTime, float AniSpeed );
		virtual void SetLoginAniByAgpFileName(string strAgpFileName);
		virtual vector<string> GetAniNameList()  const
		{
			return m_AniNameList;
		}

		CamPathAniMap						m_AniLoginFrameList;
		virtual void CleanAniFrames(const string& str);
		virtual void DeleteAniFrame(const string& str, const int nCurrKey);
		virtual void SetAniFrames(CamPathAniMap aniFrames);
		virtual void   SetLoginModelLogicName(const string& name)
		{
			m_strLogicName = name;
		}
		virtual string GetLoginModelLogicName() const
		{
			return m_strLogicName;
		}

		void   SetLoginModelPalyAniFromStart(const bool b)
		{
			m_bPlayAgainFromStart = b;
		}
	};
}
