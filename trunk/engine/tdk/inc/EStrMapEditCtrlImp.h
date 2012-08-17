#pragma once
#include "CCtrlPropBase.h"

namespace sqr_tools
{
	class CStrInsCmd;
	class CStrDelCmd;
	class CStrSetCmd;
	class EStrMapEditCtrlImp : public CCtrlPropBase
	{
		REG_UI_IMP(EStrMapEditCtrlImp);
		friend class CStrInsCmd;
		friend class CStrDelCmd;
		friend class CStrSetCmd;
	public:
		EStrMapEditCtrlImp(void* param);
		~EStrMapEditCtrlImp();

		virtual int				GetEStrMapCount();
		virtual float			GetEStrMapPer(int n);

		virtual void			SetCurPercent(float per);
		virtual float			GetCurPercent();
		virtual void			DelCurPerNode();
		virtual const char*		GetEStrNode(float idx);
		virtual void			MoveCurNodeToPer(float per);
		virtual void			InsertRoleTexNode(float idx, const char* str);
		virtual void			OpenCurTexture();

		virtual void			DelEStrNode(float idx);
		virtual void			InsertNewNode(float idx, const char* str);
		virtual void			SetNodeEStr(float idx, const char* str);
	private:
		virtual void			_DelEStrNode(float idx);
		virtual void			_InsertNewNode(float idx, const char* str);
		virtual void			_SetNodeEStr(float idx, const char* str);
		float		m_fCurPercent;
	};
}