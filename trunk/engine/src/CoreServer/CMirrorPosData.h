#pragma once
#include "CPos.h"
#include "CMirrorBufferData.h"

namespace sqr
{
	/*
		对于triple buffer来说，wb，ib和rb上面数据大小都是一样的
	*/

	class CMirrorPosData : public CMirrorBufferData 
	{
	public:
		const CFPos& GetPos() const { return m_Pos; }
		CFPos& GetPos() { return m_Pos; }

	protected:
		CMirrorPosData(const CFPos& pos);
		~CMirrorPosData();		

		void RawSetPos(const CFPos& pos) { m_Pos = pos; }

	protected:
		CFPos	m_Pos;
	};
		
}
