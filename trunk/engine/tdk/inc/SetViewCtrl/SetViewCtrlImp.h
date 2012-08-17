#pragma once
#include "CCtrlBaseObserver.h"

namespace sqr_tools {

	class CEditResSet;
	class CResSetUnit;
	class CEdit3DResSetUnit;
	class CTerrainModelBrush;
	struct CEditModelResSetGroup;
	struct CEditResSetEditModelUnit;

	class CSetViewCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CSetViewCtrlImp);

	private:
		typedef map<string, CEditResSet* > ResSetPool;
		ResSetPool				m_resSets;

		CEditResSet				*m_pActiveResSet;
		CEditModelResSetGroup	*m_pActiveModelGroup;
		CEditResSetEditModelUnit*m_pActiveEditModelUnit;
		CEditObserver*			m_pEditResSetObserver;	
		string					m_strResetFilePath;
		CTerrainModelBrush*		m_pTerrainModelBrush;

	private:
		virtual void Init();
		virtual void UpdateSetViewTree();

	public:
		CSetViewCtrlImp(void* param);
		virtual ~CSetViewCtrlImp(void);

		virtual void SetTextureViewCtrlObserver(CEditObserver* pObserver);

		virtual bool ResetIsExit( const string& resetName );
		virtual void LoadReset(const string& resetName);


		///图素包
		virtual void SelectCurReset(const string& resetName);
		virtual bool DeleteCurReset(const string& resetName);
		virtual void ImportReset(const vector<string> resetNames);

		///图素个体
		virtual void SelectActiveUnit(const string& resetName, const string& strGroupName, const string& strUnitName, const bool bSelectUnit);

		virtual void MakePathToText(const string& Src, string& Dst);

		virtual bool _Update(void);
		
		virtual void UpdateTextureViewCtrl();

		virtual int	 GetReset3DGroupCount();
		virtual void GetReset3DGroupNameByIndex( const size_t index, string&name );
		virtual int	 GetReset3DGroupUnitCount( const size_t index );
		virtual void GetReset3DUnitNameByIndex( const size_t groupindex, const size_t unitindex, string&name );

		virtual void GetResSetName(string& name);
		virtual void SetResSetResPath(const string& path);

		virtual void ActiveBrush();

	public:
		//重载
		virtual void SetContext(CEditContext* pContext);
	};

}
