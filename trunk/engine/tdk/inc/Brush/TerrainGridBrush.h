#pragma once
#include "BaseBrush.h"

namespace sqr_tools
{
	class CCmdSetVertexHeight;
	class CTerrainOperate;
	class CTerrainGridBrush : public CBaseBrush
	{
	public:

		CTerrainGridBrush(void);
		~CTerrainGridBrush(void);

		void SetTerrainOpType(ETERRAIN_OP_TYPE _type);
		void SetTemplateType(const int nType);
		void SetHeightStrength(const float f);
		float GetHeightStrength() const;

		virtual void MoveLButtonDownEvent(MSG_MOUSE_INF mouse);
		virtual bool KeyDownEvent(MSG_KEY_INF key);
		virtual void KeyUpEvent(MSG_KEY_INF key);

	private:
		ETERRAIN_OP_TYPE		m_eOpType;
		EHEIGHT_TEMPLATE_TYPE	m_eTemplateType;
		float					m_fHeightStrength;
		CTerrainOperate*		m_TerrainOp;
	};

//!namespace
}
