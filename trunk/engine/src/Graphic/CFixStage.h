#pragma once
#include "CStageNode.h"
namespace sqr
{
	class CRenderStyleExInst;

	class CFixStage : public CStageNode
	{
	protected:
		enum StageFlagEx
		{
			SFE_TEXTURE		= 0x00,
			SFE_COLOR		= 0x10,
			SFE_ALPHA		= 0x20,
			SFE_FULLCOLOR	= 0x30,
			SFE_SAMPLER		= 0x40,
			SFE_UV			= 0x80,
			SFE_SHADER		= SF_Shader, //无法在固定管线下使用
			SFE_FOURCEDWORD = 0xffffffff
		};

	public:
		CFixStage(StageFlag sf);

		DWORD	GetFlag(void);
		void	SetStage(StageFlag sf);
		virtual void	SetStageEx(StageFlagEx sfe);
		virtual void	Execute( CRenderStyleExInst* pRSInst ) = 0;
		virtual void	DefaultSetting(void){};

	protected:
		StageFlagEx	m_eStageEx;
	};

	inline DWORD
		CFixStage::GetFlag(void)
	{
		return (DWORD)(m_eStage | m_eStageEx);
	}

	inline void 
		CFixStage::SetStage(StageFlag sf)
	{
		m_eStage = sf;
	}
}