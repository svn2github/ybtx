#pragma once
#include "CCtrlBaseObserver.h"

namespace sqr
{
	class CRenderNode;
}

namespace sqr_tools {

	class CEditResSet;
	class CEditResSetUnit;
	class CEdit2DResSetUnit;
	struct CEditResSetGroup;
	struct CEditModelResSetGroup;
	struct CEditResSetEditModelUnit;

	class CMapEffectCtrlImp;
	#define	MGT_UPDATE_EFFECT	0x01

	class CEditSet;
	class CTileViewCtrlImp :public CCtrlBaseObserver
	{
		REG_UI_IMP(CTileViewCtrlImp);

	private:
		typedef map<string, CEditResSet* > ResSetPool;
		ResSetPool			m_resSets;

		CEditResSetGroup		*m_pActiveGroup;
		CEditModelResSetGroup	*m_pActiveModelGroup;
		CEditResSetEditModelUnit*m_pActiveEditModelUnit;
		CEdit2DResSetUnit		*m_pActive2DUnit;

		typedef map<string, int> terrainMatCuePool;
		terrainMatCuePool		m_mapMaterialIndexs;

		CEditObserver*			m_pEditResSetObserver;	
		CEditResSet				*m_pActiveResSet;
		std::string				m_strResSetPath;

		CMapEffectCtrlImp*		m_pMapEffectCtrlImp;
		CEditContext*			m_pImpContext;
	private:
		virtual void Init();
		virtual void SaveReset(const string& resetName);
		virtual void UpdateSetViewCtrl();
	public:
		CTileViewCtrlImp(void* param);
		virtual ~CTileViewCtrlImp(void);

		virtual void SetSetViewCtrlObserver(CEditObserver* pObserver);

		virtual void LoadReset(const string& resetName);
		virtual void LoadOldReset(const string& resetName);
		virtual bool ResetIsExit(const string& resetName);
		virtual void LoadTerrainMaterialConfig(vector<string>& names);

		///图素包
		virtual bool CreateTileReset(const string& resetName);
		virtual void SelectCurReset(const string& resetName);
		virtual bool DeleteCurReset(const string& resetName);
		virtual void ImportReset(const vector<string> resetNames);
		virtual void SaveCurReset(const string& resetName);
		virtual void PatchSaveCurReset(const vector<string> resetNames);

		///图素组
		virtual void CreateResSetGroup(const string& strGroupName, const bool bModel, const BYTE bResGroupMark, const bool bVisibleByConfigue);
		virtual bool DeleteReSetGroup(const string& strGroupName, const bool b3D);

		///图素个体
		virtual void SelectActiveUnit(const string& resetName, const string& strGroupName, const string& strUnitName, const bool b3D, const bool bSelectUnit);
		virtual bool ModelUnitIsExitInGroup(const string& strModelUnitName);
		virtual bool TexUnitIsExitInGroup(const string& strTexUnitName);
		virtual bool CreateModelUnit(const string& strGroupName, const string& strModelUnitName, string& strItemName);
		virtual bool CreateModelUnits(const string& strGroupName, const vector<string>& vecModelUnitNames, string& strItemName);
		virtual bool CreateTextureUnit(const string& strGroupName, const string& strTexUnitName, string& strItemName);
		virtual bool DeleteGroupUnit(const string& strGroupName, const string& strUnitName, const bool b3D);
		virtual void BindUnitAnimationGroup(const string& strUnitName, const string& strAgpFile);
		virtual void BindGroupAnimationGroup(const string& strGroupName, const string& strAgpFile);
		virtual int  GetUnitMaterialIndex(const string& unitName);

		virtual void SetUnitMaterialCueIndex(const string& unitName, string& strMaterialIndex);
		virtual void SetGroupMaterialCueIndex(const string& strGroupName, string& strMaterialIndex);
		virtual int  GetMaterialIndex( string& name );

		virtual void BindUnitEffectName(const string& strUnitName, const string& strEffectName);

		virtual void MakePathToText(const string& Src, string& Dst);

		/// get value data
		virtual int	 GetReset3DGroupCount();
		virtual void GetReset3DGroupNameByIndex(const size_t index, string&name);
		virtual int	 GetReset3DGroupUnitCount(const size_t index);
		virtual void GetReset3DUnitNameByIndex(const size_t groupindex, const size_t unitindex, string&name);

		virtual int	 GetReset2DGroupCount();
		virtual void GetReset2DGroupNameByIndex(const size_t index, string&name);
		virtual int	 GetReset2DGroupUnitCount(const size_t index);
		virtual void GetReset2DUnitNameByIndex(const size_t groupindex, const size_t unitindex, string&name);

		virtual void GetResetResPath(string& path);

		virtual void PathResetFile();

		virtual void SetMapEffectCtrlImp(CMapEffectCtrlImp* pImp);
		virtual bool _ProcMsg(const CMsg& msg );
		virtual void Update();

		virtual void ActiveNode( CRenderNode* pNode );
	public:
		//重载
		virtual void SetContext(CEditContext* pContext);
		virtual void SetImpContext(CEditContext* pContext);
	private:
		virtual void SelectActive3DGroupUnit(const string& strGroupName, const string& strUnitName, const bool bSelectUnit);
		virtual void SelectActive2DGroupUnit(const string& strGroupName, const string& strUnitName, const bool bSelectUnit);
	};

}
