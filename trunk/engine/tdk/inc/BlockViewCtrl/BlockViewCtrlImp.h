#pragma once

#include "CCtrlBase.h"

namespace sqr_tools
{
	class CTerrainBlockBrush;

	class CBlockViewCtrlImp :public CCtrlBase
	{
		REG_UI_IMP(CBlockViewCtrlImp);

	private:
		CTerrainBlockBrush*	m_pTerrainBlockBrush;

	public:
		CBlockViewCtrlImp(void* param);
		~CBlockViewCtrlImp(void);

	public:
		virtual void InitBlockBrush();

 		virtual void Update();
 		virtual bool _ProcMsg(const CMsg& msg );
		//жиди
		virtual void SetContext(CEditContext* pContext);

		virtual void SetBlockType(int nBlockType);
		virtual int  GetBlockType() const;

		virtual void SetEditType(int nEditType);
		virtual int  GetEditType() const;

		virtual void SetEditBrushType(int nEditBrushType);
		virtual int  GetEditBrushType() const;

		virtual void ActiveBrush();
	};

}
