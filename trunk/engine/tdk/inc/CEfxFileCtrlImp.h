#pragma once
#include "CCtrlBaseObserver.h"

class CEditEffectManager;
class CEditEffect;
namespace sqr
{
	class CRenderObject;
}

namespace sqr_tools
{
	// 完成特效文件的打开 创建 保存功能
	class CEfxFileCtrlImp 
		: public CCtrlBaseObserver
	{
		enum NodeType
		{
			ENode_Head			= 0,
			ENode_Forder		= 1,
			ENode_Effect		= 2,
			ENode_TypeCount		= 3,
		};

		struct SelectNode
		{
			NodeType	m_NodeType;
			string		m_ForderName;
			string		m_EffectName;
			SelectNode()
				: m_NodeType(ENode_TypeCount)
			{}
		};

		REG_UI_IMP(CEfxFileCtrlImp);
	public:
		CEfxFileCtrlImp(void* param);
		~CEfxFileCtrlImp(void);

	public:
		virtual bool	LoadEffectFile(const char* path);
		virtual void	ClearLoadEffect();
		virtual void	SelectTreeNode(bool root, const char* forder, const char* effect);
		virtual bool	IsSelectTreeNode( const char* forder, const char* effect);
		virtual void	ResetEditEffect();
		virtual void	GetEffectNames(map<string, vector<string>>& names);
		
		virtual uint32	OnCreateNew(const char* newname);
		virtual bool	IsSaveAs();
		virtual void	OnEffectSave();
		virtual void	OnEffectSaveAs(const char* file);
		virtual bool	LoadEffectForder(const char* path);
		virtual void	DeleteCurSelectedNode();

		// over ride
		virtual DWORD	Filter(void);
		virtual void	ActiveNode( CRenderNode* pNode );
	private:
		virtual void _InitEditEffectManger();
		virtual void _UpdateCLR();
		virtual void _ClearModeEfx();

	private:
		CEditEffectManager*		m_pEffectManager;
		set<string>				m_OpenedFiles;
		SelectNode				m_CurSelNode;
		CRenderObject*			m_RefreshEmptyMode;
		CRenderObject*			m_RenderEmptyMode;
		CRenderObject*			m_RenderMode;
		CEditEffect*			m_pEditEfx;
		string					m_CurEffectName;
	};

}