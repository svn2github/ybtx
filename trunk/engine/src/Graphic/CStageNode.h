#pragma once
#include "GraphicBase.h"
#include "CBufFile.h"

namespace sqr
{
	enum StageFlag
	{
		SF_Stage1		= 0,
		SF_Stage2		= 1,
		SF_Stage3		= 2,
		SF_Stage4		= 3,
		SF_Stage5		= 4,
		SF_Stage6		= 5,
		SF_Stage7		= 6,
		SF_Stage8		= 7,
		SF_Shader	  = 0xFF,
		SF_FORC_DWORD = 0xFFFFFFFF,
	};

	class CStageNode 
		: public CGraphicMallocObject
	{
	public: //实现部分
		CStageNode(StageFlag sf);
		virtual ~CStageNode();
	
	public:
		void BinLoad(void){};
		void BinSave(void){};
	
	protected:
		StageFlag	m_eStage;
		
	};
}