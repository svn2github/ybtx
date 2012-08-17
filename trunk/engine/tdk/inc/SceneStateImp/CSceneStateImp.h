#pragma once

#include "CCtrlBase.h"
#include <string>
#include <vector>
#include <list>
//use STL

#define MGT_SCENECOUNT_ENABLE	0x01
#define MGT_SCENECOUNT_DISABLE	0x02
#define	MGT_SCENESTATE_ENABLE	0x03
#define	MGT_SCENESTATE_DISABLE	0x04
#define	MGT_SCENESTATE_ALPHA	0x05
#define	MGT_SCENESTATE_FULL		0x06

namespace sqr_tools
{

	enum COLORREGION
	{
		STATE_BLACK = 0,
		STATE_BLUE,
		STATE_GREEN,
		STATE_YELLOW,
		STATE_RED,
		STATE_WHITE,
		STATE_REGIONNUM
	};

	class CSceneStateImp :public CCtrlBase
	{
	public:
		CSceneStateImp(void);
		~CSceneStateImp(void);
	private:

		struct StateRegion
		{
		public:
			StateRegion()
			{  max=min=0;  } 
			StateRegion(int _min,int _max)
			{  min=_min;  max=_max;  };
			int min;
			int max;
		};

		StateRegion ColorRegion[STATE_REGIONNUM];

		bool EffectSwitch;   //功能开关
		bool AlphaSwitch;    //Alpha开关

		unsigned int pnum;
		unsigned int fnum;
		unsigned int texnum;
		unsigned int dpnum;

	public:
		void Init();

		virtual bool IsEffectEnable();
		virtual bool IsAlphaEnable();

		virtual void EnableEffect(bool enable);
		virtual void EnableAlpha(bool enable);	
		
		virtual void GetColorRegion(COLORREGION colorflag,int& min,int& max);
		virtual std::string GetColorRegionDescription(COLORREGION colorflag);

		virtual unsigned int GetPNum();
		virtual unsigned int GetFNum();
		virtual unsigned int GetTexNum();
		virtual unsigned int GetDPNum();

		virtual void GetPNumString(std::string& str);
		virtual	void GetFNumString(std::string& str);
		virtual void GetTexNumString(std::string& str);
		virtual void GetDPNumString(std::string& str);

		virtual void Update();
		virtual bool _ProcMsg(const CMsg& msg );

	};

}
