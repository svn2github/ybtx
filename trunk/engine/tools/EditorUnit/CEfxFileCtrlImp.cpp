#include "stdafx.h"
#include "CEfxFileCtrlImp.h"
#include "CEditContext.h"
#include "CEditEffectProp.h"
#include "CEditEffectManager.h"
#include "CEditModel.h"
#include "PathManager/CPathManager.h"
#include "winuser.h"
#include "CAutoSearchFile.h"

namespace sqr_tools
{
	DEF_UI_IMP(CEfxFileCtrlImp);
	CEfxFileCtrlImp::CEfxFileCtrlImp(void* param)
		: m_pEffectManager(NULL)
		, m_RefreshEmptyMode(NULL)
		, m_RenderEmptyMode(NULL)
		, m_RenderMode(NULL)
		, m_pEditEfx(NULL)
	{	
		m_CurEffectName = "无标题.efx";
	}

	CEfxFileCtrlImp::~CEfxFileCtrlImp(void)
	{
		if (m_RenderEmptyMode)
			SafeRelease(m_RenderEmptyMode);
		if (m_pEditEfx)
			SafeRelease(m_pEditEfx);
	}

	DWORD CEfxFileCtrlImp::Filter(void)
	{
		return CRenderObject::st_RenderObjectType;
	}

	void CEfxFileCtrlImp::ActiveNode( CRenderNode* pNode )
	{
		if (!CEditEffectManager::HasInst())
			return;
		if (pNode == m_RefreshEmptyMode || m_RenderMode == pNode)
			return;
		m_RenderMode = static_cast<CRenderObject*>(pNode);
		ResetEditEffect();
	}

	void CEfxFileCtrlImp::_InitEditEffectManger()
	{
		m_pEffectManager = static_cast<CEditEffectManager*>(CEditEffectManager::GetInst());
		return;
	}

	void CEfxFileCtrlImp::_UpdateCLR()
	{
		//更新 tree
		CCtrlBase::Update();
	}

	bool CEfxFileCtrlImp::LoadEffectFile(const char* path)
	{
		if (!m_pEffectManager)
			_InitEditEffectManger();
		
		if (m_pEffectManager)
		{
			CPkgFile::EnableAsyncRead( false );
			// 完成加载
			string fileName = path;
			if (m_OpenedFiles.find(fileName) != m_OpenedFiles.end())
				return false;

			if (SUCCEEDED(m_pEffectManager->LoadEffectGroups(fileName)))
			{
				m_OpenedFiles.insert(fileName);
				_UpdateCLR();
				m_CurEffectName = fileName;
				return true;
			}
		}
		return false;
	}

	bool CEfxFileCtrlImp::LoadEffectForder(const char* path)
	{
		if (!m_pEffectManager)
			_InitEditEffectManger();
		if (!m_pEffectManager)
			return false;

		string tempPath = path;
		vector<std::string> effectnames;
		effectnames = CAutoSearchFile::GetInstance()->GetAllFiles(tempPath, ".efx");

		std::string effectname;
		size_t size = effectnames.size();
		for ( size_t i = 0; i < size; ++i )
		{
			effectname = effectnames[i];
			if (FAILED(m_pEffectManager->LoadEffectGroups( effectname )))
			{
				_UpdateCLR();
				string log = effectname + " open failed!";
				MessageBox(0,log.c_str(),NULL,0);
				return false;
			}
			m_OpenedFiles.insert(effectname);
		}
		_UpdateCLR();
		m_CurEffectName = "无标题.efx";
		return true;
	}

	void CEfxFileCtrlImp::ClearLoadEffect()
	{
		m_OpenedFiles.clear();
		if (m_pEffectManager)
		{
			// 清空当前信息
			SelectTreeNode(false, NULL, NULL);
			ResetEditEffect();

			m_pEffectManager->ClearAll();
			SafeRelease(m_RenderEmptyMode);
		}
		_UpdateCLR();
		m_CurEffectName = "无标题.efx";
	}

	void CEfxFileCtrlImp::SelectTreeNode(bool root, const char* forder, const char* effect)
	{
		if (root)
			m_CurSelNode.m_NodeType = ENode_Head;
		else
		{
			if (forder != NULL && effect != NULL)
			{
				// effect
				m_CurSelNode.m_NodeType = ENode_Effect;
				m_CurSelNode.m_ForderName = forder;
				m_CurSelNode.m_EffectName = effect;
			}
			else if (forder != NULL)
			{
				// forder
				m_CurSelNode.m_NodeType = ENode_Forder;
				m_CurSelNode.m_ForderName = forder;
			}
			else
				m_CurSelNode.m_NodeType = ENode_TypeCount;
		}
	}

	bool CEfxFileCtrlImp::IsSelectTreeNode( const char* forder, const char* effect)
	{
		if (forder == NULL)
		{
			if (m_CurSelNode.m_NodeType == ENode_Head)
				return true;
		}
		else if (forder != NULL && effect == NULL)
		{
			if (m_CurSelNode.m_NodeType == ENode_Forder)
			{
				string tempForder = forder;
				if (tempForder == m_CurSelNode.m_ForderName)
					return true;
			}
		}
		else if (forder != NULL && effect != NULL)
		{
			if (m_CurSelNode.m_NodeType == ENode_Effect)
			{
				string tempForder = forder;
				string tempEffect = effect;
				if (tempForder == m_CurSelNode.m_ForderName && tempEffect == m_CurSelNode.m_EffectName)
					return true;
			}
		}
		return false;
	}


