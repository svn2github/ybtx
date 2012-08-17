#pragma once
#include "BaseBrush.h"

namespace sqr_tools
{
	class CTerrainBlockBrush : public CBaseBrush
	{
	public:

		CTerrainBlockBrush(void);
		~CTerrainBlockBrush(void);

		void SetBlockType(int nBlockType)
		{			
			m_eBlockTypeHot = EBlockType(nBlockType);
		}
		int  GetBlockType() const
		{
			return m_eBlockTypeHot;
		}

		void SetEditType(int nEditType)
		{
			m_eBlockEditType = EBlockEditType(nEditType);
		}
		int  GetEditType() const
		{
			return m_eBlockEditType;
		}

		void SetEditBrushType(int nEditBrushType)
		{
			m_eEditBlockBrushType = EEditBrushStyle(nEditBrushType);
		}
		int  GetEditBrushType() const
		{
			return m_eEditBlockBrushType;
		}

		virtual bool KeyDownEvent(MSG_KEY_INF key);
		virtual void KeyUpEvent(MSG_KEY_INF key);

		void SetTerrainBlock(const EBlockType blocktype);

	private:
		EBlockType		m_eBlockTypeHot;
		EBlockEditType	m_eBlockEditType;
		EEditBrushStyle m_eEditBlockBrushType;
	};
}
