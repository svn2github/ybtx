#pragma once
#include "CCtrlBaseObserver.h"

namespace sqr_tools {

	class  CEditResSet;
	class  CResSetUnit;
	class  C3DResSetUnit;
	class  CEdit2DResSetUnit;
	struct CEditResSetGroup;
	class  CTerrainTextureBrush;

	class CTextureViewCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CTextureViewCtrlImp);

	private:
		typedef map<string, CEditResSet* > ResSetPool;
		ResSetPool			m_resSets;

		CEditResSet				*m_pActiveResSet;
		CEditResSetGroup		*m_pActiveGroup;
		CEdit2DResSetUnit		*m_pActive2DUnit;
		CTerrainTextureBrush	*m_pTerrainTextureBrush;

	private:
		virtual void Init();
		virtual void UpdateTextureViewTree();

	public:
		CTextureViewCtrlImp(void* param);
		virtual ~CTextureViewCtrlImp(void);

		///Í¼ËØ°ü
		virtual void SelectCurReset(const string& resetName);

		///Í¼ËØ¸öÌå
		virtual void SelectActiveUnit(const string& resetName, const string& strGroupName, const string& strUnitName, const bool bSelectUnit);
		virtual void MakePathToText(const string& Src, string& Dst);

		virtual bool _Update(void);

		virtual int	 GetReset2DGroupCount();
		virtual void GetReset2DGroupNameByIndex( const size_t index, string&name );
		virtual int	 GetReset2DGroupUnitCount( const size_t index );
		virtual void GetReset2DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name );

		virtual void GetResSetName( string& name );

		virtual void SetHotLayer(const int nHotLayer);

		virtual void SetContext(CEditContext* pContext);
		virtual void ActiveBrush();
		virtual void SetDirType(const int nType);
		virtual void SetWayType(const int nType);
		virtual void SetAlphaStrength(const float f);
	};

}