	void CEfxFileCtrlImp::ResetEditEffect()
	{
		if (!m_pOwnContext)
			return;
		if (!m_pEffectManager)
			_InitEditEffectManger();
		if (!m_RefreshEmptyMode)
			m_RefreshEmptyMode = m_pOwnContext->CreateEditModel(NULL);

		m_pOwnContext->SetActiveObj(m_RefreshEmptyMode);
		_ClearModeEfx();
		if (m_CurSelNode.m_NodeType == ENode_Effect)
		{
			// 设置当前
			CEditEffectProp* pEffectProp = m_pEffectManager->GetEditEffectProp( m_CurSelNode.m_ForderName.c_str(), m_CurSelNode.m_EffectName.c_str() );
			if (pEffectProp)
			{
				CRenderObject* pMode = m_RenderMode;
				if (!pMode)
				{
					if (!m_RenderEmptyMode)
					{
						m_RenderEmptyMode = m_pOwnContext->CreateEditModel(NULL);
						m_RenderMode = m_RenderEmptyMode;
					}
					pMode = m_RenderEmptyMode;
				}

				SafeRelease(m_pEditEfx);
				m_pEditEfx = pEffectProp->CreateNewEffect();
				pMode->AddEffectPtr(m_pEditEfx, CEffectPlayer::PLAY_LOOP);
				m_pOwnContext->SetActiveObj(pMode);
			}
		}
	}

	void CEfxFileCtrlImp::_ClearModeEfx()
	{
		if (m_RenderEmptyMode)
			m_RenderEmptyMode->ClearAllEfx();	
		if (m_RenderMode)
			m_RenderMode->ClearAllEfx();
	}

	void CEfxFileCtrlImp::GetEffectNames(map<string, vector<string>>& names)
	{
		if (m_pEffectManager)
			m_pEffectManager->GetEffectName( names );
	}

	uint32 CEfxFileCtrlImp::OnCreateNew(const char* newname)
	{
		if (!m_pEffectManager)
			_InitEditEffectManger();

		if (m_CurSelNode.m_NodeType == ENode_Head)
		{
			// add forder
			if(m_pEffectManager->CreateEffectGroup(newname))
			{
				SelectTreeNode(false, newname, NULL);
				ResetEditEffect();
				return 1;
			}
		}
		else if (m_CurSelNode.m_NodeType == ENode_Forder)
		{
			// add effect
			CEditEffectProp* pEditEffectProp = m_pEffectManager->CreateEffect( m_CurSelNode.m_ForderName, newname );
			if( pEditEffectProp )
			{
				string tempforder = m_CurSelNode.m_ForderName;
				SelectTreeNode(false, tempforder.c_str(), newname);
				ResetEditEffect();
				return 2;
			}
		}
		return 0;
	}

	bool CEfxFileCtrlImp::IsSaveAs()
	{
		if( m_CurEffectName.empty() || m_CurEffectName == "无标题.efx" )
			return true;
		return false;
	}

	void CEfxFileCtrlImp::OnEffectSave()
	{
		if (!m_pEffectManager)
			return;

		if( FAILED(m_pEffectManager->SaveEffectGroups(m_CurEffectName)) )
		{
			string log = m_CurEffectName + " save failed!";
			MessageBox(0,log.c_str(),NULL,0);
			return;
		}
		std::string log = m_CurEffectName + " save succeed!";;
		MessageBox(0,log.c_str(),NULL,0);
	}

	void CEfxFileCtrlImp::OnEffectSaveAs(const char* file)
	{
		if (!m_pEffectManager)
			return;
		string effectname = file;
		size_t npos = effectname.rfind(".");
		if(npos == -1)
			effectname += ".efx";
		if( !effectname.empty() )
		{
			m_CurEffectName = effectname.c_str();
			if( FAILED(m_pEffectManager->SaveEffectGroups(effectname)))
			{
				string log = m_CurEffectName + " save failed!";
				MessageBox(0,log.c_str(),NULL,0);
				return;
			}
			std::string log = m_CurEffectName + " save succeed!";;
			MessageBox(0,log.c_str(),NULL,0);
		}
	}

	void CEfxFileCtrlImp::DeleteCurSelectedNode()
	{
		if (m_CurSelNode.m_NodeType == ENode_Head)
			return;
		else if (m_CurSelNode.m_NodeType == ENode_Forder)
		{
			string eforder = m_CurSelNode.m_ForderName;
			if (m_pEditEfx)
			{
				CEditEffectProp* curEditProp = m_pEditEfx->GetEditEffectProp();
				map<string, vector<string>> efxGroups;
				GetEffectNames(efxGroups);

				if (efxGroups.find(eforder) != efxGroups.end())
				{
					vector<string> efxNames = efxGroups[eforder];
					for (vector<string>::iterator itr = efxNames.begin(); itr != efxNames.end(); ++itr)
					{
						string efxName = *itr;
						CEditEffectProp* pEffectProp = m_pEffectManager->GetEditEffectProp( eforder.c_str(), efxName.c_str() );

						if (curEditProp == pEffectProp)
						{
							SelectTreeNode(true, NULL, NULL);
							ResetEditEffect();
						}
					}
				}
				if( m_pEffectManager->DeleteGroup( eforder ) )
					_UpdateCLR();
			}
			
		}
		else if (m_CurSelNode.m_NodeType == ENode_Effect)
		{
			CEditEffectProp* curEditProp = m_pEditEfx->GetEditEffectProp();
			CEditEffectProp* pEffectProp = m_pEffectManager->GetEditEffectProp( m_CurSelNode.m_ForderName.c_str(), m_CurSelNode.m_EffectName.c_str() );
			if (curEditProp == pEffectProp)
			{
				SelectTreeNode(true, NULL, NULL);
				ResetEditEffect();
			}

			if( m_pEffectManager->DeleteEffect( m_CurSelNode.m_ForderName, m_CurSelNode.m_EffectName ) )
				_UpdateCLR();
		}
	}

}