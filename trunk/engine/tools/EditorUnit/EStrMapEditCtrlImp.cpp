#include "stdafx.h"
#include "EStrMapEditCtrlImp.h" 
#include "PathManager/CPathManager.h"
#include "CCombinCmd.h"
namespace sqr_tools
{
	class CStrInsCmd : public CPropBaseCmd<EStrMapEditCtrlImp>
	{
	public:
		CStrInsCmd( EStrMapEditCtrlImp* pCtrl, float pos, const char* str )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_strData = str;
		}

		void _Execute(void)
		{
			GetCtrl()->_InsertNewNode(m_fPos,m_strData.c_str());
		}

		void _UnExecute(void)
		{
			GetCtrl()->_DelEStrNode(m_fPos);
		}

	protected:
		float	m_fPos;
		string	m_strData;
	};

	class CStrDelCmd : public CPropBaseCmd<EStrMapEditCtrlImp>
	{
	public:
		CStrDelCmd( EStrMapEditCtrlImp* pCtrl, float pos )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_strOld = GetCtrl()->GetEStrNode(m_fPos);
		}

		void _Execute(void)
		{
			GetCtrl()->_DelEStrNode(m_fPos);
		}

		void _UnExecute(void)
		{
			GetCtrl()->_InsertNewNode(m_fPos,m_strOld.c_str());
		}
	protected:
		float	m_fPos;
		string	m_strOld;
	};

	class CStrSetCmd : public CPropBaseCmd<EStrMapEditCtrlImp>
	{
	public:
		CStrSetCmd( EStrMapEditCtrlImp* pCtrl, float pos, const char* str )
			: CPropBaseCmd(pCtrl)
		{
			m_fPos = pos;
			m_strOld = GetCtrl()->GetEStrNode(m_fPos);
			m_strData = str;
		}

		void _Execute(void)
		{
			GetCtrl()->_SetNodeEStr(m_fPos,m_strData.c_str());
		}

		void _UnExecute(void)
		{
			GetCtrl()->_SetNodeEStr(m_fPos,m_strOld.c_str());
		}
	protected:
		float	m_fPos;
		string	m_strOld;
		string	m_strData;
	};
	DEF_UI_IMP(EStrMapEditCtrlImp);

	EStrMapEditCtrlImp::EStrMapEditCtrlImp(void* param)
		: m_fCurPercent(0.0f)
	{

	}

	EStrMapEditCtrlImp::~EStrMapEditCtrlImp()
	{

	}

	int	EStrMapEditCtrlImp::GetEStrMapCount()
	{
		if (m_pCtrlData)
			return ((CEStrMapData*)(m_pCtrlData->m_pData))->GetEStrMapCount();
		return 0;
	}
	float	EStrMapEditCtrlImp::GetEStrMapPer(int n)
	{
		if (m_pCtrlData)
			return ((CEStrMapData*)(m_pCtrlData->m_pData))->GetEStrMapIndex(n);
		return 0.0f;
	}

	void	EStrMapEditCtrlImp::SetCurPercent(float per)
	{
		m_fCurPercent = per;
	}
	float	EStrMapEditCtrlImp::GetCurPercent()
	{
		return m_fCurPercent;
	}

	void	EStrMapEditCtrlImp::DelCurPerNode()
	{
		DelEStrNode(m_fCurPercent);
	}

	void EStrMapEditCtrlImp::InsertRoleTexNode(float idx, const char* tex)
	{
		string workDir;
		string strCPlus = tex;
		if( CPathSetImp::GetInst()->GetResFileName(workDir,strCPlus) )
			InsertNewNode(idx, strCPlus.c_str());	
	}

	void EStrMapEditCtrlImp::OpenCurTexture()
	{
		if (m_pCtrlData)
		{
			const char* tex = ((CEStrMapData*)(m_pCtrlData->m_pData))->GetEStrNode(m_fCurPercent);
			if (tex)
			{
				string strTex = tex;
				string d_str;
				if (CPathSetImp::GetInst()->GetFullFileName(strTex,d_str))
				{
					// open 
					ShellExecute(NULL, "open", d_str.c_str(), "", "", SW_SHOW);
				}
			}
		}
	}

	const char*	EStrMapEditCtrlImp::GetEStrNode(float idx)
	{
		if (m_pCtrlData)
			return ((CEStrMapData*)(m_pCtrlData->m_pData))->GetEStrNode(idx);
		return NULL;
	}

	void	EStrMapEditCtrlImp::MoveCurNodeToPer(float per)
	{
		if (m_pCtrlData)
		{
			const char* str = ((CEStrMapData*)(m_pCtrlData->m_pData))->GetEStrNode(m_fCurPercent);
			if (!str || m_fCurPercent == per)
				return;
			float PreCur = m_fCurPercent;
			m_fCurPercent = per;

			CCombinCmd* moveCmd = new CCombinCmd;
			moveCmd->Push(new CStrInsCmd(this,m_fCurPercent,str));
			moveCmd->Push(new CStrDelCmd(this,PreCur));
			moveCmd->Execute();
		}
	}


	void	EStrMapEditCtrlImp::InsertNewNode(float idx, const char* str)
	{
		if (m_pCtrlData)
		{
			if(GetEStrNode(idx) == NULL)
				(new CStrInsCmd(this,idx,str))->Execute();	
			else
				(new CStrSetCmd(this,idx,str))->Execute();
		}
	}

	void	EStrMapEditCtrlImp::SetNodeEStr(float idx, const char* str)
	{
		if (m_pCtrlData)
			(new CStrSetCmd(this,idx,str))->Execute();	
	}

	void	EStrMapEditCtrlImp::DelEStrNode(float idx)
	{
		if (m_pCtrlData)
			(new CStrDelCmd(this,idx))->Execute();	
	}

	void	EStrMapEditCtrlImp::_InsertNewNode(float idx, const char* str)
	{
		if (m_pCtrlData )
			((CEStrMapData*)(m_pCtrlData->m_pData))->InsertMapNode(idx, str);
		OnDataChanged();
	}

	void	EStrMapEditCtrlImp::_SetNodeEStr(float idx, const char* str)
	{
		if (m_pCtrlData)
			((CEStrMapData*)(m_pCtrlData->m_pData))->SetNodeEStr(idx, str);
		OnDataChanged();
	}

	void	EStrMapEditCtrlImp::_DelEStrNode(float idx)
	{
		if (m_pCtrlData)
			((CEStrMapData*)(m_pCtrlData->m_pData))->EraseMapNode(idx);
		OnDataChanged();
	}

}