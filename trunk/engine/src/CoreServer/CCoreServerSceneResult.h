#pragma once

#include "CCoreServerResult.h"

namespace sqr
{
	
	class CCoreEndDestroySceneResult
		:public CCoreServerResult
	{
	public:
		CCoreEndDestroySceneResult(uint32 uSceneID);
		~CCoreEndDestroySceneResult();

	private:
		uint32			m_uSceneID;
	};

	class CCoreEndCreateSyncCoreSceneResult
		:public CCoreServerResult
	{
	public:
		CCoreEndCreateSyncCoreSceneResult(uint32 uSceneID);
		~CCoreEndCreateSyncCoreSceneResult();

	private:
		uint32				m_uSceneID;
	};

}
